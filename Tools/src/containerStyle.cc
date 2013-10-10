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

bool containerAxisStyle::applyAxisRange() const{
	if(max<min) return false;
	return true;
}


textBox::textBox(): x_(0),y_(0),text_(""),textsize_(0){}
textBox::textBox(float x,float y,const TString &text,float textsize):  x_(0),y_(0),text_(text),textsize_(textsize){}
textBox::~textBox(){}
void textBox::setText(const TString & text){text_=text;}
void textBox::setTextSize(float textsize){textsize_=textsize;}
void textBox::setCoords(float x,float y){
	x_=x;
	y_=y;
}
const TString & textBox::getText() const{return text_;}
const float &textBox::getTextSize() const{return textsize_;}
const float &textBox::getX() const{return x_;}
const float &textBox::getY() const{return y_;}

//////////////////////////




containerStyle::containerStyle(): markerSize(0),markerStyle(0),markerColor(0),lineSize(0),lineStyle(0),
		lineColor(0),fillStyle(0),fillColor(0),errorStyle(normalErr),topMargin(0),bottomMargin(0),
		leftMargin(0),rightMargin(0) {
	useTemplate(normalPlot);
}
containerStyle::containerStyle(templates temp):  markerSize(0),markerStyle(0),markerColor(0),lineSize(0),lineStyle(0),
		lineColor(0),fillStyle(0),fillColor(0),errorStyle(normalErr),topMargin(0),bottomMargin(0),
		leftMargin(0),rightMargin(0){
	useTemplate(temp);
}
containerStyle::~containerStyle(){}


void containerStyle::multiplySymbols(float val){
	markerSize*=val;
	lineSize= (int)((float)lineSize * val);

}

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




	} */
}

}

