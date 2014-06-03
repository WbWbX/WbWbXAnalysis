/*
 * ttbarXsecExtractor.h
 *
 *  Created on: May 23, 2014
 *      Author: kiesej
 */

#ifndef TTBARXSECEXTRACTOR_H_
#define TTBARXSECEXTRACTOR_H_


#include "TtZAnalysis/Tools/interface/extendedVariable.h"
#include "TtZAnalysis/Tools/interface/containerStackVector.h"
#include "TString.h"

namespace ztop{
class graph;
/**
 * This class is supposed to be very specialized and provides the functionality and the
 * main function to extract the ttbar cross section using a likelihood approach
 * incorporating the "atlas method" (as used in CONF-2013-097) in a likelihood
 * and systematic variations as nuisance parameters
 * -> simultaneous fit of all
 *
 *
 * all variations will be expressed in terms of sigma, (so from -1 to +1 with 0 begin no variation)
 */
class ttbarXsecExtractor{
public:
    ttbarXsecExtractor();
    ~ttbarXsecExtractor(){}

    /**
     * This assumes that ALL syst uncertainties except for lumi are already present
     */
    void readInput(const containerStackVector & csv, const TString& stackname);
    void setLumi(float lum){done_=false;lumi_=lum;}
    /**
     * input error in %, e.g. 2.5
     */
    void setLumiError(float lumerr){done_=false;lumierr_=lumerr/100;}

    void setIsPseudoData(bool is){pseudodata_=is;}

    void setMinimizerStr(TString min){mainminimzstr_=min;}
    void setMinimizerAlgoStr(TString algo){mainminmzalgo_=algo;}

  //  void setNGenerated(const float & ngen){done_=false;ngenevents_=ngen;}

    /**
     * main function to be called after configuration
     */
    void extract();

    float getXsec()const;
    float getXsecErrUp()const;
    float getXsecErrDown()const;

    const std::vector< std::vector<graph> > & getSystGraphs()const{return systdeps_;}
    const std::vector<TString> & getSystNames(){return  sysnames_;}

     std::vector< extendedVariable* >  getAllExtendedVars()const{return varpointers_;}

     const simpleFitter* getFitter()const{return &fitter_;}

    double getChi2(const double *);

private:
    bool setup_,done_;
    //lumi and its error (relative "-1")
    float lumi_,lumierr_;
    float ngenevents_;

    ///output
    float xsec_,xsecerrup_,xsecerrdown_;


    std::vector< std::vector<graph> > systdeps_; //just a bunch for future reference
    std::vector<TString> sysnames_;

    /*
     * A lot of variables
     * This part can get messy, try to keep clean
     */
    extendedVariable Lumi_, eps_emu_, eps_b_, C_b_, N_bkg1_, N_bkg2_;

    //this is not needed but consistent in case of fictional "data systematics"
    extendedVariable n_data1_,n_data2_;


    std::vector< extendedVariable* > varpointers_;

    simpleFitter fitter_;


    size_t xsecidx_,eps_bidx_;

    float xsecoffset_;

    float minchi2_;

    bool pseudodata_;

    TString mainminimzstr_,mainminmzalgo_;

    //helper functions
    float sq(const float& a)const{return a*a;}
    float sq(const double& a)const{return a*a;}

};


}



#endif /* TTBARXSECEXTRACTOR_H_ */
