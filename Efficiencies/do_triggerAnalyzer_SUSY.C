#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "WbWbXAnalysis/DataFormats/interface/NTInflatedMuon.h"
#include "WbWbXAnalysis/DataFormats/interface/NTInflatedElectron.h"
#include "WbWbXAnalysis/DataFormats/interface/NTEvent.h"
#include "WbWbXAnalysis/DataFormats/interface/NTJet.h"
#include "WbWbXAnalysis/DataFormats/interface/NTMet.h"
#include "WbWbXAnalysis/plugins/reweightPU.h"
#include <vector>
#include <algorithm>
#include <string>
#include <utility>
#include "TCanvas.h"
#include "TROOT.h"
#include "TChain.h"
#include "makeplotsnice.h"
#include "WbWbXAnalysis/Tools/interface/histo1D.h"
#include "WbWbXAnalysis/Tools/interface/miscUtils.h"
#include <map>
#include <fstream>

using namespace std;

//ratiomulti affects plots AND syst error in tables
// NOOOOO relative paths!!


// add massrange check for the susy samples. make accessable from outside -> plot in different directories (create by system(mkdir))



double ratiomultiplier=0;
TString whichelectrons="NTInflatedElectrons";

//bool useRhoIso=false;

double jetptcut=30;

bool breakat5fb=true;
bool checktriggerpaths=false;
bool coutalltriggerpaths=false;

bool showstatusbar=true;

class triggerAnalyzer{

public:
  triggerAnalyzer(){
    binseta_.push_back(-2.5);binseta_.push_back(-1.5);binseta_.push_back(-0.8);binseta_.push_back(0.8);binseta_.push_back(1.5);binseta_.push_back(2.5); // ee standard
    binspt_ << 10 <<  20  << 30  << 40 << 50 << 60 << 100 << 200;
    binseta2dx_ << 0 << 0.9 << 1.2 << 2.1 << 2.4; //mu standard
    binseta2dy_=binseta2dx_;

    TH1::AddDirectory(kFALSE);
     };
  ~triggerAnalyzer(){};
  void setBinsEta(std::vector<float> binseta){binseta_.clear();binseta_=binseta;};
  void setBinsEta2dX(std::vector<float> binseta2dx){binseta2dx_.clear();binseta2dx_=binseta2dx;};
  void setBinsEta2dY(std::vector<float> binseta2dy){binseta2dy_.clear();binseta2dy_=binseta2dy;};
  void setBinsPt(std::vector<float> binspt){binspt_.clear();binspt_=binspt;}

  top::histo1D  getEtaPlot(){return etaeff_;}
  top::histo1D  getPtPlot(){return pteff_;}
  top::histo1D  getCorrelationPt(){return corrpt_;}
  top::histo1D  getCorrelationEta(){return correta_;}
  top::histo1D getDPhiPlot(){return dphieff_;}
  top::histo1D getVmultiPlot(){return vmultieff_;}
  top::histo1D getJetmultiPlot(){return jetmultieff_;}
  top::histo1D getDPhiPlot2(){return dphieff2_;}
  top::histo1D getCorrelationDPhi(){return corrdphi_;}
  top::histo1D getCorrelationDPhi2(){return corrdphi2_;}
  top::histo1D getCorrelationVmulti(){return corrvmulti_;}
  top::histo1D getCorrelationJetmulti(){return corrjetmulti_;}

  top::histo1D getDrlepPlot(){return drlepeff_;}
  top::histo1D getCorrelationDrlep(){return corrdrlep_;}

  TH2D getEta2D(){return eta_2dim;}



  vector<double> Eff(string mode="ee",  TChain * t1=0, const char * pufile_ =" ", bool IsMC=true, bool makeTeXTable=false){

  isMC=IsMC;

  using namespace top;
  using namespace std;

  std::vector<string> mettriggers;
  
  mettriggers.push_back("HLT_DiCentralPFJet50_PFMET80_v");
  mettriggers.push_back("HLT_MET120_HBHENoiseCleaned_v");
  mettriggers.push_back("HLT_PFHT350_PFMET100_v");
  mettriggers.push_back("HLT_PFHT400_PFMET100_v");
  mettriggers.push_back("HLT_MonoCentralPFJet80_PFMETnoMu95_NHEF0p95_v");
  mettriggers.push_back("HLT_MET120_v");
  mettriggers.push_back("HLT_DiCentralPFJet30_CaloMET50_dPhi1_PFMHT80_HBHENoiseFiltered_v");
  mettriggers.push_back("HLT_CentralPFJet80_CaloMET50_dPhi1_PFMHT80_HBHENoiseFiltered_v");
  mettriggers.push_back("HLT_DiCentralJet20_BTagIP_MET65_HBHENoiseFiltered_dPhi1_v");
  mettriggers.push_back("HLT_DiCentralJet20_CaloMET65_BTagCSV07_PFMHT80_v");
  mettriggers.push_back("HLT_MET80_Track50_dEdx3p6_v");
  mettriggers.push_back("HLT_MET80_Track60_dEdx3p7_v");
  mettriggers.push_back("HLT_MET200_v");
  mettriggers.push_back("HLT_MET200_HBHENoiseCleaned_v");
  mettriggers.push_back("HLT_MET300_v");
  mettriggers.push_back("HLT_MET300_HBHENoiseCleaned_v");
  mettriggers.push_back("HLT_PFMET150_v");
  mettriggers.push_back("HLT_DiPFJet40_PFMETnoMu65_MJJ800VBF_AllJets_v");
  mettriggers.push_back("HLT_DiPFJet40_PFMETnoMu65_MJJ600VBF_LeadingJets_v");
  mettriggers.push_back("HLT_PFMET180_v");
  mettriggers.push_back("HLT_MET80_v");
  mettriggers.push_back("HLT_Photon70_CaloIdXL_PFMET100_v");
  mettriggers.push_back("HLT_MonoCentralPFJet80_PFMETnoMu95_NHEF0p95_v");
  mettriggers.push_back("HLT_DiPFJet40_PFMETnoMu65_MJJ600VBF_LeadingJets_v");
  mettriggers.push_back("HLT_CentralPFJet80_CaloMET50_dPhi1_PFMHT80_HBHENoiseFiltered_v");
  mettriggers.push_back("HLT_DiCentralPFJet50_PFMET80_v");
  mettriggers.push_back("HLT_DiCentralPFJet30_CaloMET50_dPhi1_PFMHT80_HBHENoiseFiltered_v");
  mettriggers.push_back("HLT_PFMET150_v");
  mettriggers.push_back("HLT_PFMET180_v");
  mettriggers.push_back("HLT_DiCentralJet20_CaloMET65_BTagCSV07_PFMHT80_v");
  mettriggers.push_back("HLT_Photon70_CaloIdXL_PFMET100_v"); 
  mettriggers.push_back("HLT_PFHT350_PFMET100_v");
  mettriggers.push_back("HLT_PFHT400_PFMET100_v");
  mettriggers.push_back("HLT_DiPFJet40_PFMETnoMu65_MJJ800VBF_AllJets_v");
  mettriggers.push_back("HLT_MonoCentralPFJet80_PFMETnoMu95_NHEF0p95_v");
  mettriggers.push_back("HLT_DiCentralPFJet50_PFMET80_v");
  mettriggers.push_back("HLT_DiCentralPFJet30_CaloMET50_dPhi1_PFMHT80_HBHENoiseFiltered_v");
  mettriggers.push_back("HLT_CentralPFJet80_CaloMET50_dPhi1_PFMHT80_HBHENoiseFiltered_v");
  mettriggers.push_back("HLT_DiCentralJet20_CaloMET65_BTagCSV07_PFMHT80_v");
  mettriggers.push_back("HLT_MET80_Track50_dEdx3p6_v");
  mettriggers.push_back("HLT_MET80_Track60_dEdx3p7_v");
  mettriggers.push_back("HLT_Photon70_CaloIdXL_PFMET100_v");
  mettriggers.push_back("HLT_DiCentralJet20_BTagIP_MET65_HBHENoiseFiltered_dPhi1_v");
  mettriggers.push_back("HLT_PFHT350_PFMET100_v");
  mettriggers.push_back("HLT_MET120_HBHENoiseCleaned_v");
  mettriggers.push_back("HLT_DiPFJet40_PFMETnoMu65_MJJ600VBF_LeadingJets_v");
  mettriggers.push_back("HLT_PFMET150_v");
  mettriggers.push_back("HLT_MET120_v");
  mettriggers.push_back("HLT_MET200_v");
  mettriggers.push_back("HLT_MET200_HBHENoiseCleaned_v");
  mettriggers.push_back("HLT_MET300_v");
  mettriggers.push_back("HLT_MET300_HBHENoiseCleaned_v");
  mettriggers.push_back("HLT_MET400_v");
  mettriggers.push_back("HLT_MET400_HBHENoiseCleaned_v");
  mettriggers.push_back("HLT_PFHT400_PFMET100_v");
  mettriggers.push_back("HLT_PFMET180_v");
  mettriggers.push_back("HLT_DiPFJet40_PFMETnoMu65_MJJ800VBF_AllJets_v");
  mettriggers.push_back("HLT_MET80_v");
  mettriggers.push_back("HLT_MET400_v");
  mettriggers.push_back("HLT_MET400_HBHENoiseCleaned_v");
  mettriggers.push_back("HLT_PFHT350_PFMET100_v");
  mettriggers.push_back("HLT_PFHT400_PFMET100_v");
  mettriggers.push_back("HLT_DiCentralPFJet50_PFMET80_v");
  mettriggers.push_back("HLT_MonoCentralPFJet80_PFMETnoMu95_NHEF0p95_v");
  mettriggers.push_back("HLT_DiCentralPFJet30_PFMET80_BTagCSV07_v");
  mettriggers.push_back("HLT_DiPFJet40_PFMETnoMu65_MJJ600VBF_LeadingJets_v");
  mettriggers.push_back("HLT_DiCentralPFJet30_PFMET80_v");
  mettriggers.push_back("HLT_DiPFJet40_PFMETnoMu65_MJJ800VBF_AllJets_v");
  mettriggers.push_back("HLT_DiCentralJetSumpT100_dPhi05_DiCentralPFJet60_25_PFMET100_HBHENoiseCleaned_v");
  mettriggers.push_back("HLT_DisplacedPhoton65_CaloIdVL_IsoL_PFMET25_v");
  mettriggers.push_back("HLT_DisplacedPhoton65EBOnly_CaloIdVL_IsoL_PFMET30_v");
  


  //just for testing
  // mettriggers.clear();
  //mettriggers.push_back("HLT_MET120_v10");

  vector<string> notinMCtriggers;
  notinMCtriggers.push_back("HLT_DiCentralPFJet30_PFMET80_BTagCSV07_v");
notinMCtriggers.push_back("HLT_DiCentralPFJet30_PFMET80_v");
notinMCtriggers.push_back("HLT_DiCentralJetSumpT100_dPhi05_DiCentralPFJet60_25_PFMET100_HBHENoiseCleaned_v");
notinMCtriggers.push_back("HLT_DisplacedPhoton65_CaloIdVL_IsoL_PFMET25_v");
notinMCtriggers.push_back("HLT_DisplacedPhoton65EBOnly_CaloIdVL_IsoL_PFMET30_v");

 notinMCtriggers.push_back("HLT_DiCentralPFJet30_CaloMET50_dPhi1_PFMHT80_HBHENoiseFiltered_v");
notinMCtriggers.push_back("HLT_CentralPFJet80_CaloMET50_dPhi1_PFMHT80_HBHENoiseFiltered_v");
notinMCtriggers.push_back("HLT_DiCentralJet20_BTagIP_MET65_HBHENoiseFiltered_dPhi1_v");
notinMCtriggers.push_back("HLT_DiCentralJet20_CaloMET65_BTagCSV07_PFMHT80_v");
notinMCtriggers.push_back("HLT_CentralPFJet80_CaloMET50_dPhi1_PFMHT80_HBHENoiseFiltered_v");
notinMCtriggers.push_back("HLT_DiCentralPFJet30_CaloMET50_dPhi1_PFMHT80_HBHENoiseFiltered_v");
notinMCtriggers.push_back("HLT_DiCentralJet20_CaloMET65_BTagCSV07_PFMHT80_v");
notinMCtriggers.push_back("HLT_DiCentralPFJet30_CaloMET50_dPhi1_PFMHT80_HBHENoiseFiltered_v");
notinMCtriggers.push_back("HLT_CentralPFJet80_CaloMET50_dPhi1_PFMHT80_HBHENoiseFiltered_v");
notinMCtriggers.push_back("HLT_DiCentralJet20_CaloMET65_BTagCSV07_PFMHT80_v");
notinMCtriggers.push_back("HLT_DiCentralJet20_BTagIP_MET65_HBHENoiseFiltered_dPhi1_v");

 notinMCtriggers.push_back("DiCentralJet");  //get rid of all dijet stuff
notinMCtriggers.push_back("DiCentralPFJet");

 for(unsigned int j=0; j< mettriggers.size();j++){
   for(unsigned int i=0;i<notinMCtriggers.size();i++){
     if( ((TString)mettriggers[j]).Contains(notinMCtriggers[i]) ){
       mettriggers[j]="notrig";
     }
   }
 }

 if(checktriggerpaths){
   cout << "used MET triggers: " << endl;
   for(unsigned int i=0;i<mettriggers.size();i++){
     if(mettriggers.at(i) != "notrig") cout << mettriggers.at(i) << endl;
   }
 }

 // mettriggers=notinMCtriggers;
  std::vector<string> mettriggersMC;
 

  mettriggersMC=mettriggers;


  //////////////////////////////////////////////////////////////////////

  //inputFile="/scratch/hh/current/cms/user/kieseler/2012/trees0525/tree_mumu_mumu_allmodes_met_ttbar.root.root";
  //inputFile="/scratch/hh/current/cms/user/kieseler/2012/trees0525/tree_mumu_mumu_allmodes_met_runB_prompt.root.root";
  // const char * directoryname="TestTreePF";
  // const char * treename="pfTree";

  //new naming
  //  directoryname="PFTree";
  

  //mode="ee";
  //isMC=true;
  // bool is52v9=false; //OBSOLETE
  bool CiCId=false;
  // float maxEntries=0;
  bool doPUweight=true;

  //////////////////////////////////


  Int_t nbinseta2dx=binseta2dx_.size()-1;
  Int_t nbinseta2dy=binseta2dy_.size()-1;
  Int_t nbinspt=binspt_.size()-1; 
  Double_t binseta2dx[nbinseta2dx+1];
  Double_t binseta2dy[nbinseta2dy+1];
    Double_t binspt[nbinspt+1];

    copy(binseta2dx_.begin(), binseta2dx_.end(), binseta2dx);
    copy(binseta2dy_.begin(), binseta2dy_.end(), binseta2dy);
    copy(binspt_.begin(), binspt_.end(), binspt);
    TString MCadd="";
    if(isMC) MCadd="MC";

    eta_2dim =  TH2D("eta2d", "eta2d", nbinseta2dx, binseta2dx, nbinseta2dy, binseta2dy);
    TH2D eta_2dimsel =  TH2D("eta2dsel", "eta2dsel", nbinseta2dx, binseta2dx, nbinseta2dy, binseta2dy);
    TH2D eta_2dimtrig =  TH2D("eta2dtrig", "eta2dtrig", nbinseta2dx, binseta2dx, nbinseta2dy, binseta2dy);

    //  TDirectory * histdir=new TDirectory(mode+MCadd);
    //  histdir->cd();

    vector<float> dphi,vmulti,jetmulti,drlepbins;

    for(float i=0;i<=10;i++){
      dphi.push_back(2*3.1415 * i/10);
    }
    for(float i=0.5;i<=30.5;i++) vmulti << i;
    for(float i=-0.5;i<=7.5;i++) jetmulti << i;
    for(float i=0 ;i<15 ;i++) drlepbins << 2 * i /15;

    histo1D c_pteff  = histo1D(binspt_);
    histo1D c_etaeff = histo1D(binseta_);
    histo1D c_selpt  = histo1D(binspt_);
    histo1D c_seleta = histo1D(binseta_);
    histo1D c_trigpt = histo1D(binspt_);
    histo1D c_trigeta= histo1D(binseta_);
    histo1D c_selmettrigpt  = histo1D(binspt_);
    histo1D c_selmettrigeta = histo1D(binseta_);
    histo1D c_selbothtrigpt = histo1D(binspt_);
    histo1D c_selbothtrigeta= histo1D(binseta_);
    histo1D c_corrpt  = histo1D(binspt_);
    histo1D c_correta = histo1D(binseta_);

    histo1D c_dphieff = histo1D(dphi);
    histo1D c_seldphi = histo1D(dphi);
    histo1D c_trigdphi = histo1D(dphi);
    histo1D c_selmettrigdphi = histo1D(dphi);
    histo1D c_selbothtrigdphi = histo1D(dphi);
    histo1D c_corrdphi = histo1D(dphi);

    histo1D c_vmultieff = histo1D(vmulti);
    histo1D c_selvmulti = histo1D(vmulti);
    histo1D c_trigvmulti = histo1D(vmulti);
    histo1D c_selmettrigvmulti = histo1D(vmulti);
    histo1D c_selbothtrigvmulti = histo1D(vmulti);
    histo1D c_corrvmulti = histo1D(vmulti);

    histo1D c_drlepeff = histo1D(drlepbins);
    histo1D c_seldrlep = histo1D(drlepbins);
    histo1D c_trigdrlep = histo1D(drlepbins);
    histo1D c_selmettrigdrlep = histo1D(drlepbins);
    histo1D c_selbothtrigdrlep = histo1D(drlepbins);
    histo1D c_corrdrlep = histo1D(drlepbins);


    histo1D c_jetmultieff = histo1D(jetmulti);
    histo1D c_seljetmulti = histo1D(jetmulti);
    histo1D c_trigjetmulti = histo1D(jetmulti);
    histo1D c_selmettrigjetmulti = histo1D(jetmulti);
    histo1D c_selbothtrigjetmulti = histo1D(jetmulti);
    histo1D c_corrjetmulti = histo1D(jetmulti);

    histo1D c_dphieff2 = histo1D(dphi);
    histo1D c_seldphi2 = histo1D(dphi);
    histo1D c_trigdphi2 = histo1D(dphi);
    histo1D c_selmettrigdphi2 = histo1D(dphi);
    histo1D c_selbothtrigdphi2 = histo1D(dphi);
    histo1D c_corrdphi2 = histo1D(dphi);

    //  TFile f1(inputFile);
  TFile f2(pufile_);
  TH1D datapileup=*((TH1D*)f2.Get("pileup"));

  // create tree
  //  TDirectory * d= (TDirectory*) f1.Get(directoryname); //just the TDirectory, the tree is in
  //  TTree *t1 = (TTree*)d->Get(treename); //tree name


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

  vector<string> dileptriggersMC; // the version numbers where set as wildcards, so if statement obsolete!
  // if(!is52v9){
    if(mode=="ee"){
      dileptriggersMC.push_back("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v");
    }
    if(mode=="mumu"){
      dileptriggersMC.push_back("HLT_Mu17_Mu8_v");
      dileptriggersMC.push_back("HLT_Mu17_TkMu8_v");
    }
    if(mode=="emu"){
      dileptriggersMC.push_back("HLT_Mu17_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v");
      dileptriggersMC.push_back("HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v");
    }
    //  }
  /*  else{
    if(mode=="ee"){
      dileptriggersMC.push_back("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v"); //17
    }
    if(mode=="mumu"){
      dileptriggersMC.push_back("HLT_Mu17_Mu8_v");
      dileptriggersMC.push_back("HLT_Mu17_TkMu8_v");
    }
    if(mode=="emu"){
      dileptriggersMC.push_back("HLT_Mu17_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v");
      dileptriggersMC.push_back("HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v");
    }

    } */


  

  ///////////PU reweighting
  
  PUReweighter PUweight;
  if(isMC) PUweight.setDataTruePUInput(datapileup);
  if(isMC)PUweight.setMCDistrSum12("S07"); //for SUSY

  vector<NTInflatedMuon> * pMuons = 0;
  t1->SetBranchAddress("NTInflatedMuons",&pMuons); 
  vector<NTInflatedElectron> * pElectrons = 0;
  t1->SetBranchAddress(whichelectrons,&pElectrons); 
  vector<NTJet> * pJets = 0;
  t1->SetBranchAddress("NTJets",&pJets); 
  NTMet * pMet = 0;
  t1->SetBranchAddress("NTMet",&pMet); 
  NTEvent * pEvent = 0;
  t1->SetBranchAddress("NTEvent",&pEvent); 
  

  

  double n = t1->GetEntries(); //299569 for ttbaree
  // if(maxEntries!=0)
  //  n=min(maxEntries,n);
  cout  << "Entries in tree: " << n << endl;

 

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

  vector<pair<TString, int > >triggersummary;

  TBranch        *b_stopMass=0;
  TBranch        *b_chiMass=0;


  vector<float> * pStopMass=0;
  if(isMC)  t1->SetBranchAddress("stopMass",&pStopMass, &b_stopMass); 
  vector<float> * pChiMass=0;
  if(isMC) t1->SetBranchAddress("chiMass", &pChiMass, &b_chiMass);

  //if(!isMC) n=10;
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  for(double i=0;i<n;i++){  //main loop


    if(showstatusbar)
       displayStatusBar(i,n);

    // if(isMC && i < 38439892 /2) continue;

    if(isMC) b_stopMass->GetEntry(i);
    if(isMC) b_chiMass->GetEntry(i);

    float stopMass=0;
    float chiMass=0;
    
    //  if(!isMC) break;
    if(isMC){

      if(pStopMass->size()>0 && pChiMass->size() >0){

	stopMass=(pStopMass->at(0));
	for(size_t j=0;j<pChiMass->size();j++){
	  chiMass=(pChiMass->at(j));
	  if(chiMass >0)
	    break;
	}
	
	//	cout << stopMass << endl;
	//	cout <<chiMass << endl;
	bool rightmass=false;
	if(stopMass > 305 && stopMass < 355 && (stopMass - chiMass) > 195 && (stopMass - chiMass) < 255)
	    rightmass=true;
	if(!rightmass)
	  continue;
	//	cout << endl << "right mass point" << endl;

      }
      else{
	cout << "mass points not accessable. wrong MC? - continue with next event" << endl;
	//	exit(EXIT_FAILURE);
      }
    }

    
    t1->GetEntry(i); //performance


    //  if(!isMC && pEvent->runNo() < 199648) continue; // ##TRAP## for muon HLT stuff

    /// for 5fb
     if(breakat5fb && !isMC){

       if(pEvent->runNo() > 196531){
	 std::cout << "breaking at run " << pEvent->runNo() << std::endl;
	 break;
       }
     }
    if(i==n-1) std::cout << "last run number: " << pEvent->runNo() << std::endl;
   
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
    if(isMC&&doPUweight){
      puweight=PUweight.getPUweight(pEvent->truePU());
      puweight *= stopXSec( stopMass); // ### weight by cross section

    }
    //  if(isMC) cout << puweight <<endl;
    //  if(isMC) cout << stopMass << endl;
    

    vector<string> trigs;
    trigs=pEvent->firedTriggers();
    for(unsigned int trigit=0;trigit<trigs.size();trigit++){
      TString trig=trigs[trigit];
      

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
	temppair.first = trig; temppair.second =1;
	if(newtrigger) triggersummary.push_back(temppair);
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
    vector<NTInflatedMuon> selectedMuons;
    for(vector<top::NTInflatedMuon>::iterator muon=pMuons->begin();muon<pMuons->end();muon++){
      if(!(fabs(muon->eta())<2.4) ) continue;
      if(!(muon->pt() >20))   continue;
      if(!(muon->isGlobal())) continue;

      if(muon->vertexZ()>0.5) continue;
      if(muon->trkHits() <= 5 ) continue;
      if(muon->muonHits() <1) continue;
      
      if(muon->normChi2() >10) continue;


      if(muon->isoVal03() > 0.2 ) continue;
      if(!(muon->getMember("isPF"))) continue;

      if(muon->getMember("vPixelHits") < 1) continue;
      if(muon->getMember("matchedStations") < 2) continue;
      if(muon->getMember("dB") > 0.2) continue;

      selectedMuons.push_back(*muon);
    }
    if(mode == "mumu" && selectedMuons.size() < 2) continue;

    vector<NTInflatedElectron> selectedElecs;
    bool noHighPt=true;
    for(vector<NTInflatedElectron>::iterator elec=pElectrons->begin();elec<pElectrons->end();elec++){
      //first elec at least 20GeV

      if(noHighPt && elec->pt()<20 ) continue;
      noHighPt=false;
      if(elec->pt()<10) continue;

      double abs_eta=fabs(elec->eta());
      if(abs_eta>2.5 || (abs_eta<1.566 && abs_eta>1.4442)) continue;
      if(fabs(elec->dbs()) >0.04 ) continue;
      if(!(elec->isNotConv()) ) continue;
      if(elec->rhoIso03()>0.15 ) continue;
      if(elec->mHits() >2) continue;
      if(fabs(elec->vertexZ()>0.1)) continue;
      // if(!useRhoIso && elec->isoVal03()>0.15) continue;
      //  if(CiCId && (0x00 == (0x01 & (int) elec->id("cicTightMC"))) ) continue; //for CiC bit test

      double id=elec->getMember("vbtf11WP80");
      if(id==0 || id==2 || id==4 || id==6) continue;
      if(!(elec->getMember("isPFElectron"))) continue;


      //  if(!noOverlap(elec,*pMuons,0.1)) continue;

      selectedElecs.push_back(*elec);
    }
    double mass=0;
    if(mode=="ee"){
      if(selectedElecs.size() <2) continue;
      if(selectedElecs[0].q() * selectedElecs[1].q() >0) continue;
   
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

    if(!b_dilepton) continue; ///////////

    /////////trigger check/////////
    
    ////////////////dilepton selection
    if((mode == "mumu" || mode=="ee" ) && (mass > 106 || mass < 76)) b_ZVeto=true;
    if(mode=="emu")  b_ZVeto=true;

    vector<NTJet> selected_jets;
    for(vector<NTJet>::iterator jet=pJets->begin();jet<pJets->end();jet++){
      if(jet->pt() < jetptcut) continue;
      if(fabs(jet->eta()) >2.5) continue;
      if(!noOverlap(jet,selectedMuons,0.3)) continue; //cleaning  ##TRAP## changed to 0.4 /doesn't matter for final eff
      if(!noOverlap(jet,selectedElecs,0.3)) continue;
      if((!jet->id())) continue;

      selected_jets.push_back(*jet);
    }
    if(selected_jets.size() >0) b_oneJet=true;
    if(selected_jets.size() >1) b_twoJets=true;
    if((mode == "ee" || mode =="mumu") && pMet->met() > 40) b_met=true;
    if(mode == "emu") b_met=true;


    //  std::cout << puweight << std::endl;

    

    if(b_dilepton){
      sel_woTrig[0].second      +=puweight;
      c_selvmulti.fill(pEvent->vertexMulti() ,puweight);
      c_seljetmulti.fill(selected_jets.size() ,puweight);
      if(mode=="ee"){
	c_selpt.fill(selectedElecs[0].pt(),puweight);
	c_selpt.fill(selectedElecs[1].pt(),puweight);
	c_seleta.fill(selectedElecs[0].eta(),puweight);
	c_seleta.fill(selectedElecs[1].eta(),puweight);
	c_seldphi.fill(fabs(selectedElecs[0].phi() - pMet->phi()),puweight);
	c_seldphi2.fill(fabs(selectedElecs[1].phi() - pMet->phi()),puweight);
	c_seldrlep.fill(fabs(dR(selectedElecs[0],selectedElecs[1])),puweight);

	eta_2dimsel.Fill(fabs(selectedElecs[0].eta()),fabs(selectedElecs[1].eta()),puweight);


      }
      else if(mode=="mumu"){
	c_selpt.fill(selectedMuons[0].pt(),puweight);
	c_selpt.fill(selectedMuons[1].pt(),puweight);
	c_seleta.fill(selectedMuons[0].eta(),puweight);
	c_seleta.fill(selectedMuons[1].eta(),puweight);
	c_seldphi.fill(fabs(selectedMuons[0].phi() - pMet->phi()),puweight);
	c_seldphi2.fill(fabs(selectedMuons[1].phi() - pMet->phi()),puweight);
	c_seldrlep.fill(fabs(dR(selectedMuons[0],selectedMuons[1])),puweight);

	eta_2dimsel.Fill(fabs(selectedMuons[0].eta()),fabs(selectedMuons[1].eta()),puweight);

      }
      else if(mode =="emu"){
	c_selpt.fill(selectedElecs[0].pt(),puweight);
	c_selpt.fill(selectedMuons[0].pt(),puweight);
	c_seleta.fill(selectedElecs[0].eta(),puweight);
	c_seleta.fill(selectedMuons[0].eta(),puweight);
	c_seldphi.fill(fabs(selectedElecs[0].phi() - pMet->phi()),puweight);
	c_seldphi2.fill(fabs(selectedMuons[0].phi() - pMet->phi()),puweight);
	c_seldrlep.fill(fabs(dR(selectedElecs[0],selectedMuons[0])),puweight);

	eta_2dimsel.Fill(fabs(selectedElecs[0].eta()),fabs(selectedMuons[0].eta()),puweight);

      }
    }
  if(b_dilepton && b_ZVeto)                                   sel_woTrig[1].second      +=puweight;
  if(b_dilepton && b_ZVeto && b_oneJet)                       sel_woTrig[2].second      +=puweight;
  if(b_dilepton && b_ZVeto && b_oneJet && b_twoJets)          sel_woTrig[3].second      +=puweight;
  if(b_dilepton && b_ZVeto && b_oneJet && b_twoJets && b_met) sel_woTrig[4].second      +=puweight;

  if(firedDilepTrigger){
    if(b_dilepton){
      sel_Trig[0].second  +=puweight;
      c_trigvmulti.fill(pEvent->vertexMulti() ,puweight);
      c_trigjetmulti.fill(selected_jets.size() ,puweight);
      if(mode=="ee"){
	c_trigpt.fill(selectedElecs[0].pt(),puweight);
	c_trigpt.fill(selectedElecs[1].pt(),puweight);
	c_trigeta.fill(selectedElecs[0].eta(),puweight);
	c_trigeta.fill(selectedElecs[1].eta(),puweight);
	c_trigdphi.fill(fabs(selectedElecs[0].phi() - pMet->phi()),puweight);
	c_trigdphi2.fill(fabs(selectedElecs[1].phi() - pMet->phi()),puweight);
	c_trigdrlep.fill(fabs(dR(selectedElecs[0],selectedElecs[1])),puweight);

	eta_2dimtrig.Fill(fabs(selectedElecs[0].eta()),fabs(selectedElecs[1].eta()),puweight);
      }
      else if(mode=="mumu"){
	c_trigpt.fill(selectedMuons[0].pt(),puweight);
	c_trigpt.fill(selectedMuons[1].pt(),puweight);
	c_trigeta.fill(selectedMuons[0].eta(),puweight);
	c_trigeta.fill(selectedMuons[1].eta(),puweight);
	c_trigdphi.fill(fabs(selectedMuons[0].phi() - pMet->phi()),puweight);
	c_trigdphi2.fill(fabs(selectedMuons[1].phi() - pMet->phi()),puweight);
	c_trigdrlep.fill(fabs(dR(selectedMuons[0],selectedMuons[1])),puweight);

	eta_2dimtrig.Fill(fabs(selectedMuons[0].eta()),fabs(selectedMuons[1].eta()),puweight);

      }
      else if(mode =="emu"){
	c_trigpt.fill(selectedElecs[0].pt(),puweight);
	c_trigpt.fill(selectedMuons[0].pt(),puweight);
	c_trigeta.fill(selectedElecs[0].eta(),puweight);
	c_trigeta.fill(selectedMuons[0].eta(),puweight);
	c_trigdphi.fill(fabs(selectedElecs[0].phi() - pMet->phi()),puweight);
	c_trigdphi2.fill(fabs(selectedMuons[0].phi() - pMet->phi()),puweight);
	c_trigdrlep.fill(fabs(dR(selectedElecs[0],selectedMuons[0])),puweight);

	eta_2dimtrig.Fill(fabs(selectedElecs[0].eta()),fabs(selectedMuons[0].eta()),puweight);

      }


    }                                            
    if(b_dilepton && b_ZVeto)                                   sel_Trig[1].second  +=puweight;
    if(b_dilepton && b_ZVeto && b_oneJet)                       sel_Trig[2].second +=puweight;
    if(b_dilepton && b_ZVeto && b_oneJet && b_twoJets)          sel_Trig[3].second +=puweight;
    if(b_dilepton && b_ZVeto && b_oneJet && b_twoJets && b_met) sel_Trig[4].second    +=puweight;
  }

  if(firedMet){
    if(b_dilepton){
      c_selmettrigvmulti.fill(pEvent->vertexMulti() ,puweight);
      c_selmettrigjetmulti.fill(selected_jets.size() ,puweight);
      if(mode=="ee"){

	c_selmettrigpt.fill(selectedElecs[0].pt(),puweight);
	c_selmettrigpt.fill(selectedElecs[1].pt(),puweight);
	c_selmettrigeta.fill(selectedElecs[0].eta(),puweight);
	c_selmettrigeta.fill(selectedElecs[1].eta(),puweight);
	c_selmettrigdphi.fill(fabs(selectedElecs[0].phi() - pMet->phi()),puweight);
	c_selmettrigdphi2.fill(fabs(selectedElecs[1].phi() - pMet->phi()),puweight);
	c_selmettrigdrlep.fill(fabs(dR(selectedElecs[0],selectedElecs[1])),puweight);
      }
      else if(mode=="mumu"){
	c_selmettrigpt.fill(selectedMuons[0].pt(),puweight);
	c_selmettrigpt.fill(selectedMuons[1].pt(),puweight);
	c_selmettrigeta.fill(selectedMuons[0].eta(),puweight);
	c_selmettrigeta.fill(selectedMuons[1].eta(),puweight);
	c_selmettrigdphi.fill(fabs(selectedMuons[0].phi() - pMet->phi()),puweight);
	c_selmettrigdphi2.fill(fabs(selectedMuons[1].phi() - pMet->phi()),puweight);
	c_selmettrigdrlep.fill(fabs(dR(selectedMuons[0],selectedMuons[1])),puweight);

      }
      else if(mode =="emu"){
	c_selmettrigpt.fill(selectedElecs[0].pt(),puweight);
	c_selmettrigpt.fill(selectedMuons[0].pt(),puweight);
	c_selmettrigeta.fill(selectedElecs[0].eta(),puweight);
	c_selmettrigeta.fill(selectedMuons[0].eta(),puweight);
	c_selmettrigdphi.fill(fabs(selectedElecs[0].phi() - pMet->phi()),puweight);
	c_selmettrigdphi2.fill(fabs(selectedMuons[0].phi() - pMet->phi()),puweight);
	c_selmettrigdrlep.fill(fabs(dR(selectedElecs[0],selectedMuons[0])),puweight);

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
      c_selbothtrigvmulti.fill(pEvent->vertexMulti() ,puweight);
      c_selbothtrigjetmulti.fill(selected_jets.size() ,puweight);
      if(mode=="ee"){
	c_selbothtrigpt.fill(selectedElecs[0].pt(),puweight);
	c_selbothtrigpt.fill(selectedElecs[1].pt(),puweight);
	c_selbothtrigeta.fill(selectedElecs[0].eta(),puweight);
	c_selbothtrigeta.fill(selectedElecs[1].eta(),puweight);
	c_selbothtrigdphi.fill(fabs(selectedElecs[0].phi() - pMet->phi()),puweight);
	c_selbothtrigdphi2.fill(fabs(selectedElecs[1].phi() - pMet->phi()),puweight);
	c_selbothtrigdrlep.fill(fabs(dR(selectedElecs[0],selectedElecs[1])),puweight);
      }
      else if(mode=="mumu"){
	c_selbothtrigpt.fill(selectedMuons[0].pt(),puweight);
	c_selbothtrigpt.fill(selectedMuons[1].pt(),puweight);
	c_selbothtrigeta.fill(selectedMuons[0].eta(),puweight);
	c_selbothtrigeta.fill(selectedMuons[1].eta(),puweight);
	c_selbothtrigdphi.fill(fabs(selectedMuons[0].phi() - pMet->phi()),puweight);
	c_selbothtrigdphi2.fill(fabs(selectedMuons[1].phi() - pMet->phi()),puweight);
	c_selbothtrigdrlep.fill(fabs(dR(selectedMuons[0],selectedMuons[1])),puweight);

      }
      else if(mode =="emu"){
	c_selbothtrigpt.fill(selectedElecs[0].pt(),puweight);
	c_selbothtrigpt.fill(selectedMuons[0].pt(),puweight);
	c_selbothtrigeta.fill(selectedElecs[0].eta(),puweight);
	c_selbothtrigeta.fill(selectedMuons[0].eta(),puweight);
	c_selbothtrigdphi.fill(fabs(selectedElecs[0].phi() - pMet->phi()),puweight);
	c_selbothtrigdphi2.fill(fabs(selectedMuons[0].phi() - pMet->phi()),puweight);
	c_selbothtrigdrlep.fill(fabs(dR(selectedElecs[0],selectedMuons[0])),puweight);
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

////////////////////////////////////check MC triggers

/// show contribution of each trigger in data


  if(coutalltriggerpaths){
    cout << "all triggers containing MET" << endl;
    for(unsigned int i=0;i<triggersummary.size();i++){
      cout << "<< \"" << triggersummary[i].first ;
    }
    cout << ";" << endl;
  }

  if(checktriggerpaths){
    
    cout << "\ncontribution of different triggers:\n" << endl;
    for(unsigned int i=0;i<triggersummary.size();i++){
      cout << triggersummary[i].second << "\t" << triggersummary[i].first << endl;
    }
    
    
    ///  triggersummary mettriggers
      int trigin=0;
      vector<unsigned int> notinc;
      for(unsigned int i=0; i<mettriggersMC.size(); i++){
	bool inc=false;
	//	if(!(mettriggersMC[i].Contains("HLT"))) continue;
	for(unsigned int j=0; j<triggersummary.size();j++){
	  // cout << mettriggers[i] << " vs " << triggersummary[j].first << endl;
	  if(triggersummary[j].first.Contains(mettriggersMC[i])){
	    trigin++;
	    inc=true;
	    break;
	  }
	}
	if(!inc) notinc.push_back(i);
      }
      
      
      cout << trigin << " total; fraction of found triggers wrt datamettriggers: " << trigin/(double)mettriggers.size() << endl;
      cout << "\ntriggers in metMCtrigger list and not found in triggered triggers:\n" << endl;

      for(unsigned int i=0; i<notinc.size();i++){
	cout << mettriggersMC[notinc[i]] << endl;
      }

      // trigin=0;
      // notinc.clear();
      // for(unsigned int i=0; i<triggersummary.size(); i++){
      // 	bool inc=false;
      // 	for(unsigned int j=0; j<mettriggersMC.size();j++){
      // 	  // cout << mettriggers[i] << " vs " << triggersummary[j].first << endl;
      // 	  if(triggersummary[i].first.Contains(mettriggersMC[j])){
      // 	    trigin++;
      // 	    inc=true;
      // 	    break;
      // 	  }
      // 	}
      // 	if(!inc) notinc.push_back(i);
      // }

      // cout << "triggers with M"
    
  }

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


  
  

  c_pteff = c_trigpt / c_selpt;
  c_etaeff = c_trigeta / c_seleta;
  c_dphieff = c_trigdphi / c_seldphi;
  c_dphieff2 = c_trigdphi2 / c_seldphi2;

  c_vmultieff = c_trigvmulti / c_selvmulti;
  c_drlepeff = c_trigdrlep / c_seldrlep;

  c_jetmultieff = c_trigjetmulti / c_seljetmulti;
  

  eta_2dim= divideTH2DBinomial(eta_2dimtrig,eta_2dimsel);
  eta_2dim.GetXaxis()->SetTitle("#eta");
  eta_2dim.GetYaxis()->SetTitle("#eta");



  // histo1D c_tempmeteff=c_selmettrigpt/c_selpt;
  //  c_tempmeteff.setDivideBinomial(false);
  std::cout << "making correlation plots, ignore warnings!" <<std::endl;
   c_corrpt = (c_pteff * (c_selmettrigpt/c_selpt))/(c_selbothtrigpt/c_selpt);
  c_correta = (c_etaeff * (c_selmettrigeta/c_seleta))/(c_selbothtrigeta/c_seleta);
  c_corrdphi = (c_dphieff * (c_selmettrigdphi/c_seldphi))/(c_selbothtrigdphi/c_seldphi);
  c_corrdphi2 = (c_dphieff2 * (c_selmettrigdphi2/c_seldphi2))/(c_selbothtrigdphi2/c_seldphi2);

  c_corrvmulti = (c_vmultieff * (c_selmettrigvmulti/c_selvmulti))/(c_selbothtrigvmulti/c_selvmulti);
c_corrjetmulti = (c_jetmultieff * (c_selmettrigjetmulti/c_seljetmulti))/(c_selbothtrigjetmulti/c_seljetmulti);

  c_corrdrlep = (c_drlepeff * (c_selmettrigdrlep/c_seldrlep))/(c_selbothtrigdrlep/c_seldrlep);

  std::cout << "stop ignoring warning ;)" << std::endl;

  etaeff_=c_etaeff;
  pteff_=c_pteff;
  dphieff_=c_dphieff;
  dphieff2_=c_dphieff2;
  vmultieff_=c_vmultieff;
  jetmultieff_=c_jetmultieff;

  drlepeff_=c_drlepeff;
  correta_=c_correta;
  corrpt_=c_corrpt;
  corrdphi_=c_corrdphi;
  corrdphi2_=c_corrdphi2;
  corrvmulti_=c_corrvmulti;
  corrjetmulti_=c_corrjetmulti;
  corrdrlep_=c_corrdrlep;

  etaeff_.setDivideBinomial(false);
  pteff_.setDivideBinomial(false);
  dphieff_.setDivideBinomial(false);
  dphieff2_.setDivideBinomial(false);
  vmultieff_.setDivideBinomial(false);
  jetmultieff_.setDivideBinomial(false);
  drlepeff_.setDivideBinomial(false);

  return output;

}

//Eff(string mode="ee", bool doPUweight=true, const char * inputFile="  ", bool isMC=true){

  void writeAll(){

    TString add="";
    if(isMC) add="MC";
    etaeff_.writeTGraph("eta_eff"+add,false);
    etaeff_.writeTH1D("TH_eta_eff"+add,false);
    etaeff_.writeTH1D("axis_eta",false);


    pteff_.writeTGraph("pt_eff"+add,false);
    pteff_.writeTH1D("TH_pt_eff"+add,false);
    pteff_.writeTH1D("axis_pt",false);

    dphieff_.writeTGraph("dphi_eff"+add,false);
    dphieff_.writeTH1D("axis_dphi",false);
    dphieff2_.writeTGraph("dphi2_eff"+add,false);
    dphieff2_.writeTH1D("axis_dphi2",false);

    eta_2dim.SetName("eta2d_eff"+add);
    eta_2dim.Write();

    vmultieff_.writeTGraph("vmulti_eff"+add,false);
    vmultieff_.writeTH1D("axis_vmulti",false);

    drlepeff_.writeTGraph("drlep_eff"+add,false);
    drlepeff_.writeTH1D("axis_drlep",false);

    jetmultieff_.writeTGraph("jetmulti_eff"+add,false);
    jetmultieff_.writeTH1D("axis_jetmulti",false);

    correta_.writeTGraph("correta"+add,false);
    corrpt_.writeTGraph("corrpt"+add,false);

    corrdphi_.writeTGraph("corrdphi"+add,false);
    corrdphi2_.writeTGraph("corrdphi2"+add,false);

    corrvmulti_.writeTGraph("corrvmulti"+add,false);
    corrjetmulti_.writeTGraph("corrvmulti"+add,false);

    corrdrlep_.writeTGraph("corrdrlep"+add,false);

  }

  float stopXSec(float );


private:
  std::vector<float> binseta_;
  std::vector<float> binseta2dx_;
  std::vector<float> binseta2dy_;
  std::vector<float> binspt_;
  top::histo1D etaeff_;
  top::histo1D pteff_;
  top::histo1D correta_;
  top::histo1D corrpt_;
  top::histo1D dphieff_;
  top::histo1D vmultieff_;
  top::histo1D corrdphi_;
  top::histo1D corrvmulti_;
  top::histo1D jetmultieff_;
  top::histo1D corrjetmulti_;
  top::histo1D dphieff2_;
  top::histo1D corrdphi2_;

  top::histo1D drlepeff_;
  top::histo1D corrdrlep_;

  bool isMC;

  TH2D eta_2dim;

};

TChain * makeChain(std::vector<TString> paths){
  TChain * chain = new TChain(paths[0]);
  for(std::vector<TString>::iterator path=paths.begin();path<paths.end();++path){
    chain->Add((*path)+"/PFTree/pfTree");
    std::cout << "added " << *path << " to chain." << std::endl;
  }
  return chain;
}

TChain * makeChain(TString path){
  TChain * chain = new TChain(path);
  
  chain->Add(path+"/PFTree/pfTree");
    std::cout << "added " << path << " to chain." << std::endl;
  return chain;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<TString> getPaths(int divider=9){
  using namespace std;
  std::vector<TString> out;
  ifstream inputfiles ("/afs/naf.desy.de/user/k/kieseler/scratch/2012/HCP2/CMSSW_5_3_3_patch3/src/WbWbXAnalysis/Analysis/triggerEfficiencies12/lsout.txt");

  TString filename;
  int i=0;
  while(!inputfiles.eof()){

    inputfiles >> filename;
    if(i%divider == 1)
      out << "dcap://dcache-cms-dcap.desy.de:22125/pnfs/desy.de/cms/tier2/store/user/jkiesele/Crab_output/susyMC_2013-02-11T12-04-02/output_tree_8TeV_susy/"+filename;
    i++;
  }
  return out;

}

void analyze(){

  bool getTeX=true;

  using namespace std;
  using namespace top;

  std::vector<float> binsmumueta, bins2dee, bins2dmu;
  binsmumueta.push_back(-2.4);binsmumueta.push_back(-2.1);binsmumueta.push_back(-1.2);binsmumueta.push_back(-0.9);binsmumueta.push_back(0.9);binsmumueta.push_back(1.2);binsmumueta.push_back(2.1);binsmumueta.push_back(2.4);

  bins2dee << 0 << 1.479 << 2.5;
  bins2dmu << 0 << 0.9 << 2.4;

  triggerAnalyzer ta_eed;
  triggerAnalyzer ta_mumud;
  ta_mumud.setBinsEta(binsmumueta);
  ta_eed.setBinsEta2dX(bins2dee);
  ta_eed.setBinsEta2dY(bins2dee);
  triggerAnalyzer ta_emud;
  ta_emud.setBinsEta2dX(bins2dee);
  ta_emud.setBinsEta2dY(bins2dmu);
  triggerAnalyzer ta_eeMC=ta_eed;
  triggerAnalyzer ta_mumuMC=ta_mumud;
  triggerAnalyzer ta_emuMC=ta_emud;

  TString datadir="/scratch/hh/dust/naf/cms/user/kieseler/trees_8TeV_SUSY/";

  std::vector<TString> datafiles;
  datafiles  << datadir + "tree_8TeV_MET_runA_06Aug.root" 
	     << datadir + "tree_8TeV_MET_runA_13Jul.root" 
    	     << datadir + "tree_8TeV_MET_runB_13Jul.root" ;
  //<< datadir + "tree_8TeV_MET_runC_24Aug.root" 
  //	     << datadir + "tree_8TeV_MET_runC_prompt.root";
 

  const char * pileuproot = "/afs/naf.desy.de/user/k/kieseler/HCP2/WbWbXAnalysis/Data/HCP.json.txt_PU.root";//HCP_PU.root";//HCP_5.3fb_PU.root";

  if(breakat5fb) pileuproot = "/afs/naf.desy.de/user/k/kieseler/scratch/2012/HCP2/CMSSW_5_3_3_patch3/src/WbWbXAnalysis/Data/HCP_5.3fb.json_PU.root";
  //////// NOT REALLY VALID PU reweighting!!!

  TChain * datachain=makeChain(datafiles);


  vector<double> eed=ta_eed.Eff("ee",    datachain,pileuproot, false,getTeX);
  vector<double> mumud=ta_mumud.Eff("mumu",  datachain,pileuproot, false,getTeX);
  vector<double> emud= ta_emud.Eff("emu",   datachain,pileuproot, false,getTeX);

  // TString MCdir="/scratch/hh/dust/naf/cms/user/kieseler/trees_8TeV_SUSY/";
  TString MCdir="/scratch/hh/dust/naf/cms/user/kieseler/NJS_OUT/";

//datadir;//"/scratch/hh/dust/naf/cms/user/kieseler/trees_8TeV/";

  std::vector<TString> eemcfiles,mumumcfiles,emumcfiles;
  // eemcfiles =getPaths(200);
  eemcfiles<< MCdir+"tree_8TeV_susy.root";// << MCdir+"2.root" <<MCdir+"3.root" <<MCdir+"4.root" <<MCdir+"5.root" <<MCdir+"6.root" <<MCdir+"7.root" <<MCdir+"8.root" <<MCdir+"9.root";
   mumumcfiles=eemcfiles;
  emumcfiles=mumumcfiles;
 
  // changed back again  std::cout << "SET UP ONLY FOR DIRECT DECAY!!!!!!! this version is NOT checked in in cvs" << std::endl;

  TChain * eechain=makeChain(eemcfiles);
  TChain * mumuchain= eechain;
  TChain * emuchain= eechain;
  
  vector<double> eeMC=ta_eeMC.Eff("ee",  eechain  ,pileuproot,true,getTeX);
  vector<double> mumuMC=ta_mumuMC.Eff("mumu", mumuchain ,pileuproot,true,getTeX);
  vector<double> emuMC=ta_emuMC.Eff("emu",  emuchain ,pileuproot,true,getTeX);


  cout.precision(3);
  // get correction factors
 

  cout << "\n\n TeX table:" <<endl;

   cout << "\\begin{table}\n\\center\n\\begin{tabular}{c | c | c | c | c}" << endl;
  cout << " & $\\epsilon_{data}$ & $\\epsilon_{MC}$ & $\\alpha$ & $\\epsilon_{data}/\\epsilon_{MC}$ \\\\ \\hline" << endl;

  cout << "$ee$ & " << eed[0] <<" $\\pm$ " << eed[1] << " (stat.) & " 
       << eeMC[0] << " $\\pm$ " << eeMC[1] << " (stat.) & " 
       << eeMC[2] << " & " 
       << eed[0]/eeMC[0] 
       << " $\\pm$ " << sqrt((eed[1]/eeMC[0])*(eed[1]/eeMC[0]) + (eed[0]/(eeMC[0]*eeMC[0]))*eeMC[1]*eeMC[1]*(eed[0]/(eeMC[0]*eeMC[0]))) << " (stat.)  " 
       <<  " $\\pm$ " << sqrt(0.01*eed[0]/eeMC[0]*eed[0]/eeMC[0]*0.01  +fabs(ratiomultiplier*eed[0]/eeMC[0] * eeMC[2])*fabs(ratiomultiplier*eed[0]/eeMC[0] * eeMC[2])) << " (syst.) \\\\" << endl;
  

cout << "$\\mu\\mu$ & " << mumud[0] <<" $\\pm$ " << mumud[1] << " (stat.) & " 
     << mumuMC[0] << " $\\pm$ " << mumuMC[1] << " (stat.) & "
     << mumuMC[2] << " & "
     << mumud[0]/mumuMC[0] 
     << " $\\pm$ " << sqrt((mumud[1]/mumuMC[0])*(mumud[1]/mumuMC[0]) + (mumud[0]/(mumuMC[0]*mumuMC[0]))*mumuMC[1]*mumuMC[1]*(mumud[0]/(mumuMC[0]*mumuMC[0]))) << " (stat.)  " 
     <<  " $\\pm$ " << sqrt(0.01*mumud[0]/mumuMC[0]*mumud[0]/mumuMC[0]*0.01+fabs(ratiomultiplier*mumud[0]/mumuMC[0] * mumuMC[2])*fabs(ratiomultiplier*mumud[0]/mumuMC[0] * mumuMC[2])) << " (syst.) \\\\" << endl;

cout << "$e\\mu$ & " << emud[0] <<" $\\pm$ " << emud[1] << " (stat.) & " 
       << emuMC[0] << " $\\pm$ " << emuMC[1] << " (stat.) & " 
     <<emuMC[2] << " & "
     << emud[0]/emuMC[0] 
       << " $\\pm$ " << sqrt((emud[1]/emuMC[0])*(emud[1]/emuMC[0]) + (emud[0]/(emuMC[0]*emuMC[0]))*emuMC[1]*emuMC[1]*(emud[0]/(emuMC[0]*emuMC[0]))) << " (stat.)  " 
     <<  " $\\pm$ " << sqrt(0.01*emud[0]/emuMC[0]*emud[0]/emuMC[0]*0.01+fabs(ratiomultiplier*emud[0]/emuMC[0] * emuMC[2])*fabs(ratiomultiplier*emud[0]/emuMC[0] * emuMC[2])) << " (syst.) \\\\" << endl;

cout << "\\end{tabular}\n\\caption{Efficiencies for data and MC and correction factors as well as correlation ratios $\\alpha$ measured at the dilepton selection step}\n\\end{table}" << endl;

 cout << "\n\n\\begin{table}\n\\center\n\\begin{tabular}{c | c | c | c}" << endl;
 cout  << " & $ee$ & $\\mu\\mu$ & $e\\mu$ \\\\ \\hline" <<endl;

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

 cout << "\\end{tabular}\n\\caption{Trigger scale factors for different ttbar selection steps}\n\\end{table}" << endl;


//   histo1D getEtaPlot(){return etaeff_;}
//   histo1D getPtPlot(){return pteff_;}
//   histo1D getCorrelationPt(){return corrpt_;}
//   histo1D getCorrelationEta(){return correta_;}






  histo1D scalefactor;
  histo1D data;
  histo1D MC;
  TH2D SFdd,datadd,MCdd;

  TFile* f5 = new TFile("triggerSummary_ee.root","RECREATE");

  ta_eed.writeAll();
  ta_eeMC.writeAll();
  data=ta_eed.getEtaPlot();
  MC=ta_eeMC.getEtaPlot();
  data.setDivideBinomial(false);
  MC.setDivideBinomial(false);
  scalefactor=data/MC;
  scalefactor.addGlobalRelError("rel_sys",0.01);
  scalefactor.writeTGraph("scalefactor_eta",false);
  scalefactor.addErrorContainer("corr_ratio_up",scalefactor*ta_eeMC.getCorrelationEta(),ratiomultiplier);
  scalefactor.writeTGraph("scalefactor_eta_incl_corrErr",false);
  scalefactor.writeTH1D("TH_scalefactor_eta_incl_corrErr",false);

  datadd=ta_eed.getEta2D();
  MCdd=ta_eeMC.getEta2D();
  SFdd = divideTH2D(datadd, MCdd);
  SFdd.SetName("scalefactor_eta2d");
  SFdd.SetTitle("scalefactor_eta2d");
  SFdd.Write();
  SFdd.SetName("scalefactor_eta2d_with_syst");
  SFdd.SetTitle("scalefactor_eta2d_with_syst");
  addRelError(SFdd, 0.01);
  SFdd.Write();


  data=ta_eed.getPtPlot();
  MC=ta_eeMC.getPtPlot();
  data.setDivideBinomial(false);
  MC.setDivideBinomial(false);
  scalefactor=data/MC;
  scalefactor.addGlobalRelError("rel_sys",0.01);
  scalefactor.writeTGraph("scalefactor_pt",false);
  scalefactor.addErrorContainer("corr_ratio_up",scalefactor*ta_eeMC.getCorrelationPt(),ratiomultiplier);
  scalefactor.writeTGraph("scalefactor_pt_incl_corrErr",false);
  scalefactor.writeTH1D("TH_scalefactor_pt_incl_corrErr",false);

  data=ta_eed.getDPhiPlot();
  MC=ta_eeMC.getDPhiPlot();
  data.setDivideBinomial(false);
  MC.setDivideBinomial(false);
  scalefactor=data/MC;
  scalefactor.addGlobalRelError("rel_sys",0.01);
  scalefactor.writeTGraph("scalefactor_dphi",false);
  scalefactor.addErrorContainer("corr_ratio_up",scalefactor*ta_eeMC.getCorrelationDPhi(),ratiomultiplier);
  scalefactor.writeTGraph("scalefactor_dphi_incl_corrErr",false);
  scalefactor.writeTH1D("TH_scalefactor_dphi_incl_corrErr",false);


  data=ta_eed.getDPhiPlot2();
  MC=ta_eeMC.getDPhiPlot2();
  data.setDivideBinomial(false);
  MC.setDivideBinomial(false);
  scalefactor=data/MC;
  scalefactor.addGlobalRelError("rel_sys",0.01);
  scalefactor.writeTGraph("scalefactor_dphi2",false);
  scalefactor.addErrorContainer("corr_ratio_up",scalefactor*ta_eeMC.getCorrelationDPhi2(),ratiomultiplier);
  scalefactor.writeTGraph("scalefactor_dphi2_incl_corrErr",false);
  scalefactor.writeTH1D("TH_scalefactor_dphi2_incl_corrErr",false);

  data=ta_eed.getVmultiPlot();
  MC=ta_eeMC.getVmultiPlot();
  data.setDivideBinomial(false);
  MC.setDivideBinomial(false);
  scalefactor=data/MC;
  scalefactor.addGlobalRelError("rel_sys",0.01);
  scalefactor.writeTGraph("scalefactor_vmulti",false);
  scalefactor.addErrorContainer("corr_ratio_up",scalefactor*ta_eeMC.getCorrelationVmulti(),ratiomultiplier);
  scalefactor.writeTGraph("scalefactor_vmulti_incl_corrErr",false);
  scalefactor.writeTH1D("TH_scalefactor_vmulti_incl_corrErr",false);


  data=ta_eed.getDrlepPlot();
  MC=ta_eeMC.getDrlepPlot();
  data.setDivideBinomial(false);
  MC.setDivideBinomial(false);
  scalefactor=data/MC;
  scalefactor.addGlobalRelError("rel_sys",0.01);
  scalefactor.writeTGraph("scalefactor_drlep",false);
  scalefactor.addErrorContainer("corr_ratio_up",scalefactor*ta_eeMC.getCorrelationDrlep(),ratiomultiplier);
  scalefactor.writeTGraph("scalefactor_drlep_incl_corrErr",false);
  scalefactor.writeTH1D("TH_scalefactor_drlep_incl_corrErr",false);


  data=ta_eed.getJetmultiPlot();
  MC=ta_eeMC.getJetmultiPlot();
  data.setDivideBinomial(false);
  MC.setDivideBinomial(false);
  scalefactor=data/MC;
  scalefactor.addGlobalRelError("rel_sys",0.01);
  scalefactor.writeTGraph("scalefactor_jetmulti",false);
  scalefactor.addErrorContainer("corr_ratio_up",scalefactor*ta_eeMC.getCorrelationJetmulti(),ratiomultiplier);
  scalefactor.writeTGraph("scalefactor_jetmulti_incl_corrErr",false);
  scalefactor.writeTH1D("TH_scalefactor_jetmulti_incl_corrErr",false);



  f5->Close();

TFile fa=TFile("trigger_SF_ee.root","RECREATE");
  fa.cd();
  SFdd.Write();
  fa.Close();

  
  TFile* f6 = new TFile("triggerSummary_mumu.root","RECREATE");//ta_mumud.writeAll();

  ta_mumud.writeAll();
  ta_mumuMC.writeAll();
  data=ta_mumud.getEtaPlot();
  MC=ta_mumuMC.getEtaPlot();
  data.setDivideBinomial(false);
  MC.setDivideBinomial(false);
  scalefactor=data/MC;
  scalefactor.addGlobalRelError("rel_sys",0.01);
  scalefactor.writeTGraph("scalefactor_eta",false);
  scalefactor.addErrorContainer("corr_ratio_up",scalefactor*ta_mumuMC.getCorrelationEta(),ratiomultiplier);
  scalefactor.writeTGraph("scalefactor_eta_incl_corrErr",false);
  scalefactor.writeTH1D("TH_scalefactor_eta_incl_corrErr",false);

  datadd=ta_mumud.getEta2D();
  MCdd=ta_mumuMC.getEta2D();
  SFdd = divideTH2D(datadd, MCdd);
  SFdd.SetName("scalefactor_eta2d");
  SFdd.SetTitle("scalefactor_eta2d");
  SFdd.Write();
  SFdd.SetName("scalefactor_eta2d_with_syst");
  SFdd.SetTitle("scalefactor_eta2d_with_syst");
  addRelError(SFdd, 0.01);
  SFdd.Write();


  data=ta_mumud.getPtPlot();
  MC=ta_mumuMC.getPtPlot();
  data.setDivideBinomial(false);
  MC.setDivideBinomial(false);
  scalefactor=data/MC;
  scalefactor.addGlobalRelError("rel_sys",0.01);
  scalefactor.writeTGraph("scalefactor_pt",false);
  scalefactor.addErrorContainer("corr_ratio_up",scalefactor*ta_mumuMC.getCorrelationPt(),ratiomultiplier);
  scalefactor.writeTGraph("scalefactor_pt_incl_corrErr",false);
  scalefactor.writeTH1D("TH_scalefactor_pt_incl_corrErr",false);

  data=ta_mumud.getDPhiPlot();
  MC=ta_mumuMC.getDPhiPlot();
  data.setDivideBinomial(false);
  MC.setDivideBinomial(false);
  scalefactor=data/MC;
  scalefactor.addGlobalRelError("rel_sys",0.01);
  scalefactor.writeTGraph("scalefactor_dphi",false);
  scalefactor.addErrorContainer("corr_ratio_up",scalefactor*ta_mumuMC.getCorrelationDPhi(),ratiomultiplier);
  scalefactor.writeTGraph("scalefactor_dphi_incl_corrErr",false);
  scalefactor.writeTH1D("TH_scalefactor_dphi_incl_corrErr",false);

  data=ta_mumud.getDPhiPlot2();
  MC=ta_mumuMC.getDPhiPlot2();
  data.setDivideBinomial(false);
  MC.setDivideBinomial(false);
  scalefactor=data/MC;
  scalefactor.addGlobalRelError("rel_sys",0.01);
  scalefactor.writeTGraph("scalefactor_dphi2",false);
  scalefactor.addErrorContainer("corr_ratio_up",scalefactor*ta_mumuMC.getCorrelationDPhi2(),ratiomultiplier);
  scalefactor.writeTGraph("scalefactor_dphi2_incl_corrErr",false);
  scalefactor.writeTH1D("TH_scalefactor_dphi2_incl_corrErr",false);


  data=ta_mumud.getVmultiPlot();
  MC=ta_mumuMC.getVmultiPlot();
  data.setDivideBinomial(false);
  MC.setDivideBinomial(false);
  scalefactor=data/MC;
  scalefactor.addGlobalRelError("rel_sys",0.01);
  scalefactor.writeTGraph("scalefactor_vmulti",false);
  scalefactor.addErrorContainer("corr_ratio_up",scalefactor*ta_mumuMC.getCorrelationVmulti(),ratiomultiplier);
  scalefactor.writeTGraph("scalefactor_vmulti_incl_corrErr",false);
  scalefactor.writeTH1D("TH_scalefactor_vmulti_incl_corrErr",false);

  data=ta_mumud.getDrlepPlot();
  MC=ta_mumuMC.getDrlepPlot();
  data.setDivideBinomial(false);
  MC.setDivideBinomial(false);
  scalefactor=data/MC;
  scalefactor.addGlobalRelError("rel_sys",0.01);
  scalefactor.writeTGraph("scalefactor_drlep",false);
  scalefactor.addErrorContainer("corr_ratio_up",scalefactor*ta_mumuMC.getCorrelationDrlep(),ratiomultiplier);
  scalefactor.writeTGraph("scalefactor_drlep_incl_corrErr",false);
  scalefactor.writeTH1D("TH_scalefactor_drlep_incl_corrErr",false);

  data=ta_mumud.getJetmultiPlot();
  MC=ta_mumuMC.getJetmultiPlot();
  data.setDivideBinomial(false);
  MC.setDivideBinomial(false);
  scalefactor=data/MC;
  scalefactor.addGlobalRelError("rel_sys",0.01);
  scalefactor.writeTGraph("scalefactor_jetmulti",false);
  scalefactor.addErrorContainer("corr_ratio_up",scalefactor*ta_mumuMC.getCorrelationJetmulti(),ratiomultiplier);
  scalefactor.writeTGraph("scalefactor_jetmulti_incl_corrErr",false);
  scalefactor.writeTH1D("TH_scalefactor_jetmulti_incl_corrErr",false);

  f6->Close();


 TFile fb=TFile("trigger_SF_mumu.root","RECREATE");
  fb.cd();
  SFdd.Write();
  fb.Close();


  TFile* f7 = new TFile("triggerSummary_emu.root","RECREATE");

  ta_emud.writeAll();
  ta_emuMC.writeAll();
  data=ta_emud.getEtaPlot();
  MC=ta_emuMC.getEtaPlot();
  data.setDivideBinomial(false);
  MC.setDivideBinomial(false);
  scalefactor=data/MC;
  scalefactor.addGlobalRelError("rel_sys",0.01);
  scalefactor.writeTGraph("scalefactor_eta",false);
  scalefactor.addErrorContainer("corr_ratio_up",scalefactor*ta_emuMC.getCorrelationEta(),ratiomultiplier);
  scalefactor.writeTGraph("scalefactor_eta_incl_corrErr",false);
  scalefactor.writeTH1D("TH_scalefactor_eta_incl_corrErr",false);


  datadd=ta_emud.getEta2D();
  MCdd=ta_emuMC.getEta2D();
  SFdd = divideTH2D(datadd, MCdd);
  SFdd.SetName("scalefactor_eta2d");
  SFdd.SetTitle("scalefactor_eta2d");
  SFdd.Write();
  SFdd.SetName("scalefactor_eta2d_with_syst");
  SFdd.SetTitle("scalefactor_eta2d_with_syst");
  addRelError(SFdd, 0.01);
  SFdd.Write();


  data=ta_emud.getPtPlot();
  MC=ta_emuMC.getPtPlot();
  data.setDivideBinomial(false);
  MC.setDivideBinomial(false);
  scalefactor=data/MC;
  scalefactor.addGlobalRelError("rel_sys",0.01);
  scalefactor.writeTGraph("scalefactor_pt",false);
  scalefactor.addErrorContainer("corr_ratio_up",scalefactor*ta_emuMC.getCorrelationPt(),ratiomultiplier);
  scalefactor.writeTGraph("scalefactor_pt_incl_corrErr",false);
  scalefactor.writeTH1D("TH_scalefactor_pt_incl_corrErr",false);

  data=ta_emud.getDPhiPlot();
  MC=ta_emuMC.getDPhiPlot();
  data.setDivideBinomial(false);
  MC.setDivideBinomial(false);
  scalefactor=data/MC;
  scalefactor.addGlobalRelError("rel_sys",0.01);
  scalefactor.writeTGraph("scalefactor_dphi",false);
  scalefactor.addErrorContainer("corr_ratio_up",scalefactor*ta_emuMC.getCorrelationDPhi(),ratiomultiplier);
  scalefactor.writeTGraph("scalefactor_dphi_incl_corrErr",false);
  scalefactor.writeTH1D("TH_scalefactor_dphi_incl_corrErr",false);


  data=ta_emud.getDPhiPlot2();
  MC=ta_emuMC.getDPhiPlot2();
  data.setDivideBinomial(false);
  MC.setDivideBinomial(false);
  scalefactor=data/MC;
  scalefactor.addGlobalRelError("rel_sys",0.01);
  scalefactor.writeTGraph("scalefactor_dphi2",false);
  scalefactor.addErrorContainer("corr_ratio_up",scalefactor*ta_emuMC.getCorrelationDPhi2(),ratiomultiplier);
  scalefactor.writeTGraph("scalefactor_dphi2_incl_corrErr",false);
  scalefactor.writeTH1D("TH_scalefactor_dphi2_incl_corrErr",false);


  data=ta_emud.getVmultiPlot();
  MC=ta_emuMC.getVmultiPlot();
  data.setDivideBinomial(false);
  MC.setDivideBinomial(false);
  scalefactor=data/MC;
  scalefactor.addGlobalRelError("rel_sys",0.01);
  scalefactor.writeTGraph("scalefactor_vmulti",false);
  scalefactor.addErrorContainer("corr_ratio_up",scalefactor*ta_emuMC.getCorrelationVmulti(),ratiomultiplier);
  scalefactor.writeTGraph("scalefactor_vmulti_incl_corrErr",false);
  scalefactor.writeTH1D("TH_scalefactor_vmulti_incl_corrErr",false);


  data=ta_emud.getDrlepPlot();
  MC=ta_emuMC.getDrlepPlot();
  data.setDivideBinomial(false);
  MC.setDivideBinomial(false);
  scalefactor=data/MC;
  scalefactor.addGlobalRelError("rel_sys",0.01);
  scalefactor.writeTGraph("scalefactor_drlep",false);
  scalefactor.addErrorContainer("corr_ratio_up",scalefactor*ta_emuMC.getCorrelationDrlep(),ratiomultiplier);
  scalefactor.writeTGraph("scalefactor_drlep_incl_corrErr",false);
  scalefactor.writeTH1D("TH_scalefactor_drlep_incl_corrErr",false);


  data=ta_emud.getJetmultiPlot();
  MC=ta_emuMC.getJetmultiPlot();
  data.setDivideBinomial(false);
  MC.setDivideBinomial(false);
  scalefactor=data/MC;
  scalefactor.addGlobalRelError("rel_sys",0.01);
  scalefactor.writeTGraph("scalefactor_jetmulti",false);
  scalefactor.addErrorContainer("corr_ratio_up",scalefactor*ta_emuMC.getCorrelationJetmulti(),ratiomultiplier);
  scalefactor.writeTGraph("scalefactor_jetmulti_incl_corrErr",false);
  scalefactor.writeTH1D("TH_scalefactor_jetmulti_incl_corrErr",false);

  f7->Close();

  
 TFile fc=TFile("trigger_SF_emu.root","RECREATE");
  fc.cd();
  SFdd.Write();
  fc.Close();
}

void do_triggerAnalyzer_SUSY(){
  analyze();
  miniscript(); //makes plots nice and puts output to directory

  if(breakat5fb) std::cout << "THESE ARE THE NUMBERS AND PLOTS FOR 5fb-1!!!" << std::endl;
}

float triggerAnalyzer::stopXSec(float stopMass){

  if(fabs(stopMass-100) < 2 ) return 559.757;
  else if(fabs(stopMass-105) < 2 ) return 448.456;
  else if(fabs(stopMass-110) < 2 ) return 361.917;
  else if(fabs(stopMass-115) < 2 ) return 293.281;//    = 15.9763/100.;}
  else if(fabs(stopMass-120) < 2 ) return 240.077;//    = 15.9212/100.;}
  else if(fabs(stopMass-125) < 2 ) return 197.122;//    = 15.7303/100.;}
  else if(fabs(stopMass-130) < 2 ) return 163.376;//    = 15.8101/100.;}
  else if(fabs(stopMass-135) < 2 ) return 135.791;//    = 15.8086/100.;}
  else if(fabs(stopMass-140) < 2 ) return 113.319;//    = 15.7234/100.;}
  else if(fabs(stopMass-145) < 2 ) return  95.0292; //   = 15.649/100.;}
  else if(fabs(stopMass-150) < 2 ) return  80.268;//    = 15.5946/100.;}
  else if(fabs(stopMass-155) < 2 ) return  68.0456; //   = 15.5232/100.;}
  else if(fabs(stopMass-160) < 2 ) return  58.01; //    = 15.3899/100.;}
  else if(fabs(stopMass-165) < 2 ) return  49.6639; //   = 15.3711/100.;}
  else if(fabs(stopMass-170) < 2 ) return  42.6441; //   = 15.3017/100.;}
  else if(fabs(stopMass-175) < 2 ) return  36.7994; //   = 15.1749/100.;}
  else if(fabs(stopMass-180) < 2 ) return  31.8695; //   = 15.2449/100.;}
  else if(fabs(stopMass-185) < 2 ) return  27.7028; //   = 15.063/100.;}
  else if(fabs(stopMass-190) < 2 ) return  24.1585; //   = 15.16/100.;}
  else if(fabs(stopMass-195) < 2 ) return  21.1597; //   = 14.9422/100.;}
  else if(fabs(stopMass-200) < 2 ) return  18.5245; //   = 14.9147/100.;}
  else if(fabs(stopMass-205) < 2 ) return  16.2439; //   = 15.117/100.;}
  else if(fabs(stopMass-210) < 2 ) return  14.3201; //   = 14.8495/100.;}
  else if(fabs(stopMass-215) < 2 ) return  12.6497; //   = 14.8689/100.;}
  else if(fabs(stopMass-220) < 2 ) return  11.1808; //   = 14.9108/100.;}
  else if(fabs(stopMass-225) < 2 ) return   9.90959;//   = 14.9662/100.;}
  else if(fabs(stopMass-230) < 2 ) return   8.78125;//   = 14.796/100.;}
  else if(fabs(stopMass-235) < 2 ) return   7.81646;//   = 14.7983/100.;}
  else if(fabs(stopMass-240) < 2 ) return   6.96892;//   = 14.7878/100.;}
  else if(fabs(stopMass-245) < 2 ) return   6.22701;//   = 14.7897/100.;}
  else if(fabs(stopMass-250) < 2 ) return   5.57596;//   = 14.7529/100.;}
  else if(fabs(stopMass-255) < 2 ) return   5.00108;//   = 14.729/100.;}
  else if(fabs(stopMass-260) < 2 ) return   4.48773;//   = 14.6782/100.;}
  else if(fabs(stopMass-265) < 2 ) return   4.03416;//   = 14.7964/100.;}
  else if(fabs(stopMass-270) < 2 ) return   3.63085;//   = 14.6565/100.;}
  else if(fabs(stopMass-275) < 2 ) return   3.2781; //   = 14.7341/100.;}
  else if(fabs(stopMass-280) < 2 ) return   2.95613;//   = 14.7816/100.;}
  else if(fabs(stopMass-285) < 2 ) return   2.67442;//   = 14.7661/100.;}
  else if(fabs(stopMass-290) < 2 ) return   2.42299;//   = 14.6805/100.;}
  else if(fabs(stopMass-295) < 2 ) return   2.19684;//   = 14.8465/100.;}
  else if(fabs(stopMass-300) < 2 ) return   1.99608;//   = 14.6905/100.;}
  else if(fabs(stopMass-305) < 2 ) return   1.81486;//   = 14.4434/100.;}
  else if(fabs(stopMass-310) < 2 ) return   1.64956;//   = 14.4769/100.;}
  else if(fabs(stopMass-315) < 2 ) return   1.50385;//   = 14.4549/100.;}
  else if(fabs(stopMass-320) < 2 ) return   1.3733; //   = 14.7503/100.;}
  else if(fabs(stopMass-325) < 2 ) return   1.25277;//   = 14.2875/100.;}
  else if(fabs(stopMass-330) < 2 ) return   1.14277;//   = 14.578/100.;}
  else if(fabs(stopMass-335) < 2 ) return   1.04713;//   = 14.3659/100.;}
  else if(fabs(stopMass-340) < 2 ) return   0.959617;  // = 14.3896/100.;}
  else if(fabs(stopMass-345) < 2 ) return   0.879793;  // = 14.3881/100.;}
  else if(fabs(stopMass-350) < 2 ) return   0.807323;  // = 14.3597/100.;}
  else if(fabs(stopMass-355) < 2 ) return   0.74141;//   = 14.368/100.;}
  else if(fabs(stopMass-360) < 2 ) return   0.681346;  // = 14.3357/100.;}
  else if(fabs(stopMass-365) < 2 ) return   0.626913;  // = 14.3627/100.;}
  else if(fabs(stopMass-370) < 2 ) return   0.576882;  // = 14.2712/100.;}
  else if(fabs(stopMass-375) < 2 ) return   0.531443;  // = 14.266/100.;}
  else if(fabs(stopMass-380) < 2 ) return   0.489973;  // = 14.3962/100.;}
  else if(fabs(stopMass-385) < 2 ) return   0.452072;  // = 14.2234/100.;}
  else if(fabs(stopMass-390) < 2 ) return   0.4176; //   = 14.3166/100.;}
  else if(fabs(stopMass-395) < 2 ) return   0.385775;  // = 14.3112/100.;}
  else if(fabs(stopMass-400) < 2 ) return   0.35683;//   = 14.2848/100.;}
  else if(fabs(stopMass-405) < 2 ) return   0.329881;  // = 14.2072/100.;}
  else if(fabs(stopMass-410) < 2 ) return   0.305512;  // = 14.2648/100.;}
  else if(fabs(stopMass-415) < 2 ) return   0.283519;  // = 14.102/100.;}
  else if(fabs(stopMass-420) < 2 ) return   0.262683;  // = 14.3075/100.;}
  else if(fabs(stopMass-425) < 2 ) return   0.243755;  // = 14.0504/100.;}
  else if(fabs(stopMass-430) < 2 ) return   0.226367;  // = 14.0494/100.;}
  else if(fabs(stopMass-435) < 2 ) return   0.209966;  // = 14.0334/100.;}
  else if(fabs(stopMass-440) < 2 ) return   0.195812;  // = 14.0772/100.;}
  else if(fabs(stopMass-445) < 2 ) return   0.181783;  // = 14.1771/100.;}
  else if(fabs(stopMass-450) < 2 ) return   0.169668;  // = 14.2368/100.;}
  else if(fabs(stopMass-455) < 2 ) return   0.158567;  // = 14.2609/100.;}
  else if(fabs(stopMass-460) < 2 ) return   0.147492;  // = 14.4105/100.;}
  else if(fabs(stopMass-465) < 2 ) return   0.137392;  // = 14.4772/100.;}
  else if(fabs(stopMass-470) < 2 ) return   0.128326;  // = 14.5144/100.;}
  else if(fabs(stopMass-475) < 2 ) return   0.119275;  // = 14.6664/100.;}
  else if(fabs(stopMass-480) < 2 ) return   0.112241;  // = 14.6307/100.;}
  else if(fabs(stopMass-485) < 2 ) return   0.104155;  // = 14.7581/100.;}
  else if(fabs(stopMass-490) < 2 ) return   0.0977878; // = 14.7977/100.;}
  else if(fabs(stopMass-495) < 2 ) return   0.091451;  // = 14.8963/100.;}
  else if(fabs(stopMass-500) < 2 ) return   0.0855847; // = 14.9611/100.;}
  else if(fabs(stopMass-505) < 2 ) return   0.0801322; // = 15.0389/100.;}
  else if(fabs(stopMass-510) < 2 ) return   0.0751004; // = 15.1402/100.;}
  else if(fabs(stopMass-515) < 2 ) return   0.0703432; // = 15.2139/100.;}
  else if(fabs(stopMass-520) < 2 ) return   0.0660189; // = 15.3368/100.;}
  else if(fabs(stopMass-525) < 2 ) return   0.0618641; // = 15.4135/100.;}
  else if(fabs(stopMass-530) < 2 ) return   0.0580348; // = 15.4422/100.;}
  else if(fabs(stopMass-535) < 2 ) return   0.0545113; // = 15.5446/100.;}
  else if(fabs(stopMass-540) < 2 ) return   0.0511747; // = 15.6283/100.;}
  else if(fabs(stopMass-545) < 2 ) return   0.0481537; // = 15.726/100.;}
  else if(fabs(stopMass-550) < 2 ) return   0.0452067; // = 15.8177/100.;}
  else if(fabs(stopMass-555) < 2 ) return   0.0424781; // = 15.9022/100.;}
  else if(fabs(stopMass-560) < 2 ) return   0.0399591; // = 16.0067/100.;}
  else if(fabs(stopMass-565) < 2 ) return   0.0376398; // = 16.0367/100.;}
  else if(fabs(stopMass-570) < 2 ) return   0.0354242; // = 16.137/100.;}
  else if(fabs(stopMass-575) < 2 ) return   0.0333988; // = 16.2132/100.;}
  else if(fabs(stopMass-580) < 2 ) return   0.0313654; // = 16.3135/100.;}
  else if(fabs(stopMass-585) < 2 ) return   0.0295471; // = 16.4264/100.;}
  else if(fabs(stopMass-590) < 2 ) return   0.0279395; // = 16.4546/100.;}
  else if(fabs(stopMass-595) < 2 ) return   0.0263263; // = 16.567/100.;}
  else if(fabs(stopMass-600) < 2 ) return   0.0248009; // = 16.6406/100.;}
  else if(fabs(stopMass-605) < 2 ) return   0.0233806; // = 16.7295/100.;}
  else if(fabs(stopMass-610) < 2 ) return   0.0220672; // = 16.8447/100.;}
  else if(fabs(stopMass-615) < 2 ) return   0.0208461; // = 16.9276/100.;}
  else if(fabs(stopMass-620) < 2 ) return   0.0196331; // = 17.0459/100.;}
  else if(fabs(stopMass-625) < 2 ) return   0.0185257; // = 17.0835/100.;}
  else if(fabs(stopMass-630) < 2 ) return   0.0175075; // = 17.1478/100.;}
  else if(fabs(stopMass-635) < 2 ) return   0.0164955; // = 17.2753/100.;}
  else if(fabs(stopMass-640) < 2 ) return   0.0155809; // = 17.3814/100.;}
  else if(fabs(stopMass-645) < 2 ) return   0.0147721; // = 17.4885/100.;}
  else if(fabs(stopMass-650) < 2 ) return   0.0139566; // = 17.56/100.;}
  else if(fabs(stopMass-655) < 2 ) return   0.0132456; // = 17.6129/100.;}
  else if(fabs(stopMass-660) < 2 ) return   0.0125393; // = 17.7363/100.;}
  else if(fabs(stopMass-665) < 2 ) return   0.0118287; // = 17.7959/100.;}
  else if(fabs(stopMass-670) < 2 ) return   0.0112223; // = 17.8974/100.;}
  else if(fabs(stopMass-675) < 2 ) return   0.0106123; // = 17.9891/100.;}
  else if(fabs(stopMass-680) < 2 ) return   0.0100516; // = 18.0618/100.;}
  else if(fabs(stopMass-685) < 2 ) return   0.0095256; // = 18.1714/100.;}
  else if(fabs(stopMass-690) < 2 ) return   0.0090306; // = 18.2108/100.;}
  else if(fabs(stopMass-695) < 2 ) return   0.00856339;// = 18.3365/100.;}
  else if(fabs(stopMass-700) < 2 ) return   0.0081141; // = 18.4146/100.;}
  else if(fabs(stopMass-705) < 2 ) return   0.00769525;// = 18.4937/100.;}
  else if(fabs(stopMass-710) < 2 ) return   0.00730084;// = 18.6195/100.;}
  else if(fabs(stopMass-715) < 2 ) return   0.00692243;// = 18.7005/100.;}
  else if(fabs(stopMass-720) < 2 ) return   0.00656729;// = 18.819/100.;}
  else if(fabs(stopMass-725) < 2 ) return   0.00623244;// = 18.8796/100.;}
  else if(fabs(stopMass-730) < 2 ) return   0.00591771;// = 18.996/100.;}
  else if(fabs(stopMass-735) < 2 ) return   0.00561049;// = 19.0787/100.;}
  else if(fabs(stopMass-740) < 2 ) return   0.00532605;// = 19.1995/100.;}
  else if(fabs(stopMass-745) < 2 ) return   0.00506044;// = 19.2916/100.;}
  else if(fabs(stopMass-750) < 2 ) return   0.00480639;// = 19.4088/100.;}
  else if(fabs(stopMass-755) < 2 ) return   0.00455979;// = 19.508/100.;}
  else if(fabs(stopMass-760) < 2 ) return   0.00433688;// = 19.632/100.;}
  else if(fabs(stopMass-765) < 2 ) return   0.00412174;// = 19.7141/100.;}
  else if(fabs(stopMass-770) < 2 ) return   0.00391839;// = 19.8299/100.;}
  else if(fabs(stopMass-775) < 2 ) return   0.00372717;// = 19.9097/100.;}
  else if(fabs(stopMass-780) < 2 ) return   0.00354211;// = 20.0016/100.;}
  else if(fabs(stopMass-785) < 2 ) return   0.00336904;// = 20.123/100.;}
  else if(fabs(stopMass-790) < 2 ) return   0.00320476;// = 20.2271/100.;}
  else if(fabs(stopMass-795) < 2 ) return   0.00304935;// = 20.4479/100.;}
  else if(fabs(stopMass-800) < 2 ) return   0.00289588;// = 20.516/100.;}
  else if(fabs(stopMass-805) < 2 ) return   0.00275424;// = 20.5444/100.;}
  else if(fabs(stopMass-810) < 2 ) return   0.0026184; // = 20.8204/100.;}
  else if(fabs(stopMass-815) < 2 ) return   0.00249291;// = 21.0063/100.;}
  else if(fabs(stopMass-820) < 2 ) return   0.00237168;// = 21.0865/100.;}
  else if(fabs(stopMass-825) < 2 ) return   0.00226163;// = 21.0511/100.;}
  else if(fabs(stopMass-830) < 2 ) return   0.00214607;// = 21.3705/100.;}
  else if(fabs(stopMass-835) < 2 ) return   0.00204589;// = 21.3026/100.;}
  else if(fabs(stopMass-840) < 2 ) return   0.00195172;// = 21.6053/100.;}
  else if(fabs(stopMass-845) < 2 ) return   0.0018573; // = 21.8177/100.;}
  else if(fabs(stopMass-850) < 2 ) return   0.00176742;// = 21.836/100.;}
  else if(fabs(stopMass-855) < 2 ) return   0.00168383;// = 22.1411/100.;}
  else if(fabs(stopMass-860) < 2 ) return   0.00160403;// = 22.0506/100.;}
  else if(fabs(stopMass-865) < 2 ) return   0.00153063;// = 22.3461/100.;}
  else if(fabs(stopMass-870) < 2 ) return   0.00145772;// = 22.6206/100.;}
  else if(fabs(stopMass-875) < 2 ) return   0.0013878; // = 22.5422/100.;}
  else if(fabs(stopMass-880) < 2 ) return   0.00132077;// = 23.2161/100.;}
  else if(fabs(stopMass-885) < 2 ) return   0.00126234;// = 23.1283/100.;}
  else if(fabs(stopMass-890) < 2 ) return   0.00120568;// = 23.8404/100.;}
  else if(fabs(stopMass-895) < 2 ) return   0.00114627;// = 23.7327/100.;}
  else if(fabs(stopMass-900) < 2 ) return   0.00109501;// = 23.9439/100.;}
  else if(fabs(stopMass-905) < 2 ) return   0.001044;  // = 24.1049/100.;}
  else if(fabs(stopMass-910) < 2 ) return   0.000996193; // = 24.2789/100.;}
  else if(fabs(stopMass-915) < 2 ) return   0.00095071; // = 24.5443/100.;}
  else if(fabs(stopMass-920) < 2 ) return   0.000907494; // = 24.7597/100.;}
  else if(fabs(stopMass-925) < 2 ) return   0.000866391; // = 24.877/100.;}
  else if(fabs(stopMass-930) < 2 ) return   0.000826533; // = 25.0849/100.;}
  else if(fabs(stopMass-935) < 2 ) return   0.000789573; // = 25.2885/100.;}
  else if(fabs(stopMass-940) < 2 ) return   0.000753768; // = 25.4768/100.;}
  else if(fabs(stopMass-945) < 2 ) return   0.000719675; // = 25.6582/100.;}
  else if(fabs(stopMass-950) < 2 ) return   0.000687022; // = 25.8341/100.;}
  else if(fabs(stopMass-955) < 2 ) return   0.000656279; // = 26.0372/100.;}
  else if(fabs(stopMass-960) < 2 ) return   0.000626876; // = 26.2059/100.;}
  else if(fabs(stopMass-965) < 2 ) return   0.000598955; // = 26.3653/100.;}
  else if(fabs(stopMass-970) < 2 ) return   0.000571551; // = 26.5169/100.;}
  else if(fabs(stopMass-975) < 2 ) return   0.000546728; // = 26.7985/100.;}
  else if(fabs(stopMass-980) < 2 ) return   0.000522495; // = 26.9218/100.;}
  else if(fabs(stopMass-985) < 2 ) return   0.000499017; // = 27.1036/100.;}
  else if(fabs(stopMass-990) < 2 ) return   0.000476255; // = 27.3032/100.;}
  else if(fabs(stopMass-995) < 2 ) return   0.000455959; // = 27.4544/100.;}
  else if(fabs(stopMass-1000) < 2 ) return  0.000435488; // = 27.6595/100.;}
  else return 0;
}
