/*
 * graphFitter.cc
 *
 *  Created on: May 23, 2014
 *      Author: kiesej
 */


#include "../interface/graphFitter.h"

#include <cfloat>


namespace ztop{

graphFitter::graphFitter():simpleFitter(),shiftxy_(false),shiftx_(0),shifty_(0),interpolate_(false){}


double graphFitter::getFitOutput(const double& xin)const{
	if(!interpolate_){
		if(shiftxy_){
			return simpleFitter::getFitOutput(xin-shiftx_) + shifty_;
		}
		else{
			return simpleFitter::getFitOutput(xin);
		}
	}
	else{
		if(interpolateparas_a_.size() < 2)
			throw std::logic_error("graphFitter::getFitOutput: in interpolate mode requires \"fit()\" in interpolate mode first");

		//find appropriate range
		size_t idx=std::lower_bound(interpolateparas_a_.begin(), interpolateparas_a_.end(), xin) - interpolateparas_a_.begin();
		if(idx!=0)
			--idx;

		return (xin-interpolateparas_a_.at(idx)) * interpolateparas_b_.at(idx) + interpolateparas_c_.at(idx);
	}
}


void graphFitter::readGraph(const graph* g){

	clearPoints();

	std::vector<point2D> nompoints;
	std::vector<point2D> errsup;
	std::vector<point2D> errsdown;
	//translate
	graph temp;
	temp=*g;
	g=&temp;
	temp.sortPointsByX();
	if(shiftxy_){

		double meanya=(g->getYMax()+g->getYMin())/2;
		double xmin=g->getXMin();
		double xmax=g->getXMax();
		float xshift = (xmax+xmin)/2;
		float yshift=(meanya);
		//preapre for better fitting

		temp.shiftAllXCoordinates(-xshift);
		temp.shiftAllYCoordinates(-yshift);
		if(debug){
			std::cout << "graphFitter::readGraph: shifting x and y value " ;
			std::cout << "\nx: " << -xshift;
			std::cout << "\ny: " << -yshift <<std::endl;
		}

		shiftx_=xshift;
		shifty_=yshift;

	}


	for(size_t i=0;i<g->getNPoints();i++){

		if(debug){
			std::cout << "graphFitter::readGraph: reading point "<< i << "("<< g->getPointXContent(i) <<","<< g->getPointYContent(i)<<")" <<std::endl;

		}

		nompoints.push_back(point2D (g->getPointXContent(i),g->getPointYContent(i)));
		errsup.push_back(point2D (g->getPointXErrorUp(i,false),g->getPointYErrorUp(i,false)));
		errsdown.push_back(point2D (g->getPointXErrorDown(i,false),g->getPointYErrorDown(i,false)));
	}
	setPoints(nompoints);
	setErrorsUp(errsup);
	setErrorsDown(errsdown);

	savedinput_=*g;

}

void graphFitter::fit(){
	if(!interpolate_)
		simpleFitter::fit();
	else{
		interpolateparas_a_.clear();
		interpolateparas_b_.clear();
		interpolateparas_c_.clear();
		for(size_t i=0;i<nompoints_.size()-1;i++){
			interpolateparas_a_.push_back(nompoints_.at(i).x);
			double b=(nompoints_.at(i+1).y - nompoints_.at(i).y) / (nompoints_.at(i+1).x - nompoints_.at(i).x);
			interpolateparas_b_.push_back(b);
			interpolateparas_c_.push_back(nompoints_.at(i).y);
		}
		minsuccessful_=true;
	}
}

graph graphFitter::exportFittedCurve(size_t npoints) const{
	//produce out

	graph out(npoints);
	size_t dummy=0;
	float minx=savedinput_.getXMin(dummy,true);
	float maxx=savedinput_.getXMax(dummy,true);

	float gran=(maxx-minx)/(float)npoints;
	size_t point=0;
	for(float x=minx;x<=maxx;x+=gran){
		if(point>=npoints) break;
		out.setPointContents(point,true,x,getFitOutput(x));
		point++;
	}

	return out;

}


}

