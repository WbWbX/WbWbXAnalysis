/*
 * extendedVariable.h
 *
 *  Created on: May 23, 2014
 *      Author: kiesej
 */

#ifndef EXTENDEDVARIABLE_H_
#define EXTENDEDVARIABLE_H_


#include "TString.h"
#include "graphFitter.h"
#include "graph.h"


namespace ztop{
/**
 * This is a helper class to handle variables that depend on systematics
 *
 */
class extendedVariable{
public:
    extendedVariable():nominal_(-100000),name_(""){}
    extendedVariable(const TString & name):nominal_(-100000),name_(name){}
    ~extendedVariable(){}

    void setName(const TString & name){name_=name;}

    /**
     * just put (non shifted) graph here
     * also adds nominal
     * performs 2nd order fit and makes
     */
    void addDependence(const graph & , size_t nompoint, const TString& sysname);

    size_t getNDependencies(){return dependences_.size();}

    double getValue(const double * variations)const;
    double getValue(const float * variations)const;

    double getValue(const std::vector<float> & variations)const;
    double getValue(const std::vector<float> * variations)const;
    double getValue(const std::vector<double> & variations)const;
    double getValue(const std::vector<double> * variations)const;


    double getValue(size_t idx,float variation)const;

    const double & getNominal()const{return nominal_;}

    void clear();

private:

    double nominal_;
    TString name_;
    std::vector<graphFitter> dependences_; //one for each syst
    std::vector<TString> sysnames_; //only for later reference

};

}
#endif /* EXTENDEDVARIABLE_H_ */
