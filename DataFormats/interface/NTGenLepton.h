#ifndef NTGenLepton_h
#define NTGenLepton_h

#include "mathdefs.h"
namespace ztop{


  class NTGenLepton{
  public:
    explicit NTGenLepton(){}
    ~NTGenLepton(){}

    void setP4(const PolarLorentzVector& p4In){p4_=p4In;}
    void setP4(const LorentzVector& p4In){p4_=p4In;}
    void setP4(D_LorentzVector p4In){p4_=PolarLorentzVector(p4In.pt(),p4In.Eta(),p4In.Phi(),p4In.M());}
    void setPdgId(int id){pdgid_=id;}
    void setMothers(std::vector<int> mothers){mothers_=mothers;}
    void setDaughters(std::vector<int> daughters){daughters_=daughters;}
    void setStatus(int Status){status_=Status;}

    int pdgId(){return pdgid_;}
    int status(){return status_;}

    const PolarLorentzVector& p4(){return p4_;}
    float pt(){return p4_.Pt();}
    float E() {return p4_.E();}
    float e() {return p4_.E();}
    float phi(){return p4_.Phi();}
    float eta(){return p4_.Eta();}
    float m(){return p4_.M();}
    int q(){if(pdgid_<0) return -1; else return 1;}

    int getMother(){if(mothers_.size()>0) return mothers_.at(0); else return 0;}
    std::vector<int> getMothers(){return mothers_;}
    int getDaughter(){if(daughters_.size()>0) return daughters_.at(0); else return 0;}
    std::vector<int> getDaughters(){return daughters_;}

  private:


    int pdgid_, status_;
    PolarLorentzVector p4_;
    std::vector<int> mothers_, daughters_;

  };
}


#endif
