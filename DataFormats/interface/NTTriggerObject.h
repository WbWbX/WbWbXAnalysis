#ifndef NTTriggerObject_h
#define NTTriggerObject_h

#include "mathdefs.h"

namespace ztop{


class NTTriggerObject{
 public:
  explicit NTTriggerObject(){};
  ~NTTriggerObject(){};
  //sets
  void setP4(LorentzVector p4In){p4_=p4In;}
  void setP4(PolarLorentzVector p4In){p4_=p4In;}

  //gets
  const PolarLorentzVector & p4(){return p4_;}
  double pt(){return p4_.Pt();}
  double E() {return p4_.E();}
  double e() {return p4_.E();}
  double phi(){return p4_.Phi();}
  double eta(){return p4_.Eta();}
  double m(){return p4_.M();}

 protected:
  PolarLorentzVector p4_;  

};

}

#endif
