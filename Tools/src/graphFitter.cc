/*
 * graphFitter.cc
 *
 *  Created on: May 23, 2014
 *      Author: kiesej
 */


#include "../interface/graphFitter.h"




namespace ztop{

graphFitter::graphFitter():simpleFitter(),shiftxy_(false),shiftx_(0),shifty_(0){}


double graphFitter::getFitOutput(const double& xin)const{
	if(shiftxy_){
		return simpleFitter::getFitOutput(xin-shiftx_) + shifty_;
	}
	else{
		return simpleFitter::getFitOutput(xin);
	}
}


void graphFitter::readGraph(const graph* g){

	clearPoints();

	std::vector<point2D> nompoints;
	std::vector<point2D> errsup;
	std::vector<point2D> errsdown;
	//translate
	graph temp;
	if(shiftxy_){


		temp=*g;
		g=&temp;

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

