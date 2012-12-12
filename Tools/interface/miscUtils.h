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

TH2D divideTH2DBinomial(TH2D &h1, TH2D &h2);
TH2D divideTH2D(TH2D &h1, TH2D &h2);


TH1D divideTH1DBinomial(TH1D &h1, TH1D &h2);





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
