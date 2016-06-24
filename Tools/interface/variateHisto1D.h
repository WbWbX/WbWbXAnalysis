/*
 * variateHisto1D.h
 *
 *  Created on: Aug 19, 2014
 *      Author: kiesej
 */

#ifndef VARIATEHISTO1D_H_
#define VARIATEHISTO1D_H_

#include "extendedVariable.h"
#include <vector>
#include "histo1D.h"
#include <algorithm>
#include "taggedObject.h"
#include "simpleFitter.h"

namespace ztop{

class variateHisto1D : public taggedObject{
public:
	variateHisto1D():comparehist_(0){}
	~variateHisto1D(){}

	void import(const histo1D&);

	//exports for a certain set of variations
	//no syst for now!
	histo1D exportContainer(const std::vector<double> & variations)const;

	/**
	 * exports for a certain set of variations and constraints on them
	 */
	histo1D exportContainer(const std::vector<double> & variations,const std::vector<double> & symm_constraints,const std::vector<TString> & varnames)const;
	/**
	 * exports with 0 variation
	 */
	histo1D exportContainer()const;

	float getBinErrUp(size_t idx){return errsup_.at(idx)*contents_.at(idx).getMultiplicationFactor(zeroVar());}
	float getBinErrDown(size_t idx){return errsdown_.at(idx)*contents_.at(idx).getMultiplicationFactor(zeroVar());}
	float getBinErr(size_t idx){float out= std::max(errsdown_.at(idx),errsup_.at(idx))*contents_.at(idx).getMultiplicationFactor(zeroVar());
	if(out!=out)return 0.;else return out;}

	void setBinErrUp(size_t ,const float&);
	void setBinErrDown(size_t ,const float&);

	const std::vector<float>& bins(){return bins_;}

	size_t getNDependencies()const{if(contents_.size()>0) return contents_.at(0).getNDependencies();else return 0;}
	/**
	 * returns full size including possible UF, OF
	 */
	size_t getNBins()const{return contents_.size();}
	const std::vector<float>& getBins()const{return bins_;}

	std::vector<TString> getSystNames()const{if(contents_.size()>0) return contents_.at(0).getSystNames();else return std::vector<TString>();}

	extendedVariable * getBin(size_t idx){if(contents_.size()>0) return &contents_.at(idx);else return 0;}
	const extendedVariable * getBin(size_t idx)const{if(contents_.size()>0) return &contents_.at(idx);else return 0;}

	double getIntegral(const double * vars)const;
	extendedVariable getIntegral()const;

	/*
	 * Function only works for data-like histos with stat=sqrt(events)
	 * Uses Poisson likelihood, neglecting MC statistics
	 */
	simpleFitter fitToConstHisto(const histo1D&);


	/*
	 * Warning: All operations are assumed to have no statistical limitations!
	 * The stat uncertainty of "this" will be kept and scaled accordingly, but
	 * no extra uncertainty will be added from additional variateHisto1Ds
	 */

	variateHisto1D& operator *= (const variateHisto1D&);
	variateHisto1D operator * (const variateHisto1D&)const;
	variateHisto1D& operator /= (const variateHisto1D&);
	variateHisto1D operator / (const variateHisto1D&)const;
	variateHisto1D& operator += (const variateHisto1D&);
	variateHisto1D operator + (const variateHisto1D&)const;
	variateHisto1D& operator -= (const variateHisto1D&);
	variateHisto1D operator - (const variateHisto1D&)const;

	variateHisto1D& operator *= (const extendedVariable&);
	variateHisto1D operator * (const extendedVariable&)const;
	variateHisto1D& operator /= (const extendedVariable&);
	variateHisto1D operator / (const extendedVariable&)const;


	variateHisto1D& operator *= (const double&);
	variateHisto1D operator * (const double&)const;
	variateHisto1D& operator /= (const double&);
	variateHisto1D operator / (const double&)const;

	static bool debug;

private:

	std::vector<extendedVariable> contents_;
	std::vector<float> bins_;

	std::vector<double> zeroVar()const;

	//this is for stat uncertainties
	std::vector<float> errsup_;
	std::vector<float> errsdown_;

	void checkCompat(const variateHisto1D& rhs)const;

	double toBeMinimizedInFit(const double * variations)const;
	const histo1D* comparehist_;//fot fit

};

}//ns


#endif /* VARIATECONTAINER1D_H_ */
