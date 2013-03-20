#ifndef NTGenJet_h
#define NTGenJet_h
#include "mathdefs.h"

namespace top{

  class NTGenJet{

  public:
    explicit NTGenJet(){}
    ~NTGenJet(){}


    void setP4(PolarLorentzVector p4In){p4_=p4In;}
    void setP4(LorentzVector p4In){p4_=p4In;}
 
    void setMother(size_t it_pos)   {motherits_.push_back(it_pos);} 
    void setGenId(int genid){genid_=genid;}

    int genId(){return genid_;}
    size_t mother(size_t i=0){return motherits_.at(i);}

    const PolarLorentzVector & p4(){return p4_;}
    double pt(){return p4_.Pt();}
    double E() {return p4_.E();}
    double e() {return p4_.E();}
    double phi(){return p4_.Phi();}
    double eta(){return p4_.Eta();}
    double m(){return p4_.M();}


  protected:

    PolarLorentzVector p4_;
    std::vector<size_t> motherits_;
    int genid_;

  };
}
#endif
