#ifndef NTElectron_h
#define NTElectron_h


#include "NTLepton.h"
#include "NTIsolation.h"
#include "NTSuClu.h"
#include "NTLorentzVector.h"
#include <vector>
#include <string>

#include <utility>
#include <map>

namespace ztop{



class NTElectron : public NTLepton {
public:

    explicit NTElectron(){q_=0;rhoIso_=-99;};
    ~NTElectron(){};
    //sets
    void setNotConv(bool IsNotConv){isNotConv_=IsNotConv;}
    void setECalP4(NTLorentzVector<float> ECalp4){ecalp4_=ECalp4;}
    void setECalP4(D_LorentzVector p4In){ecalp4_=NTLorentzVector<float>(p4In.pt(),p4In.Eta(),p4In.Phi(),p4In.M());}
    void setId(std::vector<std::pair<std::string,float> > Ids , std::string keeponly="mvaTrigV0"){

        ids_=Ids;
        mvaId_=id(keeponly);
        if(keeponly.length()>0){
            ids_.clear();
        }
    }
    void setIso(ztop::NTIsolation Iso){iso_=Iso;}
    void setRhoIso(float RhoIso){rhoIso_=RhoIso;}
    //void setIso04(ztop::NTIsolation Iso04){iso04_=Iso04;}

    void setMatchedTrig(std::vector<std::string> MatchedTrig){matchedTrig_=MatchedTrig;}

    void setSuClu(NTSuClu SuCluin){suClu_=SuCluin;}
    void setMHits(int mhits){mHits_=mhits;}
    void setIsPf(bool is){ispf_=is;}

    //gets
    const NTLorentzVector<float>  &ECalP4()const{return ecalp4_;}
    const bool &isNotConv()const{return isNotConv_;}
    const std::vector<std::pair<std::string,float> > &ids()const{return ids_;}
    const ztop::NTIsolation &iso()const{return iso_;}
    //ztop::NTIsolation iso04(){return iso04_;}


    float isoVal(){
        return (iso_.chargedHadronIso() + std::max(0.0,iso_.neutralHadronIso() + iso_.photonIso() - 0.5*iso_.puChargedHadronIso()))/p4_.Pt();
    }

    float id(std::string idName) {
        float idtemp=-9999;
        for(unsigned int i=0; i<ids_.size(); i++){
            if(idName == ids_[i].first){
                idtemp=ids_[i].second;
                break;
            }
        }
        return idtemp;
    }
    //float rhoIso(){return rhoIso_;}
    const float & rhoIso() const {return rhoIso_;}
    const float & storedId() const{ return mvaId_;}

    const std::vector<std::string> & matchedTrig() const {return matchedTrig_;}
    const NTSuClu & suClu()const{return suClu_;}
    const int & mHits()const{return mHits_;}
    const bool & isPf() const{return ispf_;}


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

    float mvaId_;
    ztop::NTSuClu suClu_;
    bool isNotConv_,ispf_;
    std::vector<std::pair<std::string,float> > ids_;
    ztop::NTIsolation iso_;
    float rhoIso_;
    NTLorentzVector<float> ecalp4_;
    //ztop::NTIsolation iso04_;
    int mHits_;

    std::vector<std::string> matchedTrig_;

    std::map<std::string, float> memberss_;
    std::map<int, float> members_;

};

}

#endif
