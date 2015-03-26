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
    NTBTagSF():bTagBase(),isMC_(true),mode_(randomtagging_mode),wpval_(0){}
    ~NTBTagSF(){}

    enum modes{eventsf_mode, randomtagging_mode,shapereweighting_mode};
    void setMode(modes mode){mode_=mode;}
    modes getMode()const{return mode_;}

    void setWorkingPoint(const TString &);
    void setWorkingPointValue(const float &);

    const float & getWPDiscrValue()const;

    void fillEff(ztop::NTJet * jet, double puweight){ //!overload: jet, puweight
        if(mode_==shapereweighting_mode)
            return;
        bTagBase::fillEff(jet->pt(), fabs(jet->eta()), jet->genPartonFlavour(), jet->btag(), puweight);
    }

    void makeEffs(){if(mode_!=shapereweighting_mode) bTagBase::makeEffs();}

    bool makesEff()const{return bTagBase::getMakeEff();}
    int setSampleName(const std::string & s){
        if(mode_==shapereweighting_mode){
            std::cout << "NTBTagSF:setSampleName has no effect in shape reweighting mode" <<std::endl;
            return 1;
        }
        return bTagBase::setSampleName(getWorkingPointString()+"_"+s);
    }

    void setIsMC(bool is){isMC_=is;}

    float getNTEventWeight(const std::vector<ztop::NTJet *> &);
    /**
     * in case you only consider one jet
     */
    float getNTEventWeight( ztop::NTJet * jet);

    void changeNTJetTags( std::vector<ztop::NTJet *> &)const;
    void changeNTJetTags( std::vector<ztop::NTJet *> *)const;

    NTBTagSF  operator + (NTBTagSF  second);


    void writeToTFile(TString); //! writes whole class to TFile
    void readFromTFile(TString);  //! reads whole class from TFile
   // void readPartFromTFile(TString ident,TString file);  //! reads whole class from TFile

    void listAllSampleNames()const;

    void addInSampleNames(const TString&,const TString &);

    /**
     * returns - depending on the mode whether the variation is a "real" one depending on the mode
     */
    bool isRealSyst()const;

    void clear(){histos_.clear();effhistos_.clear();}

private:

    bool isMC_;
    modes mode_;
    float wpval_;
};
}


#endif
