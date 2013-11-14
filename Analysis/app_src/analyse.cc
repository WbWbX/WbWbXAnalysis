#include "TtZAnalysis/Analysis/interface/MainAnalyzer.h"
#include "TtZAnalysis/Analysis/interface/AnalysisUtils.h"
#include "TtZAnalysis/Tools/interface/fileReader.h"
//#include "Analyzer.cc"
//should be taken care of by the linker!


void analyse(TString channel, TString Syst, TString energy, TString outfileadd, double lumi, bool dobtag, bool status,bool testmode,TString maninputfile){ //options like syst..

	bool didnothing=false;
	//some env variables
	TString cmssw_base=getenv("CMSSW_BASE");
	TString scram_arch=getenv("SCRAM_ARCH");

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

	fileReader fr;
	fr.setComment("$");
	fr.setDelimiter(",");
	fr.setStartMarker("[parameters-begin]");
	fr.setEndMarker("[parameters-end]");
	fr.readFile(inputfile.Data());


	TString treedir,jecfile,pufile,muonsffile,muonsfhisto,elecsffile,elecsfhisto,trigsffile,elecensffile,muonensffile; //...


	if(lumi<0)
		lumi=fr.getValue<double>("Lumi");
	treedir=              fr.getValue<TString>("inputFilesDir");
	jecfile=   cmssw_base+fr.getValue<TString>("JECFile");
	muonsffile=cmssw_base+fr.getValue<TString>("MuonSFFile");
	muonsfhisto=          fr.getValue<TString>("MuonSFHisto");
	elecsffile=cmssw_base+fr.getValue<TString>("ElecSFFile");
	elecsfhisto=          fr.getValue<TString>("ElecSFHisto");
	trigsffile=cmssw_base+fr.getValue<TString>("TriggerSFFile");
	pufile=    cmssw_base+fr.getValue<TString>("PUFile");
	//elecensffile =cmssw_base+fr.getValue<TString>("ElecEnergySFFile");
	//muonensffile =cmssw_base+fr.getValue<TString>("MuonEnergySFFile");


	std::cout << "inputfile read" << std::endl;

	//hard coded:

	TString trigsfhisto="scalefactor eta2d with syst";
	TString btagfile="all_btags.root";
	if(dobtag)
		btagfile=channel+"_"+energy+"_"+Syst+"_btags.root";

	///set input files list etc (common)

	system("mkdir -p output");
	TString outdir="output";

	MainAnalyzer ana;
	ana.setMaxChilds(6);
	ana.setShowStatus(status);
	ana.setTestMode(testmode);
	ana.setLumi(lumi);
	ana.setDataSetDirectory(treedir);
	ana.getPUReweighter()->setDataTruePUInput(pufile+".root");
	ana.setChannel(channel);
	ana.setEnergy(energy);
	if(energy == "8TeV"){
		ana.getPUReweighter()->setMCDistrSum12();
	}
	else if(energy == "7TeV"){
		ana.getPUReweighter()->setMCDistrFall11();
	}
	ana.getElecSF()->setInput(elecsffile,elecsfhisto);
	ana.getMuonSF()->setInput(muonsffile,muonsfhisto);
	ana.getTriggerSF()->setInput(trigsffile,trigsfhisto);

	ana.getElecEnergySF()->setGlobal(1,0.5,0.5);
	ana.getMuonEnergySF()->setGlobal(1,0.5,0.5);

	ana.setOutFileAdd(outfileadd);
	ana.setOutDir(outdir);
	ana.getBTagSF()->setMakeEff(dobtag);
	ana.setBTagSFFile(btagfile);
	ana.getJECUncertainties()->setFile((jecfile).Data());
	ana.getJECUncertainties()->setSystematics("no");
	if(testmode){
		ana.getBTagSF()->setMakeEff(true);
		ana.setBTagSFFile(btagfile+"TESTMODE");
	}
	//add indication for non-correlated syst by adding the energy to syst name!! then the getCrossSections stuff should recognise it

	//although it produces overhead, add background rescaling here?

	if(Syst=="nominal"){
		//all already defined
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
	}
	else if(Syst=="JES_down"){
		ana.getJECUncertainties()->setSystematics("down");
	}
	/*
  else if(Syst=="7TeV_JES_up"){
    ana.getJECUncertainties()->setSystematics("up");
  }
  else if(Syst=="7TeV_JES_down"){
    ana.getJECUncertainties()->setSystematics("down");
  }
	 */ // etc

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
	else if(Syst=="BTAGH_up"){
		ana.getBTagSF()->setSystematic("heavy up");
	}
	else if(Syst=="BTAGH_down"){
		ana.getBTagSF()->setSystematic("heavy down");
	}
	else if(Syst=="BTAGL_up"){
		ana.getBTagSF()->setSystematic("light up");
	}
	else if(Syst=="BTAGL_down"){
		ana.getBTagSF()->setSystematic("light down");
	}
	else if(Syst=="TT_MATCH_up"){
		ana.setFilePostfixReplace("ttbar_ext.root","ttbar_ttmup.root");
		ana.setFilePostfixReplace("ttbarviatau_ext.root","ttbarviatau_ttmup.root");
	}
	else if(Syst=="TT_MATCH_down"){
		ana.setFilePostfixReplace("ttbar_ext.root","ttbar_ttmdown.root");
		ana.setFilePostfixReplace("ttbarviatau_ext.root","ttbarviatau_ttmdown.root");
	}
	else if(Syst=="TT_SCALE_up"){
		ana.setFilePostfixReplace("ttbar_ext.root","ttbar_ttscaleup.root");
		ana.setFilePostfixReplace("ttbarviatau_ext.root","ttbarviatau_ttscaleup.root");
	}
	else if(Syst=="TT_SCALE_down"){
		ana.setFilePostfixReplace("ttbar_ext.root","ttbar_ttscaledown.root");
		ana.setFilePostfixReplace("ttbarviatau_ext.root","ttbarviatau_ttscaledown.root");
	}
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
	else if(Syst=="MT_6_down"){
		ana.setFilePostfixReplace("ttbar_ext.root","ttbar_mt166.5.root");
		ana.setFilePostfixReplace("ttbarviatau_ext.root","ttbarviatau_mt166.5.root");
	}
	else if(Syst=="MT_3_down"){
		ana.setFilePostfixReplace("ttbar_ext.root","ttbar_mt169.5.root");
		ana.setFilePostfixReplace("ttbarviatau_ext.root","ttbarviatau_mt169.5.root");
	}
	else if(Syst=="MT_3_up"){
		ana.setFilePostfixReplace("ttbar_ext.root","ttbar_mt175.5.root");
		ana.setFilePostfixReplace("ttbarviatau_ext.root","ttbarviatau_mt175.5.root");
	}
	else if(Syst=="MT_6_up"){
		ana.setFilePostfixReplace("ttbar_ext.root","ttbar_mt178.5.root");
		ana.setFilePostfixReplace("ttbarviatau_ext.root","ttbarviatau_mt178.5.root");
	}

	//......
	else{
		didnothing=true;
	}



	ana.setFileList(inputfile);
	ana.setSyst(Syst);


	std::cout << "doing btag: " << dobtag <<std::endl;


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

		int fullsucc=ana.start();

		ztop::container1DStackVector csv;

		if(fullsucc>=0){
			TFile * f = new TFile(ana.getOutPath()+".root","read");
			if(f->Get("stored_objects")){
				csv.loadFromTree((TTree*)f->Get("stored_objects"),ana.getPlots()->getName());
			}
			vector<TString> dycontributions;
			dycontributions << "Z#rightarrowll" << "DY#rightarrowll";
			if(!channel.Contains("emu"))
				rescaleDY(&csv, dycontributions);
			//csv.writeAllToTFile(ana.getOutPath()+"_plots.root",true);
			if(!testmode){
				system(("touch "+batchdir+ana.getOutFileName()+"_fin").Data());
			}
			sleep(1);
			std::cout <<"\nFINISHED with "<< channel <<"_" << energy <<"_" << Syst <<std::endl;
			delete f;

		}
		else{
			std::cout << "at least one job failed!" << std::endl;
			if(!testmode){
				system(("touch "+batchdir+ana.getOutFileName()+"_failed").Data());
			}
		}
	}



}




//////////////////


int main(int argc, char* argv[]){


	TString channel = getChannel(argc, argv);//-c <chan>
	TString syst = getSyst(argc, argv);      //-s <syst>
	TString energy = getEnergy(argc, argv); //-e default 8TeV
	double lumi=getLumi(argc, argv);        //-l default -1
	bool dobtag=prepareBTag(argc, argv);    //-b switches on default false
	TString outfile=getOutFile(argc, argv);  //-o <outfile> should be something like channel_energy_syst.root // only for plots
	bool status=getShowStatus(argc, argv);    //-S enables default false
	bool testmode=getTestMode(argc, argv); 	//-T enables default false
	TString mode=getMode(argc,argv);     //-m (xsec,....) default xsec changes legends to some extend
	bool mergefiles=false; //get these things from the options to..
	TString inputfile=getInputfile(argc,argv);

	std::vector<TString> filestomerge;

	if(mergefiles){
		//do the merging with filestomerge
	}
	else{
		analyse(channel, syst, energy, outfile, lumi,dobtag , status, testmode,inputfile );
	}
}
