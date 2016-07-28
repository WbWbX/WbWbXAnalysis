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
			bool enable,
			const TString& size_branch="",
			const TString& weightsbranch="");
	~wReweighterInterface(){}


	/**
	 * Allows duplicate entries on purpose to apply weights twice!
	 */
	void addWeightIndex(const size_t index);
	void resetWeightIndicies(){indicies_.clear();reweighter_.clear();}

	void reWeight(float &);

	double getRenormalization()const;

	bool isActive()const{return enabled() && indicies_.size()>0;}

private:
	wReweighterInterface():wNTBaseInterface(){}

	std::vector<size_t> indicies_;
	std::vector<simpleReweighter> reweighter_;
	std::vector<double> scalers_;

};

}

#endif /* TTZANALYSIS_ANALYSIS_INTERFACE_WREWEIGHTERINTERFACE_H_ */
