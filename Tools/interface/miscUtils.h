#ifndef topMiscUtils_h
#define topMiscUtils_h

#include "TString.h"
#include <sstream>
#include <vector>
#include <math.h>
#include <iostream>
#include "TH1D.h"
#include "TH2D.h"

template<class t>
TString toTString(t in){
  std::ostringstream s;
  s << in;
  TString out=s.str();
  return out;
}

TH2D divideTH2DBinomial(TH2D &h1, TH2D &h2){ //! out = h1 / h2
  TH2D out=h1; 
  if(h1.GetNbinsX() != h2.GetNbinsX() || h1.GetNbinsY() != h2.GetNbinsY()){
    std::cout << "divideTH2DBinomial: Error! histograms must have same binning!" << std::endl;
    return h1;
  }
  for(int binx=1;binx<=h1.GetNbinsX()+1;binx++){
    for(int biny=1;biny<=h1.GetNbinsY()+1;biny++){
      double cont=0;
      double err=1;
      if(h2.GetBinContent(binx,biny) !=0){
	cont=h1.GetBinContent(binx,biny) / h2.GetBinContent(binx,biny);
	err=sqrt(cont*(1-cont)/ h1.GetBinContent(binx,biny));
      }
      out.SetBinContent(binx,biny,cont);
      out.SetBinError(binx,biny,err);
    }
  }
  return out;
}
TH2D divideTH2D(TH2D &h1, TH2D &h2){ 
TH2D out=h1; 
  if(h1.GetNbinsX() != h2.GetNbinsX() || h1.GetNbinsY() != h2.GetNbinsY()){
    std::cout << "divideTH2DBinomial: Error! histograms must have same binning!" << std::endl;
    return h1;
  }
  for(int binx=1;binx<=h1.GetNbinsX()+1;binx++){
    for(int biny=1;biny<=h1.GetNbinsY()+1;biny++){
      double cont=0;
      double err=1;
      if(h2.GetBinContent(binx,biny) !=0){
	cont=h1.GetBinContent(binx,biny) / h2.GetBinContent(binx,biny);
	err=sqrt(pow(h1.GetBinError(binx,biny)/ h2.GetBinContent(binx,biny),2) + pow((cont / h2.GetBinContent(binx,biny)* h2.GetBinError(binx,biny)),2));
      }
      out.SetBinContent(binx,biny,cont);
      out.SetBinError(binx,biny,err);
    }
  }
  return out;
}


TH1D divideTH1DBinomial(TH1D &h1, TH1D &h2){ //! out = h1 / h2
  TH1D out=h1; 
  if(h1.GetNbinsX() != h2.GetNbinsX()){
    std::cout << "divideTH1DBinomial: Error! histograms must have same binning!" << std::endl;
    return h1;
  }
  for(int binx=1;binx<=h1.GetNbinsX()+1;binx++){
    double cont=0;
    double err=1;
    if(h2.GetBinContent(binx) !=0){
      cont=h1.GetBinContent(binx) / h2.GetBinContent(binx);
      err=sqrt(cont*(1-cont)/ h1.GetBinContent(binx));
    }
    out.SetBinContent(binx,cont);
    out.SetBinError(binx,err);
  }  
  return out;
}





template <class t, class u>
bool noOverlap(t &first, u &sec, double deltaR){
    bool nooverlap=true;
      if(pow(deltaR,2) > pow(first->eta() - sec->eta(),2) + pow(first->phi() - sec->phi(),2)){
	nooverlap=false;
      }
  return nooverlap;
}

template <class T, class U>
bool noOverlap(T & first, typename std::vector<U> &vecsec, double deltaR){
  bool nooverlap=true;
  for(typename std::vector<U>::iterator sec=vecsec.begin();sec<vecsec.end();++sec){
    if(!noOverlap(first,sec,deltaR)){
      nooverlap=false;
      break;
    }
  }
  return nooverlap;
}




template <class t>
int isIn(t element, typename std::vector<t> vec){
  int IsIn=-1;
  for(unsigned int i=0;i<vec.size();i++){
    if(vec[i] == element){
      IsIn=i;
      break;
    }
  }
  return IsIn;
}



template <class T, class U>
std::vector<T>& operator<<(std::vector<T>& vec, const U& x) {
  vec.push_back((T)x);
    return vec;
}

namespace top{
  void displayStatusBar(Long64_t event, Long64_t nEvents);
}

#endif
