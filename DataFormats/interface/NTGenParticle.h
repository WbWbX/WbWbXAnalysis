#ifndef NTGenParticle_h
#define NTGenParticle_h

#include <vector>
#include "mathdefs.h"
#include <map>

namespace top{

  class NTGenParticle{
  public:
    explicit NTGenParticle(){}
    ~NTGenParticle(){}

    void setP4(LorentzVector p4In){p4_=p4In;}
    void setPdgId(int id){pdgid_=id;}

    void setGenId(int genid){genid_=genid;};

    int genId(){return genid_;}

    void setMother(size_t it_pos)   {motherits_.push_back(it_pos);} 
    void setDaughter(size_t it_pos) {daughterits_.push_back(it_pos); }

    void setDaughters(const std::vector<size_t> & Daughters){daughterits_=Daughters;}
    void setMothers  (const std::vector<size_t> & Mothers)  {motherits_=Mothers;}

    void setStatus(int Status){status_=Status;}

    int pdgId(){return pdgid_;}
    int status(){return status_;}

    const PolarLorentzVector & p4(){return p4_;}
    double pt(){return p4_.Pt();}
    double E() {return p4_.E();}
    double e() {return p4_.E();}
    double phi(){return p4_.Phi();}
    double eta(){return p4_.Eta();}
    double m(){return p4_.M();}
    int q(){if(pdgid_<0) return -1; else if(pdgid_>0) return 1; else return 0;}

    size_t daughter(size_t i=0){return daughterits_.at(i);}
    size_t mother(size_t i=0){return motherits_.at(i);}

    std::vector<size_t> & daughters(){return daughterits_;}
    std::vector<size_t> & mothers(){return motherits_;}

    void clearDaughters(){daughterits_.clear();}
    void clearMothers(){motherits_.clear();}

    //extra

    void setMember(std::string Membername, double value){
      memberss_[Membername]=value;
    }
    double getMember(std::string membername){
      if(memberss_.find(membername) != memberss_.end())
	return memberss_.find(membername)->second;
      else
	return -99999999999;
    }
    void setMember(int Memberidx, double value){
      members_[Memberidx]=value;
    }
    double getMember(int memberidx){
      if(members_.find(memberidx) != members_.end())
	return members_.find(memberidx)->second;
      else
	return -99999999999;
    }




  protected:


    int pdgid_, status_, genid_;
    PolarLorentzVector p4_;
    std::vector<size_t> motherits_,daughterits_;
    //std::vector<int> mothers_, daughters_;

    std::map<std::string, double> memberss_;
    std::map<int, double> members_;

  };
}


#endif
