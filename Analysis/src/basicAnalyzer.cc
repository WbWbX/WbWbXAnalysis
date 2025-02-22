/*
 * basicAnalyzer.cc
 *
 *  Created on: 20 May 2016
 *      Author: kiesej
 */


#include "../interface/basicAnalyzer.h"
#include "WbWbXAnalysis/Tools/interface/fileReader.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"

namespace ztop{

basicAnalyzer::basicAnalyzer():fileForker(),
		inputfile_(""),
		legendname_(""),
		col_(0),
		norm_(1),
		legorder_(0),
		signal_(false),
		lumi_(0),
		freplaced_(0),
		testmode_(false),
		isMC_(true),
		datalegend_("data")
{


}

void basicAnalyzer::process(){
	size_t anaid=ownChildIndex();
	inputfile_=infiles_.at(anaid); //modified in some mode options
	legendname_=legentries_.at(anaid);
	col_=colz_.at(anaid);
	legorder_=legords_.at(anaid);
	signal_=issignal_.at(anaid);
	isMC_ = legendname_ != datalegend_;
	analyze(anaid);
}


void basicAnalyzer::readFileList(const std::string& inputfile){
	if(debug)
		std::cout << "basicAnalyzer::readFileList" << std::endl;
	using namespace ztop;
	using namespace std;

	fileReader fr;
	fr.setDelimiter(",");
	fr.setComment("$");
	fr.setStartMarker("[inputfiles-begin]");
	fr.setEndMarker("[inputfiles-end]");
	fr.readFile(inputfile);

	infiles_.clear();
	legentries_.clear();
	colz_.clear();
	norms_.clear();
	legords_.clear();
	issignal_.clear();
	extraopts_.clear();
	std::vector<std::string> infiles;

	std::cout << "basicAnalyzer::readFileList: reading input file " << std::endl;

	for(size_t line=0;line<fr.nLines();line++){
		if(fr.nEntries(line) < 5){
			std::cout << "basicAnalyzer::readFileList: line " << line << " of inputfile is broken ("<<fr.nEntries(line)<< " entries.)" <<std::endl;
			sleep(2);
			continue;
		}
		infiles_.push_back   ((fr.getData<TString>(line,0)));
		if(debug)
			std::cout << "basicAnalyzer::readFileList: " << infiles_.at(infiles_.size()-1) << std::endl;
		legentries_.push_back(fr.getData<TString>(line,1));
		colz_.push_back      (fr.getData<int>    (line,2));
		norms_.push_back     (fr.getData<double> (line,3));
		legords_.push_back    (fr.getData<size_t> (line,4));
		if(fr.nEntries(line) > 5)
			issignal_.push_back(fr.getData<bool> (line,5));
		else
			issignal_.push_back(false);
		if(fr.nEntries(line) > 6)
			extraopts_.push_back(fr.getData<TString> (line,6));
		else
			extraopts_.push_back("");

	}
	std::vector<std::string > newinfiles;
	for(size_t i=0;i<infiles_.size();i++){
		//if(legentries_.at(i) == dataname_)
		//	continue;
		infiles_.at(i) =   replaceExtension(infiles_.at(i));
		///load pdf files
		newinfiles.push_back(infiles_.at(i).Data());

	}
	fileForker::setInputFiles(newinfiles);


}


TString basicAnalyzer::replaceExtension(TString filename){
	for(size_t i=0;i<ftorepl_.size();i++){
		if(filename.Contains(ftorepl_.at(i))){
			freplaced_++;
			return filename.ReplaceAll(ftorepl_.at(i),fwithfix_.at(i));
		}
	}
	return filename;
}

fileForker::fileforker_status basicAnalyzer::runParallels(int interval){
	prepareSpawn();
	fileForker::fileforker_status stat=fileForker::ff_status_parent_busy;
	int counter=0;
	interval*=4; //to make it roughly seconds

	double killthreshold=1800; //seconds. Kill after 30 Minutes without notice

	time_t now;
	time_t started;
	time(&started);
	time(&now);
	std::vector<double> lastAliveSignals(infiles_.size());
	std::vector<int>    lastBusyStatus(infiles_.size());
	while(stat==fileForker::ff_status_parent_busy || stat== fileForker::ff_status_parent_childstospawn){

		fileForker::fileforker_status writestat=spawnChildsAndUpdate();
		stat=getStatus();
		if(writestat == ff_status_parent_filewritten || (interval>0  && counter>interval)){
			time(&now);
			double runningseconds = fabs(difftime(started,now));
			std::cout << "PID    "<< textFormatter::fixLength("Filename",50)                << " statuscode " << " progress " <<std::endl;
			int totalstatus=0;
			for(size_t i=0;i<infiles_.size();i++){
				totalstatus+=getBusyStatus(i);
				double percentpersecond=getBusyStatus(i)/runningseconds;
				double estimate=(100-getBusyStatus(i))/percentpersecond;
				std::cout
				<< textFormatter::fixLength(toString(getChildPids().at(i)),7)
				<< textFormatter::fixLength(infiles_.at(i).Data(),         50)
				<< textFormatter::fixLength(translateStatus(getStatus(i)), 15)
				<< textFormatter::fixLength(toString(getBusyStatus(i))+"%",4,false);
				if(getBusyStatus(i)>4 && getStatus(i) == ff_status_child_busy){
					std::cout  <<" ETA: ";
					int minutes=estimate/60;
					std::cout << std::setw(2) << std::setfill('0')<< minutes << ":" <<
							std::setw(2) << std::setfill('0')<<(int)(estimate - minutes*60) ;
				}
				std::cout <<std::endl;
				if(lastBusyStatus.at(i)!=getBusyStatus(i) || getStatus(i) != ff_status_child_busy){
					lastBusyStatus.at(i)=getBusyStatus(i);
					lastAliveSignals.at(i)=runningseconds;
				}
				else if(getStatus(i) == ff_status_child_busy){
					if(runningseconds-lastAliveSignals.at(i) > killthreshold){
						//for a long time no signal, but "busy" something is very likely wrong
						std::cout << "-> seems to be hanging. Process will be killed." <<std::endl;
						abortChild(i);
					}
				}
			}
			std::cout << "total\n"<<std::endl;
			displayStatusBar(totalstatus,100* infiles_.size() ,50,true);
			std::cout << std::endl;
			std::cout << std::endl;
			counter=0;
		}
		counter++;
		usleep (250e3);
	}
	std::cout << "End report:" <<std::endl;
	std::cout << textFormatter::fixLength("Filename",30)                << " statuscode " << " progress " <<std::endl;
	for(size_t i=0;i<infiles_.size();i++)
		std::cout << textFormatter::fixLength(infiles_.at(i).Data(),50) << "     " << textFormatter::fixLength(translateStatus(getStatus(i)),15) <<"     " << "   " << getBusyStatus(i)<<"%"<<std::endl;
	std::cout << std::endl;


	//if no data, then create pseudodata
	if(std::find(legentries_.begin(),legentries_.end(),datalegend_) == legentries_.end()){
		std::cout << "no data found, generating pseudo data (Poisson smeared)" <<std::endl;
		histoStackVector * hsv= new histoStackVector();
		hsv->readFromFile(getOutputFileName());
		TRandom3 rand;
		hsv->addPseudoData(&rand);
		hsv->writeToFile(getOutputFileName());
	}

	return stat;
}


void basicAnalyzer::setOutDir(const TString& dir){
	if(dir.EndsWith("/"))
		outdir_=dir;
	else
		outdir_=dir+"/";
}

fileForker::fileforker_status basicAnalyzer::writeHistos(){
	if(debug)
		std::cout << "basicAnalyzer::writeHistos" <<std::endl;

	histo1DUnfold::flushAllListed(); //just to be sure

	ztop::histoStackVector * csv=&allplotsstackvector_;

	if(fileExists(getOutputFileName().data())) {
		csv->readFromFile(getOutputFileName());
	}
	csv->addList1DUnfold(legendname_,col_,norm_,legorder_);
	csv->addList2D(legendname_,col_,norm_,legorder_);
	csv->addList(legendname_,col_,norm_,legorder_);
	if(signal_)
		csv->addSignal(legendname_);

	if(debug)
		std::cout << "basicAnalyzer::writeHistos: writing" <<std::endl;
	if(csv->size())
		csv->writeToFile(getOutputFileName());

	csv->clear();//frees mem

	if(debug)
		std::cout << "basicAnalyzer::writeHistos: done" <<std::endl;
	return ff_status_child_success;
}


void basicAnalyzer::reportStatus(const Long64_t& entry,const Long64_t& nEntries){
	static Long64_t step=0;
	static Long64_t next=0;
	if(!step){
		step=nEntries/400;
	}
	if(entry==next || entry==nEntries-1){
		next+=step;
		int status=(entry+1) * 100 / nEntries;
		reportBusyStatus(status);
	}
}

void basicAnalyzer::setFilePostfixReplace(const TString& file,const TString& pf,bool clear){
	if(clear){ fwithfix_.clear();ftorepl_.clear();}
	ftorepl_.push_back(file); fwithfix_.push_back(pf);
}
void basicAnalyzer::setFilePostfixReplace(const std::vector<TString>& files,const std::vector<TString>& pf){
	if(files.size() != pf.size()){
		std::string errstr= "setFilePostfixReplace: vectors have to be same size!";
		throw std::logic_error(errstr);
	}
	ftorepl_=files;fwithfix_=pf;
}

bool basicAnalyzer::createOutFile()const{
	if(outdir_.Length())
		system(("mkdir -p "+outdir_).Data());
	return true;
}
}
