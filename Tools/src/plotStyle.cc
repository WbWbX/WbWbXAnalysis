/*
 * plotStyle.cc
 *
 *  Created on: Oct 1, 2013
 *      Author: kiesej
 */

#include "../interface/plotStyle.h"
#include <iostream>


namespace ztop{
plotStyle::plotStyle():bottomMargin(0.15),topMargin(0.05),leftMargin(0.15),rightMargin(0.05),divideByBinWidth(true),horizontal(false),legxlow(0.65),legxhigh(0.95),legylow(0.5),legyhigh(0.9) {setAxisDefaults();}
plotStyle::plotStyle(templates temp):bottomMargin(0.15),topMargin(0.05),leftMargin(0.15),rightMargin(0.05),divideByBinWidth(true),horizontal(false),legxlow(0.65),legxhigh(0.95),legylow(0.5),legyhigh(0.9){useTemplate(temp);}
plotStyle::~plotStyle(){}

containerStyle * plotStyle::getStyle(size_t idx){
	if(idx>=size()){
		std::cout << "plotStyle::getStyle: idx out of range: return NULL " << std::endl;
		return 0;
	}
	return &cstyles_.at(idx);
}
const containerStyle * plotStyle::getStyle(size_t idx) const{
	if(idx>=size()){
		std::cout << "plotStyle::getStyle: idx out of range: return NULL " << std::endl;
		return 0;
	}
	return &cstyles_.at(idx);
}


void plotStyle::multiplySymbols(float val){
	for(size_t i=0;i<size();i++)
		getStyle()->multiplySymbols(val);
	xAxisStyle()->labelSize*= val;
	xAxisStyle()->tickLength*= val;
	xAxisStyle()->titleOffset*= val;
	xAxisStyle()->titleSize*= val;
	xAxisStyle()->labelOffset*= val;

	yAxisStyle()->labelSize*= val;
	yAxisStyle()->tickLength*= val;
	yAxisStyle()->titleOffset*= val;
	yAxisStyle()->titleSize*= val;
	yAxisStyle()->labelOffset*= val;
}

void plotStyle::setAxisDefaults(){

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

}
void plotStyle::setLegXRange(float xlow,float xhigh){
	legxlow=xlow;
	legxhigh=xhigh;
}

void plotStyle::setLegYRange(float ylow,float yhigh){
	legylow=ylow;
	legyhigh=yhigh;
}
void plotStyle::useTemplate(templates temp){
	setAxisDefaults();
	if(temp==singlePlot){
		clear();
		containerStyle cstyle;
		cstyle.useTemplate(containerStyle::normalPlot);
		addStyle(cstyle);


		bottomMargin=0.15;
		topMargin=0.05;
		leftMargin=0.15;
		rightMargin=0.05;
	}
	else if(temp==stackPlot){ //one for data, one for MC, leave colors open
		clear();
		containerStyle data,mc;
		data.useTemplate(containerStyle::controlPlotData);
		mc.useTemplate(containerStyle::controlPlotMC);
		addStyle(data);
		addStyle(mc);
		bottomMargin=0.15;
		topMargin=0.05;
		leftMargin=0.15;
		rightMargin=0.05;
	}
	else if(temp==ratioPlot){
		clear();
		containerStyle data,mc;
		data.useTemplate(containerStyle::ratioPlotData);
		mc.useTemplate(containerStyle::ratioPlotMC);
		addStyle(data);
		addStyle(mc);
		yAxisStyle()->max= 1.5;
		yAxisStyle()->min= 0.5;
		yAxisStyle()->ndiv=505;
		bottomMargin=0.15;
		topMargin=0.05;
		leftMargin=0.15;
		rightMargin=0.05;
	}
	else if(temp==crosssectionsGenPlot){
		clear();
		containerStyle data,mc;
		data.useTemplate(containerStyle::crosssectionPlot);
		mc.useTemplate(containerStyle::crosssectionPlot);
		mc.errorStyle = containerStyle::perpErr;
		mc.fillColor = 632;
		mc.lineColor = 632;
		mc.markerColor = 632;
		addStyle(data);
		addStyle(mc);
		bottomMargin=0.15;
		topMargin=0.05;
		leftMargin=0.15;
		rightMargin=0.05;
	}
	else if(temp==systematicsBreakdownPlot){
		clear();
		containerStyle bars(containerStyle::normalPlot),errors(containerStyle::normalPlot);
		bars.errorStyle = containerStyle::noErr;
		bars.fillStyle=3003;
		errors.fillStyle=3005;
		errors.errorStyle=containerStyle::fillErr;
		bars.fillColor=kRed;
		containerStyle bars2=bars;
		bars2.fillColor=kBlue;
		bars2.fillStyle=3002;
		errors.fillColor=kBlack;
		addStyle(bars);
		addStyle(bars2);
		addStyle(errors);
		horizontal=true;
		bottomMargin=0.15;
		topMargin=0.05;
		leftMargin=0.15;
		rightMargin=0.05;
	}

}


}//namespace

