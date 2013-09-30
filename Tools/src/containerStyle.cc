/*
 * containerStyle.cc
 *
 *  Created on: Sep 30, 2013
 *      Author: kiesej
 */

#include "../interface/containerStyle.h"

namespace ztop{

///////HELPER CLASS//////
containerAxisStyle::containerAxisStyle(): titleSize(0), labelSize(0), titleOffset(0), labelOffset(0), tickLength(0), ndiv(0), max(-1), min(1){}
containerAxisStyle::~containerAxisStyle(){}

bool containerAxisStyle::applyAxisRange(){
	if(max<min) return false;
	return true;
}

//////////////////////////




containerStyle::containerStyle(): markerSize(0),markerStyle(0),markerColor(0),lineSize(0),lineStyle(0),
		lineColor(0),fillStyle(0),fillColor(0),errorStyle(normalErr),topMargin(0),bottomMargin(0),
		leftMargin(0),rightMargin(0) {
	useTemplate(normalPlot);
}
containerStyle::~containerStyle(){}

void containerStyle::useTemplate(templates temp){
	if(temp==normalPlot){
		markerSize= 1;
		markerStyle= 20;
		markerColor= 1;
		lineSize= 1;
		lineStyle= 1;
		lineColor= 1;
		fillStyle= 0;
		fillColor= 0;
		errorStyle= perpErr;

		xAxisStyle()->labelSize= 0.05;
		xAxisStyle()->max= -1;
		xAxisStyle()->min= 1;
		xAxisStyle()->ndiv= 510;
		xAxisStyle()->tickLength= 3e-2;
		xAxisStyle()->titleOffset= 1;
		xAxisStyle()->titleSize= 0.06;
		xAxisStyle()->labelOffset= 0.005;

		yAxisStyle()->labelSize= 0.05;
		yAxisStyle()->max= -1;
		yAxisStyle()->min= 1;
		yAxisStyle()->ndiv= 510;
		yAxisStyle()->tickLength= 3e-2;
		yAxisStyle()->titleOffset= 1;
		yAxisStyle()->titleSize= 0.06;
		yAxisStyle()->labelOffset= 0.005;

		bottomMargin= 0.15;
		topMargin= 0.05;
		leftMargin= 0.15;
		rightMargin= 0.05;
	}
	else if(temp==controlPlotData){/// for all styles, maybe just change wrt normal
		useTemplate(normalPlot);
	}
	else if(temp==controlPlotMC){
		useTemplate(normalPlot);
		errorStyle=fillErr;
	}
	else if(temp==ratioPlotData){
		useTemplate(normalPlot);
		yAxisStyle()->max= 1.5;
		yAxisStyle()->min= 0.5;
		yAxisStyle()->ndiv=505;
		errorStyle = noXPerpErr;
	}
	else if(temp==ratioPlotMC){
		useTemplate(ratioPlotData);
		fillStyle = 3005;
		errorStyle = fillErr;

	}
	else if(temp==crosssectionPlot){
		useTemplate(normalPlot);
	}
	else if(temp==systematicsPlot){
		useTemplate(normalPlot);
	}
	/*else if(){ //template
		markerSize= ;
		markerStyle= ;
		markerColor= ;
		lineSize= ;
		lineStyle= ;
		lineColor= ;
		fillStyle= ;
		fillColor= ;
		errorStyle= ;

		xAxisStyle()->labelSize= ;
		xAxisStyle()->max= ;
		xAxisStyle()->min= ;
		xAxisStyle()->ndiv= ;
		xAxisStyle()->tickLength= ;
		xAxisStyle()->titleOffset= ;
		xAxisStyle()->titleSize= ;
		xAxisStyle()->labelOffset= ;

		yAxisStyle()->labelOffset= ;
		yAxisStyle()->labelSize= ;
		yAxisStyle()->max= ;
		yAxisStyle()->min= ;
		yAxisStyle()->ndiv= ;
		yAxisStyle()->tickLength= ;
		yAxisStyle()->titleOffset= ;
		yAxisStyle()->titleSize= ;


		bottomMargin= ;
		topMargin= ;
		leftMargin= ;
		rightMargin= ;
	} */
}

}

