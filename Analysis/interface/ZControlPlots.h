/*
 * ZControlPlots.h
 *
 *  Created on: Nov 1, 2013
 *      Author: kiesej
 */

#ifndef ZControlPlots_H_
#define ZControlPlots_H_

#include "controlPlotBasket.h"

namespace ztop{

class ZControlPlots : public controlPlotBasket{
public:
	ZControlPlots():controlPlotBasket(){}
	~ZControlPlots(){}

	void makeControlPlots(const size_t& step);


};
}//namespace

#endif /* ZControlPlots_H_ */
