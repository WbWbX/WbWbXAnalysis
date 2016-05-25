/*
 * wanalyse.cc
 *
 *  Created on: 23 May 2016
 *      Author: kiesej
 */

#include "TtZAnalysis/Tools/interface/applicationMainMacro.h"
#include <string>
#include "TtZAnalysis/Tools/interface/fileReader.h"
#include "TtZAnalysis/Analysis/interface/wAnalyzer.h"
#include "TString.h"

invokeApplication(){
	using namespace ztop;

	std::string configfile=parser->getOpt<std::string>
	("i","config.txt","specify configuration file input manually (all in configs/wanalyse)");

	size_t maxchilds=parser->getOpt<int>("m",6,"max childs");

	parser->doneParsing();


	TString cmssw_base=getenv("CMSSW_BASE");
	TString scram_arch=getenv("SCRAM_ARCH");
	TString batchbase = getenv("ANALYSE_BATCH_BASE");


	TString database="data/wanalyse/";
	TString configbase="configs/wanalyse/";
	if(batchbase.Length()>0){
		database=batchbase+"/"+database;
		configbase=batchbase+"/"+configbase;
		std::cout << "Batch run from " << batchbase << std::endl;
	}
	else{
		database=cmssw_base+"/src/TtZAnalysis/Analysis/"+database;
		configbase=cmssw_base+"/src/TtZAnalysis/Analysis/"+configbase;
	}

	configfile=configbase+configfile;

	fileReader fr;
	fr.setComment("$");
	fr.setDelimiter(",");
	fr.setStartMarker("[parameters-begin]");
	fr.setEndMarker("[parameters-end]");
	fr.readFile(configfile);
	maxchilds++;

	std::string treedir=              fr.getValue<std::string>("inputFilesDir");
	//fileForker::debug=true;
	//basicAnalyzer::debug=true;

	//fileForker::debug=true;
	wAnalyzer ana;
	ana.setDataSetDirectory(treedir);
	ana.setOutputFileName("wout.ztop");
	ana.readFileList(configfile);
	ana.setMaxChilds(1);

	ana.runParallels(2);


	return 0;
}
