/*
 * addRelSystematics.cc
 *
 *  Created on: Nov 28, 2014
 *      Author: kiesej
 */

#include "WbWbXAnalysis/Tools/interface/applicationMainMacro.h"
#include "WbWbXAnalysis/Tools/interface/histoStackVector.h"


invokeApplication(){
	using namespace ztop;
	parser->setAdditionalDesciption("adds relative systematics from containerStackVector in first file to second file provide exactly 2 files!");
	const bool arenotcorrelated = parser->getOpt("C",false,"nominal entries are not correlated (default: false)");
	const bool plotplots = parser->getOpt("P",false,"directly produces plots (default: false)");
	const bool strict=! parser->getOpt<bool>("-approx",false,"allow a certain small difference not to count as real variation. (default false)");
	const std::vector<TString> files = parser->getRest<TString>();
	parser->doneParsing();
	if(files.size()!=2){
		parser->coutHelp();
		return 0;
	}
	if(files.at(0)==files.at(1)){
		std::cout << "same files, skip" <<std::endl;
		return 0;
	}
	std::cout << "opening files..." <<std::endl;
	histoStackVector *csv=new histoStackVector();
	try{
		csv->readFromFile(files.at(0).Data());
	}catch(...){
		std::cout << "No Plots found in file " << files.at(0) <<std::endl;
		delete csv;
		return -1;
	}
	histoStackVector *csv2=new histoStackVector();
	try{
		csv2->readFromFile(files.at(1).Data());
	}catch(...){
		std::cout << "No Plots found in file " << files.at(1) <<std::endl;
		delete csv;
		delete csv2;
		return -1;
	}

//histoStack::debug=true;
	std::cout << "adding systematics..." <<std::endl;
	//histo1D::debug=true;
	csv2->addRelSystematicsFrom(*csv,!arenotcorrelated,strict);
	TString cmd="cp "+files.at(1)+" "+files.at(1)+".bu";
	system(cmd.Data());
	csv2->writeToFile(files.at(1).Data());
	if(plotplots)
		csv2->writeAllToTFile(files.at(1)+"_plots.root",true,false);
	std::cout << "done." <<std::endl;

	delete csv;
	delete csv2;
	return 0;
}
