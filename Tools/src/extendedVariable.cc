/*
 * extendedVariable.cc
 *
 *  Created on: May 23, 2014
 *      Author: kiesej
 */

#include "../interface/extendedVariable.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include <stdexcept>
#include <algorithm>

namespace ztop{

bool extendedVariable::debug=false;
bool extendedVariable::debugoperations=false;

extendedVariable & extendedVariable::operator =(const extendedVariable&rhs){
	if(this==&rhs) return *this;
	copyFrom(rhs);
	return *this;
}
extendedVariable::extendedVariable(const extendedVariable&rhs){
	if(this==&rhs) return;
	copyFrom(rhs);
}


void extendedVariable::addDependence(const graph & g, size_t nompoint, const TString& sysname){
	if(debug)
		std::cout << "extendedVariable::addDependence" <<std::endl;

	if(nompoint>=g.getNPoints()){
		throw std::out_of_range("extendedVariable::addDependence: nompoint is out of range");
	}
	if(g.getSystSize()>0){
		throw std::logic_error("extendedVariable::addDependence: do not input graphs with systematics!");
	}

	graph gcopy=g;

	//shift all to nominal and prepare in case of more than 3 points

	if(fabs(nominal_ / -100000 -1) < 0.00001){
		nominal_=g.getPointYContent(nompoint);
	}
	else{
		if(fabs(nominal_/g.getPointYContent(nompoint) -1) > 0.001)
			throw std::logic_error("extendedVariable::addDependence: nominal values differ!");
	}


	gcopy.shiftAllYCoordinates(-nominal_);
	if(g.getNPoints()>3){
		gcopy.setPointYStat(nompoint,g.getPointYStat(nompoint)*0.0001);
	}


	graphFitter fitter;
	fitter.setFitMode(graphFitter::fm_pol2);
	fitter.setMinimizer(graphFitter::mm_minuit2);
	fitter.readGraph(&gcopy);

	//in case of const variation on one side
	if(g.getNPoints() == 3){
		gcopy.sortPointsByX();
		if(gcopy.pointsYIdentical(0,1) || gcopy.pointsYIdentical(1,2)){
			fitter.setInterpolate(true);
			if(debug)
				std::cout << "extendedVariable::addDependence: detected const variation for "<< sysname<< " on one side, switch to interpolate mode." <<std::endl;
		}
		else if(debug){
			std::cout << "extendedVariable::addDependence: detected non-const variation for "<<sysname<< std::endl;
		}
	}

	fitter.fit();

	if(!fitter.wasSuccess()){
		std::cout << "extendedVariable::addDependence: failed to fit " << sysname << " (" << name_<< ") " << std::endl;
		throw std::runtime_error("extendedVariable::addDependence: failed to fit");
	}
	if(debug){
		std::cout << "extendedVariable::addDependence(graph): fit successful"<< std::endl;
	}
	dependences_.push_back(fitter);
	sysnames_.push_back(sysname);

}

graph extendedVariable::addDependence(const float & low, const float& nominal, const float& high , const TString& sysname){
	if(debug)
		std::cout << "extendedVariable::addDependence" <<std::endl;
	graph tmpg(3);
	tmpg.setPointXContent(0,-1);
	tmpg.setPointXContent(1, 0);
	tmpg.setPointXContent(2, 1);

	if(nominal!=nominal || high!=high || low!=low)
		throw std::runtime_error("extendedVariable::addDependence: nan inserted!");

	tmpg.setPointYContent(0,low);
	tmpg.setPointYContent(1,nominal);
	tmpg.setPointYContent(2,high);

	float min=std::min(fabs(low),fabs(high));
	min=std::min(fabs(nominal),fabs(min));
	if(min==0){ //one is zero
		float maxi=std::max(fabs(low),fabs(high));
		maxi=std::max(fabs(maxi),fabs(nominal));
		if(maxi==0) min=1e-3; //will be solved instead of fitted anyway
		else min=1e-3*maxi;

		tmpg.setPointYStat(0,min);
		tmpg.setPointYStat(1,min);
		tmpg.setPointYStat(2,min);
	}
	else{
		tmpg.setPointYStat(0,low/100);
		tmpg.setPointYStat(1,nominal/100);
		tmpg.setPointYStat(2,high/100);
	}

	tmpg.setName(name_+"_"+sysname);
	tmpg.setXAxisName(sysname+"[#sigma_{"+sysname+"}]");
	tmpg.setYAxisName(name_);

	if(debug){
		std::cout << "extendedVariable::addDependence("<< low << "," << nominal <<","<<high << "," << sysname<< ")  "<<name_<< std::endl;
	}

	addDependence(tmpg,1,sysname);
	return tmpg;
}




double extendedVariable::getValue(const double * variations)const{
	if(debug)
		std::cout << "extendedVariable::getValue" <<std::endl;
	if(constant_)
		return constval_;

	double out=0;
	for(size_t i=0;i<dependences_.size();i++){
		out+=dependences_.at(i).getFitOutput(variations[i]);
		if(out!=out){
			std::cout << "extendedVariable::getValue: nan produced for " << sysnames_.at(i) << " (" << name_<< ") "<<std::endl;
			std::cout << "Variation input: " << variations[i] <<std::endl;
			std::cout << "all variations input:";
			for(size_t j=0;j<dependences_.size();j++)
				std::cout<<" "<< variations[j];
			std::cout<<std::endl;
			throw std::runtime_error("extendedVariable::getValue: nan produced");
		}
	}

	double outd=  addOperations(out+nominal_,variations);
	//if(debugoperations)std::cout << std::endl;
	return outd;

}
double extendedVariable::getValue(const float * variations)const{
	if(debug)
		std::cout << "extendedVariable::getValue" <<std::endl;
	if(constant_)
		return constval_;
	double out=0;
	for(size_t i=0;i<dependences_.size();i++){
		out+=dependences_.at(i).getFitOutput((double)variations[i]);
		if(out!=out){
			std::cout << "extendedVariable::getValue (float*): nan produced for " << sysnames_.at(i) << " (" << name_<< ") "<<std::endl;
			std::cout << "Variation input: " << variations[i] <<std::endl;

			throw std::runtime_error("extendedVariable::getValue: nan produced");
		}
	}
	double outd=  addOperations(out+nominal_,variations);
	//if(debugoperations)std::cout << "="<< outd << std::endl;
	return outd;
}

double extendedVariable::getValue(const std::vector<float> * variations)const{
	if(debug)
		std::cout << "extendedVariable::getValue" <<std::endl;
	if(constant_)
		return constval_;

	if(variations->size()!=dependences_.size()){
		throw std::out_of_range("extendedVariable::getValue: number of variations and dependencies don't match");
	}

	return getValue(&variations->at(0));

}

double extendedVariable::getValue(const std::vector<float> & variations)const{
	if(debug)
		std::cout << "extendedVariable::getValue" <<std::endl;
	if(constant_)
		return constval_;

	if(variations.size()!=dependences_.size()){
		std::string errstr=(std::string)"extendedVariable::getValue: number of variations: "+
				toString(variations.size()) +(std::string)" and dependencies: "
				+toString(dependences_.size())+(" don't match: "+name_).Data();
		throw std::out_of_range(errstr);
	}

	return getValue(&variations);

}
double extendedVariable::getValue(const std::vector<double> * variations)const{
	if(debug)
		std::cout << "extendedVariable::getValue" <<std::endl;
	if(constant_)
		return constval_;

	if(variations->size()!=dependences_.size()){
		std::string errstr=(std::string)"extendedVariable::getValue: number of variations: "+
				toString(variations->size()) +(std::string)" and dependencies: "
				+toString(dependences_.size())+" don't match";
		throw std::out_of_range(errstr);
	}

	return  getValue(&variations->at(0));

}
double extendedVariable::getValue(size_t idx,float variation)const{
	if(debug)
		std::cout << "extendedVariable::getValue" <<std::endl;
	if(constant_)
		return constval_;
	if(idx >= dependences_.size()){
		throw std::out_of_range("extendedVariable::getValue: index out of range");
	}
	return dependences_.at(idx).getFitOutput(variation)+nominal_;

}

double extendedVariable::getValue(const std::vector<double> & variations)const{
	if(debug)
		std::cout << "extendedVariable::getValue" <<std::endl;
	if(constant_)
		return constval_;

	if(variations.size()!=dependences_.size()){
		std::string errstr=(std::string)"extendedVariable::getValue: number of variations: "+
				toString(variations.size()) +(std::string)" and dependencies: "
				+toString(dependences_.size())+" don't match";
		throw std::out_of_range(errstr);
	}

	return getValue(&variations.at(0));

}


double extendedVariable::addOperations( double in,const float * variations)const{
	if(debugoperations)
		std::cout << in ;
	if(operatedon_.size()==0){
		if(debugoperations)std::cout << std::endl;
		return in;
	}
	for(size_t i=0;i<operatedon_.size();i++){
		if(operation_.at(i) == op_plus){
			if(debugoperations)
				std::cout <<"+";
			in+=operatedon_.at(i)->getValue(variations);
		}
		else if(operation_.at(i) == op_minus){
			if(debugoperations)
				std::cout <<"-";
			in-=operatedon_.at(i)->getValue(variations);
		}
		else if(operation_.at(i) == op_multi){
			if(debugoperations)
				std::cout <<"*";
			in*=operatedon_.at(i)->getValue(variations);
		}
		else if(operation_.at(i) == op_divide){
			if(debugoperations)
				std::cout <<"/";
			in/=operatedon_.at(i)->getValue(variations);
		}
	}
	return in;
}
double extendedVariable::addOperations( double in,const double * variations)const{
	if(debugoperations)
		std::cout << in ;
	if(operatedon_.size()==0){
		if(debugoperations)std::cout << std::endl;
		return in;
	}
	for(size_t i=0;i<operatedon_.size();i++){
		if(operation_.at(i) == op_plus){
			if(debugoperations)
				std::cout <<"+";
			in+=operatedon_.at(i)->getValue(variations);
		}
		else if(operation_.at(i) == op_minus){
			if(debugoperations)
				std::cout <<"-";
			in-=operatedon_.at(i)->getValue(variations);
		}
		else if(operation_.at(i) == op_multi){
			if(debugoperations)
				std::cout <<"*";
			in*=operatedon_.at(i)->getValue(variations);
		}
		else if(operation_.at(i) == op_divide){
			if(debugoperations)
				std::cout <<"/";
			in/=operatedon_.at(i)->getValue(variations);
		}
	}
	return in;
}

size_t extendedVariable::checkDepth()const{
	return 0;
	/*(size_t out=0;
	if(operatedon_){
		out++;
		out+=operatedon_->checkDepth();
	}
	if(out > 200)
		throw std::out_of_range("extendedVariable::checkDepth: performed too many operations (>200) on extended variables, result may be unreliable");
	return out;*/
}



void extendedVariable::copyFrom(const extendedVariable& rhs){
	nominal_=rhs.nominal_;
	name_=rhs.name_;
	dependences_=rhs.dependences_;
	operation_=rhs.operation_;
	sysnames_=rhs.sysnames_;
	constant_=rhs.constant_;
	constval_=rhs.constval_;
	operatedon_.clear();
	for(size_t i=0;i<rhs.operatedon_.size();i++){
		if(rhs.operatedon_.at(i))
			operatedon_.push_back(new extendedVariable(*rhs.operatedon_.at(i))); //this will be recursive
		else
			operatedon_.push_back(0);
	}
}



void extendedVariable::clear(){
	if(debug)
		std::cout << "extendedVariable::clear" <<std::endl;
	dependences_.clear();
	sysnames_.clear();
	nominal_=-100000;
	for(size_t i=0;i<operatedon_.size();i++)
		if(operatedon_.at(i)) delete operatedon_.at(i);
	operatedon_.clear();
	operation_.clear();
}
/*
extendedVariable* extendedVariable::getLast(){
	extendedVariable* out=this;
	while(1){
		if(!out->operatedon_) break;
		out=out->operatedon_;
	}
	return out;
}*/
void extendedVariable::slim(){
	name_="";
}
void extendedVariable::checkDependencies(const extendedVariable&rhs){
	if(rhs.getNDependencies()!=getNDependencies())
		throw std::out_of_range("extendedVariable::checkDependencies: ndep don't match");
	if(rhs.sysnames_!=sysnames_)
		throw std::out_of_range("extendedVariable::checkDependencies: dependencies names don't match");

}

void extendedVariable::insertOperations(const extendedVariable&rhs){
	checkDependencies(rhs);
	extendedVariable* rhscp=new extendedVariable(rhs);
	operatedon_.push_back(rhscp);


	/*
	extendedVariable* rhscp=new extendedVariable(rhs);
	extendedVariable* lastrhs=rhscp->getLast();
	lastrhs->operatedon_=operatedon_;
	lastrhs->operation_=operation_;
	operatedon_=rhscp;
	checkDepth();*/
}
extendedVariable& extendedVariable::operator *= (const extendedVariable&rhs){
	insertOperations(rhs);
	operation_.push_back(op_multi);
	return *this;
}
extendedVariable extendedVariable::operator * (const extendedVariable&rhs)const{
	extendedVariable cp=*this;
	return cp*=rhs;
}
extendedVariable& extendedVariable::operator /= (const extendedVariable&rhs){
	insertOperations(rhs);
	operation_.push_back(op_divide);
	return *this;
}
extendedVariable extendedVariable::operator / (const extendedVariable&rhs)const{
	extendedVariable cp=*this;
	return cp/=rhs;
}
extendedVariable& extendedVariable::operator += (const extendedVariable&rhs){
	insertOperations(rhs);
	operation_.push_back(op_plus);
	return *this;
}
extendedVariable extendedVariable::operator + (const extendedVariable&rhs)const{
	extendedVariable cp=*this;
	return cp+=rhs;
}
extendedVariable& extendedVariable::operator -= (const extendedVariable&rhs){
	insertOperations(rhs);
	operation_.push_back(op_minus);
	return *this;
}
extendedVariable extendedVariable::operator - (const extendedVariable&rhs)const{
	extendedVariable cp=*this;
	return cp-=rhs;
}

extendedVariable& extendedVariable::operator *= (const double&v){
	extendedVariable n=*this;
	n.setConstant(v);
	*this*=n;
	return *this;
}
extendedVariable extendedVariable::operator * (const double&v)const{
	extendedVariable cp=*this;
	return cp*=v;
}
extendedVariable& extendedVariable::operator /= (const double&v){
	extendedVariable n=*this;
	n.setConstant(v);
	*this/=n;
	return *this;
}
extendedVariable extendedVariable::operator / (const double&v)const{
	extendedVariable cp=*this;
	return cp/=v;
}


double extendedVariable::getMultiplicationFactor(const std::vector<double> * variations)const{
	if(debug)
		std::cout << "extendedVariable::getMultiplicationFactor" <<std::endl;
	if(constant_)
		return constval_;

	if(variations->size()!=dependences_.size()){
		std::string errstr=(std::string)"extendedVariable::getValue: number of variations: "+
				toString(variations->size()) +(std::string)" and dependencies: "
				+toString(dependences_.size())+" don't match";
		throw std::out_of_range(errstr);
	}
	double outd= getMultiplicationFactorRec(& variations->at(0));
	double thisvar=0;
	for(size_t i=0;i<dependences_.size();i++){
		thisvar+=dependences_.at(i).getFitOutput(variations->at(i));
		if(thisvar!=thisvar){
			std::cout << "extendedVariable::getValue: nan produced for " << sysnames_.at(i) << " (" << name_<< ") "<<std::endl;
			std::cout << "Variation input: " << variations->at(i) <<std::endl;
			std::cout << "all variations input:";
			for(size_t j=0;j<dependences_.size();j++)
				std::cout<<" "<< variations->at(j);
			std::cout<<std::endl;
			throw std::runtime_error("extendedVariable::getValue: nan produced");
		}
	}
	outd/=(thisvar+nominal_);
	return outd;
}
double extendedVariable::getMultiplicationFactor(const std::vector<double> &variations)const{
	return getMultiplicationFactor(&variations);
}




double extendedVariable::getMultiplicationFactorRec(const double * variations)const{
	if(debug)
		std::cout << "extendedVariable::getMultiplicationFactor" <<std::endl;
	if(constant_)
		return 1;

	double out=0;
	for(size_t i=0;i<dependences_.size();i++){
		out+=dependences_.at(i).getFitOutput(variations[i]);
		if(out!=out){
			std::cout << "extendedVariable::getValue: nan produced for " << sysnames_.at(i) << " (" << name_<< ") "<<std::endl;
			std::cout << "Variation input: " << variations[i] <<std::endl;
			std::cout << "all variations input:";
			for(size_t j=0;j<dependences_.size();j++)
				std::cout<<" "<< variations[j];
			std::cout<<std::endl;
			throw std::runtime_error("extendedVariable::getValue: nan produced");
		}
	}
	double outd= addMultiFactors(out+nominal_,variations);
	//if(debugoperations)std::cout << "="<< outd << std::endl;
	return outd;
}

//only adds factors,
double extendedVariable::addMultiFactors( double in,const double * variations)const{
	if(debugoperations){
		std::cout << in ;
	}
	if(operatedon_.size()==0){
		if(debugoperations)std::cout << std::endl;
		return in;
	}
	for(size_t i=0;i<operatedon_.size();i++){
		if(operation_.at(i) == op_multi){
			if(debugoperations)
				std::cout <<"*";
			in*=operatedon_.at(i)->getMultiplicationFactorRec(variations);
		}
		else if(operation_.at(i) == op_divide){
			if(debugoperations)
				std::cout <<"/";
			in/=operatedon_.at(i)->getMultiplicationFactorRec(variations);
		}
		else{
			std::cout <<"[]"<<std::endl;
			operatedon_.at(i)->getMultiplicationFactorRec(variations);
		}
	}
	return in;
}

}
