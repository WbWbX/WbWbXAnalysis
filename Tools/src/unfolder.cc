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

unfolder::unfolder(TString name):  verb_(false),unfolder_(0),bestLcurve_(0),bestLogTauLogChi2_(0),bestRhoLogTau_(0),
		lcurve_(0),histMunfold_(0),logTauX_(0),logTauY_(0),rhoLogTau_(0),
		histGlobalCorrScan_(0),knots_(0),
		init_(false),ready_(false),tau_(0),chi2a_(0),chi2l_(0),ndof_(0),ibest_(0),name_(name) {
}
unfolder::~unfolder(){
	clearmem(true,true);
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
void unfolder::clearmem(bool all,bool respm){
	if(bestLcurve_){
		delete bestLcurve_;bestLcurve_=0;
	}
	if(bestLogTauLogChi2_){
		delete bestLogTauLogChi2_;bestLogTauLogChi2_=0;
	}
	if(bestRhoLogTau_){
		delete bestRhoLogTau_;bestRhoLogTau_=0;
	}
	if(lcurve_){
		delete lcurve_;lcurve_=0;
	}
	if(logTauX_){
		delete logTauX_;logTauX_=0;
	}
	if(logTauY_){
		delete logTauY_;logTauY_=0;
	}
	if(rhoLogTau_){
		delete rhoLogTau_;rhoLogTau_=0;
	}
	if(histGlobalCorrScan_){
		delete histGlobalCorrScan_;histGlobalCorrScan_=0;
	}
	if(knots_){
		delete knots_;knots_=0;
	}
	if(respm && histMunfold_){
		delete histMunfold_;histMunfold_=0;
	}
	if(all && unfolder_){
		delete unfolder_;histMunfold_=0;
	}
	ready_=false;
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

	if(ready_){
		std::cout << "unfolder::scanLCurve: Scan already performed, redoing scan" <<std::endl;
		clearmem(false,false);
	}

	Double_t tauMin=0.0;
	Double_t tauMax=0.0;
	//clearmem(false); //don't kill TUnfold object

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
	float normchi2=(unfolder_->GetChi2A()+unfolder_->GetChi2L())/unfolder_->GetNdf();

	std::cout<< name_ << ": chi**2="<<unfolder_->GetChi2A()<<"+"<<unfolder_->GetChi2L()
		    								  <<" / "<<unfolder_->GetNdf()<<"\t=" << normchi2<<  std::endl;


	ready_=true;
	return 0;


}
int unfolder::scanTau(int nScan){
	if(!init_){
		std::cout << "unfolder::scanTau: init successfully first" << std::endl;
		return -1;
	}

	if(ready_){
		std::cout << "unfolder::scanTau: Scan already performed, redoing scan" <<std::endl;
		clearmem(false,false);
	}

	int oldInfoLevel=gErrorIgnoreLevel;
	if(!verb_){
		//switch off info stuff
		gErrorIgnoreLevel = kWarning;
	}
	ibest_=unfolder_->ScanTau(nScan,0.,0.,&rhoLogTau_,
			TUnfoldDensity::kEScanTauRhoSquareAvg,
			0,0,
			&lcurve_);

	Double_t t[1],rho[1],x[1],y[1];
	rhoLogTau_->GetKnot(ibest_,t[0],rho[0]);
	lcurve_->GetPoint(ibest_,x[0],y[0]);
	bestRhoLogTau_=new TGraph(1,t,rho);
	bestLcurve_=new TGraph(1,x,y);
	Double_t *tAll=new Double_t[nScan],*rhoAll=new Double_t[nScan];
	for(Int_t i=0;i<nScan;i++) {
		rhoLogTau_->GetKnot(i,tAll[i],rhoAll[i]);
	}
	knots_=new TGraph(nScan,tAll,rhoAll);

	float normchi2=(unfolder_->GetChi2A()+unfolder_->GetChi2L())/unfolder_->GetNdf();

		std::cout<< name_ << ": chi**2="<<unfolder_->GetChi2A()<<"+"<<unfolder_->GetChi2L()
			    								  <<" / "<<unfolder_->GetNdf()<<"\t=" << normchi2<<  std::endl;

	histGlobalCorrScan_=unfolder_->GetRhoItotal("histGlobalCorrScan",0,0,0,kFALSE);

	if(!verb_){
		//switch info stuff back to normal
		gErrorIgnoreLevel = oldInfoLevel;
	}
	ready_=true;
	delete [] rhoAll;
	delete [] tAll;
	return 0;
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


void unfolder::drawLCurve(){
	if(!init_ || !ready_)
		return;
	if(!lcurve_ || bestRhoLogTau_) //tau is ok here
		return;
	lcurve_->Draw("AL");
	bestLcurve_->SetMarkerColor(kRed);
	bestLcurve_->Draw("*");

}
void unfolder::drawTau(){
	if(!init_ || !ready_)
		return;
	if(logTauX_){ //lcurve scan
		logTauX_->Draw();
		bestLogTauLogChi2_->SetMarkerColor(kRed);
		bestLogTauLogChi2_->Draw("*");
	}
	else if(bestRhoLogTau_){//min glob corr
		rhoLogTau_->Draw();
		bestRhoLogTau_->SetMarkerColor(kRed);
		bestRhoLogTau_->Draw("*");
	}
}
/*rhoLogTau->Draw();
  knots_->Draw("*");
  bestRhoLogTau->SetMarkerColor(kRed);
  bestRhoLogTau->Draw("*");
 */


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


