#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include <iostream>

void getSubSetRoot(TString dirname){
  using namespace ztop;
  using namespace std;

  std::vector<TString> plots;
  plots << "lepton_pt" << "lepton_eta" << "dilepton_mll" << "lepton_multi"
	<< "alllepton_multi" << "lepton_etafine" << "lepton_iso";


 

  TFile *f =new TFile(dirname+"/raw.root");
  TFile *fout = new TFile("plotHistos.root","RECREATE");

  //get num and den

  std::vector<TString> adds;
  adds << "_num" << "_den" << "_num_mc" << "_den_mc";


  for(size_t i=0;i<plots.size();i++){
    TString plotname=plots.at(i);
    for(size_t j=0;j<adds.size();j++){
      TString name=plotname+adds.at(j);
      TObject * obj = f->Get(name);
      fout->cd();
      obj->Write();
    }
  }

  TFile *f3 =new TFile(dirname+"/scalefactors.root");
  std::vector<TString> addssf;
  addssf << "_eff" << "_eff_mc";

  
  for(size_t i=0;i<plots.size();i++){
    TString plotname=plots.at(i);
    for(size_t j=0;j<addssf.size();j++){
      TString name=plotname+addssf.at(j);
      TObject * obj = f3->Get(name);
      fout->cd();
      obj->Write();
    }
  }
  
}


// run number runcrecoverbla rereco 201191 one single run
