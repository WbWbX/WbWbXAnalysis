#ifndef makeplotsnice_h
#define makeplotsnice_h
#include "TFile.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TString.h"
#include "TPad.h"
#include "TStyle.h"
#include "TGraphAsymmErrors.h"
#include "TtZAnalysis/Tools/interface/miscUtils.h"

#include "TH2D.h"


void makeplot(TString inputfile, TString add){

  TLegend * leg =  new TLegend(0.35,0.15,0.70,0.42);
  leg->Clear();
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);

  TFile *f = new TFile(inputfile);
  
  TH1D *h = (TH1D*)f->Get("axis "+add);

  TGraphAsymmErrors *sfc=(TGraphAsymmErrors*) f->Get("scalefactor "+add+" incl corrErr");
  TGraphAsymmErrors *sf=(TGraphAsymmErrors*) f->Get("scalefactor "+add);
  TGraphAsymmErrors *d=(TGraphAsymmErrors*) f->Get(add+" eff");
  TGraphAsymmErrors *mc=(TGraphAsymmErrors*) f->Get(add+" effMC");

  h->SetAxisRange(0.6,1.1,"Y");
  // set style
   gStyle->SetOptTitle(0);
   gStyle->SetOptStat(0);

  sfc->SetLineColor(8);
  sf ->SetLineColor(8);
  mc->SetLineColor(kRed);
  h->GetYaxis()->SetTitle("#epsilon,SF");
  if(add=="pt") 
    h->GetXaxis()->SetTitle("p_{T} [GeV]");
  else if(add=="eta")
    h->GetXaxis()->SetTitle("#eta");
  else if(add=="dphi" || add=="dphi2"){
    h->GetXaxis()->SetTitle("| #delta#phi_{l,met} |");
  }
  else if(add=="vmulti"){
    h->GetXaxis()->SetTitle("n_{vtx}");
  }
  else if(add=="jetmulti"){
    h->GetXaxis()->SetTitle("n_{jet}");
  }

  if(!add.Contains("2d")){
  h->GetYaxis()->SetTitleSize(0.06);
  h->GetXaxis()->SetTitleSize(0.05);
  h->GetYaxis()->SetLabelSize(0.05);
  h->GetXaxis()->SetLabelSize(0.05);

  sfc->SetMarkerStyle(33);
  sfc->SetMarkerColor(8);
  sf->SetMarkerStyle(33);
  sf->SetMarkerColor(8);
  d->SetMarkerStyle(20);
  d->SetMarkerColor(1);
  mc->SetMarkerStyle(22);
  mc->SetMarkerColor(kRed);

  sfc->SetMarkerSize(1);
  sf->SetMarkerSize(1);
  d->SetMarkerSize(1);
  mc->SetMarkerSize(1);

  sfc->SetLineWidth(2);
  sf->SetLineWidth(2);
  d->SetLineWidth(2);
  mc->SetLineWidth(2);

  leg->AddEntry(sfc,"scale factor"  ,"pe");
  leg->AddEntry(d,"data efficiency"  ,"pe");
  leg->AddEntry(mc,"MC efficiency"  ,"pe");

  gPad->SetLeftMargin(0.15);
  gPad->SetBottomMargin(0.15);
  h->Draw("AXIS");
  sfc->Draw("P,e1,same");
  sf->Draw("P,e1,same");
  mc->Draw("P,e1,same");
  d->Draw("P,e1,same");
  leg->Draw("same");

  }
  ///2D stuff if f->GetName(Contains emu...

  else if(add.Contains("2d")){


    TH2D *hsf = (TH2D*)f->Get(add);
    hsf->GetYaxis()->SetTitleSize(0.06);
    hsf->GetXaxis()->SetTitleSize(0.05);
    hsf->GetYaxis()->SetLabelSize(0.05);
    hsf->GetXaxis()->SetLabelSize(0.05);
    TString xaxistitle=(TString) hsf->GetXaxis()->GetTitle() + "_{e_1}";
    TString yaxistitle=(TString) hsf->GetYaxis()->GetTitle() + "_{e_2}";
    TString filename=f->GetName();
    if(filename.Contains("_emu")){
      xaxistitle=(TString) hsf->GetXaxis()->GetTitle() + "_{e}";
      yaxistitle=(TString) hsf->GetYaxis()->GetTitle() + "_{#mu}";
    }
    else if(filename.Contains("_mumu")){
      xaxistitle=(TString) hsf->GetXaxis()->GetTitle() + "_{#mu_1}";
      yaxistitle=(TString) hsf->GetYaxis()->GetTitle() + "_{#mu_2}";
    }

    hsf->GetXaxis()->SetTitle(xaxistitle);
    hsf->GetYaxis()->SetTitle(yaxistitle);

    gPad->SetLeftMargin(0.15);
    gPad->SetBottomMargin(0.15);
  
    hsf->Draw("colz");
    hsf->Draw("text,e,same");

  }

  //different markers
  // dot data
  //stuff for other
}


void miniscript(TString outdir){

  // TString dir="/afs/naf.desy.de/user/k/kieseler/public/forCarmen/latestgreatesttriggerplots3/";
  TString rootsdir=outdir;

  std::vector<TString> channels, plots;
  channels << "ee" << "mumu" << "emu" ;
  plots << "pt" << "eta" << "dphi" << "dphi2" << "vmulti" << "jetmulti";

  TCanvas * c = new TCanvas("def","def");
  c->Clear();
  c->cd();

  for(unsigned int i=0;i<channels.size();i++){
    for(unsigned int j=0;j<plots.size();j++){
      makeplot(rootsdir+"triggerSummary_"+channels.at(i)+".root", plots.at(j));
      c->Print(outdir+"corr_"+channels.at(i)+plots.at(j)+".pdf");
      c->Print(outdir+"corr_"+channels.at(i)+plots.at(j)+".eps");
      c->Clear();
    }
  }
  


}

#endif
