#ifndef NTJet_h
#define NTJet_h

#include "Math/GenVector/LorentzVector.h"

namespace top{
  typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LorentzVector;

  class NTJet{
  public:
  explicit NTJet(){};
  ~NTJet(){};
  //sets
  void setP4(LorentzVector p4In){p4_=p4In;}
  void setGenP4(LorentzVector genP4In){genP4_=genP4In;}
  void setGenPartonFlavour(int partonFlavour){genPartonFlav_=partonFlavour;}
  void setBtag(double Btag){btag_=Btag;}
  void setId(bool ID){id_=ID;}
    void setEmEnergyFraction(double EFrac){emenergyfraction_=EFrac;}

  //gets
  LorentzVector p4(){return p4_;}
  double pt(){return p4_.Pt();}
  double E() {return p4_.E();}
  double e() {return p4_.E();}
  double phi(){return p4_.Phi();}
  double eta(){return p4_.Eta();}
  double m(){return p4_.M();}
  double btag(){return btag_;}
  bool id(){return id_;}
    double emEnergyFraction(){return emenergyfraction_;}

  //gen
  LorentzVector genP4(){return genP4_;}
  double genPt(){return genP4_.Pt();}
  double genE() {return genP4_.E();}
  double genPhi(){return genP4_.Phi();}
  double genEta(){return genP4_.Eta();}
  double genM(){return genP4_.M();}
  int genPartonFlavour(){return genPartonFlav_;}


 protected:
  LorentzVector p4_;
  LorentzVector genP4_;
  int genPartonFlav_;
  double btag_;
    double emenergyfraction_;
  bool id_;

  };
}
#endif
