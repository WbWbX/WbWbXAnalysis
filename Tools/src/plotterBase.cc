/*
 * plotterBase.cc
 *
 *  Created on: Jan 6, 2014
 *      Author: kiesej
 */



#include "../interface/plotterBase.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TGraphAsymmErrors.h"
#include "TVirtualPad.h"
#include <stdexcept>
#include <iostream>
#include "TLatex.h"
#include "TtZAnalysis/Tools/interface/textFormatter.h"
#include "TFile.h"

namespace ztop{

bool plotterBase::debug=false;

plotterBase::plotterBase(const plotterBase& rhs): tObjectList(rhs){
	pad_ = rhs.pad_;
	tbndc_=rhs.tbndc_;
	drawlegend_=rhs.drawlegend_;
	legstyle_=rhs.legstyle_;
	textboxes_ = rhs.textboxes_;
	intstyle_ = rhs.intstyle_; //start with env style
	lastplotidx_ = rhs.lastplotidx_;
	tmplegp_=0;
	yspacemulti_=1.2;

}

TVirtualPad* plotterBase::getPad()const{
	if(!pad_){
		// std::cout << "plotterBase::getCanvas: no canvas associated!" << std::endl;
		throw std::logic_error("plotterBase::getCanvas: no canvas associated!");
	}
	return pad_;
}


/**
 * make sure to safe the output before drawing again. resources will be freed
 * when plotting again
 */
void plotterBase::draw(){
	if(symmetric_styleadd_.length()>0 && plotIsApproxSymmetric(0.02))
		readStylePriv(((std::string)getenv("CMSSW_BASE") +"/"+ symmetric_styleadd_).data(), false);
	if(preparepad_)
		preparePad();//adjusts canvas that lives outside
	drawPlots(); //object handling by plot class
	drawTextBoxes(); //adds new objects to list
	if(drawlegend_)
		drawLegends();//adds new objects to list
	refreshPad();
}
/**
 * returns >1
 */
float plotterBase::getSubPadXScale(int idx){
	TVirtualPad * p=getPad()->cd(idx);
	float multiplier=1;
	double ylow,yhigh,xlow,xhigh;
	p->GetPadPar(xlow,ylow,xhigh,yhigh);
	multiplier = (float)1/(xhigh-xlow);
	return multiplier;
}



/**
 * returns >1
 */
float plotterBase::getSubPadYScale(int idx){
	TVirtualPad * p=getPad()->cd(idx);
	float multiplier=1;
	double ylow,yhigh,xlow,xhigh;
	p->GetPadPar(xlow,ylow,xhigh,yhigh);
	multiplier = (float)1/(yhigh-ylow);
	return multiplier;
}


void plotterBase::convertUserCoordsToNDC(TVirtualPad * subpad, const float & ux, const float & uy, float& ndcx, float& ndcy)const{

	subpad->Update();
	int py,px;
	px=subpad->XtoPixel(ux);
	py=subpad->YtoPixel(uy); //(ux,uy,&px,&py);

	TVirtualPad * fullpad=getPad();
	int pixh=fullpad->GetWh();
	int pixw=fullpad->GetWw();
	ndcx=(float)px/(float)pixw;
	ndcy=(float)py/(float)pixh;

	std::cout << "px: "<<px << "," << "py: "<<py << std::endl;
	std::cout << " user: " << ux << "," <<uy << "  ndx: "<<ndcx << "," << ndcy << std::endl;
}

/**
 * text boxes are ALWAYS drawn with respect to full pad
 */
void plotterBase::drawTextBoxes(){
	if(debug) std::cout << "plotterBase::drawTextBoxes: " << textboxes_.size() << std::endl;

	getPad()->cd();

	TLatex * tb=0;

	//if NDC   getPad()->GetPadPar(Double_t& xlow, Double_t& ylow, Double_t& xup, Double_t& yup)

	for(size_t i=0;i<textboxes_.size();i++){
		if(textboxes_.at(i).getText() != "DEFTITLE")
			tb=addObject(new TLatex(textboxes_.at(i).getX(), textboxes_.at(i).getY(), textboxes_.at(i).getText()));
		else
			tb=addObject(new TLatex(textboxes_.at(i).getX(), textboxes_.at(i).getY(), title_));
		tb->SetTextSize(textboxes_.at(i).getTextSize());
		tb->SetTextFont(textboxes_.at(i).getFont());
		tb->SetTextAlign(textboxes_.at(i).getAlign());
		//if(tbndc_)
		tb->SetNDC(tbndc_);
		tb->Draw("same");
	}

}


textBox&  plotterBase::addTextBox(float x,float y,const TString & text,float textsize,int align,int color){

	return textboxes_.add(x,y,text,textsize,62,align,color);

}
void plotterBase::readTextBoxesInCMSSW(const std::string& pathtofile,const std::string& marker){
	std::string path=getenv("CMSSW_BASE");
	path+="/";
	path+=pathtofile;
	textboxes_.readFromFile(path,marker);
}
void plotterBase::readTextBoxes(const std::string& pathtofile,const std::string& marker){


	textboxes_.readFromFile(pathtofile,marker);


}

void plotterBase::readStyleFromFileInCMSSW(const std::string&pathtofile){
	std::string path=getenv("CMSSW_BASE");
	path+="/";
	path+=pathtofile;
	readStyleFromFile(path);
}

void plotterBase::readTStyleFromFile(const std::string& pathtofile){
	std::cout << "TBI " <<pathtofile.length() <<std::endl;
}


void plotterBase::printToPdf(const std::string& outname){
	TVirtualPad * oldpad=pad_;
	TCanvas c;
	usePad(&c);
	draw();
	TString outnameeps=outname;
	outnameeps+=".eps";
	c.Print(outnameeps);
	TString syscall="epstopdf --outfile="+outname+".pdf " + outnameeps;
	system(syscall);
	TString delcall="rm -f "+outnameeps;
	system(delcall);
	pad_=oldpad;
}
void plotterBase::printToPng(const std::string& outname){
	TVirtualPad * oldpad=pad_;
	TCanvas c;
	usePad(&c);
	draw();
	TString outnameeps=outname;
	outnameeps+=".png";
	c.Print(outnameeps);
	pad_=oldpad;
}
void plotterBase::saveToTFile(const TString& outname){
	TVirtualPad * oldpad=pad_;
	TFile f(outname,"RECREATE");
	TCanvas c;
	usePad(&c);
	draw();
	c.Write();
	f.Close();
	pad_=oldpad;
}

void plotterBase::saveAsCanvasC(const std::string& outname){
	TVirtualPad * oldpad=pad_;
	TString outnametstring=textFormatter::makeCompatibleFileName(outname);
	TCanvas c(outnametstring);
	usePad(&c);
	draw();
	c.Print(outnametstring+".C");
	pad_=oldpad;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// HELPERS          //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
int plotterBase::errToRootFillStyle(const histoStyle *s) const{//simple ifs for noErr,normalErr,perpErr,rectErr,fillErr,noXNormalErr,noXPerpErr,noXRectErr
    if(debug) std::cout << "plotterBase::errToRootFillStyle" << std::endl;
    histoStyle::errorBarStyles errStyle=s->errorStyle;
    if(errStyle == histoStyle::fillErr){
        if(s->fillStyle != 0)
            return s->fillStyle;
        else
            return 3005;
    }
    return 0;
}
TString plotterBase::errToRootDrawOpt(const histoStyle *s) const{//simple ifs for noErr,normalErr,perpErr,rectErr,fillErr,noXNormalErr,noXPerpErr,noXRectErr
    if(debug) std::cout << "plotterBase::errToRootDrawOpt" << std::endl;
    histoStyle::errorBarStyles errStyle=s->errorStyle;
    if(errStyle == histoStyle::noErr)
        return "";
    if(errStyle == histoStyle::normalErr)
        return "e0";
    if(errStyle == histoStyle::perpErr)
        return "e1";
    if(errStyle == histoStyle::rectErr)
        return "e2";
    if(errStyle == histoStyle::noXNormalErr)
        return "e0";
    if(errStyle == histoStyle::noXPerpErr)
        return "e1";
    if(errStyle == histoStyle::noXRectErr)
        return "e2";
    if(errStyle == histoStyle::fillErr)
        return "e2";
    return "";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// APPLY THE STYLES //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void plotterBase::applyStyle(TH1D *h,const histoStyle*s)const{
    if(debug) std::cout << "plotterBase::applyStyle(TH1D*)" << std::endl;
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
void plotterBase::applyAxisStyle(TH1D *h,const plotStyle*s)const{
    if(debug) std::cout << "plotterBase::applyAxisStyle(TH1D*)" << std::endl;
    TAxis * axis=h->GetXaxis();
    const axisStyle * caxis=s->xAxisStyle();
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
void plotterBase::applyStyle(TGraphAsymmErrors *g,const histoStyle*s)const{
    if(debug) std::cout << "plotterBase::applyStyle(TGraphAsymmErrors*)" << std::endl;
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
void plotterBase::applyAxisStyle(TGraphAsymmErrors *g,const plotStyle*s)const{
    if(debug) std::cout << "plotterBase::applyAxisStyle(TGraphAsymmErrors*)" << std::endl;
    TAxis * axis=g->GetXaxis();
    const axisStyle * caxis=s->xAxisStyle();
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

        axis=g->GetYaxis();
        caxis=s->yAxisStyle();
    }
}
void plotterBase::applyStyle(TVirtualPad *p,const plotStyle*ps)const{
    if(debug) std::cout << "plotterBase::applyStyle(TVirtualPad *p)" << std::endl;
    p->SetBottomMargin(ps->bottomMargin);
    p->SetTopMargin(ps->topMargin);
    p->SetLeftMargin(ps->leftMargin);
    p->SetRightMargin(ps->rightMargin);
}

 */
}
