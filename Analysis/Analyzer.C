#include "../DataFormats/src/classes.h" //gets all the dataformats
#include "../DataFormats/interface/elecRhoIsoAdder.h"
#include "../plugins/reweightPU.h"
#include "../plugins/leptonSelector.h"
#include "../Tools/interface/miscUtils.h"
#include "../Tools/interface/containerStackVector.h"
#include "../plugins/JERAdjuster.h"
#include "../plugins/JECUncertainties.h"
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

//// run in batch mode otherwise it is dramatically slowed down by all the drawing stuff; the latter might also produce seg violations in the canvas libs.

class MainAnalyzer{

public:
  MainAnalyzer(){filelist_="";dataname_="data";analysisplots_ = new top::container1DStackVector();puweighter_= new top::PUReweighter();jeradjuster_= new top::JERAdjuster();jecuncertainties_=new top::JECUncertainties();}
  MainAnalyzer(const MainAnalyzer &);
  MainAnalyzer(TString Name, TString additionalinfo){name_=Name;additionalinfo_=additionalinfo;dataname_="data";analysisplots_ = new top::container1DStackVector(Name);puweighter_= new top::PUReweighter();jeradjuster_= new top::JERAdjuster();jecuncertainties_=new top::JECUncertainties();}
  ~MainAnalyzer(){if(analysisplots_) delete analysisplots_;if(puweighter_) delete puweighter_;if(jeradjuster_) delete jeradjuster_;if(jecuncertainties_) delete jecuncertainties_;};
  void setName(TString Name, TString additionalinfo){name_=Name;additionalinfo_=additionalinfo;analysisplots_->setName(Name);}
  void setLumi(double Lumi){lumi_=Lumi;}

  void analyze(TString, TString, int, double);

  void setFileList(const char* filelist){filelist_=filelist;}
  void setDataSetDirectory(TString dir){datasetdirectory_=dir;}
  void setShowStatusBar(bool show){showstatusbar_=show;}

  top::container1DStackVector * getPlots(){return analysisplots_;}

  void start();

  void clear(){analysisplots_->clear();}

  void setAdditionalInfoString(TString add){additionalinfo_=add;} //!for adding things like JEC up or other systematics options

  top::PUReweighter * getPUReweighter(){return puweighter_;}

  top::JERAdjuster * getJERAdjuster(){return jeradjuster_;}
  top::JECUncertainties * getJECUncertainties(){return jecuncertainties_;}

  MainAnalyzer & operator= (const MainAnalyzer &);

private:

  bool isInInfo(TString s){return additionalinfo_.Contains(s);}
  bool triggersContain(TString , top::NTEvent *);

  bool showstatusbar_;

  TString name_,dataname_;
  TString datasetdirectory_;
  double lumi_;

  const char * filelist_;
  
  top::PUReweighter * puweighter_;
  top::JERAdjuster * jeradjuster_;
  top::JECUncertainties * jecuncertainties_;

  top::container1DStackVector * analysisplots_;

  TString additionalinfo_;

};
bool MainAnalyzer::triggersContain(TString triggername, top::NTEvent * pevent){
  bool out = false;
  for(unsigned int i=0;i<pevent->firedTriggers().size();i++){
    if(((TString)pevent->firedTriggers()[i]).Contains(triggername)){
      out=true;
      break;
    }
  }
  return out;
}

void MainAnalyzer::start(){

  using namespace std;
  if(analysisplots_) clear();
  else cout << "warning analysisplots_ is null" << endl;
  analysisplots_->setName(name_+"_"+additionalinfo_);
  std::cout << "Starting analysis for " << name_ << std::endl;
  ifstream inputfiles (filelist_);
  string filename;
  string legentry;
  int color;
  double norm;
  string oldfilename="";
  if(inputfiles.is_open()){
    while(inputfiles.good()){
      inputfiles >> filename; 
      TString temp=filename;
      if(temp.BeginsWith("#") || temp==""){
	getline(inputfiles,filename); //just ignore complete line
	continue;
      }
      inputfiles >> legentry >> color >> norm;
      if(oldfilename != filename) analyze(datasetdirectory_+(TString) filename, (TString)legentry, color, norm);
      oldfilename=filename;
    }
  }
  else{
    cout << "MainAnalyzer::start(): input file list not found" << endl;
  }
  
}

MainAnalyzer::MainAnalyzer(const MainAnalyzer & analyzer){
  name_=analyzer.name_+"_clone";
  lumi_=analyzer.lumi_;
  additionalinfo_=analyzer.additionalinfo_;
  dataname_=analyzer.dataname_;
  datasetdirectory_=analyzer.datasetdirectory_;
  puweighter_= new top::PUReweighter(*analyzer.puweighter_);
  filelist_=analyzer.filelist_;
  jeradjuster_= new top::JERAdjuster(*analyzer.jeradjuster_);
  jecuncertainties_= new top::JECUncertainties(*analyzer.jecuncertainties_);
  analysisplots_ = new top::container1DStackVector(*analyzer.analysisplots_);
  showstatusbar_=analyzer.showstatusbar_;
}

MainAnalyzer & MainAnalyzer::operator = (const MainAnalyzer & analyzer){
  *this=MainAnalyzer(analyzer);
  return *this;
}

void  MainAnalyzer::analyze(TString inputfile, TString legendname, int color, double norm){ // do analysis here and store everything in analysisplots_

  using namespace std;
  using namespace top;


  bool isMC=true;
  if(legendname==dataname_) isMC=false;


  //define containers here

  //   define binnings
  vector<float> drbins;
  for(float i=0;i<40;i++) drbins << i/40;

  vector<float> etabinselec;
  etabinselec << -2.5 << -1.8 << -1 << 1 << 1.8 << 2.5 ;
  vector<float> etabinsmuon;
  etabinsmuon << -2.4 << -1.5 << -0.8 << 0.8 << 1.5 << 2.4;
  vector<float> etabinsjets;
  etabinsjets << -2.8 << -2.5 << -1.8 << -1 << 1 << 1.8 << 2.5 << 2.8;
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
  for(float i=-0.5;i<5.5;i++) multibinsbtag << i;
  vector<float> isobins;
  for(float i=0;i<50;i++) isobins << i/50;
  vector<float> selectionbins;
  for(float i=-0.5;i<11.5;i++) selectionbins << i;
  vector<float> genbin;
  genbin << 0.5 << 1.5;


  //and so on

  //invoke c_list for automatically registering  all created containers;

  ///  define containers


  /// comment: rearrange to object clusters!!   PER BINWIDTH!!!!! HAS TO BE ADDED!!
  
  container1D::c_clearlist(); // should be empty just in case
  container1D::c_makelist=true; //switch on automatic listing

  container1D generated(selectionbins, "generated events", "gen", "N_{gen}");
  container1D generated2(selectionbins, "generated filtered events", "gen", "N_{gen}");

  container1D selection(selectionbins, "selection steps", "step", "N_{sel}");

  container1D eleceta0(etabinselec, "electron eta step 0", "#eta_{l}","N_{e}");
  container1D eleceta1(etabinselec, "electron eta step 1", "#eta_{l}","N_{e}");
  container1D eleceta2(etabinselec, "electron eta step 2", "#eta_{l}","N_{e}");
  container1D eleceta3(etabinselec, "electron eta step 3", "#eta_{l}","N_{e}");
  container1D eleceta4(etabinselec, "electron eta step 4", "#eta_{l}","N_{e}");
  container1D eleceta5(etabinselec, "electron eta step 5", "#eta_{l}","N_{e}");
  container1D eleceta6(etabinselec, "electron eta step 6", "#eta_{l}","N_{e}");
  container1D eleceta7(etabinselec, "electron eta step 7", "#eta_{l}","N_{e}");
  container1D eleceta8(etabinselec, "electron eta step 8", "#eta_{l}","N_{e}");
  container1D eleceta10(etabinselec, "electron eta step 10", "#eta_{l}","N_{e}");

  container1D elecpt0(ptbinsfull, "electron pt step 0", "p_{T} [GeV]", "N_{e}");
  container1D elecpt1(ptbinsfull, "electron pt step 1", "p_{T} [GeV]", "N_{e}");
  container1D elecpt2(ptbinsfull, "electron pt step 2", "p_{T} [GeV]", "N_{e}");
  container1D elecpt3(ptbinsfull, "electron pt step 3", "p_{T} [GeV]", "N_{e}");
  container1D elecpt4(ptbinsfull, "electron pt step 4", "p_{T} [GeV]", "N_{e}");
  container1D elecpt5(ptbinsfull, "electron pt step 5", "p_{T} [GeV]", "N_{e}");
  container1D elecpt6(ptbinsfull, "electron pt step 6", "p_{T} [GeV]", "N_{e}");
  container1D elecpt7(ptbinsfull, "electron pt step 7", "p_{T} [GeV]", "N_{e}");
  container1D elecpt8(ptbinsfull, "electron pt step 8", "p_{T} [GeV]", "N_{e}");
  container1D elecpt10(ptbinsfull, "electron pt step 10", "p_{T} [GeV]", "N_{e}");

  container1D eleciso0(isobins, "electron isolation step 0", "Iso", "N_{e}");
  container1D eleciso1(isobins, "electron isolation step 1", "Iso", "N_{e}");
  container1D eleciso2(isobins, "electron isolation step 2", "Iso", "N_{e}");
  container1D eleciso3(isobins, "electron isolation step 3", "Iso", "N_{e}");
  container1D eleciso4(isobins, "electron isolation step 4", "Iso", "N_{e}");

  container1D elecid0(isobins, "electron mva id step 0", "Id", "N_{e}");
  container1D elecid1(isobins, "electron mva id step 1", "Id", "N_{e}");
  container1D elecid2(isobins, "electron mva id step 2", "Id", "N_{e}");

  container1D elecmuondR0(drbins, "electron-muon dR step 0", "dR", "N_{e}*N_{#mu}/bw",true);

  container1D muoneta0(etabinsmuon, "muon eta step 0", "#eta_{l}","N_{#mu}");
  container1D muoneta1(etabinsmuon, "muon eta step 1", "#eta_{l}","N_{#mu}");
  container1D muoneta2(etabinsmuon, "muon eta step 2", "#eta_{l}","N_{#mu}");
  container1D muoneta3(etabinsmuon, "muon eta step 3", "#eta_{l}","N_{#mu}");
  container1D muoneta4(etabinsmuon, "muon eta step 4", "#eta_{l}","N_{#mu}");
  container1D muoneta5(etabinsmuon, "muon eta step 5", "#eta_{l}","N_{#mu}");
  container1D muoneta6(etabinsmuon, "muon eta step 6", "#eta_{l}","N_{#mu}");
  container1D muoneta7(etabinsmuon, "muon eta step 7", "#eta_{l}","N_{#mu}");
  container1D muoneta8(etabinsmuon, "muon eta step 8", "#eta_{l}","N_{#mu}");
  container1D muoneta10(etabinsmuon, "muon eta step 10", "#eta_{l}","N_{#mu}");

  container1D muonpt0(ptbinsfull, "muon pt step 0", "p_{T} [GeV]", "N_{#mu}");
  container1D muonpt1(ptbinsfull, "muon pt step 1", "p_{T} [GeV]", "N_{#mu}");
  container1D muonpt2(ptbinsfull, "muon pt step 2", "p_{T} [GeV]", "N_{#mu}");
  container1D muonpt3(ptbinsfull, "muon pt step 3", "p_{T} [GeV]", "N_{#mu}");
  container1D muonpt4(ptbinsfull, "muon pt step 4", "p_{T} [GeV]", "N_{#mu}");
  container1D muonpt5(ptbinsfull, "muon pt step 5", "p_{T} [GeV]", "N_{#mu}");
  container1D muonpt6(ptbinsfull, "muon pt step 6", "p_{T} [GeV]", "N_{#mu}");
  container1D muonpt7(ptbinsfull, "muon pt step 7", "p_{T} [GeV]", "N_{#mu}");
  container1D muonpt8(ptbinsfull, "muon pt step 8", "p_{T} [GeV]", "N_{#mu}");
  container1D muonpt10(ptbinsfull, "muon pt step 10", "p_{T} [GeV]", "N_{#mu}");

  container1D muoniso0(isobins, "muon isolation step 0", "Iso", "N_{#mu}");
  container1D muoniso1(isobins, "muon isolation step 1", "Iso", "N_{#mu}");
  container1D muoniso2(isobins, "muon isolation step 2", "Iso", "N_{#mu}");
  container1D muoniso3(isobins, "muon isolation step 3", "Iso", "N_{#mu}");
  container1D muoniso4(isobins, "muon isolation step 4", "Iso", "N_{#mu}");

  container1D vertexmulti0(multibinsvertx, "vertex multiplicity step 0", "n_{vtx}", "N_{evt}");
  container1D vertexmulti1(multibinsvertx, "vertex multiplicity step 1", "n_{vtx}", "N_{evt}");
  container1D vertexmulti2(multibinsvertx, "vertex multiplicity step 2", "n_{vtx}", "N_{evt}");
  container1D vertexmulti3(multibinsvertx, "vertex multiplicity step 3", "n_{vtx}", "N_{evt}");
  container1D vertexmulti4(multibinsvertx, "vertex multiplicity step 4", "n_{vtx}", "N_{evt}");
  container1D vertexmulti5(multibinsvertx, "vertex multiplicity step 5", "n_{vtx}", "N_{evt}");
  container1D vertexmulti6(multibinsvertx, "vertex multiplicity step 6", "n_{vtx}", "N_{evt}");
  container1D vertexmulti7(multibinsvertx, "vertex multiplicity step 7", "n_{vtx}", "N_{evt}");
  container1D vertexmulti8(multibinsvertx, "vertex multiplicity step 8", "n_{vtx}", "N_{evt}");

  container1D invmass2(massbins, "dilepton invariant mass step 2", "m_{ll} [GeV]", "N_{evt}");
  container1D invmass3(massbins, "dilepton invariant mass step 3", "m_{ll} [GeV]", "N_{evt}");
  container1D invmass4(massbins, "dilepton invariant mass step 4", "m_{ll} [GeV]", "N_{evt}");
  container1D invmass5(massbins, "dilepton invariant mass step 5", "m_{ll} [GeV]", "N_{evt}");
  container1D invmass6(massbins, "dilepton invariant mass step 6", "m_{ll} [GeV]", "N_{evt}");
  container1D invmass7(massbins, "dilepton invariant mass step 7", "m_{ll} [GeV]", "N_{evt}");
  container1D invmass8(massbins, "dilepton invariant mass step 8", "m_{ll} [GeV]", "N_{evt}");
  container1D invmass9(massbins, "dilepton invariant mass step 8", "m_{ll} [GeV]", "N_{evt}");
  container1D invmass10(massbins, "dilepton invariant mass step 10", "m_{ll} [GeV]", "N_{evt}"); //Z

  container1D invmassZ5(massbins, "dilepton invariant massZ step 5", "m_{ll} [GeV]", "N_{evt}");
  container1D invmassZ6(massbins, "dilepton invariant massZ step 6", "m_{ll} [GeV]", "N_{evt}");
  container1D invmassZ7(massbins, "dilepton invariant massZ step 7", "m_{ll} [GeV]", "N_{evt}");
  container1D invmassZ8(massbins, "dilepton invariant massZ step 8", "m_{ll} [GeV]", "N_{evt}");
  container1D invmassZ9(massbins, "dilepton invariant massZ step 9", "m_{ll} [GeV]", "N_{evt}");

  container1D jetmulti3(multibinsjets, "jet multiplicity step 3", "n_{jets}","N_{jets}",true);
  container1D jetmulti4(multibinsjets, "jet multiplicity step 4", "n_{jets}","N_{jets}",true);
  container1D jetmulti5(multibinsjets, "jet multiplicity step 5", "n_{jets}","N_{jets}",true);
  container1D jetmulti6(multibinsjets, "jet multiplicity step 6", "n_{jets}","N_{jets}",true);
  container1D jetmulti7(multibinsjets, "jet multiplicity step 7", "n_{jets}","N_{jets}",true);
  container1D jetmulti8(multibinsjets, "jet multiplicity step 8", "n_{jets}","N_{jets}",true);

  container1D jetpt3(ptbinsfull, "jet pt step 3", "p_{T} [GeV]","N_{jets}");
  container1D jetpt4(ptbinsfull, "jet pt step 4", "p_{T} [GeV]","N_{jets}");
  container1D jetpt5(ptbinsfull, "jet pt step 5", "p_{T} [GeV]","N_{jets}");
  container1D jetpt6(ptbinsfull, "jet pt step 6", "p_{T} [GeV]","N_{jets}");
  container1D jetpt7(ptbinsfull, "jet pt step 7", "p_{T} [GeV]","N_{jets}");
  container1D jetpt8(ptbinsfull, "jet pt step 8", "p_{T} [GeV]","N_{jets}");

  container1D jeteta3(etabinsjets, "jet eta step 3", "#eta_{jet}","N_{jets}");
  container1D jeteta4(etabinsjets, "jet eta step 4", "#eta_{jet}","N_{jets}");
  container1D jeteta5(etabinsjets, "jet eta step 5", "#eta_{jet}","N_{jets}");
  container1D jeteta6(etabinsjets, "jet eta step 6", "#eta_{jet}","N_{jets}");
  container1D jeteta7(etabinsjets, "jet eta step 7", "#eta_{jet}","N_{jets}");
  container1D jeteta8(etabinsjets, "jet eta step 8", "#eta_{jet}","N_{jets}");

  container1D met6u(ptbins, "missing transverse energie uncorr step 6","E_{T,miss} [GeV]", "N_{evt}");
  container1D met6(ptbins, "missing transverse energie step 6","E_{T,miss} [GeV]", "N_{evt}");
  container1D met7(ptbins, "missing transverse energie step 7","E_{T,miss} [GeV]", "N_{evt}");
  container1D met8(ptbins, "missing transverse energie step 8","E_{T,miss} [GeV]", "N_{evt}");
  container1D met9(ptbins, "missing transverse energie step 9","E_{T,miss} [GeV]", "N_{evt}");

  container1D btagmulti7(multibinsbtag, "b-jet multiplicity step 7", "n_{b-tags}", "N_{jets}",true);
  container1D btagmulti8(multibinsbtag, "b-jet multiplicity step 8", "n_{b-tags}", "N_{jets}",true);
  container1D btagmulti9(multibinsbtag, "b-jet multiplicity step 9", "n_{b-tags}", "N_{jets}",true);



  container1D::c_makelist=false; //switch off automatic listing




  //get the lepton selector (maybe directly in the code.. lets see)

  leptonSelector lepSel;
  lepSel.setUseRhoIsoForElectrons(true);
  double oldnorm=norm;

  /////some boolians //channels
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
  double genentries=0;
  if(isMC){

    TTree * tnorm = (TTree*) f->Get("preCutPUInfo/preCutPUInfo");
    genentries=tnorm->GetEntries();

    delete tnorm;
    norm = lumi_ * norm / genentries;
    for(int i=1;i<=generated.getNBins();i++){
      generated.setBinContent(i, genentries);
      generated.setBinError(i, sqrt(genentries));
    }
    double fgen=0;
    TTree * tfgen = (TTree*) f->Get("postCutPUInfo/PUTreePostCut");
    if(tfgen){
      fgen=tfgen->GetEntries();
      for(int i=1;i<=generated2.getNBins();i++){
	generated2.setBinContent(i, fgen);
	generated2.setBinError(i, sqrt(fgen));
      }
    }

  }
  else{
      norm=1;
  }
  cout << "running on: " << inputfile << "    legend: " << legendname << "\nxsec: " << oldnorm << ", genEvents: " << genentries <<endl;
  // get main analysis tree

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
  Long64_t nEntries=t->GetEntries();
  if(norm==0) nEntries=0; //skip for norm0
  for(Long64_t entry=0;entry<nEntries;entry++){
    if(showstatusbar_) displayStatusBar(entry,nEntries);
    t->GetEntry(entry);

    
    double puweight;
    if(!isMC) puweight=1;
    else puweight = getPUReweighter()->getPUweight(pEvent->truePU());

    //lepton collections
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
    vector<NTMuon> kinmuons         = lepSel.selectKinMuons(*pMuons);
    

    /////////TRIGGER CUT and kin leptons STEP 0/////////////////////////////
    if(b_ee){
      if(kinelectrons.size()< 2) continue; //has to be 17??!!
      if(isMC  && !triggersContain("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v" , pEvent) ) continue;
      if(!isMC && !triggersContain("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v" , pEvent) ) continue;

    }
    if(b_mumu){
      if(kinmuons.size() < 2) continue;
      //trg
      if(isMC &&  !(triggersContain("HLT_Mu17_Mu8_v16",pEvent)  || triggersContain("HLT_Mu17_TkMu8_v9",pEvent))) continue;
      if(!isMC && !(triggersContain("HLT_Mu17_Mu8_v",pEvent)  || triggersContain("HLT_Mu17_TkMu8_v",pEvent))) continue;


    }
    if(b_emu){
      if(kinelectrons.size() + kinmuons.size() < 2) continue;
      //trg


    }


    vector<NTElectron> idelectrons   =lepSel.selectIDElectrons(kinelectrons);
    vector<NTMuon> idmuons           =lepSel.selectIDMuons(kinmuons);

    vector<NTElectron> isoelectrons  =lepSel.selectIsolatedElectrons(idelectrons);
    vector<NTMuon> isomuons          =lepSel.selectIsolatedMuons(idmuons);
    
    for(NTElectronIt elec=kinelectrons.begin();elec<kinelectrons.end();++elec){
      eleceta0.fill(elec->eta(), puweight);
      elecpt0.fill(elec->pt(),puweight);
      eleciso0.fill(elec->rhoIso03(),puweight);
      elecid0.fill(elec->mvaId(),puweight);
      for(NTMuonIt muon=kinmuons.begin();muon<kinmuons.end();++muon){
	elecmuondR0.fill(sqrt(pow(elec->eta() - muon->eta(),2) + pow(elec->phi() - muon->phi(),2)), puweight);
      }

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
      elecid1.fill(elec->mvaId(),puweight);
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
      elecid2.fill(elec->mvaId(),puweight);
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
    

    // create jec jets for met and ID jets


    // create ID Jets and correct JER

    vector<NTJet> nolidjets;
    double dpx=0;
    double dpy=0;

    for(NTJetIt jet=pJets->begin();jet<pJets->end();++jet){ //ALSO THE RESOLUTION AFFECTS MET. HERE INTENDED!!! GOOD?
      LorentzVector oldp4=jet->p4();
      if(isMC){
	getJECUncertainties()->applyJECUncertainties(jet);
	getJERAdjuster()->correctJet(jet);
      } //corrected
      if(jet->emEnergyFraction() < 0.9 && jet->pt() > 10){
	dpx += oldp4.Px() - jet->p4().Px();
	dpy += oldp4.Py() - jet->p4().Py();
      }
      if(!(jet->id())) continue;
      if(!noOverlap(jet, isomuons, 0.3)) continue;
      if(!noOverlap(jet, isoelectrons, 0.3)) continue;
      nolidjets.push_back(*jet);
    }



    //fill container

    for(NTJetIt jet=nolidjets.begin();jet<nolidjets.end();++jet){

      jetpt3.fill(jet->pt(),puweight);
      jeteta3.fill(jet->eta(),puweight);
    }

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

    jetmulti3.fill(nolidjets.size(),puweight);

    ////////////////////Z Selection//////////////////
    bool isZrange=false;

    if(invLepMass > 76 && invLepMass < 106){
      
      for(NTElectronIt elec=isoelectrons.begin();elec<isoelectrons.end();++elec){
	eleceta10.fill(elec->eta(), puweight);
	elecpt10.fill(elec->pt(),puweight);
	//some other fills
      }
      
      for(NTMuonIt muon=isomuons.begin();muon<isomuons.end();++muon){
	muoneta10.fill(muon->eta(), puweight);
	muonpt10.fill(muon->pt(),puweight);
      }
      
      invmass10.fill(invLepMass,puweight);
      selection.fill(10,puweight);

      isZrange=true;
    }

    bool Znotemu=isZrange;
    if(b_emu) Znotemu=false;

    ////////////////////Z Veto Cut STEP 4 (incl. hard jets)////////////////////////////////////
    
    /////// create  hard jets ////////

    vector<NTJet> hardjets;

    for(NTJetIt jet=nolidjets.begin();jet<nolidjets.end();++jet){
      if(jet->pt()<30) continue;
      if(fabs(jet->eta())>2.5) continue;
      hardjets.push_back(*jet);
    }
    
    //fill


    if(!Znotemu){

    for(NTJetIt jet=hardjets.begin();jet<hardjets.end();++jet){
      jetpt4.fill(jet->pt(),puweight);
      jeteta4.fill(jet->eta(),puweight);
    }

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
    jetmulti4.fill(hardjets.size(),puweight);
    selection.fill(4,puweight);

    }
    if(isZrange){ //Z done above

    }


    ///////////////////// at least one jet cut STEP 5 ////////////

    if(hardjets.size() < 1) continue;

    if(!Znotemu){

      for(NTJetIt jet=hardjets.begin();jet<hardjets.end();++jet){
	jetpt5.fill(jet->pt(),puweight);
	jeteta5.fill(jet->eta(),puweight);
      }
      
      for(NTElectronIt elec=isoelectrons.begin();elec<isoelectrons.end();++elec){
	eleceta5.fill(elec->eta(), puweight);
	elecpt5.fill(elec->pt(),puweight);
	//	eleciso5.fill(elec->rhoIso03(),puweight);
	//some other fills
      }
      
      for(NTMuonIt muon=isomuons.begin();muon<isomuons.end();++muon){
	muoneta5.fill(muon->eta(), puweight);
	muonpt5.fill(muon->pt(),puweight);
	//	muoniso5.fill(muon->isoVal04(),puweight);
      }
      
      invmass5.fill(invLepMass,puweight);
      vertexmulti5.fill(pEvent->vertexMulti(),puweight);
      jetmulti5.fill(hardjets.size(),puweight);
      selection.fill(5,puweight);

    }
    if(isZrange){
      invmassZ5.fill(invLepMass,puweight);
    }



    /////////////////////// at least two jets STEP 6 /////////////

    ///adjust MET ///

    NTMet adjustedmet = *pMet;
    double nmpx=pMet->p4().Px() - dpx;
    double nmpy=pMet->p4().Py() - dpy;
    adjustedmet.setP4(LorentzVector(nmpx,nmpy,0,sqrt(pow(nmpx,2)+pow(nmpy,2))));
    


    if(hardjets.size() < 2) continue;

    if(!Znotemu){

      for(NTJetIt jet=hardjets.begin();jet<hardjets.end();++jet){
	jetpt6.fill(jet->pt(),puweight);
	jeteta6.fill(jet->eta(),puweight);
      }
      
      for(NTElectronIt elec=isoelectrons.begin();elec<isoelectrons.end();++elec){
	eleceta6.fill(elec->eta(), puweight);
	elecpt6.fill(elec->pt(),puweight);
	//	eleciso6.fill(elec->rhoIso03(),puweight);
	//some other fills
      }
      
      for(NTMuonIt muon=isomuons.begin();muon<isomuons.end();++muon){
	muoneta6.fill(muon->eta(), puweight);
	muonpt6.fill(muon->pt(),puweight);
	//	muoniso6.fill(muon->isoVal04(),puweight);
      }
      
      invmass6.fill(invLepMass,puweight);
      vertexmulti6.fill(pEvent->vertexMulti(),puweight);
      jetmulti6.fill(hardjets.size(),puweight);
      selection.fill(6,puweight);
      met6u.fill(pMet->met(),puweight);
      met6.fill(adjustedmet.met(), puweight);
    }
    if(isZrange){
      invmassZ6.fill(invLepMass,puweight);
    }




    ///make btagged jets //
    vector<NTJet> btaggedjets;
    for(NTJetIt jet = hardjets.begin();jet<hardjets.end();++jet){
      if(jet->btag() < 0.244) continue;
      btaggedjets.push_back(*jet);
    }

   
    //////////////////// MET cut STEP 7//////////////////////////////////
    if(adjustedmet.met() < 40) continue;

    if(!Znotemu){

      for(NTJetIt jet=hardjets.begin();jet<hardjets.end();++jet){
	jetpt7.fill(jet->pt(),puweight);
	jeteta7.fill(jet->eta(),puweight);
      }
      
      for(NTElectronIt elec=isoelectrons.begin();elec<isoelectrons.end();++elec){
	eleceta7.fill(elec->eta(), puweight);
	elecpt7.fill(elec->pt(),puweight);
	//	eleciso7.fill(elec->rhoIso03(),puweight);
	//some other fills
      }
      
      for(NTMuonIt muon=isomuons.begin();muon<isomuons.end();++muon){
	muoneta7.fill(muon->eta(), puweight);
	muonpt7.fill(muon->pt(),puweight);
	//	muoniso7.fill(muon->isoVal04(),puweight);
      }
      
      invmass7.fill(invLepMass,puweight);
      vertexmulti7.fill(pEvent->vertexMulti(),puweight);
      jetmulti7.fill(hardjets.size(),puweight);
      selection.fill(7, puweight);
      met7.fill(adjustedmet.met(), puweight);
      btagmulti7.fill(btaggedjets.size(),puweight);
    }
    if(isZrange){
      invmassZ7.fill(invLepMass,puweight);
    }




    ///////////////////// btag cut STEP 8 //////////////////////////

    if(btaggedjets.size() < 1) continue;

    if(!Znotemu){

      for(NTJetIt jet=hardjets.begin();jet<hardjets.end();++jet){
	jetpt8.fill(jet->pt(),puweight);
	jeteta8.fill(jet->eta(),puweight);
      }
      
      for(NTElectronIt elec=isoelectrons.begin();elec<isoelectrons.end();++elec){
	eleceta8.fill(elec->eta(), puweight);
	elecpt8.fill(elec->pt(),puweight);
	//	eleciso8.fill(elec->rhoIso03(),puweight);
	//some other fills
      }
      
      for(NTMuonIt muon=isomuons.begin();muon<isomuons.end();++muon){
	muoneta8.fill(muon->eta(), puweight);
	muonpt8.fill(muon->pt(),puweight);
	//	muoniso8.fill(muon->isoVal04(),puweight);
      }
      
      invmass8.fill(invLepMass,puweight);
      vertexmulti8.fill(pEvent->vertexMulti(),puweight);
      jetmulti8.fill(hardjets.size(),puweight);
      selection.fill(8,puweight);
      met8.fill(adjustedmet.met(), puweight);
      btagmulti8.fill(btaggedjets.size(),puweight);

    }
    if(isZrange){
      invmassZ8.fill(invLepMass,puweight);
    }

    if(btaggedjets.size() < 2) continue;
    if(!Znotemu){
      selection.fill(9,puweight);
      btagmulti9.fill(btaggedjets.size(),puweight);
      met9.fill(adjustedmet.met(), puweight);
    }
    if(isZrange){
      invmassZ9.fill(invLepMass,puweight);
    }


  } //main event loop ends
  std::cout << std::endl; //for status bar

  // Fill all containers in the stackVector

  // std::cout << "Filling containers to the Stack\n" << std::endl;
  getPlots()->addList(legendname,color,norm);


  delete t;
  f->Close();
  delete f;
  

}


void Analyzer(){
  using namespace std;



  cout << "\n\n\n" <<endl; //looks better ;)

  TString outfile = "0817_out.root";
  TString pufolder="/afs/naf.desy.de/user/k/kieseler/scratch/2012/TestArea2/CMSSW_5_2_5/src/TtZAnalysis/Data/PUDistr/";

  //initialize mumu


  MainAnalyzer analyzermumu("default_mumu","mumu");
  analyzermumu.setShowStatusBar(false);  //for running with text output mode
  analyzermumu.setLumi(5097);
  analyzermumu.setFileList("mumu_8TeV_inputfiles.txt");
  analyzermumu.setDataSetDirectory("/scratch/hh/dust/naf/cms/user/kieseler/trees0724/");
  analyzermumu.getPUReweighter()->setDataTruePUInput(pufolder+"data_pu_190456-196531_735_def.root");
  analyzermumu.getJECUncertainties()->setFile("/afs/naf.desy.de/user/k/kieseler/scratch/2012/TestArea2/CMSSW_5_2_5/src/TtZAnalysis/Data/JECUnc/Summer12_V2_DATA_AK5PF_UncertaintySources.txt");
  analyzermumu.getPUReweighter()->setMCDistrSum12();

  //start with ee

  MainAnalyzer analyzeree=analyzermumu;
  analyzeree.setFileList("ee_8TeV_inputfiles.txt");
  analyzeree.setName("default_ee","ee");
  analyzeree.start();
  analyzeree.getPlots()->writeAllToTFile(outfile,true);

  
  MainAnalyzer eewithlumi=analyzeree;
  eewithlumi.setName("ee_lumi","ee");

  eewithlumi.getPlots()->addGlobalRelMCError("Lumi", 0.045);
  eewithlumi.getPlots()->writeAllToTFile(outfile);
  
  // jer systematics 
  
  MainAnalyzer analyzereejerup=analyzeree;
  analyzereejerup.setName("jerup_ee","ee");
  analyzereejerup.getJERAdjuster()->setSystematics("up");
  analyzereejerup.start();
  analyzereejerup.getPlots()->writeAllToTFile(outfile);


  MainAnalyzer analyzereejerdown=analyzeree;
  analyzereejerdown.setName("jerdown_ee","ee");
  analyzereejerdown.getJERAdjuster()->setSystematics("down");
  analyzereejerdown.start();
  analyzereejerdown.getPlots()->writeAllToTFile(outfile);

  MainAnalyzer eewithjerunc=analyzeree;
  eewithjerunc.setName("jerunc_ee","ee");
  eewithjerunc.getPlots()->addMCErrorStackVector("JER_up",*analyzereejerup.getPlots());
  eewithjerunc.getPlots()->addMCErrorStackVector("JER_down",*analyzereejerdown.getPlots());
  eewithjerunc.getPlots()->writeAllToTFile(outfile);

  // jes systematics corr

  MainAnalyzer analyzereecorr_jesup=analyzeree;
  analyzereecorr_jesup.setName("corr_jesup_ee","ee");
  analyzereecorr_jesup.getJECUncertainties()->sources() << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 12 << 13 << 15;
  analyzereecorr_jesup.getJECUncertainties()->setVariation("up");
  analyzereecorr_jesup.start();
  analyzereecorr_jesup.getPlots()->writeAllToTFile(outfile);


  MainAnalyzer analyzereecorr_jesdown=analyzereecorr_jesup;
  analyzereecorr_jesdown.setName("corr_jesdown_ee","ee");
  analyzereecorr_jesdown.getJECUncertainties()->setVariation("down");
  analyzereecorr_jesdown.start();
  analyzereecorr_jesdown.getPlots()->writeAllToTFile(outfile);

  MainAnalyzer eewithcorr_jesunc=analyzeree;
  eewithcorr_jesunc.setName("corr_jesunc_ee","ee");
  eewithcorr_jesunc.getPlots()->addMCErrorStackVector("CORR_JES_up",*analyzereecorr_jesup.getPlots());
  eewithcorr_jesunc.getPlots()->addMCErrorStackVector("CORR_JES_down",*analyzereecorr_jesdown.getPlots());
  eewithcorr_jesunc.getPlots()->writeAllToTFile(outfile);


  // jes systematics uncorr

  MainAnalyzer analyzeree8TeV_jesup=analyzeree;
  analyzeree8TeV_jesup.setName("8TeV_jesup_ee","ee");
  analyzeree8TeV_jesup.getJECUncertainties()->sources() << 0 << 11 << 14;
  analyzeree8TeV_jesup.getJECUncertainties()->setVariation("up");
  analyzeree8TeV_jesup.start();
  analyzeree8TeV_jesup.getPlots()->writeAllToTFile(outfile);


  MainAnalyzer analyzeree8TeV_jesdown=analyzeree8TeV_jesup;
  analyzeree8TeV_jesdown.setName("8TeV_jesdown_ee","ee");
  analyzeree8TeV_jesdown.getJECUncertainties()->setVariation("down");
  analyzeree8TeV_jesdown.start();
  analyzeree8TeV_jesdown.getPlots()->writeAllToTFile(outfile);

  MainAnalyzer eewith8TeV_jesunc=analyzeree;
  eewith8TeV_jesunc.setName("8TeV_jesunc_ee","ee");
  eewith8TeV_jesunc.getPlots()->addMCErrorStackVector("8TEV_JES_up",*analyzeree8TeV_jesup.getPlots());
  eewith8TeV_jesunc.getPlots()->addMCErrorStackVector("8TEV_JES_down",*analyzeree8TeV_jesdown.getPlots());
  eewith8TeV_jesunc.getPlots()->writeAllToTFile(outfile);



  // pu systematics

  MainAnalyzer analyzereepuup=analyzeree;
  analyzereepuup.getPUReweighter()->setDataTruePUInput(pufolder+"data_pu_190456-196531_735_up.root"); 
  analyzereepuup.setName("puup_ee", "ee");
  analyzereepuup.start();
  analyzereepuup.getPlots()->writeAllToTFile(outfile);

  MainAnalyzer analyzereepudown=analyzeree;
  analyzereepudown.getPUReweighter()->setDataTruePUInput(pufolder+"data_pu_190456-196531_735_down.root");
  analyzereepudown.setName("pudown_ee","ee");
  analyzereepudown.start();
  analyzereepudown.getPlots()->writeAllToTFile(outfile);

  MainAnalyzer eewithPU=analyzeree;
  eewithPU.setName("puunc_ee","ee");
  eewithPU.getPlots()->addMCErrorStackVector("PU_up",*analyzereepuup.getPlots());
  eewithPU.getPlots()->addMCErrorStackVector("PU_down",*analyzereepudown.getPlots());
  eewithPU.getPlots()->writeAllToTFile(outfile);
  
  // all systematics
  
  MainAnalyzer eeWithAll=eewithlumi; //already includes lumi uncert
  eeWithAll.setName("allunc_ee","ee");
  eeWithAll.getPlots()->addMCErrorStackVector("PU_up",*analyzereepuup.getPlots());
  eeWithAll.getPlots()->addMCErrorStackVector("PU_down",*analyzereepudown.getPlots());
  eeWithAll.getPlots()->addMCErrorStackVector("JER_up",*analyzereejerup.getPlots());
  eeWithAll.getPlots()->addMCErrorStackVector("JER_down",*analyzereejerdown.getPlots());
  eeWithAll.getPlots()->addMCErrorStackVector("CORR_JES_up",*analyzereecorr_jesup.getPlots());
  eeWithAll.getPlots()->addMCErrorStackVector("CORR_JES_down",*analyzereecorr_jesdown.getPlots());
  eeWithAll.getPlots()->addMCErrorStackVector("8TEV_JES_up",*analyzeree8TeV_jesup.getPlots());
  eeWithAll.getPlots()->addMCErrorStackVector("8TEV_JES_down",*analyzeree8TeV_jesdown.getPlots());

  // do rescaling stuff
  std::vector<TString> ident;
  std::vector<double> scales;
  for(int i=4;i<=9;i++){
    TString stepstring="step "+toTString(i);
    double dymcee = eeWithAll.getPlots()->getStack("dilepton invariant massZ "+stepstring).getContribution("DY#rightarrowee").integral();
    double dee = eeWithAll.getPlots()->getStack("dilepton invariant massZ "+stepstring).getContribution("data").integral();
    std::vector<TString> allbutdyanddata;
    allbutdyanddata << "data" << "DY#rightarrowee";
    double ddyee = eeWithAll.getPlots()->getStack("dilepton invariant massZ "+stepstring).getContributionsBut(allbutdyanddata).integral();
    if(dymcee==0) dymcee=1;
    double scale = (dee-ddyee)/dymcee;
    scales << scale;
    ident << stepstring;
    std::cout << "Scalefactor for DY ee " << stepstring << ": " << scale << std::endl;
  }
  eeWithAll.getPlots()->multiplyNorms("DY#rightarrowee", scales, ident);
  eeWithAll.getPlots()->writeAllToTFile(outfile);
  ident.clear();
  scales.clear();

  // start the mumu analysis
  
  analyzermumu.start();

  analyzermumu.getPlots()->writeAllToTFile(outfile); // after that just update file
  MainAnalyzer mumuwithlumi=analyzermumu;
  mumuwithlumi.setName("lumi_mumu","");
  mumuwithlumi.getPlots()->addGlobalRelMCError("Lumi", 0.045);
  mumuwithlumi.getPlots()->writeAllToTFile(outfile); 

  //make JER systematics
  
  MainAnalyzer analyzermumujerup=analyzermumu;
  analyzermumujerup.setName("jerup_mumu","mumu");
  analyzermumujerup.getJERAdjuster()->setSystematics("up");
  analyzermumujerup.start();
  analyzermumujerup.getPlots()->writeAllToTFile(outfile);

  MainAnalyzer analyzermumujerdown=analyzermumu;
  analyzermumujerdown.setName("jerdown_mumu","mumu");
  analyzermumujerdown.getJERAdjuster()->setSystematics("down");
  analyzermumujerdown.start();
  analyzermumujerdown.getPlots()->writeAllToTFile(outfile);

  MainAnalyzer mumuwithjerunc=analyzermumu;
  mumuwithjerunc.setName("jerunc_mumu","mumu");
  mumuwithjerunc.getPlots()->addMCErrorStackVector("JER_up",*analyzermumujerup.getPlots());
  mumuwithjerunc.getPlots()->addMCErrorStackVector("JER_down",*analyzermumujerdown.getPlots());
  mumuwithjerunc.getPlots()->writeAllToTFile(outfile);

// corr_jes systematics

  MainAnalyzer analyzermumucorr_jesup=analyzermumu;
  analyzermumucorr_jesup.setName("corr_jesup_mumu","mumu");
  analyzermumucorr_jesup.getJECUncertainties()->setVariation("up");
  analyzermumucorr_jesup.start();
  analyzermumucorr_jesup.getPlots()->writeAllToTFile(outfile);


  MainAnalyzer analyzermumucorr_jesdown=analyzermumu;
  analyzermumucorr_jesdown.setName("corr_jesdown_mumu","mumu");
  analyzermumucorr_jesdown.getJECUncertainties()->setVariation("down");
  analyzermumucorr_jesdown.start();
  analyzermumucorr_jesdown.getPlots()->writeAllToTFile(outfile);

  MainAnalyzer mumuwithcorr_jesunc=analyzermumu;
  mumuwithcorr_jesunc.setName("corr_jesunc_mumu","mumu");
  mumuwithcorr_jesunc.getPlots()->addMCErrorStackVector("CORR_JES_up",*analyzermumucorr_jesup.getPlots());
  mumuwithcorr_jesunc.getPlots()->addMCErrorStackVector("CORR_JES_down",*analyzermumucorr_jesdown.getPlots());
  mumuwithcorr_jesunc.getPlots()->writeAllToTFile(outfile);


  // jes systematics uncorr

  MainAnalyzer analyzermumu8TeV_jesup=analyzermumu;
  analyzermumu8TeV_jesup.setName("8TeV_jesup_mumu","mumu");
  analyzermumu8TeV_jesup.getJECUncertainties()->sources() << 0 << 11 << 14;
  analyzermumu8TeV_jesup.getJECUncertainties()->setVariation("up");
  analyzermumu8TeV_jesup.start();
  analyzermumu8TeV_jesup.getPlots()->writeAllToTFile(outfile);


  MainAnalyzer analyzermumu8TeV_jesdown=analyzermumu8TeV_jesup;
  analyzermumu8TeV_jesdown.setName("8TeV_jesdown_mumu","mumu");
  analyzermumu8TeV_jesdown.getJECUncertainties()->setVariation("down");
  analyzermumu8TeV_jesdown.start();
  analyzermumu8TeV_jesdown.getPlots()->writeAllToTFile(outfile);

  MainAnalyzer mumuwith8TeV_jesunc=analyzermumu;
  mumuwith8TeV_jesunc.setName("8TeV_jesunc_mumu","mumu");
  mumuwith8TeV_jesunc.getPlots()->addMCErrorStackVector("8TEV_JES_up",*analyzermumu8TeV_jesup.getPlots());
  mumuwith8TeV_jesunc.getPlots()->addMCErrorStackVector("8TEV_JES_down",*analyzermumu8TeV_jesdown.getPlots());
  mumuwith8TeV_jesunc.getPlots()->writeAllToTFile(outfile);



  //make PU systematics
  
  MainAnalyzer analyzermumupuup=analyzermumu;
  analyzermumupuup.getPUReweighter()->setDataTruePUInput(pufolder+"data_pu_190456-196531_735_up.root"); 
  analyzermumupuup.setName("puup_mumu", "mumu");
  analyzermumupuup.start();
  analyzermumupuup.getPlots()->writeAllToTFile(outfile);

  MainAnalyzer analyzermumupudown=analyzermumu;
  analyzermumupudown.getPUReweighter()->setDataTruePUInput(pufolder+"data_pu_190456-196531_735_down.root");
  analyzermumupudown.setName("pudown_mumu","mumu");
  analyzermumupudown.start();
  analyzermumupudown.getPlots()->writeAllToTFile(outfile);

  MainAnalyzer mumuwithPU=analyzermumu;
  mumuwithPU.setName("puunc_mumu","mumu");
  mumuwithPU.getPlots()->addMCErrorStackVector("PU_up",*analyzermumupuup.getPlots());
  mumuwithPU.getPlots()->addMCErrorStackVector("PU_down",*analyzermumupudown.getPlots());
  mumuwithPU.getPlots()->writeAllToTFile(outfile);
  
  // combine all syst
  
  MainAnalyzer mumuWithAll=mumuwithlumi;
  mumuWithAll.setName("allunc_mumu","mumu");
  mumuWithAll.getPlots()->addMCErrorStackVector("PU_up",*analyzermumupuup.getPlots());
  mumuWithAll.getPlots()->addMCErrorStackVector("PU_down",*analyzermumupudown.getPlots());
  mumuWithAll.getPlots()->addMCErrorStackVector("JER_up",*analyzermumujerup.getPlots());
  mumuWithAll.getPlots()->addMCErrorStackVector("JER_down",*analyzermumujerdown.getPlots());
  mumuWithAll.getPlots()->addMCErrorStackVector("CORR_JES_up",*analyzermumucorr_jesup.getPlots());
  mumuWithAll.getPlots()->addMCErrorStackVector("CORR_JES_down",*analyzermumucorr_jesdown.getPlots());
  mumuWithAll.getPlots()->addMCErrorStackVector("8TEV_JES_up",*analyzermumu8TeV_jesup.getPlots());
  mumuWithAll.getPlots()->addMCErrorStackVector("8TEV_JES_down",*analyzermumu8TeV_jesdown.getPlots());




  ident.clear();
  scales.clear();
  for(int i=4;i<=9;i++){
    TString stepstring="step "+toTString(i);
    double dymcmumu = mumuWithAll.getPlots()->getStack("dilepton invariant massZ "+stepstring).getContribution("DY#rightarrow#mu#mu").integral();
    double dmumu = mumuWithAll.getPlots()->getStack("dilepton invariant massZ "+stepstring).getContribution("data").integral();
    std::vector<TString> allbutdyanddata;
    allbutdyanddata << "data" << "DY#rightarrow#mu#mu";
    double ddymumu = mumuWithAll.getPlots()->getStack("dilepton invariant massZ "+stepstring).getContributionsBut(allbutdyanddata).integral();
    if(dymcmumu==0) dymcmumu=1;
    double scale = (dmumu-ddymumu)/dymcmumu;
    scales << scale;
    ident << stepstring;
    std::cout << "Scalefactor for DY #mu#mu " << stepstring << ": " << scale << std::endl;
  }
  mumuWithAll.getPlots()->multiplyNorms("DY#rightarrow#mu#mu", scales, ident);
  mumuWithAll.getPlots()->writeAllToTFile(outfile);
  ident.clear();
  scales.clear();



  std::cout <<"\n\n\n\n\nFINISHED!" <<std::endl;


}
