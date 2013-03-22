#ifndef NTJet_h
#define NTJet_h

#include "mathdefs.h"
#include <map>

namespace top{
 

  class NTJet{
  public:
  explicit NTJet(){};
    ~NTJet(){};
    //sets
    void setP4(LorentzVector p4In){p4_=p4In;}
    void setP4(PolarLorentzVector p4In){p4_=p4In;}
    void setGenP4(PolarLorentzVector genP4In){genP4_=genP4In;}
    void setGenP4(LorentzVector genP4In){genP4_=genP4In;}
    void setGenPartonFlavour(int partonFlavour){genPartonFlav_=partonFlavour;}
    void setBtag(double Btag){btag_=Btag;}
    void setId(bool ID){id_=ID;}
    void setEmEnergyFraction(double EFrac){emenergyfraction_=EFrac;}
    void setCorrFactor(double fac){corrfact_=fac;}
    void setGenMatch(int id_int){genid_=id_int;}

  //gets
    const PolarLorentzVector &  p4(){return p4_;}
    PolarLorentzVector p4Uncorr(){return p4_ * (1/corrfact_);}
    int genMatch(){return genid_;}

    double pt(){return p4_.Pt();}
    double E() {return p4_.E();}
    double e() {return p4_.E();}
    double phi(){return p4_.Phi();}
    double eta(){return p4_.Eta();}
    double m(){return p4_.M();}
    double btag(){return btag_;}
    bool id(){return id_;}
    double emEnergyFraction(){return emenergyfraction_;}

    //gen
    const PolarLorentzVector & genP4(){return genP4_;}
    double genPt(){return genP4_.Pt();}
    double genE() {return genP4_.E();}
    double genPhi(){return genP4_.Phi();}
    double genEta(){return genP4_.Eta();}
    double genM(){return genP4_.M();}
    int genPartonFlavour(){return genPartonFlav_;}

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
    int genid_;
    PolarLorentzVector p4_;
    PolarLorentzVector genP4_;
    int genPartonFlav_;
    double btag_;
    double emenergyfraction_;
    bool id_;
    double corrfact_;

    std::map<std::string, double> memberss_;
    std::map<int, double> members_;

  };
}
#endif
