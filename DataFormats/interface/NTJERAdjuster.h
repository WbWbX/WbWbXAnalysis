#ifndef JERADJUSTER_H
#define JERADJUSTER_H

#include "WbWbXAnalysis/DataFormats/interface/NTJet.h"
#include "TopAnalysis/ZTopUtils/interface/JERBase.h"
#include "NTLorentzVector.h"


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
        float recoeta=std::abs(jet->eta());
        float recophi=jet->phi();
        float recom=jet->p4().M();
        correctP4(recopt,recoeta,recophi,recom, jet->genP4().pt());
        jet->setP4(ztop::NTLorentzVector<float>(recopt,jet->eta(),recophi,recom));
    }

};
}
#endif
