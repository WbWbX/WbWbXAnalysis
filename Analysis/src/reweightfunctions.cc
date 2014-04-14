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

float reweightfunctions::getWeight(const float& in) const{
    if(type_ == toppt){
        if(syst_ == nominal)
            return TMath::Exp(0.156-0.00137*in);
        if(syst_ == up)
            return std::max(0.,1+(2.*(TMath::Exp(0.156-0.00137*in)-1)));
        if(syst_ == down)
            return 1;
    }
    else if(type_==flat){
        return 1;
    }

    return 1;
}

float reweightfunctions::getWeight(const float& in1,const float& in2) const{
    float j=in1;
    j=in2;
    j++;
    throw std::logic_error("reweightfunctions::getWeight(const float& in1,const float& in2): not implemented yet");
    return 1;
}


}
