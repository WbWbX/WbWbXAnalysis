#ifndef NTGenParticle_h
#define NTGenParticle_h

#include <vector>
#include "mathdefs.h"
#include <map>
#include "NTLorentzVector.h"


namespace ztop{

  class NTGenParticle{
  public:
    explicit NTGenParticle(){}
    ~NTGenParticle(){}

    void setP4(NTLorentzVector<float> p4In){p4_=p4In;}
    void setP4(D_LorentzVector p4In){p4_=NTLorentzVector<float>(p4In.pt(),p4In.Eta(),p4In.Phi(),p4In.M());}
    void setPdgId(int id){pdgid_=id;}

    void setGenId(int genid){genid_=genid;};
    void setQ(int charge){q_=charge;}

    const int & genId()const{return genid_;}

    void setMothers(const std::vector<ztop::NTGenParticle *> & p){motherPs_=p;}
    void setDaughters(const std::vector<ztop::NTGenParticle *> & p){daughterPs_=p;}

    void setDaughterIts(const std::vector<int> & Daughters){daughterits_=Daughters;}
    void setMotherIts  (const std::vector<int> & Mothers)  {motherits_=Mothers;}

    void addMotherIt(int it){motherits_.push_back(it);}
    void addDaughterIt(int it){daughterits_.push_back(it);}

    void setStatus(int Status){status_=Status;}

   const int& pdgId()const{return pdgid_;}
   const int& status()const{return status_;}

    const NTLorentzVector<float> & p4(){return p4_;}
    float pt()const{return p4_.Pt();}
    float  E()const {return p4_.E();}
    float  e()const {return p4_.E();}
    float  phi()const{return p4_.Phi();}
    float  eta()const{return p4_.Eta();}
    float  m()const{return p4_.M();}
    const int& q()const{return q_;}

    //setget mixed
    std::vector<int> & daughterIts(){return daughterits_;}
    std::vector<int> & motherIts(){return motherits_;}

    const std::vector<NTGenParticle *> & daughters(){return daughterPs_;}
    const std::vector<NTGenParticle *> & mothers(){return motherPs_;}

    //extra

    void setMember(std::string Membername, float value){
      memberss_[Membername]=value;
    }
    float getMember(std::string membername){
      if(memberss_.find(membername) != memberss_.end())
	return memberss_.find(membername)->second;
      else
	return -99999999999;
    }
    void setMember(int Memberidx, float value){
      members_[Memberidx]=value;
    }
    float getMember(int memberidx){
      if(members_.find(memberidx) != members_.end())
	return members_.find(memberidx)->second;
      else
	return -99999999999;
    }




  protected:


    int pdgid_, status_, genid_,q_;
    NTLorentzVector<float> p4_;
    std::vector<int> motherits_,daughterits_;
    std::vector<ztop::NTGenParticle *> daughterPs_,motherPs_;
    //std::vector<int> mothers_, daughters_;

    std::map<std::string, float> memberss_;
    std::map<int, float> members_;

  };
}


#endif
