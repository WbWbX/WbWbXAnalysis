/*
 * simpleFitter.cc
 *
 *  Created on: May 22, 2014
 *      Author: kiesej
 */

#include "../interface/simpleFitter.h"
#include <TROOT.h>
#include <TMinuit.h>
#include "Math/Minimizer.h"
#include "Minuit2/MnStrategy.h"
#include <iostream>
#include "Math/Functor.h"
#include "Math/Factory.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include <Minuit2/MnMachinePrecision.h>

#include <stdexcept>



//use TMinuit
//define all the functions - nee dto be c-stylish



namespace ztop{

namespace chi2defs{


class simpleChi2 {

public:

	simpleChi2(simpleFitter * fp):fp_(fp) {npars_=fp_->getParameters()->size();}


	double operator() (const double* par) const {
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

	double Up() const { return 1.; }

private:
	simpleFitter* fp_;
	size_t npars_;

};

}//ns

int simpleFitter::printlevel=0;
bool simpleFitter::debug=false;

simpleFitter::simpleFitter():fitmode_(fm_pol0),minimizer_(mm_minuitMinos),maxcalls_(5000000),
		requirefitfunction_(true),minsuccessful_(false),tolerance_(0.1),functobemin_(0),algorithm_(""),minimizerstr_("Minuit2"){

	setFitMode(fitmode_);


}

simpleFitter::~simpleFitter(){}


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
	std::vector<double> dummy;
	dummy.resize(paras_.size(),0);
	paracorrs_.clear();
	paracorrs_.resize(paras_.size(),dummy);

}

void simpleFitter::setParameters(const std::vector<double>& inpars,const std::vector<double>& steps){
	paras_=inpars;stepsizes_=steps;
	paraerrsup_.resize(paras_.size(),0);
	paraerrsdown_.resize(paras_.size(),0);
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


double simpleFitter::getFitOutput(const double& xin)const{
	return fitfunction(xin,&(paras_.at(0)));
}


void simpleFitter::fit(){

	if(!checkSizes()){
		std::cout << "EROR IN simpleFitter::fit" <<std::endl;

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
		std::cout << "simpleFitter::fit(): fitting " << hasNParameters(fitmode_) << "/" << paras_.size() <<  " parameters." <<std::endl;

	minsuccessful_=false;

	//  if(functobemin_)

	if(minimizer_==mm_minuit2 || minimizer_==mm_minuitMinos){

		chi2defs::simpleChi2 fcn(this);

		ROOT::Math::Minimizer* min =
				ROOT::Math::Factory::CreateMinimizer(minimizerstr_.Data(), algorithm_.Data());

		ROOT::Math::Functor f(fcn,paras_.size());

		if(!functobemin_){
			min->SetFunction(f);
		}
		else{
			min->SetFunction(*functobemin_);

		}


		min->SetMaxFunctionCalls(maxcalls_); // for Minuit/Minuit2
		min->SetMaxIterations(maxcalls_);  // for GSL
		min->SetTolerance(tolerance_);
		min->SetPrintLevel(printlevel);


		ROOT::Minuit2::MnMachinePrecision prec;
		//  min->SetPrecision(1e-6);


		for(size_t i=0;i<paras_.size();i++){
			TString paraname="";
			if(paranames_.size()>i)
				paraname=paranames_.at(i);
			else
				paraname=(TString)"par"+toTString(i);
			min->SetVariable((unsigned int)i,paraname.Data(),paras_.at(i), stepsizes_.at(i));
		}

		ROOT::Minuit2::MnStrategy strat;
		strat.SetHighStrategy();
		min->SetStrategy(strat.Strategy());
		minsuccessful_=min->Minimize();

		const double *xs = min->X();
		//feed back
		for(size_t i=0;i<paras_.size();i++){
			paras_.at(i)=xs[i];

			TString paraname="";
			if(paranames_.size()>i)
				paraname=paranames_.at(i);
			else
				paraname=(TString)"par"+toTString(i);

			if(minospars_.size()==0 || std::find(minospars_.begin(),minospars_.end(),i) != minospars_.end()){
				min->GetMinosError(i, paraerrsdown_.at(i), paraerrsup_.at(i));
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


		delete min;


	}

}

double simpleFitter::getParameter(size_t idx)const{
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
	return (1e5*disttoone*disttoone*disttoone*disttoone);
}
double simpleFitter::nuisanceLogNormal(const double & in){
	throw std::logic_error("simpleFitter::nuisanceLogNormal: nor properly implemented");

	double alph=in+1;
	return log(alph * (3+ log(alph)/0.693147180559945) ) ;
}

}


