/*
 * extendedVariable.cc
 *
 *  Created on: May 23, 2014
 *      Author: kiesej
 */

#include "../interface/extendedVariable.h"
#include <stdexcept>
#include <algorithm>

namespace ztop{

bool extendedVariable::debug=false;

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

	double out=0;
	for(size_t i=0;i<dependences_.size();i++){
		out+=dependences_.at(i).getFitOutput(variations[i]);
		if(out!=out){
			std::cout << "extendedVariable::getValue: nan produced for " << sysnames_.at(i) << " (" << name_<< ") "<<std::endl;
			throw std::runtime_error("extendedVariable::getValue: nan produced");
		}
	}
	return out+nominal_;
}
double extendedVariable::getValue(const float * variations)const{
	if(debug)
		std::cout << "extendedVariable::getValue" <<std::endl;
	double out=0;
	for(size_t i=0;i<dependences_.size();i++){
		out+=dependences_.at(i).getFitOutput(variations[i]);
		if(out!=out){
			std::cout << "extendedVariable::getValue: nan produced for " << sysnames_.at(i) << " (" << name_<< ") "<<std::endl;
			throw std::runtime_error("extendedVariable::getValue: nan produced");
		}
	}
	return out+nominal_;
}

double extendedVariable::getValue(const std::vector<float> * variations)const{
	if(debug)
		std::cout << "extendedVariable::getValue" <<std::endl;

	if(variations->size()!=dependences_.size()){
		throw std::out_of_range("extendedVariable::getValue: number of variations and dependencies don't match");
	}

	return getValue(&variations->at(0));

}

double extendedVariable::getValue(const std::vector<float> & variations)const{
	if(debug)
		std::cout << "extendedVariable::getValue" <<std::endl;

	if(variations.size()!=dependences_.size()){
		throw std::out_of_range("extendedVariable::getValue: number of variations and dependencies don't match");
	}

	return getValue(&variations);

}
double extendedVariable::getValue(const std::vector<double> * variations)const{
	if(debug)
		std::cout << "extendedVariable::getValue" <<std::endl;

	if(variations->size()!=dependences_.size()){
		throw std::out_of_range("extendedVariable::getValue: number of variations and dependencies don't match");
	}

	return getValue(&variations->at(0));

}
double extendedVariable::getValue(size_t idx,float variation)const{
	if(debug)
		std::cout << "extendedVariable::getValue" <<std::endl;
	if(idx >= dependences_.size()){
		throw std::out_of_range("extendedVariable::getValue: index out of range");
	}
	return dependences_.at(idx).getFitOutput(variation)+nominal_;

}

double extendedVariable::getValue(const std::vector<double> & variations)const{
	if(debug)
		std::cout << "extendedVariable::getValue" <<std::endl;

	if(variations.size()!=dependences_.size()){
		throw std::out_of_range("extendedVariable::getValue: number of variations and dependencies don't match");
	}

	return getValue(&variations.at(0));

}


void extendedVariable::clear(){
	if(debug)
		std::cout << "extendedVariable::clear" <<std::endl;
	dependences_.clear();
	sysnames_.clear();
	nominal_=-100000;
}
}
