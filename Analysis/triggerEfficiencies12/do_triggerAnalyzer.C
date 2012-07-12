
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TtZAnalysis/DataFormats/interface/NTMuon.h"
#include "TtZAnalysis/DataFormats/interface/NTElectron.h"
#include "TtZAnalysis/DataFormats/interface/NTEvent.h"
#include "TtZAnalysis/DataFormats/interface/NTJet.h"
#include "TtZAnalysis/DataFormats/interface/NTMet.h"
#include "TtZAnalysis/plugins/reweightPU.h"
#include <vector>
#include <algorithm>
#include <string>
#include <utility>
#include "TCanvas.h"
#include "TROOT.h"
#include "makeplotsnice.h"
#include "TtZAnalysis/plugins/container.h"

using namespace std;



  double ratiomultiplier=1;




class triggerAnalyzer{

public:
  triggerAnalyzer(){
    binseta_.push_back(-2.5);binseta_.push_back(-1.5);binseta_.push_back(-0.8);binseta_.push_back(0.8);binseta_.push_back(1.5);binseta_.push_back(2.5);

    binspt_.push_back(20);binspt_.push_back(30);binspt_.push_back(40);binspt_.push_back(50);binspt_.push_back(200);
     };
  ~triggerAnalyzer(){};
  void setBinsEta(std::vector<float> binseta){binseta_.clear();binseta_=binseta;};
  void setBinsPt(std::vector<float> binspt){binspt_.clear();binspt_=binspt;}

  top::container1D getEtaPlot(){return etaeff_;}
  top::container1D getPtPlot(){return pteff_;}
  top::container1D getCorrelationPt(){return corrpt_;}
  top::container1D getCorrelationEta(){return correta_;}
  top::container1D getDPhiPlot(){return dphieff_;}
  top::container1D getCorrelationDPhi(){return corrdphi_;}



  vector<double> Eff(string mode="ee",  const char * inputFile="  ", const char * pufile_ =" ", bool IsMC=true, bool makeTeXTable=false){

  isMC=IsMC;

  using namespace top;
  using namespace std;

  std::vector<string> mettriggers;
  
  mettriggers.push_back("HLT_DiCentralPFJet50_PFMET80_v4");
  mettriggers.push_back("HLT_MET120_HBHENoiseCleaned_v3");
  mettriggers.push_back("HLT_PFHT350_PFMET100_v4");
  mettriggers.push_back("HLT_PFHT400_PFMET100_v4");
  mettriggers.push_back("HLT_MonoCentralPFJet80_PFMETnoMu95_NHEF0p95_v3");
  mettriggers.push_back("HLT_MET120_v10");
  mettriggers.push_back("HLT_DiCentralPFJet30_CaloMET50_dPhi1_PFMHT80_HBHENoiseFiltered_v4");
  mettriggers.push_back("HLT_CentralPFJet80_CaloMET50_dPhi1_PFMHT80_HBHENoiseFiltered_v4");
  mettriggers.push_back("HLT_DiCentralJet20_BTagIP_MET65_HBHENoiseFiltered_dPhi1_v4");
  mettriggers.push_back("HLT_DiCentralJet20_CaloMET65_BTagCSV07_PFMHT80_v3");
  mettriggers.push_back("HLT_MET80_Track50_dEdx3p6_v4");
  mettriggers.push_back("HLT_MET80_Track60_dEdx3p7_v4");
  mettriggers.push_back("HLT_MET200_v10");
  mettriggers.push_back("HLT_MET200_HBHENoiseCleaned_v3");
  mettriggers.push_back("HLT_MET300_v2");
  mettriggers.push_back("HLT_MET300_HBHENoiseCleaned_v3");
  mettriggers.push_back("HLT_PFMET150_v3");
  mettriggers.push_back("HLT_DiPFJet40_PFMETnoMu65_MJJ800VBF_AllJets_v3");
  mettriggers.push_back("HLT_DiPFJet40_PFMETnoMu65_MJJ600VBF_LeadingJets_v3");
  mettriggers.push_back("HLT_PFMET180_v3");
  mettriggers.push_back("HLT_MET80_v3");
  mettriggers.push_back("HLT_Photon70_CaloIdXL_PFMET100_v3");
  mettriggers.push_back("HLT_MonoCentralPFJet80_PFMETnoMu95_NHEF0p95_v4");
  mettriggers.push_back("HLT_DiPFJet40_PFMETnoMu65_MJJ600VBF_LeadingJets_v4");
  mettriggers.push_back("HLT_CentralPFJet80_CaloMET50_dPhi1_PFMHT80_HBHENoiseFiltered_v5");
  mettriggers.push_back("HLT_DiCentralPFJet50_PFMET80_v5");
  mettriggers.push_back("HLT_DiCentralPFJet30_CaloMET50_dPhi1_PFMHT80_HBHENoiseFiltered_v5");
  mettriggers.push_back("HLT_PFMET150_v4");
  mettriggers.push_back("HLT_PFMET180_v4");
  mettriggers.push_back("HLT_DiCentralJet20_CaloMET65_BTagCSV07_PFMHT80_v4");
  mettriggers.push_back("HLT_Photon70_CaloIdXL_PFMET100_v4");
  mettriggers.push_back("HLT_PFHT350_PFMET100_v5");
  mettriggers.push_back("HLT_PFHT400_PFMET100_v5");
  mettriggers.push_back("HLT_DiPFJet40_PFMETnoMu65_MJJ800VBF_AllJets_v4");
  mettriggers.push_back("HLT_MonoCentralPFJet80_PFMETnoMu95_NHEF0p95_v2");
  mettriggers.push_back("HLT_DiCentralPFJet50_PFMET80_v3");
  mettriggers.push_back("HLT_DiCentralPFJet30_CaloMET50_dPhi1_PFMHT80_HBHENoiseFiltered_v3");
  mettriggers.push_back("HLT_CentralPFJet80_CaloMET50_dPhi1_PFMHT80_HBHENoiseFiltered_v3");
  mettriggers.push_back("HLT_DiCentralJet20_CaloMET65_BTagCSV07_PFMHT80_v2");
  mettriggers.push_back("HLT_MET80_Track50_dEdx3p6_v3");
  mettriggers.push_back("HLT_MET80_Track60_dEdx3p7_v3");
  mettriggers.push_back("HLT_Photon70_CaloIdXL_PFMET100_v2");
  mettriggers.push_back("HLT_DiCentralJet20_BTagIP_MET65_HBHENoiseFiltered_dPhi1_v3");
  mettriggers.push_back("HLT_PFHT350_PFMET100_v3");
  mettriggers.push_back("HLT_MET120_HBHENoiseCleaned_v2");
  mettriggers.push_back("HLT_DiPFJet40_PFMETnoMu65_MJJ600VBF_LeadingJets_v2");
  mettriggers.push_back("HLT_PFMET150_v2");
  mettriggers.push_back("HLT_MET120_v9");
  mettriggers.push_back("HLT_MET200_v9");
  mettriggers.push_back("HLT_MET200_HBHENoiseCleaned_v2");
  mettriggers.push_back("HLT_MET300_v1");
  mettriggers.push_back("HLT_MET300_HBHENoiseCleaned_v2");
  mettriggers.push_back("HLT_MET400_v4");
  mettriggers.push_back("HLT_MET400_HBHENoiseCleaned_v2");
  mettriggers.push_back("HLT_PFHT400_PFMET100_v3");
  mettriggers.push_back("HLT_PFMET180_v2");
  mettriggers.push_back("HLT_DiPFJet40_PFMETnoMu65_MJJ800VBF_AllJets_v2");
  mettriggers.push_back("HLT_MET80_v2");
  mettriggers.push_back("HLT_MET400_v5");
  mettriggers.push_back("HLT_MET400_HBHENoiseCleaned_v3");
  mettriggers.push_back("HLT_PFHT350_PFMET100_v6");
  mettriggers.push_back("HLT_PFHT400_PFMET100_v6");
  mettriggers.push_back("HLT_DiCentralPFJet50_PFMET80_v6");
  mettriggers.push_back("HLT_MonoCentralPFJet80_PFMETnoMu95_NHEF0p95_v5");
  mettriggers.push_back("HLT_DiCentralPFJet30_PFMET80_BTagCSV07_v1");
  mettriggers.push_back("HLT_DiPFJet40_PFMETnoMu65_MJJ600VBF_LeadingJets_v5");
  mettriggers.push_back("HLT_DiCentralPFJet30_PFMET80_v2");
  mettriggers.push_back("HLT_DiPFJet40_PFMETnoMu65_MJJ800VBF_AllJets_v5");
  mettriggers.push_back("HLT_DiCentralJetSumpT100_dPhi05_DiCentralPFJet60_25_PFMET100_HBHENoiseCleaned_v1");
  mettriggers.push_back("HLT_DisplacedPhoton65_CaloIdVL_IsoL_PFMET25_v1");
  mettriggers.push_back("HLT_DisplacedPhoton65EBOnly_CaloIdVL_IsoL_PFMET30_v1");



  //just for testing
  // mettriggers.clear();
  //mettriggers.push_back("HLT_MET120_v10");

  vector<string> notinMCtriggers;
notinMCtriggers.push_back("HLT_DiCentralPFJet30_PFMET80_BTagCSV07_v");
notinMCtriggers.push_back("HLT_DiCentralPFJet30_PFMET80_v");
notinMCtriggers.push_back("HLT_DiCentralJetSumpT100_dPhi05_DiCentralPFJet60_25_PFMET100_HBHENoiseCleaned_v");
notinMCtriggers.push_back("HLT_DisplacedPhoton65_CaloIdVL_IsoL_PFMET25_v");
notinMCtriggers.push_back("HLT_DisplacedPhoton65EBOnly_CaloIdVL_IsoL_PFMET30_v");

 for(vector<string>::iterator triggername=mettriggers.begin();triggername<mettriggers.end();triggername++){
   for(unsigned int i=0;i<notinMCtriggers.size();i++){
     if( ((TString) *triggername).Contains(notinMCtriggers[i]) ){
        mettriggers.erase(triggername);
     }
   }
 }


  std::vector<string> mettriggersMC;
 

  mettriggersMC=mettriggers;
  for(unsigned int i=0;i<mettriggersMC.size();i++){
    size_t sz=mettriggersMC[i].size();
    mettriggersMC[i].resize(sz-1,'s');
    //cout << mettriggersMC[i] << endl;
  }
  // two have number more than 1 digit
  mettriggersMC.push_back("HLT_MET120_v");
  mettriggersMC.push_back("HLT_MET200_v");


  //////////////////////////////////////////////////////////////////////

  //inputFile="/scratch/hh/current/cms/user/kieseler/2012/trees0525/tree_mumu_mumu_allmodes_met_ttbar.root.root";
  //inputFile="/scratch/hh/current/cms/user/kieseler/2012/trees0525/tree_mumu_mumu_allmodes_met_runB_prompt.root.root";
  const char * directoryname="TestTreePF";
  const char * treename="pfTree";

  //new naming
    directoryname="PFTree";
  

  //mode="ee";
  //isMC=true;
  bool is52v9=false;
  bool CiCId=false;
  float maxEntries=0;
  bool doPUweight=true;

  //////////////////////////////////


  Int_t nbinseta=binseta_.size()-1;
  Int_t nbinspt=binspt_.size()-1;
    Double_t binseta[nbinseta+1];
    Double_t binspt[nbinspt+1];

    copy(binseta_.begin(), binseta_.end(), binseta);
    copy(binspt_.begin(), binspt_.end(), binspt);
    TString MCadd="";
    if(isMC) MCadd="MC";

    
    //  TDirectory * histdir=new TDirectory(mode+MCadd);
    //  histdir->cd();

    vector<float> dphi;

    for(float i=0;i<=10;i++){
      dphi.push_back(3.1415 * i/10);
    }

    container1D c_pteff  = container1D(binspt_);
    container1D c_etaeff = container1D(binseta_);
    container1D c_selpt  = container1D(binspt_);
    container1D c_seleta = container1D(binseta_);
    container1D c_trigpt = container1D(binspt_);
    container1D c_trigeta= container1D(binseta_);
    container1D c_selmettrigpt  = container1D(binspt_);
    container1D c_selmettrigeta = container1D(binseta_);
    container1D c_selbothtrigpt = container1D(binspt_);
    container1D c_selbothtrigeta= container1D(binseta_);
    container1D c_corrpt  = container1D(binspt_);
    container1D c_correta = container1D(binseta_);

    container1D c_dphieff = container1D(dphi);
    container1D c_seldphi = container1D(dphi);
    container1D c_trigdphi = container1D(dphi);
    container1D c_selmettrigdphi = container1D(dphi);
    container1D c_selbothtrigdphi = container1D(dphi);
    container1D c_corrdphi = container1D(dphi);


  TFile f1(inputFile);
  TFile f2(pufile_);
  TH1D datapileup=*((TH1D*)f2.Get("pileup"));

  // create tree
  TDirectory * d= (TDirectory*) f1.Get(directoryname); //just the TDirectory, the tree is in
  TTree *t1 = (TTree*)d->Get(treename); //tree name


  //set trigger paths for 5E33 and 7E33 with upper run no cut

  // recheck trigger paths!!!!!!

  vector<pair<string, double> >dileptriggers;

  if(mode=="ee"){
    pair<string, double> trig1("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v",193805.);
    dileptriggers.push_back(trig1);
    pair<string, double> trig2("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v",999999.);
    dileptriggers.push_back(trig2);

  }
  if(mode=="mumu"){
    pair<string, double> trig1("HLT_Mu17_Mu8_v",193805);
    dileptriggers.push_back(trig1);
    pair<string, double> trig2("HLT_Mu17_TkMu8_v",193805);
    dileptriggers.push_back(trig2);
    pair<string, double> trig3("HLT_Mu17_Mu8_v",999999);
    dileptriggers.push_back(trig3);
    pair<string, double> trig4("HLT_Mu17_TkMu8_v",999999);
    dileptriggers.push_back(trig4);
  }

  if(mode=="emu"){
    pair<string, double> trig1("HLT_Mu17_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v",193805);
    dileptriggers.push_back(trig1);
    pair<string, double> trig2("HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v",193805);
    dileptriggers.push_back(trig2);
    pair<string, double> trig3("HLT_Mu17_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v",999999);
    dileptriggers.push_back(trig3);
    pair<string, double> trig4("HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v",999999);
    dileptriggers.push_back(trig4);
  }

  vector<string> dileptriggersMC;
  if(!is52v9){
    if(mode=="ee"){
      dileptriggersMC.push_back("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v15");
    }
    if(mode=="mumu"){
      dileptriggersMC.push_back("HLT_Mu17_Mu8_v16");
      dileptriggersMC.push_back("HLT_Mu17_TkMu8_v9");
    }
    if(mode=="emu"){
      dileptriggersMC.push_back("HLT_Mu17_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v4");
      dileptriggersMC.push_back("HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v4");
    }
  }
  else{
    if(mode=="ee"){
      dileptriggersMC.push_back("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v17"); //17
    }
    if(mode=="mumu"){
      dileptriggersMC.push_back("HLT_Mu17_Mu8_v16 ");
      dileptriggersMC.push_back("HLT_Mu17_TkMu8_v9");
    }
    if(mode=="emu"){
      dileptriggersMC.push_back("HLT_Mu17_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v6");
      dileptriggersMC.push_back("HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v6");
    }

  }


  

  ///////////PU reweighting
  
  PUReweighter PUweight;
  if(isMC) PUweight.setDataTruePUInput(datapileup);
  if(isMC)PUweight.setMCDistrSum12();

  vector<NTMuon> * pMuons = 0;
  t1->SetBranchAddress("NTMuons",&pMuons); 
  vector<NTElectron> * pElectrons = 0;
  t1->SetBranchAddress("NTElectrons",&pElectrons); 
  vector<NTJet> * pJets = 0;
  t1->SetBranchAddress("NTJets",&pJets); 
  NTMet * pMet = 0;
  t1->SetBranchAddress("NTMet",&pMet); 
  NTEvent * pEvent = 0;
  t1->SetBranchAddress("NTEvent",&pEvent); 

  float n = (int) t1->GetEntries(); //299569 for ttbaree
  if(maxEntries!=0)
    n=min(maxEntries,n);
  cout << n << endl;


  pair<string,double> dilepton("dilepton", 0);
  pair<string,double> ZVeto   ("ZVeto   ", 0);
  pair<string,double> oneJet  ("oneJet  ", 0);
  pair<string,double> twoJets ("twoJets ", 0);
  pair<string,double> met     ("met     ", 0);


  vector<pair<string,double> >sel_woTrig;
  vector<pair<string,double> >sel_Trig;
  vector<pair<string,double> >sel_MetTrig;
  vector<pair<string,double> >sel_BothTrig;

  sel_woTrig.push_back(dilepton);
  sel_woTrig.push_back(ZVeto);
  sel_woTrig.push_back(oneJet);
  sel_woTrig.push_back(twoJets);
  sel_woTrig.push_back(met);

  sel_Trig.push_back(dilepton);
  sel_Trig.push_back(ZVeto);
  sel_Trig.push_back(oneJet);
  sel_Trig.push_back(twoJets);
  sel_Trig.push_back(met);

  sel_MetTrig.push_back(dilepton);
  sel_MetTrig.push_back(ZVeto);
  sel_MetTrig.push_back(oneJet);
  sel_MetTrig.push_back(twoJets);
  sel_MetTrig.push_back(met);

  sel_BothTrig.push_back(dilepton);
  sel_BothTrig.push_back(ZVeto);
  sel_BothTrig.push_back(oneJet);
  sel_BothTrig.push_back(twoJets);
  sel_BothTrig.push_back(met);

  vector<pair<TString, int> >triggersummary;

  for(float i=0;i<n;i++){  //main loop

    t1->GetEntry(i);
    int entr=i;
    int nentr=n;
    if((100* entr % nentr)){
      cout << "\rprocessing: " << (int) 100*i/n << "%";
      fflush(stdin);
    }
    
   //  if(!isMC){

//       if(pEvent->runNo() < 193336) continue;
//     }

    bool firedDilepTrigger=false;
    bool firedMet=false;
    bool b_dilepton=false;
    bool b_ZVeto=false;
    bool b_oneJet=false;
    bool b_twoJets=false;
    bool b_met=false;

    //limit to 2.45fb^-1
    //if(!isMC && pEvent->runNo() > 195016) continue;
    //cout << pEvent->runNo() << endl;

    double puweight=1;
    if(isMC&&doPUweight) puweight=PUweight.getPUweight(pEvent->truePU());

    vector<string> trigs;
    trigs=pEvent->firedTriggers();
    for(unsigned int trigit=0;trigit<trigs.size();trigit++){
      TString trig=trigs[trigit];
      bool newtrigger=false;
      if(trig.Contains("MET")){
	newtrigger=true;
	for(unsigned int J=0; J<triggersummary.size();J++){
	  if(triggersummary[J].first == trig){
	    newtrigger=false;
	    triggersummary[J].second++;
	  }
	}
      }
      pair<TString, int> temppair;
      temppair.first = trig; temppair.second = 1;
      if(newtrigger) triggersummary.push_back(temppair);

      if(isMC){
	for(unsigned int ctrig=0;ctrig<dileptriggersMC.size(); ctrig++){
	  if(trig.Contains(dileptriggersMC[ctrig])){
	    firedDilepTrigger=true;
	    break;
	  }
	}
	for(unsigned int ctrig=0;ctrig<mettriggersMC.size(); ctrig++){
	  if(trig.Contains(mettriggersMC[ctrig])){
	    firedMet=true;
	    break;
	  }
	}
      }
      else{
	for(unsigned int ctrig=0;ctrig<dileptriggers.size(); ctrig++){
	  if(trig.Contains(dileptriggers[ctrig].first) && pEvent->runNo() < dileptriggers[ctrig].second){
	    firedDilepTrigger=true;
	    break;
	  }
	}
	for(unsigned int ctrig=0;ctrig<mettriggers.size(); ctrig++){
	  if(trig.Contains(mettriggers[ctrig])){
	    firedMet=true;
	    break;
	  }
	}
      }
    }
    if(!isMC && !firedMet) continue;

    // lepton selection and event selection (at least two leptons etc)

    if(pEvent->vertexMulti() <1) continue;
    if(mode=="ee" && pElectrons->size()<2) continue;
    if(mode=="mumu" && pMuons->size()<2) continue;
    vector<string> ids;
    vector<NTMuon> selectedMuons;
    for(vector<top::NTMuon>::iterator muon=pMuons->begin();muon<pMuons->end();muon++){
      if(!(fabs(muon->eta())<2.4) ) continue;
      if(!(muon->pt() >20))   continue;
      if(!(muon->isGlobal() || muon->isTracker()) ) continue;
      //if(muon->trkHits()<3) continue;
      //if(muon->trkHits() <= 5 ) continue;
      //if(muon->muonHits() <1) continue;
      //if(muon->dbs()> 0.02) continue;
      if(muon->isoVal04() > 0.2 ) continue;
      //if(muon->normChi2() >10) continue;
      selectedMuons.push_back(*muon);
    }
    if(mode == "mumu" && selectedMuons.size() < 2) continue;

    vector<NTElectron> selectedElecs;
    for(vector<NTElectron>::iterator elec=pElectrons->begin();elec<pElectrons->end();elec++){
      if(elec->pt()<20 ) continue;
      if(fabs(elec->eta())>2.5 ) continue;
      if(fabs(elec->dbs()) >0.04 ) continue;
      if(!(elec->isNotConv()) ) continue;
      if(elec->rhoIso03()>0.15 ) continue;
      if(CiCId && (0x00 == (0x01 & (int) elec->id("cicTightMC"))) ) continue; //for CiC bit test
      if(!CiCId && (elec->mvaId() < -0.1)) continue;
      bool isCleaned=true;
      //DeltaR cleaning against muons:
      for(vector<top::NTMuon>::iterator muon=pMuons->begin();muon<pMuons->end();muon++){
	if(!(muon->isGlobal())) continue;
	double dR=0.1;
	double Rsq=(muon->eta() - elec->eta())*(muon->eta() - elec->eta()) + (muon->phi()-elec->phi())*(muon->phi()-elec->phi());
	if(Rsq < dR*dR){
	  isCleaned=false;
	  break;
	}
      }
      if(!isCleaned) continue;

      selectedElecs.push_back(*elec);
    }
    double mass=0;
    if(mode=="ee"){
      if(selectedElecs.size() <2) continue;
      if(selectedElecs[0].q() == selectedElecs[1].q()) continue;
      mass=(selectedElecs[0].p4() + selectedElecs[1].p4()).M();
      if(mass > 20)
	b_dilepton=true;
    }

    if(mode=="mumu"){
      if(selectedMuons.size() < 2) continue;
      if(selectedMuons[0].q() == selectedMuons[1].q()) continue;
      mass=(selectedMuons[0].p4() + selectedMuons[1].p4()).M();
      if(mass > 20) 
	b_dilepton=true;
    }

    if(mode=="emu"){
      if(selectedMuons.size() < 1 || selectedElecs.size() < 1) continue;
      if(selectedMuons[0].q() * selectedElecs[0].q() == 1) continue;
      mass=(selectedMuons[0].p4() + selectedElecs[0].p4()).M();
      if(mass > 20) 
	b_dilepton=true;
    }



    ////////////////dilepton selection
    if((mode == "mumu" || mode=="ee" ) && (mass > 106 || mass < 76)) b_ZVeto=true;
    if(mode=="emu")  b_ZVeto=true;

    vector<NTJet> selected_jets;
    for(vector<NTJet>::iterator jet=pJets->begin();jet<pJets->end();jet++){
      if(jet->pt() < 30) continue;
      if(fabs(jet->eta()) >2.5) continue;
      double dR=0.3;
      //do cleaning
      bool cleanjet=true;
      for(vector<NTMuon>::iterator muon=selectedMuons.begin(); muon<selectedMuons.end(); muon++){
	double sqR=(muon->phi()-jet->phi())*(muon->phi()-jet->phi()) + (muon->eta()-jet->eta())*(muon->eta()-jet->eta());
	if(dR*dR > sqR){
	  cleanjet=false;
	  break;
	}
      }
      for(vector<NTElectron>::iterator electron=selectedElecs.begin(); electron<selectedElecs.end(); electron++){
	double sqR=(electron->phi()-jet->phi())*(electron->phi()-jet->phi()) + (electron->eta()-jet->eta())*(electron->eta()-jet->eta());
	if(dR*dR > sqR){
	  cleanjet=false;
	  break;
	}
      }
      if(!cleanjet) continue;
      if((!jet->id())) continue;
      //if(jet->btag() >-999) cout << jet->btag() << endl;

      selected_jets.push_back(*jet);
    }
    if(selected_jets.size() >0) b_oneJet=true;
    if(selected_jets.size() >1) b_twoJets=true;
    if((mode == "ee" || mode =="mumu") && pMet->met() > 40) b_met=true;
    if(mode == "emu") b_met=true;


    //  std::cout << puweight << std::endl;

    

    if(b_dilepton){
      sel_woTrig[0].second      +=puweight;
      if(mode=="ee"){
	c_selpt.fill(selectedElecs[0].pt(),puweight);
	c_selpt.fill(selectedElecs[1].pt(),puweight);
	c_seleta.fill(selectedElecs[0].eta(),puweight);
	c_seleta.fill(selectedElecs[1].eta(),puweight);
	c_seldphi.fill(fabs(selectedElecs[0].phi() - pMet->phi()));
      }
      else if(mode=="mumu"){
	c_selpt.fill(selectedMuons[0].pt(),puweight);
	c_selpt.fill(selectedMuons[1].pt(),puweight);
	c_seleta.fill(selectedMuons[0].eta(),puweight);
	c_seleta.fill(selectedMuons[1].eta(),puweight);
	c_seldphi.fill(fabs(selectedMuons[0].phi() - pMet->phi()));

      }
      else if(mode =="emu"){
	c_selpt.fill(selectedElecs[0].pt(),puweight);
	c_selpt.fill(selectedMuons[0].pt(),puweight);
	c_seleta.fill(selectedElecs[0].eta(),puweight);
	c_seleta.fill(selectedMuons[0].eta(),puweight);
	c_seldphi.fill(fabs(selectedElecs[0].phi() - pMet->phi()));

      }
    }
  if(b_dilepton && b_ZVeto)                                   sel_woTrig[1].second      +=puweight;
  if(b_dilepton && b_ZVeto && b_oneJet)                       sel_woTrig[2].second      +=puweight;
  if(b_dilepton && b_ZVeto && b_oneJet && b_twoJets)          sel_woTrig[3].second      +=puweight;
  if(b_dilepton && b_ZVeto && b_oneJet && b_twoJets && b_met) sel_woTrig[4].second      +=puweight;

  if(firedDilepTrigger){
    if(b_dilepton){
      sel_Trig[0].second  +=puweight;
      if(mode=="ee"){
	c_trigpt.fill(selectedElecs[0].pt(),puweight);
	c_trigpt.fill(selectedElecs[1].pt(),puweight);
	c_trigeta.fill(selectedElecs[0].eta(),puweight);
	c_trigeta.fill(selectedElecs[1].eta(),puweight);
	c_trigdphi.fill(fabs(selectedElecs[0].phi() - pMet->phi()));
      }
      else if(mode=="mumu"){
	c_trigpt.fill(selectedMuons[0].pt(),puweight);
	c_trigpt.fill(selectedMuons[1].pt(),puweight);
	c_trigeta.fill(selectedMuons[0].eta(),puweight);
	c_trigeta.fill(selectedMuons[1].eta(),puweight);
	c_trigdphi.fill(fabs(selectedMuons[0].phi() - pMet->phi()));

      }
      else if(mode =="emu"){
	c_trigpt.fill(selectedElecs[0].pt(),puweight);
	c_trigpt.fill(selectedMuons[0].pt(),puweight);
	c_trigeta.fill(selectedElecs[0].eta(),puweight);
	c_trigeta.fill(selectedMuons[0].eta(),puweight);
	c_trigdphi.fill(fabs(selectedElecs[0].phi() - pMet->phi()));

      }


    }                                            
    if(b_dilepton && b_ZVeto)                                   sel_Trig[1].second  +=puweight;
    if(b_dilepton && b_ZVeto && b_oneJet)                       sel_Trig[2].second +=puweight;
    if(b_dilepton && b_ZVeto && b_oneJet && b_twoJets)          sel_Trig[3].second +=puweight;
    if(b_dilepton && b_ZVeto && b_oneJet && b_twoJets && b_met) sel_Trig[4].second    +=puweight;
  }

  if(firedMet){
    if(b_dilepton){
      if(mode=="ee"){

	c_selmettrigpt.fill(selectedElecs[0].pt(),puweight);
	c_selmettrigpt.fill(selectedElecs[1].pt(),puweight);
	c_selmettrigeta.fill(selectedElecs[0].eta(),puweight);
	c_selmettrigeta.fill(selectedElecs[1].eta(),puweight);
	c_selmettrigdphi.fill(fabs(selectedElecs[0].phi() - pMet->phi()));
      }
      else if(mode=="mumu"){
	c_selmettrigpt.fill(selectedMuons[0].pt(),puweight);
	c_selmettrigpt.fill(selectedMuons[1].pt(),puweight);
	c_selmettrigeta.fill(selectedMuons[0].eta(),puweight);
	c_selmettrigeta.fill(selectedMuons[1].eta(),puweight);
	c_selmettrigdphi.fill(fabs(selectedMuons[0].phi() - pMet->phi()));

      }
      else if(mode =="emu"){
	c_selmettrigpt.fill(selectedElecs[0].pt(),puweight);
	c_selmettrigpt.fill(selectedMuons[0].pt(),puweight);
	c_selmettrigeta.fill(selectedElecs[0].eta(),puweight);
	c_selmettrigeta.fill(selectedMuons[0].eta(),puweight);
	c_selmettrigdphi.fill(fabs(selectedElecs[0].phi() - pMet->phi()));

      }
      sel_MetTrig[0].second  +=puweight;
    }
    if(b_dilepton && b_ZVeto)                                   sel_MetTrig[1].second  +=puweight;
    if(b_dilepton && b_ZVeto && b_oneJet)                       sel_MetTrig[2].second +=puweight;
    if(b_dilepton && b_ZVeto && b_oneJet && b_twoJets)          sel_MetTrig[3].second +=puweight;
    if(b_dilepton && b_ZVeto && b_oneJet && b_twoJets && b_met) sel_MetTrig[4].second    +=puweight;
  }

  if(firedMet && firedDilepTrigger){
    if(b_dilepton){
      if(mode=="ee"){
	c_selbothtrigpt.fill(selectedElecs[0].pt(),puweight);
	c_selbothtrigpt.fill(selectedElecs[1].pt(),puweight);
	c_selbothtrigeta.fill(selectedElecs[0].eta(),puweight);
	c_selbothtrigeta.fill(selectedElecs[1].eta(),puweight);
	c_selbothtrigdphi.fill(fabs(selectedElecs[0].phi() - pMet->phi()));
      }
      else if(mode=="mumu"){
	c_selbothtrigpt.fill(selectedMuons[0].pt(),puweight);
	c_selbothtrigpt.fill(selectedMuons[1].pt(),puweight);
	c_selbothtrigeta.fill(selectedMuons[0].eta(),puweight);
	c_selbothtrigeta.fill(selectedMuons[1].eta(),puweight);
	c_selbothtrigdphi.fill(fabs(selectedMuons[0].phi() - pMet->phi()));

      }
      else if(mode =="emu"){
	c_selbothtrigpt.fill(selectedElecs[0].pt(),puweight);
	c_selbothtrigpt.fill(selectedMuons[0].pt(),puweight);
	c_selbothtrigeta.fill(selectedElecs[0].eta(),puweight);
	c_selbothtrigeta.fill(selectedMuons[0].eta(),puweight);
	c_selbothtrigdphi.fill(fabs(selectedElecs[0].phi() - pMet->phi()));
      }

      sel_BothTrig[0].second  +=puweight;
    }
    if(b_dilepton && b_ZVeto)                                   sel_BothTrig[1].second  +=puweight;
    if(b_dilepton && b_ZVeto && b_oneJet)                       sel_BothTrig[2].second +=puweight;
    if(b_dilepton && b_ZVeto && b_oneJet && b_twoJets)          sel_BothTrig[3].second +=puweight;
    if(b_dilepton && b_ZVeto && b_oneJet && b_twoJets && b_met) sel_BothTrig[4].second    +=puweight;
  }



  }//eventloop
  cout << endl;

  string MCdata="data";

  if(isMC) MCdata="MC";
  cout << "\n\nIn channel " << mode << "   " << MCdata << endl;
  cout << "triggers: " <<endl;
  if(!isMC){
  for(unsigned int i=0;i<dileptriggers.size();i++){
    cout << dileptriggers[i].first << "   until runnumber "<< dileptriggers[i].second << endl;
  }
  }


  // set up output
  vector<double> output;

  for(unsigned int i=0; i< sel_woTrig.size();i++){
    double eff=sel_Trig[i].second / sel_woTrig[i].second;
    double erreff=sqrt(eff*(1-eff)/sel_Trig[i].second);

    double ratio= (eff * sel_MetTrig[i].second/sel_woTrig[i].second ) /(sel_BothTrig[i].second/ sel_woTrig[i].second) -1;
    //double ratiostat=sel_BothTrig[i].second ;

    output.push_back(eff);
    output.push_back(erreff);
    output.push_back(ratio);

    cout << "selected " << sel_woTrig[i].first << ":  \t" << sel_woTrig[i].second << "  vs. \t" << sel_Trig[i].second << "  \teff: " << eff  << " +- " << erreff << "\tCorrR-1: " <<  ratio << "\t stat: " << sel_BothTrig[i].second  <<endl;
    //if(i==0) output=effdeffR;

  }
  //output=effdeffR;

  if(makeTeXTable){
   
    if(!isMC) cout <<  '\n' << mode << " \t\t& $N_{sel}$ \t& $N_{sel}^{trig}$ \t& $\\epsilon_d$ \t& $\\delta\\epsilon$ (stat.) \\\\ \\hline" << endl;
    if(isMC) cout <<  '\n' << mode<< " MC" << " \t\t& $N_{sel}$ \t& $N_{sel}^{trig}$ \t& $\\epsilon_{MC}$  \t& $\\delta\\epsilon$ (stat.) \t&R$_c$ - 1 \\\\ \\hline" << endl;
    for(unsigned int i=0; i< sel_woTrig.size();i++){
      double eff=sel_Trig[i].second / sel_woTrig[i].second;
      double erreff=sqrt(eff*(1-eff)/sel_woTrig[i].second);
      double ratio= (eff * sel_MetTrig[i].second/sel_woTrig[i].second ) /(sel_BothTrig[i].second/ sel_woTrig[i].second) -1;
      cout.setf(ios::fixed,ios::floatfield);
      cout.precision(0);
      cout <<  sel_woTrig[i].first << "\t& " << sel_woTrig[i].second << "\t\t& " << sel_Trig[i].second << "\t\t\t& ";
      //cout.unsetf(ios::floatfield);
      cout.precision(3);
      if(isMC) cout << eff  << " \t& " << erreff << " \t& " << ratio << " \\\\" << endl;
      else cout << eff << " \t& " << erreff <<" \\\\" << endl;
    }
  }


  ////////////////////////////////////check MC triggers



  ///  triggersummary mettriggers
  int trigin=0;
  vector<unsigned int> notinc;
  for(unsigned int i=0; i<mettriggersMC.size(); i++){
    bool inc=false;
    for(unsigned int j=0; j<triggersummary.size();j++){
      //cout << mettriggers[i] << " vs " << triggersummary[j].first << endl;
      if(triggersummary[j].first.Contains(mettriggersMC[i])){
	trigin++;
	inc=true;
      break;
      }
    }
    if(!inc) notinc.push_back(i);
  }
  if(isMC) cout <<trigin << "  fraction of incorp MC triggers wrt datamettriggers: " << trigin/(double)mettriggers.size() << endl;


  c_pteff = c_trigpt / c_selpt;
  c_etaeff = c_trigeta / c_seleta;
  c_dphieff = c_trigdphi / c_seldphi;

  c_corrpt = (c_pteff * (c_selmettrigpt/c_selpt))/(c_selbothtrigpt/c_selpt);
  c_correta = (c_etaeff * (c_selmettrigeta/c_seleta))/(c_selbothtrigeta/c_seleta);
  c_corrdphi = (c_dphieff * (c_selmettrigdphi/c_seldphi))/(c_selbothtrigdphi/c_seldphi);

  etaeff_=c_etaeff;
  pteff_=c_pteff;
  dphieff_=c_dphieff;
  correta_=c_correta;
  corrpt_=c_corrpt;
  corrdphi_=c_corrdphi;

  etaeff_.setDivideBinomial(false);
  pteff_.setDivideBinomial(false);
  dphieff_.setDivideBinomial(false);

  return output;

}

//Eff(string mode="ee", bool doPUweight=true, const char * inputFile="  ", bool isMC=true){

  void writeAll(){

    TString add="";
    if(isMC) add="MC";
    etaeff_.writeTGraph("eta eff"+add);
    etaeff_.writeTH1D("axis eta");

    pteff_.writeTGraph("pt eff"+add);
    pteff_.writeTH1D("axis pt");

    dphieff_.writeTGraph("dphi eff"+add);
    dphieff_.writeTH1D("axis dphi");

    correta_.writeTGraph("correta"+add);

    correta_.writeTGraph("corrpt"+add);

    corrdphi_.writeTGraph("corrdphi"+add);

  }


private:
  std::vector<float> binseta_;
  std::vector<float> binspt_;
  top::container1D etaeff_;
  top::container1D pteff_;
  top::container1D correta_;
  top::container1D corrpt_;
  top::container1D dphieff_;
  top::container1D corrdphi_;
  bool isMC;
};


void analyze(){

  bool getTeX=true;

  using namespace std;
  using namespace top;

  std::vector<float> binsmumueta;
  binsmumueta.push_back(-2.4);binsmumueta.push_back(-2.1);binsmumueta.push_back(-1.2);binsmumueta.push_back(-0.9);binsmumueta.push_back(0.9);binsmumueta.push_back(1.2);binsmumueta.push_back(2.1);binsmumueta.push_back(2.4);

  triggerAnalyzer ta_eed;
  triggerAnalyzer ta_mumud;
  ta_mumud.setBinsEta(binsmumueta);
  triggerAnalyzer ta_emud;
  triggerAnalyzer ta_eeMC, ta_mumuMC, ta_emuMC;
  ta_mumuMC.setBinsEta(binsmumueta);

  const char * datafile = "/scratch/hh/current/cms/user/kieseler/2012/trees0704/metData_23_06.root";

  const char * pileuproot = "/afs/naf.desy.de/user/k/kieseler/public/dataPileUp_23_06.root";


  vector<double> eed=ta_eed.Eff("ee",    datafile,pileuproot, false,getTeX);
  vector<double> mumud=ta_mumud.Eff("mumu",  datafile,pileuproot, false,getTeX);
  vector<double> emud= ta_emud.Eff("emu",   datafile,pileuproot, false,getTeX);

  
  
  
  vector<double> eeMC=ta_eeMC.Eff("ee",    "/scratch/hh/current/cms/user/kieseler/2012/trees0704/tree_eettbar.root.root",pileuproot,true,getTeX);
  
  vector<double> mumuMC=ta_mumuMC.Eff("mumu",  "/scratch/hh/current/cms/user/kieseler/2012/trees0704/tree_mumuttbar.root.root",pileuproot,true,getTeX);
  vector<double> emuMC=ta_emuMC.Eff("emu",   "/scratch/hh/current/cms/user/kieseler/2012/trees0704/tree_emuttbar.root.root",pileuproot,true,getTeX);


  cout.precision(3);
  // get correction factors
 

  cout << "\n\n TeX table:" <<endl;
  cout << "$ee$ & " << eed[0] <<" $\\pm$ " << eed[1] << " (stat.) & " 
       << eeMC[0] << " $\\pm$ " << eeMC[1] << " (stat.) & " << eed[0]/eeMC[0] 
       << " $\\pm$ " << sqrt((eed[1]/eeMC[0])*(eed[1]/eeMC[0]) + (eed[0]/(eeMC[0]*eeMC[0]))*eeMC[1]*eeMC[1]*(eed[0]/(eeMC[0]*eeMC[0]))) << " (stat.)  " 
       <<  " $\\pm$ " << sqrt(0.01*eed[0]/eeMC[0]*eed[0]/eeMC[0]*0.01  +fabs(ratiomultiplier*eed[0]/eeMC[0] * eeMC[2])*fabs(ratiomultiplier*eed[0]/eeMC[0] * eeMC[2])) << " (syst.) \\\\" << endl;

cout << "$\\mu\\mu$ & " << mumud[0] <<" $\\pm$ " << mumud[1] << " (stat.) & " 
       << mumuMC[0] << " $\\pm$ " << mumuMC[1] << " (stat.) & " << mumud[0]/mumuMC[0] 
       << " $\\pm$ " << sqrt((mumud[1]/mumuMC[0])*(mumud[1]/mumuMC[0]) + (mumud[0]/(mumuMC[0]*mumuMC[0]))*mumuMC[1]*mumuMC[1]*(mumud[0]/(mumuMC[0]*mumuMC[0]))) << " (stat.)  " 
     <<  " $\\pm$ " << sqrt(0.01*mumud[0]/mumuMC[0]*mumud[0]/mumuMC[0]*0.01+fabs(ratiomultiplier*mumud[0]/mumuMC[0] * mumuMC[2])*fabs(ratiomultiplier*mumud[0]/mumuMC[0] * mumuMC[2])) << " (syst.) \\\\" << endl;

cout << "$e\\mu$ & " << emud[0] <<" $\\pm$ " << emud[1] << " (stat.) & " 
       << emuMC[0] << " $\\pm$ " << emuMC[1] << " (stat.) & " << emud[0]/emuMC[0] 
       << " $\\pm$ " << sqrt((emud[1]/emuMC[0])*(emud[1]/emuMC[0]) + (emud[0]/(emuMC[0]*emuMC[0]))*emuMC[1]*emuMC[1]*(emud[0]/(emuMC[0]*emuMC[0]))) << " (stat.)  " 
     <<  " $\\pm$ " << sqrt(0.01*emud[0]/emuMC[0]*emud[0]/emuMC[0]*0.01+fabs(ratiomultiplier*emud[0]/emuMC[0] * emuMC[2])*fabs(ratiomultiplier*emud[0]/emuMC[0] * emuMC[2])) << " (syst.) \\\\" << endl;


 cout << "\n\n" << "channel & $ee$ & $\\mu\\mu$ & $e\\mu$ \\\\ \\hline" <<endl;

 cout.precision(3);
 cout.setf(ios::fixed,ios::floatfield);
 
 //make summarizing table
 for(unsigned int i=0; i<eed.size();i++){
   TString step;
   if(i==0) step="dilepton";
   if(i==3) step="Z-Veto";
   if(i==6) step="$\\geq1$jet";
   if(i==9) step="$\\geq2$jets";
   if(i==12) step="MET";
   cout << step << " & " << eed[i]/eeMC[i] << " $\\pm$ " << sqrt((eed[i+1]/eeMC[i])*(eed[1]/eeMC[i]) + (eed[i]/(eeMC[i]*eeMC[i]))*eeMC[i+1]*eeMC[i+1]*(eed[i]/(eeMC[i]*eeMC[i])) + fabs(ratiomultiplier*eed[i]/eeMC[i] * eeMC[i+2])*fabs(ratiomultiplier*eed[i]/eeMC[i] * eeMC[i+2]) + 0.01*0.01*eed[i]/eeMC[i]*eed[i]/eeMC[i]) << " & "
	<< mumud[i]/mumuMC[i] << " $\\pm$ " << sqrt((mumud[i+1]/mumuMC[i])*(mumud[1]/mumuMC[i]) + (mumud[i]/(mumuMC[i]*mumuMC[i]))*mumuMC[i+1]*mumuMC[i+1]*(mumud[i]/(mumuMC[i]*mumuMC[i])) + fabs(ratiomultiplier*mumud[i]/mumuMC[i] * mumuMC[i+2])*fabs(ratiomultiplier*mumud[i]/mumuMC[i] * mumuMC[i+2]) + 0.01*0.01*mumud[i]/mumuMC[i]*mumud[i]/mumuMC[i] ) << "  & "
	<< emud[i]/emuMC[i] << " $\\pm$ " << sqrt((emud[i+1]/emuMC[i])*(emud[1]/emuMC[i]) + (emud[i]/(emuMC[i]*emuMC[i]))*emuMC[i+1]*emuMC[i+1]*(emud[i]/(emuMC[i]*emuMC[i])) + fabs(ratiomultiplier*emud[i]/emuMC[i] * emuMC[i+2])*fabs(ratiomultiplier*emud[i]/emuMC[i] * emuMC[i+2]) + 0.01*0.01*emud[i]/emuMC[i]*emud[i]/emuMC[i]) << " \\\\" << endl;
   i++;i++;
 }




//   container1D getEtaPlot(){return etaeff_;}
//   container1D getPtPlot(){return pteff_;}
//   container1D getCorrelationPt(){return corrpt_;}
//   container1D getCorrelationEta(){return correta_;}






  container1D scalefactor;
  container1D data;
  container1D MC;


  TFile* f5 = new TFile("test_triggerplots/triggerSummaryEE.root","RECREATE");

  ta_eed.writeAll();
  ta_eeMC.writeAll();
  data=ta_eed.getEtaPlot();
  MC=ta_eeMC.getEtaPlot();
  scalefactor=data/MC;
  scalefactor.addGlobalRelError(0.01);
  scalefactor.writeTGraph("scalefactor eta");
  scalefactor.addErrorContainer(scalefactor*ta_eeMC.getCorrelationEta(),ratiomultiplier);
  scalefactor.writeTGraph("scalefactor eta incl corrErr");

  data=ta_eed.getPtPlot();
  MC=ta_eeMC.getPtPlot();
  scalefactor=data/MC;
  scalefactor.addGlobalRelError(0.01);
  scalefactor.writeTGraph("scalefactor pt");
  scalefactor.addErrorContainer(scalefactor*ta_eeMC.getCorrelationPt(),ratiomultiplier);
  scalefactor.writeTGraph("scalefactor pt incl corrErr");

  data=ta_eed.getDPhiPlot();
  MC=ta_eeMC.getDPhiPlot();
  scalefactor=data/MC;
  scalefactor.addGlobalRelError(0.01);
  scalefactor.writeTGraph("scalefactor dphi");
  scalefactor.addErrorContainer(scalefactor*ta_eeMC.getCorrelationDPhi(),ratiomultiplier);
  scalefactor.writeTGraph("scalefactor dphi incl corrErr");

  f5->Close();

  
  TFile* f6 = new TFile("test_triggerplots/triggerSummaryMUMU.root","RECREATE");ta_mumud.writeAll();

  ta_mumud.writeAll();
  ta_mumuMC.writeAll();
  data=ta_mumud.getEtaPlot();
  MC=ta_mumuMC.getEtaPlot();
  scalefactor=data/MC;
  scalefactor.addGlobalRelError(0.01);
  scalefactor.writeTGraph("scalefactor eta");
  scalefactor.addErrorContainer(scalefactor*ta_mumuMC.getCorrelationEta(),ratiomultiplier);
  scalefactor.writeTGraph("scalefactor eta incl corrErr");

  data=ta_mumud.getPtPlot();
  MC=ta_mumuMC.getPtPlot();
  scalefactor=data/MC;
  scalefactor.addGlobalRelError(0.01);
  scalefactor.writeTGraph("scalefactor pt");
  scalefactor.addErrorContainer(scalefactor*ta_mumuMC.getCorrelationPt(),ratiomultiplier);
  scalefactor.writeTGraph("scalefactor pt incl corrErr");

  data=ta_mumud.getDPhiPlot();
  MC=ta_mumuMC.getDPhiPlot();
  scalefactor=data/MC;
  scalefactor.addGlobalRelError(0.01);
  scalefactor.writeTGraph("scalefactor dphi");
  scalefactor.addErrorContainer(scalefactor*ta_mumuMC.getCorrelationDPhi(),ratiomultiplier);
  scalefactor.writeTGraph("scalefactor dphi incl corrErr");

  f6->Close();

  TFile* f7 = new TFile("test_triggerplots/triggerSummaryEMU.root","RECREATE");

  ta_emud.writeAll();
  ta_emuMC.writeAll();
  data=ta_emud.getEtaPlot();
  MC=ta_emuMC.getEtaPlot();
  scalefactor=data/MC;
  scalefactor.addGlobalRelError(0.01);
  scalefactor.writeTGraph("scalefactor eta");
  scalefactor.addErrorContainer(scalefactor*ta_emuMC.getCorrelationEta(),ratiomultiplier);
  scalefactor.writeTGraph("scalefactor eta incl corrErr");

  data=ta_emud.getPtPlot();
  MC=ta_emuMC.getPtPlot();
  scalefactor=data/MC;
  scalefactor.addGlobalRelError(0.01);
  scalefactor.writeTGraph("scalefactor pt");
  scalefactor.addErrorContainer(scalefactor*ta_emuMC.getCorrelationPt(),ratiomultiplier);
  scalefactor.writeTGraph("scalefactor pt incl corrErr");

  data=ta_emud.getDPhiPlot();
  MC=ta_emuMC.getDPhiPlot();
  scalefactor=data/MC;
  scalefactor.addGlobalRelError(0.01);
  scalefactor.writeTGraph("scalefactor dphi");
  scalefactor.addErrorContainer(scalefactor*ta_emuMC.getCorrelationDPhi(),ratiomultiplier);
  scalefactor.writeTGraph("scalefactor dphi incl corrErr");

  f7->Close();

  
}

void do_triggerAnalyzer(){
  analyze();
  miniscript("5fbTrig/"); //makes plots nice and puts output to directory
}
