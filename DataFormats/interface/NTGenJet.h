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
 
    void addMother(size_t it_pos)   {motherits_.push_back(it_pos);} 
    void setMother(size_t it_pos)   {motherits_.clear();motherits_.push_back(it_pos);} 
    void setGenId(int genid){genid_=genid;}

    int genId(){return genid_;}
    size_t mothersSize(){return motherits_.size();}
    size_t mother(size_t i=0){return motherits_.at(i);}

    const PolarLorentzVector & p4(){return p4_;}
    double pt(){return p4_.Pt();}
    double E() {return p4_.E();}
    double e() {return p4_.E();}
    double phi(){return p4_.Phi();}
    double eta(){return p4_.Eta();}
    double m(){return p4_.M();}

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
    std::vector<size_t> motherits_;
    int genid_;

    std::map<std::string, double> memberss_;
    std::map<int, double> members_;

  };
}
#endif
