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
#include <iostream>
#include "TH2D.h"

void make2dplots(TString inputfile, TString plot, TString channel){

  TFile *f = new TFile(inputfile);

  std::vector<TString> plots; plots << plot+"_eff" << plot+"_effMC" << "scalefactor_"+plot;
  //std::vector<TString> plots;plots<< "scalefactor "+plot;

  TCanvas *c = new TCanvas();

  if(gStyle)
    gStyle->SetPaintTextFormat("5.3f");
  else
    std::cout << "gStyle not available" <<std::endl;

  for(unsigned int i=0;i<plots.size();i++){
    std::cout << "checking " << plots.at(i) << std::endl;
    if(!(f->Get(plots.at(i)))){
      std::cout << "plot " << plots.at(i) << " not found" << std::endl;
      continue;
    }
    TH2D * h = (TH2D*)f->Get(plots.at(i));
   
    gStyle->SetOptTitle(0);
    gStyle->SetOptStat(0);
   
    h->GetYaxis()->SetTitleSize(0.06);
    h->GetXaxis()->SetTitleSize(0.05);
    h->GetYaxis()->SetLabelSize(0.05);
    h->GetXaxis()->SetLabelSize(0.05);
    h->SetMarkerSize(2.0);

   
    if(((TString)f->GetName()).Contains("mumu")){
      h->GetXaxis()->SetTitle("#eta_{#mu_{1}}");
      h->GetYaxis()->SetTitle("#eta_{#mu_{2}}");
    }
    else if(((TString)f->GetName()).Contains("emu")){
      h->GetXaxis()->SetTitle("#eta_{e}");
      h->GetYaxis()->SetTitle("#eta_{#mu}");
    }
    else if(((TString)f->GetName()).Contains("ee")){
      h->GetXaxis()->SetTitle("#eta_{e_{1}}");
      h->GetYaxis()->SetTitle("#eta_{e_{2}}");
    }
   
  
   
    //  h->Draw("colz,text,e");
    gPad->SetLeftMargin(0.15);
    gPad->SetBottomMargin(0.15);
   
   
    h->Draw("colz,text,e");
    c->Print(channel+ "_"+ plots.at(i)+".eps");
    c->Print(channel+ "_"+ plots.at(i)+".pdf");
  }
  delete c;
}

void makeplot(TString inputfile, TString plot, TString channel){

    TLegend * leg =  new TLegend(0.35,0.15,0.70,0.42);
    leg->Clear();
    leg->SetFillStyle(0);
    leg->SetBorderSize(0);


    gStyle->SetOptTitle(0);
    gStyle->SetOptStat(0);

    TFile *f = new TFile(inputfile);


 std::cout << "searching for plot " << plot << " " << channel << std::endl;
      

    if(f->Get(plot) && ((TString)f->Get(plot)->ClassName()).Contains("TH2D")){
      make2dplots(inputfile, plot, channel);
    }
    if(f->Get(plot +"_eff") && (((TString)f->Get(plot +"_eff")->ClassName()).Contains("TH2D"))){
      make2dplots(inputfile, plot, channel);
    }
    if(f->Get("scalefactor_"+plot) && (((TString)f->Get("scalefactor_"+plot)->ClassName()).Contains("TH2D"))){
      make2dplots(inputfile, plot, channel);
    }
    else{
      if(!(f->Get(plot+"_eff")) || !(f->Get(plot+"_eff")))
	 return;

      std::cout << "making 1D plot for " << plot << " " << channel << std::endl;
      
      TCanvas * c = new TCanvas();
      c->Clear();
      c->cd();

      TH1D *h = (TH1D*)f->Get("axis_"+plot);

      TGraphAsymmErrors *sfc=(TGraphAsymmErrors*) f->Get("scalefactor_"+plot+"_incl_corrErr");
      TGraphAsymmErrors *sf=(TGraphAsymmErrors*) f->Get("scalefactor_"+plot);
      TGraphAsymmErrors *d=(TGraphAsymmErrors*) f->Get(plot+"_eff");
      TGraphAsymmErrors *mc=(TGraphAsymmErrors*) f->Get(plot+"_effMC");

      h->SetAxisRange(0.5,1.1,"Y");
      // set style
      gStyle->SetOptTitle(0);
      gStyle->SetOptStat(0);

      sfc->SetLineColor(8);
      sf ->SetLineColor(8);
      mc->SetLineColor(kRed);
      h->GetYaxis()->SetTitle("#epsilon,SF");
      if(plot=="pt") 
	h->GetXaxis()->SetTitle("p_{T} [GeV]");
      else if(plot=="eta")
	h->GetXaxis()->SetTitle("#eta");
      else if(plot=="dphi" || plot=="dphi2"){
	h->GetXaxis()->SetTitle("| #delta#phi_{l,met} |");
      }
      else if(plot=="vmulti"){
	h->GetXaxis()->SetTitle("n_{vtx}");
	//  h->SetAxisRange(0.4,1.1,"Y");
	// h->SetAxisRange(0,30,"X");
      }
      else if(plot=="jetmulti"){
	h->GetXaxis()->SetTitle("n_{jet}");
      }
      else if (plot=="drlep"){
	h->GetXaxis()->SetTitle("#DeltaR(l_{1},l_{2})");
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

      c->Print(channel+"_"+plot+".pdf");
      c->Print(channel+"_"+plot+".eps");
      delete c;

    }

    
    //different markers
    // dot data
    //stuff for other
  }


  void miniscript(){

    TString outdir="";
    // TString dir="/afs/naf.desy.de/user/k/kieseler/public/forCarmen/latestgreatesttriggerplots3/";
    TString rootsdir=outdir;


    std::vector<TString> channels, plots;
    channels << "ee" << "mumu" << "emu" ;
    plots << "eta2d" << "eta2d_with_syst" << "pt" << "eta" << "dphi" << "dphi2" << "vmulti" << "jetmulti" << "drlep";

 

    for(unsigned int i=0;i<channels.size();i++){
      for(unsigned int j=0;j<plots.size();j++){
	std::cout << "making plot for " << "triggerSummary_"+channels.at(i)+".root" << " plot "  << plots.at(j) <<endl;
	makeplot(rootsdir+"triggerSummary_"+channels.at(i)+".root", plots.at(j),channels.at(i));
      }
    }
  


  }

#endif
