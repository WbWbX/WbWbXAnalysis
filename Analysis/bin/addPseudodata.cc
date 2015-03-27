/*
 * addPseudodata.cc
 *
 *  Created on: Dec 18, 2014
 *      Author: kiesej
 */




#include "TtZAnalysis/Tools/interface/applicationMainMacro.h"
#include "TtZAnalysis/Tools/interface/histoStackVector.h"
#include "TRandom.h"

invokeApplication(){
	using namespace ztop;
	parser->setAdditionalDesciption("small program to add pseudo-data on a full pure-MC HistoStackVector.\nOnly works if no data is present!\
\nusage: addPseudodata <inputfile>.\n will scan all control, 2d, unfold plots included and add pseudodata to it.\nOutput can be specified, otherwise input will be overwritten");
	const TString output=parser->getOpt<TString>("o","","output file name. Default: Overwrite input file.");
	const bool makecanvases=parser->getOpt<bool>("P",false,"plots canvases directly using default plotting style. (default false). Produces additional .root file");
	const int randomseed=parser->getOpt<int>("r",1234,"Specify random seed. (default 1234)");
	const TString infile=parser->getRest<TString>(0);
	parser->doneParsing();
	if(infile.Length()<1){
		parser->coutHelp(); //exits
	}
	histoStackVector * csv=new histoStackVector();

	try{
		csv->readFromFile(infile.Data());
	}catch(...){
		std::cout << infile << " does not contain a valid histoStackVector"<<std::endl;
		delete csv;
		return -1;
	}

	//do adding
	TRandom3 * rand=new TRandom3(randomseed);
	csv->addPseudoData(rand);
	delete rand;


	if(output.Length()>0){
		csv->writeToFile(output.Data());
		if(makecanvases)
			csv->writeAllToTFile(output,true,!makecanvases);
	}
	else{
		csv->writeToFile(infile.Data());
		if(makecanvases)
			csv->writeAllToTFile(infile+".root",true,!makecanvases);
	}
	delete csv;
	return 0;
}
