/*
 * wChargeFlipGenerator.h
 *
 *  Created on: 20 Jun 2016
 *      Author: jkiesele
 */

#ifndef TTZANALYSIS_ANALYSIS_INTERFACE_WCHARGEFLIPGENERATOR_H_
#define TTZANALYSIS_ANALYSIS_INTERFACE_WCHARGEFLIPGENERATOR_H_
#include  "TRandom3.h"
#include "WbWbXAnalysis/DataFormats/interface/NTLepton.h"
namespace ztop{


class wChargeFlipGenerator{
public:
	wChargeFlipGenerator(size_t seed=1234): rate_(0){
		rand_=new TRandom3(seed);
	}
	~wChargeFlipGenerator(){
		if(rand_)delete rand_;
	}
	void setRate(double r){rate_=r;}

	inline bool flip(NTLepton *lep){
		if(rate_<=0) return false;
		double dice=rand_->Uniform();
		if(dice<rate_){
			lep->setQ( - lep->q());
			return true;
		}
		return false;
	}

private:
	TRandom3 * rand_;
	double rate_;
};

}



#endif /* TTZANALYSIS_ANALYSIS_INTERFACE_WCHARGEFLIPGENERATOR_H_ */
