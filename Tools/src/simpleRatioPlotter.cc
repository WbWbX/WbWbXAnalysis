/*
 * simpleRatioPlotter.cc
 *
 *  Created on: Nov 8, 2013
 *      Author: kiesej
 */



#include "../interface/simpleRatioPlotter.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TGraphAsymmErrors.h"
#include <iostream>

namespace ztop{

simpleRatioPlotter::~simpleRatioPlotter(){

}

void simpleRatioPlotter::drawPlots(const std::vector<container1D> &conts , const std::vector<int> &colz){
	//some style
	bool divebybinw=true;
	//style
	int fillstyle=3005;
	//...


	if(conts.size()<1) return;
	std::vector<int> newcols=checkSizes(conts,colz);
	float max=conts.at(0).getYMax(divebybinw);
	float min=conts.at(0).getYMin(divebybinw);
	for(size_t i=0;i<conts.size();i++){
		float tmax=conts.at(i).getYMax(divebybinw);
		float tmin=conts.at(i).getYMin(divebybinw);
		if(max<tmax)max=tmax;
		if(min>tmin)min=tmin;
	}
	if(fabs(min/max) < 0.05 && min>=0) min=0;

	//draw first
	float multiplier=getLabelMultiplier();
	container1D nom=conts.at(0);
	nom.setLabelSize(multiplier);
	TH1D * h = addObject(nom.getTH1D("",divebybinw));
	h->GetXaxis()->SetTickLength(0.03 * multiplier);
	h->GetYaxis()->SetRangeUser(min,max*1.1);
	h->GetXaxis()->SetLabelSize(0);
	if(!getDrawSame()) h->Draw("AXIS");
	for(size_t i=conts.size()-1;(int)i>=0;i--){
		TGraphAsymmErrors *g=addObject(conts.at(i).getTGraph("",divebybinw));
		//TGraphAsymmErrors *gstat=addObject(conts.at(i).getTGraph("",divebybinw,true));
		g->SetFillStyle(fillstyle);
		setColors(g,newcols.at(i));
		if(i>0){
			g->Draw(drawopt_+",same");
			//gstat->Draw("5,same");
		}
		else{
			g->Draw(drawopt_+",same");
			//gstat->Draw("e1,same");
		}
	}


}
/**
 * consider first entry reference
 */
void simpleRatioPlotter::drawRatio(const std::vector<container1D> &conts , const std::vector<int> &colz){

	//style
	int fillstyle=3005;
	//...


	if(conts.size()<1) return;
	std::vector<int> newcols=checkSizes(conts,colz);
	container1D ref=conts.at(0);
	container1D refnorm=ref.getRelErrorsContainer();
	//FIXME
	//think about whether to include errors or not

	std::vector<container1D> normlzd;
	normlzd.push_back(refnorm);
	float max=refnorm.getYMax(false),min=refnorm.getYMin(false);
	for(size_t i=1;i<conts.size();i++){
		container1D temp=conts.at(i);
		temp.normalizeToContainer(ref);
		float tmax=temp.getYMax(false);
		float tmin=temp.getYMin(false);
		if(tmax>max)max=tmax;
		if(tmin<min)min=tmin;
		normlzd.push_back(temp);
	}
	float multiplier=getLabelMultiplier();
	refnorm.setLabelSize(multiplier);
	TH1D *h = addObject(refnorm.getTH1D("",false));
	h->GetYaxis()->SetRangeUser(min*0.9,max*1.1);
	h->GetXaxis()->SetTickLength(0.03 * multiplier);
	h->GetYaxis()->SetNdivisions(505);
	h->GetYaxis()->SetTitle("");
	if(!getDrawSame()) h->Draw("AXIS");

	for(size_t i=normlzd.size()-1;(int)i>=0;i--){
		TGraphAsymmErrors * g = addObject(normlzd.at(i).getTGraph("",false));
		//TGraphAsymmErrors * gsys=addObject(normlzd.at(i).getTGraph("",false,false,false,true));
		g->SetFillStyle(fillstyle);
		setColors(g,newcols.at(i));
		//setColors(gsys,newcols.at(i)-1);
		if(i>0){
			g->Draw(drawopt_+",same");
			//gsys->Draw("5,same");
		}
		else{
			g->Draw(drawopt_+",same");
			//gsys->Draw("e1,P,same");
		}
	}


}

TCanvas * simpleRatioPlotter::plotAll(const TString & canvasname, const std::vector<container1D>& conts, const std::vector<int>& colz){
	TH1::AddDirectory(false);

	TCanvas * c= getCanvas();
	//style options
	double labelresizeTOBEIMPL=1.;
	double divideat=0.3;
	if(!getDrawSame()) {
		c->Divide(1,2);
		c->SetFrameBorderSize(0);
		//prepare pad1(plots)
		c->cd(1)->SetLeftMargin(0.15);
		c->cd(1)->SetBottomMargin(0.03);
		c->cd(1)->SetPad(0,divideat,1,1);


		//prepare pad2 (ratio
		c->cd(2)->SetBottomMargin(0.5 * 0.3/divideat);
		c->cd(2)->SetLeftMargin(0.15);
		c->cd(2)->SetTopMargin(0);
		c->cd(2)->SetPad(0,0,1,divideat);
	}
	//draw pad1
	//draw pad2
	c->cd(2);
	drawRatio(conts,colz);
	c->cd(1);
	drawPlots(conts,colz);

	c->cd(2)->RedrawAxis();
	c->cd(1)->RedrawAxis();


	return c;
}

/////PRIVATE///////
/**
 * cheks size. if more conts than colors, cout + add random colors
 */
std::vector<int> simpleRatioPlotter::checkSizes(const std::vector<container1D> & cs , const std::vector<int> & cols){
	if(cs.size()<=cols.size()) return cols;
	std::vector<int> out;
	std::cout << "simpleRatioPlotter::checkSizes: less colors defined than containers. adding random colors" <<std::endl;
	for(size_t i=0;i<cs.size();i++){
		if(i<cols.size())
			out.push_back(cols.at(i));
		else
			out.push_back((int)i);
	}
	return out;
}

}
