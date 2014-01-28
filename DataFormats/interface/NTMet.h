#ifndef NTMet_h
#define NTMet_h
#include "mathdefs.h"
namespace ztop{

 
  class NTMet{
 public:
  explicit NTMet(){};
  ~NTMet(){};
  //sets

  void setP4(ztop::LorentzVector P4){p4_=P4;}
  void setP4(ztop::PolarLorentzVector P4){p4_=P4;}
  void setP4(D_LorentzVector p4In){p4_=PolarLorentzVector(p4In.pt(),p4In.Eta(),p4In.Phi(),p4In.M());}

  //gets
  float met()const{return p4_.Et();}
  float phi()const{return p4_.Phi();}
  const ztop::PolarLorentzVector &p4()const{return p4_;};

 protected:

  ztop::PolarLorentzVector p4_;
  };
}
#endif
