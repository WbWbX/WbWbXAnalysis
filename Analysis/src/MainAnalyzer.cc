
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "../interface/MainAnalyzer.h"

#include "TtZAnalysis/DataFormats/interface/elecRhoIsoAdder.h"
#include "TtZAnalysis/plugins/leptonSelector2.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "TTree.h"
#include "TFile.h"
#include <fstream>
//#include <omp.h>
#include "TtZAnalysis/Analysis/interface/AnalysisUtils.h"

#include "TtZAnalysis/Tools/interface/fileReader.h"

#include "eventLoop.h"


namespace ztop{
  typedef std::vector<ztop::NTElectron>::iterator NTElectronIt;
  typedef std::vector<ztop::NTMuon>::iterator NTMuonIt;
  typedef std::vector<ztop::NTJet>::iterator NTJetIt;
  typedef std::vector<ztop::NTTrack>::iterator NTTrackIt;
  typedef std::vector<ztop::NTSuClu>::iterator NTSuCluIt;
}


MainAnalyzer::MainAnalyzer(){

	AutoLibraryLoader::enable();
	freplaced_=0;
	dataname_="data";
	writeAllowed_=true;
	b_ee_=false;
	b_mumu_=false;
	b_emu_=false;
	is7TeV_=false;
	showstatus_=false;
	onlySummary_=false;
	filecount_=0;
	testmode_=false;
	//showstatusbar_=false;
	lumi_=0;

}


int MainAnalyzer::checkForWriteRequest(){
	for(size_t i=0;i<p_askwrite.size();i++){
		if(p_askwrite.get(i)->preadready()){
			p_askwrite.get(i)->pread(); //to free pipe again
			return i; //return first ready to write!
		}
	}
	return -1;
}




int MainAnalyzer::start(){

	using namespace std;
	using namespace ztop;


	/// put MC code here, make containerlist private!!

	clear();

	readFileList();

	//TString name=channel_+"_"+energy_+"_"+syst_;

	if(channel_=="" || energy_=="" || syst_ == ""){
		std::cout << "MainAnalyzer::start Analyzer not properly named - check!" << std::endl;
		return -1;
	}

	analysisplots_.setName(getOutFileName());
	analysisplots_.setSyst(getSyst());
	int btagsyst=getBTagSF()->getSystematic();
	//load btag:
	if(!(getBTagSF()->makesEff())){
		std::cout << "loading b-tag File: " << btagsffile_ << std::endl;
		std::ifstream testfile(btagsffile_.Data());
		if(!testfile){
			std::cout << "b-tag File " << btagsffile_ << " not found, exit (-3)" << std::endl;
			return -3;
		}
		getBTagSF()->readFromTFile(btagsffile_);
		getBTagSF()->setSystematic(btagsyst);
	}

	///communication stuff...

	size_t filenumber=infiles_.size();
	std::cout << "Running on files: " <<std::endl;
	for(size_t i=0;i<infiles_.size();i++)
		std::cout << infiles_.at(i) <<std::endl;

	//create pipes
	p_idx.open(filenumber);
	p_finished.open(filenumber);
	p_allowwrite.open(filenumber);
	p_askwrite.open(filenumber);
	p_status.open(filenumber);

	daughPIDs_.resize(filenumber,0);

	//spawn child processes
	for(size_t i=0;i<filenumber;i++){
		daughPIDs_.at(i)=fork();
		if(daughPIDs_.at(i)==0){ //child
			try{
				analyze(p_idx.get(i)->pread());
			}
			catch(...){
				p_finished.get(i)->pwrite(-99);
				std::exit(EXIT_FAILURE);
			}
			std::exit(EXIT_SUCCESS);
		}
		else{ //send start signal for ith daughter process
			p_idx.get(i)->pwrite(i);
			usleep(100000);
		}
	}

	//daughters never reach here
	//wait until all childs are initialised and screen output is done
	if(!testmode_)
		sleep(10);

	std::cout << "running on " << filenumber << " files" << std::endl;
	std::vector<int> succ;
	succ.resize(filenumber,0);
	std::vector<int> status;
	status.resize(filenumber,0);

	while(!NoneEqual(succ,0)){ //wait for daughters until all are done or failed
		bool newwrite=false;
		for(size_t i=0;i<filenumber;i++){
			if(p_finished.get(i)->preadready())
				succ.at(i)=p_finished.get(i)->pread(); //testing
			while(p_status.get(i)->preadready())
				status.at(i)=p_status.get(i)->pread();// asks for status, read full buffer and store last value
		}

		int it_readytowrite=checkForWriteRequest();
		if(it_readytowrite >= 0){ // daugh ready to write
			p_allowwrite.get(it_readytowrite)->pwrite(1);
			succ.at(it_readytowrite)=p_finished.get(it_readytowrite)->pread();   //wait for successful/ns write
			newwrite=true;
		} //else do nothing - none ready to write

		else{
			sleep(5); //if nothing happened only check every 5 sec
		}
		if(showstatus_ || newwrite){
			int done=0,sdone=0;
			std::cout << "\n\n" << std::endl;
			for(size_t i=0;i<filenumber;i++){
				if(succ.at(i) == 0)
					std::cout <<  "running "<< status.at(i) << "%:"<< "\t" <<infiles_.at(i)  << std::endl;
				if(succ.at(i) !=0)
					done++;
				if(succ.at(i) >0)
					sdone++;
			}
			std::cout << "-----------------" << std::endl;
			for(size_t i=0;i<filenumber;i++){
				if(succ.at(i) <0)
					std::cout  << "failed(" << succ.at(i)<<"):   \t" << infiles_.at(i) << std::endl;
			}

			for(size_t i=0;i<filenumber;i++){
				if(succ.at(i) >0)
					std::cout  << " done:      \t" << infiles_.at(i) << std::endl;
			}

			std::cout << "\n\n" << sdone << "(" << done-sdone << " failed) / " << filenumber << " done\n\n"<< std::endl;
		}

	}
	sleep(1);
	bool nonefailed=true;
	for(size_t i=0;i<succ.size();i++){
		std::cout << succ.at(i) << "\t" << infiles_.at(i) << std::endl;
		if(succ.at(i) < 0)
			nonefailed=false;
	}
	if(nonefailed)
		return 1; //only parent
	else
		return -1;

}

TString MainAnalyzer::replaceExtension(TString filename){

	for(size_t i=0;i<ftorepl_.size();i++){
		if(filename.Contains(ftorepl_.at(i))){
			freplaced_++;
			return filename.ReplaceAll(ftorepl_.at(i),fwithfix_.at(i));
		}
	}
	return filename;
}

void MainAnalyzer::readFileList(){
	using namespace ztop;
	using namespace std;

	fileReader fr;
	fr.setDelimiter(",");
	fr.setComment("$");
	fr.setStartMarker("[inputfiles-begin]");
	fr.setEndMarker("[inputfiles-end]");
	fr.readFile(filelist_.Data());

	infiles_.clear();
	legentries_.clear();
	colz_.clear();
	norms_.clear();
	legord_.clear();
	issignal_.clear();

	for(size_t line=0;line<fr.nLines();line++){
		if(fr.nEntries(line) < 5){
			std::cout << "MainAnalyzer::readFileList: line " << line << " of inputfile is broken ("<<fr.nEntries(line)<< " entries.)" <<std::endl;
			sleep(2);
			continue;
		}
		infiles_.push_back   (replaceExtension(fr.getData<TString>(line,0)));
		legentries_.push_back(fr.getData<TString>(line,1));
		colz_.push_back      (fr.getData<int>    (line,2));
		norms_.push_back     (fr.getData<double> (line,3));
		legord_.push_back    (fr.getData<size_t> (line,4));
		if(fr.nEntries(line) > 5)
			issignal_.push_back(fr.getData<bool> (line,5));
		else
			issignal_.push_back(false);
	}

/*

	ifstream inputfiles (filelist_.Data());
	string filename;
	string legentry;
	int color;
	double norm;
	string oldline="";
	size_t legord;
	if(inputfiles.is_open()){
		while(inputfiles.good()){
			inputfiles >> filename;
			if(filename.size()<1)
				continue;
			if(((TString)filename).Contains("#")){
				getline(inputfiles,filename); //just ignore complete line
				//	std::cout << "ignoring: " << filename << std::endl;
				continue;
			}
			inputfiles >> legentry >> color >> norm >> legord;
			if(oldline != filename){

				std::cout << "adding: " << replaceExtension(filename) << "\t" << legentry << "\t" << color << "\t" << norm << "\t" << legord << std::endl;

				TString infile=replaceExtension(filename);

				infiles_    << infile;
				legentries_ << legentry;
				colz_       << color;
				norms_      << norm;
				legord_ << legord;
				oldline=filename;
			}

		}
		if((uint)freplaced_ != fwithfix_.size()){
			cout << "replacing at least some postfixes was not sucessful! expected to replace "<< fwithfix_.size() << " replaced "<< freplaced_ << " exit" << endl;

		}
	}
	else{
		cout << "MainAnalyzer::setFileList(): input file list not found" << endl;
	}
	*/

}

void MainAnalyzer::copyAll(const MainAnalyzer & analyzer){

	channel_=analyzer.channel_;
	b_ee_=analyzer.b_ee_;
	b_emu_=analyzer.b_emu_;
	b_mumu_=analyzer.b_mumu_;
	syst_=analyzer.syst_;
	energy_=analyzer.energy_;
	dataname_=analyzer.dataname_;

	lumi_=analyzer.lumi_;
	datasetdirectory_=analyzer.datasetdirectory_;
	puweighter_= analyzer.puweighter_;
	filelist_=analyzer.filelist_;
	jeradjuster_= analyzer.jeradjuster_;
	jecuncertainties_= analyzer.jecuncertainties_;
	btagsf_ = analyzer.btagsf_;
	analysisplots_ = analyzer.analysisplots_;
	showstatus_=analyzer.showstatus_;

	infiles_=analyzer.infiles_;
	legentries_=analyzer.legentries_;
	colz_=analyzer.colz_;
	norms_=analyzer.norms_;
	issignal_=analyzer.issignal_;
	outfileadd_=analyzer.outfileadd_;

	//pipes are NOT in here. they need to be created during running?

	writeAllowed_=analyzer.writeAllowed_;
}

MainAnalyzer::MainAnalyzer(const MainAnalyzer & analyzer){
	copyAll(analyzer);
}

MainAnalyzer & MainAnalyzer::operator = (const MainAnalyzer & analyzer){
	copyAll(analyzer);
	return *this;
}

void MainAnalyzer::analyze(size_t i){

	//  std::cout << " analyze " << i<< std::endl;
	analyze(infiles_.at(i),legentries_.at(i),colz_.at(i),norms_.at(i),legord_.at(i),i);

}



