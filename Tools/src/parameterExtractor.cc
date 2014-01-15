/*
 * parameterExtractor.cc
 *
 *  Created on: Jan 6, 2014
 *      Author: kiesej
 */



#include "../interface/parameterExtractor.h"
#include "TCanvas.h"
#include <iostream>
#include <stdexcept>

namespace ztop{

bool parameterExtractor::checkSizes(){
    bool check=agraphs_.size() == bgraphs_.size();
    if(!check){
        std::cout << "parameterExtractor::checkSizes: Input vectors don't match in size" << std::endl;
    }
    return check;
}
/**
 * includes range check. returns 0 if something is wrong
 */
size_t parameterExtractor::getSize(){
    if(!checkSizes())
        return 0;
    else return agraphs_.size();
}
std::vector<graph> parameterExtractor::createLikelihoods(){
    std::vector<graph> out;
    size_t size=getSize();
    for(size_t i=0;i<size;i++){
        out.push_back(createLikelihood(i));
    }
    return out;
}


//////////protected member funcs


graph parameterExtractor::createLikelihood(size_t idx){

    if(LHMode_==lh_chi2)
        return createChi2Likelihood(agraphs_.at(idx),bgraphs_.at(idx));
    else if(LHMode_==lh_chi2Swapped)
        return createChi2SwappedLikelihood(agraphs_.at(idx),bgraphs_.at(idx));

    /*  other modes  */

    std::cout << "parameterExtractor::createLikelihood: likelihood definition not found" << std::endl;
    throw std::runtime_error("parameterExtractor::createLikelihood: likelihood definition not found");
}


////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Helpers //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

void parameterExtractor::checkChi2Conditions(const graph& g,const graph& gcompare){
    if(g.getNPoints()!=gcompare.getNPoints()){
        std::cout << "parameterExtractor::checkChi2Conditions: needs same number of points" << std::endl;
        throw std::logic_error("parameterExtractor::checkChi2Conditions: needs same number of points" );
    }
    //only performing loose x check
    for(size_t i=0;i<g.getNPoints();i++){
        if(g.getPointXContent(i) != gcompare.getPointXContent(i)){
            std::cout << "parameterExtractor::checkChi2Conditions: needs same x coordinates" << std::endl;
            throw std::logic_error("parameterExtractor::checkChi2Conditions: needs same x coordinates" );
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// IMPLEMETATION OF LIKELIHOODS ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////


graph parameterExtractor::createChi2Likelihood(const graph& g,const graph& gcompare){
    checkChi2Conditions(g,gcompare);


    graph out=g;//get number of points
    out.clear();//clear
    //return out;//////////
    size_t nans=0;
    //compare systematics by systematics if present -> later
    //first ignore syst correlations
    //treat everything in g ang gcompare as uncorrelated

    for(size_t p=0;p<g.getNPoints();p++){
        float diff=g.getPointYContent(p)-gcompare.getPointYContent(p);
        float err2=0;

        float lerr=g.getPointYError(p,false);
        float rerr=gcompare.getPointYError(p,false);
        err2=lerr*lerr+rerr*rerr;

        out.setPointXContent(p,g.getPointXContent(p));
        float chi2point=diff*diff/err2;
        if(chi2point!=chi2point){
            chi2point=0;
            nans++;
        }
        out.setPointYContent(p,chi2point);
    }
    if(nans>0){
        std::cout << "parameterExtractor::createChi2Likelihood: nans created. ";
        if(nans == out.getNPoints())
            std::cout << " All chi2=0, keep in mind for ndof "<<std::endl;
        else
            std::cout << " Only some chi2=0, WARNING!!! Result will not be useful!" <<std::endl;
    }

    out.setYAxisName("#chi^{2}");
    return out;

}


graph parameterExtractor::createChi2SwappedLikelihood(const graph& g,const graph& gcompare){
    checkChi2Conditions(g,gcompare);

    graph out=g;//get number of points
    out.clear();//clear
    //return out;//////////
    size_t nans=0;
    //compare systematics by systematics if present -> later
    //first ignore syst correlations
    //treat everything as uncorrelated
    for(size_t p=0;p<g.getNPoints();p++){
        float diff=g.getPointYContent(p)-gcompare.getPointYContent(p);
        float err2=0;

        if(diff>0){ //use left down, right up
            float lerr=g.getPointYErrorDown(p,false);
            float rerr=gcompare.getPointYErrorUp(p,false);
            err2=lerr*lerr+rerr*rerr;
        }
        else{
            float lerr=g.getPointYErrorUp(p,false);
            float rerr=gcompare.getPointYErrorDown(p,false);
            err2=lerr*lerr+rerr*rerr;
        }


        out.setPointXContent(p,g.getPointXContent(p));
        float chi2point=diff*diff/err2;
        if(chi2point!=chi2point){
            chi2point=0;
            nans++;
        }
        out.setPointYContent(p,chi2point);
    }
    if(nans>0){
        std::cout << "graph::getChi2Points: nans created. ";
        if(nans == out.getNPoints())
            std::cout << " All chi2=0, keep in mind for ndof "<<std::endl;
        else
            std::cout << " Only some chi2=0, WARNING!!! Result will not be useful!" <<std::endl;
    }

    out.setYAxisName("#chi^{2}");
    return out;
}


}//namespace
