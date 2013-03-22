#ifndef NTElectron_h
#define NTElectron_h


#include "NTLepton.h"
#include "NTIsolation.h"
#include "NTSuClu.h"
#include <vector>
#include <string>

#include <utility>
#include <map>

namespace top{



  class NTElectron : public NTLepton {
  public:

    explicit NTElectron(){q_=0;rhoIso_=-99;};
    ~NTElectron(){};
    //sets
    void setNotConv(bool IsNotConv){isNotConv_=IsNotConv;}
    void setECalP4(LorentzVector ECalp4){ecalp4_=ECalp4;}
    void setId(std::vector<std::pair<std::string,float> > Ids , bool keepMvaOnly=true){

      ids_=Ids;
      mvaId_=id("mvaTrigV0");
      if(keepMvaOnly){
	ids_.clear();
      }
    }
    void setIso(top::NTIsolation Iso){iso_=Iso;}
    void setRhoIso(double rhoIso){rhoIso_=rhoIso;}
    //void setIso04(top::NTIsolation Iso04){iso04_=Iso04;}

    void setMatchedTrig(std::vector<std::string> MatchedTrig){matchedTrig_=MatchedTrig;}

    void setSuClu(NTSuClu SuCluin){suClu_=SuCluin;}
    void setMHits(int mhits){mHits_=mhits;}
    void setIsPf(bool is){ispf_=is;}

    //gets
    PolarLorentzVector ECalP4(){return ecalp4_;}
    bool isNotConv(){return isNotConv_;}
    std::vector<std::pair<std::string,float> > ids(){return ids_;}
    top::NTIsolation iso(){return iso_;}
    //top::NTIsolation iso04(){return iso04_;}


    double isoVal(){
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
    double rhoIso(){return rhoIso_;}
    float mvaId(){ return mvaId_;}

    std::vector<std::string> matchedTrig(){return matchedTrig_;}
    NTSuClu suClu(){return suClu_;}
    int mHits(){return mHits_;}
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
    float mvaId_;
    top::NTSuClu suClu_;
    bool isNotConv_,ispf_;
    std::vector<std::pair<std::string,float> > ids_;
    top::NTIsolation iso_;
    double rhoIso_;
    PolarLorentzVector ecalp4_;
    //top::NTIsolation iso04_;
    int mHits_;

    std::vector<std::string> matchedTrig_;

    std::map<std::string, double> memberss_;
    std::map<int, double> members_;

  };

}

#endif
