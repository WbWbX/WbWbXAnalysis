/*
 * Unfolder.h
 *
 *  Created on: Aug 22, 2013
 *      Author: kiesej
 */

#ifndef UNFOLDER_H_
#define UNFOLDER_H_

#include "../TUnfold/TUnfoldDensity.h"

class TCanvas;
class TGraph;
class TSpline;
class TH1;
class TH2;

namespace ztop{

class unfolder {
public:
	unfolder() : verb_(false),unfolder_(0),bestLcurve_(0),bestLogTauLogChi2_(0),lcurve_(0),histMunfold_(0),logTauX_(0),logTauY_(0),
	init_(false),ready_(false),tau_(0),chi2a_(0),chi2l_(0),ndof_(0),ibest_(0),name_(""){}
	unfolder(TString name);
	~unfolder();

	void setName(TString name){name_=name;}
	const TString & getName() const {return name_;}

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
	TH1*  getRefolded(); //returns output directly

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

	/* functions like:
	 *
	 * -drawLCurve ...
	 */
	/**
	 * draws lcurve, tauscan, refolded vs input
	 */
	TCanvas * drawControlHistos() const;


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

	void multiplyByBinWidth(TH1 *)const;

};


}//namespace
#endif /* UNFOLDER_H_ */
