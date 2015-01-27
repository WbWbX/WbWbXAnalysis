/*
 * addPseudodata.cc
 *
 *  Created on: Dec 18, 2014
 *      Author: kiesej
 */




#include "TtZAnalysis/Tools/interface/applicationMainMacro.h"
#include "TtZAnalysis/Tools/interface/containerStackVector.h"
#include "TRandom.h"

invokeApplication(){
	using namespace ztop;
	parser->setAdditionalDesciption("small program to add pseudo-data on a full pure-MC containerStackVector.\nOnly works if no data is present!\
\nusage: addPseudodata <inputfile>.\n will scan all control, 2d, unfold plots included and add pseudodata to it.\nOutput can be specified, otherwise input will be overwritten");
	const TString output=parser->getOpt<TString>("o","","output file name. Default: Overwrite input file.");
	const bool makecanvases=parser->getOpt<bool>("P",false,"plots canvases directly using default plotting style. (default false)");
	const TString infile=parser->getRest<TString>(0);
	parser->doneParsing();
	if(infile.Length()<1){
		parser->coutHelp(); //exits
	}
	containerStackVector * csv=new containerStackVector();

	try{
		csv->loadFromTFile(infile);
	}catch(...){
		std::cout << infile << " does not contain a valid containerStackVector"<<std::endl;
		delete csv;
		return -1;
	}

	//do adding
	TRandom3 * rand=new TRandom3();
	csv->addPseudoData(rand);
	delete rand;


	if(output.Length()>0){
		csv->writeAllToTFile(output,true,!makecanvases);
	}
	else{
		csv->writeAllToTFile(infile,true,!makecanvases);
	}
	delete csv;
	return 0;
}
