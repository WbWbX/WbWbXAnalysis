#include "TH1D.h"
#include "TH2D.h"
#include "TtZAnalysis/Tools/interface/effTriple.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TString.h"
#include "TPad.h"
#include "TPaveText.h"

#ifndef histoStyle_h
#define histoStyle_h

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


void plotAll(std::vector<ztop::histWrapper> & hvec, TString addlabel="", TString dir = "./", bool setStyle=false){

  //num,den,eff
  size_t i=0;

  bool ignorewarnings=setStyle;

  if(dir != "./")
    system(("mkdir -p "+dir).Data());

 

  TCanvas * c=new TCanvas();
  c->SetBatch();
  TLegend * leg=new TLegend(0.35,0.15,0.70,0.42);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);


  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);

  gPad->SetLeftMargin(0.15);
  gPad->SetBottomMargin(0.15);

  gStyle->SetPaintTextFormat("5.3f");

  TPaveText *label = new TPaveText();

  label -> SetX1NDC(gStyle->GetPadLeftMargin());
  label -> SetY1NDC(1.0-gStyle->GetPadTopMargin());
  label -> SetX2NDC(1.0-gStyle->GetPadRightMargin());
  label -> SetY2NDC(1.0);
  label -> SetTextFont(42);
  label -> AddText(Form(addlabel));

  label->SetFillStyle(0);
  label->SetBorderSize(0);
  label->SetTextAlign(22);
  //  label->Draw("same"); 

  while(i<hvec.size()){
    c->Clear();
    leg->Clear();
    if(hvec.at(i).isTH1D()){ //do 1D plotting
      TH1D * h=&(hvec.at(i).getTH1D());
      leg->AddEntry(h,"data efficiency"  ,"pe");
      h->GetYaxis()->SetTitle("#epsilon,SF");
      h->Draw("e1");
      i++;
      h=&(hvec.at(i).getTH1D());
      leg->AddEntry(h,"MC efficiency"  ,"pe");
      h->Draw("e1,same");
      i++;
      h=&(hvec.at(i).getTH1D());
      leg->AddEntry(h,"scale factor"  ,"pe");
      h->Draw("e1,same");

      TString canvasname=hvec.at(i).getName();
      canvasname.ReplaceAll("_eff","");
      c->SetName(canvasname);
      c->SetTitle(canvasname);

      leg->Draw("same");
      label->Draw("same"); 

      c->Write();
      c->Print(dir+canvasname+".pdf");

      i++; //next
    }
    else{                    //do 2d plotting
      size_t j=i;
      while(j<i+3){
	c->Clear();
	TH2D * h=&(hvec.at(j).getTH2D()); 
	TString canvasname=hvec.at(j).getName();
	c->SetName(canvasname);
	c->SetTitle(canvasname);
	h->Draw("colz,text,e"); 
	label->Draw("same"); 
	c->Write();
	c->Print(dir+canvasname+".pdf");
	j++;
      }
      i+=3; //next
    }
  }

  delete c;
  delete label;
}




#endif