/*
 * plotter.cc
 *
 *  Created on: Oct 1, 2013
 *      Author: kiesej
 */




/*enum plotTemplates{standardPlot,controlPlot,controlPlusRatioPlot,controlSigBGPlot,};

	plotter();
	~plotter();

	void setStyle(containerStyle *); //for only one pad plots
	void setStyle(canvasStyle *); //for only one pad plots

	void draw(const container1D *);

private:
	canvasStyle canvasstyle_;
	std::vector<TObject *> allobj_;
 *
 */

#include "../interface/plotter.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1D.h"
#include "TGraphAsymmErrors.h"
#include "TH2D.h"
#include "TObject.h"
#include <iostream>
#include "../interface/container.h"
#include "../interface/containerStack.h"
#include "../interface/container1DUnfold.h"

namespace ztop{

plotter::plotter():canvas_(0),optstat_(0){}
plotter::plotter(const plotter & pl){
	copyfrom(pl);
}
const plotter & plotter::operator = (const plotter & pl){
	copyfrom(pl);
	return *this;
}
plotter::~plotter(){
	for(size_t i=0;i<allobj_.size();i++){
		if(allobj_.at(i)) delete allobj_.at(i);}
	if(canvas_) delete canvas_;
}
void plotter::setStyle(const canvasStyle & style){canvasstyle_=style;}

void plotter::clearPad(size_t pad){
	if(!padexists(pad)){
		std::cout << "plotter::clearPad: pad out of range, doing nothing" <<std::endl;
		return;
	}
	if(padobj_.size()<=pad) //pad was empty
		return;
	for(size_t i=0;i<padobj_.at(pad).size();i++)
		if(padobj_.at(pad).at(i)) delete padobj_.at(pad).at(i);
}

/////////////////
/**
 * plotter::draw(const TH1D * h, const size_t & pad=0,const size_t & cidx=0,bool same,  plotStyle *ps=0, containerStyle *cs=0)
 */
void plotter::draw(const TH1D * h, const size_t & pad,const size_t & cidx,bool same, const plotStyle *ps,const containerStyle *cs) {
	if(!padexists(pad)){
		std::cout << "plotter::draw: pad out of range, doing nothing" <<std::endl;
		return;
	}
	if(cidx>=getStyle()->getPlotStyle(pad)->size()){ //no cstyles
		std::cout << "plotter::draw: subplot index out of range, doing nothing" <<std::endl;
		return;
	}
	setupStyles(pad,cidx,ps,cs);
	initCanvas();
	gStyle->SetOptStat(optstat_);
	TH1D * drawh=addPadObject<TH1D>((TH1D*)h->Clone(),pad);
	plotStyle ips=*ps;
	ips.multiplySymbols(1/getStyle()->getPadHeight(pad));
	applyStyle(drawh,cs);
	applyStyle(drawh,&ips);
	//setup done
	TVirtualPad * rpad=cdRootPad(pad);
	rpad->SetBottomMargin(ps->bottomMargin);
	rpad->SetTopMargin(ps->topMargin);
	rpad->SetLeftMargin(ps->leftMargin);
	rpad->SetRightMargin(ps->rightMargin);
	TString drawopts=errToRootDrawOpt(cs);
	if(same)
		drawopts+=",same";
	drawh->Draw(drawopts);
	drawTextBoxes();
}
/*
void plotter::draw(const container1D *c, size_t pad, const containerStyle *cs,const plotStyle *ps ){
	if(!padexists(pad)){
		std::cout << "plotter::draw: pad out of range, doing nothing" <<std::endl;
		return;
	}
	initcanvas();
	TH1D* h=addPadObject<TH1D>(c->getTH1D(c->getName(),canvasstyle_.getPlotStyle(pad)->divideByBinWidth,true),pad);
	TGraphAsymmErrors * g=addPadObject<TGraphAsymmErrors>(c->getTGraph(c->getName(),canvasstyle_.getPlotStyle(pad)->divideByBinWidth,false),pad);


//applyStyle(g,getStyle()->getPlotStyle(pad)->getStyle())
}
void plotter::draw(const container2D *, size_t pad){
	initcanvas();
}
void plotter::draw(const containerStack *, size_t pad){ //only complicated function, all other needs pads need to be fed from outside
	initcanvas();
}
void plotter::draw(const TH1D *, size_t pad){
	initcanvas();
}
void plotter::draw(const TH2D *, size_t pad){
	initcanvas();
}
void plotter::draw(const TGraphAsymmErrors *, size_t pad){
	initcanvas();
}
 */
///
/*
 * provide translation from cont -> ROOT
 */
void plotter::applyStyle(TH1D * h,const containerStyle *s) const{
	h->SetMarkerSize(s->markerSize);
	h->SetMarkerStyle(s->markerStyle);
	h->SetMarkerColor(s->markerColor);
	h->SetLineWidth(s->lineSize);
	h->SetLineStyle(s->lineStyle);
	h->SetLineColor(s->lineColor);
	h->SetFillStyle(s->fillStyle);
	h->SetFillColor(s->fillColor);
	int newfs=errToRootFillStyle(s);
	if(newfs!=0)
		h->SetFillStyle(newfs);
}
void plotter::applyStyle(TH1D * h,const plotStyle *s) const{ //axis info from plotStyle
	TAxis * axis=h->GetXaxis();
	const containerAxisStyle * caxis=s->xAxisStyle();
	for(size_t i=0;i<2;i++){ //both axis
		axis->SetLabelSize(caxis->labelSize);
		if(caxis->applyAxisRange()){
			axis->SetRangeUser(caxis->min,caxis->max);
		}
		axis->SetNdivisions(caxis->ndiv);
		axis->SetTickLength(caxis->tickLength);
		axis->SetTitleOffset(caxis->titleOffset);
		axis->SetTitleSize(caxis->titleSize);
		axis->SetLabelOffset(caxis->labelOffset);

		axis=h->GetYaxis();
		caxis=s->yAxisStyle();
	}
}
void plotter::applyStyle(TH2D * h,const containerStyle *s) const{

}
void plotter::applyStyle(TH2D * h,const plotStyle *s) const{

}
void plotter::applyStyle(TGraphAsymmErrors * g,const containerStyle * s) const{
	g->SetMarkerSize(s->markerSize);
	g->SetMarkerStyle(s->markerStyle);
	g->SetMarkerColor(s->markerColor);
	g->SetLineWidth(s->lineSize);
	g->SetLineStyle(s->lineStyle);
	g->SetLineColor(s->lineColor);
	g->SetFillStyle(s->fillStyle);
	g->SetFillColor(s->fillColor);
	int newfs=errToRootFillStyle(s);
	if(newfs!=0)
		g->SetFillStyle(newfs);
}



TString plotter::errToRootDrawOpt(const containerStyle *s) const{//simple ifs for noErr,normalErr,perpErr,rectErr,fillErr,noXNormalErr,noXPerpErr,noXRectErr
	containerStyle::errorBarStyles errStyle=s->errorStyle;
	if(errStyle == containerStyle::noErr)
		return "";
	if(errStyle == containerStyle::normalErr)
		return "e0";
	if(errStyle == containerStyle::perpErr)
		return "e1";
	if(errStyle == containerStyle::rectErr)
		return "e2";
	if(errStyle == containerStyle::noXNormalErr)
		return "e0";
	if(errStyle == containerStyle::noXPerpErr)
		return "e1";
	if(errStyle == containerStyle::noXRectErr)
		return "e2";
	if(errStyle == containerStyle::fillErr)
		return "e2";
	return "";
}

int plotter::errToRootFillStyle(const containerStyle *s) const{//simple ifs for noErr,normalErr,perpErr,rectErr,fillErr,noXNormalErr,noXPerpErr,noXRectErr
	containerStyle::errorBarStyles errStyle=s->errorStyle;
	if(errStyle == containerStyle::fillErr){
		if(s->fillStyle != 0)
			return s->fillStyle;
		else
			return 3005;
	}
	return 0;
}
void plotter::reDrawCanvas(){
	//clear all pads
	//FIXME
	if(canvas_) delete canvas_;
	initCanvas();

}
void plotter::drawTextBoxes(){
	initCanvas();
	canvas_->cd(0);
	std::vector<textBox> * boxes=canvasstyle_.textBoxes();
	for(size_t i=0;i<boxes->size();i++){
		textBox * box=&boxes->at(i);
		TLatex * l=addObject<TLatex>(new TLatex(box->getX(),box->getY(),box->getText()));
		l->SetTextSize(getRootTextSize(*box));
		l->Draw();
	}
}
/////////////
/**
 * plotter::setupStyles(const size_t & pad=0,const size_t & cidx=0,bool same, plotStyle *ps=0, containerStyle *cs=0) const
 */
bool plotter::setupStyles(const size_t & pad,const size_t & cidx, const plotStyle * &ps, const containerStyle * &cs) const{

	//all indices in range
	if(!ps) //use pre-defined
		ps=canvasstyle_.getPlotStyle(pad);
	if(!cs)
		cs=canvasstyle_.getPlotStyle(pad)->getStyle(cidx);
	return true;
}

bool plotter::padexists(const size_t & idx) const{
	if(idx >= canvasstyle_.size())
		return false;
	return true;
}
/**
 * returns whether canvas already exists
 */
bool plotter::initCanvas(){
	if(!canvas_){
		canvas_ = new TCanvas();
		if(canvasstyle_.size()>1){
			canvas_->Divide(1,canvasstyle_.size());
			for(size_t i=0;i<canvasstyle_.size();i++){
				float ylow=0;
				float yhigh=1;
				if(i<canvasstyle_.size()-1) //last ylow is 0
					ylow=canvasstyle_.getDivision(i);
				if(i>0)
					yhigh=canvasstyle_.getDivision(i-1);
				canvas_->cd(i+1)->SetPad(0,ylow,1,yhigh);
			}
		}
		return false;
	}
	else{
		canvas_->cd();}
	return true;

}
/**
 * select canvas before
 */

float plotter::getRootTextSize(const textBox& tb) const{
	return tb.getTextSize()*0.004;
}
TVirtualPad* plotter::cdRootPad(const size_t &padidx){
	if(canvasstyle_.size()>1)
		return canvas_->cd(padidx+1);
	else
		return canvas_->cd(0);
}

void plotter::copyfrom(const plotter & pl){
	canvasstyle_ = pl.canvasstyle_;
	allobj_.clear();
	canvas_=0;
}
}//namespace
