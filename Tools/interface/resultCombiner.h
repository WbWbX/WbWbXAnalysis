/*
 * resultCombiner.h
 *
 *  Created on: Aug 19, 2014
 *      Author: kiesej
 */

#ifndef RESULTCOMBINER_H_
#define RESULTCOMBINER_H_
#include "variateContainer1D.h"
#include <vector>
#include "TString.h"
#include "simpleFitter.h"

namespace ztop{

//small helper
class matrix{
public:
	matrix(size_t sizei, size_t sizej){
		std::vector<double> tmp(sizei,0);
		matrix_ = std::vector<std::vector<double> > (sizej, tmp);
	}

	const double& getEntry(size_t i,size_t j)const{return matrix_.at(i).at(j);}
	void setEntry(const double& content, size_t i,size_t j){matrix_.at(i).at(j)=content;}

	void setDiagonal(){ if(matrix_.size()>0 && matrix_.size() == matrix_.at(0).size()) for(size_t i=0;i<matrix_.size();i++) setEntry(1,i,i);}

private:
	std::vector<std::vector<double> > matrix_;

};


/**
 * class to combine results
 * the input should be in form of container1Ds
 * additional constraint could be "normalized"
 * put in diff assumptions for initial syst distr (gaus, box, ...)
 * for now treat exp. uncertainties as gaussian (keep in mind change)
 * right now only correlations 1 or 0. but keep in mind extension
 *   (plug in dummy factors? or at least leave room)
 *
 * First fill in containers, then define syst forms
 *
 * needs a conversion from container1D <-> vector<extendedVariable> + bins
 * put this conversion as part of container1D class! or make own class?
 * -->variateContainer1D
 */
class resultCombiner{
public:

	enum rc_sys_forms{rc_sysf_gaus,rc_sysf_box};
	enum rc_add_constraints{rc_addc_none,rc_addc_normalized};

	resultCombiner():success_(false),additionalconstraint_(rc_addc_none),forcednorm_(0){}
	~resultCombiner(){}

	void setAdditionalConstraint(rc_add_constraints c){additionalconstraint_=c;}

	void setSystForm(const TString& sys,rc_sys_forms form);


	void addInput(const container1D& ); //reset success here, check binning

	void setLastCorrMatrix(double content, size_t i,size_t j);

	bool minimize();
	/* invoke a fitter temporarily? -> no access to correlations etc */

	const simpleFitter* getFitter()const{return &fitter_;}

	const container1D& getOutput(){return combined_;}

	void reset(); //clears every input, resets fitter, deletes syst forms, resets additional constraints

	void coutSystBreakDownInBin(size_t idx)const;

// TBI!	void clear(); //clears inputs, keeps all syst_forms and add constr

private:

	double getChi2(const double * variations); //main functione

	std::vector<variateContainer1D> distributions_;
	std::vector<matrix>  statcorrelations_;

	bool success_;
	container1D combined_,temp_;
	//std::vector<double> workingentries_;

	rc_add_constraints additionalconstraint_;
	double forcednorm_;

	simpleFitter fitter_;
	std::vector<rc_sys_forms> sysforms_;

	//nuisance functions - all assume central @ 0
	double getNuisanceLogGaus(const double & )const;
	double getNuisanceLogBox(const double & )const;
	double getNuisanceLogNormal(const double & )const;

	double sq(const double&)const;

};

inline double resultCombiner::sq(const double&a)const {
	return a*a;
}

}




#endif /* RESULTCOMBINER_H_ */
