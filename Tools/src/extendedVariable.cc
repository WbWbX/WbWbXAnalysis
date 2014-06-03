/*
 * extendedVariable.cc
 *
 *  Created on: May 23, 2014
 *      Author: kiesej
 */

#include "../interface/extendedVariable.h"
#include <stdexcept>

namespace ztop{

void extendedVariable::addDependence(const graph & g, size_t nompoint, const TString& sysname){

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
    fitter.fit();

    if(!fitter.wasSuccess()){
        std::cout << "extendedVariable::addDependence: failed to fit " << sysname << " (" << name_<< ") " << std::endl;
        throw std::runtime_error("extendedVariable::addDependence: failed to fit");
    }

    dependences_.push_back(fitter);
    sysnames_.push_back(sysname);

}

double extendedVariable::getValue(const double * variations)const{

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

    if(variations->size()!=dependences_.size()){
        throw std::out_of_range("extendedVariable::getValue: number of variations and dependencies don't match");
    }

    return getValue(&variations->at(0));

}

double extendedVariable::getValue(const std::vector<float> & variations)const{

    if(variations.size()!=dependences_.size()){
        throw std::out_of_range("extendedVariable::getValue: number of variations and dependencies don't match");
    }

    return getValue(&variations);

}
double extendedVariable::getValue(const std::vector<double> * variations)const{

    if(variations->size()!=dependences_.size()){
        throw std::out_of_range("extendedVariable::getValue: number of variations and dependencies don't match");
    }

    return getValue(&variations->at(0));

}
double extendedVariable::getValue(size_t idx,float variation)const{
    if(idx >= dependences_.size()){
        throw std::out_of_range("extendedVariable::getValue: index out of range");
    }
    return dependences_.at(idx).getFitOutput(variation)+nominal_;

}

double extendedVariable::getValue(const std::vector<double> & variations)const{

    if(variations.size()!=dependences_.size()){
        throw std::out_of_range("extendedVariable::getValue: number of variations and dependencies don't match");
    }

    return getValue(&variations.at(0));

}


void extendedVariable::clear(){
    dependences_.clear();
    sysnames_.clear();
    nominal_=-100000;
}
}
