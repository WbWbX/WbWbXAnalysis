/*
 * graphFitter.h
 *
 *  Created on: May 23, 2014
 *      Author: kiesej
 */

#ifndef GRAPHFITTER_H_
#define GRAPHFITTER_H_

#include "simpleFitter.h"
#include "../interface/graph.h"

namespace ztop{


class graphFitter: public simpleFitter{
public:
	graphFitter();
	~graphFitter(){}


	/**
	 * If set to true, the graph is not fitted
	 * Instead, the space between the points is
	 * described by linear functions
	 */
	void setInterpolate(bool set){interpolate_=set;}



	/**
	 * reads points, errors etc as input to fitting
	 * systematic (if any) uncertainties are added in quadrature
	 */
	void readGraph(const graph* );

	void fit();

	/**
	 * this allows the fit to shift the graph in x and y to approx 0
	 * and makes fits converge easier.
	 * shifts will be corrected when using the getFitOutput(const double& xin)const;
	 * function. parameters should NOT be fed to other functions!!
	 */
	void setDoXYShift(bool dos){shiftxy_=dos;}

	const float& getXShift()const{return shiftx_;}
	const float& getYShift()const{return shifty_;}
	void setXShift(const float& shift){ shiftx_=shift;}
	void setYShift(const float& shift){ shifty_=shift;}

	graph exportFittedCurve(size_t npoints=100) const;

	double getFitOutput(const double& xin)const;

private:

	bool shiftxy_;
	float shiftx_,shifty_;

	//shadow some to be not used
	void addPoint(const double & px, const double & py){simpleFitter::addPoint(px,py);}
	void setPoints(const std::vector<point2D> inp){simpleFitter::setPoints(inp);}

	void addYError(const double & err){simpleFitter::addYError(err);}
	void setErrorsUp(const std::vector<point2D> inp){simpleFitter::setErrorsUp(inp);}
	void setErrorsDown(const std::vector<point2D> inp){simpleFitter::setErrorsDown(inp);}

	void setRequireFitFunction(bool req){simpleFitter::setRequireFitFunction(req);}

	const std::vector<point2D> * getNomPoints()const{return simpleFitter::getNomPoints();}
	const std::vector<point2D> * getErrsUp()const{return simpleFitter::getErrsUp();}
	const std::vector<point2D> * getErrsDown()const{return simpleFitter::getErrsDown();}

	bool interpolate_;

	std::vector<double> interpolateparas_a_;
	std::vector<double> interpolateparas_b_;
	//this is mathematically redundant but makes handling easier
	std::vector<double> interpolateparas_c_;

	graph savedinput_;

};


}



#endif /* GRAPHFITTER_H_ */
