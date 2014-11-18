#include "TtZAnalysis/Analysis/triggerEfficiencies12/correlationChecker.h"




void checkCorrelations(){

	  using namespace top;
	  using namespace std;
  vector<TString> allmettriggers, selectedmettriggers;;
  allmettriggers << "HLT_DiCentralJetSumpT100_dPhi05_DiCentralPFJet60_25_PFMET100_HBHENoiseCleaned_v1" << "HLT_DiCentralPFJet30_PFMET80_v2" << "HLT_DiCentralPFJet50_PFMET80_v6" << "HLT_DiCentralPFJet30_PFMET80_BTagCSV07_v1" << "HLT_MET80_v3" << "HLT_MET120_v10" << "HLT_MET120_HBHENoiseCleaned_v3" << "HLT_Ele32_WP80_CentralPFJet35_CentralPFJet25_PFMET20_v1" << "HLT_DoubleEle14_CaloIdT_TrkIdVL_Mass8_PFMET40_v5" << "HLT_DoubleEle14_CaloIdT_TrkIdVL_Mass8_PFMET50_v5" << "HLT_Ele27_WP80_PFMET_MT50_v4" << "HLT_MonoCentralPFJet80_PFMETnoMu95_NHEF0p95_v5" << "HLT_Photon70_CaloIdXL_PFMET100_v4" << "HLT_LooseIsoPFTau35_Trk20_Prong1_MET70_v6" << "HLT_LooseIsoPFTau35_Trk20_Prong1_MET75_v6" << "HLT_PFHT400_Mu5_PFMET45_v6" << "HLT_PFHT400_Mu5_PFMET50_v6" << "HLT_CleanPFHT350_Ele5_CaloIdT_CaloIsoVL_TrkIdT_TrkIsoVL_PFMET45_v5" << "HLT_CleanPFHT350_Ele5_CaloIdT_CaloIsoVL_TrkIdT_TrkIsoVL_PFMET50_v5" << "HLT_CleanPFHT300_Ele15_CaloIdT_CaloIsoVL_TrkIdT_TrkIsoVL_PFMET45_v5" << "HLT_CleanPFHT300_Ele15_CaloIdT_CaloIsoVL_TrkIdT_TrkIsoVL_PFMET50_v5" << "HLT_Mu14_Ele14_CaloIdT_TrkIdVL_Mass8_PFMET40_v5" << "HLT_Mu14_Ele14_CaloIdT_TrkIdVL_Mass8_PFMET50_v5" << "HLT_PFMET150_v4" << "HLT_PFMET180_v4" << "HLT_PFHT350_PFMET100_v6" << "HLT_MET80_Track50_dEdx3p6_v4" << "HLT_PFHT400_PFMET100_v6" << "HLT_DiPFJet40_PFMETnoMu65_MJJ800VBF_AllJets_v5" << "HLT_L2Mu70_eta2p1_PFMET55_v1" << "HLT_L2Mu70_eta2p1_PFMET60_v1" << "HLT_MET200_v10" << "HLT_MET200_HBHENoiseCleaned_v3" << "HLT_MET80_Track60_dEdx3p7_v4" << "HLT_PFHT350_Mu15_PFMET45_v6" << "HLT_PFHT350_Mu15_PFMET50_v6" << "HLT_DisplacedPhoton65_CaloIdVL_IsoL_PFMET25_v1" << "HLT_DisplacedPhoton65EBOnly_CaloIdVL_IsoL_PFMET30_v1" << "HLT_DiPFJet40_PFMETnoMu65_MJJ600VBF_LeadingJets_v5" << "HLT_DoubleMu14_Mass8_PFMET40_v5" << "HLT_DoubleMu14_Mass8_PFMET50_v5" << "HLT_IsoMu24_CentralPFJet30_CentralPFJet25_PFMET20_v1" << "HLT_MET300_v2" << "HLT_MET300_HBHENoiseCleaned_v3" << "HLT_MET400_v5" << "HLT_MET400_HBHENoiseCleaned_v3";

  selectedmettriggers << "HLT_DiCentralPFJet30_PFMET80_BTagCSV07_v1" << "HLT_LooseIsoPFTau35_Trk20_Prong1_MET75_v6";
  


  TString MCdir="/scratch/hh/dust/naf/cms/user/kieseler/trees_8TeV/";

  std::vector<TString> eemcfiles,mumumcfiles,emumcfiles;
  eemcfiles << MCdir+"tree_8TeV_eettbar.root"
    	    << MCdir+"tree_8TeV_eettbarviatau.root" ;

  mumumcfiles << MCdir+"tree_8TeV_mumuttbar.root" 
    	      << MCdir+"tree_8TeV_mumuttbarviatau.root" ;

  emumcfiles << MCdir+"tree_8TeV_emuttbar.root"
    	     << MCdir+"tree_8TeV_emuttbarviatau.root";


  correlationChecker ee("ee");
  ee.dileptonTriggers() << "HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v";
  ee.crossTriggers() = selectedmettriggers;//allmettriggers;
  ee.inputfiles()    = eemcfiles;

    ee.getTriggers("MET");
  // ee.getCorrelations();

}

