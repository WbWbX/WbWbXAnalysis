#ifndef NTGenJet_h
#define NTGenJet_h
#include "mathdefs.h"
#include <map>

namespace ztop{

  class NTGenJet{

  public:
    explicit NTGenJet(){}
    ~NTGenJet(){}


    void setP4(PolarLorentzVector p4In){p4_=p4In;}
    void setP4(LorentzVector p4In){p4_=p4In;}
 
    void setMotherIts(const std::vector<int> & its){motherits_=its;}
    void setMothers(const std::vector<ztop::NTGenParticle *> & p){motherPs_=p;}
    void addMotherIt(int it){motherits_.push_back(it);}

    void setGenId(int genid){genid_=genid;}

    const int& genId()const{return genid_;}
    const std::vector<int> &  motherIts()const{return motherits_;}
    const std::vector<ztop::NTGenParticle *> & mothers()const{return motherPs_;}

    const PolarLorentzVector & p4()const{return p4_;}
    double pt()const{return p4_.Pt();}
    double E()const {return p4_.E();}
    double e()const {return p4_.E();}
    double phi()const{return p4_.Phi();}
    double eta()const{return p4_.Eta();}
    double m()const{return p4_.M();}

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

    PolarLorentzVector p4_;
    std::vector<int> motherits_;
    std::vector<ztop::NTGenParticle *> motherPs_;
    int genid_;

    std::map<std::string, double> memberss_;
    std::map<int, double> members_;

  };
}
#endif
