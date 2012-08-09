#ifndef miscUtils_h
#define miscUtils_h

#include "TString.h"
#include <sstream>
#include <vector>
#include <math.h>
#include <iostream>

template<class t>
TString toTString(t in){
  std::ostringstream s;
  s << in;
  TString out=s.str();
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
bool noOverlap(T & first, typename std::vector<U> vecsec, double deltaR){
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


void displayStatusBar(Long64_t event, Long64_t nEvents){

  if((event +1)* 50 % nEvents <50){ //2% steps
    int statusbar=(event+1) * 50 / nEvents ;
    std::cout << "\r[";
    for(int i=0;i<statusbar;i++){
      std::cout << "=";
    }
    for(int i=statusbar;i<50;i++){
      std::cout << " ";
    }
    std::cout << "] " << statusbar*2 << "%   ";
    flush(std::cout);
    statusbar++;
  }
  if(event==0){
    std::cout << "[                                                  ] " << "0%   ";
    flush(std::cout);
  }

}

#endif

// if((100* entr % nentr)){
//       cout << "\rprocessing: " << (int) 100*i/n << "%";
//       fflush(stdin);
//     }
