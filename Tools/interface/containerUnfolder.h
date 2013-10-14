/*
 * containerUnfolder.h
 *
 *  Created on: Sep 4, 2013
 *      Author: kiesej
 */

#ifndef CONTAINERUNFOLDER_H_
#define CONTAINERUNFOLDER_H_
#include "container1DUnfold.h"
#include "unfolder.h"
#include "containerStack.h"

namespace ztop{

/**
 * unfolds a container1DUnfold and returns the unfolded container.
 * additionally all tau scan plots for all systematics are available
 *
 *
 * dev:
 * -name unfolder objects same as systematics
 */

class containerUnfolder{
public:
	containerUnfolder(): unfnominal_(0),usecontoptions_(true),regmode_(TUnfold::kRegModeCurvature),LCurve_(true){}
	~containerUnfolder();

	void setOptions(/* options */);
	container1D binbybinunfold(container1DUnfold & cuf);
	container1D unfold( container1DUnfold &);
	//container1DUnfold prepareFromStack(const containerStack&);

	const std::vector<unfolder * > & getSystUnfolder() const {return unfsyst_;}
	unfolder * getNominalUnfolder() const {return unfnominal_;}

	static bool printinfo;
	static bool debug;

private:
	unfolder * unfnominal_;
	std::vector<unfolder * > unfsyst_;
	bool usecontoptions_;

	TUnfold::ERegMode regmode_;
	bool LCurve_;
};

}//namespace
#endif /* CONTAINERUNFOLDER_H_ */

