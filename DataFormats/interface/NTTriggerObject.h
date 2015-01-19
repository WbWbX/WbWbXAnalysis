#ifndef NTTriggerObject_h
#define NTTriggerObject_h

#include "mathdefs.h"
#include "NTLorentzVector.h"


namespace ztop{


class NTTriggerObject{
 public:
  explicit NTTriggerObject(){};
  ~NTTriggerObject(){};
  //sets
  void setP4(NTLorentzVector<float> p4In){p4_=p4In;}
 // void setP4(PolarLorentzVector p4In){p4_=p4In;}
  void setP4(D_LorentzVector p4In){p4_=NTLorentzVector<float>(p4In.pt(),p4In.Eta(),p4In.Phi(),p4In.M());}

  //gets
  const NTLorentzVector<float> & p4(){return p4_;}
  float pt(){return p4_.Pt();}
  float E() {return p4_.E();}
  float e() {return p4_.E();}
  float phi(){return p4_.Phi();}
  float eta(){return p4_.Eta();}
  float m(){return p4_.M();}

 protected:
	////////////////////////
	/*
	 * Data members. changes here produce incompatibilities between ntuple versions!
	 */
	////////////////////////

  NTLorentzVector<float> p4_;

};

}

#endif
