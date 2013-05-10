#include "../interface/MainAnalyzer.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"


void MainAnalyzer::start(){

  using namespace std;

  AutoLibraryLoader::enable();
  
 
  clear();

  TString name=channel_+"_"+energy_+"_"+syst_;

  if(channel_=="" || energy_=="" || syst_ == ""){
    std::cout << "MainAnalyzer::start Analyzer not properly named - check!" << std::endl;
    return;
  }

  analysisplots_.setName(name);
  analysisplots_.setSyst(getSyst());

  std::cout << "Starting analysis for " << name << std::endl;
  ifstream inputfiles (filelist_.Data());
  string filename;
  string legentry;
  int color;
  double norm;
  string oldline="";
  if(inputfiles.is_open()){
    while(inputfiles.good()){
      inputfiles >> filename; 
      inputfiles >> legentry >> color >> norm;
      if(oldline != filename) analyze((TString) filename, (TString)legentry, color, norm);
      oldline=filename;
    }
  }
  else{
    cout << "MainAnalyzer::start(): input file list not found" << endl;
  }

  //rescaleDY(&analysisplots_, dycontributions);
  
}


void MainAnalyzer::copyAll(const MainAnalyzer & analyzer){
  
  channel_=analyzer.channel_;
  syst_=analyzer.syst_;
  energy_=analyzer.energy_;
  dataname_=analyzer.dataname_;

  lumi_=analyzer.lumi_;
  datasetdirectory_=analyzer.datasetdirectory_;
  puweighter_= analyzer.puweighter_;
  filelist_=analyzer.filelist_;
  jeradjuster_= analyzer.jeradjuster_;
  jecuncertainties_= analyzer.jecuncertainties_;
  btagsf_ = analyzer.btagsf_;
  analysisplots_ = analyzer.analysisplots_;
  showstatusbar_=analyzer.showstatusbar_;
}

MainAnalyzer::MainAnalyzer(const MainAnalyzer & analyzer){
  copyAll(analyzer);
}

MainAnalyzer & MainAnalyzer::operator = (const MainAnalyzer & analyzer){
  copyAll(analyzer);
  return *this;
}

void  MainAnalyzer::analyze(TString inputfile, TString legendname, int color, double norm){

  using namespace std;
  using namespace ztop;


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
  vector<float> etabinsdil;
  etabinsdil << -2.4 << -2 << -1.6 << -1.2 << -0.8 << -0.4 << 0.4 << 0.8 << 1.2 << 1.6 << 2 << 2.4;
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
  vector<float> onebin;
  onebin << 0.5 << 1.5;

  vector<float> bsfs;
  for(float i=0;i<120;i++) bsfs <<  (i/100);


  //and so on

  //invoke c_list for automatically registering  all created containers;

  ///  define containers


  /// comment: rearrange to object clusters!!   PER BINWIDTH!!!!! HAS TO BE ADDED!!
  
  container1D::c_clearlist(); // should be empty just in case
  container1D::c_makelist=true; //switch on automatic listing

  container1D generated(onebin, "generated events", "gen", "N_{gen}");
  container1D generated2(onebin, "generated filtered events", "gen", "N_{gen}");


  container1D diletaZgen(etabinsdil, "dilepton eta gen", "#eta_{ll}", "N_{evt}");

  container1D selection(selectionbins, "some selection steps", "step", "N_{sel}");


  // need to be different containers to get the background renormalization right

  container1D ttfinal_selection8 (onebin, "ttbar selection step 8", "step", "N_{sel}"); // NO Z,  1btag
  container1D ttfinal_selection9 (onebin, "ttbar selection step 9", "step", "N_{sel}"); // NO Z,  2btag 
  container1D ttfinal_selection10(onebin, "ttbar selection step 10", "step", "N_{sel}"); // NO Z, jet stuff?
  container1D Zfinal_selection20 (onebin, "Z final selection step 20", "step", "N_{sel}"); //2 -4 the same


  container1D eleceta0(etabinselec, "electron eta step 0", "#eta_{l}","N_{e}");
  container1D eleceta1(etabinselec, "electron eta step 1", "#eta_{l}","N_{e}");
  container1D eleceta2(etabinselec, "electron eta step 2", "#eta_{l}","N_{e}");
  container1D eleceta3(etabinselec, "electron eta step 3", "#eta_{l}","N_{e}");
  container1D eleceta20(etabinselec, "electron eta step 20", "#eta_{l}","N_{e}");
  container1D eleceta4(etabinselec, "electron eta step 4", "#eta_{l}","N_{e}");
  container1D eleceta5(etabinselec, "electron eta step 5", "#eta_{l}","N_{e}");
  container1D eleceta6(etabinselec, "electron eta step 6", "#eta_{l}","N_{e}");
  container1D eleceta7(etabinselec, "electron eta step 7", "#eta_{l}","N_{e}");
  container1D eleceta8(etabinselec, "electron eta step 8", "#eta_{l}","N_{e}");

  container1D elecpt0(ptbinsfull, "electron pt step 0", "p_{T} [GeV]", "N_{e}");
  container1D elecpt1(ptbinsfull, "electron pt step 1", "p_{T} [GeV]", "N_{e}");
  container1D elecpt2(ptbinsfull, "electron pt step 2", "p_{T} [GeV]", "N_{e}");
  container1D elecpt3(ptbinsfull, "electron pt step 3", "p_{T} [GeV]", "N_{e}");
  container1D elecpt20(ptbinsfull, "electron pt step 20", "p_{T} [GeV]", "N_{e}");
  container1D elecpt4(ptbinsfull, "electron pt step 4", "p_{T} [GeV]", "N_{e}");
  container1D elecpt5(ptbinsfull, "electron pt step 5", "p_{T} [GeV]", "N_{e}");
  container1D elecpt6(ptbinsfull, "electron pt step 6", "p_{T} [GeV]", "N_{e}");
  container1D elecpt7(ptbinsfull, "electron pt step 7", "p_{T} [GeV]", "N_{e}");
  container1D elecpt8(ptbinsfull, "electron pt step 8", "p_{T} [GeV]", "N_{e}");

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
  container1D muoneta20(etabinsmuon, "muon eta step 20", "#eta_{l}","N_{#mu}");
  container1D muoneta4(etabinsmuon, "muon eta step 4", "#eta_{l}","N_{#mu}");
  container1D muoneta5(etabinsmuon, "muon eta step 5", "#eta_{l}","N_{#mu}");
  container1D muoneta6(etabinsmuon, "muon eta step 6", "#eta_{l}","N_{#mu}");
  container1D muoneta7(etabinsmuon, "muon eta step 7", "#eta_{l}","N_{#mu}");
  container1D muoneta8(etabinsmuon, "muon eta step 8", "#eta_{l}","N_{#mu}");

  container1D muonpt0(ptbinsfull, "muon pt step 0", "p_{T} [GeV]", "N_{#mu}");
  container1D muonpt1(ptbinsfull, "muon pt step 1", "p_{T} [GeV]", "N_{#mu}");
  container1D muonpt2(ptbinsfull, "muon pt step 2", "p_{T} [GeV]", "N_{#mu}");
  container1D muonpt3(ptbinsfull, "muon pt step 3", "p_{T} [GeV]", "N_{#mu}");
  container1D muonpt20(ptbinsfull, "muon pt step 20", "p_{T} [GeV]", "N_{#mu}");
  container1D muonpt4(ptbinsfull, "muon pt step 4", "p_{T} [GeV]", "N_{#mu}");
  container1D muonpt5(ptbinsfull, "muon pt step 5", "p_{T} [GeV]", "N_{#mu}");
  container1D muonpt6(ptbinsfull, "muon pt step 6", "p_{T} [GeV]", "N_{#mu}");
  container1D muonpt7(ptbinsfull, "muon pt step 7", "p_{T} [GeV]", "N_{#mu}");
  container1D muonpt8(ptbinsfull, "muon pt step 8", "p_{T} [GeV]", "N_{#mu}");

  container1D muoniso0(isobins, "muon isolation step 0", "Iso", "N_{#mu}");
  container1D muoniso1(isobins, "muon isolation step 1", "Iso", "N_{#mu}");
  container1D muoniso2(isobins, "muon isolation step 2", "Iso", "N_{#mu}");
  container1D muoniso3(isobins, "muon isolation step 3", "Iso", "N_{#mu}");
  container1D muoniso4(isobins, "muon isolation step 4", "Iso", "N_{#mu}");

  container1D vertexmulti0(multibinsvertx, "vertex multiplicity step 0", "n_{vtx}", "N_{evt}");
  container1D vertexmulti1(multibinsvertx, "vertex multiplicity step 1", "n_{vtx}", "N_{evt}");
  container1D vertexmulti2(multibinsvertx, "vertex multiplicity step 2", "n_{vtx}", "N_{evt}");
  container1D vertexmulti3(multibinsvertx, "vertex multiplicity step 3", "n_{vtx}", "N_{evt}");
  container1D vertexmulti20(multibinsvertx, "vertex multiplicity step 20", "n_{vtx}", "N_{evt}");
  container1D vertexmulti4(multibinsvertx, "vertex multiplicity step 4", "n_{vtx}", "N_{evt}");
  container1D vertexmulti5(multibinsvertx, "vertex multiplicity step 5", "n_{vtx}", "N_{evt}");
  container1D vertexmulti6(multibinsvertx, "vertex multiplicity step 6", "n_{vtx}", "N_{evt}");
  container1D vertexmulti7(multibinsvertx, "vertex multiplicity step 7", "n_{vtx}", "N_{evt}");
  container1D vertexmulti8(multibinsvertx, "vertex multiplicity step 8", "n_{vtx}", "N_{evt}");

  container1D diletaZ3(etabinsdil, "dilepton eta step 3", "#eta_{ll}", "N_{evt}");

  container1D invmass2(massbins, "dilepton invariant mass step 2", "m_{ll} [GeV]", "N_{evt}");
  container1D invmass3(massbins, "dilepton invariant mass step 3", "m_{ll} [GeV]", "N_{evt}");
  container1D invmass20(massbins, "dilepton invariant mass step 20", "m_{ll} [GeV]", "N_{evt}"); //Z
  container1D invmass4(massbins, "dilepton invariant mass step 4", "m_{ll} [GeV]", "N_{evt}");
  container1D invmass5(massbins, "dilepton invariant mass step 5", "m_{ll} [GeV]", "N_{evt}");
  container1D invmass6(massbins, "dilepton invariant mass step 6", "m_{ll} [GeV]", "N_{evt}");
  container1D invmass7(massbins, "dilepton invariant mass step 7", "m_{ll} [GeV]", "N_{evt}");
  container1D invmass8(massbins, "dilepton invariant mass step 8", "m_{ll} [GeV]", "N_{evt}");
  container1D invmass9(massbins, "dilepton invariant mass step 9", "m_{ll} [GeV]", "N_{evt}");

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

  container1D met6u(ptbins, "missing transverse energy uncorr step 6","E_{T,miss} [GeV]", "N_{evt}");
  container1D met6(ptbins, "missing transverse energy step 6","E_{T,miss} [GeV]", "N_{evt}");
  container1D met7(ptbins, "missing transverse energy step 7","E_{T,miss} [GeV]", "N_{evt}");
  container1D met8(ptbins, "missing transverse energy step 8","E_{T,miss} [GeV]", "N_{evt}");
  container1D met9(ptbins, "missing transverse energy step 9","E_{T,miss} [GeV]", "N_{evt}");

  container1D btagmulti7(multibinsbtag, "b-jet multiplicity step 7", "n_{b-tags}", "N_{jets}",true);
  container1D btagmulti8(multibinsbtag, "b-jet multiplicity step 8", "n_{b-tags}", "N_{jets}",true);
  container1D btagmulti9(multibinsbtag, "b-jet multiplicity step 9", "n_{b-tags}", "N_{jets}",true);

  container1D btagScFs(bsfs, "b-tag event SFs", "SF_{evt}", "N_{evt}",true);


  container1D::c_makelist=false; //switch off automatic listing




  //get the lepton selector (maybe directly in the code.. lets see)

  //leptonSelector2 lepSel;
  //lepSel.setUseRhoIsoForElectrons(false);
  double oldnorm=norm;

  /////some boolians //channels
  bool b_mumu=false;bool b_ee=false;bool b_emu=false;;
  if(channel_.Contains("mumu")){
    b_mumu=true;
    cout << "\nrunning in mumu mode" <<endl;
  }
  else if (channel_.Contains("ee")){
    b_ee=true;
    cout << "\nrunning in ee mode" <<endl;
  }
  else if (channel_.Contains("emu")){
    b_emu=true;
    cout << "\nrunning in emu mode" <<endl;
  }
  bool is7TeV=false;
  if(energy_.Contains("7TeV")){
    is7TeV=true;
    cout << "running with 2011 data/MC!" <<endl;
  }

  TFile *f=TFile::Open(datasetdirectory_+inputfile);
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
    else{
      generated2=generated;
    }

  }
  else{
    norm=1;
  }


  
  // getBTagSF()->prepareEff(inputfile , norm );
  getBTagSF()->setSampleName(toString(inputfile));

  cout << "running on: " << datasetdirectory_ << inputfile << "    legend: " << legendname << "\nxsec: " << oldnorm << ", genEvents: " << genentries <<endl;
  // get main analysis tree

  /////////prepare collections

  TTree * t = (TTree*) f->Get("PFTree/PFTree");

  cout << "opened tree" << endl;

  TBranch * b_TriggerBools=0;
  std::vector<bool> * p_TriggerBools=0;
  t->SetBranchAddress("TriggerBools",&p_TriggerBools, &b_TriggerBools);

  //0 Ele
  //1,2 Mu

  TBranch * b_Electrons=0;
  vector<NTElectron> * pElectrons = 0;
  t->SetBranchAddress("NTPFElectrons",&pElectrons,&b_Electrons);

  TBranch * b_Muons=0;
  vector<NTMuon> * pMuons = 0;
  t->SetBranchAddress("NTMuons",&pMuons, &b_Muons); 

  TBranch * b_Jets=0;         // ##TRAP##
  vector<NTJet> * pJets=0;
  t->SetBranchAddress("NTJets",&pJets, &b_Jets);

  TBranch * b_Met=0;
  NTMet * pMet = 0;
  t->SetBranchAddress("NTMet",&pMet,&b_Met); 

  TBranch * b_Event=0;
  NTEvent * pEvent = 0;
  t->SetBranchAddress("NTEvent",&pEvent,&b_Event); 


  /*
  norm=1;
  isMC=false;
  */
  // just for sync reasons

  double sel_step[]={0,0,0,0,0,0,0,0,0};
    
  // start main loop /////////////////////////////////////////////////////////
  Long64_t nEntries=t->GetEntries();
  if(norm==0) nEntries=0; //skip for norm0

  //if(testmode) nEntries=10;

  for(Long64_t entry=0;entry<nEntries;entry++){
    if(showstatusbar_) displayStatusBar(entry,nEntries);

    b_TriggerBools->GetEntry(entry);

    //do trigger stuff - onlye 8TeV for now

    if(p_TriggerBools->size() < 3)
      continue;

    if(b_mumu){
      if(!(p_TriggerBools->at(1) || p_TriggerBools->at(2)))
	continue;
    }
    else if(b_ee){
      if(!p_TriggerBools->at(0))
	continue;
    }

    // t->GetEntry(entry);

    //make collections

    b_Muons->GetEntry(entry);

    vector<NTMuon*> kinmuons,idmuons,isomuons;
    for(size_t i=0;i<pMuons->size();i++){
      if(pMuons->at(i).pt() < 20)       continue;
      if(fabs(pMuons->at(i).eta())>2.4) continue;
      kinmuons << &(pMuons->at(i));
      ///select id muons
      if(!(pMuons->at(i).isGlobal() || pMuons->at(i).isTracker()) ) continue;
      idmuons <<  &(pMuons->at(i));

      if(pMuons->at(i).isoVal() > 0.2) continue;
      isomuons <<  &(pMuons->at(i));

    }

    if(b_mumu && kinmuons.size()<2)
      continue;

    b_Electrons->GetEntry(entry);

    vector<NTElectron *> kinelectrons,idelectrons,isoelectrons;
    for(size_t i=0;i<pElectrons->size();i++){
      if(pElectrons->at(i).ECalP4().Pt() < 20)  continue;
      if(fabs(pElectrons->at(i).eta()) > 2.5) continue;
      if(!noOverlap(&(pElectrons->at(i)), kinmuons, 0.1)) continue;   ////!!!CHANGE
      kinelectrons  << &(pElectrons->at(i));

      ///select id electrons
      if(fabs(pElectrons->at(i).d0V()) >0.04 ) continue;
      if(!(pElectrons->at(i).isNotConv()) ) continue;
      if(pElectrons->at(i).mvaId() < 0.5) continue;
      if(pElectrons->at(i).mHits() > 0) continue;

      idelectrons <<  &(pElectrons->at(i));

      //select iso electrons
      if(pElectrons->at(i).isoVal()>0.15) continue;
      isoelectrons <<  &(pElectrons->at(i));
    }

    
    // ask for the elecs/muons etc

    
    if(b_ee && kinelectrons.size() <2)
      continue;

    b_Jets->GetEntry(entry);

    vector<NTJet *> treejets,nolidjets,hardjets;
    for(size_t i=0;i<pJets->size();i++){
      treejets << &(pJets->at(i));
    }

    b_Event->GetEntry(entry);

    double puweight=1;
    if (isMC) puweight = getPUReweighter()->getPUweight(pEvent->truePU());
  
    ///triggers here - reimplement
    
    /////// make collections

    for(size_t i=0;i<kinelectrons.size();i++){
      //fill plots for kinelecs
      eleceta0.fill(kinelectrons.at(i)->eta(), puweight);
      elecpt0.fill(kinelectrons.at(i)->pt(),puweight);
      eleciso0.fill(kinelectrons.at(i)->isoVal(),puweight);
      elecid0.fill(kinelectrons.at(i)->mvaId(),puweight);
    }

    //some other fills
  

    for(size_t i=0;i<kinmuons.size();i++){
      muoneta0.fill(kinmuons.at(i)->eta(), puweight);
      muonpt0.fill(kinmuons.at(i)->pt(),puweight);
      muoniso0.fill(kinmuons.at(i)->isoVal(),puweight);
    }

    vertexmulti0.fill(pEvent->vertexMulti(),puweight);
    selection.fill(0,puweight);

    sel_step[0]+=puweight;

    //////////two ID leptons STEP 1///////////////////////////////

    if(b_ee && idelectrons.size() < 2) continue;
    if(b_mumu && idmuons.size() < 2 ) continue;
    if(b_emu && idmuons.size() + idelectrons.size() < 2) continue;


    for(size_t i=0;i<idelectrons.size();i++){
      eleceta1.fill(idelectrons.at(i)->eta(), puweight);
      elecpt1.fill(idelectrons.at(i)->pt(),puweight);
      eleciso1.fill(idelectrons.at(i)->isoVal(),puweight);
      elecid1.fill(idelectrons.at(i)->mvaId(),puweight);
      //some other fills
    }

    for(size_t i=0;i<idmuons.size();i++){
      muoneta1.fill(idmuons.at(i)->eta(), puweight);
      muonpt1.fill(idmuons.at(i)->pt(),puweight);
      muoniso1.fill(idmuons.at(i)->isoVal(),puweight);
    }


    vertexmulti1.fill(pEvent->vertexMulti(),puweight);
    selection.fill(1,puweight);

    sel_step[1]+=puweight;

    //////// require two iso leptons  STEP 2  //////

    if(b_ee && isoelectrons.size() < 2) continue;
    if(b_mumu && isomuons.size() < 2 ) continue;
    if(b_emu && isomuons.size() + isoelectrons.size() < 2) continue;
    
    //make pair
    pair<vector<NTElectron*>, vector<NTMuon*> > leppair;
    leppair = ztop::getOppoQHighestPtPair(isoelectrons, isomuons);

    double invLepMass=0;
    LorentzVector dilp4;

    if(b_ee){
    if(leppair.first.size() <2) continue;
    dilp4=leppair.first[0]->p4() + leppair.first[1]->p4();
    invLepMass=dilp4.M();
    }
    else if(b_mumu){
    if(leppair.second.size() < 2) continue;
    dilp4=leppair.second[0]->p4() + leppair.second[1]->p4();
    invLepMass=dilp4.M();
    }
    else if(b_emu){
    if(leppair.first.size() < 1 || leppair.second.size() < 1) continue;
    dilp4=leppair.first[0]->p4() + leppair.second[0]->p4();
    invLepMass=dilp4.M();
    }
   

    for(size_t i=0;i<isoelectrons.size();i++){
      eleceta2.fill(isoelectrons.at(i)->eta(), puweight);
      elecpt2.fill(isoelectrons.at(i)->pt(),puweight);
      eleciso2.fill(isoelectrons.at(i)->isoVal(),puweight);
      elecid2.fill(isoelectrons.at(i)->mvaId(),puweight);
      //some other fills
    }

    for(size_t i=0;i<isomuons.size();i++){
      muoneta2.fill(isomuons.at(i)->eta(), puweight);
      muonpt2.fill(isomuons.at(i)->pt(),puweight);
      muoniso2.fill(isomuons.at(i)->isoVal(),puweight);
    }

    invmass2.fill(invLepMass,puweight);
    vertexmulti2.fill(pEvent->vertexMulti(),puweight);
    selection.fill(2,puweight);

    sel_step[2]+=puweight;

    ///////// 20 GeV cut /// STEP 3 ///////////////////

    if(invLepMass < 20) 
      continue;
    

    // create jec jets for met and ID jets


    // create ID Jets and correct JER


    double dpx=0;
    double dpy=0;
    for(size_t i=0;i<treejets.size();i++){ //ALSO THE RESOLUTION AFFECTS MET. HERE INTENDED!!! GOOD?
      PolarLorentzVector oldp4=treejets.at(i)->p4();
      if(isMC){// && !is7TeV){     
	bool useJetForMet=false;
	if(treejets.at(i)->emEnergyFraction() < 0.9 && treejets.at(i)->pt() > 10)
	  useJetForMet=true; //dont even do something 
         
	getJECUncertainties()->applyToJet(treejets.at(i));
	getJERAdjuster()->correctJet(treejets.at(i));
	//corrected
	if(useJetForMet){
	  dpx += oldp4.Px() - treejets.at(i)->p4().Px();
	  dpy += oldp4.Py() - treejets.at(i)->p4().Py();
	}
      }
      if(!(treejets.at(i)->id())) continue;
      if(!noOverlap(treejets.at(i), isomuons, 0.4)) continue;
      if(!noOverlap(treejets.at(i), isoelectrons, 0.4)) continue;
      nolidjets << (treejets.at(i));

      if(treejets.at(i)->pt() < 30 || fabs(treejets.at(i)->eta())>2.5) continue;
      hardjets << treejets.at(i);
    }



    //fill container

    for(size_t i=0;i<nolidjets.size();i++){ 
      jetpt3.fill( nolidjets.at(i)->pt(),puweight);
      jeteta3.fill(nolidjets.at(i)->eta(),puweight);
    }

    for(size_t i=0;i<isoelectrons.size();i++){
      eleceta3.fill(isoelectrons.at(i)->eta(), puweight);
      elecpt3.fill(isoelectrons.at(i)->pt(),puweight);
      eleciso3.fill(isoelectrons.at(i)->isoVal(),puweight);
      //some other fills
    }

    for(size_t i=0;i<isomuons.size();i++){
      muoneta3.fill(isomuons.at(i)->eta(), puweight);
      muonpt3.fill(isomuons.at(i)->pt(),puweight);
      muoniso3.fill(isomuons.at(i)->isoVal(),puweight);
    }

    invmass3.fill(invLepMass,puweight);
    vertexmulti3.fill(pEvent->vertexMulti(),puweight);
    selection.fill(3,puweight);

    jetmulti3.fill(nolidjets.size(),puweight);

    sel_step[3]+=puweight;

    ////////////////////Z Selection//////////////////
    bool isZrange=false;

    if(invLepMass > 76 && invLepMass < 106){
      
      for(size_t i=0;i<isoelectrons.size();i++){
	eleceta20.fill(isoelectrons.at(i)->eta(), puweight);
	elecpt20.fill(isoelectrons.at(i)->pt(),puweight);
	//some other fills
      }
      
      for(size_t i=0;i<isomuons.size();i++){
	muoneta20.fill(isomuons.at(i)->eta(), puweight);
	muonpt20.fill(isomuons.at(i)->pt(),puweight);
      }
      
      invmass20.fill(invLepMass,puweight);
      
      Zfinal_selection20.fill(1,puweight);

      isZrange=true;

      //  diletaZ3.fill(dilp4.Eta(),puweight);

    }

    bool Znotemu=isZrange;
    if(b_emu) Znotemu=false;

    ////////////////////Z Veto Cut STEP 4 (incl. hard jets)////////////////////////////////////
    
    /////// create  hard jets ////////

    //fill


    if(!Znotemu){
      
      for(size_t i=0;i<hardjets.size();i++){
	jetpt4.fill(hardjets.at(i)->pt(),puweight);
	jeteta4.fill(hardjets.at(i)->eta(),puweight);
      }
      
      for(size_t i=0;i<isoelectrons.size();i++){
	eleceta4.fill(isoelectrons.at(i)->eta(), puweight);
	elecpt4.fill(isoelectrons.at(i)->pt(),puweight);
	eleciso4.fill(isoelectrons.at(i)->isoVal(),puweight);
	//some other fills
      }
      
      for(size_t i=0;i<isomuons.size();i++){
	muoneta4.fill(isomuons.at(i)->eta(), puweight);
	muonpt4.fill(isomuons.at(i)->pt(),puweight);
	muoniso4.fill(isomuons.at(i)->isoVal(),puweight);
      }

      invmass4.fill(invLepMass,puweight);
      vertexmulti4.fill(pEvent->vertexMulti(),puweight);
      jetmulti4.fill(hardjets.size(),puweight);
      selection.fill(4,puweight);

      sel_step[4]+=puweight;
      
    }
    if(isZrange){ //Z done above

    }


    ///////////////////// at least one jet cut STEP 5 ////////////  

    if(hardjets.size() < 1) continue;

    if(!Znotemu){

      for(size_t i=0;i<hardjets.size();i++){
	jetpt5.fill(hardjets.at(i)->pt(),puweight);
	jeteta5.fill(hardjets.at(i)->eta(),puweight);
      }
      
      for(size_t i=0;i<isoelectrons.size();i++){
	eleceta5.fill(isoelectrons.at(i)->eta(), puweight);
	elecpt5.fill(isoelectrons.at(i)->pt(),puweight);
	//	eleciso5.fill(elec->isoVal(),puweight);
	//some other fills
      }
      
      for(size_t i=0;i<isomuons.size();i++){
	muoneta5.fill(isomuons.at(i)->eta(), puweight);
	muonpt5.fill(isomuons.at(i)->pt(),puweight);
	//	muoniso5.fill(isomuons.at(i)->isoVal(),puweight);
      }
      
      invmass5.fill(invLepMass,puweight);
      vertexmulti5.fill(pEvent->vertexMulti(),puweight);
      jetmulti5.fill(hardjets.size(),puweight);
      selection.fill(5,puweight);

      sel_step[5]+=puweight;

    }
    if(isZrange){
      invmassZ5.fill(invLepMass,puweight);
    }



    /////////////////////// at least two jets STEP 6 /////////////

    ///adjust MET ///
    b_Met->GetEntry(entry);

    NTMet adjustedmet = *pMet;
    double nmpx=pMet->p4().Px() + dpx;
    double nmpy=pMet->p4().Py() + dpy;
    adjustedmet.setP4(LorentzVector(nmpx,nmpy,0,sqrt(nmpx*nmpx+nmpy*nmpy)));
    


    if(hardjets.size() < 2) continue;

    if(!Znotemu){

      for(size_t i=0;i<hardjets.size();i++){
	jetpt6.fill(hardjets.at(i)->pt(),puweight);
	jeteta6.fill(hardjets.at(i)->eta(),puweight);
      }
      
      for(size_t i=0;i<isoelectrons.size();i++){
	eleceta6.fill(isoelectrons.at(i)->eta(), puweight);
	elecpt6.fill(isoelectrons.at(i)->pt(),puweight);
	//	eleciso6.fill(isoelectrons.at(i)->isoVal(),puweight);
	//some other fills
      }
      
      for(size_t i=0;i<isomuons.size();i++){
	muoneta6.fill(isomuons.at(i)->eta(), puweight);
	muonpt6.fill(isomuons.at(i)->pt(),puweight);
	//	muoniso6.fill(isomuons.at(i)->isoVal(),puweight);
      }
      
      invmass6.fill(invLepMass,puweight);
      vertexmulti6.fill(pEvent->vertexMulti(),puweight);
      jetmulti6.fill(hardjets.size(),puweight);
      selection.fill(6,puweight);
      met6u.fill(pMet->met(),puweight);
      met6.fill(adjustedmet.met(), puweight);

      sel_step[6]+=puweight;
    }
    if(isZrange){
      invmassZ6.fill(invLepMass,puweight);
    }




    ///make btagged jets //
    

   
    //////////////////// MET cut STEP 7//////////////////////////////////
    if(adjustedmet.met() < 40) continue;


    vector<NTJet*> btaggedjets;
    for(size_t i=0;i<hardjets.size();i++){
      getBTagSF()->fillEff(hardjets.at(i), puweight);
      //  cout << hardjets.at(i)->genPartonFlavour() << endl;
      if(hardjets.at(i)->btag() < 0.244) continue;
      btaggedjets.push_back(hardjets.at(i));
    }

    if(!Znotemu){

      for(size_t i=0;i<hardjets.size();i++){
	jetpt7.fill(hardjets.at(i)->pt(),puweight);
	jeteta7.fill(hardjets.at(i)->eta(),puweight);
      }
      
      for(size_t i=0;i<isoelectrons.size();i++){
	eleceta7.fill(isoelectrons.at(i)->eta(), puweight);
	elecpt7.fill(isoelectrons.at(i)->pt(),puweight);
	//	eleciso7.fill(isoelectrons.at(i)->isoVal(),puweight);
	//some other fills
      }
      
      for(size_t i=0;i<isomuons.size();i++){
	muoneta7.fill(isomuons.at(i)->eta(), puweight);
	muonpt7.fill(isomuons.at(i)->pt(),puweight);
	//	muoniso7.fill(isomuons.at(i)->isoVal(),puweight);
      }
      
      invmass7.fill(invLepMass,puweight);
      vertexmulti7.fill(pEvent->vertexMulti(),puweight);
      jetmulti7.fill(hardjets.size(),puweight);
      selection.fill(7, puweight);
      met7.fill(adjustedmet.met(), puweight);
      btagmulti7.fill(btaggedjets.size(),puweight);

      sel_step[7]+=puweight;
    }
    if(isZrange){
      invmassZ7.fill(invLepMass,puweight);
    }


    ///////////////////// btag cut STEP 8 //////////////////////////

    if(btaggedjets.size() < 1) continue;

    double bsf=1;//getBTagSF()->getSF(hardjets); // returns 1 for data!!!
    bsf=getBTagSF()->getNTEventWeight(hardjets);
    //  if(bsf < 0.3) cout << bsf << endl;
    btagScFs.fill(bsf, puweight);
    //  puweight= puweight * bsf;

    if(!Znotemu){

      for(size_t i=0;i<hardjets.size();i++){
	jetpt8.fill(hardjets.at(i)->pt(),puweight);
	jeteta8.fill(hardjets.at(i)->eta(),puweight);
      }
      
      for(size_t i=0;i<isoelectrons.size();i++){
	eleceta8.fill(isoelectrons.at(i)->eta(), puweight);
	elecpt8.fill(isoelectrons.at(i)->pt(),puweight);
	//	eleciso8.fill(isoelectrons.at(i)->isoVal(),puweight);
	//some other fills
      }
      
      for(size_t i=0;i<isomuons.size();i++){
	muoneta8.fill(isomuons.at(i)->eta(), puweight);
	muonpt8.fill(isomuons.at(i)->pt(),puweight);
	//	muoniso8.fill(isomuons.at(i)->isoVal(),puweight);
      }
      
      invmass8.fill(invLepMass,puweight);
      vertexmulti8.fill(pEvent->vertexMulti(),puweight);
      jetmulti8.fill(hardjets.size(),puweight);
      selection.fill(8,puweight);
      met8.fill(adjustedmet.met(), puweight);
      btagmulti8.fill(btaggedjets.size(),puweight);

      ttfinal_selection8.fill(1,puweight);
      sel_step[8]+=puweight;

    }
    if(isZrange){
      invmassZ8.fill(invLepMass,puweight);
    }

    if(btaggedjets.size() < 2) continue;
    if(!Znotemu){
      selection.fill(9,puweight);
      btagmulti9.fill(btaggedjets.size(),puweight);
      met9.fill(adjustedmet.met(), puweight);

      ttfinal_selection9.fill(1,puweight);

    }
    if(isZrange){
      invmassZ9.fill(invLepMass,puweight);
    }


  }//main event loop ends
  std::cout << std::endl; //for status bar

  // Fill all containers in the stackVector

  // std::cout << "Filling containers to the Stack\n" << std::endl;
  getPlots()->addList(legendname,color,norm);

  for(unsigned int i=0;i<9;i++){
    std::cout << "selection step "<< toTString(i)<< " "  << sel_step[i];
    if(i==3)
      cout << "  => sync step 1";
    if(i==4)
      cout << "  => sync step 2";
    if(i==6)
      cout << "  => sync step 3";
    if(i==7)
      cout << "  => sync step 4";
    if(i==8)
      cout << "  => sync step 5";
    std::cout  << std::endl;
  }

  delete t;
  f->Close();
  delete f;

  getBTagSF()->makeEffs();

  

}


