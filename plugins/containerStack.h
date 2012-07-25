#ifndef containerStack_h
#define containerStack_h

#include "container.h"
#include <iostream>
#include "TString.h"
#include <vector>
#include "miscUtils.h"
#include "THStack.h"
#include "TLegend.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLine.h"
#include "TStyle.h"

namespace top{

  


  class container1DStack{
    
  public:
    container1DStack(){};
    container1DStack(TString name) : name_(name), dataleg_("data") {}

    void push_back(top::container1D, TString, int, double);
    
    void setDataLegend(TString leg="data"){dataleg_=leg;}
    void mergeSameLegends();
    top::container1D getContribution(TString,bool stopAfterfound=false);
    top::container1D getContributionsBut(TString);
    TString getName(){return name_;}
    unsigned int size(){return colors_.size();}
    TString getLegend(unsigned int i){return legends_[i];}
    int & getColor (unsigned int i){return colors_[i];};
    double & getNorm  (unsigned int i){return norms_[i];}
    container1D & getContainer(unsigned int i){return containers_[i];}
    container1D getFullMCContainer();        //! gets the sum of all MC containers (normalized with their stored norm) including error handling
    void multiplyNorm(TString , double);
    void addGlobalRelMCError(double);
    void addMCErrorStack(container1DStack,bool ignoreMCStat=true);  //! considers deviation to "this" container as error, which is added in squares; 
                                                                    //! if ignoreMCStat=false, the MC stat error of the error container is added in addition 
                                                                    //! (in squares) and a check is performed whether the mc stat. is sufficient to justify variation
                                                                    //! errors are only added if legends are the same

    void clear(){containers_.clear();legends_.clear();colors_.clear();norms_.clear();}
    
    void setName(TString name){name_=name;}
    
    THStack * makeTHStack(TString stackname = "");
    TLegend * makeTLegend();
    void drawControlPlot(TString name="", bool drawxaxislabels=true, double resizelabels=1);
    TGraphAsymmErrors * makeMCErrors();
    void drawRatioPlot(TString name="",double resizelabels=1);

    TCanvas * makeTCanvas(bool drawratioplot=true);
    
    
  private:
    TString name_;
    std::vector<top::container1D> containers_;
    std::vector<TString> legends_;
    std::vector<int> colors_;
    std::vector<double> norms_;
    TString dataleg_;
    
};

 void container1DStack::push_back(top::container1D cont, TString legend, int color, double norm){
   bool wasthere=false;
   for(unsigned int i=0;i<legends_.size();i++){
     if(legend == legends_[i]){
       containers_[i] = containers_[i] * norms_[i] + cont * norm;
       norms_[i]=1;
       wasthere=true;
       break;
     }
   }
   if(!wasthere){
     containers_.push_back(cont);
     legends_.push_back(legend);
     colors_.push_back(color);
     norms_.push_back(norm);
   }
 }

 void container1DStack::mergeSameLegends(){
   //redundant
 }

 top::container1D container1DStack::getContribution(TString contr, bool stopAfterfound){
   top::container1D out;
   if(containers_.size() > 0){
     out=containers_[0];
     out.copyNames(containers_[0]);
     out.clear();
     int i=0;
     for(std::vector<TString>::iterator name=legends_.begin();name<legends_.end();++name){
       if(contr == *name){
	 out=out + containers_[i] * norms_[i] ;
	 if(stopAfterfound){
	   break;
	 }
       }
       i++;
     }
   }
   return out;
 }
  top::container1D container1DStack::getContributionsBut(TString contr){
   top::container1D out;
   if(containers_.size() > 0){
     out=containers_[0];
     out.clear();
     int i=0;
     for(std::vector<TString>::iterator name=legends_.begin();name<legends_.end();++name){
       if(contr != *name) out=out+containers_[i] * norms_[i];
     i++;
     }
   }
   return out;
 }
  
  void container1DStack::multiplyNorm(TString legendentry, double multi){
    int i=0;
    for(std::vector<TString>::iterator name=legends_.begin();name<legends_.end();++name){
      if(legendentry == *name){
	norms_[i] = norms_[i] * multi;
      }
      i++;
    }
  }

  void container1DStack::addGlobalRelMCError(double error){
    for(unsigned int i=0;i<containers_.size();i++){
      if(legends_[i]!=dataleg_) containers_[i].addGlobalRelError(error);
    }
  }
  void container1DStack::addMCErrorStack(container1DStack errorstack, bool ignoreMCStat){
    for(unsigned int i=0; i<size();i++){
      for(unsigned int j=i;j<errorstack.size();j++){
	if(legends_[i] == errorstack.legends_[j] && legends_[i]!=dataleg_){
	  containers_[i].addErrorContainer(errorstack.containers_[j],ignoreMCStat);
	}
      }
    }
  }
  top::container1D container1DStack::getFullMCContainer(){
    container1D out=containers_[0];
    out.clear();
    out.copyNames(containers_[0]);
    for(unsigned int i=0;i<containers_.size();i++){
      if(legends_[i] != dataleg_) out = out + containers_[i]*norms_[i];
    }
    return out;
  }

  THStack * container1DStack::makeTHStack(TString stackname){
    if(stackname == "") stackname = name_+"_s";
    THStack *tstack = new THStack(stackname,stackname);
    for(unsigned int i=0;i<size();i++){
      if(getLegend(i) != dataleg_){
	container1D tempcont = getContainer(i);
	tempcont.copyNames(getContainer(i));
	tempcont = tempcont * getNorm(i);
	TH1D * h = (TH1D*)tempcont.getTH1D(getLegend(i)+" "+getName()+"_stack_h")->Clone();
	h->SetFillColor(getColor(i));
	for(int bin = 1 ; bin < h->GetNbinsX()+1;bin++){
	  h->SetBinError(bin,0);
	}
	tstack->Add(h);
      }
    }
    return  tstack;
  }
  TLegend * container1DStack::makeTLegend(){
    TLegend *leg = new TLegend(0.70,0.65,0.95,0.90);
    leg->Clear();
    leg->SetFillStyle(0);
    leg->SetBorderSize(0);
    for(unsigned int i=0;i<size();i++){
      container1D tempcont = getContainer(i);
      tempcont = tempcont * getNorm(i);
      TH1D * h = (TH1D*)tempcont.getTH1D(getLegend(i)+" "+getName()+"_leg")->Clone();
      h->SetFillColor(getColor(i));
      if(getLegend(i) != dataleg_) leg->AddEntry(h,getLegend(i),"f");
      else leg->AddEntry(h,getLegend(i),"ep");
    }
    return leg;
  }
  void container1DStack::drawControlPlot(TString name, bool drawxaxislabels, double resizelabels){
    if(name=="") name=name_;
    int dataentry=0;
    for(unsigned int i=0;i<size();i++){
      if(getLegend(i) == dataleg_){
        dataentry=i;
	break;
      }
    }
    containers_[dataentry].setLabelSize(resizelabels);
    TGraphAsymmErrors * g = containers_[dataentry].getTGraph(name);

    TH1D * h =containers_[dataentry].getTH1D(name+"_h"); // needed to be able to set log scale etc.
    h->Draw("AXIS");
    if(!drawxaxislabels){
      h->GetXaxis()->SetLabelSize(0);
    }
    else if(gPad){
      gPad->SetLeftMargin(0.15);
      gPad->SetBottomMargin(0.15);
    }
    g->Draw("P");
    makeTHStack(name)->Draw("same");
    makeMCErrors()->Draw("2");
    makeTLegend()->Draw("same");
    g->Draw("e1,P,same");
    if(gPad) gPad->RedrawAxis();
  }

  TGraphAsymmErrors * container1DStack::makeMCErrors(){
    TGraphAsymmErrors * g =getFullMCContainer().getTGraph();
    g->SetFillStyle(3002);
    return g;
  }

  void container1DStack::drawRatioPlot(TString name,double resizelabels){
    //prepare container
    if(name=="") name=name_;
    int dataentry=0;
    for(unsigned int i=0;i<size();i++){
      if(getLegend(i) == dataleg_){
        dataentry=i;
	break;
      }
    }
    container1D data = containers_[dataentry];
    data.copyNames(containers_[dataentry]);
    data.setShowWarnings(false);   
    container1D mc = getFullMCContainer();
    mc.copyNames(getFullMCContainer());
    mc.setShowWarnings(false);   
    data.setDivideBinomial(false);
    mc.setDivideBinomial(false);
    container1D ratio=data;
    ratio.copyNames(data);
    ratio.clear(); 
    ratio.setShowWarnings(false);   
    container1D relmcerr=data;
    relmcerr.copyNames(data);
    relmcerr.clear();
    for(int i=0; i<ratio.getNBins()+1; i++){ //includes underflow right now, doesn't matter
      relmcerr.setBinContent(i,1.);
      relmcerr.setBinErrorUp(i,mc.getBinErrorUp(i) / mc.getBinContent(i));
      relmcerr.setBinErrorDown(i,mc.getBinErrorDown(i) / mc.getBinContent(i));
      //set mc error to zero for the ratio plot
      mc.setBinErrorUp(i,0);
      mc.setBinErrorDown(i,0);
    }
    ratio = data / mc;
    float multiplier=1;
    double ylow,yhigh,xlow,xhigh;
    if(gPad){
      gPad->GetPadPar(xlow,ylow,xhigh,yhigh);
      multiplier = (float)(1-yhigh)/(yhigh-ylow);
    }
    multiplier = multiplier * resizelabels;
    ratio.setLabelSize(multiplier);

    TGraphAsymmErrors * gratio = ratio.getTGraph(name,true);
    // rescale axis titles etc.
    TH1D * h = ratio.getTH1D(name+"_h_r");
    h->GetYaxis()->SetTitle("data/MC");
    h->GetYaxis()->SetRangeUser(0.5,1.5);
    h->GetYaxis()->SetNdivisions(505);
    h->Draw("AXIS");
    gratio->Draw("P");
    TGraphAsymmErrors * gmcerr = relmcerr.getTGraph(name+"_relerr");
    gmcerr->SetFillStyle(3002);
    gmcerr->Draw("2,same");
    TLine * l = new TLine(mc.getXMin(),1,mc.getXMax(),1);
    l->Draw("same");

  }
  TCanvas * container1DStack::makeTCanvas(bool drawratioplot){
    TCanvas * c = new TCanvas(name_+"_c",name_+"_c");
    if(gStyle){
      gStyle->SetOptTitle(0);
      gStyle->SetOptStat(0);
    }
    if(!drawratioplot){
      c->cd();
      drawControlPlot();
    }
    else{
      double labelresize=1.33;
      double divideat=0.3;
      c->Divide(1,2);
      c->SetFrameBorderSize(0);
      c->cd(1)->SetLeftMargin(0.15);
      c->cd(1)->SetBottomMargin(0.03);
      c->cd(1)->SetPad(0,divideat,1,1);
      drawControlPlot(name_,false,labelresize);
      c->cd(2)->SetBottomMargin(0.4*labelresize/1.3 * 0.3/divideat);
      c->cd(2)->SetLeftMargin(0.15);
      c->cd(2)->SetTopMargin(0);
      c->cd(2)->SetPad(0,0,1,divideat);
      gStyle->SetOptTitle(0);
      drawRatioPlot("",labelresize);
      c->cd(1)->RedrawAxis();
      //   c->cd(0)->SetBottomMargin(0.3 * labelresize/1.3);
    }
    return c;
  }

}
#endif
