/*
 * observable.h
 *
 *  Created on: Oct 9, 2013
 *      Author: kiesej
 */

#ifndef OBSERVABLE_H_
#define OBSERVABLE_H_
#include "histoContent.h"

namespace ztop{
/**
 * generic observable class that can handle stat, systematic uncertainties, ...
 * based on histocontent
 */
class observable{
public:
	observable();
	~observable();


private:
	histoContent content_;

};
}//namespace


#endif /* OBSERVABLE_H_ */
