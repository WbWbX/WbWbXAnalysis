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
  TFile *fout = new TFile(dirname+"/plotHistos.root","RECREATE");

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

  TString channel="";
  if(dirname.Contains("ee")) channel="ee";
  else if(dirname.Contains("emu")) channel="emu";
  else if(dirname.Contains("mumu")) channel="mumu";
  else std::cout << "naming of triggerSummary  nor done automatically - if not intended, check dirnames (should contain channel name" << std::endl;

  TFile *fout2 = new TFile(dirname+"/triggerSummary_"+channel+".root","RECREATE"); 
  fout2->cd();
  std::vector<TString> addsdssf;
  addsdssf << "lepton_eta2d_sf";


  for(size_t i=0;i<addsdssf.size();i++){
    TString plotname=addsdssf.at(i);
    TObject * obj = f3->Get(plotname);
    fout2->cd();
    
    if((TString)obj->ClassName() == "TH2D"){
      TH2D * h=(TH2D*)f3->Get(plotname);
      h->SetName("scalefactor eta2d with syst");
      std::cout << "Writing: "<< h->GetName() << std::endl;
      h->Write();
    }
  }

}


// run number runcrecoverbla rereco 201191 one single run
