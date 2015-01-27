#include <vector>
#include <math.h>

template <class t, class u>
typename std::vector<t> noOverlap(std::vector<t> &a, std::vector<u> &b, double deltaR){ //! checks overlap and returns vector of first argument without overlap candidates
  std::vector<t> out;
  for(typename std::vector<t>::iterator first=a.begin();first<a.end();first++){
    bool nooverlap=true;
    for(typename std::vector<u>::iterator sec=b.begin();sec<b.end();sec++){
      if(pow(deltaR,2) > pow(first->eta() - sec->eta(),2) + pow(first->phi() - sec->phi(),2)){
	nooverlap=false;
	break;
      }
    }
    if(nooverlap){
      out.push_back(*first);
    }
  }
  return out;
}

#include <iostream>
void selectorTools(){
  for(int i=0;i<10;++i){
    std::cout << i << std::endl;
  }


}
