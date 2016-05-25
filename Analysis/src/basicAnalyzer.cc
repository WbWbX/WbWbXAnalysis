/*
 * basicAnalyzer.cc
 *
 *  Created on: 20 May 2016
 *      Author: kiesej
 */


#include "../interface/basicAnalyzer.h"
#include "TtZAnalysis/Tools/interface/fileReader.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"

namespace ztop{

basicAnalyzer::basicAnalyzer():fileForker(),
		inputfile_(""),
		legendname_(""),
		col_(0),
		norm_(1),
		legorder_(0),
		signal_(false)
{


}

void basicAnalyzer::process(){
	size_t anaid=ownChildIndex();
	inputfile_=infiles_.at(anaid); //modified in some mode options
	legendname_=legentries_.at(anaid);
	col_=colz_.at(anaid);
	legorder_=legords_.at(anaid);
	signal_=issignal_.at(anaid);
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
	for(size_t i=0;i<infiles_.size();i++)
		infiles.push_back(infiles_.at(i).Data());
	setInputFiles(infiles);


}

fileForker::fileforker_status basicAnalyzer::runParallels(int interval){
	prepareSpawn();
	fileForker::fileforker_status stat=fileForker::ff_status_parent_busy;
	while(stat==fileForker::ff_status_parent_busy || stat== fileForker::ff_status_parent_childstospawn){

		spawnChilds();
		stat=getStatus();
		std::cout << textFormatter::fixLength("Filename",30)                << " statuscode " << " progress " <<std::endl;
		for(size_t i=0;i<infiles_.size();i++)
			std::cout << textFormatter::fixLength(infiles_.at(i).Data(),30) << "     " << getStatus(i) <<"     " << "   " << getBusyStatus(i)<<"%"<<std::endl;
		std::cout << std::endl;
		sleep (interval);
	}
	return stat;
}


fileForker::fileforker_status basicAnalyzer::writeHistos(){
	if(debug)
		std::cout << "basicAnalyzer::writeHistos" <<std::endl;
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

bool basicAnalyzer::createOutFile()const{
	return true;
}
}
