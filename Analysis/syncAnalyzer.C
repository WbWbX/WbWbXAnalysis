#include "TopAnalysis/DataFormats/src/classes.h"
#include "TopAnalysis/DataFormats/src/elecRhoIsoAdder.h"
#include "~/public/forCarmen/plugins/reweightPU_new.h"
#include "~/public/forCarmen/plugins/leptonSelector.h"
#include "TTree.h"
#include "TFile.h"



class MainAnalyzer{

public:
  MainAnalyzer(){};
  ~MainAnalyzer(){};

private:

  TString inputfile;
  int color_;
  double norm_;
  TString name_;

};
void  MainAnalyzer::analyze(){

  using namespace std;
  using namespace top;

  leptonSelector lepSel;

  vector<NTMuon> * pMuons = 0;
  tMC->SetBranchAddress("NTMuons",&pMuons); 
  vector<NTElectron> * pElectrons = 0;
  tMC->SetBranchAddress("NTElectrons",&pElectrons);
  vector<NTTrack> * pTracks = 0;
  tMC->SetBranchAddress("NTTracks",&pTracks);
  vector<NTSuClu> * pSuClus = 0;
  tMC->SetBranchAddress("NTSuClu",&pSuClus);

  
}


void syncAnalyzer(){


  top::NTElectron ele;
  top::NTEvent evt;


}
