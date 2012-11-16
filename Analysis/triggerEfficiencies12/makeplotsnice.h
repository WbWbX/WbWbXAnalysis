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

  //different markers
  // dot data
  //stuff for other
}


void miniscript(TString outdir){

  // TString dir="/afs/naf.desy.de/user/k/kieseler/public/forCarmen/latestgreatesttriggerplots3/";
  TString rootsdir=outdir;

  TCanvas * c = new TCanvas("def","def");
  c->Clear();
  c->cd();
  makeplot(rootsdir+"triggerSummary_ee.root", "pt");
  c->Print(outdir+"corr_eept.pdf");
  c->Print(outdir+"corr_eept.eps");
  c->Clear();
  makeplot(rootsdir+"triggerSummary_ee.root", "eta");
  c->Print(outdir+"corr_eeeta.pdf");
  c->Print(outdir+"corr_eeeta.eps");
  c->Clear();
  makeplot(rootsdir+"triggerSummary_ee.root", "dphi");
  c->Print(outdir+"corr_eedphi.pdf");
  c->Print(outdir+"corr_eedphi.eps");
  c->Clear();
  makeplot(rootsdir+"triggerSummary_ee.root", "dphi2");
  c->Print(outdir+"corr_eedphi2.pdf");
  c->Print(outdir+"corr_eedphi2.eps");
  c->Clear();
  makeplot(rootsdir+"triggerSummary_ee.root", "vmulti");
  c->Print(outdir+"corr_eevmulti.pdf");
  c->Print(outdir+"corr_eevmulti.eps");
  c->Clear();

  makeplot(rootsdir+"triggerSummary_mumu.root", "pt");
  c->Print(outdir+"corr_mumupt.pdf");
  c->Print(outdir+"corr_mumupt.eps");
  c->Clear();
  makeplot(rootsdir+"triggerSummary_mumu.root", "eta");
  c->Print(outdir+"corr_mumueta.pdf");
  c->Print(outdir+"corr_mumueta.eps");
  c->Clear();
  makeplot(rootsdir+"triggerSummary_mumu.root", "dphi");
  c->Print(outdir+"corr_mumudphi.pdf");
  c->Print(outdir+"corr_mumudphi.eps");
  c->Clear();
  makeplot(rootsdir+"triggerSummary_mumu.root", "dphi2");
  c->Print(outdir+"corr_mumudphi2.pdf");
  c->Print(outdir+"corr_mumudphi2.eps");
  c->Clear();
  makeplot(rootsdir+"triggerSummary_mumu.root", "vmulti");
  c->Print(outdir+"corr_mumuvmulti.pdf");
  c->Print(outdir+"corr_mumuvmulti.eps");
  c->Clear();

  makeplot(rootsdir+"triggerSummary_emu.root", "pt");
  c->Print(outdir+"corr_emupt.pdf");
  c->Print(outdir+"corr_emupt.eps");
  c->Clear();
  makeplot(rootsdir+"triggerSummary_emu.root", "eta");
  c->Print(outdir+"corr_emueta.pdf");
  c->Print(outdir+"corr_emueta.eps");
  c->Clear();
  makeplot(rootsdir+"triggerSummary_emu.root", "dphi");
  c->Print(outdir+"corr_emudphi.pdf");
  c->Print(outdir+"corr_emudphi.eps");
  c->Clear();
  makeplot(rootsdir+"triggerSummary_emu.root", "dphi2");
  c->Print(outdir+"corr_emudphi2.pdf");
  c->Print(outdir+"corr_emudphi2.eps");
  c->Clear();
  makeplot(rootsdir+"triggerSummary_emu.root", "vmulti");
  c->Print(outdir+"corr_emuvmulti.pdf");
  c->Print(outdir+"corr_emuvmulti.eps");
  c->Clear();


}

#endif
