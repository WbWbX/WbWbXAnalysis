#ifndef Analysis_Utils_h
#define Analysis_Utils_h

#include "TtZAnalysis/Tools/interface/histoStackVector.h"
#include "TString.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
//#include "MainAnalyzer.h"
#include <cstdlib>
#include <string>

/*
template<class a>
void recreateRelations(typename std::vector<a *> & mothers, typename std::vector<b *> & daughters){

  for(size_t j=0;i<mothers.size();i++){
    a * mother=mothers.at(i);
    for(size_t j=0;j<daughters.size();j++){
      b * daughter=daughters.at(j);
      typename std::vector<a *> mothers;


    }
  }

}

 */
///DON'T !!! do options stuff here!!!


namespace ztop{


//////old options parser


void rescaleDY(ztop::container1DStackVector * vec, std::vector<TString> contributions, double scalescale=1, bool textout=true,
        TString identifier="mll Z Range", int startingstep=4);

template <class T>
std::vector<T> subdivide(const std::vector<T> & bins, size_t div){
    std::vector<T> out;
    for(size_t i=0;i<bins.size()-1;i++){
        float width=bins.at(i+1)-bins.at(i);
        if((int)i<(int)bins.size()-2){
            for(size_t j=0;j<div;j++)
                out.push_back(bins.at(i)+j*width/div);
        }
        else{
            for(size_t j=0;j<=div;j++)
                out.push_back(bins.at(i)+j*width/div);
        }
    }
    return out;
}



template <class T>
std::vector<T> mergeVectors(const std::vector<T>& a, const std::vector<T> & b){
    std::vector<T> out=b;
    for(size_t i=0;i<a.size();i++){
        bool notfound=true;
        for(size_t j=0;j<b.size();j++){
            if(a.at(i)==b.at(j)){
                notfound=false;
                break;
            }
        }
        if(notfound) out.push_back(a.at(i));
    }
    return out;
}

bool hasEnding (std::string const &fullString, std::string const &ending);


}//ztop


#endif
