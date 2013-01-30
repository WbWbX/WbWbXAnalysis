#include "TtZAnalysis/Tools/interface/JERBase.h"


namespace top{

  void JERBase::setSystematics(std::string type){
    resranges_.clear();
    resranges_ << 0 << 0.5 << 1.1 << 1.7 << 2.3 << 5;

    if(type=="def"){//standard
      resfactors_.clear();
      resfactors_ <<  1.052 << 1.057 << 1.096 << 1.134 << 1.288;
      std::cout << "JER set to default" << std::endl;
    }
    else if(type== "down"){
      resfactors_.clear();
      resfactors_ <<  1.115 << 1.114 << 1.162 << 1.228 << 1.488;
      std::cout << "JER set to syst down" << std::endl;
    }
    else if(type== "up"){
      resfactors_.clear();
      resfactors_ <<  0.990 << 1.001 << 1.032 << 1.042 << 1.089;
      std::cout << "JER set to syst up" << std::endl;
    }

  }

  void JERBase::correctP4(top::LorentzVector & toBeCorrectedRecoP4,const  top::LorentzVector & genP4){
    if(genP4.Pt() < 1)
      return;
    for(unsigned int i=0;i<resranges_.size();i++){
      if(fabs(toBeCorrectedRecoP4.Eta()) < resranges_.at(i)){
	if(i!=0){ //not outside range
	  double deltapt = (1. - resfactors_.at(i-1)) * (toBeCorrectedRecoP4.Pt() - genP4.Pt());
	  double scale = std::max(0., toBeCorrectedRecoP4.Pt() + deltapt)/toBeCorrectedRecoP4.Pt();
	  toBeCorrectedRecoP4 = toBeCorrectedRecoP4 * scale;
	  break;
	}
      }
    }
  }
} //namespace
