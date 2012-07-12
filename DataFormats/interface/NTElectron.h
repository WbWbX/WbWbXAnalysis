#ifndef NTElectron_h
#define NTElectron_h


#include "Math/GenVector/LorentzVector.h"
#include "NTLepton.h"
#include "NTIsolation.h"
#include "NTSuClu.h"
#include <vector>
#include <string>

#include <utility>

namespace top{



  class NTElectron : public NTLepton {
  public:

    explicit NTElectron(){q_=0;rhoIso03_=-99;};
    ~NTElectron(){};
  //sets
  void setDbs(double Dbs){dbs_=Dbs;}
  void setNotConv(bool IsNotConv){isNotConv_=IsNotConv;}
  void setId(std::vector<std::pair<std::string,float> > Ids , bool keepMvaOnly=true){

    ids_=Ids;
    mvaId_=id("mvaTrigV0");
    if(keepMvaOnly){
      ids_.clear();
    }
  }
  void setIso03(top::NTIsolation Iso03){iso03_=Iso03;}
  void setRhoIso03(double rhoIso){rhoIso03_=rhoIso;}
  //void setIso04(top::NTIsolation Iso04){iso04_=Iso04;}

  void setMatchedTrig(std::vector<std::string> MatchedTrig){matchedTrig_=MatchedTrig;}

  void setSuClu(NTSuClu SuCluin){suClu_=SuCluin;}

  //gets
  double dbs(){return dbs_;}
  bool isNotConv(){return isNotConv_;}
  std::vector<std::pair<std::string,float> > ids(){return ids_;}
  top::NTIsolation iso03(){return iso03_;}
  //top::NTIsolation iso04(){return iso04_;}

  double isoVal03(){
    return (iso03_.chargedHadronIso() + std::max(0.0,iso03_.neutralHadronIso() + iso03_.photonIso() - 0.5*iso03_.puChargedHadronIso()))/p4_.pt();
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
  double rhoIso03(){return rhoIso03_;}
  float mvaId(){ return mvaId_;}

  std::vector<std::string> matchedTrig(){return matchedTrig_;}
  NTSuClu suClu(){return suClu_;}

 protected:
  float mvaId_;
  top::NTSuClu suClu_;
  double dbs_;
  bool isNotConv_;
  std::vector<std::pair<std::string,float> > ids_;
  top::NTIsolation iso03_;
  double rhoIso03_;
  //top::NTIsolation iso04_;

  std::vector<std::string> matchedTrig_;

};

}

#endif
