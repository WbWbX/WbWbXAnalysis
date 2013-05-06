#include "Analyzer.h"
#include "Analyzer.cc"



void analyze(TString channel, TString coupleofoptions){ //options like syst..
  using namespace std;
  using namespace ztop;

 

  //bool sed_doBTag


  //creates dir
  //saves output to dir, when finished: set some file

  //dir structure: /output/<channel>_<syst>_output.root

  //in the last merge step maybe check via ls? to include variations
  //and only consider relative systematics (makes possible to "add" syst easily or just run one set


  cout << "\n\n\n" <<endl; //looks better ;)

  TString outfile = option+"analysis_output.root";
  // TString pufolder="/afs/naf.desy.de/user/k/kieseler/scratch/2012/TestArea2/CMSSW_5_2_5/src/TtZAnalysis/Data/PUDistr/";
  // reorder the whole stuff in a way. specify ee mumu 7TeV 8TeV and then do the systematics "in parallel" to better keep track. maybe even produce a vector of analyzers as done for getCrossSections.C


  string cmssw_base=getenv("CMSSW_BASE");
  TString scram_arch=getenv("SCRAM_ARCH");
  //initialize mumu

  double lumi8TeV=12200;
  double lumi8TeVunc=0.036; //?!
  double lumi7TeV=5100;
  double lumi7TeVunc=0.025; //?!

  bool runInNotQuietMode=true;

  TString treedir="/scratch/hh/dust/naf/cms/user/kieseler";
  TString treedir7="/scratch/hh/dust/naf/cms/user/diezcar/trees_jan";
  if( scram_arch.Contains("osx")){
    cout << "running locally" << endl;
    treedir="/Users/kiesej/CMS_data_nobk";
  }
  else if ((TString)getenv("SGE_CELL") == ""){
    cout <<"running on WGS" << endl;
    treedir = "/data/user/kiesej/Analysis2012/trees";
  }



  bool onlytest=false;


  //prepare defaults and get btag SF
  
  MainAnalyzer mumu_8TeV("8TeV_nobtag_mumu","mumu");
  mumu_8TeV.setShowStatusBar(runInNotQuietMode);  //for running with text output mode
  mumu_8TeV.setLumi(lumi8TeV);
  mumu_8TeV.setFileList("mumu_8TeV_inputfiles.txt");
  mumu_8TeV.setDataSetDirectory(treedir+"/trees_8TeV_newJEC/");
  mumu_8TeV.getPUReweighter()->setDataTruePUInput((TString)cmssw_base+"/src/TtZAnalysis/Data/HCP.json.txt_PU.root");
  mumu_8TeV.getJECUncertainties()->setFile(cmssw_base+"/src/TtZAnalysis/Data/Fall12_V7_DATA_UncertaintySources_AK5PF.txt");
  mumu_8TeV.getPUReweighter()->setMCDistrSum12();
  mumu_8TeV.getBTagSF()->setMakeEff(true);
  if(!onlytest) mumu_8TeV.start();
  TFile * btagfile = new TFile("mumu_8TeV_btags.root","RECREATE");
  // mumu_8TeV.getBTagSF()->writeToTFile(btagfile);
  btagfile->Close();
  delete btagfile;
  mumu_8TeV.getPlots()->writeAllToTFile(outfile,true);
  mumu_8TeV.getBTagSF()->setMakeEff(false);
  mumu_8TeV.setName("8TeV_default_mumu","mumu");
   if(!onlytest) mumu_8TeV.start();
  // rescaleDY(*mumu_8TeV.getPlots(),dycontributions);
  mumu_8TeV.getPlots()->writeAllToTFile(outfile,false);
 
  

  MainAnalyzer ee_8TeV=mumu_8TeV;
  ee_8TeV.setFileList("ee_8TeV_inputfiles.txt");
  ee_8TeV.setName("8TeV_nobtag_ee","ee");
  ee_8TeV.getBTagSF()->setMakeEff(true);
  if(!onlytest) ee_8TeV.start();
  btagfile = new TFile("ee_8TeV_btags.root","RECREATE");
  ee_8TeV.getBTagSF()->writeToTFile(btagfile);
  btagfile->Close();
  delete btagfile;
  ee_8TeV.getPlots()->writeAllToTFile(outfile,false);
  ee_8TeV.getBTagSF()->setMakeEff(false);
  ee_8TeV.setName("8TeV_default_ee","ee");
  if(!onlytest) ee_8TeV.start();
  // rescaleDY(*ee_8TeV.getPlots(),dycontributions);
  ee_8TeV.getPlots()->writeAllToTFile(outfile,false);
  
 
  //to have something to play with prepare the nobtag 7 TeV analyzer
  MainAnalyzer ee_7TeV;
  ee_7TeV.setFileList("ee_7TeV_inputfiles.txt");
  ee_7TeV.setLumi(lumi7TeV);
  ee_7TeV.setShowStatusBar(runInNotQuietMode);
  ee_7TeV.setName("7TeV_nobtag_ee","ee");
  ee_7TeV.setDataSetDirectory(treedir7+"/trees_7TeV/");
  ee_7TeV.getPUReweighter()->setDataTruePUInput((TString)cmssw_base+"/src/TtZAnalysis/Data/ReRecoNov2011.json_PU.root");
  ee_7TeV.getPUReweighter()->setMCDistrFall11();
  ee_7TeV.getJECUncertainties()->setIs2012(false);
  ee_7TeV.getJECUncertainties()->setFile(cmssw_base+"/src/TtZAnalysis/Data/JEC11_V12_AK5PF_UncertaintySources.txt");
  ee_7TeV.getBTagSF()->setMakeEff(true);
  ee_7TeV.getBTagSF()->setIs2011(true);
  if(!onlytest) ee_7TeV.start();
  btagfile = new TFile("ee_7TeV_btags.root","RECREATE");
  //  ee_7TeV.getBTagSF()->writeToTFile(btagfile);
  btagfile->Close();
  delete btagfile;
  ee_7TeV.getPlots()->writeAllToTFile(outfile,false);
  ee_7TeV.getBTagSF()->setMakeEff(false);
  ee_7TeV.setName("7TeV_default_ee","ee");
  if(!onlytest) ee_7TeV.start();
  // rescaleDY(*ee_7TeV.getPlots(),dycontributions);
  ee_7TeV.getPlots()->writeAllToTFile(outfile,false);
  
  MainAnalyzer mumu_7TeV=ee_7TeV;
  mumu_7TeV.setFileList("mumu_7TeV_inputfiles.txt");
  mumu_7TeV.setName("7TeV_nobtag_mumu","mumu");
  mumu_7TeV.getBTagSF()->setMakeEff(true);
  if(!onlytest) mumu_7TeV.start();
  btagfile = new TFile("all_btags.root","RECREATE");
  //  mumu_7TeV.getBTagSF()->writeToTFile(btagfile);
  btagfile->Close();
  delete btagfile;
  mumu_7TeV.getPlots()->writeAllToTFile(outfile,false);
  mumu_7TeV.getBTagSF()->setMakeEff(false);
  mumu_7TeV.setName("7TeV_default_mumu","mumu");
  if(!onlytest) mumu_7TeV.start();
  // rescaleDY(*mumu_7TeV.getPlots(),dycontributions);
  mumu_7TeV.getPlots()->writeAllToTFile(outfile,false);

  
  ///////////////////////////////////////

  vector<MainAnalyzer> full_ana, full_errana;
  full_ana << ee_7TeV << mumu_7TeV << ee_8TeV << mumu_8TeV;
  full_errana << ee_7TeV << mumu_7TeV << ee_8TeV << mumu_8TeV;

  cout << "doing systematics" << endl;

  for(unsigned int i=0; i<full_ana.size(); i++){


    TString energystring="8TeV";
    if(full_ana.at(i).getName().Contains("7TeV")) energystring="7TeV";

    
    //b systematics (correlated)
    cout << "doing b-tag systematics" << endl;

    MainAnalyzer temp=full_ana.at(i);
    
    temp.getBTagSF()->setSystematic("light up");
    temp.replaceInName("default","btag_light_up");
    if(!onlytest) temp.start();
    temp.getPlots()->writeAllToTFile(outfile,false);
    full_errana.at(i).getPlots()->addMCErrorStackVector("btag_light_up",*temp.getPlots());

    temp=full_ana.at(i);
    temp.getBTagSF()->setSystematic("light down");
    temp.replaceInName("default","btag_light_down");
    if(!onlytest) temp.start();
    temp.getPlots()->writeAllToTFile(outfile,false);
    full_errana.at(i).getPlots()->addMCErrorStackVector("btag_light_down",*temp.getPlots());

    temp=full_ana.at(i);
    temp.getBTagSF()->setSystematic("heavy up");
    temp.replaceInName("default","btag_heavy_up");
    if(!onlytest) temp.start();
    temp.getPlots()->writeAllToTFile(outfile,false);
    full_errana.at(i).getPlots()->addMCErrorStackVector("btag_heavy_up",*temp.getPlots());

    temp=full_ana.at(i);
    temp.getBTagSF()->setSystematic("heavy down");
    temp.replaceInName("default","btag_heavy_down");
    if(!onlytest) temp.start();
    temp.getPlots()->writeAllToTFile(outfile,false);
    full_errana.at(i).getPlots()->addMCErrorStackVector("btag_heavy_down",*temp.getPlots());


    // JER correlated
    cout << "doing JER systematics" << endl;

    temp=full_ana.at(i);
    temp.replaceInName("default","JER_up");
    temp.getJERAdjuster()->setSystematics("up");
    if(!onlytest) temp.start();
    temp.getPlots()->writeAllToTFile(outfile,false);
    full_errana.at(i).getPlots()->addMCErrorStackVector("JER_up",*temp.getPlots());

    temp=full_ana.at(i);
    temp.replaceInName("default","JER_down");
    temp.getJERAdjuster()->setSystematics("down");
    if(!onlytest) temp.start();
    temp.getPlots()->writeAllToTFile(outfile,false);
    full_errana.at(i).getPlots()->addMCErrorStackVector("JER_down",*temp.getPlots());


    

    // JES correlated
    cout << "doing JES systematics" << endl;

    std::vector<unsigned int> JECsources_corr,JECsources_uncorr;
    if(energystring == "7TeV"){
      JECsources_corr << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 12 << 13 << 15;
      JECsources_uncorr << 0 << 11 << 14;
    }
    else{
      JECsources_corr << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12 << 13;
      JECsources_uncorr << 0 << 14 << 15 << 16 << 17 << 18;
    }
    
    temp=full_ana.at(i);
    temp.replaceInName("default","JES_up");
    temp.getJECUncertainties()->sources() = JECsources_corr;
    temp.getJECUncertainties()->setSystematics("up");
    if(!onlytest) temp.start();
    temp.getPlots()->writeAllToTFile(outfile,false);
    full_errana.at(i).getPlots()->addMCErrorStackVector("JES_corr_up",*temp.getPlots());

    temp=full_ana.at(i);
    temp.replaceInName("default","JES_down");
    temp.getJECUncertainties()->sources() = JECsources_corr;
    temp.getJECUncertainties()->setSystematics("down");
    if(!onlytest) temp.start();
    temp.getPlots()->writeAllToTFile(outfile,false);
    full_errana.at(i).getPlots()->addMCErrorStackVector("JES_corr_down",*temp.getPlots());

    //JES uncorrelated

    temp=full_ana.at(i);
    temp.replaceInName("default","JES_"+energystring+"_up");
    temp.getJECUncertainties()->sources() = JECsources_uncorr;
    temp.getJECUncertainties()->setSystematics("up");
    if(!onlytest) temp.start();
    temp.getPlots()->writeAllToTFile(outfile,false);
    full_errana.at(i).getPlots()->addMCErrorStackVector("JES_"+energystring+"_up",*temp.getPlots());

    temp=full_ana.at(i);
    temp.replaceInName("default","JES_"+energystring+"_down");
    temp.getJECUncertainties()->sources() = JECsources_uncorr;
    temp.getJECUncertainties()->setSystematics("down");
    if(!onlytest) temp.start();
    temp.getPlots()->writeAllToTFile(outfile,false);
    full_errana.at(i).getPlots()->addMCErrorStackVector("JES_"+energystring+"_down",*temp.getPlots());

    //PileUp
    cout << "doing PU systematics" << endl;

    //7 TeV
    TString pufile;

    pufile=cmssw_base+"/src/TtZAnalysis/Data/"+"HCP.json.txt_PU_up.root";
    if(energystring=="7TeV")
      pufile=cmssw_base+"/src/TtZAnalysis/Data/"+"ReRecoNov2011.json_PU_up.root";

    temp=full_ana.at(i);
    temp.replaceInName("default","PU_"+energystring+"_up");
    temp.getPUReweighter()->setDataTruePUInput(pufile);
    if(!onlytest) if(!onlytest) temp.start();
    temp.getPlots()->writeAllToTFile(outfile,false);
    full_errana.at(i).getPlots()->addMCErrorStackVector("PU_"+energystring+"_up",*temp.getPlots());
    
    pufile=cmssw_base+"/src/TtZAnalysis/Data/"+"HCP.json.txt_PU_down.root";
    if(energystring=="7TeV")
      pufile=cmssw_base+"/src/TtZAnalysis/Data/"+"ReRecoNov2011.json_PU_down.root";

    temp=full_ana.at(i);
    temp.replaceInName("default","PU_"+energystring+"_down");
    temp.getPUReweighter()->setDataTruePUInput(pufile);
    if(!onlytest) temp.start();
    temp.getPlots()->writeAllToTFile(outfile,false);
    full_errana.at(i).getPlots()->addMCErrorStackVector("PU_"+energystring+"_down",*temp.getPlots());

    //matching etc. to be yet done

    //treedir + "/trees_"+energystring+"/"  btagging has to be set properly
    /*
      temp=full_ana.at(i);
      temp.replaceInName("default","ttmatch_up");
      if(!onlytest) temp.start("ttmup");
      temp.getPlots()->writeAllToTFile(outfile,false);
      full_errana.at(i).getPlots()->addMCErrorStackVector("ttmatch_up",*temp.getPlots());

      temp=full_ana.at(i);
      temp.replaceInName("default","ttmatch_down");
      if(!onlytest) temp.start("ttmdown");
      temp.getPlots()->writeAllToTFile(outfile,false);
      full_errana.at(i).getPlots()->addMCErrorStackVector("ttmatch_down",*temp.getPlots());

      temp=full_ana.at(i);
      temp.replaceInName("default","Zmatch_up");
      if(!onlytest) temp.start("Zmup");
      temp.getPlots()->writeAllToTFile(outfile,false);
      full_errana.at(i).getPlots()->addMCErrorStackVector("Zmatch_up",*temp.getPlots());

      temp=full_ana.at(i);
      temp.replaceInName("default","Zmatch_down");
      if(!onlytest) temp.start("Zmdown");
      temp.getPlots()->writeAllToTFile(outfile,false);
      full_errana.at(i).getPlots()->addMCErrorStackVector("Zmatch_down",*temp.getPlots());
    */
  
    /*
      temp=full_ana.at(i);
      temp.replaceInName("default","ttscale_up");
      if(!onlytest) temp.start("ttsup");
      temp.getPlots()->writeAllToTFile(outfile,false);
      full_errana.at(i).getPlots()->addMCErrorStackVector("ttscale_up",*temp.getPlots());

      temp=full_ana.at(i);
      temp.replaceInName("default","ttscale_down");
      if(!onlytest) temp.start("ttsdown");
      temp.getPlots()->writeAllToTFile(outfile,false);
      full_errana.at(i).getPlots()->addMCErrorStackVector("ttscale_down",*temp.getPlots());

      temp=full_ana.at(i);
      temp.replaceInName("default","Zscale_up");
      if(!onlytest) temp.start("Zsup");
      temp.getPlots()->writeAllToTFile(outfile,false);
      full_errana.at(i).getPlots()->addMCErrorStackVector("Zscale_up",*temp.getPlots());

      temp=full_ana.at(i);
      temp.replaceInName("default","Zscale_down");
      if(!onlytest) temp.start("Zsdown");
      temp.getPlots()->writeAllToTFile(outfile,false);
      full_errana.at(i).getPlots()->addMCErrorStackVector("Zscale_down",*temp.getPlots());
    */
  

    cout << "writing full error output" << endl;
    full_errana.at(i).replaceInName("default","allErrors");
    full_errana.at(i).getPlots()->writeAllToTFile(outfile,false);

    cout << "\n\n\n\n\n******************** \ndone with " << ((double)(1+i))/((double)full_ana.size())
	 << " of the systematic variations\n\n\n\n\n******************" << endl;


  }



  std::cout <<"\n\n\n\n\nFINISHED!" <<std::endl;

  
}
