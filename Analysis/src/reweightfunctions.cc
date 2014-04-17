/*
 * reweightfunctions.cc
 *
 *  Created on: Dec 18, 2013
 *      Author: kiesej
 */

#include "TMath.h"
#include "../interface/reweightfunctions.h"
#include <stdexcept>
#include <algorithm>

namespace ztop{

float reweightfunctions::reWeight(const float& in,const float& previousweight) {
    if(type_==flat){
        return previousweight;
    }

    float newweight=1;

    if(type_ == toppt){
        if(syst_ == nominal)
            newweight= TMath::Exp(0.156-0.00137*in);
        if(syst_ == up)
            newweight= std::max(0.,1+(2.*(TMath::Exp(0.156-0.00137*in)-1)));
        if(syst_ == down)
            newweight= previousweight;
    }
    unwcounter_+=previousweight;
    wcounter_+=newweight*previousweight;

    return newweight*previousweight;
}

float reweightfunctions::reWeight(const float& in1,const float& in2,const float& previousweight) {
    if(type_==flat){
        return previousweight;
    }
    float newweight=1;

    ////
    float j=in1;
    j=in2;
    j++;
    throw std::logic_error("reweightfunctions::getWeight(const float& in1,const float& in2): not implemented yet");
    /////

    unwcounter_+=previousweight;
    wcounter_+=newweight*previousweight;

    return newweight*previousweight;
}


}
