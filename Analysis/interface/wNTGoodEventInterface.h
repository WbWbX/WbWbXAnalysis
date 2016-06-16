/*
 * wNTGoodEventInterface.h
 *
 *  Created on: 16 Jun 2016
 *      Author: jkiesele
 */

#ifndef TTZANALYSIS_ANALYSIS_INTERFACE_WNTGOODEVENTINTERFACE_H_
#define TTZANALYSIS_ANALYSIS_INTERFACE_WNTGOODEVENTINTERFACE_H_
#include "wNTBaseInterface.h"

namespace ztop{

class wNTGoodEventInterface: public wNTBaseInterface{
public:
	wNTGoodEventInterface(tTreeHandler * t, bool isMC);
	~wNTGoodEventInterface(){}

	const bool& pass();

private:
	wNTGoodEventInterface();
	const bool&  check(const size_t& idx);
	bool pass_;
	size_t maxidx_;
};


}


#endif /* TTZANALYSIS_ANALYSIS_INTERFACE_WNTGOODEVENTINTERFACE_H_ */
