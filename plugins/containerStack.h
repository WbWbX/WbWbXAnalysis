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
#include "TLatex.h"
#include "TGaxis.h"

namespace top{

  class container1DStack{
    
  public:
    container1DStack(){};
    container1DStack(TString name) : name_(name), dataleg_("data") {}

    void push_back(top::container1D, TString, int, double);
    
    void setDataLegend(TString leg="data"){dataleg_=leg;}
    void mergeSameLegends();
    top::container1D getContribution(TString);   //! does not ignore data
    top::container1D getContributionsBut(TString);  //!does not ignore data
    top::container1D getContributionsBut(std::vector<TString>);  //!does not ignore data
    TString getName(){return name_;}
    unsigned int size(){return colors_.size();}
    TString getLegend(unsigned int i){return legends_[i];}
    int & getColor (unsigned int i){return colors_[i];};
    double & getNorm  (unsigned int i){return norms_[i];}
    container1D & getContainer(unsigned int i){return containers_[i];}
    container1D & getContainer(TString);
    container1D getFullMCContainer();           //! gets the sum of all MC containers (normalized with their stored norm) including error handling
    void multiplyNorm(TString , double);
    void addGlobalRelMCError(TString,double);   //! adds a global systematic variation to the systematics stored (e.g. lumi)
    void addMCErrorStack(TString,container1DStack,bool ignoreMCStat=true);  //! calls container1D::addErrorContainer for each same named member container
    void removeError(TString);

    void clear(){containers_.clear();legends_.clear();colors_.clear();norms_.clear();}
    
    void setName(TString name){name_=name;}
    
    THStack * makeTHStack(TString stackname = "");
    TLegend * makeTLegend();
    void drawControlPlot(TString name="", bool drawxaxislabels=true, double resizelabels=1); // the extra axis is in both... sorry for that!
    TGraphAsymmErrors * makeMCErrors();
    void drawRatioPlot(TString name="",double resizelabels=1);

    TCanvas * makeTCanvas(bool drawratioplot=true);


    //define operators (propagate from containers) for easy handling

    container1DStack operator + ( container1DStack );
    container1DStack operator - ( container1DStack );
    container1DStack operator / ( container1DStack );
    container1DStack operator * ( container1DStack );
    container1DStack operator * (double);
    container1DStack operator * (float);
    container1DStack operator * (int);
    
    
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

 top::container1D container1DStack::getContribution(TString contr){
   top::container1D out;
   if(containers_.size() > 0){
     out=containers_[0];
     out.clear();
     int i=0;
     for(std::vector<TString>::iterator name=legends_.begin();name<legends_.end();++name){
       if(contr == *name){
	 out=out + containers_[i] * norms_[i] ;
	 break;
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

  top::container1D container1DStack::getContributionsBut(std::vector<TString> contr){
    top::container1D out;
    if(containers_.size() > 0){
      out=containers_[0];
      out.clear();
      for(unsigned int i=0;i<legends_.size();i++){
	bool get=true;
	for(std::vector<TString>::iterator name=contr.begin();name<contr.end();++name){
	  if(*name == legends_[i]){
	    get=false;
	    break;
	  }
	}
	if(get) out = out+ containers_[i] * norms_[i];
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

  void container1DStack::addGlobalRelMCError(TString sysname,double error){
    for(unsigned int i=0;i<containers_.size();i++){
      if(legends_[i]!=dataleg_) containers_[i].addGlobalRelError(sysname,error);
    }
  }
  void container1DStack::addMCErrorStack(TString sysname,container1DStack errorstack, bool ignoreMCStat){
    for(unsigned int i=0; i<size();i++){
      for(unsigned int j=i;j<errorstack.size();j++){
	errorstack.containers_[j] = errorstack.containers_[j] * errorstack.norms_[j]; //normalize (in case there is any remultiplication done or something)
	errorstack.norms_[j]=1;
	if(legends_[i] == errorstack.legends_[j] && legends_[i]!=dataleg_){
	  containers_[i] = containers_[i] * norms_[i];
	  norms_[i]=1;
	  containers_[i].addErrorContainer(sysname,errorstack.containers_[j],ignoreMCStat);
	}
      }
    }
  }
  void container1DStack::removeError(TString sysname){
    for(unsigned int i=0; i<size();i++){
      containers_[i].removeError(sysname);
    }
  }
  top::container1D & container1DStack::getContainer(TString name){
    bool found=false;
    for(unsigned int i=0;i<containers_.size();i++){
      if(containers_[i].getName() == name){
	containers_[i] = containers_[i] * norms_[i];
	norms_[i] = 1.;
	found=true;
	return containers_[i];
      }
    }
    if(!found) std::cout << "container1DStack::getContainer: container with name " << name << " not found, returning first container!" << std::endl;
    return containers_[0];;
  }
  top::container1D container1DStack::getFullMCContainer(){
    container1D out=containers_[0];
    out.clear();
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
    TLegend *leg = new TLegend(0.55,0.55,0.95,0.90);
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
    bool gotdentry=false;
    bool gotuf=false;
    bool gotof=false;
    for(unsigned int i=0;i<size();i++){ // get datalegend and check if underflow or overflow in any container
      if(getLegend(i) == dataleg_){
        dataentry=i;
	gotdentry=true;
	if(gotof && gotuf) break;
      }
      if(containers_[i].getOverflow() < -0.9){
	gotof=true;
	if(gotdentry && gotuf) break;
      }
      if(containers_[i].getUnderflow() < -0.9){
	gotuf=true;
	if(gotdentry && gotof) break;
      }
    }
    float multiplier=1;
    double ylow,yhigh,xlow,xhigh;
    if(gPad){
      gPad->GetPadPar(xlow,ylow,xhigh,yhigh);
      multiplier = (float)1/(yhigh-ylow);
    }
    containers_[dataentry].setLabelSize(resizelabels * multiplier);
    TGraphAsymmErrors * g = containers_[dataentry].getTGraph(name);
    TH1D * h =containers_[dataentry].getTH1D(name+"_h"); // needed to be able to set log scale etc.

    float xmax=containers_[dataentry].getXMax();
    float xmin=containers_[dataentry].getXMin();
    h->Draw("AXIS");
    if(!drawxaxislabels){
      h->GetXaxis()->SetLabelSize(0);
    }
    else {
      if(gPad){
	gPad->SetLeftMargin(0.15);
	gPad->SetBottomMargin(0.15);
      }
    }
    g->Draw("P");
    makeTHStack(name)->Draw("same");
    makeMCErrors()->Draw("2");
    makeTLegend()->Draw("same");
    g->Draw("e1,P,same");
    if(gPad) gPad->RedrawAxis();

    if(gPad && drawxaxislabels && containers_[dataentry].getNBins() >0){
      float yrange=fabs(gPad->GetUymax()-gPad->GetUymin());
      float xrange = fabs(xmax-xmin);
      if(gotuf){
	TLatex * la = new TLatex(containers_[dataentry].getBinCenter(1)-xrange*0.06,gPad->GetUymin()-0.15*yrange,"#leq");
	la->Draw("same");
      }
      if(gotof){
	TLatex * la2 = new TLatex(containers_[dataentry].getBinCenter(containers_[dataentry].getNBins())+xrange*0.06,gPad->GetUymin()-0.15*yrange,"#leq");
	la2->Draw("same");
      }
    }

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
    bool gotdentry=false;
    bool gotuf=false;
    bool gotof=false;
    for(unsigned int i=0;i<size();i++){ // get datalegend and check if underflow or overflow in any container
      if(getLegend(i) == dataleg_){
        dataentry=i;
	gotdentry=true;
	if(gotof && gotuf) break;
      }
      if(containers_[i].getOverflow() < -0.9){
	gotof=true;
	if(gotdentry && gotuf) break;
      }
      if(containers_[i].getUnderflow() < -0.9){
	gotuf=true;
	if(gotdentry && gotof) break;
      }
    }
    container1D data = containers_[dataentry];
    data.setShowWarnings(false);   
    container1D mc = getFullMCContainer();
    mc.setShowWarnings(false);   
    data.setDivideBinomial(false);
    mc.setDivideBinomial(false);
    container1D ratio=data;
    ratio.clear(); 
    ratio.setShowWarnings(false);   
    container1D relmcerr=data;
    relmcerr.clear();
    container1D mccopy=mc;
    for(int i=0; i<ratio.getNBins()+1; i++){ //includes underflow right now, doesn't matter
      relmcerr.setBinContent(i,1.);
      relmcerr.setBinErrorUp(i,mc.getBinErrorUp(i) / mc.getBinContent(i));
      relmcerr.setBinErrorDown(i,mc.getBinErrorDown(i) / mc.getBinContent(i));
      //set mc error to zero for the ratio plot
      mccopy.setBinErrorUp(i,0);
      mccopy.setBinErrorDown(i,0);
    }
    ratio = data / mccopy;
    float multiplier=1;
    double ylow,yhigh,xlow,xhigh;
    if(gPad){
      gPad->GetPadPar(xlow,ylow,xhigh,yhigh);
      multiplier = (float)1/(yhigh-ylow);
    }
    //   multiplier = multiplier * resizelabels;
    ratio.setLabelSize(multiplier * resizelabels);

    TGraphAsymmErrors * gratio = ratio.getTGraph(name,true,false); //don't divide by binwidth
    // rescale axis titles etc.
    TH1D * h = ratio.getTH1D(name+"_h_r",false); //no bw div
    h->GetYaxis()->SetTitle("data/MC");
    h->GetYaxis()->SetRangeUser(0.5,1.5);
    h->GetYaxis()->SetNdivisions(505);
    h->GetXaxis()->SetTickLength(0.03 * multiplier);
    h->Draw("AXIS");
    gratio->Draw("P");
    TGraphAsymmErrors * gmcerr = relmcerr.getTGraph(name+"_relerr",false,false);
    gmcerr->SetFillStyle(3002);
    gmcerr->Draw("2,same");
    TLine * l = new TLine(mc.getXMin(),1,mc.getXMax(),1);
    l->Draw("same");
    float xmax=containers_[dataentry].getXMax();
    float xmin=containers_[dataentry].getXMin();
    if(gPad && containers_[dataentry].getNBins() >0){
      float yrange=fabs(gPad->GetUymax()-gPad->GetUymin());
      float xrange = fabs(xmax-xmin);
      if(gotuf){
	TLatex * la = new TLatex(containers_[dataentry].getBinCenter(1)-xrange*0.06,gPad->GetUymin()-0.15*yrange,"#leq");
	la->Draw("same");
      }
      if(gotof){
	TLatex * la2 = new TLatex(containers_[dataentry].getBinCenter(containers_[dataentry].getNBins())+xrange*0.06,gPad->GetUymin()-0.15*yrange,"#leq");
	la2->Draw("same");
      }
    }
  }

  TCanvas * container1DStack::makeTCanvas(bool drawratioplot){
    TCanvas * c = new TCanvas(name_+"_c",name_+"_c");
    TH1D * htemp=new TH1D("sometemphisto"+name_,"sometemphisto"+name_,2,0,1); //creates all gPad stuff etc and prevents seg vio, which happens in some cases; weird
    htemp->Draw();
    if(!drawratioplot){
      c->cd();
      drawControlPlot();
    }
    else{
      double labelresize=1.;
      double divideat=0.3;
      c->Divide(1,2);
      c->SetFrameBorderSize(0);
      c->cd(1)->SetLeftMargin(0.15);
      c->cd(1)->SetBottomMargin(0.03);
      c->cd(1)->SetPad(0,divideat,1,1);
      drawControlPlot(name_,false,labelresize);
      c->cd(2)->SetBottomMargin(0.4 * 0.3/divideat);
      c->cd(2)->SetLeftMargin(0.15);
      c->cd(2)->SetTopMargin(0);
      c->cd(2)->SetPad(0,0,1,divideat);
      gStyle->SetOptTitle(0);
      drawRatioPlot("",labelresize);
      c->cd(1)->RedrawAxis();
    }
    if(gStyle){
       gStyle->SetOptTitle(0);
       gStyle->SetOptStat(0);
    }
    delete htemp;
    return c;
  }

  //just perform functions on the containers with same names
  top::container1DStack container1DStack::operator + (container1DStack stack){
    for(unsigned int i=0;i<containers_.size();i++){
      for(unsigned int j=0;j<stack.containers_.size();j++){
	if(containers_[i].getName() == stack.containers_[j].getName()){
	  stack.containers_[j] = containers_[i] + stack.containers_[j];
	}
      }
    }
    return stack;
  }
  top::container1DStack container1DStack::operator - (container1DStack stack){
    for(unsigned int i=0;i<containers_.size();i++){
      for(unsigned int j=0;j<stack.containers_.size();j++){
	if(containers_[i].getName() == stack.containers_[j].getName()){
	  stack.containers_[j] = containers_[i] - stack.containers_[j];
	}
      }
    }
    return stack;
  }
  top::container1DStack container1DStack::operator / (container1DStack  stack){
    for(unsigned int i=0;i<containers_.size();i++){
      for(unsigned int j=0;j<stack.containers_.size();j++){
	if(containers_[i].getName() == stack.containers_[j].getName()){
	  stack.containers_[j] = containers_[i] / stack.containers_[j];
	}
      }
    }
    return stack;
  }
  top::container1DStack container1DStack::operator * (container1DStack  stack){
    for(unsigned int i=0;i<containers_.size();i++){
      for(unsigned int j=0;j<stack.containers_.size();j++){
	if(containers_[i].getName() == stack.containers_[j].getName()){
	  stack.containers_[j] = containers_[i] * stack.containers_[j];
	}
      }
    }
    return stack;
  }
  top::container1DStack container1DStack::operator * (double scalar){
    top::container1DStack out=*this;
    for(unsigned int i=0;i<containers_.size();i++) containers_[i] = containers_[i] * scalar;
    return out;
  }
  top::container1DStack container1DStack::operator * (float scalar){
    return *this * (double)scalar;
  }
  top::container1DStack container1DStack::operator * (int scalar){
    return *this * (double)scalar;
  }
    
  

}
#endif
