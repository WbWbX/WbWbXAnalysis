#ifndef MAKE2DPLOTS_H
#define MAKE2DPLOTS_H

#include "TFile.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TString.h"
#include "TPad.h"
#include "TStyle.h"
#include "TGraphAsymmErrors.h"
#include <vector>
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"

void make2dplots(TString inputfile, TString plot, TString output){

 TFile *f = new TFile(inputfile);

 std::vector<TString> plots; adds << plot+" eff" << plot+" effMC" << "scalefactor "+plot;

 for(unsgined int i=0;i<plots.size();i++){

   TH2D * h = (TH2D*)f->Get(plots.at(i));
   
   gStyle->SetOptTitle(0);
   gStyle->SetOptStat(0);
   
   h->GetYaxis()->SetTitleSize(0.06);
   h->GetXaxis()->SetTitleSize(0.05);
   h->GetYaxis()->SetLabelSize(0.05);
   h->GetXaxis()->SetLabelSize(0.05);
   
   if(((TString)f->GetName()).Contains("mumu")){
     h->GetXaxis()->SetTitle("#eta_{#mu_{1}}");
     h->GetYaxis()->SetTitle("#eta_{#mu_{2}}");
   }
   if(((TString)f->GetName()).Contains("emu")){
     h->GetXaxis()->SetTitle("#eta_{e}");
     h->GetYaxis()->SetTitle("#eta_{#mu}");
   }
   if(((TString)f->GetName()).Contains("ee")){
     h->GetXaxis()->SetTitle("#eta_{e_{1}}");
     h->GetYaxis()->SetTitle("#eta_{e_{2}}");
   }
   
  
   
   //  h->Draw("colz,text,e");
   gPad->SetLeftMargin(0.15);
   gPad->SetBottomMargin(0.15);
   
   
   h->Draw("colz,text,e");
 }
}
#endif
