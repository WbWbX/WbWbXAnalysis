#include "triggerAnalyzer_base2.h"



double 
triggerAnalyzer::selectDileptons(std::vector<ztop::NTMuon> * inputMuons, std::vector<ztop::NTElectron> * inputElectrons){

  using namespace std;
  using namespace ztop;


  for(size_t i=0;i<inputMuons->size();i++){
    ztop::NTMuon * muon = &inputMuons->at(i);
    //select
    if(muon->pt() < 20) continue;
    if(fabs(muon->eta()) > 2.4) continue;
    if(!(muon->isGlobal() || muon->isTracker())) continue;
    if(fabs(muon->isoVal()) > 0.2) continue;
    selectedMuons_ << muon;
  }

 
  /*
  for(size_t i=0;i<inputElectrons->size();i++){
    ztop::NTElectron * elec = &inputElectrons->at(i);

  }

  for(size_t i=0;i<inputElectrons->size();i++){
    ztop::NTElectron * elec = &inputElectrons->at(i);

  }
  */
  double mass=0;

  LorentzVector dilp4;
  if(mode_<0){ //ee
    return 0;
  }
  else if(mode_==0){ //emu
    return 0;
  }
  else{ //mumu

    if(selectedMuons_.size() <2) return 0;

    if(selectedMuons_.at(0)->q() == selectedMuons_.at(1)->q())   return 0;
    mass=(selectedMuons_.at(0)->p4() + selectedMuons_.at(1)->p4()).M();
  }

  

  return mass;

}



void trigger_looseMu(){

  

  using namespace std;
  using namespace ztop;

  std::vector<float> binsmumueta, bins2dee, bins2dmue,binsptmu, binspte, bins2dmumu;
  
  binsmumueta << -2.4 << -2.1 << -1.2 << -0.9 << 0.9 << 1.2 << 2.1 << 2.4;
  bins2dmumu << 0 << 0.9 << 1.2 << 2.1 << 2.4;

  triggerAnalyzer ta_mumud;

  ta_mumud.setMode("mumu");
  ta_mumud.setMassCutLow(12);
  ta_mumud.setIncludeCorr(true);

  ta_mumud.setBinsEta(binsmumueta);
  ta_mumud.setBinsEta2dX(bins2dmumu);
  ta_mumud.setBinsEta2dY(bins2dmumu);

  ta_mumud.setDileptonTrigger("HLT_Mu17_Mu8_v");

  triggerAnalyzer ta_mumuMC=ta_mumud;
  ta_mumuMC.setIsMC(true);


  

  TString dir="/scratch/hh/dust/naf/cms/user/kieseler/trees_ES_tth/";

  TString cmssw_base=getenv("CMSSW_BASE");
  TString pileuproot = cmssw_base+"/src/TtZAnalysis/Data/ttH.json_PU.root";


  std::vector<TString> mumumcfiles, datafilesFull,datafilesRunb,datafilesRunc, datafilesRuna;
 
  mumumcfiles << dir+"tree_8TeV_mumuttbar.root" 
	      << dir+"tree_8TeV_mumuttbarviatau.root" ;

  datafilesFull  << dir + "tree_8TeV_MET_runA_06Aug.root"  
	     << dir + "tree_8TeV_MET_runB_13Jul.root"  
	     << dir + "tree_8TeV_MET_runC_prompt.root"
	     << dir + "tree_8TeV_MET_runA_13Jul.root"  
	     << dir + "tree_8TeV_MET_runC_24Aug.root"  
	     << dir + "tree_8TeV_MET_runD_prompt.root";
  

  datafilesRuna << dir + "tree_8TeV_MET_runA_06Aug.root"
		<< dir + "tree_8TeV_MET_runA_13Jul.root" ;

  datafilesRunb << dir + "tree_8TeV_MET_runB_13Jul.root"  ;
  datafilesRunc << dir + "tree_8TeV_MET_runC_24Aug.root" 
		<< dir + "tree_8TeV_MET_runC_prompt.root" ; //TRAP dec11 is missing!!



  //for others just copy data and mc, change input and pu file
 

  TChain * datachainFull=makeChain(datafilesFull);
  TChain * datachainRunA=makeChain(datafilesRuna);
  TChain * datachainRunB=makeChain(datafilesRunb);
  TChain * datachainRunC=makeChain(datafilesRunc);
  TChain * mumuchain=makeChain(mumumcfiles);

  ta_mumud.setChain(datachainFull);
  ta_mumuMC.setChain(mumuchain);
  ta_mumuMC.setPUFile(pileuproot);
 
  /*
  ta_mumud.Eff();
  ta_mumuMC.Eff();
  makeFullOutput(ta_mumud, ta_mumuMC, "full", "full dataset", 0.01);
  */
 
  //Run A

  triggerAnalyzer ta_mumuRunA=ta_mumud;
  ta_mumuRunA.setChain(datachainRunA);
  ta_mumuRunA.setRunCutLow(190456);
  ta_mumuRunA.setRunCutHigh(193621);

  ta_mumuMC.setPUFile(cmssw_base+"/src/TtZAnalysis/Data/RunAComp.json.txt_PU.root");
  ta_mumuRunA.Eff();
  ta_mumuMC.Eff();
  makeFullOutput(ta_mumuRunA, ta_mumuMC, "RunAMu17Mu8", "Run A, Mu17Mu8", 0.01);

  //Run B

  triggerAnalyzer ta_mumuRunB=ta_mumud;
  ta_mumuRunB.setChain(datachainRunB);
  ta_mumuRunB.setRunCutLow(193834);
  ta_mumuRunB.setRunCutHigh(196531);

  ta_mumuMC.setPUFile(cmssw_base+"/src/TtZAnalysis/Data/RunB13Jul.json.txt_PU.root");
  ta_mumuRunB.Eff();
  ta_mumuMC.Eff();
  makeFullOutput(ta_mumuRunB, ta_mumuMC, "RunBMu17Mu8", "Run B, Mu17Mu8", 0.01);


  //Run C

  triggerAnalyzer ta_mumuRunC=ta_mumud;
  ta_mumuRunC.setChain(datachainRunC);
  ta_mumuRunC.setRunCutLow(198022);
  ta_mumuRunC.setRunCutHigh(203746);

  ta_mumuMC.setPUFile(cmssw_base+"/src/TtZAnalysis/Data/RunCComp.json.txt_PU.root");
  ta_mumuRunC.Eff();
  ta_mumuMC.Eff();
  makeFullOutput(ta_mumuRunC, ta_mumuMC, "RunCMu17Mu8", "Run C, Mu17Mu8", 0.01);

  ///////other trigger: Mu17_TkMu8


  ta_mumud.setDileptonTrigger("HLT_Mu17_TkMu8_v");
  ta_mumuMC.setDileptonTrigger("HLT_Mu17_TkMu8_v");

  //Run A 
  
  ta_mumuRunA=ta_mumud;
  ta_mumuRunA.setChain(datachainRunA);
  ta_mumuRunA.setRunCutLow(190456);
  ta_mumuRunA.setRunCutHigh(193621);

  ta_mumuMC.setPUFile(cmssw_base+"/src/TtZAnalysis/Data/RunAComp.json.txt_PU.root");
  ta_mumuRunA.Eff();
  ta_mumuMC.Eff();
  makeFullOutput(ta_mumuRunA, ta_mumuMC, "RunAMu17TkMu8", "Run A, Mu17TkMu8", 0.01);

  //Run B

  ta_mumuRunB=ta_mumud;
  ta_mumuRunB.setChain(datachainRunB);
  ta_mumuRunB.setRunCutLow(193834);
  ta_mumuRunB.setRunCutHigh(196531);

  ta_mumuMC.setPUFile(cmssw_base+"/src/TtZAnalysis/Data/RunB13Jul.json.txt_PU.root");
  ta_mumuRunB.Eff();
  ta_mumuMC.Eff();
  makeFullOutput(ta_mumuRunB, ta_mumuMC, "RunBMu17TkMu8", "Run B, Mu17TkMu8", 0.01);

  //Run C

  ta_mumuRunC=ta_mumud;
  ta_mumuRunC.setChain(datachainRunC);
  ta_mumuRunC.setRunCutLow(198022);
  ta_mumuRunC.setRunCutHigh(203746);

  ta_mumuMC.setPUFile(cmssw_base+"/src/TtZAnalysis/Data/RunCComp.json.txt_PU.root");
  ta_mumuRunC.Eff();
  ta_mumuMC.Eff();
  makeFullOutput(ta_mumuRunC, ta_mumuMC, "RunCMu17TkMu8", "Run C, Mu17TkMu8", 0.01);



  std::vector<string> bothtrig;
  bothtrig << "HLT_Mu17_TkMu8_v" << "HLT_Mu17_Mu8_v";



  ta_mumud.setDileptonTriggers(bothtrig);
  ta_mumuMC.setDileptonTriggers(bothtrig);

  //Run A 
  
  ta_mumuRunA=ta_mumud;
  ta_mumuRunA.setChain(datachainRunA);
  ta_mumuRunA.setRunCutLow(190456);
  ta_mumuRunA.setRunCutHigh(193621);

  ta_mumuMC.setPUFile(cmssw_base+"/src/TtZAnalysis/Data/RunAComp.json.txt_PU.root");
  ta_mumuRunA.Eff();
  ta_mumuMC.Eff();
  makeFullOutput(ta_mumuRunA, ta_mumuMC, "RunAOR", "Run A,  Mu17TkMu8 OR Mu17Mu8", 0.01);

  //Run B

  ta_mumuRunB=ta_mumud;
  ta_mumuRunB.setChain(datachainRunB);
  ta_mumuRunB.setRunCutLow(193834);
  ta_mumuRunB.setRunCutHigh(196531);

  ta_mumuMC.setPUFile(cmssw_base+"/src/TtZAnalysis/Data/RunB13Jul.json.txt_PU.root");
  ta_mumuRunB.Eff();
  ta_mumuMC.Eff();
  makeFullOutput(ta_mumuRunB, ta_mumuMC, "RunBOR", "Run B,  Mu17TkMu8 OR Mu17Mu8", 0.01);

  //Run C

  ta_mumuRunC=ta_mumud;
  ta_mumuRunC.setChain(datachainRunC);
  ta_mumuRunC.setRunCutLow(198022);
  ta_mumuRunC.setRunCutHigh(203746);

  ta_mumuMC.setPUFile(cmssw_base+"/src/TtZAnalysis/Data/RunCComp.json.txt_PU.root");
  ta_mumuRunC.Eff();
  ta_mumuMC.Eff();
  makeFullOutput(ta_mumuRunC, ta_mumuMC, "RunCOR", "Run C,  Mu17TkMu8 OR Mu17Mu8", 0.01);




}


/*
change breakat5fb in triggerAna..bas2 to the one in without 2 (probably not checked in....

runB first: 193834
runB last: 196531

*/
  
