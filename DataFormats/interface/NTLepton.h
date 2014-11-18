#ifndef NTLepton_h
#define NTLepton_h

#include "mathdefs.h"
#include "NTLorentzVector.h"

namespace ztop{

class NTLepton{
 public:
  explicit NTLepton(){q_=0;};
  ~NTLepton(){};
  //sets
  void setP4(NTLorentzVector<float> p4In){p4_=p4In;}
 // void setP4(PolarLorentzVector p4In){p4_=p4In;}
  void setP4(D_LorentzVector p4In){p4_=NTLorentzVector<float>(p4In.pt(),p4In.Eta(),p4In.Phi(),p4In.M());}
  void setP4Err(const float & err){p4err_=err;}
  void setQ(int qIn){q_=qIn;}
  void setDzV(float DZV){dZV_=DZV;}
  void setDzVErr(float DZErr){dZVErr_=DZErr;}
  void setDzBs(float in){dZBs_=in;}
  void setD0V(float in){d0V_=in;}
  void setD0Bs(float in){d0Bs_=in;}

  void setGenMatch(int matchid){genid_=matchid;}
 // void setGenP4(PolarLorentzVector genP4In){genP4_=genP4In;}
  void setGenP4(NTLorentzVector<float> genP4In){genP4_=genP4In;}

  //gets
  const NTLorentzVector<float> & p4()const{return p4_;}
  const NTLorentzVector<float> & genP4()const{return genP4_;}
  const float & p4Err(){return p4err_;}
  float pt()const{return p4_.Pt();}
  float E() const{return p4_.E();}
  float e() const{return p4_.E();}
  float phi()const{return p4_.Phi();}
  float eta()const{return p4_.Eta();}
  float m()const{return p4_.M();}
 const int& q()const{ return q_;}
  const float &dzV()const{return dZV_;}
  const float &dzVErr()const{return dZVErr_;}
 const float & dZBs()const{return dZBs_;}
  const float& d0V()const{return d0V_;}
  const float &d0Bs()const{return d0Bs_;}

  const int& genMatch()const{return genid_;}


 protected:
  NTLorentzVector<float> p4_;
  float p4err_;
  int q_;
  NTLorentzVector<float> genP4_;
  float dZV_;
  float dZVErr_;
  float dZBs_;
  float d0Bs_;
  float d0V_;
  int genid_;


};

}

#endif
