#ifndef JECUncertainties_h
#define JECUncertainties_h
//#include "gROOT.h"


#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include <vector>

#include "../DataFormats/interface/NTJet.h"
#include "TString.h"

#include <iostream>

namespace top{

  class JECUncertainties{

private:
    std::vector<JetCorrectionUncertainty*> vsrc_;
    JetCorrectionUncertainty* totalunc_;


    bool init_;

    double getUncertainty(unsigned int src, double eta, double pt, bool up){
      double dunc=0;
      if(init_){
	if(src == 100){ //total
	  dunc= totalunc_->getUncertainty(up);
	}
	else if(src < vsrc_.size()){ //for a spec source
	  JetCorrectionUncertainty *unc = vsrc_[src];
	  unc->setJetPt(pt);
	  unc->setJetEta(eta);
	  dunc=unc->getUncertainty(up);
	}
	else{
	  std::cout << "JECUncertainty: wrong source number. Must be below " << vsrc_.size() << "." << std::endl;
	}
      }
      else{
	std::cout << "JECUncertainty: no file set!" << std::endl;
      }
      return dunc;
    }

  public:
    
    JECUncertainties(){
      init_=false;
    }
    JECUncertainties(const char * filename){
      setFile(filename);
    }
    ~JECUncertainties(){
      delete totalunc_;
      for(unsigned int i=0;i<vsrc_.size();i++){
	delete vsrc_[i];
      }
    }
    
    void setFile(const char * filename){
      
      const int nsrc = 16;
      const char* srcnames[nsrc] =
	{"Absolute", "HighPtExtra", "SinglePion", "Flavor", "Time",
	 "RelativeJEREC1", "RelativeJEREC2", "RelativeJERHF",
	 "RelativeStatEC2", "RelativeStatHF", "RelativeFSR",
	 "PileUpDataMC", "PileUpOOT", "PileUpPt", "PileUpBias", "PileUpJetRate"};
      
      
      for (int isrc = 0; isrc < nsrc; isrc++) {
	
	const char *name = srcnames[isrc];
	JetCorrectorParameters *p = new JetCorrectorParameters(filename, name);
	JetCorrectionUncertainty *unc = new JetCorrectionUncertainty(*p);
	vsrc_.push_back(unc);
      } 
    
      // Total uncertainty
      totalunc_ = new JetCorrectionUncertainty(*(new JetCorrectorParameters(filename, "Total")));
      init_=true;
    }
    
    
    
    double uncertaintyUp(top::NTJet & jet,int source=100){
      return getUncertainty(source,jet.eta(), jet.pt(), true);
    }
    
    double uncertaintyDown(top::NTJet & jet,int source=100){
      return getUncertainty(source,jet.eta(), jet.pt(), false);
    }
    
    void scaleJetEnergyUp(top::NTJet & jet, int source=100){
      jet.setP4(jet.p4() * (uncertaintyUp(jet, source)));
    }

    void scaleJetEnergyDown(top::NTJet & jet, int source=100){
      jet.setP4(jet.p4() * (uncertaintyDown(jet, source)));
    }  
  };

}
    
#endif
