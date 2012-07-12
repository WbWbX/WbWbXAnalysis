#include "../interface/NTElectron.h"
#include <vector>
#include <iostream>
#include <algorithm>

namespace top{

  class elecRhoIsoAdder{

  public:

    elecRhoIsoAdder(){incorr_=false;cone_=0.3;};
    elecRhoIsoAdder(bool IsMC, bool use2012=true){ismc_=IsMC;use2012EA_=use2012;cone_=0.3;incorr_=false;};
    ~elecRhoIsoAdder(){};

    void setUse2012EA(bool use){use2012EA_=use;}
    void setCone(float cone){cone_=cone;}

    void setRho(double rho){rho_=rho;};

    void addRhoIso(std::vector<top::NTElectron> &);
    void addRhoIso(top::NTElectron &);


  private:

    void checkCorrectInput();

    double getRhoIso(top::NTElectron &);
    double Aeff(double);

    double rho_;
    float cone_;
    bool ismc_;
    bool use2012EA_;
    bool incorr_;

  };

  void elecRhoIsoAdder::checkCorrectInput(){
    if(!incorr_ && cone_!=0.3f) std::cout << "elecRhoIsoAdder: cone not properly set!! (" << cone_ << ")" << std::endl; 
    else incorr_=true;
  }

  double elecRhoIsoAdder::getRhoIso(top::NTElectron & elec){

    double chargedH  = elec.iso03().chargedHadronIso();
    double neutralH  = elec.iso03().neutralHadronIso();
    double photon    = elec.iso03().photonIso();

    double rhoIso=(chargedH + std::max(neutralH+photon - rho_*Aeff(elec.suClu().eta()), 0.0))/elec.pt();
    return rhoIso;
  } 


  void  elecRhoIsoAdder::addRhoIso(top::NTElectron & elec){
    checkCorrectInput();
    double rhoiso=getRhoIso(elec);
    elec.setRhoIso03(rhoiso);
  }

  void  elecRhoIsoAdder::addRhoIso(std::vector<top::NTElectron> & electrons){
    for(std::vector<top::NTElectron>::iterator elec=electrons.begin();elec<electrons.end();++elec){
      addRhoIso(*elec);
    }
  }

  double elecRhoIsoAdder::Aeff(double suclueta){
    double aeff=0;
    double eta=fabs(suclueta);
    if(cone_==0.3f){
      if(use2012EA_){
	if(ismc_){
	  //not yet provided
	  if(eta <= 1.0)                     aeff = 1;
	  if(1.0    <= eta && eta < 1.479)   aeff = 1;
	  if(1.479  <= eta && eta < 2.0)     aeff = 1;
	  if(2.0    <= eta && eta < 2.2)     aeff = 1;
	  if(2.2    <= eta && eta < 2.3)     aeff = 1;
	  if(2.3    <= eta && eta < 2.4)     aeff = 1;
	  if(2.4   <= eta)                   aeff = 1; 
	}
	else{ //data
	  //not yet provided
	  if(eta <= 1.0)                     aeff = 1;
	  if(1.0    <= eta && eta < 1.479)   aeff = 1;
	  if(1.479  <= eta && eta < 2.0)     aeff = 1;
	  if(2.0    <= eta && eta < 2.2)     aeff = 1;
	  if(2.2    <= eta && eta < 2.3)     aeff = 1;
	  if(2.3    <= eta && eta < 2.4)     aeff = 1;
	  if(2.4   <= eta)                   aeff = 1; 	  
	}
      }
      else{ // 2011 corrections
	if(ismc_){
	  if(eta <= 1.0)                     aeff = 0.110; //1
	  if(1.0    <= eta && eta < 1.479)   aeff = 0.130; //1
	  if(1.479  <= eta && eta < 2.0)     aeff = 0.089; //1
	  if(2.0    <= eta && eta < 2.2)     aeff = 0.130; //1
	  if(2.2    <= eta && eta < 2.3)     aeff = 0.150; //2
	  if(2.3    <= eta && eta < 2.4)     aeff = 0.160; //3
	  if(2.4   <= eta)                   aeff = 0.190; //3	  
	}
	else{ //data
	  if(eta <= 1.0)                     aeff = 0.100; //2
	  if(1.0    <= eta && eta < 1.479)   aeff = 0.120; //3
	  if(1.479  <= eta && eta < 2.0)     aeff = 0.085; //2
	  if(2.0    <= eta && eta < 2.2)     aeff = 0.110; //3
	  if(2.2    <= eta && eta < 2.3)     aeff = 0.120; //4
	  if(2.3    <= eta && eta < 2.4)     aeff = 0.120; //5
	  if(2.4   <= eta)                   aeff = 0.130; //6
	}
      }
    }
    else if(cone_==0.4f){
      ///////not implemented, just for future use?
    }
    return aeff;
  }


}
