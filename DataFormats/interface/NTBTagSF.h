#ifndef NTBTagSF_H
#define NTBTagSF_H
#include "TopAnalysis/ZTopUtils/interface/bTagSFBase.h"
#include "TtZAnalysis/DataFormats/interface/NTJet.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
//#include "mathdefs.h"
#include <iostream>

namespace ztop{

class NTBTagSF : public bTagSFBase{
public:
    NTBTagSF():bTagSFBase(),isMC_(true),mode_(randomtagging_mode){}
    ~NTBTagSF(){}

    enum modes{randomtagging_mode,shapereweighting_mode};
    void setMode(modes mode){mode_=mode;}
    modes getMode()const{return mode_;}



    void fillEff(ztop::NTJet * jet,const float& puweight){ //!overload: jet, puweight
        bTagSFBase::fillEff(jet->pt(), jet->eta(), jet->genPartonFlavour(), jet->btag(), puweight);
    }

    bool makesEff()const{return bTagSFBase::getMakeEff();}


    void setIsMC(bool is){isMC_=is;}


    void changeNTJetTags( std::vector<ztop::NTJet *> &)const;
    void changeNTJetTags( std::vector<ztop::NTJet *> *)const;


    void writeToFile(const std::string&); //! creates and writes histograms
    void readFromFile(const std::string&);  //! reads histograms


private:
    NTBTagSF(const ztop::NTBTagSF&rhs):bTagSFBase(),isMC_(true),mode_(randomtagging_mode){}
    NTBTagSF& operator=(const ztop::NTBTagSF&rhs){return *this;}

    bool isMC_;
    modes mode_;
};
}


#endif
