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
  void setDzV(double DZV){dZV_=DZV;}
  void setDzVErr(double DZErr){dZVErr_=DZErr;}
  void setDzBs(double in){dZBs_=in;}
  void setD0V(double in){d0V_=in;}
  void setD0Bs(double in){d0Bs_=in;}

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
  double dzV(){return dZV_;}
  double dzVErr(){return dZVErr_;}
  double dZBs(){return dZBs_;}
  double d0V(){return d0V_;}
  double d0Bs(){return d0Bs_;}

  int genMatch(){return genid_;}


 protected:
  PolarLorentzVector p4_;  
  int q_;
  PolarLorentzVector genP4_;
  double dZV_;
  double dZVErr_;
  double dZBs_;
  double d0Bs_;
  double d0V_;
  int genid_;


};

}

#endif
