/*
 * simpleReweighter.cc
 *
 *  Created on: May 28, 2014
 *      Author: kiesej
 */

#include "../interface/simpleReweighter.h"
#include <iostream>

namespace ztop{

simpleReweighter::simpleReweighter():tmpweight_(1),unwcounter_(0),wcounter_(0),switchedoff_(false){
    //only initializer list
}

void simpleReweighter::reWeight( float &oldweight){
    if(switchedoff_) return;
    unwcounter_+= (double) oldweight;
    float newweight=oldweight*tmpweight_;
    wcounter_+=(double) newweight;
    oldweight=newweight;
}

double simpleReweighter::getRenormalization()const{
    if(switchedoff_) return 1;
    if(wcounter_!=0)return unwcounter_/wcounter_;
    else return 1;
}


}
