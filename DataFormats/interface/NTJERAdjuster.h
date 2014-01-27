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
      float recopt=jet->pt();
      float recoeta=jet->eta();
      float recophi=jet->phi();
      float recom=jet->p4().M();
      correctP4(recopt,recoeta,recophi,recom, jet->genP4().pt());
      jet->setP4(ztop::PolarLorentzVector(recopt,recoeta,recophi,recom));
    }

  };
}
#endif
