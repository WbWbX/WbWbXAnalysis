#ifndef NTBTagSF_H
#define NTBTagSF_H
#include "TtZAnalysis/Tools/interface/bTagBase.h"
#include "TtZAnalysis/DataFormats/interface/NTJet.h"

namespace top{

  class NTBTagSF : public bTagBase{
  public:
    NTBTagSF(){bTagBase();std::cout << "WARNING DONT USE THIS CLASS - except for testing - UNFINISHED"<<std::endl;}
    ~NTBTagSF(){}

    void fillEff(top::NTJet * jet, double puweight){ //!overload: jet, puweight
      bTagBase::fillEff(jet->p4(), jet->genPartonFlavour(), jet->btag(), puweight);
    }

    double getNTEventWeight(const std::vector<top::NTJet *> &);

  };
}


namespace top{

  double NTBTagSF::getNTEventWeight(const std::vector<top::NTJet *> & jets){
    // std::vector<const top::LorentzVector *> p4s;p4s.clear();
    std::vector< top::LorentzVector> p4sv; p4sv.clear();//!change asap when new dataformats ready 
    std::vector<int> genPartonFlavours;
    for(size_t i=0;i<jets.size();i++){
      //  top::LorentzVector * p4= &(jets.at(i)->p4());
      p4sv.push_back((jets.at(i)->p4()));
      if(jets.at(i)->pt() != jets.at(i)->pt())
	std::cout << "nan input" << std::endl;
      //   p4s << &(p4sv.at(i));
      genPartonFlavours.push_back(jets.at(i)->genPartonFlavour());
    }
    double weight= bTagBase::getEventWeight(p4sv , genPartonFlavours);
    return weight;
  }
}


#endif
