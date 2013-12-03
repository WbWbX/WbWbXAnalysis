#ifndef NTMuon_h
#define NTMuon_h


#include "mathdefs.h"
#include "NTLepton.h"
#include "NTIsolation.h"
#include <vector>
#include <string>
#include <algorithm>
#include <map>

namespace ztop{



  class NTMuon : public NTLepton {
  public:
    explicit NTMuon(){q_=0;};
    ~NTMuon(){};
    //sets
    void setIsGlobal(bool IsGlobal){isGlobal_=IsGlobal;}
    void setIsTracker(bool IsTracker){isTracker_=IsTracker;}
    void setNormChi2(double NormChi2){normChi2_=NormChi2;}
    void setTrkHits(int TrkHits){trkHits_=TrkHits;}
    void setPixHits(int PixHits){pixHits_=PixHits;}
    void setMuonHits(int MuonHits){muonHits_=MuonHits;}
    void setIso(ztop::NTIsolation Iso){iso_=Iso;}
    void setMatchedTrig(std::vector<std::string> MatchedTrig){matchedTrig_=MatchedTrig;}
    void setTrackP4(ztop::LorentzVector trkp4in){trkP4_=trkp4in;}
    void setIsPf(bool ispf){ispf_=ispf;}
  

    //gets

    const bool& isGlobal()const{return isGlobal_;}
    const bool& isTracker()const{return isTracker_;}
    const double &normChi2()const{return normChi2_;}
    const int& trkHits()const{return trkHits_;}
    const int& pixHits()const{return pixHits_;}
    const int& muonHits()const{return muonHits_;}
    const ztop::NTIsolation& iso()const{return iso_;}
    double isoVal() const{
return (iso_.chargedHadronIso() + std::max(0.0,iso_.neutralHadronIso() + iso_.photonIso() - 0.5*iso_.puChargedHadronIso()))/p4_.pt();
}
   const std::vector<std::string>& matchedTrig()const{return matchedTrig_;}
    const PolarLorentzVector &trackP4()const{return trkP4_;}
    const bool &isPf()const{return ispf_;}

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

    bool isGlobal_;
    bool isTracker_;
    bool ispf_;
    double normChi2_;
    int trkHits_,pixHits_;
  
    int muonHits_;
    ztop::NTIsolation iso_;
    std::vector<std::string> matchedTrig_;
    PolarLorentzVector trkP4_;


    std::map<std::string, double> memberss_;
    std::map<int, double> members_;
  };

}

#endif
