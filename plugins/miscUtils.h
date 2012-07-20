#ifndef miscUtils_h
#define miscUtils_h

#include "TString.h"
#include <sstream>

template<class t>
TString toTString(t in){
  std::ostringstream s;
  s << in;
  TString out=s.str();
  return out;
}

#include <vector>
#include <math.h>



template <class t, class u>
bool noOverlap(t &first, u &sec, double deltaR){ //! checks overlap and returns vector of first argument without overlap candidates
    bool nooverlap=true;
      if(pow(deltaR,2) > pow(first->eta() - sec->eta(),2) + pow(first->phi() - sec->phi(),2)){
	nooverlap=false;
      }
  return nooverlap;
}

template <class t>
bool isIn(t element, typename std::vector<t> vec){
  bool IsIn=false;
  for(unsigned int i=0;i<vec.size();i++){
    if(vec[i] == element){
      IsIn=true;
      break;
    }
  }
  return IsIn;
}



template <class T, class U>
vector<T>& operator<<(vector<T>& vec, const U& x) {
  vec.push_back((T)x);
    return vec;
}



#endif
