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
  h->GetYaxis()->SetTitle("#epsilon");
  if(add=="pt") 
    h->GetXaxis()->SetTitle("p_{T} [GeV]");
  else if(add=="eta")
    h->GetXaxis()->SetTitle("#eta");
  else 
    h->GetXaxis()->SetTitle("#delta#phi_{l,met}");

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
  h->Draw();
  sfc->Draw("P,e1,same");
  sf->Draw("P,e1,same");
  mc->Draw("P,e1,same");
  d->Draw("P,e1,same");
  leg->Draw("same");

  //different markers
  // dot data
  //stuff for other
}


void miniscript(TString outdir){

  // TString dir="/afs/naf.desy.de/user/k/kieseler/public/forCarmen/latestgreatesttriggerplots3/";
  TString rootsdir="temp_triggerplots/";

  TCanvas * c = new TCanvas("def","def");
  c->Clear();
  c->cd();
  makeplot(rootsdir+"triggerSummaryEE.root", "pt");
  c->Print(outdir+"corr_eept.pdf");
  c->Clear();
  makeplot(rootsdir+"triggerSummaryEE.root", "eta");
  c->Print(outdir+"corr_eeeta.pdf");
  c->Clear();
  makeplot(rootsdir+"triggerSummaryEE.root", "dphi");
  c->Print(outdir+"corr_eedphi.pdf");
  c->Clear();
  makeplot(rootsdir+"triggerSummaryMUMU.root", "pt");
  c->Print(outdir+"corr_mumupt.pdf");
  c->Clear();
  makeplot(rootsdir+"triggerSummaryMUMU.root", "eta");
  c->Print(outdir+"corr_mumueta.pdf");
  c->Clear();
  makeplot(rootsdir+"triggerSummaryMUMU.root", "dphi");
  c->Print(outdir+"corr_mumudphi.pdf");
  c->Clear();
  makeplot(rootsdir+"triggerSummaryEMU.root", "pt");
  c->Print(outdir+"corr_emupt.pdf");
  c->Clear();
  makeplot(rootsdir+"triggerSummaryEMU.root", "eta");
  c->Print(outdir+"corr_emueta.pdf");
  c->Clear();
  makeplot(rootsdir+"triggerSummaryEMU.root", "dphi");
  c->Print(outdir+"corr_emudphi.pdf");
  //c->Clear();


}

#endif
