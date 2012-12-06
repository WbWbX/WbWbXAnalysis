#include "../interface/JECUncertainties.h"


namespace top{

  JECUncertainties::~JECUncertainties(){
    if(totalunc_) delete totalunc_;
      for(unsigned int i=0;i<vsrc_.size();i++){
	if(vsrc_[i]) delete vsrc_[i];
      }
    }

  JECUncertainties::JECUncertainties(const JECUncertainties& junc){
    filename_=junc.filename_;
    setFile(junc.filename_,false);
    nomupdown_=junc.nomupdown_;
    sources_=junc.sources_;
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


  void JECUncertainties::setFile(TString filename, bool newset){
    if(filename != ""){
      const int nsrc = 16;
      const char* srcnames[nsrc] =
	{"Absolute",       //0
	 "HighPtExtra",    //1
	 "SinglePion",     //2
	 "Flavor",         //3
	 "Time",           //4
	 "RelativeJEREC1", //5
	 "RelativeJEREC2", //6
	 "RelativeJERHF",  //7
	 "RelativeStatEC2",//8 
	 "RelativeStatHF", //9
	 "RelativeFSR",    //10
	 "PileUpDataMC",   //11
	 "PileUpOOT",      //12
	 "PileUpPt",       //13
	 "PileUpBias",     //14 
	 "PileUpJetRate"}; //15
      
      if(newset) std::cout << "setting JEC uncertainties file to: " << filename << std::endl;
      std::ifstream check_file(filename.Data());
      if(check_file.good()){

	for (int isrc = 0; isrc < nsrc; isrc++) {
	  
	  const char *name = srcnames[isrc];
	  JetCorrectorParameters *p = new JetCorrectorParameters(filename.Data(), name);
	JetCorrectionUncertainty *unc = new JetCorrectionUncertainty(*p);
	vsrc_.push_back(unc);
	} 
	
	// Total uncertainty
	totalunc_ = new JetCorrectionUncertainty(*(new JetCorrectorParameters(filename.Data(), "Total")));
	ninit_=false;
	
	filename_=filename;
      }
      else{
	std::cout << "JECUncertainties::setFile: File "<< filename <<" does not exist! quitting!" << std::endl;
	std::exit(EXIT_FAILURE);
      }
    }
  }
  
  void JECUncertainties::applyJECUncertainties(std::vector<top::NTJet>::iterator jet){
    if(nomupdown_ < 0){ //down
	jet->setP4( jet->p4() * (1 - getUncertainty(jet->eta(), jet->pt(), false)) );
      }
    else if(nomupdown_ > 0){ //up
	jet->setP4( jet->p4() * (1 + getUncertainty(jet->eta(), jet->pt(), true)) );
      }
      else{
      }
    }

  void JECUncertainties::applyJECUncertainties(top::NTJet &jet){
    if(nomupdown_ < 0){ //down
      jet.setP4( jet.p4() * (1 - getUncertainty(jet.eta(), jet.pt(), false)) );
      }
    else if(nomupdown_ > 0){ //up
      jet.setP4( jet.p4() * (1 + getUncertainty(jet.eta(), jet.pt(), true)) );
    }
    else{
    }
  }
  
  double JECUncertainties::getUncertainty(double eta, double pt, bool up){
    double dunc=0;
    if(!ninit_){
      if(sources_.size() == 0){ //total
	totalunc_->setJetPt(pt);
	totalunc_->setJetEta(eta);
	dunc= totalunc_->getUncertainty(up);
      }
      else if(sources_.size() <= vsrc_.size()){
	for(unsigned int i=0;i<sources_.size();i++){
	  if(sources_[i] < vsrc_.size()){ //for a spec source
	    JetCorrectionUncertainty *unc = vsrc_[sources_[i]];
	    unc->setJetPt(pt);
	    unc->setJetEta(eta);
	    dunc=sqrt(pow(dunc,2)+pow(unc->getUncertainty(up),2));
	  }
	  else{
	    std::cout << "JECUncertainty: wrong source number. Must be below " << vsrc_.size()-1 << "." << std::endl;
	  }
	}
      }
      else{
	std::cout << "JECUncertainty: too many sources; must be below " << vsrc_.size()-1 << "." << std::endl;
      }  
    }
    else{
      std::cout << "JECUncertainty: no file set!" << std::endl;
    }
    return dunc;
  }
  
}
    