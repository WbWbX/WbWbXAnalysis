#include "triggerAnalyzer_base3.h"



double 
triggerAnalyzer::selectDileptons(std::vector<ztop::NTMuon> * inputMuons, std::vector<ztop::NTElectron> * inputElectrons){


  bool domatching=false;

  ////////ONLY FOR TESTING!!! IF SWITCHED ON EFF=1 BY CONSTRUCTION DONT USE THIS THING FOR ANY EFF!!

  using namespace std;
  using namespace ztop;
  bool match=false;
  for(size_t i=0;i<inputMuons->size();i++){
    ztop::NTMuon * muon = &inputMuons->at(i);
    //select
    if(muon->pt() < 20) continue;
    if(fabs(muon->eta()) > 2.4) continue;
    if(!(muon->isGlobal())) continue;
    if(!(muon->isTracker())) continue;
    if(fabs(muon->normChi2()) > 10) continue;
    if(muon->muonHits() < 1) continue;
    if(fabs(muon->d0V()) > 0.2) continue;
    if(muon->trkHits() <= 5) continue;
    if(muon->pixHits() <= 0) continue;
    if(fabs(muon->dzV()) >= 0.5) continue;


    //number of matched stations missing!!!

    if(fabs(muon->isoVal()) > 0.2) continue;
       match=false;

    if(domatching){

      for(size_t j=0;j<muon->matchedTrig().size();j++){
	//std::cout << muon->matchedTrig().at(j) << std::endl;
	for(size_t k=0;k<trigs_.size();k++){
	  if(((TString)muon->matchedTrig().at(j)).Contains(trigs_.at(k))){
	    match=true;
	    break;
	  }
	 
	}
	if(match)
	  break;
      }
      if(!match)
	continue;
    }
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



void trigger_tightMu(){

  

  using namespace std;
  using namespace ztop;

  std::vector<float> binsmumueta, bins2dee, bins2dmue,binsptmu, binspte, bins2dmumu;
  
  binsmumueta << -2.4 << -2.1 << -1.2 << -0.9 << 0.9 << 1.2 << 2.1 << 2.4;
  bins2dmumu << 0 << 0.9 << 1.2 << 2.1 << 2.4;



  double lumiA=0.890;
  double lumiB=4.430;
  double lumiC=7.026;
  double lumiD=7.272;

  double lumitotal=lumiA+lumiB+lumiC+lumiD;

  triggerAnalyzer ta_mumud;

  ta_mumud.setMode("mumu");
  ta_mumud.setMassCutLow(12);
  ta_mumud.setIncludeCorr(true);
  ta_mumud.checkTriggerPaths(false);
  ta_mumud.setUseMatching(false);

  ta_mumud.setBinsEta(binsmumueta);
  ta_mumud.setBinsEta2dX(bins2dmumu);
  ta_mumud.setBinsEta2dY(bins2dmumu);

  ta_mumud.setDileptonTrigger("HLT_Mu17_Mu8_v");

  // 

  triggerAnalyzer ta_mumuMC=ta_mumud;
  ta_mumuMC.setIsMC(true);

 


  
  TString dir="/scratch/hh/dust/naf/cms/user/kieseler/trees_Apr13_04/";

  TString cmssw_base=getenv("CMSSW_BASE");
  TString pileuproot = cmssw_base+"/src/TtZAnalysis/Data/Full19.json.txt_PU.root";


  std::vector<TString> mumumcfiles, datafilesFull,datafilesRunb,datafilesRunc, datafilesRuna,datafilesRund;
 
  mumumcfiles << dir+"tree_8TeV_mumuttbar.root" 
	      << dir+"tree_8TeV_mumuttbarviatau.root" ;

  datafilesFull  << dir + "tree_8TeV_MET_runA_06Aug.root"  
		 << dir + "tree_8TeV_MET_runA_13Jul.root"  
		 << dir + "tree_8TeV_MET_runB_13Jul.root"  
		 << dir + "tree_8TeV_MET_runC_prompt.root"
		 << dir + "tree_8TeV_MET_runC_24Aug.root"  
		 << dir + "tree_8TeV_MET_runC_11Dec.root"  
  		 << dir + "tree_8TeV_MET_runD_prompt.root";
  

  datafilesRuna << dir + "tree_8TeV_MET_runA_06Aug.root"
  		<< dir + "tree_8TeV_MET_runA_13Jul.root" ;

  datafilesRunb << dir + "tree_8TeV_MET_runB_13Jul.root"  ;
  datafilesRunc << dir + "tree_8TeV_MET_runC_24Aug.root"  
  		<< dir + "tree_8TeV_MET_runC_11Dec.root" 
  		<< dir + "tree_8TeV_MET_runC_prompt.root" ; 

  datafilesRund << dir + "tree_8TeV_MET_runD_prompt.root" ; 



  //for others just copy data and mc, change input and pu file
 

  TChain * datachainFull=makeChain(datafilesFull);
  TChain * datachainRunA=makeChain(datafilesRuna);
  TChain * datachainRunB=makeChain(datafilesRunb);
  TChain * datachainRunC=makeChain(datafilesRunc);
  TChain * datachainRunD=makeChain(datafilesRund);
  TChain * mumuchain=makeChain(mumumcfiles);

  ta_mumud.setChain(datachainFull);
  ta_mumuMC.setChain(mumuchain);
  ta_mumuMC.setPUFile(pileuproot);
 
  
  //  ta_mumud.Eff();
  //  ta_mumuMC.Eff();
  //  makeFullOutput(ta_mumud, ta_mumuMC, "fullMu17Mu8", "19 fb^{-1}, Mu17Mu8", 0.01);
  
  
  //Run A

  triggerAnalyzer ta_mumudA=ta_mumud;
  ta_mumudA.setChain(datachainRunA);
  ta_mumudA.setRunCutLow(190456);
  ta_mumudA.setRunCutHigh(193621);

  triggerAnalyzer ta_mumuMCA=ta_mumuMC;
  ta_mumuMCA.setPUFile(cmssw_base+"/src/TtZAnalysis/Data/RunAComp.json.txt_PU.root");
  ta_mumudA.Eff();
  ta_mumuMCA.Eff();
  makeFullOutput(ta_mumudA, ta_mumuMCA, "RunAMu17Mu8", "Run A, Mu17Mu8", 0.01);

  

  //Run B

  triggerAnalyzer ta_mumudB=ta_mumud;
  ta_mumudB.setChain(datachainRunB);
  ta_mumudB.setRunCutLow(193834);
  ta_mumudB.setRunCutHigh(196531);

  triggerAnalyzer ta_mumuMCB=ta_mumuMC;
  ta_mumuMCB.setPUFile(cmssw_base+"/src/TtZAnalysis/Data/RunB13Jul.json.txt_PU.root");
  ta_mumudB.Eff();
  ta_mumuMCB.Eff();
  makeFullOutput(ta_mumudB, ta_mumuMCB, "RunBMu17Mu8", "Run B, Mu17Mu8", 0.01);
  

  //Run C

  triggerAnalyzer ta_mumudC=ta_mumud;
  ta_mumudC.setChain(datachainRunC);
  ta_mumudC.setRunCutLow(198022);
  ta_mumudC.setRunCutHigh(203746);

  triggerAnalyzer ta_mumuMCC=ta_mumuMC;
  ta_mumuMCC.setPUFile(cmssw_base+"/src/TtZAnalysis/Data/RunCComp.json.txt_PU.root");
  ta_mumudC.Eff();
  ta_mumuMCC.Eff();
  makeFullOutput(ta_mumudC, ta_mumuMCC, "RunCMu17Mu8", "Run C, Mu17Mu8", 0.01);


  //Run D
  
  triggerAnalyzer ta_mumudD=ta_mumud;
  ta_mumudD.setChain(datachainRunD);
  ta_mumudD.setRunCutLow(203768);
  ta_mumudD.setRunCutHigh(208686);

  triggerAnalyzer ta_mumuMCD=ta_mumuMC;
  ta_mumuMCD.setPUFile(cmssw_base+"/src/TtZAnalysis/Data/RunDprompt.json.txt_PU.root");
  ta_mumudD.Eff();
  ta_mumuMCD.Eff();
  makeFullOutput(ta_mumudD, ta_mumuMCD, "RunDMu17Mu8", "Run D, Mu17Mu8", 0.01);


  //combine

  double mumutotal=ta_mumudA.getGlobalDen() + ta_mumudB.getGlobalDen() + ta_mumudC.getGlobalDen() + ta_mumudD.getGlobalDen();
  double mumuAscale=mumutotal/ta_mumudA.getGlobalDen() * lumiA/lumitotal;
  ta_mumudA.scale(mumuAscale);
  ta_mumuMCA.scale(mumuAscale);
  double mumuBscale=mumutotal/ta_mumudB.getGlobalDen() * lumiB/lumitotal;
  ta_mumudB.scale(mumuBscale);
  ta_mumuMCB.scale(mumuBscale);
  double mumuCscale=mumutotal/ta_mumudC.getGlobalDen() * lumiC/lumitotal;
  ta_mumudC.scale(mumuCscale);
  ta_mumuMCC.scale(mumuCscale);
  double mumuDscale=mumutotal/ta_mumudD.getGlobalDen() * lumiD/lumitotal;
  ta_mumudD.scale(mumuDscale);
  ta_mumuMCD.scale(mumuDscale);


  triggerAnalyzer mumudFull = ta_mumudA + ta_mumudB + ta_mumudC + ta_mumudD;
  triggerAnalyzer mumumcFull = ta_mumuMCA + ta_mumuMCB + ta_mumuMCC + ta_mumuMCD;
  makeFullOutput(mumudFull, mumumcFull, "FullMu17Mu8", "Mu17Mu8, 19 fb^{-1}", 0.01);


  ///////other trigger: Mu17_TkMu8
  
  
  ta_mumud.setDileptonTrigger("HLT_Mu17_TkMu8_v");
  ta_mumuMC.setDileptonTrigger("HLT_Mu17_TkMu8_v");


  ta_mumud.setChain(datachainFull);
  ta_mumuMC.setChain(mumuchain);
  ta_mumuMC.setPUFile(pileuproot);
 
  
  // ta_mumud.Eff();
  //  ta_mumuMC.Eff();
  // makeFullOutput(ta_mumud, ta_mumuMC, "fullMu17TkMu8", "19 fb^{-1}, Mu17TkMu8", 0.01);

  
  //Run A 
  
  ta_mumudA=ta_mumud;
  ta_mumudA.setChain(datachainRunA);
  ta_mumudA.setRunCutLow(190456);
  ta_mumudA.setRunCutHigh(193621);

  ta_mumuMCA=ta_mumuMC;
  ta_mumuMCA.setPUFile(cmssw_base+"/src/TtZAnalysis/Data/RunAComp.json.txt_PU.root");
  ta_mumudA.Eff();
  ta_mumuMCA.Eff();
  makeFullOutput(ta_mumudA, ta_mumuMCA, "RunAMu17TkMu8", "Run A, Mu17TkMu8", 0.01);

  //Run B

  ta_mumudB=ta_mumud;
  ta_mumudB.setChain(datachainRunB);
  ta_mumudB.setRunCutLow(193834);
  ta_mumudB.setRunCutHigh(196531);

  ta_mumuMCB=ta_mumuMC;
  ta_mumuMCB.setPUFile(cmssw_base+"/src/TtZAnalysis/Data/RunB13Jul.json.txt_PU.root");
  ta_mumudB.Eff();
  ta_mumuMCB.Eff();
  makeFullOutput(ta_mumudB, ta_mumuMCB, "RunBMu17TkMu8", "Run B, Mu17TkMu8", 0.01);

  //Run C

  ta_mumudC=ta_mumud;
  ta_mumudC.setChain(datachainRunC);
  ta_mumudC.setRunCutLow(198022);
  ta_mumudC.setRunCutHigh(203746);

  ta_mumuMCC=ta_mumuMC;
  ta_mumuMCC.setPUFile(cmssw_base+"/src/TtZAnalysis/Data/RunCComp.json.txt_PU.root");
  ta_mumudC.Eff();
  ta_mumuMCC.Eff();
  makeFullOutput(ta_mumudC, ta_mumuMCC, "RunCMu17TkMu8", "Run C, Mu17TkMu8", 0.01);

  //Run D

  ta_mumudD=ta_mumud;
  ta_mumudD.setChain(datachainRunD);
  ta_mumudD.setRunCutLow(203768);
  ta_mumudD.setRunCutHigh(208686);

  ta_mumuMCD=ta_mumuMC;
  ta_mumuMCD.setPUFile(cmssw_base+"/src/TtZAnalysis/Data/RunDprompt.json.txt_PU.root");
  ta_mumudD.Eff();
  ta_mumuMCD.Eff();
  makeFullOutput(ta_mumudC, ta_mumuMCD, "RunDMu17TkMu8", "Run D, Mu17TkMu8", 0.01);

  
  ///combine

  mumutotal=ta_mumudA.getGlobalDen() + ta_mumudB.getGlobalDen() + ta_mumudC.getGlobalDen() + ta_mumudD.getGlobalDen();
  mumuAscale=mumutotal/ta_mumudA.getGlobalDen() * lumiA/lumitotal;
  ta_mumudA.scale(mumuAscale);
  ta_mumuMCA.scale(mumuAscale);
  mumuBscale=mumutotal/ta_mumudB.getGlobalDen() * lumiB/lumitotal;
  ta_mumudB.scale(mumuBscale);
  ta_mumuMCB.scale(mumuBscale);
  mumuCscale=mumutotal/ta_mumudC.getGlobalDen() * lumiC/lumitotal;
  ta_mumudC.scale(mumuCscale);
  ta_mumuMCC.scale(mumuCscale);
  mumuDscale=mumutotal/ta_mumudD.getGlobalDen() * lumiD/lumitotal;
  ta_mumudD.scale(mumuDscale);
  ta_mumuMCD.scale(mumuDscale);


  mumudFull = ta_mumudA + ta_mumudB + ta_mumudC + ta_mumudD;
  mumumcFull = ta_mumuMCA + ta_mumuMCB + ta_mumuMCC + ta_mumuMCD;
  makeFullOutput(mumudFull, mumumcFull, "FullMu17TkMu8", "Mu17TkMu8, 19 fb^{-1}", 0.01);




  std::vector<string> bothtrig;
  bothtrig << "HLT_Mu17_TkMu8_v" << "HLT_Mu17_Mu8_v";



  ta_mumud.setDileptonTriggers(bothtrig);
  ta_mumuMC.setDileptonTriggers(bothtrig);

  ta_mumud.setChain(datachainFull);
  ta_mumuMC.setChain(mumuchain);
  ta_mumuMC.setPUFile(pileuproot);
 
  
  //  ta_mumud.Eff();
  //  ta_mumuMC.Eff();
  //  makeFullOutput(ta_mumud, ta_mumuMC, "fullOR", "19 fb^{-1}, Mu17TkMu8 OR Mu17Mu8", 0.01);

  
  //Run A 

  
  ta_mumudA=ta_mumud;
  ta_mumudA.setChain(datachainRunA);
  ta_mumudA.setRunCutLow(190456);
  ta_mumudA.setRunCutHigh(193621);

  ta_mumuMCA=ta_mumuMC;
  ta_mumuMCA.setPUFile(cmssw_base+"/src/TtZAnalysis/Data/RunAComp.json.txt_PU.root");
  ta_mumudA.Eff();
  ta_mumuMCA.Eff();
  makeFullOutput(ta_mumudA, ta_mumuMCA, "RunAOR", "Run A,  Mu17TkMu8 OR Mu17Mu8", 0.01);

  //Run B

  ta_mumudB=ta_mumud;
  ta_mumudB.setChain(datachainRunB);
  ta_mumudB.setRunCutLow(193834);
  ta_mumudB.setRunCutHigh(196531);

  ta_mumuMCB=ta_mumuMC;
  ta_mumuMCB.setPUFile(cmssw_base+"/src/TtZAnalysis/Data/RunB13Jul.json.txt_PU.root");
  ta_mumudB.Eff();
  ta_mumuMCB.Eff();
  makeFullOutput(ta_mumudB, ta_mumuMCB, "RunBOR", "Run B,  Mu17TkMu8 OR Mu17Mu8", 0.01);

  //Run C

  ta_mumudC=ta_mumud;
  ta_mumudC.setChain(datachainRunC);
  ta_mumudC.setRunCutLow(198022);
  ta_mumudC.setRunCutHigh(203746);

  ta_mumuMCC=ta_mumuMC;
  ta_mumuMC.setPUFile(cmssw_base+"/src/TtZAnalysis/Data/RunCComp.json.txt_PU.root");
  ta_mumudC.Eff();
  ta_mumuMCC.Eff();
  makeFullOutput(ta_mumudC, ta_mumuMCC, "RunCOR", "Run C,  Mu17TkMu8 OR Mu17Mu8", 0.01);


  //Run D

  ta_mumudD=ta_mumud;
  ta_mumudD.setChain(datachainRunD);
  ta_mumudD.setRunCutLow(203768);
  ta_mumudD.setRunCutHigh(208686);

  ta_mumuMCD=ta_mumuMC;
  ta_mumuMCD.setPUFile(cmssw_base+"/src/TtZAnalysis/Data/RunDprompt.json.txt_PU.root");
  ta_mumudD.Eff();
  ta_mumuMCD.Eff();
  makeFullOutput(ta_mumudC, ta_mumuMCD, "RunDOR", "Run D, Mu17TkMu8 OR Mu17Mu8", 0.01);

  
  ////combine



  mumutotal=ta_mumudA.getGlobalDen() + ta_mumudB.getGlobalDen() + ta_mumudC.getGlobalDen() + ta_mumudD.getGlobalDen();
  mumuAscale=mumutotal/ta_mumudA.getGlobalDen() * lumiA/lumitotal;
  ta_mumudA.scale(mumuAscale);
  ta_mumuMCA.scale(mumuAscale);
  mumuBscale=mumutotal/ta_mumudB.getGlobalDen() * lumiB/lumitotal;
  ta_mumudB.scale(mumuBscale);
  ta_mumuMCB.scale(mumuBscale);
  mumuCscale=mumutotal/ta_mumudC.getGlobalDen() * lumiC/lumitotal;
  ta_mumudC.scale(mumuCscale);
  ta_mumuMCC.scale(mumuCscale);
  mumuDscale=mumutotal/ta_mumudD.getGlobalDen() * lumiD/lumitotal;
  ta_mumudD.scale(mumuDscale);
  ta_mumuMCD.scale(mumuDscale);


  mumudFull = ta_mumudA + ta_mumudB + ta_mumudC + ta_mumudD;
  mumumcFull = ta_mumuMCA + ta_mumuMCB + ta_mumuMCC + ta_mumuMCD;
  makeFullOutput(mumudFull, mumumcFull, "FullOR", "Mu17TkMu8 OR Mu17Mu8, 19 fb^{-1}", 0.01);
  


}


/*
  change breakat5fb in triggerAna..bas2 to the one in without 2 (probably not checked in....

  runB first: 193834
  runB last: 196531

*/
  
