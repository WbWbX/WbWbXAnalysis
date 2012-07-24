#include "../DataFormats/src/classes.h" //gets all the dataformats
#include "../DataFormats/interface/elecRhoIsoAdder.h"
#include "../plugins/reweightPU.h"
#include "../plugins/leptonSelector.h"
#include "../plugins/miscUtils.h"
#include "../plugins/containerStackVector.h"
#include "../plugins/JERAdjuster.h"
#include "TTree.h"
#include "TFile.h"
#include <fstream>

namespace top{
  typedef std::vector<top::NTElectron>::iterator NTElectronIt;
  typedef std::vector<top::NTMuon>::iterator NTMuonIt;
  typedef std::vector<top::NTJet>::iterator NTJetIt;
  typedef std::vector<top::NTTrack>::iterator NTTrackIt;
  typedef std::vector<top::NTSuClu>::iterator NTSuCluIt;
}

//need function for find container in vector by name and create if not existing

class MainAnalyzer{

public:
  MainAnalyzer(){filelist_="";dataname_="data";analysisPlots = new top::container1DStackVector();}
  MainAnalyzer(TString Name, TString additionalinfo){name_=Name;additionalinfo_=additionalinfo;analysisPlots = new top::container1DStackVector(Name);}
  ~MainAnalyzer(){if(analysisPlots) delete analysisPlots;};
  void setName(TString Name, TString additionalinfo){name_=Name;additionalinfo_=additionalinfo;analysisPlots->setName(Name);}
  void setLumi(double Lumi){lumi_=Lumi;}

  void analyze(TString, TString, int, double);

  void setFileList(const char* filelist){filelist_=filelist;}

  top::container1DStackVector * getPlots(){return analysisPlots;}

  void start();

  void clear(){analysisPlots->clear();}

  void copySettings(MainAnalyzer &);

  MainAnalyzer & operator = (const MainAnalyzer &){std::cout <<"use copySettings() instead!" << std::endl;return *this;}

  void setAdditionalInfoString(TString add){additionalinfo_=add;} //!for adding things like JEC up or other systematics options

  top::PUReweighter & getPUReweighter(){return puweighter_;}

  top::JERAdjuster & getJERAdjuster(){return jeradjuster_;}

private:

  bool isInInfo(TString s){return additionalinfo_.Contains(s);}

  TString name_,dataname_;
  double lumi_;

  const char * filelist_;
  
  top::PUReweighter puweighter_;
  top::JERAdjuster jeradjuster_;

  top::container1DStackVector * analysisPlots;

  TString additionalinfo_;

};

void MainAnalyzer::start(){
  using namespace std;
  if(analysisPlots) clear();
  else cout << "warning analysisPlots is null" << endl;
  analysisPlots->setName(name_);
  ifstream inputfiles (filelist_);
  string filename;
  string legentry;
  int color;
  double norm;
  string oldfilename="";
  if(inputfiles.is_open()){
    while(inputfiles.good()){
      inputfiles >> filename >> legentry >> color >> norm;

      if(oldfilename != filename) analyze((TString) filename, (TString)legentry, color, norm);
      oldfilename=filename;
    }
  }
  else{
    cout << "MainAnalyzer::start(): input file list not found" << endl;
  }
  
}
void MainAnalyzer::copySettings(MainAnalyzer & analyzer){
  dataname_=analyzer.dataname_;
  puweighter_=analyzer.puweighter_;
  filelist_=analyzer.filelist_;
}


void  MainAnalyzer::analyze(TString inputfile, TString legendname, int color, double norm){ // do analysis here and store everything in analysisPlots

  using namespace std;
  using namespace top;
  //define containers here

  //   define binnings

  vector<float> etabinselec;
  etabinselec << -2.5 << -1.8 << -1 << 1 << 1.8 << 2.5 ;
  vector<float> etabinsmuon;
  etabinsmuon << -2.4 << -1.5 << -0.8 << 0.8 << 1.5 << 2.4;
  vector<float> ptbinsfull;
  ptbinsfull << 0 << 10 << 20 << 25 << 30 << 35 << 40 << 45 << 50 << 60 << 70 << 100 << 200;
  vector<float> ptbins;
  ptbins << 20 << 25 << 30 << 35 << 40 << 45 << 50 << 60 << 70 << 100 << 200;
  vector<float> massbins;
  for(int i=0;i<40;++i) massbins << 5*i;
  vector<float> multibinsjets;
  for(float i=-0.5;i<7.5;i++) multibinsjets << i;
  vector<float> multibinsvertx;
  for(float i=0.5;i<50.5;i++) multibinsvertx << i;
  vector<float> multibinsbtag;
  for(float i=-0.5;i<4.5;i++) multibinsbtag << i;
  vector<float> isobins;
  for(float i=0;i<50;i++) isobins << i/50;
  vector<float> selectionbins;
  for(float i=-0.5;i<11.5;i++) selectionbins << i;


  //and so on

  //invoke c_list for automatically registering  all created containers;

  c_list = new List<container1D>();

  ///  define containers

  container1D selection(selectionbins, "selection steps", "step", "N_{sel}");

  container1D eleceta0(etabinselec, "electron eta step 0", "#eta_{l}","N_{e}");
  container1D elecpt0(ptbinsfull, "electron pt step 0", "p_{T} [GeV]", "N_{e}");
  container1D eleciso0(isobins, "electron isolation step 0", "Iso", "N_{e}");
  container1D elecid0(isobins, "electron mva id step 0", "Id", "N_{e}");

  container1D muoneta0(etabinsmuon, "muon eta step 0", "#eta_{l}","N_{#mu}");
  container1D muonpt0(ptbinsfull, "muon pt step 0", "p_{T} [GeV]", "N_{#mu}");
  container1D muoniso0(isobins, "muon isolation step 0", "Iso", "N_{#mu}");

  container1D vertexmulti0(multibinsvertx, "vertex multiplicity step 0", "n_{vtx}", "N_{evt}");

  container1D eleceta1(etabinselec, "electron eta step 1", "#eta_{l}","N_{e}");
  container1D elecpt1(ptbins, "electron pt step 1", "p_{T} [GeV]", "N_{e}");
  container1D eleciso1(isobins, "electron isolation step 1", "Iso", "N_{e}");
  container1D muoneta1(etabinsmuon, "muon eta step 1", "#eta_{l}","N_{#mu}");
  container1D muonpt1(ptbins, "muon pt step 1", "p_{T} [GeV]", "N_{#mu}");
  container1D muoniso1(isobins, "muon isolation step 1", "Iso", "N_{#mu}");

  container1D vertexmulti1(multibinsvertx, "vertex multiplicity step 1", "n_{vtx}", "N_{evt}");


  container1D eleceta2(etabinselec, "electron eta step 2", "#eta_{l}","N_{e}");
  container1D elecpt2(ptbins, "electron pt step 2", "p_{T} [GeV]", "N_{e}");
  container1D eleciso2(isobins, "electron isolation step 2", "Iso", "N_{e}");
  container1D muoneta2(etabinsmuon, "muon eta step 2", "#eta_{l}","N_{#mu}");
  container1D muonpt2(ptbins, "muon pt step 2", "p_{T} [GeV]", "N_{#mu}");
  container1D muoniso2(isobins, "muon isolation step 2", "Iso", "N_{#mu}");

  container1D vertexmulti2(multibinsvertx, "vertex multiplicity step 2", "n_{vtx}", "N_{evt}");
  container1D invmass2(massbins, "dilepton invariant mass step 2", "m_{ll} [GeV]", "N_{evt}");


  container1D eleceta3(etabinselec, "electron eta step 3", "#eta_{l}","N_{e}");
  container1D elecpt3(ptbins, "electron pt step 3", "p_{T} [GeV]", "N_{e}");
  container1D eleciso3(isobins, "electron isolation step 3", "Iso", "N_{e}");
  container1D muoneta3(etabinsmuon, "muon eta step 3", "#eta_{l}","N_{#mu}");
  container1D muonpt3(ptbins, "muon pt step 3", "p_{T} [GeV]", "N_{#mu}");
  container1D muoniso3(isobins, "muon isolation step 3", "Iso", "N_{#mu}");

  container1D vertexmulti3(multibinsvertx, "vertex multiplicity step 3", "n_{vtx}", "N_{evt}");
  container1D invmass3(massbins, "dilepton invariant mass step 3", "m_{ll} [GeV]", "N_{evt}");


  container1D jetmulti3(multibinsjets, "jet multiplicity step 3", "n_{jets}","N_{jets}");
  container1D jetpt3(ptbins, "jet pt step 3", "p_{T} [GeV]","N_{jets}");
  container1D jeteta3(etabinselec, "jet eta step 3", "#eta_{jet}","N_{jets}");


  container1D eleceta10(etabinselec, "electron eta step 10", "#eta_{l}","N_{e}");
  container1D elecpt10(ptbins, "electron pt step 10", "p_{T} [GeV]", "N_{e}");
  container1D eleciso10(isobins, "electron isolation step 10", "Iso", "N_{e}");
  container1D muoneta10(etabinsmuon, "muon eta step 10", "#eta_{l}","N_{#mu}");
  container1D muonpt10(ptbins, "muon pt step 10", "p_{T} [GeV]", "N_{#mu}");
  container1D muoniso10(isobins, "muon isolation step 10", "Iso", "N_{#mu}");

  container1D vertexmulti10(multibinsvertx, "vertex multiplicity step 10", "n_{vtx}", "N_{evt}");
  container1D invmass10(massbins, "dilepton invariant mass step 10", "m_{ll} [GeV]", "N_{evt}");


  container1D eleceta4(etabinselec, "electron eta step 4", "#eta_{l}","N_{e}");
  container1D elecpt4(ptbins, "electron pt step 4", "p_{T} [GeV]", "N_{e}");
  container1D eleciso4(isobins, "electron isolation step 4", "Iso", "N_{e}");
  container1D muoneta4(etabinsmuon, "muon eta step 4", "#eta_{l}","N_{#mu}");
  container1D muonpt4(ptbins, "muon pt step 4", "p_{T} [GeV]", "N_{#mu}");
  container1D muoniso4(isobins, "muon isolation step 4", "Iso", "N_{#mu}");

  container1D vertexmulti4(multibinsvertx, "vertex multiplicity step 4", "n_{vtx}", "N_{evt}");
  container1D invmass4(massbins, "dilepton invariant mass step 4", "m_{ll} [GeV]", "N_{evt}");

  container1D jetmulti4(multibinsjets, "jet multiplicity step 4", "n_{jets}","N_{jets}");
  container1D jetpt4(ptbins, "jet pt step 4", "p_{T} [GeV]","N_{jets}");
  container1D jeteta4(etabinselec, "jet eta step 4", "#eta_{jet}","N_{jets}");



  container1D met6(ptbins, "missing transverse energie step 6","E_{T,miss} [GeV]", "N_{evt}");
  container1D met7(ptbins, "missing transverse energie step 7","E_{T,miss} [GeV]", "N_{evt}");



  container1D btagmulti7(multibinsbtag, "b-jet multiplicity step 7", "n_{b-tags}", "N_{jets}");


  container1D btagmulti8(multibinsbtag, "b-jet multiplicity step 8", "n_{b-tags}", "N_{jets}");

  //get the lepton selector (maybe directly in the code.. lets see)

  leptonSelector lepSel;
  lepSel.setUseRhoIsoForElectrons(true);

  /////some boolians
  bool b_mumu=false;bool b_ee=false;bool b_emu=false;;
  if(isInInfo("mumu")){
    b_mumu=true;
    cout << "\nrunning in mumu mode" <<endl;
  }
  else if (isInInfo("ee")){
    b_ee=true;
    cout << "\nrunning in ee mode" <<endl;
  }
  else if (isInInfo("emu")){
    b_emu=true;
    cout << "\nrunning in emu mode" <<endl;
  }



  TFile *f=TFile::Open(inputfile);
  cout << "running on: " << inputfile << "   legend: " << legendname << endl;
  TTree * tnorm = (TTree*) f->Get("preCutPUInfo/preCutPUInfo");
  if(legendname!="data"){

    norm = lumi_ * norm / (tnorm->GetEntries());
  }
  else{
      norm=1;
  }
  // get main analysis tree

  norm=1; ///FOR TESTING
  cout << "NORM IS SET TO 1 FOR ALL MC!! JUST FOR TESTING!" <<endl;

  TTree * t = (TTree*) f->Get("PFTree/pfTree");

  vector<NTMuon> * pMuons = 0;
  t->SetBranchAddress("NTMuons",&pMuons); 
  vector<NTElectron> * pElectrons = 0;
  t->SetBranchAddress("NTElectrons",&pElectrons);
  vector<NTJet> * pJets=0;
  t->SetBranchAddress("NTJets",&pJets);
  NTMet * pMet = 0;
  t->SetBranchAddress("NTMet",&pMet); 
  NTEvent * pEvent = 0;
  t->SetBranchAddress("NTEvent",&pEvent); 


  // start main loop /////////////////////////////////////////////////////////
  for(float entry=0;entry<t->GetEntries();entry++){
    t->GetEntry(entry);
    
    double puweight;
    if(legendname==dataname_) puweight=1;
    else puweight = puweighter_.getPUweight(pEvent->truePU());

    //lepton collections
    bool isMC=true;
    if(legendname==dataname_) isMC=false;
    elecRhoIsoAdder rho(isMC,false);

    vector<NTElectron> kinelectrons;

    ////// clean electrons against muons and make kin cuts on the fly//////
    for(NTElectronIt elec=pElectrons->begin();elec<pElectrons->end();++elec){
      if(elec->pt() < 20) continue;
      if(fabs(elec->eta()) > 2.5) continue;
      if(!noOverlap(elec, *pMuons, 0.1)) continue;
      kinelectrons.push_back(*elec);
    }

    rho.setRho(pEvent->isoRho(2));
    rho.addRhoIso(kinelectrons);
    vector<NTElectron> idelectrons   =lepSel.selectIDElectrons(kinelectrons);

    vector<NTMuon> kinmuons         = lepSel.selectKinMuons(*pMuons);
    vector<NTMuon> idmuons           =lepSel.selectIDMuons(kinmuons);

    vector<NTElectron> isoelectrons  =lepSel.selectIsolatedElectrons(idelectrons);
    vector<NTMuon> isomuons          =lepSel.selectIsolatedMuons(idmuons);

    /////////TRIGGER CUT and kin leptons STEP 0/////////////////////////////
    if(b_ee){
      if(kinelectrons.size()< 2) continue;
      //trg
    }
    if(b_mumu){
      if(kinmuons.size() < 2) continue;
      //trg

    }
    if(b_emu){
      if(kinelectrons.size() + kinmuons.size() < 2) continue;
      //trg

    }

    for(NTElectronIt elec=kinelectrons.begin();elec<kinelectrons.end();++elec){
      eleceta0.fill(elec->eta(), puweight);
      elecpt0.fill(elec->pt(),puweight);
      eleciso0.fill(elec->rhoIso03(),puweight);
      elecid0.fill(elec->mvaId(),puweight);
    //some other fills
    }

    for(NTMuonIt muon=kinmuons.begin();muon<kinmuons.end();++muon){
      muoneta0.fill(muon->eta(), puweight);
      muonpt0.fill(muon->pt(),puweight);
      muoniso0.fill(muon->isoVal04(),puweight);
    }

    vertexmulti0.fill(pEvent->vertexMulti(),puweight);
    selection.fill(0,puweight);


    //////////two ID leptons STEP 1///////////////////////////////

    if(b_ee && idelectrons.size() < 2) continue;
    if(b_mumu && idmuons.size() < 2 ) continue;
    if(b_emu && idmuons.size() + idelectrons.size() < 2) continue;


    for(NTElectronIt elec=idelectrons.begin();elec<idelectrons.end();++elec){
      eleceta1.fill(elec->eta(), puweight);
      elecpt1.fill(elec->pt(),puweight);
      eleciso1.fill(elec->rhoIso03(),puweight);
    //some other fills
    }

    for(NTMuonIt muon=idmuons.begin();muon<idmuons.end();++muon){
      muoneta1.fill(muon->eta(), puweight);
      muonpt1.fill(muon->pt(),puweight);
      muoniso1.fill(muon->isoVal04(),puweight);
    }


    vertexmulti1.fill(pEvent->vertexMulti(),puweight);
    selection.fill(1,puweight);

    //////// require two iso leptons  STEP 2  //////

    if(b_ee && isoelectrons.size() < 2) continue;
    if(b_mumu && isomuons.size() < 2 ) continue;
    if(b_emu && isomuons.size() + isoelectrons.size() < 2) continue;

    //make pair
    pair<vector<NTElectron>, vector<NTMuon> > leppair;
    leppair = lepSel.getOppoQHighestPtPair(isoelectrons, isomuons);

    double invLepMass=0;

    if(b_ee){
      if(leppair.first.size() <2) continue;
      invLepMass=(leppair.first[0].p4() + leppair.first[1].p4()).M();
    }
    else if(b_mumu){
      if(leppair.second.size() < 2) continue;
      invLepMass=(leppair.second[0].p4() + leppair.second[1].p4()).M();
    }
    else if(b_emu){
         if(leppair.first.size() < 1 || leppair.second.size() < 1) continue;
         invLepMass=(leppair.first[0].p4() + leppair.second[0].p4()).M();
    }

    for(NTElectronIt elec=isoelectrons.begin();elec<isoelectrons.end();++elec){
      eleceta2.fill(elec->eta(), puweight);
      elecpt2.fill(elec->pt(),puweight);
      eleciso2.fill(elec->rhoIso03(),puweight);
    //some other fills
    }

    for(NTMuonIt muon=isomuons.begin();muon<isomuons.end();++muon){
      muoneta2.fill(muon->eta(), puweight);
      muonpt2.fill(muon->pt(),puweight);
      muoniso2.fill(muon->isoVal04(),puweight);
    }

    invmass2.fill(invLepMass,puweight);
    vertexmulti2.fill(pEvent->vertexMulti(),puweight);
    selection.fill(2,puweight);


    ///////// 20 GeV cut /// STEP 3 ///////////////////

    if(invLepMass < 20) continue;

    for(NTElectronIt elec=isoelectrons.begin();elec<isoelectrons.end();++elec){
      eleceta3.fill(elec->eta(), puweight);
      elecpt3.fill(elec->pt(),puweight);
      eleciso3.fill(elec->rhoIso03(),puweight);
    //some other fills
    }

    for(NTMuonIt muon=isomuons.begin();muon<isomuons.end();++muon){
      muoneta3.fill(muon->eta(), puweight);
      muonpt3.fill(muon->pt(),puweight);
      muoniso3.fill(muon->isoVal04(),puweight);
    }

    invmass3.fill(invLepMass,puweight);
    vertexmulti3.fill(pEvent->vertexMulti(),puweight);
    selection.fill(3,puweight);

    vector<NTJet> nolidjets;

    for(NTJetIt jet=pJets->begin();jet<pJets->end();++jet){
      if(!(jet->id())) continue;
      if(!noOverlap(jet, isomuons, 0.3)) continue;
      if(!noOverlap(jet, isoelectrons, 0.3)) continue;
      jetpt3.fill(jet->pt(),puweight);
      jeteta3.fill(jet->eta(),puweight);
      NTJet jerjet=*jet;

      jeradjuster_.recalculate(jerjet);
      nolidjets.push_back(jerjet);
    }
    jetmulti3.fill(nolidjets.size(),puweight);

    ////////////////////Z Selection//////////////////
    if(invLepMass > 76 && invLepMass < 106){
      
      for(NTElectronIt elec=isoelectrons.begin();elec<isoelectrons.end();++elec){
	eleceta10.fill(elec->eta(), puweight);
	elecpt10.fill(elec->pt(),puweight);
	eleciso10.fill(elec->rhoIso03(),puweight);
	//some other fills
      }
      
      for(NTMuonIt muon=isomuons.begin();muon<isomuons.end();++muon){
	muoneta10.fill(muon->eta(), puweight);
	muonpt10.fill(muon->pt(),puweight);
	muoniso10.fill(muon->isoVal04(),puweight);
      }
      
      invmass10.fill(invLepMass,puweight);
      vertexmulti10.fill(pEvent->vertexMulti(),puweight);
      selection.fill(10,puweight);

      if(!b_emu) continue; // Z Veto

    }
    ////////////////////Z Veto Cut STEP 4 (incl. hard jets)////////////////////////////////////
    
    for(NTElectronIt elec=isoelectrons.begin();elec<isoelectrons.end();++elec){
      eleceta4.fill(elec->eta(), puweight);
      elecpt4.fill(elec->pt(),puweight);
      eleciso4.fill(elec->rhoIso03(),puweight);
    //some other fills
    }

    for(NTMuonIt muon=isomuons.begin();muon<isomuons.end();++muon){
      muoneta4.fill(muon->eta(), puweight);
      muonpt4.fill(muon->pt(),puweight);
      muoniso4.fill(muon->isoVal04(),puweight);
    }

    invmass4.fill(invLepMass,puweight);
    vertexmulti4.fill(pEvent->vertexMulti(),puweight);
    selection.fill(4,puweight);

    /////// create jets ////////

    vector<NTJet> hardjets;

    for(NTJetIt jet=nolidjets.begin();jet<nolidjets.end();++jet){
      if(jet->pt()<30) continue;
      if(fabs(jet->eta())>2.5) continue;
      jetpt4.fill(jet->pt(),puweight);
      jeteta4.fill(jet->eta(),puweight);
      hardjets.push_back(*jet);
    }

    jetmulti4.fill(hardjets.size(),puweight);


    ///////////////////// at least one jet cut STEP 5 ////////////

    if(hardjets.size() < 1) continue;

    selection.fill(5,puweight);




    /////////////////////// at least two jets STEP 6 /////////////

    if(hardjets.size() < 2) continue;

    selection.fill(6,puweight);



    met6.fill(pMet->met(), puweight);


    //////////////////// MET cut STEP 7//////////////////////////////////

    if(pMet->met() < 40 && !b_emu) continue;

    selection.fill(7, puweight);


    met7.fill(pMet->met(), puweight);
    

    ///make btagged jets //
    vector<NTJet> btaggedjets;
    for(NTJetIt jet = hardjets.begin();jet<hardjets.end();++jet){
      if(jet->btag() > 0.244) continue;
      btaggedjets.push_back(*jet);
    }

    btagmulti7.fill(btaggedjets.size(),puweight);

    ///////////////////// btag cut STEP 8 //////////////////////////

    if(btaggedjets.size() < 1) continue;

    btagmulti8.fill(btaggedjets.size(),puweight);


  } //main event loop ends

  // Fill all containers in the stackVector

  analysisPlots->addList(c_list,legendname,color,norm);


  delete t;
  delete tnorm;
  f->Close();
  delete f;

  //and so on

}


void syncAnalyzer(){

  TString outfile = "syncOut_jer.root";

  MainAnalyzer analyzermumu("default_mumu","mumu");
  analyzermumu.setLumi(5000);
  analyzermumu.setFileList("inputfiles.txt");
  analyzermumu.getPUReweighter().setDataTruePUInput("/afs/naf.desy.de/user/k/kieseler/public/dataPileUp_23_06.root");
  analyzermumu.getPUReweighter().setMCDistrSum12();

  analyzermumu.getPUReweighter().setAllOne(); //TESTING!!!!! SYNC FILE

  analyzermumu.start();
  analyzermumu.getPlots()->writeAllToTFile(outfile,true); // after that just update file
  
  MainAnalyzer analyzeree("default_ee", "ee somemoreoptions");
  analyzeree.copySettings(analyzermumu);
  analyzeree.start();
  analyzeree.getPlots()->writeAllToTFile(outfile);
  
  MainAnalyzer analyzeremu("default_emu", "emu somemoreoptions");
  analyzeremu.copySettings(analyzermumu);
  analyzeremu.start();
  analyzeremu.getPlots()->writeAllToTFile(outfile);



}
