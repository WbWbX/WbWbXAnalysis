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
class TCanvas;

namespace ztop{

/**
 * NO PLOTTING!!!
 */

class parameterExtractor {
public:

    enum likelihoodModes{lh_chi2,lh_chi2Swapped};

    parameterExtractor(): LHMode_(lh_chi2){}
    ~parameterExtractor(){}

    // defaults should suffice

    // parameterExtractor(const parameterExtractor&);
    // parameterExtractor & operator = (const parameterExtractor&);


    void setInputA(const std::vector<graph>& vg){agraphs_=vg;} //graphs already come with para information; graph per bin
    void setInputB(const std::vector<graph>& vg){bgraphs_=vg;}

    void setInputA(const graph &g){agraphs_.clear();agraphs_.push_back(g);} //for one bin (just internally clears and pushes back)
    void setInputB(const graph &g){bgraphs_.clear();bgraphs_.push_back(g);}


    bool checkSizes();

    ////

    size_t getSize();



    ////

    void setLikelihoodMode(likelihoodModes lhm){LHMode_=lhm;}

    std::vector<graph> createLikelihoods();


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

    /* ..... */



};
}//ns



#endif /* PARAMETEREXTRACTOR_H_ */
