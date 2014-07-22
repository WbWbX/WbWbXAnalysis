/*
 * parameterExtractor.h
 *
 *  Created on: Jan 6, 2014
 *      Author: kiesej
 */

#ifndef PARAMETEREXTRACTOR_H_
#define PARAMETEREXTRACTOR_H_

#include "graph.h"
#include "tObjectList.h"
#include "TString.h"
#include <vector>
#include "TF1.h"

class TCanvas;

namespace ztop{

/**
 * NO PLOTTING!!!
 */

class parameterExtractor {
public:

    enum likelihoodModes{lh_chi2,lh_chi2Swapped,lh_fit,lh_fitintersect};
    /**
     * fitunc_stat(un)corrpossion uses the expected statistics as poisson and folds it with uncertainties from the fit
     * These are also assumed poissonian
     * all distributions are generalized to continuity
     * only apply this mode to ONE of the distributions (the one that is your prediction ~ prior)
     */
    enum fituncertaintyModes{fitunc_statcorrgaus, fitunc_statuncorrgaus, fitunc_statcorrpoisson,fitunc_statuncorrpoisson};


    parameterExtractor(): LHMode_(lh_chi2),tmpfa_(0),tmpfb_(0),granularity_(300),clfit_(0.68),
            fituncmodea_(fitunc_statuncorrgaus),fituncmodeb_(fitunc_statuncorrgaus),poisson_(false){}
    ~parameterExtractor(){}

    // defaults should suffice

    // parameterExtractor(const parameterExtractor&);
    // parameterExtractor & operator = (const parameterExtractor&);




    void setInputA(const std::vector<graph>& vg){agraphs_=vg;} //graphs already come with para information; graph per bin
    void setInputB(const std::vector<graph>& vg){bgraphs_=vg;}

    void setInputA(const graph &g){agraphs_.clear();agraphs_.push_back(g);} //for one bin (just internally clears and pushes back)
    void setInputB(const graph &g){bgraphs_.clear();bgraphs_.push_back(g);}


    void setFitUncertaintyModeA(fituncertaintyModes uncmode){fituncmodea_=uncmode;}
    void setFitUncertaintyModeB(fituncertaintyModes uncmode){fituncmodeb_=uncmode;}


    bool checkSizes();

    ////

    size_t getSize();



    ////

    void setLikelihoodMode(likelihoodModes lhm){LHMode_=lhm;}
    likelihoodModes getLikelihoodMode()const{return LHMode_;}

    std::vector<graph> createLikelihoods();

    void setUsePoisson(bool use){poisson_=use;}


    void setFitFunctionA(const TString& ffc){fitfunctiona_=ffc;}
    void setFitFunctionB(const TString& ffc){fitfunctionb_=ffc;}
    void setFitFunctions(const TString& ffc){fitfunctionb_=ffc;fitfunctiona_=ffc;}

    /*
    const std::vector<std::vector<TF1* > >&  getFitFunctionA()const{return fittedFunctionsa_;}
    const std::vector<std::vector<TF1* > >&  getFitFunctionB()const{return fittedFunctionsb_;}
     */

    const std::vector<graph> & getFittedGraphsA()const{return fittedgraphsa_;}
    const std::vector<graph> & getFittedGraphsB()const{return fittedgraphsb_;}

    void setIntersectionGranularity(size_t grn){granularity_=grn;}

    void setConfidenceLevelFitInterval(const float & cl){clfit_=cl;}

    static bool debug;

protected:
    /* */
    graph createLikelihood(size_t ); //for one graph, not directly size protected


private:

    std::vector<graph> agraphs_,bgraphs_;
    likelihoodModes LHMode_;


    ///helper functions, throw except if something wrong
    void checkChi2Conditions(const graph&,const graph&);

    ////implementation of likelihoods for one graph
    graph createChi2Likelihood(const graph&,const graph&);
    graph createChi2SwappedLikelihood(const graph&,const graph&);

    ///////// intersection part ///

    /**
     * will just return graph with one point and errors
     * the fitted functions will be put into the last references:
     * first: graph for fit a, second: graph for fit b
     */
    graph createIntersectionLikelihood(const graph& grapha,const graph& graphb,
            graph& fitteda,graph& fittedb);


    /**
     * only temps
     */
    TF1* tmpfa_,*tmpfb_;

    /**
     * for further printout
     */
    /*
    std::vector<std::vector<TF1* > > fittedFunctionsa_;
    std::vector<std::vector<TF1* > > fittedFunctionsb_;
     */
    std::vector<graph> fittedgraphsa_,fittedgraphsb_;

    TString fitfunctiona_,fitfunctionb_;
    size_t granularity_;
    float clfit_;
    //std::vector<>

    fituncertaintyModes fituncmodea_,fituncmodeb_;

    bool poisson_;

    double findintersect(TF1* a, TF1* b,float min, float max);

    /* ..... */

    /**
     * extnorm is the integral (not the inverse integral) of the distribution you want to get
     */
    double lnNormedGaus(const float & centre, const float& width, const float& evalpoint, const float& extnorm=0)const;
    double chi_square(const float & centre, const float& widthsquared, const float& evalpoint) const;
    // double mcLnPoisson(const float & centreN, const float & mcstat2, const float& evalpoint)const;


};
}//ns



#endif /* PARAMETEREXTRACTOR_H_ */
