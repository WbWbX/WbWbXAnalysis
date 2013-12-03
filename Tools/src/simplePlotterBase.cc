/*
 * simplePlotterBase.cc
 *
 *  Created on: Nov 8, 2013
 *      Author: kiesej
 */



#include "../interface/simplePlotterBase.h"
#include "TGraphAsymmErrors.h"
#include "TH1D.h"
#include "TCanvas.h"
#include <iostream>

namespace ztop{
simplePlotterBase::simplePlotterBase(): c_(0),drawsame_(false) {TH1::AddDirectory(false);}

simplePlotterBase::~simplePlotterBase(){
	cleanMem();
}

void simplePlotterBase::cleanMem(){
	for(size_t i=0;i<allobj_.size();i++){
		if(allobj_.at(i)) delete allobj_.at(i);
	}
	allobj_.clear();
	if(c_) delete c_;
	c_=0;
}
TCanvas * simplePlotterBase::getCanvas(){
	if(c_) return c_;
	c_=new TCanvas();
	return c_;
}
void simplePlotterBase::setDrawSame(bool drawsame){
	if(!c_){
		std::cout << "simplePlotterBase::setDrawSame: first create or specify canvas " << std::endl;
		return;
	}
	drawsame_=drawsame;
}
float simplePlotterBase::getLabelMultiplier(){
	float multiplier=1;
	double ylow,yhigh,xlow,xhigh;
	if(gPad){
		gPad->GetPadPar(xlow,ylow,xhigh,yhigh);
		multiplier = (float)1/(yhigh-ylow);
	}
	return multiplier;
}
simplePlotterBase & simplePlotterBase::operator = (const simplePlotterBase& rhs){
	if(&rhs==this) return *this;
	//not same
	cleanMem();
	return *this;
}

void simplePlotterBase::setColors(TGraphAsymmErrors * g,int col){
	/* get style options here */
	g->SetLineColor(col);
	g->SetMarkerColor(col);
	g->SetFillColor(col);

}
void simplePlotterBase::setColors(TH1D * h,int col){

	h->SetLineColor(col);
	h->SetMarkerColor(col);
	h->SetFillColor(col);

}

}
