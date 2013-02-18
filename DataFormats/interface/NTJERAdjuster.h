#ifndef JERADJUSTER_H
#define JERADJUSTER_H

#include "TtZAnalysis/DataFormats/interface/NTJet.h"
#include "TtZAnalysis/Tools/interface/JERBase.h"

namespace top{

  class NTJERAdjuster : public JERBase{

  public:
    NTJERAdjuster(){}
    ~NTJERAdjuster(){}

    void correctJets(std::vector<top::NTJet*> & jets){
      for(size_t i=0;i<jets.size();i++){
	correctJet(jets.at(i));
      }
    }
    void correctJet(top::NTJet* jet){
      top::LorentzVector temp=jet->p4();
      correctP4(temp, jet->genP4());
      jet->setP4(temp);
    }

  };
}
#endif
