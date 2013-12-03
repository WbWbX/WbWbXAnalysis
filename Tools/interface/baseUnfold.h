/*
 * baseUnfold.h
 *
 *  Created on: Jul 31, 2013
 *      Author: kiesej
 *
 *      Supposed to be a class only requiring matrices as input! NO TH... container.... or anything else should be possible by adding and overloading some functions of base class
 *rethink this approach! maybe its better to only interface it to containerXD
 *
 */

#ifndef BASEUNFOLD_H_
#define BASEUNFOLD_H_

#include "../TUnfold/TUnfoldDensity.h"

namespace ztop{

class baseUnfold : public TUnfoldDensity{
	baseUnfold(){
		TH1::AddDirectory(false);
		TH1::SetDefaultSumw2();
	}
	~baseUnfold(){}

};


}

void test(){
using namespace ztop;

baseUnfold bla;
bla.AddSysError()


}


#endif /* BASEUNFOLD_H_ */
