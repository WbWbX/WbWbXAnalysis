#include "TtZAnalysis/Analysis/interface/MainAnalyzer.h"
#include "TtZAnalysis/Analysis/interface/AnalysisUtils.h"
//#include "Analyzer.cc"
//should be taken care of by the linker!


void analyse(TString channel, TString Syst, TString energy, TString outfileadd, double lumi, bool dobtag, bool status,bool testmode=false){ //options like syst..

	bool didnothing=false;
	//some env variables
	TString cmssw_base=getenv("CMSSW_BASE");
	TString scram_arch=getenv("SCRAM_ARCH");

	using namespace std;
	using namespace ztop;
	// ztop::container1D::debug =true;

	//define all options HERE!!!
	//read PU file from filelist.txt? makes sense somehow... do it!!!
	//all files relative to CMSSW_BASE
	//put systematics requiring other samples in a separate subdir in treedir and call it from other inputfiles.txt
	//let samplename for btag be without the whole path (check in btagbase)
	//make btagbase writeable

	TString treedir="/data/user/kiesej/Analysis2012/trees/trees_Jun13_03/";
	if((TString)getenv("SGE_CELL") != ""){ //on the naf
		treedir="/scratch/hh/dust/naf/cms/user/kieseler/trees_Jun13_03/";
	}
	else if((TString)getenv("HOST") == "cmsng401"){
		treedir="/data/kiesej/Analysis/trees/trees_Jun13_03/";
	}


	TString jecfile,btagfile,pufile,inputfilewochannel,muonsffile,muonsfhisto,elecsffile,elecsfhisto,trigsffile,trigsfhisto; //...

	inputfilewochannel="inputfiles.txt";
	// 7 TeV stuff needs to be implemented
	// Syst variations
	// define all nominal here

	// bug fixes with jets?!? the index stuff?

	// all relative to cmssw_base except for inputfiles (will be copied)
	if(energy == "8TeV"){
		jecfile="/src/TtZAnalysis/Data/Summer12_V2_DATA_AK5PF_UncertaintySources.txt";
		//btagfile="/src/TtZAnalysis/Data";
		// pufile="/src/TtZAnalysis/Data/Full19.json.txt_PU.root";
		pufile="ReRecoJan13.json.txt_PU";
		muonsffile="/src/TopAnalysis/Configuration/analysis/diLeptonic/data/MuonSFtop12028_19fb.root";
		muonsfhisto="MuonSFtop12028";
		elecsffile="/src/TopAnalysis/Configuration/analysis/diLeptonic/data/ElectronSFtop12028_19fb.root";
		elecsfhisto="ElectronSFtop12028";
		trigsffile="/src/TopAnalysis/Configuration/analysis/diLeptonic/data/triggerSummary_"+channel+"_19fb.root";
		trigsfhisto="scalefactor eta2d with syst";
	}


	else if(energy == "7TeV"){ // 7 TeV definitions here including nominal ones
		jecfile="/src/TtZAnalysis/Data/JEC11_V12_AK5PF_UncertaintySources.txt";
		pufile="ReRecoNov2011.json_PU";

	}
	btagfile="all_btags.root";

	if(dobtag)
		btagfile=channel+"_"+energy+"_"+Syst+"_btags.root";




	///set input files list etc (common)

	system("mkdir -p output");
	TString outdir="output";

	MainAnalyzer ana;
	ana.setShowStatus(status);
	ana.setTestMode(testmode);
	ana.setLumi(lumi);
	ana.setDataSetDirectory(treedir);
	ana.getPUReweighter()->setDataTruePUInput((TString)cmssw_base+"/src/TtZAnalysis/Data/"+pufile+".root");
	ana.setChannel(channel);
	ana.setEnergy(energy);
	if(energy == "8TeV"){
		ana.getPUReweighter()->setMCDistrSum12();
	}
	else if(energy == "7TeV"){
		ana.getPUReweighter()->setMCDistrFall11();
	}
	ana.getElecSF()->setInput(cmssw_base+elecsffile,elecsfhisto);
	ana.getMuonSF()->setInput(cmssw_base+muonsffile,muonsfhisto);
	ana.getTriggerSF()->setInput(cmssw_base+trigsffile,trigsfhisto);
	ana.setOutFileAdd(outfileadd);
	ana.setOutDir(outdir);
	ana.getBTagSF()->setMakeEff(dobtag);
	ana.setBTagSFFile(btagfile);
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
		ana.getPUReweighter()->setDataTruePUInput((TString)cmssw_base+"/src/TtZAnalysis/Data/"+pufile+"_up.root");
	}
	else if(Syst=="PU_down"){
		ana.getPUReweighter()->setDataTruePUInput((TString)cmssw_base+"/src/TtZAnalysis/Data/"+pufile+"_down.root");
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
	//......
	else{
		didnothing=true;
	}


	TString inputfile=channel+"_"+energy+"_"+inputfilewochannel;
	// TString
	ana.setFileList(inputfile);
	ana.setSyst(Syst);


	std::cout << "doing btag: " << dobtag <<std::endl;


	if(didnothing){
		//create a file outputname_norun that gives a hint that nothing has been done
		//and check in the sleep loop whether outputname_norun exists
		//not needed because merging is done by hand after check, now
		std::cout << "analyse.exe: "<< channel <<"_" << energy <<"_" << Syst << " nothing done - maybe some input strings wrong" << std::endl;
	}
	else{
		//if PDF var not found make the same file as above
		//end of configuration

		// recreate file
		system(("rm -rf "+ana.getOutPath()+".root").Data());

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
			csv.writeAllToTFile(ana.getOutPath()+"_plots.root",true);
			sleep(1);
			std::cout <<"\nFINISHED with "<< channel <<"_" << energy <<"_" << Syst <<std::endl;
			delete f;

		}
		else{
			std::cout << "at least one job failed!" << std::endl;
			system(("touch "+ana.getOutPath()+"_failed").Data());
		}
	}



}




//////////////////


int main(int argc, char* argv[]){


	TString channel = getChannel(argc, argv);//-c <chan>
	TString syst = getSyst(argc, argv);      //-s <syst>
	TString energy = getEnergy(argc, argv); //-e default 8TeV
	double lumi=getLumi(argc, argv);        //-l default 19789
	bool dobtag=prepareBTag(argc, argv);    //-b switches on default false
	TString outfile=getOutFile(argc, argv);  //-o <outfile> should be something like channel_energy_syst.root // only for plots
	bool status=getShowStatus(argc, argv);    //-S enables default false
	bool testmode=getTestMode(argc, argv); 	//-T enables default false

	bool mergefiles=false; //get these things from the options to..
	std::vector<TString> filestomerge;

	if(mergefiles){
		//do the merging with filestomerge
	}
	else{
		analyse(channel, syst, energy, outfile, lumi,dobtag , status, testmode);
	}
}
