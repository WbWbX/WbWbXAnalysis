/*
 * wReweighterInterface.h
 *
 *  Created on: 30 May 2016
 *      Author: jkiesele
 */

#ifndef TTZANALYSIS_ANALYSIS_INTERFACE_WREWEIGHTERINTERFACE_H_
#define TTZANALYSIS_ANALYSIS_INTERFACE_WREWEIGHTERINTERFACE_H_

#include "wNTBaseInterface.h"
#include "simpleReweighter.h"

namespace ztop{
class wReweighterInterface: public wNTBaseInterface{
public:
	wReweighterInterface(tTreeHandler* t,
			const TString& size_branch="",
			const TString& weightsbranch="");
	~wReweighterInterface(){}

	void enable(bool en){enabled_=en;}

	/**
	 * Allows duplicate entries on purpose to apply weights twice!
	 */
	void addWeightIndex(const size_t index);
	void resetWeightIndicies(){indicies_.clear();reweighter_.clear();}

	void reWeight(float &);

	double getRenormalization()const;

private:
	wReweighterInterface():wNTBaseInterface(){}

	std::vector<size_t> indicies_;
	std::vector<simpleReweighter> reweighter_;

	bool enabled_;
};

}

#endif /* TTZANALYSIS_ANALYSIS_INTERFACE_WREWEIGHTERINTERFACE_H_ */
