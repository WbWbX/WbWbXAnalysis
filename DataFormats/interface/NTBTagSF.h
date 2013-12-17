#ifndef NTBTagSF_H
#define NTBTagSF_H
#include "TopAnalysis/ZTopUtils/interface/bTagBase.h"
#include "TtZAnalysis/DataFormats/interface/NTJet.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"

namespace ztop{

class NTBTagSF : public bTagBase{
public:
    NTBTagSF():bTagBase(){}
    ~NTBTagSF(){}

    void fillEff(ztop::NTJet * jet, double puweight){ //!overload: jet, puweight
        bTagBase::fillEff(jet->pt(), fabs(jet->eta()), jet->genPartonFlavour(), jet->btag(), puweight);
    }

    void setSystematic(const TString &);
    bool makesEff(){return bTagBase::getMakeEff();}

    float getNTEventWeight(const std::vector<ztop::NTJet *> &);

    NTBTagSF  operator + (NTBTagSF  second);


    void writeToTFile(TString, std::string treename="stored_objects"); //! writes whole class to TFile
    void readFromTFile(TString , std::string treename="stored_objects");  //! reads whole class from TFile


};
}


namespace ztop{

inline float NTBTagSF::getNTEventWeight(const std::vector<ztop::NTJet *> & jets){
    resetCounter();
    for(size_t i=0;i<jets.size();i++){
        ztop::NTJet *jet=jets.at(i);
        countJet(jet->pt(),fabs(jet->eta()),jet->genPartonFlavour());
    }
    return getEventSF();
}
}


#endif
