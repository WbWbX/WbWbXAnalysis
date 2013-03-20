#ifndef NTJECUNCERTAINTIES_H
#define NTJECUNCERTAINTIES_H

#include "TtZAnalysis/Tools/interface/JECBase.h"
#include "TtZAnalysis/DataFormats/interface/NTJet.h"


namespace top{

  class NTJECUncertainties{

  public:
    NTJECUncertainties(){}
    ~NTJECUncertainties(){}

    void setFile(std::string pathToFile){JecBase_.setFile(pathToFile);}
    void setSystematics(std::string syst){JecBase_.setSystematics(syst);} //! up, down, no
    void setIs2012(bool is){JecBase_.setIs2012(is);}

    std::vector<unsigned int> &  sources(){JecBase_.sources().clear(); return JecBase_.sources();}

    void applyToJet(top::NTJet * jet ){
      top::PolarLorentzVector p4in = jet->p4();
      JecBase_.applyJECUncertainties(p4in);
      jet->setP4(p4in);
    }
    void applyToJets(std::vector<top::NTJet *> jets){
      for(size_t i=0;i<jets.size();i++)
	applyToJet(jets.at(i));
    }

  protected:
    JECBase JecBase_;

  };

}
#endif
