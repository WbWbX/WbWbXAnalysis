#include "TtZAnalysis/Analysis/interface/MainAnalyzer.h"
#include "TtZAnalysis/Analysis/interface/AnalysisUtils.h"
//#include "Analyzer.cc"
//should be taken care of by the linker!


void analyse(TString channel, TString Syst, TString energy, TString outfileadd, double lumi, bool dobtag, bool statbar){ //options like syst..

  bool didnothing=false;

  // ztop::container1D::debug =true;

  //define all options HERE!!!
  //read PU file from filelist.txt? makes sense somehow... do it!!!
  //all files relative to CMSSW_BASE
  //put systematics requiring other samples in a separate subdir in treedir and call it from other inputfiles.txt
  //let samplename for btag be without the whole path (check in btagbase)
  //make btagbase writeable

  TString treedir="/data/user/kiesej/Analysis2012/trees/trees_Mai13_A03/";
  if((TString)getenv("SGE_CELL") != ""){ //on the naf
    treedir="/scratch/hh/dust/naf/cms/user/kieseler/trees_Mai13_A03/";
  }
  else if((TString)getenv("HOST") == "cmsng401"){
    treedir="/data/kiesej/Analysis/trees/trees_Mai13_A03/";
  }
  

  TString jecfile,btagfile,pufile,inputfilewochannel; //...

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
    pufile="Full19.json.txt_PU";
    //here dont specify channel or energy
  }
 

  else if(energy == "7TeV"){ // 7 TeV definitions here including nominal ones
    jecfile="/src/TtZAnalysis/Data/JEC11_V12_AK5PF_UncertaintySources.txt";
    pufile="ReRecoNov2011.json_PU";

  }
   btagfile=channel+"_"+energy+"_btags.root";
 
  //some env variables
  TString cmssw_base=getenv("CMSSW_BASE");
  TString scram_arch=getenv("SCRAM_ARCH");

  using namespace std;
  using namespace ztop;

  ///set input files list etc (common)

  system("mkdir -p output");
  TString outdir="output";  

  MainAnalyzer ana;
  ana.setShowStatusBar(statbar);
  ana.setLumi(lumi);
  ana.setDataSetDirectory(treedir);
  ana.getPUReweighter()->setDataTruePUInput((TString)cmssw_base+"/src/TtZAnalysis/Data/"+pufile+".root");
  ana.setChannel(channel);
  ana.setEnergy(energy);
  if(energy == "8TeV")
    ana.getPUReweighter()->setMCDistrSum12();
  else if(energy == "7TeV")
    ana.getPUReweighter()->setMCDistrFall11();
  ana.setOutFileAdd(outfileadd);
  ana.setOutDir(outdir);
  ana.getBTagSF()->setMakeEff(dobtag);
  ana.setBTagSFFile(btagfile);

  //add indication for non-correlated syst by adding the energy to syst name for the INPUT!!

  if(Syst=="nominal"){
    //all already defined
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
  else if(Syst=="Z_MATCH_up"){
    ana.setFilePostfixReplace("60120.root","60120_Zmup.root");
  }
  else if(Syst=="Z_MATCH_down"){
    ana.setFilePostfixReplace("60120.root","60120_Zmudown.root");
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
  double lumi=getLumi(argc, argv);        //-l default 19100
  bool dobtag=prepareBTag(argc, argv);    //-b switches on default false
  TString outfile=getOutFile(argc, argv);  //-o <outfile> should be something like channel_energy_syst.root // only for plots
  bool statbar=false;

  bool mergefiles=false; //get these things from the options to..
  std::vector<TString> filestomerge;

  if(mergefiles){
    //do the merging with filestomerge
  }
  else{
    analyse(channel, syst, energy, outfile, lumi,dobtag , statbar);
  }
}
