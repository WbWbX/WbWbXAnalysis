#ifndef NTLepton_h
#define NTLepton_h

#include "mathdefs.h"

namespace top{


class NTLepton{
 public:
  explicit NTLepton(){q_=0;};
  ~NTLepton(){};
  //sets
  void setP4(LorentzVector p4In){p4_=p4In;}
  void setP4(PolarLorentzVector p4In){p4_=p4In;}
  void setQ(int qIn){q_=qIn;}
  void setDz(double DZ){dZ_=DZ;}
  void setDzErr(double DZErr){dZErr_=DZErr;}

  void setGenMatch(int matchid){genid_=matchid;}
  void setGenP4(PolarLorentzVector genP4In){genP4_=genP4In;}
  void setGenP4(LorentzVector genP4In){genP4_=genP4In;}

  //gets
  const PolarLorentzVector & p4(){return p4_;}
  const PolarLorentzVector & genP4(){return genP4_;}
  double pt(){return p4_.Pt();}
  double E() {return p4_.E();}
  double e() {return p4_.E();}
  double phi(){return p4_.Phi();}
  double eta(){return p4_.Eta();}
  double m(){return p4_.M();}
  int q(){ return q_;}
  double dZ(){return dZ_;}
  double dZErr(){return dZErr_;}

  int genMatch(){return genid_;}


 protected:
  PolarLorentzVector p4_;  
  int q_;
  PolarLorentzVector genP4_;
  double dZ_;
  double dZErr_;
  int genid_;


};

}

#endif
