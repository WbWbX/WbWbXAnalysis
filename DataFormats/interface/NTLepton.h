#ifndef NTLepton_h
#define NTLepton_h

#include "mathdefs.h"

namespace ztop{


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
  const PolarLorentzVector & p4()const{return p4_;}
  const PolarLorentzVector & genP4()const{return genP4_;}
  double pt()const{return p4_.Pt();}
  double E() const{return p4_.E();}
  double e() const{return p4_.E();}
  double phi()const{return p4_.Phi();}
  double eta()const{return p4_.Eta();}
  double m()const{return p4_.M();}
 const int& q()const{ return q_;}
  const double &dzV()const{return dZV_;}
  const double &dzVErr()const{return dZVErr_;}
 const double & dZBs()const{return dZBs_;}
  const double& d0V()const{return d0V_;}
  const double &d0Bs()const{return d0Bs_;}

  const int& genMatch()const{return genid_;}


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
