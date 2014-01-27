#ifndef NTJECUNCERTAINTIES_H
#define NTJECUNCERTAINTIES_H

#include "TopAnalysis/ZTopUtils/interface/JECBase.h"
#include "TtZAnalysis/DataFormats/interface/NTJet.h"


namespace ztop{

class NTJECUncertainties{

public:
    NTJECUncertainties(){}
    ~NTJECUncertainties(){}

    void setFile(std::string pathToFile){JecBase_.setFile(pathToFile);}
    void setSystematics(std::string syst){JecBase_.setSystematics(syst);} //! up, down, no
    void setIs2012(bool is){JecBase_.setIs2012(is);}

    std::vector<unsigned int> &  sources(){JecBase_.sources().clear(); return JecBase_.sources();}

    void applyToJet(ztop::NTJet * jet ){
        float recopt=jet->pt();
        float recoeta=jet->eta();
        float recophi=jet->phi();
        float recom=jet->p4().M();
        JecBase_.applyJECUncertainties(recopt,recoeta,recophi,recom);
        jet->setP4(ztop::PolarLorentzVector(recopt,recoeta,recophi,recom));
    }
    void applyToJets(std::vector<ztop::NTJet *> jets){
        for(size_t i=0;i<jets.size();i++)
            applyToJet(jets.at(i));
    }

protected:
    JECBase JecBase_;

};

}
#endif
