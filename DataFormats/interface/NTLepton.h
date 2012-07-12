#ifndef NTLepton_h
#define NTLepton_h


#include "Math/GenVector/LorentzVector.h"
namespace top{
  typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LorentzVector;



class NTLepton{
 public:
  explicit NTLepton(){q_=0;};
  ~NTLepton(){};
  //sets
  void setP4(LorentzVector p4In){p4_=p4In;}
  void setGenP4(LorentzVector GenP4In){genP4_=GenP4In;}
  void setQ(int qIn){q_=qIn;}
  void setVertexZ(double VertexZ){vertexZ_=VertexZ;}
  void setVertexZErr(double VertexZErr){vertexZErr_=VertexZErr;}

  //gets
  LorentzVector p4(){return p4_;}
  LorentzVector genP4(){return genP4_;}
  double pt(){return p4_.Pt();}
  double E() {return p4_.E();}
  double e() {return p4_.E();}
  double phi(){return p4_.Phi();}
  double eta(){return p4_.Eta();}
  double m(){return p4_.M();}
  int q(){ return q_;}
  double vertexZ(){return vertexZ_;}
  double vertexZErr(){return vertexZErr_;}

 protected:
  LorentzVector p4_;
  LorentzVector genP4_;
  int q_;
  double vertexZ_;
  double vertexZErr_;


};

}

#endif
