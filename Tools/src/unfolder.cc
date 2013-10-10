/*
 * unfolder.cc
 *
 *  Created on: Aug 22, 2013
 *      Author: kiesej
 */
#include "../interface/unfolder.h"
#include <TError.h>
#include <iostream>
#include "TSpline.h"
#include "TH1.h"
#include "TGraph.h"
#include "TString.h"
#include "TCanvas.h"

/**
 * takes care that all output histograms are NOT divided by binwidth (what about UF/OF?)
 *
 */

namespace ztop{

unfolder::unfolder(TString name): verb_(false),unfolder_(0),bestLcurve_(0),bestLogTauLogChi2_(0),lcurve_(0),histMunfold_(0),logTauX_(0),logTauY_(0),
		init_(false),ready_(false),tau_(0),chi2a_(0),chi2l_(0),ndof_(0),ibest_(0),name_(name) {


}
unfolder::~unfolder(){
	clearmem(true);
}

void unfolder::copyfrom(const unfolder & rhs){
	    verb_=rhs.verb_;
		ready_=false;
		init_=false;
}

/**
 * clearmem(bool all=false)
 * clears all pointers. if all=false, the initialised TUnfold object remains intact
 */
void unfolder::clearmem(bool all){
	if(all && unfolder_) delete unfolder_;

	if(bestLcurve_) delete bestLcurve_;
	if(bestLogTauLogChi2_) delete bestLogTauLogChi2_;
	if(lcurve_) delete lcurve_;
	if(histMunfold_) delete histMunfold_;
	if(logTauX_) delete logTauX_;
	if(logTauY_) delete logTauY_;
}

int unfolder::init(TH2 * respmatrix, TH1* data, TUnfold::EHistMap mapping, TUnfold::ERegMode regmode){
	TH1::AddDirectory(false);
	ready_=false;
	init_=false;
	unfolder_ = new TUnfoldDensity(respmatrix,mapping,regmode);
	int verb=unfolder_->SetInput(data);

	if(verb>=10000){
		return verb;
	}
	init_=true;
	return verb;
}
int unfolder::scanLCurve(int nScan){
	if(!init_){
		std::cout << "unfolder::scanLCurve: init successfully first" << std::endl;
		return -1;
	}
	Double_t tauMin=0.0;
	Double_t tauMax=0.0;
	clearmem(false); //don't kill TUnfold object

	int oldInfoLevel=gErrorIgnoreLevel;
	if(!verb_){
		//switch off info stuff
		gErrorIgnoreLevel = kWarning;
	}
	ibest_=unfolder_->ScanLcurve(nScan,tauMin,tauMax,&lcurve_,&logTauX_,&logTauY_);

	tau_=  unfolder_->GetTau();
	chi2a_=unfolder_->GetChi2A();
	chi2l_=unfolder_->GetChi2L();
	ndof_= unfolder_->GetNdf();

	Double_t t[1],x[1],y[1];
	logTauX_->GetKnot(ibest_,t[0],x[0]);
	logTauY_->GetKnot(ibest_,t[0],y[0]);
	bestLcurve_=new TGraph(1,x,y);
	bestLogTauLogChi2_=new TGraph(1,t,x);

	if(!verb_){
		//switch info stuff back to normal
		gErrorIgnoreLevel = oldInfoLevel;
	}

	ready_=true;
	return 0;

}
int unfolder::scanTau(int nScan){
	std::cout << "unfolder::scanTau: not implemented yet" << std::endl;
	//ibest_=unfolder_->ScanTau();
	return -1;
}

TH1*  unfolder::getUnfolded(){
	if(!init_ || !ready_)
		return 0;
	TH1::AddDirectory(false);
	TH1 * h=unfolder_->GetOutput(name_+"_unfolded");
	multiplyByBinWidth(h);
	return h;
}
TH1*  unfolder::getRefolded(){
	if(!init_ || !ready_)
		return 0;
	TH1::AddDirectory(false);
	return unfolder_->GetFoldedOutput(name_+"_refolded");
}

TCanvas * unfolder::drawControlHistos() const{ //needs to be done
	TCanvas * c = new TCanvas();
	return c;
}

void unfolder::multiplyByBinWidth(TH1 *h) const{
	return;
	for(int bin=1;bin<h->GetNbinsX();bin++){
		double cont=h->GetBinContent(bin)*(h->GetBinWidth(bin));
		double err=h->GetBinError(bin)*(h->GetBinWidth(bin));
		h->SetBinContent(bin, cont);
		h->SetBinError(bin, err);
	}
}

}//namespace


