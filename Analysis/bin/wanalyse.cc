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
	using namespace std;

	std::string configfile=parser->getOpt<std::string>
	("i","config.txt","specify configuration file input manually (all in configs/wanalyse)");

	const bool testmode=parser->getOpt<bool>("T",false,"testmode");

	const size_t maxchilds=parser->getOpt<int>("m",6,"max childs");

	const TString syst=parser->getOpt<TString>("s","nominal","systematics string");

	parser->doneParsing();


	TString cmssw_base=getenv("CMSSW_BASE");
	TString scram_arch=getenv("SCRAM_ARCH");
	TString batchbase = getenv("ANALYSE_BATCH_BASE");


	TString database="data/wanalyse/";
	TString configbase="configs/wanalyse/";
	if(batchbase.Length()>0){
		batchbase+="/";
		database=batchbase+database;
		configbase=batchbase+configbase;
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


	std::string treedir=              fr.getValue<std::string>("inputFilesDir");
	const double luminosity = fr.getValue<double>("Lumi");
	const TString jecfile = cmssw_base+fr.getValue<TString>("JECUncertainties");
	//fileForker::debug=true;
	//basicAnalyzer::debug=true;

	//fileForker::debug=true;
	wAnalyzer ana;
	ana.setLumi(luminosity);
	ana.setTestMode(testmode);
	ana.setDataSetDirectory(treedir);
	ana.setOutputFileName("mu_8TeV_80");
	ana.setOutDir(batchbase+"output");
	ana.readFileList(configfile);
	ana.setMaxChilds(maxchilds);
	ana.getJecUnc()->setFile(jecfile.Data());

	ana.setSyst(syst);
	system(("rm -f "+ana.getOutPath()+".ztop").Data());

	/*
	 * Systematics configuration. Will use some code space!
	 */
	if(syst == "nominal"){
		//do nothing or?
	}
	else if(syst == "SCALE_up"){
		ana.addWeightIndex(1); //just dummy
	}
	else if(syst == "SCALE_down"){
		ana.addWeightIndex(2); //just dummy
	}
	else if(syst == "JES_up"){
		ana.getJecUnc()->setSystematics("up");
	}
	else if(syst == "JES_down"){
		ana.getJecUnc()->setSystematics("down");
	}
	else if(syst == "CHARGEFLIP_up"){
		ana.getChargeFlip()->setRate(0.001);
	}
	else if(syst == "CHARGEFLIP_down"){
		//nothing
	}



	fileForker::fileforker_status status= ana.start();

	if(batchbase.Length()){
		std::string filename=textFormatter::getFilename(ana.getOutPath().Data(),true);

		if(status == fileForker::ff_status_parent_success)
			system(("touch "+batchbase+"/batch/"+filename+"_fin").Data());
		else
			system(("touch "+batchbase+"/batch/"+filename+"_failed").Data());
	}
	return 0;
}
