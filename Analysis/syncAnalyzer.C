#include "../DataFormats/src/classes.h" //gets all the dataformats
#include "../DataFormats/interface/elecRhoIsoAdder.h"
#include "../plugins/reweightPU.h"
#include "../plugins/leptonSelector.h"
#include "../plugins/miscUtils.h"
#include "../plugins/containerStackVector.h"
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
  MainAnalyzer(){filelist_="";dataname_="data";}
  MainAnalyzer(TString Name){name_=Name;}
  ~MainAnalyzer(){};
  void setName(TString Name){name_=Name;}
  void setLumi(double Lumi){lumi_=Lumi;}

  void analyze(TString, TString, int, double);

  void setFileList(const char* filelist){filelist_=filelist;}

  top::container1DStackVector & getPlots(){return analysisPlots;}

  void start();

  void clear(){analysisPlots.clear();}

  void setAdditionalInfoString(TString add){additionalinfo_=add;} //!for adding things like JEC up or other systematics options

  top::PUReweighter & getPUReweighter(){return puweighter_;}

private:

  TString name_,dataname_;
  double lumi_;

  const char * filelist_;
  
  top::PUReweighter puweighter_;
  top::container1DStackVector analysisPlots;

  TString additionalinfo_;

};

void MainAnalyzer::start(){
  using namespace std;
  analysisPlots.setName(name_);
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

void  MainAnalyzer::analyze(TString inputfile, TString legendname, int color, double norm){ // do analysis here and store everything in analysisPlots

  using namespace std;
  using namespace top;
  //define containers here

  //   define binnings

  vector<float> etabins;
  etabins << -2.5 << -1.8 << -1 << 1 << 1.8 << 2.5 ;
  vector<float> ptbins;
  ptbins << 0 << 10 << 20 << 30 << 40 << 50 << 70 << 100 << 200;
  vector<float> massbins;
  for(int i=0;i<30;++i) massbins << 10*i;

  //and so on

  ///  define containers

  container1D eleceta1(etabins, "lepton eta step 1", "#eta_{l}","N_{evt}");
  container1D elecpt1(ptbins, "lepton pt step 1", "p_{T} [GeV]", "N_{evt}");

  container1D invmass1(massbins, "lepton invariant mass step4", "m_{ll} [GeV]", "N_{evt}");

  //get the lepton selector (maybe directly in the code.. lets see)

  leptonSelector lepSel;
  lepSel.setUseRhoIsoForElectrons(true);

  //just for testing
  // inputfile = "/afs/naf.desy.de/user/k/kieseler/scratch/2012/TestArea2/CMSSW_5_2_5/src/TtZAnalysis/Analysis/" +inputfile;

  TFile *f=TFile::Open(inputfile);
  cout << "running on: " << inputfile << "   legend: " << legendname << endl;
  TTree * tnorm = (TTree*) f->Get("preCutPUInfo/preCutPUInfo");
  // if(legendname!="data"){

  //  norm = lumi_ * norm / (tnorm->GetEntries());
    // }
    // else{
    //   norm=1;
    //  }
  // get main analysis tree

  norm=1;
  if(legendname!="data") norm=0.97;

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


  // start main loop
  for(float entry=0;entry<t->GetEntries();entry++){
    t->GetEntry(entry);
    
    double puweight;
    if(legendname==dataname_) puweight=1;
    else puweight = puweighter_.getPUweight(pEvent->truePU());

    //lepton collections
    vector<NTElectron> idelectrons   =lepSel.selectIDElectrons(*pElectrons);
    vector<NTMuon> idmuons           =lepSel.selectIDMuons(*pMuons);
    vector<NTElectron> isoelectrons  =lepSel.selectIsolatedElectrons(idelectrons);
    vector<NTMuon> isomuons          =lepSel.selectIsolatedMuons(idmuons);

    for(NTElectronIt elec=idelectrons.begin();elec<idelectrons.end();++elec){
      eleceta1.fill(elec->eta(), puweight);
      elecpt1.fill(elec->pt(),puweight);
    //some other fills
    }

    for(NTMuonIt muon=idmuons.begin();muon<idmuons.end();++muon){

      
    }

    // just fill the invariant mass distrib for fun
    if(idelectrons.size() >1){
      invmass1.fill((idelectrons[0].p4() + idelectrons[1].p4()).M(),puweight);
    }

    //some more analysis code


  } //main event loop ends

  // Fill all containers in the stackVector
  cout << "adding containers to the stack.\n" << endl;

  analysisPlots.add(eleceta1,legendname,color,norm);
  analysisPlots.add(invmass1,legendname,color,norm);
  f->Close();
  delete f;
  //and so on

}


void syncAnalyzer(){

  TString outfile = "syncOut.root";

  MainAnalyzer analyzer("default");
  analyzer.setLumi(5000);
  analyzer.setFileList("inputfiles.txt");
  analyzer.getPUReweighter().setDataTruePUInput("/afs/naf.desy.de/user/k/kieseler/public/dataPileUp_23_06.root");
  analyzer.getPUReweighter().setMCDistrSum12();
  analyzer.start();
  analyzer.getPlots().writeAllToTFile(outfile);


 

}
