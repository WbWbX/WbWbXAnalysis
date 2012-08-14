#ifndef JECUncertainties_h
#define JECUncertainties_h
//#include "gROOT.h"


#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "CondFormats/JetMETObjects/src/Utilities.cc"
//#include "CondFormats/JetMETObjects/src/JetCorrectorParameters.cc"
//#include "CondFormats/JetMETObjects/src/SimpleJetCorrectionUncertainty.cc"
//#include "CondFormats/JetMETObjects/src/JetCorrectionUncertainty.cc"
#include <vector>

#include "../DataFormats/interface/NTJet.h"
#include "TString.h"

#include <iostream>

namespace top{

  class JECUncertainties{

  public:
    
    JECUncertainties(){ninit_=true;source_=100;filename_=0;}
    JECUncertainties(const JECUncertainties &);
    JECUncertainties(const char * filename){ninit_=true;source_=100;setFile(filename);filename_=filename;}
    ~JECUncertainties();

    void setSource(int source){source_=source;}
    void setVariation(TString);
    void setFile(const char * );
    
    void applyJECUncertainties(std::vector<top::NTJet>::iterator );

    JECUncertainties & operator = (const JECUncertainties &);

  private:
    std::vector<JetCorrectionUncertainty*> vsrc_;
    JetCorrectionUncertainty* totalunc_;
    const char * filename_;
    int nomupdown_;
    int source_;
    bool ninit_;

    double getUncertainty(unsigned int, double, double , bool);


  };


  JECUncertainties::~JECUncertainties(){
    if(totalunc_) delete totalunc_;
      for(unsigned int i=0;i<vsrc_.size();i++){
	if(vsrc_[i]) delete vsrc_[i];
      }
    }

  JECUncertainties::JECUncertainties(const JECUncertainties& junc){
    filename_=junc.filename_;
    if(filename_ && *filename_ != '\0') setFile(junc.filename_);
    nomupdown_=junc.nomupdown_;
    source_=junc.source_;
    ninit_=junc.ninit_;
  }

  JECUncertainties & JECUncertainties::operator = (const JECUncertainties & junc){
    *this =  JECUncertainties(junc);
    return *this;
  }

  void JECUncertainties::setVariation(TString var){
    if(var=="up"){
      nomupdown_=1;
      std::cout << "JECUncertainties: Variation set to up" << std::endl;
    }
    else if(var=="down"){
      nomupdown_=-1;
      std::cout << "JECUncertainties: Variation set to down" << std::endl;
    }
    else{
      nomupdown_=0;
      std::cout << "JECUncertainties: Variation set to none" << std::endl;
    }
  }


  void JECUncertainties::setFile(const char * filename){
    if(*filename != '\0'){
      const int nsrc = 16;
      const char* srcnames[nsrc] =
	{"Absolute", "HighPtExtra", "SinglePion", "Flavor", "Time",
	 "RelativeJEREC1", "RelativeJEREC2", "RelativeJERHF",
	 "RelativeStatEC2", "RelativeStatHF", "RelativeFSR",
	 "PileUpDataMC", "PileUpOOT", "PileUpPt", "PileUpBias", "PileUpJetRate"};
      
      std::cout << "setting JEC uncertainties file to: " << filename << std::endl;

      for (int isrc = 0; isrc < nsrc; isrc++) {
	
	const char *name = srcnames[isrc];
	JetCorrectorParameters *p = new JetCorrectorParameters(filename, name);
	JetCorrectionUncertainty *unc = new JetCorrectionUncertainty(*p);
	vsrc_.push_back(unc);
      } 
      
      // Total uncertainty
      totalunc_ = new JetCorrectionUncertainty(*(new JetCorrectorParameters(filename, "Total")));
      ninit_=false;
    }
  }
  
  void JECUncertainties::applyJECUncertainties(std::vector<top::NTJet>::iterator jet){
      if(nomupdown_ < 0){
	jet->setP4( jet->p4() * (1 + getUncertainty(source_,jet->eta(), jet->pt(), false)) );
      }
      else if(nomupdown_ > 0){
	jet->setP4( jet->p4() * (1 + getUncertainty(source_,jet->eta(), jet->pt(), true)) );
      }
      else{
      }
    }

  double JECUncertainties::getUncertainty(unsigned int src, double eta, double pt, bool up){
    double dunc=0;
    if(!ninit_){
      if(src == 100){ //total
	totalunc_->setJetPt(pt);
	totalunc_->setJetEta(eta);
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
  
}
    
#endif
