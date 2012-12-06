#include "../DataFormats/interface/NTJet.h"
#include <vector>
#include "../Tools/interface/miscUtils.h"
#include <algorithm>
#include <iostream>

namespace top{

  class JERAdjuster{

  public:
    JERAdjuster(){setSystematics("def");}
    ~JERAdjuster(){}

    top::NTJet getCorrectedJet (top::NTJet);
    std::vector<top::NTJet> getCorrectedJets(std::vector<top::NTJet> &);
    void correctJet(std::vector<top::NTJet>::iterator );

    void setResolutionEtaRanges(std::vector<double>);
    void setResolutionFactors(std::vector<double>);

    void setSystematics(TString); //! def down up

  private:

    std::vector<double> resranges_;
    std::vector<double> resfactors_;


  };

  // // following recommendation https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetResolution;   https://twiki.cern.ch/twiki/bin/viewauth/CMS/TWikiTopRefSyst

  top::NTJet JERAdjuster::getCorrectedJet(NTJet jet){
    if(jet.genP4().Pt() < 1){
      return jet;
    }
    for(unsigned int i=0;i<resranges_.size();i++){
      if(jet.eta() < resranges_[i]){
	if(i!=0){ //not outside range
	  double deltapt = resfactors_[i-1] * (jet.pt() - jet.genP4().Pt());
	  double scale = std::max(0., jet.pt() + deltapt)/jet.pt();
	  top::LorentzVector newp4=jet.p4() * scale;
	  jet.setP4(newp4);
	}
      }
    }
    return jet;
  }
  std::vector<top::NTJet> JERAdjuster::getCorrectedJets(std::vector<NTJet> & jets){
    std::vector<NTJet> out;
    for(std::vector<NTJet>::iterator jet=jets.begin();jet<jets.end();++jet){
      out.push_back(getCorrectedJet(*jet));
    }
    return out;
  }
  void JERAdjuster::correctJet(std::vector<top::NTJet>::iterator jet){
    top::LorentzVector zero(0,0,0,0);
    if(jet->genP4().Pt() > 1){
      for(unsigned int i=0;i<resranges_.size();i++){
	if(jet->eta() < resranges_[i]){
	  if(i!=0){ //not outside range
	    double deltapt = (1. - resfactors_[i-1]) * (jet->pt() - jet->genP4().Pt());
	    double scale = std::max(0., jet->pt() + deltapt)/jet->pt();
	    top::LorentzVector newp4=jet->p4() * scale;
	    jet->setP4(newp4);
	  }
	}
      }
    }
  }


  void JERAdjuster::setSystematics(TString type){
    resranges_.clear();
    resranges_ << -5 << -2.3 << -1.7 << -1.1 << -0.5 << 0 << 0.5 << 1.1 << 1.7 << 2.3 << 5;

    if(type=="def"){//standard
      resfactors_.clear();
      resfactors_ << 1.288 << 1.134 << 1.096 << 1.057 << 1.052 << 1.052 << 1.057 << 1.096 << 1.134 << 1.288;
      std::cout << "JER set to default" << std::endl;
    }
    else if(type== "down"){
      resfactors_.clear();
      resfactors_ << 1.428 << 1.228 << 1.162 << 1.114 << 1.115 << 1.115 << 1.114 << 1.162 << 1.228 << 1.428;
      std::cout << "JER set to syst down" << std::endl;
    }
    else if(type== "up"){
      resfactors_.clear();
      resfactors_ << 1.029 << 1.042 << 1.032 << 1.000 << 0.999 << 0.999 << 1.000 << 1.032 << 1.042 << 1.029;
      std::cout << "JER set to syst up" << std::endl;
    }

  }


}
