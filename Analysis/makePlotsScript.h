#include "TtZAnalysis/Tools/interface/containerStackVector.h"
#include "TtZAnalysis/Tools/interface/containerUF.h"
#include "TtZAnalysis/Tools/interface/miscUtils.h"

void makePlotsScript(TString out_dir){

  using namespace std;
  using namespace top;

  TString outdir=out_dir+"/controlPlots";
  TString cmdtemp="mkdir "+outdir;
  const char * outdir_c=cmdtemp;

  system(outdir_c);

  TString infile="fullcontrol_and_xsec_graphs.root";

  vector<TString> plots,cvsvs;
  cvsvs << "8TeV_allErrors_ee_BGErrors"
	<< "8TeV_allErrors_mumu_BGErrors"
	<< "7TeV_allErrors_ee_BGErrors"
	<< "7TeV_allErrors_mumu_BGErrors";


  plots << "vertex multiplicity step 3"
	<< "dilepton invariant mass step 3"
	<< "jet multiplicity step 4"
	<< "jet pt step 4"
	<< "missing transverse energy step 6"
	<< "b-jet multiplicity step 7"
	<< "b-jet multiplicity step 8"
	<< "muon pt step 8"
	<< "electron pt step 8"
	<< "missing transverse energy step 8";
	

  //getStack makeTCanvas() c->Print()

  TFile * f = new TFile(infile);
  TTree * t=(TTree*) f->Get("stored_objects");

  container1DStackVector vec;
  TFile * fout = new TFile("selectedPlots.root","RECREATE");
  fout->cd();

  TCanvas * c;
  
  for(size_t j=0;j<cvsvs.size();j++){
    vec.loadFromTree(t,cvsvs.at(j));

    for(size_t i=0;i<plots.size();i++){
      container1DStack stack = vec.getStack(plots.at(i));
      TString outname = stack.getName() + "_" + cvsvs.at(j) + ".pdf";
     
      c=stack.makeTCanvas();
      c->Write();
      c->Print(outdir + "/" + stack.getName() + "_" + cvsvs.at(j) + ".pdf");
      delete c;
    //set name to plottet stack to stackname+ cvsvs.at(i) + pdf

    }
  }

  //delete c;
  f->Close();
  delete f;
  fout->Close();
  delete fout;
}


