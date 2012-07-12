#ifndef NTMuon_h
#define NTMuon_h


#include "Math/GenVector/LorentzVector.h"
#include "NTLepton.h"
#include "NTIsolation.h"
#include <vector>
#include <string>
#include <algorithm>

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
  void setMuonHits(int MuonHits){muonHits_=MuonHits;}
  void setDbs(double Dbs){dbs_=Dbs;}
  void setIso03(top::NTIsolation Iso03){iso03_=Iso03;}
  void setIso04(top::NTIsolation Iso04){iso04_=Iso04;}
  void setMatchedTrig(std::vector<std::string> MatchedTrig){matchedTrig_=MatchedTrig;}
  void setTrackP4(top::LorentzVector trkp4in){trkP4_=trkp4in;}

  //gets

  bool isGlobal(){return isGlobal_;}
  bool isTracker(){return isTracker_;}
  double normChi2(){return normChi2_;}
  int trkHits(){return trkHits_;}
  int muonHits(){return muonHits_;}
  double dbs(){return dbs_;}
  top::NTIsolation iso03(){return iso03_;}
  top::NTIsolation iso04(){return iso04_;}
  double isoVal03(){
    return (iso03_.chargedHadronIso() + std::max(0.0,iso03_.neutralHadronIso() + iso03_.photonIso() - 0.5*iso03_.puChargedHadronIso()))/p4_.pt();
  }
  double isoVal04(){
    return (iso04_.chargedHadronIso() + std::max(0.0,iso04_.neutralHadronIso() + iso04_.photonIso() - 0.5*iso04_.puChargedHadronIso()))/p4_.pt();
  }

  std::vector<std::string> matchedTrig(){return matchedTrig_;}
  LorentzVector trackP4(){return trkP4_;}

 protected:

  bool isGlobal_;
  bool isTracker_;
  double normChi2_;
  int trkHits_;
  int muonHits_;
  double dbs_;
  top::NTIsolation iso03_;
  top::NTIsolation iso04_;
  std::vector<std::string> matchedTrig_;
  LorentzVector trkP4_;


};

}

#endif
