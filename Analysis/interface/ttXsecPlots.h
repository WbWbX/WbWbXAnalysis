/*
 * ttXsecPlots.h
 *
 *  Created on: Oct 16, 2014
 *      Author: kiesej
 */

#ifndef TTXSECPLOTS_H_
#define TTXSECPLOTS_H_



#include "ttbarControlPlots.h"

namespace ztop{

class ttXsecPlots : public ttbarControlPlots{
public:
	ttXsecPlots():ttbarControlPlots(){}
	~ttXsecPlots(){}

	void makeControlPlots(const size_t& step);



};
}//namespace


#endif /* TTXSECPLOTS_H_ */
