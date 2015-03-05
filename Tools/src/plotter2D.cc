/*
 * plotter2D.cc
 *
 *  Created on: Oct 27, 2014
 *      Author: kiesej
 */



#include "../interface/plotter2D.h"
#include "TColor.h"
#include "../interface/fileReader.h"

namespace ztop{


plotter2D::plotter2D(): plotterBase(),dividebybinarea_(false),zaxismin_(1),zaxismax_(-1){
	// not here, defaults set by constr : readStyleFromFileInCMSSW("/src/TtZAnalysis/Tools/styles/plot2D_default.txt");
}
plotter2D::~plotter2D(){}



void plotter2D::setPlot(const container2D* c, bool dividebybinarea){
	plot_=*c;
	if(dividebybinarea)
		dividebybinarea_=true;
	else
		dividebybinarea_=false;
}
void plotter2D::readStyleFromFile(const std::string&file){

	fileReader fr;
	fr.setComment("$");
	fr.readFile(file);

	zaxismin_ = fr.getValue("zaxis.min",zaxismin_);
	zaxismax_ = fr.getValue("zaxis.max",zaxismax_);

	rootDrawOpt_ = fr.getValue<TString>("rootDrawOpt",rootDrawOpt_);

}

void plotter2D::preparePad(){
	TVirtualPad* c=getPad();
	c->Clear();
	c->SetLogy(0);
	c->SetBottomMargin(0.15);
	c->SetLeftMargin(0.15);
	c->SetRightMargin(0.2);
}
void plotter2D::drawPlots(){

	if(plot_.isDummy())
		throw std::logic_error("plotter2D::draw: No plot defined");

	const Int_t NRGBs = 5;
	const Int_t NCont = 255;

	Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
	Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
	Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
	Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
	TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
	gStyle->SetNumberContours(NCont);

	//applt style
	// STC!!!



	TH2D* h=addObject(plot_.getTH2D(plot_.getName(),dividebybinarea_,false));


	h->GetZaxis()->SetTitleSize(0.06);
	h->GetZaxis()->SetTitle(zaxistitle_);
	h->GetZaxis()->SetLabelSize(0.05);
	h->GetYaxis()->SetTitleSize(0.06);
	h->GetYaxis()->SetLabelSize(0.05);
	h->GetXaxis()->SetTitleSize(0.06);
	h->GetXaxis()->SetLabelSize(0.05);
	float max=plot_.getMax();
	float min=plot_.getMin();
	max=floor(max)+1;
	min=floor(min)-1;
	if(zaxismin_>zaxismax_)
		h->GetZaxis()->SetRangeUser(min,max);
	else
		h->GetZaxis()->SetRangeUser(zaxismin_,zaxismax_);

	/*
	h->GetZaxis()->SetTitle(plot_.getZAxisName()); */
	h->Draw("colz");


}
// void drawTextBoxes();
void plotter2D::drawLegends(){

}




}//ns



