/*
 * wControlPlots.h
 *
 *  Created on: 30 May 2016
 *      Author: jkiesele
 */

#ifndef TTZANALYSIS_ANALYSIS_INTERFACE_WCONTROLPLOTS_H_
#define TTZANALYSIS_ANALYSIS_INTERFACE_WCONTROLPLOTS_H_

#include "controlPlotBasket.h"


namespace ztop{

class wControlPlots : public controlPlotBasket{
public:
	wControlPlots():controlPlotBasket(){}
	~wControlPlots(){}


private:
	void makeControlPlotsPriv();



};
}//namespace



#endif /* TTZANALYSIS_ANALYSIS_INTERFACE_WCONTROLPLOTS_H_ */
