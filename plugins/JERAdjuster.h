#include "../DataFormats/interface/NTJet.h"
#include <vector>
#include "miscUtils.h"
#include <algorithm>

namespace top{

  class JERAdjuster{

  public:
    JERAdjuster(){setSystematics(0);}
    ~JERAdjuster(){}

    void recalculate(top::NTJet &);
    void recalculate(std::vector<top::NTJet> &);

    void setResolutionEtaRanges(std::vector<double>);
    void setResolutionFactors(std::vector<double>);

    void setSystematics(int); //!0: default, -1:down, 1: up

  private:

    std::vector<double> resranges_;
    std::vector<double> resfactors_;


  };

  // // following recommendation https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetResolution

  void JERAdjuster::recalculate(NTJet & jet){
    top::LorentzVector zero(0,0,0,0);
    if(jet.genP4() != zero){
      for(unsigned int i=0;i<resranges_.size();i++){
	if(jet.eta() < resranges_[i]){
	  if(i!=0){ //not outside range
	    double factor = std::max(0.,(double)(jet.genP4().Pt() + resfactors_[i-1] * (jet.pt() - jet.genP4().Pt())));
	    top::LorentzVector newp4=jet.p4() * factor;
	    jet.setP4(newp4);
	  }
	}
      }
    }
  }
  void JERAdjuster::recalculate(std::vector<NTJet> & jets){
    for(std::vector<NTJet>::iterator jet=jets.begin();jet<jets.end();++jet){
      recalculate(*jet);
    }
  }
  void JERAdjuster::setSystematics(int type){
    resranges_.clear();
    resranges_ << -5 << -2.3 << -1.7 << -1.1 << -0.5 << 0 << 0.5 << 1.1 << 1.7 << 2.3 << 5;

    if(type==0i){//standard
      resfactors_.clear();
      resfactors_ << 1.288 << 1.134 << 1.096 << 1.057 << 1.052 << 1.052 << 1.057 << 1.096 << 1.134 << 1.288;
    }
    else if(type== -1i){
      resfactors_.clear();
      resfactors_ << 1.428 << 1.228 << 1.162 << 1.114 << 1.115 << 1.115 << 1.114 << 1.162 << 1.228 << 1.428;
    }
    else if(type== 1i){
      resfactors_.clear();
      resfactors_ << 1.029 << 1.042 << 1.032 << 1.000 << 0.999 << 0.999 << 1.000 << 1.032 << 1.042 << 1.029;
    }

  }


}
