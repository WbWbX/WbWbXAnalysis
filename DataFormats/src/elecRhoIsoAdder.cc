#include "../interface/elecRhoIsoAdder.h"

namespace ztop{


  void elecRhoIsoAdder::checkCorrectInput(){
    if(!incorr_ && cone_!=0.3f) std::cout << "elecRhoIsoAdder: cone not properly set!! (" << cone_ << ")" << std::endl; 
    else incorr_=true;
  }

  float elecRhoIsoAdder::getRhoIso(ztop::NTElectron & elec){

    float chargedH  = elec.iso().chargedHadronIso();
    float neutralH  = elec.iso().neutralHadronIso();
    float photon    = elec.iso().photonIso();

    float rhoIso=(chargedH + std::max(neutralH+photon - rho_*Aeff(elec.suClu().eta()), (float) 0.0))/elec.pt();
    return rhoIso;
  } 

 

  void  elecRhoIsoAdder::addRhoIso(ztop::NTElectron & elec){
    checkCorrectInput();
    float rhoiso=getRhoIso(elec);
    elec.setRhoIso(rhoiso);
  }

  void  elecRhoIsoAdder::addRhoIso(std::vector<ztop::NTElectron> & electrons){
    for(std::vector<ztop::NTElectron>::iterator elec=electrons.begin();elec<electrons.end();++elec){
      addRhoIso(*elec);
    }
  }


 

  float elecRhoIsoAdder::Aeff(float suclueta){
    float aeff=0;
    float eta=fabs(suclueta);
    if(cone_==0.3f){
      if(use2012EA_){
	if(ismc_ && 1==0){ //always false
	  //not yet provided right now the same values as for data
	  if(eta <= 1.0)                     aeff = 0.13;
	  if(1.0    <= eta && eta < 1.479)   aeff = 0.14;
	  if(1.479  <= eta && eta < 2.0)     aeff = 0.07;
	  if(2.0    <= eta && eta < 2.2)     aeff = 0.09;
	  if(2.2    <= eta && eta < 2.3)     aeff = 0.11;
	  if(2.3    <= eta && eta < 2.4)     aeff = 0.11;
	  if(2.4   <= eta)                   aeff = 0.14; 
	}
	else{ //data
	  if(eta <= 1.0)                     aeff = 0.13;
	  if(1.0    <= eta && eta < 1.479)   aeff = 0.14;
	  if(1.479  <= eta && eta < 2.0)     aeff = 0.07;
	  if(2.0    <= eta && eta < 2.2)     aeff = 0.09;
	  if(2.2    <= eta && eta < 2.3)     aeff = 0.11;
	  if(2.3    <= eta && eta < 2.4)     aeff = 0.11;
	  if(2.4   <= eta)                   aeff = 0.14; 
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
