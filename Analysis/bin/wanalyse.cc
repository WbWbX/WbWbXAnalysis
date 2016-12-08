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

	const bool genonly=parser->getOpt<bool>("-genonly",false,"run only on generator part");

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
	const std::string a7reweightfile= cmssw_base.Data()+fr.getValue<std::string>("A7reweightFile");

	const float detamax=fr.getValue<float>("vis_DEtaMax");
	const float detamin=fr.getValue<float>("vis_DEtaMin");
	const float wptmax=fr.getValue<float>("vis_WPtMax");
	const float wptmin=fr.getValue<float>("vis_WPtMin");

	//fileForker::debug=true;
	//basicAnalyzer::debug=true;

	//fileForker::debug=true;
	wAnalyzer ana;
	ana.setGenOnly(genonly);
	ana.setLumi(luminosity);
	ana.setTestMode(testmode);
	ana.setDataSetDirectory(treedir);
	ana.setOutputFileName("mu_8TeV_80");
	if(testmode)
		ana.setOutputFileName("mutest_8TeV_80");
	ana.setOutDir(batchbase+"output");
	ana.readFileList(configfile);
	ana.setMaxChilds(maxchilds);
	ana.getJecUnc()->setFile(jecfile.Data());
	ana.getMuonSF()->setGlobal(1,1.5,1.5);
	ana.getMuonESF()->setGlobal(1,0.5,0.5);

	ana.getNLOReweighter()->switchOff(true);
	ana.getNLOReweighter()->setSimple(false);



	ana.setSyst(syst);
	system(("rm -f "+ana.getOutPath()+".ztop").Data());

	/*
	 * Systematics configuration. Will use some code space!
	 */
	if(syst == "nominal"){
		//do nothing or?
	}
	else if(syst == "MUONSF_up"){
		ana.getMuonSF()->setSystematics("up");
	}
	else if(syst == "MUONSF_down"){
		ana.getMuonSF()->setSystematics("down");
	}
	else if(syst == "MUONESF_up"){
		ana.getMuonESF()->setSystematics("up");
	}
	else if(syst == "MUONESF_down"){
		ana.getMuonESF()->setSystematics("down");
	}
	else if(syst == "SCALEF_up"){
		ana.addWeightIndex(1); //just dummy
	}
	else if(syst == "SCALEF_down"){
		ana.addWeightIndex(2); //just dummy
	}
	else if(syst == "SCALER_up"){
		ana.addWeightIndex(3); //just dummy
	}
	else if(syst == "SCALER_down"){
		ana.addWeightIndex(6); //just dummy
	}
	else if(syst == "SCALERF_up"){
		ana.addWeightIndex(4); //just dummy
	}
	else if(syst == "SCALERF_down"){
		ana.addWeightIndex(8); //just dummy
	}
	else if(syst == "SCALERaF_up"){
		ana.addWeightIndex(7); //just dummy
	}
	else if(syst == "SCALERaF_down"){
		ana.addWeightIndex(5); //just dummy
	}
	else if(syst.BeginsWith("PDF")){
		//just define up and down arbitrary. this is NNPDF. take care of Gaussian replicas later
		//weights start at 9
		const size_t weightoffset=9;
		const size_t lastweight=110;
		size_t runningindex=0;
		for(size_t i=0; i<=lastweight-weightoffset;i++){
			if(syst == "PDF" + toTString(i) + "_down"){
				ana.addWeightIndex(runningindex+weightoffset);
				std::cout << "setting index to " << runningindex << " for " << syst << std::endl;
				break;
			}
			runningindex++;
			if(syst == "PDF" + toTString(i) + "_up"){
				ana.addWeightIndex(runningindex+weightoffset);
				std::cout << "setting index to " << runningindex << " for " << syst << std::endl;
				break;
			}
			runningindex++;
		}
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
	else if(syst.BeginsWith("WparaA7")){
		ana.getNLOReweighter()->switchOff(false);
		ana.getNLOReweighter()->readParameterFile(a7reweightfile);
		ana.getNLOReweighter()->setDEtaMin(detamin);
		ana.getNLOReweighter()->setDEtaMax(detamax);
		ana.getNLOReweighter()->setWptMin(wptmin);
		ana.getNLOReweighter()->setWptMax(wptmax);
		if(syst == "WparaA7_up"){
			ana.getNLOReweighter()->setReweightParameter(7,1);
		}
		else if(syst == "WparaA7_down"){
			ana.getNLOReweighter()->setReweightParameter(7,-1);

		}
		else if(syst.BeginsWith("WparaA7_oops")){
			ana.getNLOReweighter()->setDEtaMin(detamax);
			ana.getNLOReweighter()->setDEtaMax(1e13);
			ana.getNLOReweighter()->setWptMin(0);
			ana.getNLOReweighter()->setWptMax(wptmin);
			if(syst.EndsWith("up"))
				ana.getNLOReweighter()->setReweightParameter(7,1);
			else
				ana.getNLOReweighter()->setReweightParameter(7,-1);
		}
	}
	else if(syst.BeginsWith("WparaA") ){
		TString number=syst;number.ReplaceAll("WparaA","");
		bool up=number.EndsWith("_up");
		number.ReplaceAll("_up","");
		number.ReplaceAll("_down","");
		size_t parameter=number.Atoi();
		ana.getNLOReweighter()->switchOff(false);
		ana.getNLOReweighter()->readParameterFile(a7reweightfile);
		ana.getNLOReweighter()->setDEtaMin(0);
		ana.getNLOReweighter()->setDEtaMax(1e13);
		ana.getNLOReweighter()->setWptMin(0);
		ana.getNLOReweighter()->setWptMax(1e9);
		if(up)
			ana.getNLOReweighter()->setReweightParameter(parameter,0.3);
		else
			ana.getNLOReweighter()->setReweightParameter(parameter,-0.3);

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
