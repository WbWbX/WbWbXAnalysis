#include "TtZAnalysis/DataFormats/src/classes.h" //gets all the dataformats
#include "TtZAnalysis/DataFormats/interface/elecRhoIsoAdder.h"
#include "TtZAnalysis/plugins/reweightPU.h"
#include "TtZAnalysis/plugins/leptonSelector.h"
#include "TtZAnalysis/Tools/interface/miscUtils.h"
#include "TtZAnalysis/Tools/interface/containerStackVector.h"
#include "TtZAnalysis/Tools/interface/containerUF.h"
#include "TtZAnalysis/plugins/JERAdjuster.h"
#include "TtZAnalysis/Tools/interface/JECUncertainties.h"
#include "TtZAnalysis/Tools/interface/bTagSF.h"
#include "TTree.h"
#include "TFile.h"
#include <fstream>

#include <stdlib.h>

bool testmode=true;


std::vector<TString> dycontributions;

namespace top{
  typedef std::vector<top::NTElectron>::iterator NTElectronIt;
  typedef std::vector<top::NTMuon>::iterator NTMuonIt;
  typedef std::vector<top::NTJet>::iterator NTJetIt;
  typedef std::vector<top::NTTrack>::iterator NTTrackIt;
  typedef std::vector<top::NTSuClu>::iterator NTSuCluIt;
}

///// now available: removeContribution; use it to run on the systematics etc (only signal) or pdf stuff (remove nominal,for(i) add pdf[i], xsec, remove pdf[i])

//// run in batch mode otherwise it is dramatically slowed down by all the drawing stuff; the latter might also produce seg violations in the canvas libs.
//// name Z contribution "Z" or something similar for generator stuff and so on


void rescaleDY(top::container1DStackVector * vec, std::vector<TString> contributions, double scalescale=1, bool textout=true, TString identifier="dilepton invariant massZ "){
 
  std::vector<TString> ident;ident.clear();
  std::vector<double> scales;scales.clear();
  for(int i=5;i<=9;i++){
    TString stepstring="step "+toTString(i);
    double dymc = 0;
    std::vector<TString> allbutdyanddata;

    for(unsigned int j=0;j<contributions.size();j++){
      dymc += vec->getStack(identifier+stepstring).getContribution(contributions.at(j)).integral();
      allbutdyanddata << contributions.at(j);
    }
    double d = vec->getStack(identifier+stepstring).getContribution("data").integral();
    allbutdyanddata << "data";
    double rest = vec->getStack(identifier+stepstring).getContributionsBut(allbutdyanddata).integral();
    if(rest==0) rest=1;
    double scale = (d-rest)/dymc;
    scales << scale*scalescale;
    ident << stepstring;
    if(textout) std::cout << "Scalefactor for "<< vec->getName() << " " << stepstring << ": " << scale << std::endl;
  }
  //  top::container1DStackVector rescaled=vec;
  for(unsigned int i=0;i<contributions.size();i++){
    vec->multiplyNorms(contributions.at(i), scales, ident);
  }

}




class MainAnalyzer{

public:
  MainAnalyzer(){
    filelist_="";
    dataname_="data";
    analysisplots_ = new top::container1DStackVector();
    puweighter_= new top::PUReweighter();
    jeradjuster_= new top::JERAdjuster();
    jecuncertainties_=new top::JECUncertainties();
    btagsf_=new top::bTagSF();
  }
  MainAnalyzer(const MainAnalyzer &);
  MainAnalyzer(TString Name, TString additionalinfo){
    name_=Name;additionalinfo_=additionalinfo;
    dataname_="data";
    analysisplots_ = new top::container1DStackVector(Name);
    puweighter_= new top::PUReweighter();
    jeradjuster_= new top::JERAdjuster();
    jecuncertainties_=new top::JECUncertainties();
    btagsf_=new top::bTagSF();
  }
  ~MainAnalyzer(){if(analysisplots_) delete analysisplots_;if(puweighter_) delete puweighter_;if(jeradjuster_) delete jeradjuster_;if(jecuncertainties_) delete jecuncertainties_; if(btagsf_) delete btagsf_;};
  void setName(TString Name, TString additionalinfo){name_=Name;additionalinfo_=additionalinfo;analysisplots_->setName(Name);}
  void setLumi(double Lumi){lumi_=Lumi;}

  void replaceInName(TString replace, TString with){name_.ReplaceAll(replace,with);analysisplots_->setName(name_);}
  TString getName(){return name_;}

  void analyze(TString, TString, int, double);

  void setFileList(const char* filelist){filelist_=filelist;}
  void setDataSetDirectory(TString dir){datasetdirectory_=dir;}
  void setShowStatusBar(bool show){showstatusbar_=show;}

  top::container1DStackVector * getPlots(){return analysisplots_;}

  void start();
  //  void start(TString);

  void clear(){analysisplots_->clear();}

  void setAdditionalInfoString(TString add){additionalinfo_=add;} //!for adding things like JEC up or other systematics options

  top::PUReweighter * getPUReweighter(){return puweighter_;}

  top::JERAdjuster * getJERAdjuster(){return jeradjuster_;}
  top::JECUncertainties * getJECUncertainties(){return jecuncertainties_;}

  top::bTagSF * getBTagSF(){return btagsf_;}

  MainAnalyzer & operator= (const MainAnalyzer &);

private:

  void copyAll(const MainAnalyzer &);

  bool isInInfo(TString s){return additionalinfo_.Contains(s);}
  bool triggersContain(TString , top::NTEvent *);

  bool showstatusbar_;

  TString name_,dataname_;
  TString datasetdirectory_;
  double lumi_;

  TString filelist_;
  
  top::PUReweighter * puweighter_;
  top::JERAdjuster * jeradjuster_;
  top::JECUncertainties * jecuncertainties_;
  top::bTagSF * btagsf_;

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

  dycontributions.clear();
  dycontributions << "Z#rightarrowll" << "DY#rightarrowll";

  using namespace std;
  if(analysisplots_) clear();
  else cout << "warning analysisplots_ is null" << endl;
  analysisplots_->setName(name_+"_"+additionalinfo_);
  std::cout << "Starting analysis for " << name_ << std::endl;
  ifstream inputfiles (filelist_.Data());
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

  rescaleDY(analysisplots_, dycontributions);
  
}


void MainAnalyzer::copyAll(const MainAnalyzer & analyzer){
  name_=analyzer.name_;
  lumi_=analyzer.lumi_;
  additionalinfo_=analyzer.additionalinfo_;
  dataname_=analyzer.dataname_;
  datasetdirectory_=analyzer.datasetdirectory_;
  puweighter_= new top::PUReweighter(*analyzer.puweighter_);
  filelist_=analyzer.filelist_;
  jeradjuster_= new top::JERAdjuster(*analyzer.jeradjuster_);
  jecuncertainties_= new top::JECUncertainties(*analyzer.jecuncertainties_);
  btagsf_ = new top::bTagSF(*analyzer.btagsf_);
  analysisplots_ = new top::container1DStackVector(*analyzer.analysisplots_);
  showstatusbar_=analyzer.showstatusbar_;
}

MainAnalyzer::MainAnalyzer(const MainAnalyzer & analyzer){
  copyAll(analyzer);
}

MainAnalyzer & MainAnalyzer::operator = (const MainAnalyzer & analyzer){
   copyAll(analyzer);
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
  for(float i=0;i<100;i++) bsfs << 0.9 + 0.3 * (i/100);


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
  container1D invmass9(massbins, "dilepton invariant mass step 8", "m_{ll} [GeV]", "N_{evt}");

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

  leptonSelector lepSel;
  lepSel.setUseRhoIsoForElectrons(false);
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
  bool is7TeV=false;
  if(name_.Contains("7TeV") ||  isInInfo("7TeV")){
    is7TeV=true;
    cout << "running with 2011 data/MC!" <<endl;
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
    else{
      generated2=generated;
    }

  }
  else{
      norm=1;
  }


  
  getBTagSF()->prepareEff(inputfile , norm );

  cout << "running on: " << inputfile << "    legend: " << legendname << "\nxsec: " << oldnorm << ", genEvents: " << genentries <<endl;
  // get main analysis tree

  /////////prepare collections

  TTree * t = (TTree*) f->Get("PFTree/pfTree");

  vector<NTMuon> * pMuons = 0;
  t->SetBranchAddress("NTMuons",&pMuons); 
  vector<NTElectron> * pElectrons = 0;
  t->SetBranchAddress("NTPFElectrons",&pElectrons);           // ##TRAP##
  vector<NTJet> * pJets=0;
  t->SetBranchAddress("NTJets",&pJets);
  NTMet * pMet = 0;
  t->SetBranchAddress("NTMet",&pMet); 
  NTEvent * pEvent = 0;
  t->SetBranchAddress("NTEvent",&pEvent); 


  // just for sync reasons

  double sel_step[]={0,0,0,0,0,0,0,0,0};
    
  // start main loop /////////////////////////////////////////////////////////
  Long64_t nEntries=t->GetEntries();
  if(norm==0) nEntries=0; //skip for norm0

  if(testmode) nEntries=100;

  for(Long64_t entry=0;entry<nEntries;entry++){
    if(showstatusbar_) displayStatusBar(entry,nEntries);
    t->GetEntry(entry);

    
    //  if(!isMC && pEvent->runNo() > 196531) continue; // ##TRAP##
    
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
       if(!noOverlap(elec, *pMuons, 0.1)) continue;        //obsolete when using pfElectrons!!!! MAKE SURE ##TRAP##

      kinelectrons.push_back(*elec);
    }

    //   rho.setRho(pEvent->isoRho(2));
    //    rho.addRhoIso(kinelectrons);
    vector<NTMuon> kinmuons         = lepSel.selectKinMuons(*pMuons);
    

    /////////TRIGGER CUT and kin leptons STEP 0/////////////////////////////
    if(b_ee){
      if(kinelectrons.size()< 2) continue; //has to be 17??!!
      if(is7TeV){
	if(!(triggersContain("Ele17_SW_TightCaloEleId_Ele8_HE_L1R_v", pEvent) || triggersContain("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v", pEvent) || triggersContain("HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v", pEvent))) continue;
	if(isMC) puweight= puweight * 0.992; //ONLY trigger #TRAP#
      }
      else{                           
	if(isMC  && !triggersContain("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v" , pEvent) ) continue;
	if(!isMC && !triggersContain("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v" , pEvent) ) continue;

	if(isMC) puweight= puweight * 0.938; //full lep+id+trig SF from AN 12 389 ###TRAP##

      }

    }
    if(b_mumu){
      if(kinmuons.size() < 2) continue;
      //trg wildcards
      if(is7TeV){
	if(isMC && !triggersContain("HLT_DoubleMu7", pEvent)) continue;

	if(!isMC){
	  if(pEvent->runNo() < 163869){
	    if(!triggersContain("HLT_DoubleMu7", pEvent)) continue;
	  }
	  else{
	    if(!triggersContain("HLT_Mu13_Mu8_v", pEvent)) continue;
	  }
	}
	else{
	  puweight= puweight * 0.946; //ONLY trigger!
	}

      }
      else{
	if(isMC &&  !(triggersContain("HLT_Mu17_Mu8_v",pEvent)  || triggersContain("HLT_Mu17_TkMu8_v",pEvent))) continue;
	if(!isMC && !(triggersContain("HLT_Mu17_Mu8_v",pEvent)  || triggersContain("HLT_Mu17_TkMu8_v",pEvent))) continue;

	if(isMC) puweight= puweight * 0.962; //full lep+id+trig SF from AN 12 389 ###TRAP###

      }

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
      eleciso0.fill(elec->isoVal03(),puweight);
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

    sel_step[0]+=puweight;

    //////////two ID leptons STEP 1///////////////////////////////

    if(b_ee && idelectrons.size() < 2) continue;
    if(b_mumu && idmuons.size() < 2 ) continue;
    if(b_emu && idmuons.size() + idelectrons.size() < 2) continue;


    for(NTElectronIt elec=idelectrons.begin();elec<idelectrons.end();++elec){
      eleceta1.fill(elec->eta(), puweight);
      elecpt1.fill(elec->pt(),puweight);
      eleciso1.fill(elec->isoVal03(),puweight);
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

    sel_step[1]+=puweight;

    //////// require two iso leptons  STEP 2  //////

    if(b_ee && isoelectrons.size() < 2) continue;
    if(b_mumu && isomuons.size() < 2 ) continue;
    if(b_emu && isomuons.size() + isoelectrons.size() < 2) continue;

    //make pair
    pair<vector<NTElectron>, vector<NTMuon> > leppair;
    leppair = lepSel.getOppoQHighestPtPair(isoelectrons, isomuons);

    double invLepMass=0;
    LorentzVector dilp4;

    if(b_ee){
      if(leppair.first.size() <2) continue;
      dilp4=leppair.first[0].p4() + leppair.first[1].p4();
      invLepMass=dilp4.M();
    }
    else if(b_mumu){
      if(leppair.second.size() < 2) continue;
      dilp4=leppair.second[0].p4() + leppair.second[1].p4();
      invLepMass=dilp4.M();
    }
    else if(b_emu){
         if(leppair.first.size() < 1 || leppair.second.size() < 1) continue;
	 dilp4=leppair.first[0].p4() + leppair.second[0].p4();
         invLepMass=dilp4.M();
    }

    for(NTElectronIt elec=isoelectrons.begin();elec<isoelectrons.end();++elec){
      eleceta2.fill(elec->eta(), puweight);
      elecpt2.fill(elec->pt(),puweight);
      eleciso2.fill(elec->isoVal03(),puweight);
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

    sel_step[2]+=puweight;

    ///////// 20 GeV cut /// STEP 3 ///////////////////

    if(invLepMass < 20) continue;
    

    // create jec jets for met and ID jets


    // create ID Jets and correct JER

    vector<NTJet> nolidjets;
    nolidjets.clear();
    double dpx=0;
    double dpy=0;
    vector<NTJet> treejets=*pJets;

    for(NTJetIt jet=treejets.begin();jet<treejets.end();++jet){ //ALSO THE RESOLUTION AFFECTS MET. HERE INTENDED!!! GOOD?
      LorentzVector oldp4=jet->p4();
      if(isMC){// && !is7TeV){               
	getJECUncertainties()->applyJECUncertainties(jet);
	getJERAdjuster()->correctJet(jet);
	//corrected
	if(jet->emEnergyFraction() < 0.9 && jet->pt() > 10){
	  dpx += oldp4.Px() - jet->p4().Px();
	  dpy += oldp4.Py() - jet->p4().Py();
	}
      }
      if(!(jet->id())) continue;
      if(!noOverlap(jet, isomuons, 0.4)) continue;
      if(!noOverlap(jet, isoelectrons, 0.4)) continue;
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
      eleciso3.fill(elec->isoVal03(),puweight);
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

    sel_step[3]+=puweight;

    ////////////////////Z Selection//////////////////
    bool isZrange=false;

    if(invLepMass > 76 && invLepMass < 106){
      
      for(NTElectronIt elec=isoelectrons.begin();elec<isoelectrons.end();++elec){
	eleceta20.fill(elec->eta(), puweight);
	elecpt20.fill(elec->pt(),puweight);
	//some other fills
      }
      
      for(NTMuonIt muon=isomuons.begin();muon<isomuons.end();++muon){
	muoneta20.fill(muon->eta(), puweight);
	muonpt20.fill(muon->pt(),puweight);
      }
      
      invmass20.fill(invLepMass,puweight);
      

      Zfinal_selection20.fill(1,puweight);

      isZrange=true;

      diletaZ3.fill(dilp4.Eta(),puweight);

    }

    bool Znotemu=isZrange;
    if(b_emu) Znotemu=false;

    ////////////////////Z Veto Cut STEP 4 (incl. hard jets)////////////////////////////////////
    
    /////// create  hard jets ////////

    vector<NTJet> hardjets;
    hardjets.clear();
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
	eleciso4.fill(elec->isoVal03(),puweight);
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

      sel_step[4]+=puweight;
      
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
	//	eleciso5.fill(elec->isoVal03(),puweight);
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

      sel_step[5]+=puweight;

    }
    if(isZrange){
      invmassZ5.fill(invLepMass,puweight);
    }



    /////////////////////// at least two jets STEP 6 /////////////

    ///adjust MET ///

    NTMet adjustedmet = *pMet;
    double nmpx=pMet->p4().Px() + dpx;
    double nmpy=pMet->p4().Py() + dpy;
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
	//	eleciso6.fill(elec->isoVal03(),puweight);
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

      sel_step[6]+=puweight;
    }
    if(isZrange){
      invmassZ6.fill(invLepMass,puweight);
    }




    ///make btagged jets //
    

   
    //////////////////// MET cut STEP 7//////////////////////////////////
    if(adjustedmet.met() < 40) continue;


    vector<NTJet> btaggedjets;
    for(NTJetIt jet = hardjets.begin();jet<hardjets.end();++jet){
      getBTagSF()->fillEff(*jet, puweight);
      //  cout << jet->genPartonFlavour() << endl;
      if(jet->btag() < 0.244) continue;
      btaggedjets.push_back(*jet);
    }

    if(!Znotemu){

      for(NTJetIt jet=hardjets.begin();jet<hardjets.end();++jet){
	jetpt7.fill(jet->pt(),puweight);
	jeteta7.fill(jet->eta(),puweight);
      }
      
      for(NTElectronIt elec=isoelectrons.begin();elec<isoelectrons.end();++elec){
	eleceta7.fill(elec->eta(), puweight);
	elecpt7.fill(elec->pt(),puweight);
	//	eleciso7.fill(elec->isoVal03(),puweight);
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

      sel_step[7]+=puweight;
    }
    if(isZrange){
      invmassZ7.fill(invLepMass,puweight);
    }

    double bsf=getBTagSF()->getSF(hardjets); // returns 1 for data!!!
    btagScFs.fill(bsf, puweight);
    puweight= puweight * bsf;

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
	//	eleciso8.fill(elec->isoVal03(),puweight);
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


  } //main event loop ends
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


void Analyzer(){
  using namespace std;
  using namespace top;


  cout << "\n\n\n" <<endl; //looks better ;)

  TString outfile = "analysis_output.root";
  // TString pufolder="/afs/naf.desy.de/user/k/kieseler/scratch/2012/TestArea2/CMSSW_5_2_5/src/TtZAnalysis/Data/PUDistr/";
  // reorder the whole stuff in a way. specify ee mumu 7TeV 8TeV and then do the systematics "in parallel" to better keep track. maybe even produce a vector of analyzers as done for getCrossSections.C


  TString cmssw_base=getenv("CMSSW_BASE");
  TString scram_arch=getenv("SCRAM_ARCH");
  //initialize mumu

  double lumi8TeV=12100;
  double lumi8TeVunc=0.036; //?!
  double lumi7TeV=5100;
  double lumi7TeVunc=0.025; //?!

  bool runInNotQuietMode=true;

  TString treedir="/scratch/hh/dust/naf/cms/user/kieseler";
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
  mumu_8TeV.setDataSetDirectory(treedir+"/trees_8TeV/");
  mumu_8TeV.getPUReweighter()->setDataTruePUInput(cmssw_base+"/src/TtZAnalysis/Data/HCP.json.txt_PU.root");
  mumu_8TeV.getJECUncertainties()->setFile(cmssw_base+"/src/TtZAnalysis/Data/Summer12_V2_DATA_AK5PF_UncertaintySources.txt");
  mumu_8TeV.getPUReweighter()->setMCDistrSum12();
  mumu_8TeV.getBTagSF()->setMakeEff(true);
  if(!onlytest) mumu_8TeV.start();
  TFile * btagfile = new TFile("mumu_8TeV_btags.root","RECREATE");
  mumu_8TeV.getBTagSF()->writeToTFile(btagfile);
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
  ee_7TeV.setDataSetDirectory(treedir+"/trees_7TeV/");
  ee_7TeV.getPUReweighter()->setDataTruePUInput(cmssw_base+"/src/TtZAnalysis/Data/ReRecoNov2011.json_PU.root");
  ee_7TeV.getPUReweighter()->setMCDistrFall11();
  ee_7TeV.getJECUncertainties()->setFile(cmssw_base+"/src/TtZAnalysis/Data/JEC11_V12_AK5PF_UncertaintySources.txt");
  ee_7TeV.getBTagSF()->setMakeEff(true);
  ee_7TeV.getBTagSF()->setIs2011(true);
  if(!onlytest) ee_7TeV.start();
  btagfile = new TFile("ee_7TeV_btags.root","RECREATE");
  ee_7TeV.getBTagSF()->writeToTFile(btagfile);
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
  mumu_7TeV.getBTagSF()->writeToTFile(btagfile);
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

    temp=full_ana.at(i);
    temp.replaceInName("default","JES_up");
    temp.getJECUncertainties()->sources() << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 12 << 13 << 15;
    temp.getJECUncertainties()->setVariation("up");
    if(!onlytest) temp.start();
    temp.getPlots()->writeAllToTFile(outfile,false);
    full_errana.at(i).getPlots()->addMCErrorStackVector("JES_corr_up",*temp.getPlots());

    temp=full_ana.at(i);
    temp.replaceInName("default","JES_down");
    temp.getJECUncertainties()->sources() << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 12 << 13 << 15;
    temp.getJECUncertainties()->setVariation("down");
    if(!onlytest) temp.start();
    temp.getPlots()->writeAllToTFile(outfile,false);
    full_errana.at(i).getPlots()->addMCErrorStackVector("JES_corr_down",*temp.getPlots());

    //JES uncorrelated

    temp=full_ana.at(i);
    temp.replaceInName("default","JES_"+energystring+"_up");
    temp.getJECUncertainties()->sources() << 0 << 11 << 14;
    temp.getJECUncertainties()->setVariation("up");
    if(!onlytest) temp.start();
    temp.getPlots()->writeAllToTFile(outfile,false);
    full_errana.at(i).getPlots()->addMCErrorStackVector("JES_"+energystring+"_up",*temp.getPlots());

    temp=full_ana.at(i);
    temp.replaceInName("default","JES_"+energystring+"_down");
    temp.getJECUncertainties()->sources() << 0 << 11 << 14;
    temp.getJECUncertainties()->setVariation("down");
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
      pufile=cmssw_base+"/src/TtZAnalysis/Data/"+"ReRecoNov2011.json_PU_up.root";

    temp=full_ana.at(i);
    temp.replaceInName("default","PU_"+energystring+"_down");
    temp.getPUReweighter()->setDataTruePUInput(pufile);
    if(!onlytest) temp.start();
    temp.getPlots()->writeAllToTFile(outfile,false);
    full_errana.at(i).getPlots()->addMCErrorStackVector("PU_"+energystring+"_down",*temp.getPlots());

    //matching etc. to be yet done


 /*
full_ana.at(i).
  */
    cout << "writing full error output" << endl;
    full_errana.at(i).replaceInName("default","allErrors");
    full_errana.at(i).getPlots()->writeAllToTFile(outfile,false);
  }



  std::cout <<"\n\n\n\n\nFINISHED!" <<std::endl;

  
}
