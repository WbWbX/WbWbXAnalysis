#ifndef NTMet_h
#define NTMet_h

namespace ztop{
  typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LorentzVector;
 
  class NTMet{
 public:
  explicit NTMet(){};
  ~NTMet(){};
  //sets

  void setP4(ztop::LorentzVector P4){p4_=P4;}

  //gets
  double met(){return p4_.Et();}
  double phi(){return p4_.Phi();}
  ztop::LorentzVector p4(){return p4_;};

 protected:

  ztop::LorentzVector p4_;
  };
}
#endif
