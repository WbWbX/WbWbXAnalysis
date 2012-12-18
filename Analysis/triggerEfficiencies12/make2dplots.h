#include "TFile.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TString.h"
#include "TPad.h"
#include "TStyle.h"
#include "TGraphAsymmErrors.h"

void make2dplots(TString inputfile, TString output){

 TFile *f = new TFile(inputfile);

 TH2D * hsf = (TH2D*)f->Get("eta2d eff");

 gStyle->SetOptTitle(0);
 gStyle->SetOptStat(0);

 hsf->GetYaxis()->SetTitleSize(0.06);
 hsf->GetXaxis()->SetTitleSize(0.05);
 hsf->GetYaxis()->SetLabelSize(0.05);
 hsf->GetXaxis()->SetLabelSize(0.05);
    
 hsf->GetXaxis()->SetTitle("#eta_{#mu_{1}}");
 hsf->GetYaxis()->SetTitle("#eta_{#mu_{2}}");

 TCanvas * c = new TCanvas();

 hsf->Draw("colz,text,e");
 gPad->SetLeftMargin(0.15);
 gPad->SetBottomMargin(0.15);


 hsf->Draw("colz,text,e");

 c->Print(output+".pdf");

}
