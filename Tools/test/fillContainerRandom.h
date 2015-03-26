/*
 * fillContainerRandom.h
 *
 *  Created on: Jun 17, 2014
 *      Author: kiesej
 */

#ifndef FILLCONTAINERRANDOM_H_
#define FILLCONTAINERRANDOM_H_

#include "../interface/histo1D.h"
#include "TRandom.h"

enum en_functions{func_gaus,func_binomial,func_breitwigner,func_exp};

void fillRandom(ztop::histo1D * c, en_functions func, float para0, float para1, size_t ntimes){
    using namespace ztop;

    TRandom * r = new TRandom();

    for(size_t i=0;i<ntimes;i++){
        if(func==func_gaus)
            c->fill(r->Gaus(para0,para1));
        else if(func==func_binomial)
            c->fill(r->Binomial(para0,para1));
        else if(func==func_breitwigner)
            c->fill(r->BreitWigner(para0,para1));
        else if(func==func_exp)
            c->fill(r->Exp(para0));

    }




}


#endif /* FILLCONTAINERRANDOM_H_ */
