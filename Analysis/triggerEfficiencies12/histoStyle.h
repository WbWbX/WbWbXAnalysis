#include "TH1D.h"
#include "TH2D.h"
#include "TtZAnalysis/Tools/interface/effTriple.h"
#include "TStyle.h"

void setGStyle(){
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);

  gPad->SetLeftMargin(0.15);
  gPad->SetBottomMargin(0.15);

  gStyle->SetPaintTextFormat("5.3f");
}


void setCommon(TH2D &h){
  h.GetYaxis()->SetTitleSize(0.06);
  h.GetXaxis()->SetTitleSize(0.05);
  h.GetYaxis()->SetLabelSize(0.05);
  h.GetXaxis()->SetLabelSize(0.05);
  h.SetMarkerSize(2.0);
}

void setCommon(TH1D & h){
  h.SetAxisRange(0.5,1.1,"Y");
  h.GetYaxis()->SetTitleSize(0.06);
  h.GetXaxis()->SetTitleSize(0.05);
  h.GetYaxis()->SetLabelSize(0.05);
  h.GetXaxis()->SetLabelSize(0.05);
  h.SetMarkerSize(1);
  h.SetLineWidth(2);
}

void applyEfficiencyStyleMC(TH1D & h){
  setCommon(h);
  h.SetMarkerStyle(22);
  h.SetMarkerColor(kRed);
  h.SetLineColor(kRed);
}

void applyEfficiencyStyleData(TH1D & h){
  setCommon(h);
  h.SetMarkerStyle(20);
  h.SetMarkerColor(1);
  h.SetLineColor(1);
}

void applySFStyle(TH1D & h){
  setCommon(h);
  h.SetMarkerStyle(33);
  h.SetMarkerColor(8);
  h.SetLineColor(8);
}


void applyEfficiencyStyleMC(TH2D & h){
  setCommon(h);
}

void applyEfficiencyStyleData(TH2D & h){
  setCommon(h);
}

void applySFStyle(TH2D & h){
  setCommon(h);
}




////


void applyEfficiencyStyleMC(ztop::histWrapper & h){
  if(h.isTH1D())
    applyEfficiencyStyleMC(h.getTH1D());
  else
    applyEfficiencyStyleMC(h.getTH2D());
}

void applyEfficiencyStyleData(ztop::histWrapper & h){
 if(h.isTH1D())
    applyEfficiencyStyleData(h.getTH1D());
  else
    applyEfficiencyStyleData(h.getTH2D());
}

void applySFStyle(ztop::histWrapper & h){
 if(h.isTH1D())
    applySFStyle(h.getTH1D());
  else
    applySFStyle(h.getTH2D());
}
