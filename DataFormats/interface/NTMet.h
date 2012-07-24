#ifndef NTMet_h
#define NTMet_h

namespace top{
  typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LorentzVector;
 
  class NTMet{
 public:
  explicit NTMet(){};
  ~NTMet(){};
  //sets

  void setP4(top::LorentzVector P4){p4_=P4;}

  //gets
  double met(){return p4_.Et();}
  double phi(){return p4_.Phi();}
  top::LorentzVector p4(){return p4_;};

 protected:

  top::LorentzVector p4_;
  };
}
#endif
