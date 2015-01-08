/*
 * simpleFitter.cc
 *
 *  Created on: May 22, 2014
 *      Author: kiesej
 */

#include "../interface/simpleFitter.h"
#include <TROOT.h>
#include <TMinuit.h>
#include "Minuit2/MnStrategy.h"
#include <iostream>
#include "Math/Functor.h"
#include "Math/Factory.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include <Minuit2/MnMachinePrecision.h>
#include "TtZAnalysis/Tools/interface/container2D.h"
#include "Math/Minimizer.h"
#include "Math/MinimizerOptions.h"
#include <stdexcept>
#include "Minuit2/Minuit2Minimizer.h"
#include "TMinuitMinimizer.h"
#include "Math/GSLMinimizer.h"
#include "Math/GSLNLSMinimizer.h"
#include "Math/GSLSimAnMinimizer.h"
#include <limits>
#include <cfloat>
//use TMinuit
//define all the functions - nee dto be c-stylish



namespace ztop{



simpleFitter::simpleChi2::simpleChi2():fp_(0),npars_(0){}
simpleFitter::simpleChi2::simpleChi2(simpleFitter * fp):fp_(fp) {npars_=fp_->getParameters()->size();}
double simpleFitter::simpleChi2::operator() (const double* par) const {
	if(!fp_)
		throw std::out_of_range("simpleChi2: no fitter associated");
	double chisq = 0;
	for (size_t i=0;i<fp_->getNomPoints()->size(); i++) {
		// chi square is the quadratic sum of the distance from the point to the function weighted by its error
		double delta  = (fp_->getNomPoints()->at(i).y
				-fp_->fitfunction(fp_->getNomPoints()->at(i).x,par));
		double err=1;
		if(delta>0)
			err=fp_->getErrsUp()->at(i).y;
		else
			err=fp_->getErrsDown()->at(i).y;

		if(err!=0)
			delta/=err;
		else
			continue;
		chisq += delta*delta;
	}
	return chisq;
}
double simpleFitter::simpleChi2::Up() const { return 1.; }


int simpleFitter::printlevel=0;
bool simpleFitter::debug=false;

simpleFitter::simpleFitter():fitmode_(fm_pol0),minimizer_(mm_minuitMinos),maxcalls_(4e8),
		requirefitfunction_(true),minsuccessful_(false),minossuccessful_(false),tolerance_(0.1),functobemin_(0),
		algorithm_(""),minimizerstr_("Minuit2"),pminimizer_(0),chi2func_(this),strategy_(2){

	setFitMode(fitmode_);
}

simpleFitter::~simpleFitter(){
	if(pminimizer_){
		delete pminimizer_;
		pminimizer_=0;
	}
}

simpleFitter::simpleFitter(const simpleFitter& rhs){
	copyFrom(rhs);
}
simpleFitter& simpleFitter::operator=(const simpleFitter& rhs){
	copyFrom(rhs);
	return *this;
}

size_t simpleFitter::hasNParameters(fitmodes mode)const{
	size_t npars=0;
	if(mode==fm_pol0)
		npars=1;
	if(mode==fm_pol1)
		npars=2;
	if(mode==fm_pol2)
		npars=3;
	if(mode==fm_pol3)
		npars=4;


	if(mode==fm_gaus)
		npars=3;
	if(mode==fm_offgaus)
		npars=4;

	return npars;

}

void simpleFitter::setFitMode(fitmodes fitmode){
	fitmode_=fitmode;
	size_t npars=hasNParameters(fitmode);

	paras_.resize(npars,0);
	stepsizes_.resize(npars,0.001);
	paraerrsup_.resize(npars,0);
	paraerrsdown_.resize(npars,0);
	parafixed_.resize(npars,false);
	std::vector<double> dummy;
	dummy.resize(paras_.size(),0);
	paracorrs_.clear();
	paracorrs_.resize(paras_.size(),dummy);

}
void simpleFitter::setMinFunction( ROOT::Math::Functor* f){
	functobemin_= f;
}
void simpleFitter::setParameters(const std::vector<double>& inpars,const std::vector<double>& steps){
	paras_=inpars;
	if(steps.size()>0)
		stepsizes_=steps;
	paraerrsup_.resize(paras_.size(),0);
	paraerrsdown_.resize(paras_.size(),0);
	parafixed_.resize(paras_.size(),false);
	std::vector<double> dummy;
	dummy.resize(paras_.size(),0);
	paracorrs_.clear();
	paracorrs_.resize(paras_.size(),dummy);
}

void simpleFitter::setParameter(size_t idx,double value){
	if(idx >= paras_.size())
		throw std::out_of_range("simpleFitter::setParameter: index out of range");
	paras_.at(idx)=value;
}
void simpleFitter::setParameterFixed(size_t idx,bool fixed){
	if(idx >= paras_.size())
		throw std::out_of_range("simpleFitter::setParameterFixed: index out of range");
	parafixed_.at(idx)=fixed;
}

void simpleFitter::setAsMinosParameter(size_t parnumber,bool set){
	std::vector<int>::iterator it=std::find(minospars_.begin(),minospars_.end(),parnumber);
	size_t pos=it-minospars_.begin();
	if(set){
		if(pos >= minospars_.size())
			minospars_.push_back(parnumber);
	}
	else{
		if(pos>=minospars_.size()){
			std::cout << "simpleFitter::setAsMinosParameter: WARNING trying to unset but was never set" <<std::endl;}
		else if(minospars_.size()>0){
			std::cout << "removing " << minospars_.at(pos) << " from minos " << std::endl;
			minospars_.erase(it);
			std::cout << "remaining parameters:";
			for(size_t i=0;i<minospars_.size();i++)
				std::cout << " " << minospars_.at(i);
			std::cout << std::endl;
		}
	}
}

double simpleFitter::getFitOutput(const double& xin)const{
	return fitfunction(xin,&(paras_.at(0)));
}
void simpleFitter::feedErrorsToSteps(){
	for(size_t i=0;i<stepsizes_.size();i++)
		stepsizes_.at(i)=paraerrsup_.at(i)*0.05; //good enough
}

void simpleFitter::fit(){

	if(!checkSizes()){
		std::cout << "ERROR IN simpleFitter::fit" <<std::endl;

		std::cout << "parameter:             " << paras_.size() << std::endl;
		std::cout << "parameter errors up:   " << paraerrsup_.size() << std::endl;
		std::cout << "parameter errors down: " << paraerrsdown_.size() << std::endl;
		std::cout << "parameter steps size:  " << stepsizes_.size() << std::endl;
		std::cout << "\nOnly if important when fitting to points ("<< requirefitfunction_ <<"): "  << std::endl;
		std::cout << "points:            " << nompoints_.size() << std::endl;
		std::cout << "point errors up:   " << errsup_.size() << std::endl;
		std::cout << "point errors down: " << errsdown_.size() << std::endl;

		throw std::runtime_error("simpleFitter::fit: number of parameters<->stepsizes or parameters<->fitfunction ");
		//  return;
	}

	if(printlevel>0)
		std::cout << "simpleFitter::fit(): fitting "  << paras_.size() <<  " parameters." <<std::endl;

	minsuccessful_=false;

	//  if(functobemin_)
	if(pminimizer_){
		delete pminimizer_;
		pminimizer_=0;
	}

	if(minimizer_==mm_minuit2 || minimizer_==mm_minuitMinos){
		ROOT::Math::Minimizer * min=invokeMinimizer();
		ROOT::Math::Functor fcn(chi2func_,paras_.size());
		if(functobemin_){
			min->SetFunction(*functobemin_);
		}
		else{
			min->SetFunction(fcn);
		}
		minsuccessful_=min->Minimize();
		const double *xs = min->X();
		const double * errs=min->Errors();
		//feed back
		if(minospars_.size()>0)
			minossuccessful_=true;
		for(size_t i=0;i<paras_.size();i++){
			paras_.at(i)=xs[i];
			TString paraname="";
			if(paranames_.size()>i)
				paraname=paranames_.at(i);
			else
				paraname=(TString)"par"+toTString(i);
			if(std::find(minospars_.begin(),minospars_.end(),i) != minospars_.end()){

				if(!min->GetMinosError(i, paraerrsdown_.at(i), paraerrsup_.at(i))){
					paraerrsdown_.at(i)=errs[i];
					paraerrsup_.at(i)=errs[i];
					minossuccessful_=false;
				}
			}
			else{
				paraerrsdown_.at(i)=errs[i];
				paraerrsup_.at(i)=errs[i];
			}
			if(printlevel>0){
				TString someblanks;
				if(paraname.Length()<20){
					size_t add=20-paraname.Length();
					for(size_t k=0;k<add;k++)
						someblanks+=" ";
				}
				if(paras_.at(i)>0)
					someblanks+=" ";
				if(printlevel>-1)
					std::cout << paraname+" "  << someblanks << paras_.at(i)<< " +"<< paraerrsup_.at(i) << " " << paraerrsdown_.at(i)<< std::endl;
			}
		}
		chi2min_=min->MinValue();
		std::vector<double> dummy;
		dummy.resize(paras_.size(),0);
		paracorrs_.clear();
		paracorrs_.resize(paras_.size(),dummy);
		for(size_t i=0;i<paras_.size();i++){
			for(size_t j=0;j<paras_.size();j++){
				paracorrs_.at(i).at(j)=min->Correlation(i,j);
				//  std::cout << i << " " <<j << paracorrs_.at(i).at(j) <<std::endl;
			}
		}
		pminimizer_ = min;
	}
}

const double& simpleFitter::getParameter(size_t idx)const{
	if(idx>=paras_.size())
		throw std::out_of_range("simpleFitter::getParameter: index out of range");

	return paras_.at(idx);
}

double simpleFitter::getParameterErr(size_t idx)const{
	if(idx>=paraerrsup_.size())
		throw std::out_of_range("simpleFitter::getParameterErr: index out of range");
	if(paraerrsdown_.size()!=paraerrsup_.size())
		throw std::out_of_range("simpleFitter::getParameterErr: serious problem errors up/down not of same size");

	double err=fabs(paraerrsup_.at(idx));
	if(err<fabs(paraerrsdown_.at(idx)))err=fabs(paraerrsdown_.at(idx));
	return err;
}


const double& simpleFitter::getCorrelationCoefficient(size_t i, size_t j)const{
	if(i>=paracorrs_.size())
		throw std::out_of_range("simpleFitter::getCorrelationCoefficient: first index out of range");
	if(j>=paracorrs_.at(i).size())
		throw std::out_of_range("simpleFitter::getCorrelationCoefficient: second index out of range");

	return paracorrs_.at(i).at(j);

}


void simpleFitter::fillCorrelationCoefficients(container2D * c)const{
	if(!c)
		return;
	std::vector<float> bins;
	for(size_t i=0;i<paracorrs_.size()+1;i++)
		bins.push_back(i);
	c->setBinning(bins,bins);
	c->setName("Correlations");
	c->setXAxisName("");
	c->setYAxisName("");
	for(size_t i=0;i<paracorrs_.size();i++){
		size_t binnox=c->getBinNoX((float)i+0.5);
		for(size_t j=0;j<paracorrs_.at(i).size();j++){
			size_t binnoy=c->getBinNoY((float)j+0.5);
			c->getBin(binnox,binnoy).setContent(paracorrs_.at(i).at(j));
		}
	}
}

void simpleFitter::getParameterErrorContribution(size_t a, size_t b,double & errup, double& errdown){

	if(!wasSuccess())
		throw std::logic_error("simpleFitter::getParameterErrorContribution: can only be called after fit");
	if(!pminimizer_)
		throw std::logic_error("simpleFitter::getParameterErrorContribution: can only be called after fit (minimizer pointer 0) IF YOU SEE THIS< SOMETHING IS SERIOUSLY WRONG");
	if(a>=paras_.size() || b>=paras_.size())
		throw std::out_of_range("simpleFitter::getParameterErrorContribution: one index out of range");
	if(parafixed_.at(a)){
		errdown=0;
		errup=0;
		return;
	}
	//work on hesse errors
	double olderr=pminimizer_->Errors()[b];
	//std::cout << "olderr: " << olderr<<std::endl;
	ROOT::Math::Minimizer *  min=invokeMinimizer();
	min->SetFixedVariable(a,paranames_.at(a).Data(),paras_.at(a));
	ROOT::Math::Functor fcn(chi2func_,paras_.size());
	if(functobemin_){
		min->SetFunction(*functobemin_);
	}
	else{
		min->SetFunction(fcn);
	}

	bool succ=min->Minimize();
	if(succ){
		errdown=min->Errors()[b];
		//std::cout << "newerr: " << errdown<<std::endl;
		double reldiff= (olderr-errdown)/olderr;
		if(reldiff>=0){
			errdown=sqrt(olderr*olderr-errdown*errdown);}
		else{
			if(reldiff>-0.001) //purely numerical difference
				errdown=0;
			else
				errdown=-100;
		}

	}
	else{
		errdown=-200;
	}
	errup=errdown;
	delete min;
}
void simpleFitter::getStatErrorContribution(size_t b,double & errup, double& errdown){

	if(!wasSuccess())
		throw std::logic_error("simpleFitter::getStatErrorContribution: can only be called after fit");
	if(!pminimizer_)
		throw std::logic_error("simpleFitter::getStatErrorContribution: can only be called after fit (minimizer pointer 0) IF YOU SEE THIS< SOMETHING IS SERIOUSLY WRONG");
	if(b>=paras_.size())
		throw std::out_of_range("simpleFitter::getStatErrorContribution:  index out of range");

	//work on hesse errors
	double olderr=pminimizer_->Errors()[b];
	//std::cout << "olderr: " << olderr<<std::endl;
	ROOT::Math::Minimizer *  min=invokeMinimizer();
	for(size_t i=0;i<paranames_.size();i++){
		if(i==b) continue;
		min->SetFixedVariable(i,paranames_.at(i).Data(),paras_.at(i));
	}
	ROOT::Math::Functor fcn(chi2func_,paras_.size());
	if(functobemin_){
		min->SetFunction(*functobemin_);
	}
	else{
		min->SetFunction(fcn);
	}

	bool succ=min->Minimize();
	if(succ){
		errdown=min->Errors()[b];
		double reldiff= (olderr-errdown)/olderr;
		if(reldiff<0){
			if(reldiff>-0.001) //purely numerical difference
				errdown=0;
			else
				errdown=-100;
		}

	}
	else{
		errdown=-200;
	}
	errup=errdown;
	delete min;
}



size_t simpleFitter::findParameterIdx(const TString& paraname)const{
	size_t idx=std::find(paranames_.begin(), paranames_.end(),paraname) - paranames_.begin();
	if(idx<paranames_.size()) return idx;
	throw std::runtime_error("simpleFitter::findParameterIdx: name not found");
}

//FixParameter(Int_t parNo)
/*
 * all function definitions are placed here
 */
double simpleFitter::fitfunction(const double& x,const double *par)const{
	//needs some safety
	double value=1;
	if(fitmode_==fm_pol0){
		value=par[0];
		if(value!=value){
			std::cout << "simpleFitter::fitfunction: NAN!: " <<x << " pars: "<< par[0]  << std::endl;
		}
	}
	else if(fitmode_==fm_pol1){
		value=par[0] + x* par[1];
		if(value!=value){
			std::cout << "simpleFitter::fitfunction: NAN!: " <<x << " pars: "<< par[0] << "  " << par[1]  << std::endl;
		}
	}
	else if(fitmode_==fm_pol2){
		value=par[0] + x* par[1] + x*x*par[2];
		if(value!=value){
			std::cout << "simpleFitter::fitfunction: NAN!: " <<x << " pars: "<< par[0] << "  " << par[1] << "  " <<par[2] << std::endl;
		}
	}
	else if(fitmode_==fm_pol3){
		value=par[0] + x* par[1] + x*x*par[2] + x*x*x*par[3];
	}
	else if(fitmode_==fm_pol4){
		value=par[0] + x* par[1] + x*x*par[2]  + x*x*x*par[3]  + x*x*x*x*par[4];
	}
	else if(fitmode_==fm_gaus){
		value =  par[0] * exp(- (x-par[1])*(x-par[1]) / (2* par[2]*par[2]) ) ;
	}
	else if(fitmode_==fm_offgaus){
		value = par[0]+ par[1] * exp(- (x-par[2])*(x-par[2]) / (2* par[3]*par[3]) ) ;
	}

	if(value!=value){
		throw std::runtime_error("simpleFitter::fitfunction: NAN produced");
	}
	return value;
}


void simpleFitter::clearPoints(){
	nompoints_.clear();
	errsup_.clear();
	errsdown_.clear();

}

bool simpleFitter::checkSizes()const{


	if(paras_.size() != stepsizes_.size())
		return false;
	if(paras_.size() != paraerrsup_.size())
		return false;
	if(paras_.size() != paraerrsdown_.size())
		return false;

	if(requirefitfunction_){
		if(paras_.size() != hasNParameters(fitmode_))
			return false;
		if(nompoints_.size()< hasNParameters(fitmode_))
			return false;
		if(nompoints_.size() != errsup_.size())
			return false;
		if(nompoints_.size() != errsdown_.size())
			return false;
		if(nompoints_.size() != errsup_.size())
			return false;

	}


	return true;
}


double simpleFitter::nuisanceGaus(const double & in){
	return in*in;
}
double simpleFitter::nuisanceBox(const double & in){
	double disttoone=fabs(in)-1;
	if(disttoone<0) return 0;
	return (1e3*disttoone*disttoone);
}
double simpleFitter::nuisanceLogNormal(const double & in){
	throw std::logic_error("simpleFitter::nuisanceLogNormal: nor properly implemented");

	double alph=in+1;
	return log(alph * (3+ log(alph)/0.693147180559945) ) ;
}


void simpleFitter::copyFrom(const simpleFitter& rhs){
	if(this==&rhs) return;
	fitmode_=rhs.fitmode_ ;
	minimizer_=rhs.minimizer_ ;
	chi2min_=-1;
	nompoints_=rhs.nompoints_ ;
	errsup_=rhs.errsup_ ;
	errsdown_=rhs.errsdown_ ;
	paras_=rhs.paras_ ;
	stepsizes_=rhs.stepsizes_ ;
	paraerrsup_=rhs.paraerrsup_ ;
	paraerrsdown_=rhs.paraerrsdown_ ;
	paranames_=rhs. paranames_;
	parafixed_=rhs.parafixed_ ;
	paracorrs_=rhs.paracorrs_ ;
	minospars_=rhs.minospars_ ;
	maxcalls_=rhs.maxcalls_ ;
	requirefitfunction_=rhs.requirefitfunction_ ;
	minsuccessful_=false;
	tolerance_=rhs.tolerance_ ;
	functobemin_=rhs.functobemin_;
	algorithm_=rhs.algorithm_ ;
	minimizerstr_=rhs.minimizerstr_ ;
	pminimizer_=0;
	chi2func_=simpleChi2(this);
}

ROOT::Math::Minimizer * simpleFitter::invokeMinimizer()const{
	//ROOT::Math::Minimizer* min =
	//	ROOT::Math::Factory::CreateMinimizer(minimizerstr_.Data(), algorithm_.Data());

	/////
	ROOT::Math::Minimizer* min=0;

	if (minimizerstr_ ==  "Minuit2")
		min = new ROOT::Minuit2::Minuit2Minimizer(algorithm_.Data());
	else if (minimizerstr_ ==  "Fumili2")
		min = new ROOT::Minuit2::Minuit2Minimizer("fumili");
	else if (minimizerstr_ ==  "Minuit" || minimizerstr_ ==  "TMinuit")
		min = new TMinuitMinimizer(algorithm_.Data(),paras_.size());
	else if (minimizerstr_ ==  "GSL")
		min = new ROOT::Math::GSLMinimizer(algorithm_.Data());
	else if (minimizerstr_ ==  "GSL_NLS")
		min = new ROOT::Math::GSLNLSMinimizer();
	else if (minimizerstr_ ==  "GSL_SIMAN")
		min = new ROOT::Math::GSLSimAnMinimizer();
	else
		min = new ROOT::Minuit2::Minuit2Minimizer();

	////


	min->SetMaxFunctionCalls(maxcalls_); // for Minuit/Minuit2
	min->SetMaxIterations(maxcalls_);  // for GSL
	min->SetTolerance(tolerance_);

	if(printlevel>0)
		min->SetPrintLevel(printlevel);
	else
		min->SetPrintLevel(0);


	for(size_t i=0;i<paras_.size();i++){
		TString paraname="";
		if(paranames_.size()>i)
			paraname=paranames_.at(i);
		else
			paraname=(TString)"par"+toTString(i);
		if(parafixed_.at(i))
			min->SetFixedVariable((unsigned int)i,paraname.Data(),paras_.at(i));
		else
			min->SetVariable((unsigned int)i,paraname.Data(),paras_.at(i), stepsizes_.at(i));
	}
	//std::numeric_limits<double> lim;
	min->SetPrecision(DBL_EPSILON); //16
	min->SetStrategy(strategy_);

	return min;
}

}


