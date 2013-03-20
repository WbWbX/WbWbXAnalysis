#ifndef NTTrack_h
#define NTTrack_h


#include "mathdefs.h"
namespace top{
 
  class NTTrack{

  public:
    explicit NTTrack(){q_=0;}
    ~NTTrack(){}

    void setP4(LorentzVector p4In){p4_=p4In;}
    void setQ(int qIn){q_=qIn;}
    void setDz(double Dz){dz_=Dz;}
    void setDzErr(double DzErr){dzErr_=DzErr;}

    const PolarLorentzVector & p4(){return p4_;}
    double pt(){return p4_.Pt();}
    double phi(){return p4_.Phi();}
    double eta(){return p4_.Eta();}
    double p() {return p4_.E();}
    int q(){ return q_;}
    double dz(){return dz_;}
    double dzErr(){return dzErr_;}
  protected:
    PolarLorentzVector p4_;
    int q_;
    double dz_;
    double dzErr_;

  };

}
#endif
