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
    extendedVariable():nominal_(-100000),name_(""),operatedon_(0),operation_(op_plus),constant_(false),constval_(0){}
    extendedVariable(const TString & name):nominal_(-100000),name_(name),operatedon_(0),operation_(op_plus),constant_(false),constval_(0){}
    ~extendedVariable(){
    	clear();
    }
    extendedVariable & operator =(const extendedVariable&);
    extendedVariable(const extendedVariable&);


    void setName(const TString & name){name_=name;}

    /**
     * Use to create a dummy that always returns one. it can be handy in some cases
     */
    void setToOne(bool set){constant_=set;constval_=1.;}

    void setConstant(double val){constant_=true;constval_=val;}

    void resetConstant(){constant_=false;}

    /**
     * just put (non shifted) graph here
     * also adds nominal
     * performs 2nd order fit and makes
     */
    void addDependence(const graph & , size_t nompoint, const TString& sysname);
    /**
     * add dependence
     * performs 2nd order "fit"
     * errors will be arbitrary
     *
     * will have scaling in terms of sigma (low = -sigma, high=sigma), sigma=1
     *
     */
    graph addDependence(const float & low, const float& nominal, const float& high , const TString& sysname);


    size_t getNDependencies()const{return dependences_.size();}

    double getValue(const double * variations)const;
    double getValue(const float * variations)const;

    double getValue(const std::vector<float> & variations)const;
    double getValue(const std::vector<float> * variations)const;
    double getValue(const std::vector<double> & variations)const;
    double getValue(const std::vector<double> * variations)const;


    double getValue(size_t idx,float variation)const;

    const double & getNominal()const{return nominal_;}

    const std::vector<TString>& getSystNames()const{return sysnames_;}

    void clear();


    extendedVariable& operator *= (const extendedVariable&);
    extendedVariable operator * (const extendedVariable&)const;
    extendedVariable& operator /= (const extendedVariable&);
    extendedVariable operator / (const extendedVariable&)const;
    extendedVariable& operator += (const extendedVariable&);
    extendedVariable operator + (const extendedVariable&)const;
    extendedVariable& operator -= (const extendedVariable&);
    extendedVariable operator - (const extendedVariable&)const;


    extendedVariable& operator *= (const double&);
    extendedVariable operator * (const double&)const;
    extendedVariable& operator /= (const double&);
    extendedVariable operator / (const double&)const;

    static bool debug;
private:

    enum operators{op_plus,op_minus,op_multi,op_divide};

    double nominal_;
    TString name_;
    std::vector<graphFitter>  dependences_; //one for each syst

    extendedVariable * operatedon_;
    operators operation_;
    double addOperations(const double&,const float * )const;
    double addOperations(const double&,const double * )const;
//return max dependence depth
    size_t checkDepth()const;

    std::vector<TString> sysnames_; //only for later reference

    bool constant_;
    double constval_;

    void copyFrom(const extendedVariable&) ;

    extendedVariable* getLast();
    void slim();
    void checkDependencies(const extendedVariable&);
    void insertOperations(const extendedVariable&) ;
};

}
#endif /* EXTENDEDVARIABLE_H_ */
