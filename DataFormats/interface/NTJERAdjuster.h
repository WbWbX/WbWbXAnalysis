#ifndef JERADJUSTER_H
#define JERADJUSTER_H

#include "TtZAnalysis/DataFormats/interface/NTJet.h"
#include "TopAnalysis/ZTopUtils/interface/JERBase.h"

namespace ztop{

  class NTJERAdjuster : public JERBase{

  public:
    NTJERAdjuster(){}
    ~NTJERAdjuster(){}

    void correctJets(std::vector<ztop::NTJet*> & jets){
      for(size_t i=0;i<jets.size();i++){
	correctJet(jets.at(i));
      }
    }
    void correctJet(ztop::NTJet* jet){
      ztop::PolarLorentzVector temp=jet->p4();
      correctP4(temp, jet->genP4());
      jet->setP4(temp);
    }

  };
}
#endif
