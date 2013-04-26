#ifndef cefftriple_h
#define cefftriple_h


/// put possibility for TH2Ds and containers here
// avoid dep to miscUtils.. aaaand on the .contrary..

//#include "container.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TString.h"
#include <iostream>
#include <cstdlib>
#include <cmath>

namespace ztop{


  class histWrapper{
  public:
    histWrapper(){}
    histWrapper(const TH1D & h, TString name=""){setHisto(h);setName(name);dividebinomial_=true;ptf_="";}
    histWrapper(const TH2D & h, TString name=""){setHisto(h);setName(name);dividebinomial_=true;ptf_="";}
    ~histWrapper(){}

    void setHisto(const TH2D & h2d){th2d_=h2d;th2d_.Sumw2();is2d_=true;}
    void setHisto(const TH1D & h){th1d_=h;th1d_.Sumw2();is2d_=false;}
    void setName(TString name){
      name_=name;
      if(isTH1D()){
	th1d_.SetName(name_);
	th1d_.SetTitle(name_);
      }
      else{
	th2d_.SetName(name_);
	th2d_.SetTitle(name_);
      }
    }
    void setDivideBinomial(bool div){dividebinomial_=div;}

    bool isTH1D(){return !is2d_;}
    bool isTH2D(){return is2d_;}

    TH2D & getTH2D()  {return th2d_;}
    TH1D & getTH1D()  {return th1d_;}
    TString getName(){return name_;}

    void setFormatInfo(TString ptf){ptf_=ptf;}
    TString getFormatInfo(){return ptf_;}

    void fill(double val, double val2weight=1, double weight=1){
      if(isTH1D())
	th1d_.Fill(val, val2weight);
      else
	th2d_.Fill(val, val2weight, weight);
    }

    void addRelError(double err){
      if(isTH1D())
	addRelError(th1d_,err);
      else
	addRelError(th2d_,err);
    }

    void write(){
     if(isTH1D())
       th1d_.Write();
     else
       th2d_.Write();
    }
    void draw(TString opt=""){
      if(isTH1D())
       th1d_.Draw(opt);
     else
       th2d_.Draw(opt);
    }

    histWrapper operator / (histWrapper & denominator){ // only binomial division
      histWrapper def;
      if(isTH1D() != denominator.isTH1D()){
	std::cout << "histWrapper operator /: tried to divide different histo types, returning empty histogram!" << std::endl;
	return def;
      }
      if(isTH1D() && denominator.isTH1D()){
	if(th1d_.GetNbinsX() != denominator.th1d_.GetNbinsX()){
	  std::cout << "histWrapper operator /: tried to divide different histo binnings, returning empty histogram!" << std::endl;
	  return def;
	}
	//do binomial
	TH1D h=divideTH1D(getTH1D(),denominator.getTH1D(), dividebinomial_);
	h.Sumw2();
	histWrapper out(h);
	out.setFormatInfo(getFormatInfo());
	return out;
      }
      else if(isTH2D() && denominator.isTH2D()){
	if(th2d_.GetNbinsX() != denominator.th2d_.GetNbinsX() || th2d_.GetNbinsY() != denominator.th2d_.GetNbinsY()){
	  std::cout << "histWrapper operator /: tried to divide different histo binnings, returning empty histogram!" << std::endl;
	  return def;
	}
	//do binomial
	TH2D h=divideTH2D(getTH2D(),denominator.getTH2D(), dividebinomial_);
	h.Sumw2();
	histWrapper out(h);
	out.setFormatInfo(getFormatInfo());
	return out;
      }
      return def;
    }

  private:
    TString name_;

    TH2D th2d_;
    TH1D th1d_;

    bool is2d_;

    bool dividebinomial_;

    TString ptf_;

    TH1D divideTH1D(const TH1D &h1, const TH1D &h2, bool binomial=true){ //! out = h1 / h2
      TH1D out=h1; 
      for(int binx=1;binx<=h1.GetNbinsX()+1;binx++){
	double cont=0;
	double err=0;
	if(h2.GetBinContent(binx) !=0){
	  cont=h1.GetBinContent(binx) / h2.GetBinContent(binx);
	  double err1=h1.GetBinError(binx)/h2.GetBinContent(binx);
	  double err2=cont/h2.GetBinContent(binx) * h2.GetBinError(binx);
	  if(binomial)
	    err=sqrt(cont*(1-cont)/ h1.GetBinContent(binx));
	  else
	    err=sqrt(err1*err1 + err2*err2);
	}
	out.SetBinContent(binx,cont);
	out.SetBinError(binx,err);
      }  
      return out;
    }


    TH2D divideTH2D(const TH2D &h1, const TH2D &h2, bool binomial=true){ //! out = h1 / h2
      TH2D out=h1; 
      for(int binx=1;binx<=h1.GetNbinsX()+1;binx++){
	for(int biny=1;biny<=h1.GetNbinsY()+1;biny++){
	  double cont=0;
	  double err=0;
	  if(h2.GetBinContent(binx,biny) !=0){
	    cont=h1.GetBinContent(binx,biny) / h2.GetBinContent(binx,biny);
	    double err1=h1.GetBinError(binx,biny)/h2.GetBinContent(binx,biny);
	    double err2=cont/h2.GetBinContent(binx,biny) * h2.GetBinError(binx,biny);
	    if(binomial)
	      err=sqrt(cont*(1-cont)/ h1.GetBinContent(binx,biny));
	    else
	      err=sqrt(err1*err1 + err2*err2);
	  }
	  out.SetBinContent(binx,biny,cont);
	  out.SetBinError(binx,biny,err);
	}
      }
      return out;
    }

    void addRelError(TH2D &h, double err){
      for(int binx=1;binx<=h.GetNbinsX()+1;binx++){
	for(int biny=1;biny<=h.GetNbinsY()+1;biny++){
	  double add=h.GetBinContent(binx,biny) * err;
	  double newerr=sqrt(pow(h.GetBinError(binx,biny),2) + pow(add,2));
	  h.SetBinError(binx,biny,newerr);
	}
      }
    }
    void addRelError(TH1D &h, double err){
      for(int binx=1;binx<=h.GetNbinsX()+1;binx++){
	double add=h.GetBinContent(binx) * err;
	double newerr=sqrt(pow(h.GetBinError(binx),2) + pow(add,2));
	h.SetBinError(binx,newerr);  
      }
    }


  };


  ///////

  class effTriple{
  public:
    effTriple();
    effTriple(std::vector<float> binsx,  
	      TString name="",
	      TString xaxisname="",
	      TString yaxisname="",  
	      TString formatinfo="",
	      bool mergeufof=false, 
	      size_t size=3); //! bins, name, xaxisname, yaxisname, mergeUFOF=false - for 1D histos
    effTriple(std::vector<float> binsx,  
	      std::vector<float> binsy, 
	      TString name="",
	      TString xaxisname="",
	      TString yaxisname="", 
	      TString formatinfo="",
	      bool mergeufof=false, 
	      size_t size=3); //! binsx, binsy, name, xaxisname, yaxisname, mergeUFOF=false
    
    ~effTriple(); //! delete pointers in efflist

    void fillNum(double value, double val2weight=1, double weight=1);
    void fillDen(double value, double val2weight=1, double weight=1);

    
    histWrapper getEff(){makeEff(); return hists_.at(2);}
    histWrapper getNum(){return hists_.at(0);}
    histWrapper getDen(){return hists_.at(1);}

    TString getName(){return name_;}

    TString getFormatInfo(){return hists_.at(0).getFormatInfo();}

    static std::vector<effTriple *> effTriple_list;
    static bool makelist;

  protected:
    //see where to put these things. first everything here
    // map with histos etc. possibility to add more than 3 (for corr or something) and to define operations(?) to be implemented later

    TString name_;
    std::vector<histWrapper> hists_;
   

  private:
    //les see what comes here
    void addToList();
    void check1d();
    void check2d();
    void makeEff();
    void setStyle2d(TH2D &){}; //can do
    void setStyle(TH1D &){};   //can do

  };

  //Some helper functions:
  /*
  vector? histWrapper makeSFs(vector<effTriple> num, vector<effTriple> den){

    // for each check whether 2d or 1D etc
    //make SF and 

  }


  

//afterwards make functions for triggeranalyzer/triggeranalyzer to produce the set of SF plots

  */

}
#endif

