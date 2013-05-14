#include "TtZAnalysis/Analysis/interface/MainAnalyzer.h"
#include "TtZAnalysis/Analysis/interface/AnalysisUtils.h"
//#include "Analyzer.cc"
//should be taken care of by the linker!


void analyse(TString channel, TString Syst, TString energy, TString outfile, double lumi, bool dobtag, bool statbar){ //options like syst..

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
  

  TString jecfile,btagfile,pufile,inputfilewochannel; //...


  // 7 TeV stuff needs to be implemented
  // Syst variations
  // define all nominal here

  // bug fixes with jets?!? the index stuff?

  // all relative to cmssw_base except for inputfiles (will be copied)

  jecfile="/src/TtZAnalysis/Data/Summer12_V2_DATA_AK5PF_UncertaintySources.txt";
  btagfile="/src/TtZAnalysis/Data";
  pufile="/src/TtZAnalysis/Data/Full19.json.txt_PU.root";
  inputfilewochannel="inputfiles.txt"; //here dont specify channel or energy

  btagfile="btags.root";

  if(Syst=="nominal"){
    //all already defined
  }
  else if(Syst=="JES_up"){

  }
  else if(Syst=="JES_down"){

  }
  else if(Syst=="JER_up"){

  }
  else if(Syst=="JER_down"){

  }
  else if(Syst=="PU_up"){

  }
  else if(Syst=="PU_down"){

  }
  else if(Syst=="MATCH_up"){
    inputfilewochannel="inputfiles_matchup.txt";
  }
  else if(Syst=="MATCH_down"){

  }
  else if(Syst=="SCALE_up"){

  }
  else if(Syst=="SCALE_down"){

  }
  //......
  else{
    didnothing=true;
  }
  
  TString inputfile=channel+"_"+energy+"_"+inputfilewochannel;
  // TString 


  std::cout << "doing btag: " << dobtag <<std::endl;

  //some env variables
  TString cmssw_base=getenv("CMSSW_BASE");
  TString scram_arch=getenv("SCRAM_ARCH");

  using namespace std;
  using namespace ztop;

  ///set input files list etc (common)

  system("mkdir -p output");
 
  MainAnalyzer ana;
  ana.setShowStatusBar(statbar);
  ana.setLumi(lumi);
  ana.setFileList(inputfile);
  ana.setDataSetDirectory(treedir);
  ana.getPUReweighter()->setDataTruePUInput((TString)cmssw_base+pufile);
  ana.setChannel(channel);
  ana.setSyst(Syst);
  ana.setEnergy(energy);
  ana.getPUReweighter()->setMCDistrSum12();
 
  //ana.getBTagSF()->setMakeEff(dobtag);
  if(!dobtag)
    ana.getBTagSF()->readFromTFile(btagfile);

  //btag?? no idea...

  if(didnothing){
    //create a file outputname_norun that gives a hint that nothing has been done
    //and check in the sleep loop whether outputname_norun exists
  }
  else{
    //if PDF var not found make the same file as above
    ana.start();
      ana.getPlots()->writeAllToTFile("output/"+outfile+".root",true);

    if(dobtag)
      ana.getBTagSF()->writeToTFile(btagfile);
    
  }

  
  std::cout <<"\nFINISHED with "<< channel <<"_" << energy <<"_" << Syst <<std::endl;
  if(didnothing) std::cout << "...but nothing done - maybe some input strings wrong" << std::endl;
  
}




//////////////////


int main(int argc, char* argv[]){

 
  TString channel = getChannel(argc, argv);//-c <chan>
  TString syst = getSyst(argc, argv);      //-s <syst>
  TString energy = getEnergy(argc, argv); //-e default 8TeV
  double lumi=getLumi(argc, argv);        //-l default 19100
  bool dobtag=prepareBTag(argc, argv);    //-b switches on default false
  TString outfile=getOutFile(argc, argv);  //-o <outfile> should be something like channel_energy_syst.root
  bool statbar=true;

  bool mergefiles=false; //get these things from the options to
  std::vector<TString> filestomerge;

  if(mergefiles){
    //do the merging with filestomerge
  }
  else{
    analyse(channel, syst, energy, outfile, lumi,dobtag , statbar);
  }
}
