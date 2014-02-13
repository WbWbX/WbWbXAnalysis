#ifndef NTTrack_h
#define NTTrack_h


#include "mathdefs.h"
#include "NTLorentzVector.h"

namespace ztop{
 
  class NTTrack{

  public:
    explicit NTTrack(){q_=0;}
    ~NTTrack(){}

    void setP4(NTLorentzVector<float> p4In){p4_=p4In;}
    void setP4(D_LorentzVector p4In){p4_=NTLorentzVector<float>(p4In.pt(),p4In.Eta(),p4In.Phi(),p4In.M());}
    void setQ(int qIn){q_=qIn;}
    void setDzV(float Dz){dzV_=Dz;}
    void setDzVErr(float DzErr){dzVErr_=DzErr;}

    const NTLorentzVector<float> & p4(){return p4_;}
    float pt(){return p4_.Pt();}
    float phi(){return p4_.Phi();}
    float eta(){return p4_.Eta();}
    float p() {return p4_.E();}
    int q(){ return q_;}
    float dzV(){return dzV_;}
    float dzVErr(){return dzVErr_;}
  protected:
    NTLorentzVector<float> p4_;
    int q_;
    float dzV_;
    float dzVErr_;

  };

}
#endif
