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
#include <cmath>

namespace ztop{
plotStyle::plotStyle():bottomMargin(0.15),topMargin(0.05),leftMargin(0.15),rightMargin(0.05),divideByBinWidth(true),horizontal(false){setAxisDefaults();}
//plotStyle::plotStyle(templates temp):bottomMargin(0.15),topMargin(0.05),leftMargin(0.15),rightMargin(0.05),divideByBinWidth(true),horizontal(false){useTemplate(temp);}
plotStyle::~plotStyle(){}



void plotStyle::readFromFile(const std::string & filename,const std::string  stylename, bool requireall){
	fileReader fr;
	fr.setComment("$");
	fr.setDelimiter(",");
	fr.setStartMarker("[plotStyle - "+stylename+"]");
	fr.setEndMarker("[end plotStyle]");
	fr.readFile(filename);
	if(fr.nLines()<1 && requireall){
		std::cout << "plotStyle::readFromFile: did not find style "  << stylename <<std::endl;
		throw std::runtime_error("plotStyle::readFromFile: no plotStyle found");
	}
	fr.setRequireValues(requireall);

	bottomMargin       = fr.getValue<float>("bottomMargin",bottomMargin);
	topMargin          = fr.getValue<float>("topMargin",topMargin);
	leftMargin         = fr.getValue<float>("leftMargin",leftMargin);
	rightMargin        = fr.getValue<float>("rightMargin",rightMargin);

	divideByBinWidth   = fr.getValue<bool>("divideByBinWidth",divideByBinWidth);
	horizontal         = fr.getValue<bool>("horizontal",horizontal);

	xaxis_.titleSize   = fr.getValue<float>("xaxis.titleSize",xaxis_.titleSize);
	xaxis_.labelSize   = fr.getValue<float>("xaxis.labelSize",xaxis_.labelSize);
	xaxis_.titleOffset = fr.getValue<float>("xaxis.titleOffset",xaxis_.titleOffset);
	xaxis_.labelOffset = fr.getValue<float>("xaxis.labelOffset",xaxis_.labelOffset);
	xaxis_.tickLength  = fr.getValue<float>("xaxis.tickLength",xaxis_.tickLength);

	xaxis_.ndiv        = fr.getValue<int>  ("xaxis.ndiv", xaxis_.ndiv);
	xaxis_.max         = fr.getValue<float>("xaxis.max",xaxis_.max);
	xaxis_.min         = fr.getValue<float>("xaxis.min",xaxis_.min);
	xaxis_.log         = fr.getValue<bool> ("xaxis.log",xaxis_.log );
	xaxis_.name         = fr.getValue<TString> ("xaxis.name",xaxis_.name);

	yaxis_.titleSize   = fr.getValue<float>("yaxis.titleSize",yaxis_.titleSize);
	yaxis_.labelSize   = fr.getValue<float>("yaxis.labelSize",yaxis_.labelSize);
	yaxis_.titleOffset = fr.getValue<float>("yaxis.titleOffset",yaxis_.titleOffset);
	yaxis_.labelOffset = fr.getValue<float>("yaxis.labelOffset",yaxis_.labelOffset);
	yaxis_.tickLength  = fr.getValue<float>("yaxis.tickLength",yaxis_.tickLength);
	yaxis_.ndiv        = fr.getValue<int>  ("yaxis.ndiv",yaxis_.ndiv  );
	yaxis_.max         = fr.getValue<float>("yaxis.max",yaxis_.max);
	yaxis_.min         = fr.getValue<float>("yaxis.min",yaxis_.min);
	yaxis_.log         = fr.getValue<bool> ("yaxis.log",yaxis_.log);
	yaxis_.name         = fr.getValue<TString> ("yaxis.name",yaxis_.name);


	fr.setRequireValues(false);
	yaxis_.forcezero   = fr.getValue<bool> ("yaxis.forcezero",yaxis_.forcezero);

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
	yAxisStyle()->tickLength*= sqrt(val);
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

	//new
	// yAxisStyle()->labelSize*= val;
	yAxisStyle()->tickLength*= val;
	yAxisStyle()->titleOffset*= val;

	//new
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
		h->SetAxisRange(xaxis_.min,xaxis_.max,"X");

	h->GetYaxis()->SetTitleSize(yaxis_.titleSize);
	h->GetYaxis()->SetTitleOffset(yaxis_.titleOffset);
	h->GetYaxis()->SetLabelSize(yaxis_.labelSize);
	h->GetYaxis()->SetLabelOffset(yaxis_.labelOffset);
	h->GetYaxis()->SetTickLength(yaxis_.tickLength);
	h->GetYaxis()->SetNdivisions(yaxis_.ndiv);

	if(!yaxis_.name.Contains("USEDEF"))
		h->GetYaxis()->SetTitle(yaxis_.name);
	if(yaxis_.applyAxisRange())
		h->SetAxisRange(yaxis_.min,yaxis_.max,"Y");

}
void plotStyle::applyPadStyle(TVirtualPad*c)const{
	c->SetBottomMargin(bottomMargin);
	c->SetTopMargin(topMargin);
	c->SetLeftMargin(leftMargin);
	c->SetRightMargin(rightMargin);
	int number=c->GetNumber();
	// Apparently this is necessary in glorious root

	c->GetMother()->cd(number);
	//c->cd();
	if(yaxis_.log)
		gPad->SetLogy(1);
	else
		gPad->SetLogy(0);
	if(xaxis_.log)
		gPad->SetLogx(1);
	else
		gPad->SetLogx(0);
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


}//namespace

