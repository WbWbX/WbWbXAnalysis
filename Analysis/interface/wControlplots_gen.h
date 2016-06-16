/*
 * wControlplots_gen.h
 *
 *  Created on: 15 Jun 2016
 *      Author: jkiesele
 */

#ifndef TTZANALYSIS_ANALYSIS_SRC_WCONTROLPLOTS_GEN_H_
#define TTZANALYSIS_ANALYSIS_SRC_WCONTROLPLOTS_GEN_H_

#include "controlPlotBasket.h"

namespace ztop{

class wControlplots_gen : public controlPlotBasket{
public:
	wControlplots_gen():controlPlotBasket(){}
	~wControlplots_gen(){}


private:
	void makeControlPlotsPriv();



};



}



#endif /* TTZANALYSIS_ANALYSIS_SRC_WCONTROLPLOTS_GEN_H_ */
