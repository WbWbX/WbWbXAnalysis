/*
 * histoUnfolder.h
 *
 *  Created on: Sep 4, 2013
 *      Author: kiesej
 */

#ifndef HISTOUNFOLDER_H_
#define HISTOUNFOLDER_H_
#include "histo1DUnfold.h"
#include "unfolder.h"
#include "histoStack.h"

namespace ztop{

/**
 * unfolds a histo1DUnfold and returns the unfolded container.
 * additionally all tau scan plots for all systematics are available
 *
 *
 * dev:
 * -name unfolder objects same as systematics
 */

class histoUnfolder{
public:
	enum scanModes{LCurve,minCorr};

	histoUnfolder(): unfnominal_(0),usecontoptions_(true),regmode_(TUnfold::kRegModeCurvature),scanmode_(minCorr){}
	~histoUnfolder();

	void clear();

	void setOptions(/* options */);
	void setScanMode(scanModes scmd){scanmode_=scmd;}
	void setRegMode(TUnfold::ERegMode regmode){regmode_=regmode;}
	void setIgnoreSyst(std::vector<TString> ign){ignoresyst_=ign;}
	histo1D binbybinunfold(histo1DUnfold & cuf);
	histo1D unfold( histo1DUnfold &);
	//histo1DUnfold prepareFromStack(const histoStack&);

	const std::vector<unfolder * > & getSystUnfolder() const {return unfsyst_;}
	unfolder * getNominalUnfolder() const {return unfnominal_;}

	static bool printinfo;
	static bool debug;

private:
	unfolder * unfnominal_;
	std::vector<unfolder * > unfsyst_;
	bool usecontoptions_;

	TUnfold::ERegMode regmode_;
	scanModes scanmode_;
	std::vector<TString> ignoresyst_;
};

}//namespace
#endif /* CONTAINERUNFOLDER_H_ */

