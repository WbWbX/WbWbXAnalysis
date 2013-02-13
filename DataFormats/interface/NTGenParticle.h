#ifndef NTGenParticle_h
#define NTGenParticle_h

#include <vector>
#include "Math/GenVector/LorentzVector.h"
namespace top{
  typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LorentzVector;

  class NTGenParticle{
  public:
    explicit NTGenParticle(){}
    ~NTGenParticle(){}

    void setP4(LorentzVector p4In){p4_=p4In;}
    void setPdgId(int id){pdgid_=id;}

    void setMother(NTGenParticle * moth, bool clear=true)   {
      if(clear) 
	mothers_.clear();
      mothers_.push_back(moth);
      bool newdaugh=true;
      for(size_t i=0;i<moth->daughters_.size();i++){
	if((moth->daughters_[i]) == this)
	  newdaugh=false;
      }
      if(newdaugh)
	moth->daughters_.push_back(this);
    } 
    void setDaughter( NTGenParticle * daugh, bool clear=true){
      if(clear)
	daughters_.clear();
      daughters_.push_back(daugh);
      bool newmoth=true;
      for(size_t i=0;i<daugh->mothers_.size();i++){
	if((daugh->mother(i)) == this)
	  newmoth=false;
      }
      if(newmoth)
	daugh->mothers_.push_back(this);
    }

    void setDaughters(std::vector< NTGenParticle *> Daughters){daughters_=Daughters;}
    void setMothers(std::vector< NTGenParticle *> Mothers){mothers_=Mothers;}

    void setStatus(int Status){status_=Status;}

    int pdgId(){return pdgid_;}
    int status(){return status_;}

    const LorentzVector & p4(){return p4_;}
    double pt(){return p4_.Pt();}
    double E() {return p4_.E();}
    double e() {return p4_.E();}
    double phi(){return p4_.Phi();}
    double eta(){return p4_.Eta();}
    double m(){return p4_.M();}
    int q(){if(pdgid_<0) return -1; else return 1;}

    const NTGenParticle * daughter(size_t i=0){return daughters_.at(i);}
    const NTGenParticle * mother(size_t i=0){return mothers_.at(i);}

    std::vector< NTGenParticle *> daughters(){return daughters_;}
    std::vector< NTGenParticle *> mothers(){return mothers_;}

  protected:


    int pdgid_, status_;
    LorentzVector p4_;
    std::vector<NTGenParticle *> mothers_,daughters_;
    //std::vector<int> mothers_, daughters_;

  };
}


#endif
