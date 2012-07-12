#ifndef NTTrack_h
#define NTTrack_h


#include "Math/GenVector/LorentzVector.h"
namespace top{
  typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LorentzVector;

  class NTTrack{

  public:
    explicit NTTrack(){q_=0;}
    ~NTTrack(){}

    void setP4(LorentzVector p4In){p4_=p4In;}
    void setQ(int qIn){q_=qIn;}
    void setVertexZ(double VertexZ){vertexZ_=VertexZ;}
    void setVertexZErr(double VertexZErr){vertexZErr_=VertexZErr;}

    LorentzVector p4(){return p4_;}
    double pt(){return p4_.Pt();}
    double phi(){return p4_.Phi();}
    double eta(){return p4_.Eta();}
    double p() {return p4_.E();}
    int q(){ return q_;}
    double vertexZ(){return vertexZ_;}
    double vertexZErr(){return vertexZErr_;}
  protected:
    LorentzVector p4_;
    int q_;
    double vertexZ_;
    double vertexZErr_;

  };

}
#endif
