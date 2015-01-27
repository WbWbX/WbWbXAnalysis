#include "TtZAnalysis/Analysis/interface/MainAnalyzer.h"
#include "TtZAnalysis/Analysis/interface/AnalysisUtils.h"
#include "TtZAnalysis/Tools/interface/fileReader.h"
#include "../interface/discriminatorFactory.h"
#include <time.h>
//#include "Analyzer.cc"
//should be taken care of by the linker!



void analyse(TString channel, TString Syst, TString energy, TString outfileadd,
		double lumi, bool dobtag, bool status,bool testmode,bool tickonce,TString maninputfile,
		TString mode,TString topmass,TString btagfile,bool createLH, std::string discrfile,float fakedatastartentries,bool interactive,float wpval){ //options like syst..

	bool didnothing=false;
	//some env variables
	TString cmssw_base=getenv("CMSSW_BASE");
	TString scram_arch=getenv("SCRAM_ARCH");
	TString batchbase = getenv("ANALYSE_BATCH_BASE");

	TString database="data/analyse/";
	TString configbase="configs/analyse/";

	if(batchbase.Length()>0){
		database=batchbase+"/"+database;
		configbase=batchbase+"/"+configbase;
		std::cout << "Batch run from " << batchbase << std::endl;
	}
	else{
		database=cmssw_base+"/TtZAnalysis/Analysis/"+database;
		configbase=cmssw_base+"/TtZAnalysis/Analysis/"+configbase;
	}

	using namespace std;
	using namespace ztop;
	if(testmode){
		ztop::container1D::debug =false;
		ztop::containerStack::debug=false;
		ztop::containerStackVector::debug=false;
	}

	TString inputfilewochannel="config.txt";
	TString inputfile=channel+"_"+energy+"_"+inputfilewochannel;
	// TString
	if(maninputfile!="")
		inputfile=maninputfile;
	//do not prepend absolute path (batch submission)
	inputfile=configbase+inputfile;

	fileReader fr;
	fr.setComment("$");
	fr.setDelimiter(",");
	fr.setStartMarker("[parameters-begin]");
	fr.setEndMarker("[parameters-end]");
	fr.readFile(inputfile.Data());


	TString treedir,jecfile,pufile,muonsffile,muonsfhisto,elecsffile,elecsfhisto,trigsffile,elecensffile,muonensffile,trackingsffile,trackingsfhisto; //...
	TString btagWP,btagshapehffile,btagshapelffile;

	if(lumi<0)
		lumi=fr.getValue<double>("Lumi");
	treedir=              fr.getValue<TString>("inputFilesDir");
	jecfile=   cmssw_base+fr.getValue<TString>("JECFile");
	muonsffile=cmssw_base+fr.getValue<TString>("MuonSFFile");
	muonsfhisto=          fr.getValue<TString>("MuonSFHisto");
	trackingsffile=cmssw_base+fr.getValue<TString>("TrackingSFFile");
	trackingsfhisto=          fr.getValue<TString>("TrackingSFHisto");
	elecsffile=cmssw_base+fr.getValue<TString>("ElecSFFile");
	elecsfhisto=          fr.getValue<TString>("ElecSFHisto");
	trigsffile=cmssw_base+fr.getValue<TString>("TriggerSFFile");
	pufile=    cmssw_base+fr.getValue<TString>("PUFile");
	//elecensffile =cmssw_base+fr.getValue<TString>("ElecEnergySFFile");
	//muonensffile =cmssw_base+fr.getValue<TString>("MuonEnergySFFile");

	btagWP=               fr.getValue<TString>("btagWP");
	btagshapehffile       =  fr.getValue<TString>("btagShapeFileHF");
	btagshapelffile       =  fr.getValue<TString>("btagShapeFileLF");

	//mode options CAN be additionally parsed by file
	fr.setRequireValues(false);
	mode                 += fr.getValue<TString>("modeOptions",TString(mode));




	TString trigsfhisto="scalefactor_eta2d_with_syst";


	std::cout << "inputfile read" << std::endl;

	//hard coded:

	system("mkdir -p output");
	TString outdir="output";


	if(dobtag){
		btagfile=outdir+"/"+ channel+"_"+energy+"_"+topmass+"_"+Syst+"_btags.root";
		system(("rm -f "+btagfile).Data());
	}
	else if(Syst.BeginsWith("BTAG")){
		btagfile=database+btagfile+"/"+ channel+"_"+energy+"_"+topmass+"_"+"nominal"+"_btags.root";
	}
	else{
		btagfile=database+btagfile+"/"+ channel+"_"+energy+"_"+topmass+"_"+Syst+"_btags.root";
	}


	///set input files list etc (common)



	MainAnalyzer ana;
	//only used in special cases!
	ana.setPathToConfigFile(inputfile);

	ana.setFakeDataStartNEntries(fakedatastartentries);
	///some checks



	NTBTagSF::modes btagmode=NTBTagSF::randomtagging_mode;
	if(mode.Contains("Btagshape")){
		std::cout << "Using b-tag shape reweighting" <<std::endl;
		btagmode=NTBTagSF::shapereweighting_mode;
		ana.getBTagSF()->readShapeReweightingFiles(cmssw_base+btagshapehffile, cmssw_base+btagshapelffile);

	}

	ana.setMaxChilds(6);
	if(interactive || tickonce)
		ana.setMaxChilds(20);
	if(testmode)
		ana.setMaxChilds(1);
	ana.setMode(mode);
	ana.setShowStatus(status);
	ana.setTestMode(testmode);
	ana.setTickOnceMode(tickonce);
	ana.setLumi(lumi);
	ana.setDataSetDirectory(treedir);
	ana.setUseDiscriminators(!createLH);
	ana.setDiscriminatorInputFile(discrfile);
	ana.getPUReweighter()->setDataTruePUInput(pufile+".root");
	ana.setChannel(channel);
	ana.setEnergy(energy);
	ana.setSyst(Syst);
	ana.setTopMass(topmass);
	if(energy == "8TeV"){
		ana.getPUReweighter()->setMCDistrSum12();
	}
	else if(energy == "7TeV"){
		ana.getPUReweighter()->setMCDistrSummer11Leg();
	}
	ana.getElecSF()->setInput(elecsffile,elecsfhisto);
	ana.getMuonSF()->setInput(muonsffile,muonsfhisto);
	ana.getTrackingSF()->setInput(trackingsffile,trackingsfhisto);
	ana.getTriggerSF()->setInput(trigsffile,trigsfhisto);

	ana.getElecEnergySF()->setGlobal(1,0.15,0.15);
	ana.getMuonEnergySF()->setGlobal(1,0.3,0.3); //new from muon POG twiki
	//https://twiki.cern.ch/twiki/bin/viewauth/CMS/MuonReferenceResolution
	//this is for muons without the corrections so it should be even better with

	ana.getTopPtReweighter()->setFunction(reweightfunctions::toppt);
	ana.getTopPtReweighter()->setSystematics(reweightfunctions::nominal);

	ana.setOutFileAdd(outfileadd);
	ana.setOutDir(outdir);
	ana.getBTagSF()->setMakeEff(dobtag);
	ana.setBTagSFFile(btagfile);
	ana.getBTagSF()->setMode(btagmode);
	ana.getBTagSF()->setWorkingPoint(btagWP);
	if(btagmode==NTBTagSF::shapereweighting_mode && wpval>-2)
		ana.getBTagSF()->setWorkingPointValue(wpval);
	ana.getJECUncertainties()->setFile((jecfile).Data());
	ana.getJECUncertainties()->setSystematics("no");
	if(testmode && dobtag){
		ana.setBTagSFFile(btagfile+"TESTMODE");
		system(("rm -f "+btagfile+"TESTMODE").Data());
	}
	//add indication for non-correlated syst by adding the energy to syst name!! then the getCrossSections stuff should recognise it

	//although it produces overhead, add background rescaling here?

	//using the sys nominal
	//btag efficiencies are NOT required here

	if(Syst.Contains("_sysnominal")){
		ana.getBTagSF()->setMakeEff(true);
		ana.setBTagSFFile(btagfile+Syst+"_dummy");
	}

	if(Syst=="nominal"){
		//all already defined
	}
	///sys nominals...
	else if(Syst=="P11_sysnominal"){
		ana.setFilePostfixReplace("ttbar.root","ttbar_mgdecays_p11.root");
		ana.setFilePostfixReplace("ttbarviatau.root","ttbarviatau_mgdecays_p11.root");
	}
	else if(Syst=="P11_sysnominal_CR_up"){ //do nothing
		ana.setFilePostfixReplace("ttbar.root","ttbar_mgdecays_p11.root");
		ana.setFilePostfixReplace("ttbarviatau.root","ttbarviatau_mgdecays_p11.root");
	}
	else if(Syst=="P11_sysnominal_CR_down"){ //no CR
		ana.setFilePostfixReplace("ttbar.root","ttbar_mgdecays_p11nocr.root");
		ana.setFilePostfixReplace("ttbarviatau.root","ttbarviatau_mgdecays_p11nocr.root");
	}
	else if(Syst=="P11_sysnominal_UE_up"){
		ana.setFilePostfixReplace("ttbar.root","ttbar_mgdecays_p11mpihi.root");
		ana.setFilePostfixReplace("ttbarviatau.root","ttbarviatau_mgdecays_p11mpihi.root");
	}
	else if(Syst=="P11_sysnominal_UE_down"){
		ana.setFilePostfixReplace("ttbar.root","ttbar_mgdecays_p11tev.root");
		ana.setFilePostfixReplace("ttbarviatau.root","ttbarviatau_mgdecays_p11tev.root");
	}
	else if(Syst=="PDF_sysnominal"){
		ana.getPdfReweighter()->setPdfIndex(0);
	}
	else if(Syst.Contains("PDF_sysnominal_")){
		size_t pdfindex=0;
		for(size_t i=1;i<10000;i++){
			pdfindex++;
			if(Syst == "PDF_sysnominal_PDF" + toTString(i) + "_down"){
				ana.getPdfReweighter()->setPdfIndex(pdfindex);
				std::cout << "setting index to " << pdfindex << " for " << Syst << std::endl;
				break;
			}
			pdfindex++;
			if(Syst == "PDF_sysnominal_PDF" + toTString(i) + "_up"){
				ana.getPdfReweighter()->setPdfIndex(pdfindex);
				std::cout << "setting index to " << pdfindex << " for " << Syst << std::endl;
				break;
			}

		}
	}



	else if(Syst=="TRIGGER_down"){
		ana.getTriggerSF()->setSystematics("down");
	}
	else if(Syst=="TRIGGER_up"){
		ana.getTriggerSF()->setSystematics("up");
	}
	else if(Syst=="ELECSF_up"){
		ana.getElecSF()->setSystematics("up");
	}
	else if(Syst=="ELECSF_down"){
		ana.getElecSF()->setSystematics("down");
	}
	else if(Syst=="MUONSF_up"){
		ana.getMuonSF()->setSystematics("up");
	}
	else if(Syst=="MUONSF_down"){
		ana.getMuonSF()->setSystematics("down");
	}
	else if(Syst=="MUONES_up"){
		ana.getMuonEnergySF()->setSystematics("up");
	}
	else if(Syst=="MUONES_down"){
		ana.getMuonEnergySF()->setSystematics("down");
	}
	else if(Syst=="ELECES_up"){
		ana.getElecEnergySF()->setSystematics("up");
	}
	else if(Syst=="ELECES_down"){
		ana.getElecEnergySF()->setSystematics("down");
	}
	else if(Syst=="JES_up"){
		ana.getJECUncertainties()->setSystematics("up");
		ana.getBTagSF()->setSystematic(bTagBase::jesup);
	}
	else if(Syst=="JES_down"){
		ana.getJECUncertainties()->setSystematics("down");
		ana.getBTagSF()->setSystematic(bTagBase::jesdown);
	}


	/*
	 * Individual JES sources block!
	 *
	 */
	else if(Syst.BeginsWith("JES_")){
		std::vector<std::string> sources;

		sources = ana.getJECUncertainties()->getSourceNames();

		for(size_t i=0;i<=sources.size();i++){
			std::cout << sources.at(i) <<std::endl;
			if(i==sources.size())
				throw std::runtime_error("JES source not found! exit!");

			//jes systematics
			if("JES_"+(TString)sources.at(i)+"_down" == Syst || "JES_"+sources.at(i)+"_up" == Syst){
				ana.getJECUncertainties()->setSource(sources.at(i));
				//some specials

				if(sources.at(i) == "FlavorPureGluon"){
					ana.getJECUncertainties()->restrictToFlavour(23);
					ana.getJECUncertainties()->restrictToFlavour(0);
				}
				else if(sources.at(i) == "FlavorPureQuark"){
					ana.getJECUncertainties()->restrictToFlavour (1);
					ana.getJECUncertainties()->restrictToFlavour(-1);
					ana.getJECUncertainties()->restrictToFlavour (2);
					ana.getJECUncertainties()->restrictToFlavour(-2);
					ana.getJECUncertainties()->restrictToFlavour (3);
					ana.getJECUncertainties()->restrictToFlavour(-3);
				}
				else if(sources.at(i) == "FlavorPureCharm"){
					ana.getJECUncertainties()->restrictToFlavour(4);
					ana.getJECUncertainties()->restrictToFlavour(-4);
				}
				else if(sources.at(i) == "FlavorPureBottom"){
					ana.getJECUncertainties()->restrictToFlavour(5);
					ana.getJECUncertainties()->restrictToFlavour(-5);
				}

				if("JES_"+(TString)sources.at(i)+"_down" == Syst){
					ana.getJECUncertainties()->setSystematics("down");
					break;
				}
				else if("JES_"+sources.at(i)+"_up" == Syst){
					ana.getJECUncertainties()->setSystematics("up");
					break;
				}
			}
		}
	}



	/*
	 * END INDIVI JES SOURCES BLOCK
	 */


	else if(Syst=="JEC_residuals_up"){
		//use default
		//	ana.setFilePostfixReplace("_22Jan.root","_22Jan_ptres.root");
	}
	else if(Syst=="JEC_residuals_down"){
		ana.setFilePostfixReplace("_22Jan.root","_22Jan_noptres.root");
	}

	else if(Syst=="JER_up"){
		ana.getJERAdjuster()->setSystematics("up");
	}
	else if(Syst=="JER_down"){
		ana.getJERAdjuster()->setSystematics("down");
	}
	else if(Syst=="PU_up"){
		ana.getPUReweighter()->setDataTruePUInput(pufile+"_up.root");
	}
	else if(Syst=="PU_down"){
		ana.getPUReweighter()->setDataTruePUInput(pufile+"_down.root");
	}

	/////////btag

	else if(Syst=="BTAGH_up"){
		ana.getBTagSF()->setSystematic(bTagBase::heavyup);
	}
	else if(Syst=="BTAGH_down"){
		ana.getBTagSF()->setSystematic(bTagBase::heavydown);
	}
	else if(Syst=="BTAGL_up"){
		ana.getBTagSF()->setSystematic(bTagBase::lightup);
	}
	else if(Syst=="BTAGL_down"){
		ana.getBTagSF()->setSystematic(bTagBase::lightdown);
	}
	else if(Syst=="BTAGHFS1_up"){
		ana.getBTagSF()->setSystematic(bTagBase::hfstat1up);
	}
	else if(Syst=="BTAGHFS1_down"){
		ana.getBTagSF()->setSystematic(bTagBase::hfstat1down);
	}
	else if(Syst=="BTAGHFS2_up"){
		ana.getBTagSF()->setSystematic(bTagBase::hfstat2up);
	}
	else if(Syst=="BTAGHFS2_down"){
		ana.getBTagSF()->setSystematic(bTagBase::hfstat2down);
	}
	else if(Syst=="BTAGLFS1_up"){
		ana.getBTagSF()->setSystematic(bTagBase::lfstat1up);
	}
	else if(Syst=="BTAGLFS1_down"){
		ana.getBTagSF()->setSystematic(bTagBase::lfstat1down);
	}
	else if(Syst=="BTAGLFS2_up"){
		ana.getBTagSF()->setSystematic(bTagBase::lfstat2up);
	}
	else if(Syst=="BTAGLFS2_down"){
		ana.getBTagSF()->setSystematic(bTagBase::lfstat2down);
	}
	else if(Syst=="BTAGPUR_up"){
		ana.getBTagSF()->setSystematic(bTagBase::purityup);
	}
	else if(Syst=="BTAGPUR_down"){
		ana.getBTagSF()->setSystematic(bTagBase::puritydown);
	}


	/////////top pt

	else if(Syst=="TOPPT_up"){
		ana.getTopPtReweighter()->setSystematics(reweightfunctions::up);
	}
	else if(Syst=="TOPPT_down"){
		ana.getTopPtReweighter()->setSystematics(reweightfunctions::down);
	}
	///////////////////file replacements/////////////
	else if(Syst=="TT_MATCH_up"){
		ana.setFilePostfixReplace("ttbar.root","ttbar_ttmup.root");
		ana.setFilePostfixReplace("ttbarviatau.root","ttbarviatau_ttmup.root");
	}
	else if(Syst=="TT_MATCH_down"){
		ana.setFilePostfixReplace("ttbar.root","ttbar_ttmdown.root");
		ana.setFilePostfixReplace("ttbarviatau.root","ttbarviatau_ttmdown.root");
	}
	else if(Syst=="TT_SCALE_up"){
		ana.setFilePostfixReplace("ttbar.root","ttbar_ttscaleup.root");
		ana.setFilePostfixReplace("ttbarviatau.root","ttbarviatau_ttscaleup.root");
	}
	else if(Syst=="TT_SCALE_down"){
		ana.setFilePostfixReplace("ttbar.root","ttbar_ttscaledown.root");
		ana.setFilePostfixReplace("ttbarviatau.root","ttbarviatau_ttscaledown.root");
	}
	/////////
	else if(Syst=="TT_SCALE_down"){
		ana.setFilePostfixReplace("ttbar.root","ttbar_ttscaledown.root");
		ana.setFilePostfixReplace("ttbarviatau.root","ttbarviatau_ttscaledown.root");
	}
	////////////
	else if(Syst=="Z_MATCH_up"){
		ana.setFilePostfixReplace("60120.root","60120_Zmup.root");
	}
	else if(Syst=="Z_MATCH_down"){
		ana.setFilePostfixReplace("60120.root","60120_Zmdown.root");
	}
	else if(Syst=="Z_SCALE_up"){
		ana.setFilePostfixReplace("60120.root","60120_Zscaleup.root");
	}
	else if(Syst=="Z_SCALE_down"){
		ana.setFilePostfixReplace("60120.root","60120_Zscaledown.root");
	}
	else if(Syst=="TT_GENPOWPY_down"){
		//nothing
	}
	else if(Syst=="TT_GENPOWPY_up"){
		ana.setFilePostfixReplace("ttbar.root","ttbar_powpy.root");
		ana.setFilePostfixReplace("ttbarviatau.root","ttbarviatau_powpy.root");
	}
	else if(Syst=="TT_GENPOWHERW_down"){
		//nothing
	}
	else if(Syst=="TT_GENPOWHERW_up"){
		ana.setFilePostfixReplace("ttbar.root","ttbar_powherw.root");
		ana.setFilePostfixReplace("ttbarviatau.root","ttbarviatau_powherw.root");
	}
	else if(Syst=="TT_GENMCATNLO_down"){
		//nothing
	}
	else if(Syst=="TT_GENMCATNLO_up"){
		ana.setFilePostfixReplace("ttbar.root","ttbar_mcatnlo.root");
		ana.setFilePostfixReplace("ttbarviatau.root","ttbarviatau_mcatnlo.root");
	}
	else if(Syst=="TT_BJESNUDEC_down"){
		ana.addWeightBranch("NTWeight_bJesweightNuDown");
	}
	else if(Syst=="TT_BJESNUDEC_up"){
		ana.addWeightBranch("NTWeight_bJesweightNuUp");
	}
	else if(Syst=="TT_BJESRETUNE_up"){
		ana.addWeightBranch("NTWeight_bJesweightRetune");
	}
	else if(Syst=="TT_BJESRETUNE_down"){
		//default
	}
	else if(Syst=="TOPMASS_up"){ //consider as systematic variation. for testing purp! leaves normalization fixed
		//default
		ana.setFilePostfixReplace("ttbar.root","ttbar_mt175.5.root");
		ana.setFilePostfixReplace("ttbarviatau.root","ttbarviatau_mt175.5.root");
	}
	else if(Syst=="TOPMASS_down"){
		//default
		ana.setFilePostfixReplace("ttbar.root","ttbar_mt169.5.root");
		ana.setFilePostfixReplace("ttbarviatau.root","ttbarviatau_mt169.5.root");
	}
	else{
		didnothing=true;
	}

	//top mass

	if(topmass != "172.5"){
		std::cout << "replacing top mass value of 172.5 with "<< topmass << std::endl;
		ana.setFilePostfixReplace("ttbar.root","ttbar_mt"+topmass+ ".root");
		ana.setFilePostfixReplace("ttbarviatau.root","ttbarviatau_mt"+topmass+ ".root");

		if(topmass == "178.5" || topmass == "166.5"){
			//	ana.setFilePostfixReplace("_tWtoLL.root","_tWtoLL_mt"+topmass+ ".root");
			//	ana.setFilePostfixReplace("_tbarWtoLL.root","_tbarWtoLL_mt"+topmass+ ".root");
		}

	}

	ana.setFileList(inputfile);



	std::cout << "Calculating btag efficiencies: " << dobtag <<std::endl;



	if(didnothing){
		//create a file outputname_norun that gives a hint that nothing has been done
		//and check in the sleep loop whether outputname_norun exists
		//not needed because merging is done by hand after check, now
		std::cout << "analyse "<< channel <<"_" << energy <<"_" << Syst << " nothing done - maybe some input strings wrong" << std::endl;
	}
	else{
		//if PDF var not found make the same file as above
		//end of configuration

		// recreate file

		TString fulloutfilepath=ana.getOutPath()+".root";
		TString workdir=ana.getOutDir() +"../";
		TString batchdir=workdir+"batch/";

		system(("rm -rf "+fulloutfilepath).Data());

		time_t now = time(0);
		char* dt = ctime(&now);
		cout << "\nStarting analyse at: " << dt << endl;


		int fullsucc=ana.start();

		ztop::container1DStackVector csv;

		if(fullsucc>=0){

			csv.loadFromTFile(fulloutfilepath);
			vector<TString> dycontributions;
			dycontributions << "Z#rightarrowll" << "DY#rightarrowll";
			if(!channel.Contains("emu")){
				rescaleDY(&csv, dycontributions);
				if(testmode)
					std::cout << "drawing plots..." <<std::endl;

				//	std::vector<discriminatorFactory> disc=discriminatorFactory::readAllFromTFile(ana.getOutPath()+".root");
				//	for(size_t i=0;i<disc.size();i++){
				//		disc.at(i).extractLikelihoods(csv);
				//	}
				csv.writeAllToTFile(fulloutfilepath,true,!testmode); //recreates file
				system(("rm -f "+ana.getOutPath()+"_discr.root").Data());
				//	discriminatorFactory::writeAllToTFile(ana.getOutPath()+"_discr.root",disc);


			}
			else{
				std::cout << "drawing plots..." <<std::endl;
				//	std::vector<discriminatorFactory> disc=discriminatorFactory::readAllFromTFile(ana.getOutPath()+".root");
				//	for(size_t i=0;i<disc.size();i++){
				//		disc.at(i).extractLikelihoods(csv);
				//	}
				if(testmode)
					csv.writeAllToTFile(fulloutfilepath,true,!testmode);
				system(("rm -f "+ana.getOutPath()+"_discr.root").Data());
				//	discriminatorFactory::writeAllToTFile(ana.getOutPath()+"_discr.root",disc);
			}


			if(!testmode){
				system(("touch "+batchdir+ana.getOutFileName()+"_fin").Data());
			}
			sleep(1);
			now = time(0);
			cout << "Started  analyse at: " << dt;// << endl;
			dt = ctime(&now);
			cout << "Finished analyse at: " << dt << endl;

			std::cout <<"\nFINISHED with "<< channel <<"_" << energy <<"_" << Syst <<std::endl;


		}
		else{
			std::cout << "at least one job failed!\n"
					<< "error code meaning: \n"
					<< "-99    : std::exception thrown somewhere (see output)\n"
					<< "-88    : a suspiciously high weight was assigned to a MC event\n"
					<< "-3     : problems finding b-tagging efficiencies (file/file format) \n"
					<< "-2     : write to output file failed\n"
					<< "-1     : input file not found\n"
					<< "<-1000 : problems with output file\n"
					<< std::endl;
			//put more once you introduce one
			if(!testmode){
				system(("touch "+batchdir+ana.getOutFileName()+"_failed").Data());
			}
		}
	}



}




//////////////////
#include "TtZAnalysis/Tools/interface/applicationMainMacro.h"

invokeApplication(){
	using namespace ztop;

	TString channel= parser->getOpt<TString>   ("c","emu","channel (ee, emu, mumu), default: emu\n");         //-c channel
	TString syst   = parser->getOpt<TString>   ("s","nominal","systematic variation <var>_<up/down>, default: nominal");     //-s <syst>
	TString energy = parser->getOpt<TString>   ("e","8TeV","energy (8TeV, 7 TeV), default: 8TeV");        //-e default 8TeV
	double lumi    = parser->getOpt<float>     ("l",-1,"luminosity, default -1 (= read from config file)");            //-l default -1
	bool dobtag    = parser->getOpt<bool>      ("B",false,"produce b-tag efficiencies (switch)");         //-B switches on default false
	TString btagfile = parser->getOpt<TString> ("b","all_btags.root","use btagfile (default all_btags.root)");        //-b btagfile default all_btags.root
	const float wpval = parser->getOpt<float>     ("bwp",-100,"btag discriminator cut value (default: not use)");            //-l default -1

	TString outfile= parser->getOpt<TString>   ("o","","additional output id");            //-o <outfile> should be something like channel_energy_syst.root // only for plots
	bool status    = parser->getOpt<bool>      ("S",false,"show regular status update (switch)");         //-S enables default false
	bool testmode  = parser->getOpt<bool>      ("T",false,"enable testmode: 8% of stat, more printout");         //-T enables default false
	bool tickonce  = parser->getOpt<bool>      ("TO",false,"enable tick once: breaks as soon as 1 event survived full selection (for software testing)");         //-T enables default false
	TString mode   = parser->getOpt<TString>   ("m","xsec","additional mode options");        //-m (xsec,....) default xsec changes legends? to some extend
	TString inputfile= parser->getOpt<TString> ("i","","specify configuration file input manually (default is configs/analyse/<channel>_<energy>_config.txt");          //-i empty will use automatic
	TString topmass  = parser->getOpt<TString> ("mt","172.5","top mass value to be used, default: 172.5");          //-i empty will use automatic
	TString discrFile=parser->getOpt<TString>  ("lh","" , "specify discriminator input file. If not specified, likelihoods are created");          //-i empty will use automatic

	const bool interactive = parser->getOpt<bool>      ("I",testmode,"enable interactive mode: no fork limit");

	float fakedatastartentries = parser->getOpt<float>    ("-startdiv",0.9,"point to start fake data entries wrt to to evts");

	bool createLH=false;
	if(discrFile.Length()<1)
		createLH=true;

	parser->doneParsing();

	bool mergefiles=false;
	std::vector<TString> filestomerge;

	if(mergefiles){
		//do the merging with filestomerge
	}
	else{
		analyse(channel, syst, energy, outfile, lumi,dobtag , status, testmode,tickonce,inputfile,mode,topmass,btagfile,createLH,discrFile.Data(),fakedatastartentries,interactive,wpval);
	}
	return 0;
}
