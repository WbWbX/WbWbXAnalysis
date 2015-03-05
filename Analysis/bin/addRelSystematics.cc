/*
 * addRelSystematics.cc
 *
 *  Created on: Nov 28, 2014
 *      Author: kiesej
 */

#include "TtZAnalysis/Tools/interface/applicationMainMacro.h"
#include "TtZAnalysis/Tools/interface/containerStackVector.h"


invokeApplication(){
	using namespace ztop;
	parser->setAdditionalDesciption("adds relative systematics from containerStackVector in first file to second file\nprovide exactly 2 files!");
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
	std::cout << "WARNING HACKS!!!"<<std::endl;
	for(size_t i=0;i<csv2->size();i++){
		csv2->getStack(i).mergeLegends("t#bar{t}(#tau)","t#bar{t}","t#bar{t}",633,true);
		csv2->getStack(i).mergeLegends("Z#rightarrowll","DY#rightarrowll","DY#rightarrowll",858,false);

	//	csv2->getStack(i).addEmptyLegend("QCD",400,9);

	}
//containerStack::debug=true;
	std::cout << "adding systematics..." <<std::endl;
	//container1D::debug=true;
	csv2->addRelSystematicsFrom(*csv,!arenotcorrelated,strict);
	TString cmd="cp "+files.at(1)+" "+files.at(1)+".bu";
	system(cmd.Data());
	csv2->writeAllToTFile(files.at(1),true,!plotplots);
	std::cout << "done." <<std::endl;

	delete csv;
	delete csv2;
	return 0;
}
