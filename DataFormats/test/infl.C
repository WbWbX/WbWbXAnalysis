#include "TTree.h"
#include "TFile.h"
#include <fstream>
#include "WbWbXAnalysis/DataFormats/src/classes.h" 
#include "WbWbXAnalysis/Tools/interface/histoStackVector.h"
#include "WbWbXAnalysis/Tools/interface/miscUtils.h"


void infl(){

  using namespace top;
  using namespace std;

  TFile *f=TFile::Open("/scratch/hh/dust/naf/cms/user/kieseler/trees_8TeV_SUSY/tree_8TeV_singlee_runA_06Aug.root");

  TTree * t = (TTree*) f->Get("PFTree/pfTree");
  float n=t->GetEntries();

  vector<NTInflatedElectron> * pElecs = 0;
  t->SetBranchAddress("NTInflatedElectrons",&pElecs); 

  vector<float> bins;
  for(int i=0;i<32;i++) bins << i;


  histo1D cont(bins);

  for(float i=0;i<n;i++){
    t->GetEntry(i);
    displayStatusBar(i,n);
    for(unsigned int j=0;j<pElecs->size();j++)
      cont.fill(pElecs->at(j).getMember("vbtf11WP80"));
  }
  cout << endl;
  cont.getTH1D()->Draw();


}
