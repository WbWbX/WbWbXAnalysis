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
    extendedVariable():nominal_(-100000){}
    ~extendedVariable(){}



    /**
     * just put (non shifted) graph here
     * also adds nominal
     * performs 2nd order fit and makes
     */
    void addDependence(const graph & , size_t nompoint, const TString& sysname);

    size_t getNDependencies(){return dependences_.size();}

    float getValue(const std::vector<float> & variations)const;
    float getValue(const std::vector<float> * variations)const;
    float getValue(const std::vector<double> & variations)const;
    float getValue(const std::vector<double> * variations)const;

    float getValue(const double * variations)const;

    float getValue(size_t idx,float variation)const;

    const float & getNominal()const{return nominal_;}

    void clear();

private:

    float nominal_;
    std::vector<graphFitter> dependences_; //one for each syst
    std::vector<TString> sysnames_; //only for later reference

};

}
#endif /* EXTENDEDVARIABLE_H_ */
