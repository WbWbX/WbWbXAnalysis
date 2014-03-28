/*
 * testfancydefines.cc
 *
 *  Created on: Mar 26, 2014
 *      Author: kiesej
 */


#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "TtZAnalysis/Analysis/interface/NTFullEvent.h"
#include "TtZAnalysis/DataFormats/src/classes.h"


#define SHOW(a) std::cout << #a << ": " << (a) << std::endl
#define TO_STREAM(stream,variable) (stream) <<#variable
#define VAR_TO_TSTRING(variable) ztop::toTString(#variable)
#include <iostream>



int main(){
    using namespace ztop;
    NTFullEvent evt;
    NTFullEvent *evtp=&evt;
    TString varstring=VAR_TO_TSTRING(evtp->adjustedmet->phi);
    std::cout << varstring << std::endl;
}
