#ifndef NTMuon_h
#define NTMuon_h


#include "mathdefs.h"
#include "NTLepton.h"
#include "NTIsolation.h"
#include <vector>
#include <string>
#include <algorithm>
#include <map>

namespace top{



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
    void setIso(top::NTIsolation Iso){iso_=Iso;}
    void setMatchedTrig(std::vector<std::string> MatchedTrig){matchedTrig_=MatchedTrig;}
    void setTrackP4(top::LorentzVector trkp4in){trkP4_=trkp4in;}
    void setIsPf(bool ispf){ispf_=ispf;}
  

    //gets

    bool isGlobal(){return isGlobal_;}
    bool isTracker(){return isTracker_;}
    double normChi2(){return normChi2_;}
    int trkHits(){return trkHits_;}
    int pixHits(){return pixHits_;}
    int muonHits(){return muonHits_;}
    top::NTIsolation iso(){return iso_;}
    double isoVal(){
return (iso_.chargedHadronIso() + std::max(0.0,iso_.neutralHadronIso() + iso_.photonIso() - 0.5*iso_.puChargedHadronIso()))/p4_.pt();
}
    std::vector<std::string> matchedTrig(){return matchedTrig_;}
    PolarLorentzVector trackP4(){return trkP4_;}
    bool isPf(){return ispf_;}

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
    top::NTIsolation iso_;
    std::vector<std::string> matchedTrig_;
    PolarLorentzVector trkP4_;


    std::map<std::string, double> memberss_;
    std::map<int, double> members_;
  };

}

#endif
