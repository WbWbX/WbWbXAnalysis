/*
 * Unfolder.h
 *
 *  Created on: Aug 22, 2013
 *      Author: kiesej
 */

#ifndef UNFOLDER_H_
#define UNFOLDER_H_

#include "../TUnfold/TUnfoldDensity.h"
#include <iostream>
#include "TSpline.h"
#include "TH1.h"
#include "TGraph.h"
#include "TString.h"

namespace ztop{

class unfolder {
public:
	unfolder(){};
	unfolder(TString name);
	~unfolder();


	unfolder & operator = (const unfolder & rhs){copyfrom(rhs);return *this;}
	unfolder(const unfolder & rhs){copyfrom(rhs);}

	void clearmem(bool all=false);
	//for testing
	TUnfoldDensity * getTUnfold(){return unfolder_;}

	void setVerbose(bool verb=true){verb_=verb;}

	int init(TH2 * respmatrix, TH1* data,TUnfold::EHistMap mapping=TUnfold::kHistMapOutputHoriz ,TUnfold::ERegMode regmode=TUnfold::kRegModeCurvature); //creates unfold..blabla
	int scanLCurve(int steps=30);
	int scanTau(int steps=30);
//if ready
	TH1*  getUnfolded(); //returns output directly
	TH1*  getReUnfolded(); //returns output directly

	bool ready(){return ready_;}

	double tau(){return tau_;}
	double chi2a(){return chi2a_;}
	double chi2l(){return chi2l_;}
	double ndof(){return ndof_;}

	TGraph *bestLCurve(){return bestLcurve_;}
	TGraph *bestLogTauLogChi2(){return bestLogTauLogChi2_;}
	TGraph * lCurve(){return lcurve_;}
	TH1 *histMunfold(){return histMunfold_;}
	TSpline *logTauX(){return logTauX_;}
	TSpline *logTauY(){return logTauY_;}


private:

	//for future implementation

	void copyfrom(const unfolder&);


	bool verb_;

	TUnfoldDensity * unfolder_;

	TGraph *bestLcurve_;
	TGraph *bestLogTauLogChi2_;
	TGraph * lcurve_;
	TH1 *histMunfold_;
	TSpline * logTauX_, * logTauY_;


	bool init_,ready_;
	double tau_,chi2a_,chi2l_,ndof_;
	int ibest_;

	TString name_;

};


}//namespace
#endif /* UNFOLDER_H_ */
