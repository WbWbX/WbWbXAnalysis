#ifndef NTSuClu_h
#define NTSuClu_h
#include "mathdefs.h"
#include "NTLorentzVector.h"

namespace ztop{

class NTSuClu{
public:
    explicit NTSuClu(){NTLorentzVector<float> temp(0,0,0,0); setP4(temp);};
    ~NTSuClu(){};

    void setP4(NTLorentzVector<float> P4energ){P4_=P4energ;};
    void setP4(D_LorentzVector p4In){P4_=NTLorentzVector<float>(p4In.pt(),p4In.Eta(),p4In.Phi(),p4In.M());}
   // const PolarLorentzVector & p4()const{return P4_;};
    float  eta()const{return P4_.Eta();}
    float pt() const{return P4_.Pt();}
private:
    NTLorentzVector<float> P4_;
};
}

#endif
