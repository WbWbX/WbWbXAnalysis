/*
 * simpleFitter.h
 *
 *  Created on: May 22, 2014
 *      Author: kiesej
 */

#ifndef SIMPLEFITTER_H_
#define SIMPLEFITTER_H_

#include <TROOT.h>
#include <vector>
#include "Math/Functor.h"
#include "Math/Minimizer.h"
#include <algorithm>

#include "corrMatrix.h"
#include "TtZAnalysis/Tools/interface/graph.h"
namespace ztop{
class histo2D;

/**
 * small 2d point helper
 *
 * */
class point2D{
public:
	point2D(): x(0),y(0){}
	point2D(const double xin, const double yin): x(xin),y(yin){}
	double x,y;
};

/**
 * only for functionality
 * no interface to ztop data formats!
 *
 * just keep it simple at first
 *
 * minimizes chi2
 *
 *
 * MAY ALLOW FOR PARALLELIZATION!!!! PROBLEM WITH TMINUIT
 *
 */
class simpleFitter{
public:
	/**
	 * Fit functions:
	 * fm_pol: polynomial:
	 *  - 0th parameter: offset, next ones x^(parno) * para
	 *
	 * fm_gaus:
	 *  - 0th parameter: normalization
	 *  - 1st parameter: x- shift
	 *  - 2nd parameter: width
	 *
	 * fm_offgaus:
	 *  - 0th parameter: offset
	 *  - 1st parameter: normalization
	 *  - 2st parameter: x- shift
	 *  - 3nd parameter: width
	 */
	enum fitmodes{fm_pol0,fm_pol1,fm_pol2,fm_pol3,fm_pol4,fm_pol5,fm_pol6,fm_gaus,fm_offgaus,fm_exp};
	enum printlevels{pl_silent,pl_normal,pl_verb}; //TBI
	enum minimizers{mm_minuitMinos,mm_minuit2};

	simpleFitter();
	~simpleFitter();
	simpleFitter(const simpleFitter&);
	simpleFitter& operator=(const simpleFitter&);
	/**
	 * Available fit modes:
	 * <some docu>
	 */
	void setFitMode(fitmodes fitmode);
	void setMinimizer(minimizers min){minimizer_=min;}
	void setMinFunction( ROOT::Math::Functor* f);
	void setAlgorithm(const TString& algo){algorithm_=algo;}
	void setMinimizer(const TString& minmizer){minimizerstr_=minmizer;}

	size_t hasNParameters(fitmodes)const;

	void addPoint(const double & px, const double & py){nompoints_.push_back(point2D(px,py));}
	void setPoints(const std::vector<point2D> inp){nompoints_=inp;}

	void addYError(const double & err){errsup_.push_back(point2D(0,err));errsdown_.push_back(point2D(0,err));}
	void setErrorsUp(const std::vector<point2D> inp){errsup_=inp;}
	void setErrorsDown(const std::vector<point2D> inp){errsdown_=inp;}

	/**
	 * these are then considered as start values
	 * if not defined, will assume 0 for all parameters
	 * and step sizes of 0.01 for all parameters
	 */
	void setParameters(const std::vector<double>& inpars,const std::vector<double>& steps=std::vector<double>());
	void setParameterNames(const std::vector<TString>& nms){paranames_=nms;}

	void setParameter(size_t idx,double value);
	void setParameterFixed(size_t idx,bool fixed=true);
	void setParameterLowerLimit(size_t idx,double value);
	void setParameterUpperLimit(size_t idx,double value);
	void removeParameterLowerLimit(size_t idx);
	void removeParameterUpperLimit(size_t idx);


	void setRequireFitFunction(bool req){requirefitfunction_=req;}

	void setMaxCalls(unsigned  int calls){maxcalls_=calls;}

	void setAsMinosParameter(size_t parnumber,bool set=true);
	void setTolerance(double tol){tolerance_=tol;}

	double getFitOutput(const double& xin)const;

	static int printlevel;

	void setStrategy(int str){strategy_=str;}
	void feedErrorsToSteps();

	//void fit(void (*chi2function)(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag) );
	void fit();

	const std::vector<point2D> * getNomPoints()const{return &nompoints_;}
	const std::vector<point2D> * getErrsUp()const{return &errsup_;}
	const std::vector<point2D> * getErrsDown()const{return &errsdown_;}
	const std::vector<double> *getParameters()const{return &paras_;}
	/**
	 * these are signed!
	 */
	const std::vector<double> *getParameterErrUp()const{return &paraerrsup_;}
	/**
	 * these are signed!
	 */
	const std::vector<double> *getParameterErrDown()const{return &paraerrsdown_;}
	/**
	 * always > 0
	 */
	double getParameterErr(size_t idx)const;
	const double&  getParameter(size_t idx)const;
	const std::vector<TString> *getParameterNames()const {return &paranames_;}


	const double& getCorrelationCoefficient(size_t i, size_t j)const;
	void  fillCorrelationCoefficients(histo2D *)const;
	corrMatrix getCorrelationMatrix()const;

	graph getContourScan(size_t i, size_t j)const;
	/**
	 * This is not the same as the minuit scan method!
	 * The function fixes the parameter to a certain value and then
	 * minimizes the function.
	 * This function is multi-threaded. However, it can take a long time to get all
	 * the points. Try to keep the number as low as feasible
	 */
	graph scan(size_t paraidx,const double& from, const double& to, size_t npoints=12)const;

	/**
	 * gets the contribution of a to b by fixing the parameter a, repeating minos and returning the changes in errup and errdown
	 * failures are indicated by negative return values!
	 * requires parameter b to be a minos parameter
	 */
	void getParameterErrorContribution(size_t a, size_t b,double & errup, double& errdown);
	/**
	 * gets the contribution of a to b by fixing the parameters a, repeating minos and returning the changes in errup and errdown
	 * failures are indicated by negative return values!
	 * requires parameter b to be a minos parameter
	 */
	void getParameterErrorContributions(std::vector<size_t> a, size_t b,double & errup, double& errdown);

	/**
	 * Fixes all parameters and thereby determines the statistical component of the
	 * error to parameter b only
	 */
	void getStatErrorContribution( size_t b,double & errup, double& errdown);

	size_t findParameterIdx(const TString& paraname)const;

	double fitfunction(const double& x,const Double_t *par)const;
	// double fitfunction(const double& x,const std::vector<double> & par)const;

	bool wasSuccess(){return minsuccessful_;}
	bool minosWasSuccess(){return minossuccessful_;}

	/**
	 * clears all input points
	 */
	void clearPoints();

	void setOnlyRunDummy(bool dummyrun){dummyrun_=dummyrun;}


	static bool debug;


	///some handy functions that can be used

	static double nuisanceGaus(const double & in);
	static double nuisanceBox(const double & in);
	//for sure not ok!
	static double nuisanceLogNormal(const double & in);




private: //set some to protected if inheritance is needed
	fitmodes fitmode_;
	minimizers minimizer_;
	double chi2min_;

protected:
	std::vector<point2D> nompoints_;
	std::vector<point2D> errsup_;
	std::vector<point2D> errsdown_;
	std::vector<double> paras_;
private:
	std::vector<double> stepsizes_;
	std::vector<double> paraerrsup_,paraerrsdown_;
	std::vector<TString> paranames_;
	std::vector<bool> parafixed_;

	//FIXME change to corrMatrix class
	std::vector< std::vector<double> > paracorrs_;

	std::vector<int> minospars_;
	std::vector<std::pair<bool, double> > lowerlimits_;
	std::vector<std::pair<bool, double> > upperlimits_;

	unsigned int  maxcalls_;

	//definitely privateL

	//1D section
	//this is moved to global due to tminuit reasons
	// void chisq(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);


	bool checkSizes()const;

	bool requirefitfunction_;
protected:
	bool minsuccessful_,minossuccessful_;
private:
	double tolerance_;
	ROOT::Math::Functor* functobemin_;
	TString algorithm_;
	TString minimizerstr_;

	ROOT::Math::Minimizer * pminimizer_;


	void copyFrom(const simpleFitter&);


	ROOT::Math::Minimizer * invokeMinimizer()const;


	class simpleChi2 {
	public:
		simpleChi2();
		simpleChi2(simpleFitter * fp);
		double operator() (const double* par) const;
		double Up() const;
	private:
		simpleFitter* fp_;
		size_t npars_;
	}chi2func_;

	int strategy_;
	bool dummyrun_;
};

}


#endif /* SIMPLEFITTER_H_ */
