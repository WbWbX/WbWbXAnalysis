#include "triggerAnalyzer_base.h"
#include "TtZAnalysis/plugins/leptonSelector2.h"


double 
triggerAnalyzer::selectDileptons(std::vector<ztop::NTMuon> * inputMuons, std::vector<ztop::NTElectron> * inputElectrons){

  for(size_t i=0;i<inputMuons->size();i++){
    ztop::NTMuon * muon = &inputMuons->at(i);

    selectedMuons_ << muon;
  }
  for(size_t i=0;i<inputElectrons->size();i++){
    ztop::NTElectron * elec = &inputElectrons->at(i);


    selectedElecs_ << elec;
  }
  double mass=0;

  pair<vector<NTElectron*>, vector<NTMuon*> > leppair;
  leppair = getOppoQHighestPtPair(selectedElecs_, selectedMuons_);


  return mass;

}



void blabla(){

  

  using namespace std;
  using namespace ztop;

  std::vector<float> binsmumueta, bins2dee, bins2dmu;
  binsmumueta.push_back(-2.4);binsmumueta.push_back(-2.1);binsmumueta.push_back(-1.2);binsmumueta.push_back(-0.9);binsmumueta.push_back(0.9);binsmumueta.push_back(1.2);binsmumueta.push_back(2.1);binsmumueta.push_back(2.4);

  bins2dee << 0 << 1.479 << 2.5;
  bins2dmu << 0 << 0.9 << 2.4;

  triggerAnalyzer ta_eed;
  triggerAnalyzer ta_mumud;
  triggerAnalyzer ta_emud;
  ta_eed.setMode("ee");
  ta_emud.setMode("emu");
  ta_mumud.setMode("mumu");

  ta_mumud.setBinsEta(binsmumueta);
  ta_eed.setBinsEta2dX(bins2dee);
  ta_eed.setBinsEta2dY(bins2dee);
  ta_emud.setBinsEta2dX(bins2dee);
  ta_emud.setBinsEta2dY(bins2dmu);
  triggerAnalyzer ta_eeMC=ta_eed;
  triggerAnalyzer ta_mumuMC=ta_mumud;
  triggerAnalyzer ta_emuMC=ta_emud;

  ta_eeMC.setIsMC(true);
  ta_mumuMC.setIsMC(true);
  ta_emuMC.setIsMC(true);



  TString dir="/scratch/hh/dust/naf/cms/user/kieseler/trees_ES_tth/";

  TString cmssw_base=getenv("CMSSW_BASE");
  TString pileuproot = cmssw_base+"/src/TtZAnalysis/Data/ttH.json_PU.root";//HCP_PU.root";//HCP_5.3fb_PU.root";


  std::vector<TString> eemcfiles,mumumcfiles,emumcfiles, datafiles;
  eemcfiles << dir+"tree_8TeV_eettbar.root"
    	    << dir+"tree_8TeV_eettbarviatau.root" ;

  mumumcfiles << dir+"tree_8TeV_mumuttbar.root" 
    	      << dir+"tree_8TeV_mumuttbarviatau.root" ;

  emumcfiles << dir+"tree_8TeV_emuttbar.root"
    	     << dir+"tree_8TeV_emuttbarviatau.root";


  datafiles  << dir + "tree_8TeV_MET_runA_06Aug.root"  
	     << dir + "tree_8TeV_MET_runB_13Jul.root"  
	     << dir + "tree_8TeV_MET_runC_prompt.root"
	     << dir + "tree_8TeV_MET_runA_13Jul.root"  
	     << dir + "tree_8TeV_MET_runC_24Aug.root"  
	     << dir + "tree_8TeV_MET_runD_prompt.root";
  

  
  ta_eeMC.setPUFile(pileuproot);
  ta_mumuMC.setPUFile(pileuproot);
  ta_emuMC.setPUFile(pileuproot);

  TChain * datachain=makeChain(datafiles);

  TChain * eechain=makeChain(eemcfiles);
  TChain * mumuchain=makeChain(mumumcfiles);
  TChain * emuchain=makeChain(emumcfiles);

  ta_eeMC.setChain(eechain);
  ta_mumuMC.setChain(mumuchain);
  ta_emuMC.setChain(emuchain);

  ta_eed.setChain(datachain);
  ta_mumud.setChain(datachain);
  ta_emud.setChain(datachain);


  analyze( ta_eed,  ta_eeMC,  ta_mumud,  ta_mumuMC,  ta_emud,  ta_emuMC);
  
  miniscript();
}

  
