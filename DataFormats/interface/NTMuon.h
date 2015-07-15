#ifndef NTMuon_h
#define NTMuon_h


#include "mathdefs.h"
#include "NTLepton.h"
#include "NTIsolation.h"
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include "NTLorentzVector.h"


namespace ztop{



class NTMuon : public NTLepton {
public:
    explicit NTMuon(){q_=0;};
    ~NTMuon(){};


    //sets
    void setIsGlobal(bool IsGlobal){isGlobal_=IsGlobal;}
    void setIsTracker(bool IsTracker){isTracker_=IsTracker;}
    void setNormChi2(float NormChi2){normChi2_=NormChi2;}
    void setTrkHits(int TrkHits){trkHits_=TrkHits;}
    void setPixHits(int PixHits){pixHits_=PixHits;}
    void setMuonHits(int MuonHits){muonHits_=MuonHits;}
    void setMatchedStations(int matched){ matchedstations_=matched;}
    void setIso(ztop::NTIsolation Iso){iso_=Iso;}
    void setMatchedTrig(std::vector<std::string> MatchedTrig){matchedTrig_=MatchedTrig;}
    void setTrackP4(ztop::NTLorentzVector<float> trkp4in){trkP4_=trkp4in;}
    void setIsPf(bool ispf){ispf_=ispf;}


    //gets

    const bool& isGlobal()const{return isGlobal_;}
    const bool& isTracker()const{return isTracker_;}
    const float &normChi2()const{return normChi2_;}
    const int& trkHits()const{return trkHits_;}
    const int& pixHits()const{return pixHits_;}
    const int& muonHits()const{return muonHits_;}
    const int& matchedStations()const{return matchedstations_;}
    const ztop::NTIsolation& iso()const{return iso_;}
    float isoVal() const{
        return (iso_.chargedHadronIso() + std::max(0.0,iso_.neutralHadronIso() + iso_.photonIso() - 0.5*iso_.puChargedHadronIso()))/p4_.pt();
    }

    const std::vector<std::string>& matchedTrig()const{return matchedTrig_;}
    const NTLorentzVector<float> &trackP4()const{return trkP4_;}
    const bool &isPf()const{return ispf_;}

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

	////////////////////////
	/*
	 * Data members. changes here produce incompatibilities between ntuple versions!
	 */
	////////////////////////


    bool isGlobal_;
    bool isTracker_;
    bool ispf_;
    float normChi2_;
    int trkHits_,pixHits_;

    int muonHits_,matchedstations_;
    ztop::NTIsolation iso_;
    std::vector<std::string> matchedTrig_;
    NTLorentzVector<float> trkP4_;


    std::map<std::string, float> memberss_;
    std::map<int, float> members_;
};

}

#endif
