/*
 * canvasStyle.cc
 *
 *  Created on: Sep 30, 2013
 *      Author: kiesej
 */



#include "../interface/canvasStyle.h"
#include <iostream>

namespace ztop{
/*
enum canvasTemplate{normalStackCanvas,stackPlusRatioCanvas,crosssectionPlusRatioCanvas};

	canvasStyle();
	canvasStyle(const std::vector<containerStyle> &,std::vector<float> sd );
	~canvasStyle();

	void setDivisions(size_t div);
	void setStyle(size_t idx,const containerStyle&);
	void setStyle(const containerStyle&);

	void useTemplate(canvasTemplate temp);

void clear();
private:
	std::vector<containerStyle> contstyles_;
	std::vector<float> divs_;
 */
canvasStyle::canvasStyle(){useTemplate(normalPlotCanvas);}
canvasStyle::canvasStyle(canvasTemplate temp){
	useTemplate(temp);
}
canvasStyle::canvasStyle(const std::vector<plotStyle> & plotstyles,const std::vector<float> & divs ): plotstyles_(plotstyles),divs_(divs){}
canvasStyle::~canvasStyle(){}

plotStyle * canvasStyle::getPlotStyle(size_t idx){
	if(idx>=plotstyles_.size()){
		std::cout << "canvasStyle::getPlotStyle: out of range return NULL" << std::endl;
		return 0;
	}
	return &plotstyles_.at(idx);
}
const plotStyle * canvasStyle::getPlotStyle(size_t idx) const{
	if(idx>=plotstyles_.size()){
		std::cout << "canvasStyle::getPlotStyle: out of range return NULL" << std::endl;
		return 0;
	}
	return &plotstyles_.at(idx);
}

/**
 * deletes all styles
 */
void canvasStyle::setNDivisions(size_t div){
	divs_.clear();
	plotstyles_.clear();
	if(div!=0){
		for(float i=0;i<(float)div;i+=1/(float)div) divs_.push_back(i);
		for(float i=0;i<=div;i+=1/div)
			plotstyles_.push_back(plotStyle());
	}
	else{
		plotstyles_.push_back(plotStyle());
	}
}
/**
 * deletes all styles
 */
void canvasStyle::setDivisions(const std::vector<float> &divs){
	std::cout << "canvasStyle::setDivision: check ORDERING" <<std::endl; //MISSING
	divs_=divs;
	plotstyles_.clear();
	for(float i=0;i<=divs_.size();i++)
		plotstyles_.push_back(plotStyle());
}
void canvasStyle::setDivision(size_t idx, float div){
	if(idx>=divs_.size()){
		std::cout << "canvasStyle::setDivision: idx out of range, doing nothing" <<std::endl;
		return;
	}
	divs_.at(idx)=div;
}
const float & canvasStyle::getDivision(const size_t &idx) const{
	if(idx>=divs_.size()){
		std::cout << "canvasStyle::getDivision: idx out of range" <<std::endl;
	}
	return divs_.at(idx);
}
float canvasStyle::getPadHeight(const size_t & idx) const{
	float height=1;
	if(idx>0 && idx<divs_.size())
		height=(divs_.at(idx)-divs_.at(idx-1));
	return height;
}
void canvasStyle::setStyle(size_t idx,const plotStyle& style){
	if(idx>=plotstyles_.size()){
		std::cout << "canvasStyle::setStyle: idx out of range!" <<std::endl;
		return;
	}
	plotstyles_.at(idx)=style;
}
/**
 * clears everything and adds only ONE style for one subpad
 */
void canvasStyle::setStyle(const plotStyle& style){
	divs_.clear();
	plotstyles_.clear();
	plotstyles_.push_back(style);
}
bool canvasStyle::check(){
	if(plotstyles_.size() != divs_.size() +1)
		return false;
	return true;
}
void canvasStyle::clear(){
	plotstyles_.clear();
	divs_.clear();
}

void canvasStyle::newTextBox(float x,float y,const TString& text,float textsize){
	textboxes_.push_back(textBox(x,y,text,textsize));
}
void canvasStyle::useTemplate(canvasTemplate temp){ //singlePlot,stackPlot,ratioPlot,crosssectionsGenPlot};
	if(temp==normalPlotCanvas){
		setNDivisions(0);
		plotStyle style;
		style.useTemplate(plotStyle::singlePlot);
		setStyle(0,style);
	}
	else if(temp==normalStackCanvas){
		setNDivisions(0);
		plotStyle style;
		style.useTemplate(plotStyle::stackPlot);
		setStyle(0,style);
	}
	else if(temp==stackPlusRatioCanvas){
		setNDivisions(1);
		setDivision(0,0.3);
		plotStyle stack(plotStyle::stackPlot),ratio(plotStyle::ratioPlot);
		for(size_t i=0;i<stack.size();i++)
			stack.bottomMargin=0;
		for(size_t i=0;i<ratio.size();i++)
			ratio.topMargin=0;
		setStyle(0,stack);
		setStyle(1,ratio);
	}
	else if(temp==crosssectionPlusRatioCanvas){
		setNDivisions(1);
		setDivision(0,0.3);
		plotStyle xsec(plotStyle::crosssectionsGenPlot),ratio(plotStyle::ratioPlot);
		for(size_t i=0;i<xsec.size();i++)
			xsec.bottomMargin=0;
		for(size_t i=0;i<ratio.size();i++)
			ratio.topMargin=0;
		setStyle(0,xsec);
		setStyle(1,ratio);
	}
}

}//namespace
