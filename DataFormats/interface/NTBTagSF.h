#ifndef NTBTagSF_H
#define NTBTagSF_H
#include "TopAnalysis/ZTopUtils/interface/bTagBase.h"
#include "TtZAnalysis/DataFormats/interface/NTJet.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
//#include "mathdefs.h"
#include <iostream>

namespace ztop{

class NTBTagSF : public bTagBase{
public:
    NTBTagSF():bTagBase(),userandom_(false){}
    ~NTBTagSF(){}

    void useRandomTechnique(bool use){userandom_=use;}
    void setWorkingPoint(const TString &);

    void fillEff(ztop::NTJet * jet, double puweight){ //!overload: jet, puweight
        bTagBase::fillEff(jet->pt(), fabs(jet->eta()), jet->genPartonFlavour(), jet->btag(), puweight);
    }

    void setSystematic(const TString &);
    bool makesEff()const{return bTagBase::getMakeEff();}
    int setSampleName(const std::string & s){
        return bTagBase::setSampleName(getWorkingPointString()+"_"+s);
    }

    float getNTEventWeight(const std::vector<ztop::NTJet *> &);

    void changeNTJetTags( std::vector<ztop::NTJet *> &)const;
    void changeNTJetTags( std::vector<ztop::NTJet *> *)const;

    NTBTagSF  operator + (NTBTagSF  second);


    void writeToTFile(TString); //! writes whole class to TFile
    void readFromTFile(TString);  //! reads whole class from TFile

private:
    bool userandom_;
};
}


namespace ztop{

inline float NTBTagSF::getNTEventWeight(const std::vector<ztop::NTJet *> & jets){
    if(userandom_) return 1.;
    resetCounter();
    for(size_t i=0;i<jets.size();i++){
        ztop::NTJet *jet=jets.at(i);
        countJet(jet->pt(),fabs(jet->eta()),jet->genPartonFlavour());
    }
    return getEventSF();
}

/**
 * changes the discriminator values of the jets using random technique.
 * After this step do not expect the discriminator distrubtions in data MC to match!
 */

inline void NTBTagSF::changeNTJetTags( std::vector<ztop::NTJet *> *jets)const{
    if(!userandom_ || makesEff()) return;
    for(size_t i=0;i<jets->size();i++){

        ztop::NTJet * jet=jets->at(i);
        if(!jetIsTagged(jet->pt(),std::abs(jet->eta()),jet->genPartonFlavour(),jet->btag(),std::abs<int>(1.e6 * sin(jet->phi())))){
            if(jet->btag()>getWPDiscrValue())
                jet->setBtag(-0.1);
        }
        else if(jet->btag()<getWPDiscrValue()){
            jet->setBtag(1.1);
        }
    }
}
inline void NTBTagSF::changeNTJetTags( std::vector<ztop::NTJet *> &jets)const{
    NTBTagSF::changeNTJetTags(&jets);
}
}


#endif
