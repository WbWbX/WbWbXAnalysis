/*
 * variateContainer1D.h
 *
 *  Created on: Aug 19, 2014
 *      Author: kiesej
 */

#ifndef VARIATECONTAINER1D_H_
#define VARIATECONTAINER1D_H_

#include "extendedVariable.h"
#include <vector>
#include "container.h"
#include <algorithm>

namespace ztop{

class variateContainer1D{
public:
	variateContainer1D(){}
	~variateContainer1D(){}

	void import(const container1D&);

	//exports for a certain set of variations
	//no syst for now!
	container1D exportContainer(const std::vector<float> & variations);

	float getBinErrUp(size_t idx){return errsup_.at(idx);}
	float getBinErrDown(size_t idx){return errsdown_.at(idx);}
	float getBinErr(size_t idx){return std::max(errsdown_.at(idx),errsup_.at(idx));}

	void setBinErrUp(size_t ,const float&);
	void setBinErrDown(size_t ,const float&);

	const std::vector<float>& bins(){return bins_;}

	size_t getNDependencies()const{if(contents_.size()>0) return contents_.at(0).getNDependencies();else return 0;}

	std::vector<TString> getSystNames(){if(contents_.size()>0) return contents_.at(0).getSystNames();else return std::vector<TString>();}

	extendedVariable * getBin(size_t idx){if(contents_.size()>0) return &contents_.at(idx);else return 0;}

	static bool debug;

private:

	std::vector<extendedVariable> contents_;
	std::vector<float> bins_;

	//this is for stat uncertainties
	std::vector<float> errsup_;
	std::vector<float> errsdown_;

};

}//ns


#endif /* VARIATECONTAINER1D_H_ */
