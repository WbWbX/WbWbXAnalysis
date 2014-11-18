
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "../interface/MainAnalyzer.h"

#include "TtZAnalysis/DataFormats/interface/elecRhoIsoAdder.h"

#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "TTree.h"
#include "TFile.h"
#include <fstream>
//#include <omp.h>
#include "TtZAnalysis/Analysis/interface/AnalysisUtils.h"

#include "TtZAnalysis/Tools/interface/fileReader.h"

#include "eventLoop.h"
#include <algorithm>
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"

#include <sys/stat.h>
#include "../interface/tBranchHandler.h"



void MainAnalyzer::reportError(int errorno, size_t anaid){
	if(errorno >0) errorno=-errorno;
	p_askwrite.get(anaid)->pwrite(anaid);
	p_allowwrite.get(anaid)->pread();
	p_finished.get(anaid)->pwrite(errorno);
}

void MainAnalyzer::reportStatus(Long64_t entry,Long64_t nEntries,size_t anaid){
	if((entry +1)* 100 % nEntries <100){
		int status=(entry+1) * 100 / nEntries;
		p_status.get(anaid)->pwrite(status);
	}
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
	tickoncemode_=false;
	//showstatusbar_=false;
	lumi_=0;
	mode_="";
	singlefile_=false;
	maxchilds_=8;
	topmass_="172.5";
	usepdfw_=-1;
	usediscr_=false;
	eventbranch_="NTEvent";

	///set defaults for SF
	pdfweighter_.switchOff(true); //switch off as default
	fakedata_startentries_=0.9;

}
/**
 * takes care of not already deleted containers etc
 */
MainAnalyzer::~MainAnalyzer(){
	ztop::container1D::c_deletelist();
	ztop::container1DUnfold::c_deletelist();
	ztop::container2D::c_deletelist();
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



/**
 * handles all the parallalization and global init things
 */
int MainAnalyzer::start(){

	using namespace std;
	using namespace ztop;


	///////////////////////////////////////////////////////
	////////////////// GLOBAL FILE READ ETC..//////////////
	///////////////////////////////////////////////////////

	clear();

	//add changes due to top mass value


	readFileList();




	//TString name=channel_+"_"+energy_+"_"+syst_;

	if(channel_=="" || energy_=="" || syst_ == ""){
		std::cout << "MainAnalyzer::start Analyzer not properly named - check!" << std::endl;
		return -1;
	}




	allplotsstackvector_.setName(getOutFileName());
	allplotsstackvector_.setSyst(getSyst());

	//further SF configuration

	if(! getPdfReweighter()->switchedOff()){
		usepdfw_ = getPdfReweighter()->getConfIndex();
	}


	bTagBase::systematics btagsyst=getBTagSF()->getSystematic();
	//load btag:
	if(!(getBTagSF()->makesEff()) && getBTagSF()->getMode() != NTBTagSF::shapereweighting_mode){ //no input needed
		std::cout << "loading b-tag File: " << btagsffile_ << std::endl;
		std::ifstream testfile(btagsffile_.Data());
		if(!testfile){
			std::cout << "b-tag File " << btagsffile_ << " not found, exit (-3)" << std::endl;
			return -3;
		}

		//only a workaround
		TString  btagwps=getBTagSF()->getWorkingPointString();
		getBTagSF()->readFromTFile(btagsffile_);
		getBTagSF()->setWorkingPoint(btagwps);
		getBTagSF()->bTagBase::setSystematic(btagsyst);
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
	std::cout << "running on " << filenumber << " files" << std::endl;
	std::vector<int> succ;
	succ.resize(filenumber,0);
	std::vector<int> status;
	status.resize(filenumber,0);

	size_t running=0;

	size_t done=0,failed=0;

	//for graphics output:
	size_t nsamplechars=50;

	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	////////////////// SPAWN CHILD PROCESSES AND KEEP TRACK ////////////////
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////

	if(filenumber>1){
		for(size_t i=0;i<filenumber;i++){
			daughPIDs_.at(i)=fork();
			if(daughPIDs_.at(i)==0){ //child
				try{
					analyze(p_idx.get(i)->pread());
				}
				catch(std::exception& e){
					//  reportError(-99,i);
					//this is a special error report. make sure the couts are atomic
					p_askwrite.get(i)->pwrite(i);
					p_allowwrite.get(i)->pread();
					std::cout << "*************** Exception ***************" << std::endl;
					std::cout << e.what() << std::endl;
					std::cout << "in " << infiles_.at(i) << '\n'<<std::endl;
					p_finished.get(i)->pwrite(-99);

					std::exit(EXIT_FAILURE);
				}
				std::exit(EXIT_SUCCESS);
			}
			else{ //send start signal for ith daughter process
				p_idx.get(i)->pwrite(i);
				running++;

				while(done < filenumber){//!NoneEqual(succ,0)){
					//wait for daughters until all are done or failed
					if(running < maxchilds_ &&
							(filenumber - done >= maxchilds_ || (i<filenumber-1 /* dont skip last */ && filenumber < maxchilds_)))
						break;


					bool newwrite=false;
					for(size_t j=0;j<filenumber;j++){
						//if(p_finished.get(j)->preadready()){
						//	succ.at(j)=p_finished.get(j)->pread(); //testing
						//}
						while(p_status.get(j)->preadready())
							status.at(j)=p_status.get(j)->pread();// asks for status, read full buffer and store last value
					}

					int it_readytowrite=checkForWriteRequest();

					if(it_readytowrite >= 0){ // daugh ready to write
						p_allowwrite.get(it_readytowrite)->pwrite(1);
						succ.at(it_readytowrite)=p_finished.get(it_readytowrite)->pread();   //wait for successful/ns write
						newwrite=true;
						if(succ.at(it_readytowrite) !=0){
							done++;
							running--;
						}
						if(succ.at(it_readytowrite)<0)
							failed++;
					} //else do nothing - none ready to write

					else{
						sleep(5); //if nothing happened only check every 5 sec
					}
					if(showstatus_ || newwrite){
						int sdone=0;
						failed=0;
						std::cout << "\n\n" << std::endl;
						for(size_t j=0;j<filenumber;j++){
							if(succ.at(j) == 0){
								size_t lengthname=infiles_.at(j).Length();
								size_t addblanks=nsamplechars-lengthname;
								std::cout <<  "running "<< status.at(j) << "%:" << "\t" <<infiles_.at(j);
								for(size_t iblnk=0;iblnk<addblanks;iblnk++)
									std::cout << " ";
								std::cout  << legentries_.at(j) << std::endl;

							}
							if(succ.at(j) >0)
								sdone++;
							if(succ.at(j) <0)
								failed++;
						}
						std::cout << "-----------------" << std::endl;
						for(size_t j=0;j<filenumber;j++){
							if(succ.at(j) <0){
								size_t lengthname=infiles_.at(j).Length();
								size_t addblanks=nsamplechars-lengthname;
								std::cout  << "failed(" << succ.at(j)<<"):   \t"<< infiles_.at(j);
								for(size_t iblnk=0;iblnk<addblanks;iblnk++)
									std::cout << " ";
								std::cout << legentries_.at(j)  << std::endl;
							}
						}

						for(size_t j=0;j<filenumber;j++){
							if(succ.at(j) >0){
								size_t lengthname=infiles_.at(j).Length();
								size_t addblanks=nsamplechars-lengthname;
								std::cout  << " done:      \t" << infiles_.at(j);
								for(size_t iblnk=0;iblnk<addblanks;iblnk++)
									std::cout << " ";
								std::cout  << legentries_.at(j)  << std::endl;

							}
						}
						std::cout << "\n\n" << done << "(" << failed << " failed) / " << filenumber << " done\n\n"<< std::endl;
					}
				}

			}
		}

		//daughters never reach here
		//wait until all childs are initialised and screen output is done
		if(!testmode_)
			sleep(1);

		bool nonefailed=true;
		containerStackVector *csv = new containerStackVector();
		try{
			csv->loadFromTFile(getOutPath()+".root");
			for(size_t i=0;i<legentries_.size();i++){
				bool legentryfound=false;
				for(size_t j=0;j<csv->getStack(0).size();j++){
					if(csv->getStack(0).getLegend(j) == legentries_.at(i)){
						legentryfound=true;
						break;
					}
				}
				if(!legentryfound)
					succ.at(i)=-2000;
			}
		}
		catch(...){
			for(size_t i=0;i<succ.size();i++){
				succ.at(i)=-1000;
			}
		}
		delete csv;

		for(size_t i=0;i<succ.size();i++){
			//check if everything went fine
			std::cout << succ.at(i) << "\t" << infiles_.at(i) << std::endl;
			if(succ.at(i) < 0)
				nonefailed=false;
		}
		if(nonefailed)
			return 1; //only parent
		else
			return -1;
	}//
	///part for only one single file (test modes)
	else{ //only one process to spawn
		singlefile_=true;
		try{
			analyze(0);
		}
		catch(...){
			return -99;
		}
		if(testmode_)
			std::cout << "single file done" <<std::endl;
		return 1;
	}
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

	std::cout << "MainAnalyzer::readFileList: reading input file " << std::endl;

	for(size_t line=0;line<fr.nLines();line++){
		if(fr.nEntries(line) < 5){
			std::cout << "MainAnalyzer::readFileList: line " << line << " of inputfile is broken ("<<fr.nEntries(line)<< " entries.)" <<std::endl;
			sleep(2);
			continue;
		}
		infiles_.push_back   ((fr.getData<TString>(line,0)));
		legentries_.push_back(fr.getData<TString>(line,1));
		colz_.push_back      (fr.getData<int>    (line,2));
		norms_.push_back     (fr.getData<double> (line,3));
		legord_.push_back    (fr.getData<size_t> (line,4));
		if(fr.nEntries(line) > 5)
			issignal_.push_back(fr.getData<bool> (line,5));
		else
			issignal_.push_back(false);
	}
	for(size_t i=0;i<infiles_.size();i++){
		//if(legentries_.at(i) == dataname_)
		//	continue;
		infiles_.at(i) =   replaceExtension(infiles_.at(i));
		///load pdf files
		if(issignal_.at(i)  && !getPdfReweighter()->switchedOff()){
			textFormatter ftm;
			std::string filename= ftm.addFilenameSuffix(infiles_.at(i).Data(), "_pdf");
			infiles_.at(i) = filename;

		}
	}


}
/*
void MainAnalyzer::copyAll(const MainAnalyzer & analyzer){


    showstatus_=analyzer.showstatus_;
    onlySummary_=analyzer.
    testmode_=analyzer.
    singlefile_=analyzer.

    channel_=analyzer.channel_;
    b_ee_=analyzer.b_ee_;
    b_emu_=analyzer.b_emu_;
    b_mumu_=analyzer.b_mumu_;
    syst_=analyzer.syst_;
    energy_=analyzer.energy_;
    dataname_=analyzer.dataname_;
    topmass_=analyzer.topmass_;


    discrInput_=analyzer.discrInput_;
    usediscr_=analyzer.discrInput_;

    lumi_=analyzer.lumi_;
    datasetdirectory_=analyzer.datasetdirectory_;
    filelist_=analyzer.filelist_;

    puweighter_= analyzer.puweighter_;
    jeradjuster_= analyzer.jeradjuster_;
    jecuncertainties_= analyzer.jecuncertainties_;
    btagsf_ = analyzer.btagsf_;
    elecsf_=analyzer.elecsf_;
    muonsf_=analyzer.muonsf_;
    triggersf_=analyzer.triggersf_;
    elecenergysf_=analyzer.elecenergysf_;
    muonenergysf_=analyzer.muonenergysf_;
    topptReweighter_=analyzer.topptReweighter_;


    allplotsstackvector_ = analyzer.allplotsstackvector_;

    infiles_=analyzer.infiles_;
    legentries_=analyzer.legentries_;
    colz_=analyzer.colz_;
    norms_=analyzer.norms_;
    issignal_=analyzer.issignal_;
    outfileadd_=analyzer.outfileadd_;

    //pipes are NOT in here. they need to be created during running?

    writeAllowed_=analyzer.writeAllowed_;
}
 */
/*
MainAnalyzer::MainAnalyzer(const MainAnalyzer & analyzer){
    copyAll(analyzer);
}

MainAnalyzer & MainAnalyzer::operator = (const MainAnalyzer & analyzer){
    copyAll(analyzer);
    return *this;
}
 */

/////////use default implementations! as long as there are not pointers or
///other things in the member list, this is sufficient and bugs are less likely

void MainAnalyzer::analyze(size_t i){

	//  std::cout << " analyze " << i<< std::endl;
	analyze(infiles_.at(i),legentries_.at(i),colz_.at(i),legord_.at(i),i);

}


float MainAnalyzer::createNormalizationInfo(TFile *f, bool isMC,size_t anaid){
	float norm=1;
	using namespace ztop;
	using namespace std;

	vector<float> onebin;
	onebin << 0.5 << 1.5;
	bool tmp=container1D::c_makelist;
	container1D::c_makelist=true;
	container1D * generated=  new container1D(onebin, "generated events", "gen", "N_{gen}");
	container1D * generated2=  new container1D(onebin, "generated filtered events", "gen", "N_{gen}");
	container1D::c_makelist=tmp; //switch off automatic listing

	float genentries=0;
	if(isMC){

		TTree * tnorm = (TTree*) f->Get("preCutPUInfo/preCutPUInfo");
		genentries=tnorm->GetEntries();

		norm = lumi_   / genentries;
		for(size_t i=1;i<=generated->getNBins();i++){
			generated->setBinContent(i, (float)genentries);
			generated->setBinStat(i,sqrt(genentries));
		}
		double fgen=0;
		TTree * tfgen = (TTree*) f->Get("postCutPUInfo/PUTreePostCut");
		if(tfgen){
			fgen=tfgen->GetEntries();
			for(size_t i=1;i<=generated2->getNBins();i++){
				generated2->setBinContent(i, (float)fgen);
				generated2->setBinStat(i, sqrt(fgen));
			}
		}
		else{
			for(size_t i=1;i<=generated->getNBins();i++){
				generated2->setBinContent(i, (float)genentries);
				generated2->setBinStat(i, sqrt(genentries));
			}
		}
		//for PDF weights to keep normalization the same

		/*
		if(usepdfw_>-1
				&& ! getPdfReweighter()->switchedOff()
				&& ! getPdfReweighter()->getReweightToNominalEigenvector()){

			std::cout << "warning: using pdf weights is implemented, but there are pitfalls:\n"
					<< " - the nominal pdf weight (idx 0) can be different from 1 depending on\n"
					<< "   which pdf set is used to produce the MC sample and which to reweight.\n"
					<< "   CHECKCHECKCHECK!!! TBI properly" ;

			size_t pdfw=0;//usepdfw_;
			//use central weight to renormalize
			float wgenentries=0;

			tBranchHandler<NTEvent> b_Event(tnorm,eventbranch_);
			Long64_t nEntries=tnorm->GetEntries();
			for(Long64_t entry=0;entry<nEntries;entry++){
				b_Event.getEntry(entry);
				if(pdfw < b_Event.content()->PDFWeightsSize()){
					wgenentries+=b_Event.content()->PDFWeight(pdfw);
				}
				else{
					throw std::out_of_range("PDF weight index not found for at least one event. Check input!");
				}
			}
			for(size_t i=1;i<=generated->getNBins();i++){
				generated->setBinContent(i, (float)wgenentries);
				generated->setBinStat(i,sqrt(wgenentries));
			}
			norm = lumi_  * wgenentries;

		} */

	}
	else{//not mc
		for(size_t i=1;i<=generated->getNBins();i++){
			generated->setBinContent(i, 0);
			generated->setBinStat(i, 0);
		}
		for(size_t i=1;i<=generated2->getNBins();i++){
			generated2->setBinContent(i, 0);
			generated2->setBinStat(i, 0);
		}
		norm=1;
	}
	//check for top mass scaling
	//if(infiles_.at(anaid).Contains("ttbar") && ! infiles_.at(anaid).Contains("_ttbar")){ //selects all ttbar samples but not ttbarg/w/z
	if(legentries_.at(anaid) != dataname_){ //don't automatically rescale pseudodata
		float sampletopmass = atof(topmass_.Data());
		float energy=8;
		if(energy_ == "7TeV")
			energy=7;
		if(infiles_.at(anaid).Contains("ttbar") &&  !infiles_.at(anaid).Contains("_ttbar")){

			float xsec=getTtbarXsec(sampletopmass,energy);
			std::cout << "GetNorm: File " << infiles_.at(anaid) << "\tis a ttbar sample , top mass is "
					<< sampletopmass << " xsec: " << xsec <<std::endl;
			norms_.at(anaid)=xsec;
		}
		//tW
		if(infiles_.at(anaid).Contains("_tW.root") || infiles_.at(anaid).Contains("_tbarW.root")
				|| infiles_.at(anaid).Contains("_tbarWtoLL")|| infiles_.at(anaid).Contains("_tWtoLL")){

			float xsec=getTWXsec(sampletopmass,energy);
			norms_.at(anaid)=xsec;
		}
		//}
	}

	norm*=norms_.at(anaid);

	return norm;
}




void MainAnalyzer::setCacheProperties(TTree * t,const TString& inputtree)const{
	struct stat filestatus;
	stat(inputtree.Data(), &filestatus );

	if(!testmode_){
		t->SetCacheSize(filestatus.st_size/10);
		if(t->GetCacheSize() > 100e6)
			t->SetCacheSize(100e6);
		// t->AddBranchToCache("*");
		t->SetCacheLearnEntries(1000);
	}


}

