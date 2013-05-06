#ifndef NTGenParticle_h
#define NTGenParticle_h

#include <vector>
#include "mathdefs.h"
#include <map>

namespace ztop{

  class NTGenParticle{
  public:
    explicit NTGenParticle(){}
    ~NTGenParticle(){}

    void setP4(LorentzVector p4In){p4_=p4In;}
    void setPdgId(int id){pdgid_=id;}

    void setGenId(int genid){genid_=genid;};

    const int & genId(){return genid_;}

    void setMothers(const std::vector<ztop::NTGenParticle *> & p){motherPs_=p;}
    void setDaughters(const std::vector<ztop::NTGenParticle *> & p){daughterPs_=p;}

    void setDaughterIts(const std::vector<int> & Daughters){daughterits_=Daughters;}
    void setMotherIts  (const std::vector<int> & Mothers)  {motherits_=Mothers;}

    void addMotherIt(int it){motherits_.push_back(it);}
    void addDaughterIt(int it){daughterits_.push_back(it);}

    void setStatus(int Status){status_=Status;}

    int pdgId(){return pdgid_;}
    int status(){return status_;}

    const PolarLorentzVector & p4(){return p4_;}
    double pt(){return p4_.Pt();}
    double  E() {return p4_.E();}
    double  e() {return p4_.E();}
    double  phi(){return p4_.Phi();}
    double  eta(){return p4_.Eta();}
    double  m(){return p4_.M();}
    int q(){if(pdgid_<0) return -1; else if(pdgid_>0) return 1; else return 0;}

    
    std::vector<int> & daughterIts(){return daughterits_;}
    std::vector<int> & motherIts(){return motherits_;}

    const std::vector<NTGenParticle *> & daughters(){return daughterPs_;}
    const std::vector<NTGenParticle *> & mothers(){return motherPs_;}

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
    std::vector<int> motherits_,daughterits_;
    std::vector<ztop::NTGenParticle *> daughterPs_,motherPs_;
    //std::vector<int> mothers_, daughters_;

    std::map<std::string, double> memberss_;
    std::map<int, double> members_;

  };
}


#endif
