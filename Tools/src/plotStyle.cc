/*
 * plotStyle.cc
 *
 *  Created on: Oct 1, 2013
 *      Author: kiesej
 */

#include "../interface/plotStyle.h"
#include <iostream>
#include "../interface/fileReader.h"
#include "TH1.h"
#include "TVirtualPad.h"
#include <stdexcept>

namespace ztop{
plotStyle::plotStyle():bottomMargin(0.15),topMargin(0.05),leftMargin(0.15),rightMargin(0.05),divideByBinWidth(true),horizontal(false){setAxisDefaults();}
//plotStyle::plotStyle(templates temp):bottomMargin(0.15),topMargin(0.05),leftMargin(0.15),rightMargin(0.05),divideByBinWidth(true),horizontal(false){useTemplate(temp);}
plotStyle::~plotStyle(){}



void plotStyle::readFromFile(const std::string & filename,const std::string  stylename){
    fileReader fr;
    fr.setComment("$");
    fr.setDelimiter(",");
    fr.setStartMarker("[plotStyle - "+stylename+']');
    fr.setEndMarker("[end plotStyle]");
    fr.readFile(filename);
    if(fr.nLines()<1){
        std::cout << "plotStyle::readFromFile: did not find style "  << stylename <<std::endl;
        throw std::runtime_error("plotStyle::readFromFile: no plotStyle found");
    }


    bottomMargin       = fr.getValue<float>("bottomMargin");
    topMargin          = fr.getValue<float>("topMargin");
    leftMargin         = fr.getValue<float>("leftMargin");
    rightMargin        = fr.getValue<float>("rightMargin");

    divideByBinWidth   = fr.getValue<bool>("divideByBinWidth");
    horizontal         = fr.getValue<bool>("horizontal");

    xaxis_.titleSize   = fr.getValue<float>("xaxis.titleSize");
    xaxis_.labelSize   = fr.getValue<float>("xaxis.labelSize");
    xaxis_.titleOffset = fr.getValue<float>("xaxis.titleOffset");
    xaxis_.labelOffset = fr.getValue<float>("xaxis.labelOffset");
    xaxis_.tickLength  = fr.getValue<float>("xaxis.tickLength");
    xaxis_.ndiv        = fr.getValue<int>  ("xaxis.ndiv");
    xaxis_.max         = fr.getValue<float>("xaxis.max");
    xaxis_.min         = fr.getValue<float>("xaxis.min");
    xaxis_.log         = fr.getValue<bool> ("xaxis.log");
    xaxis_.name         = fr.getValue<std::string> ("xaxis.name");


    yaxis_.titleSize   = fr.getValue<float>("yaxis.titleSize");
    yaxis_.labelSize   = fr.getValue<float>("yaxis.labelSize");
    yaxis_.titleOffset = fr.getValue<float>("yaxis.titleOffset");
    yaxis_.labelOffset = fr.getValue<float>("yaxis.labelOffset");
    yaxis_.tickLength  = fr.getValue<float>("yaxis.tickLength");
    yaxis_.ndiv        = fr.getValue<int>  ("yaxis.ndiv");
    yaxis_.max         = fr.getValue<float>("yaxis.max");
    yaxis_.min         = fr.getValue<float>("yaxis.min");
    yaxis_.log         = fr.getValue<bool> ("yaxis.log");
    yaxis_.name         = fr.getValue<std::string> ("yaxis.name");

}


void plotStyle::absorbYScaling(float val){
    //for(size_t i=0;i<size();i++)
    //	getStyle(i)->multiplySymbols(val);
    xAxisStyle()->labelSize*= val;
    xAxisStyle()->tickLength*= val;
    //xAxisStyle()->titleOffset*= val;
    xAxisStyle()->titleSize*= val;
   // xAxisStyle()->labelOffset*= val;

    yAxisStyle()->labelSize*= val;
    //yAxisStyle()->tickLength*= val;
     yAxisStyle()->titleOffset/= val;
    yAxisStyle()->titleSize*= val;
    //yAxisStyle()->labelOffset*= val;


    bottomMargin*=val;
    topMargin*=val;

}

void plotStyle::absorbXScaling(float val){
    //for(size_t i=0;i<size();i++)
    //  getStyle(i)->multiplySymbols(val);
    // xAxisStyle()->labelSize*= val;
    // xAxisStyle()->tickLength*= val;
    //  xAxisStyle()->titleOffset*= val;
    // xAxisStyle()->titleSize*= val;
    // xAxisStyle()->labelOffset*= val;

    // yAxisStyle()->labelSize*= val;
    yAxisStyle()->tickLength*= val;
    yAxisStyle()->titleOffset*= val;
    // yAxisStyle()->titleSize*= val;
    yAxisStyle()->labelOffset*= val;


    leftMargin*=val;
    rightMargin*=val;
}


void plotStyle::applyAxisStyle(TH1*h)const{
    h->GetXaxis()->SetTitleSize(xaxis_.titleSize);
    h->GetXaxis()->SetTitleOffset(xaxis_.titleOffset);
    h->GetXaxis()->SetLabelSize(xaxis_.labelSize);
    h->GetXaxis()->SetLabelOffset(xaxis_.labelOffset);
    h->GetXaxis()->SetTickLength(xaxis_.tickLength);
    h->GetXaxis()->SetNdivisions(xaxis_.ndiv);
    if(!xaxis_.name.Contains("USEDEF"))
        h->GetXaxis()->SetTitle(xaxis_.name);
    if(xaxis_.applyAxisRange())
        h->GetXaxis()->SetRangeUser(xaxis_.min,xaxis_.max);

    h->GetYaxis()->SetTitleSize(yaxis_.titleSize);
    h->GetYaxis()->SetTitleOffset(yaxis_.titleOffset);
    h->GetYaxis()->SetLabelSize(yaxis_.labelSize);
    h->GetYaxis()->SetLabelOffset(yaxis_.labelOffset);
    h->GetYaxis()->SetTickLength(yaxis_.tickLength);
    h->GetYaxis()->SetNdivisions(yaxis_.ndiv);
    if(!yaxis_.name.Contains("USEDEF"))
        h->GetYaxis()->SetTitle(yaxis_.name);
    if(yaxis_.applyAxisRange())
        h->GetYaxis()->SetRangeUser(yaxis_.min,yaxis_.max);

}
void plotStyle::applyPadStyle(TVirtualPad*c)const{
    c->SetBottomMargin(bottomMargin);
    c->SetTopMargin(topMargin);
    c->SetLeftMargin(leftMargin);
    c->SetRightMargin(rightMargin);
    if(yaxis_.log)  c->SetLogy();
    if(xaxis_.log)  c->SetLogx();
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////









void plotStyle::setAxisDefaults(){

    xAxisStyle()->labelSize= 0.05;
    xAxisStyle()->max= -1;
    xAxisStyle()->min= 1;
    xAxisStyle()->ndiv= 510;
    xAxisStyle()->tickLength= 3e-2;
    xAxisStyle()->titleOffset= 0.8;
    xAxisStyle()->titleSize= 0.06;
    xAxisStyle()->labelOffset= 0.005;

    yAxisStyle()->labelSize= 0.05;
    yAxisStyle()->max= -1;
    yAxisStyle()->min= 1;
    yAxisStyle()->ndiv= 510;
    yAxisStyle()->tickLength= 3e-2;
    yAxisStyle()->titleOffset= 0.8;
    yAxisStyle()->titleSize= 0.06;
    yAxisStyle()->labelOffset= 0.005;

}
/*
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
 */

}//namespace

