/*
 * analysisPlots.cc
 *
 *  Created on: Feb 12, 2014
 *      Author: kiesej
 */

#include "../interface/analysisPlots.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"

namespace ztop{


container1DUnfold* analysisPlots::addPlot(const std::vector<float>&genbins, const std::vector<float>&recobins,
        const TString&name, const TString &xname, const TString &yname){
    container1DUnfold*newplot=new container1DUnfold(genbins,recobins,name+" step "+toTString(step_), xname, yname);
    conts_.push_back(newplot);
    return newplot;
}


}//ns
