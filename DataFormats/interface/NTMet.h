#ifndef NTMet_h
#define NTMet_h
#include "mathdefs.h"
#include "NTLorentzVector.h"

namespace ztop{

 
  class NTMet{
 public:
  explicit NTMet(){};
  ~NTMet(){};
  //sets

  void setP4(ztop::NTLorentzVector<float> P4){p4_=P4;}
 // void setP4(ztop::PolarLorentzVector P4){p4_=P4;}
  void setP4(D_LorentzVector p4In){p4_=NTLorentzVector<float>(p4In.pt(),p4In.Eta(),p4In.Phi(),p4In.M());}

  //gets
  float met()const{return p4_.pt();}
  float phi()const{return p4_.Phi();}
  const ztop::NTLorentzVector<float> &p4()const{return p4_;};

 protected:

  ztop::NTLorentzVector<float> p4_;
  };
}
#endif
