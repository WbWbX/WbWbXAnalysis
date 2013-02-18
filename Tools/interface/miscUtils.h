#ifndef TOPMISCUTILS_h
#define TOPMISCUTILS_h

#include "TString.h"
#include <sstream>
#include <vector>
#include <math.h>
#include <iostream>
#include <algorithm>
#include "TH1D.h"
#include "TH2D.h"

template<class t>
t square(const t & in){return in*in;}

template<class t>
TString toTString(t in){
  std::ostringstream s;
  s << in;
  TString out=s.str();
  return out;
}

template<class t>
std::string toString(t in){
  std::ostringstream s;
  s << in;
  std::string out=s.str();
  return out;
}

TH2D divideTH2DBinomial(TH2D &h1, TH2D &h2);
TH2D divideTH2D(TH2D &h1, TH2D &h2);

void addRelError(TH2D &h, double err);


TH1D divideTH1DBinomial(TH1D &h1, TH1D &h2);

template <class t>
std::vector<unsigned int> getOrderAscending(typename std::vector<t> vec){
  typename std::vector<t> vec2=vec;
  std::sort (vec.begin(), vec.end());
  std::vector<unsigned int> out;
  for(unsigned int i=0;i<vec.size();i++){
    for(unsigned int j=0;j<vec2.size();j++){
      if(vec.at(i) == vec2.at(j)){
	out.push_back(j);
	break;
      }
    }
  }
  return out;
}

template <class t, class u>
double dR(t &first, u &sec){
  return sqrt((first.eta() - sec.eta())*(first.eta() - sec.eta()) + (first.phi() - sec.phi())*(first.phi() - sec.phi()));
}

/*
template <class t, class u>
bool noOverlap(t &first, u &sec, double deltaR){
    bool nooverlap=true;
      if(deltaR*deltaR > square(first.eta() - sec.eta()) + square(first.phi() - sec.phi())){
	nooverlap=false;
      }
  return nooverlap;
}
*/
template <class t, class u>
bool noOverlap(t *first, u *sec, double deltaR){
    bool nooverlap=true;
      if((deltaR*deltaR) > square(first->eta() - sec->eta()) + square(first->phi() - sec->phi())){
	nooverlap=false;
      }
  return nooverlap;
}
/*
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
*/
template <class T, class U>
bool noOverlap(T * first, typename std::vector<U*> &vecsec, double deltaR){
  bool nooverlap=true;
  for(size_t i=0;i<vecsec.size();i++){
    if(!noOverlap(first,vecsec.at(i),deltaR)){
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
