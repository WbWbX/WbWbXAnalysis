/*
 * addRelSystematics.cc
 *
 *  Created on: Nov 28, 2014
 *      Author: kiesej
 */

#include "mainMacro.h"
#include "TtZAnalysis/Tools/interface/containerStackVector.h"


invokeApplication(){
	using namespace ztop;
	parser->setAdditionalDesciption("adds relative systematics from containerStackVector in first file to second file\nprovide exactly 2 files!");
	const bool arenotcorrelated = parser->getOpt("-C",false,"nominal entries are not correlated (default: false)");
	const std::vector<TString> files = parser->getRest<TString>();
	parser->doneParsing();
	if(files.size()!=2){
		parser->coutHelp();
	}
	std::cout << "opening files..." <<std::endl;
	containerStackVector *csv=new containerStackVector();
	try{
		csv->loadFromTFile(files.at(0));
	}catch(...){
		std::cout << "No Plots found in file " << files.at(0) <<std::endl;
		delete csv;
		return -1;
	}
	containerStackVector *csv2=new containerStackVector();
	try{
		csv2->loadFromTFile(files.at(1));
	}catch(...){
		std::cout << "No Plots found in file " << files.at(1) <<std::endl;
		delete csv;
		delete csv2;
		return -1;
	}

	std::cout << "adding systematics..." <<std::endl;
	csv2->addRelSystematicsFrom(*csv,!arenotcorrelated);

	csv2->writeAllToTFile(files.at(1),true,true);
	std::cout << "done." <<std::endl;

	delete csv;
	delete csv2;
	return 0;
}
