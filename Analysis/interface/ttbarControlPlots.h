/*
 * ttbarControlPlots.h
 *
 *  Created on: Nov 1, 2013
 *      Author: kiesej
 */

#ifndef TTBARCONTROLPLOTS_H_
#define TTBARCONTROLPLOTS_H_

#include "controlPlotBasket.h"

namespace ztop{

class ttbarControlPlots : public controlPlotBasket{
public:
	ttbarControlPlots():controlPlotBasket(){}
	~ttbarControlPlots(){}

	void makeControlPlots(const size_t& step);


};
}//namespace

#endif /* TTBARCONTROLPLOTS_H_ */
