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
    NTBTagSF():bTagBase(),userandom_(false),isMC_(true){}
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

    void setIsMC(bool is){isMC_=is;}

    float getNTEventWeight(const std::vector<ztop::NTJet *> &);

    void changeNTJetTags( std::vector<ztop::NTJet *> &)const;
    void changeNTJetTags( std::vector<ztop::NTJet *> *)const;

    NTBTagSF  operator + (NTBTagSF  second);


    void writeToTFile(TString); //! writes whole class to TFile
    void readFromTFile(TString);  //! reads whole class from TFile

private:
    bool userandom_;
    bool isMC_;
};
}


#endif
