/*
 * histoStyle.cc
 *
 *  Created on: Sep 30, 2013
 *      Author: kiesej
 */

#include "../interface/histoStyle.h"
#include "../interface/fileReader.h"
#include "TH1.h"
#include "TGraphAsymmErrors.h"
#include <stdexcept>
#include <iostream>
#include "../interface/plot.h"
#include "TLatex.h"

namespace ztop{

///////HELPER CLASS//////
axisStyle::axisStyle(): titleSize(0), labelSize(0), titleOffset(0), labelOffset(0), tickLength(0), ndiv(0), max(-1), min(1),log(false),forcezero(false),posx(0),posy(0),posz(0){}
axisStyle::~axisStyle(){}

bool axisStyle::applyAxisRange() const{
	if(max<=min) return false;
	else return true;
}
TString axisStyle::makeTexConsistentName(const TString& in){
	TString out;
	int open=0;
	for(size_t i=0;i<(size_t)in.Length();i++){
		if(in.Data()[i] == '{'){
			open++;
			out+=in.Data()[i];
		}
		else if(in.Data()[i] == '}'){
			open--;
			if(open>=0){
				out+=in.Data()[i];
			}
		}
		else{
			out+=in.Data()[i];
		}
	}
	for(int i=0;i<open;i++){
		out+="}";
	}
	return out;
}


textBox::textBox(): x_(0),y_(0),text_(""),textsize_(0),align_(11),font_(62),color_(1){}
textBox::textBox(float x,float y,const TString &text,float textsize,int font,int align,int color):  x_(x),y_(y),text_(text),textsize_(textsize),align_(align),font_(font),color_(1){}
textBox::~textBox(){}
void textBox::setText(const TString & text){text_=text;}
void textBox::setTextSize(float textsize){textsize_=textsize;}
void textBox::setCoords(float x,float y){
	x_=x;
	y_=y;
}
void textBox::setFont(int f){
	font_=f;
}
void textBox::setAlign(int a){
	align_=a;
}
void textBox::setColor(int a){
	color_=a;
}
const TString & textBox::getText() const{return text_;}
const float &textBox::getTextSize() const{return textsize_;}
const float &textBox::getX() const{return x_;}
const float &textBox::getY() const{return y_;}
const int& textBox::getFont()const{return font_;}
const int& textBox::getAlign()const{return align_;}
const int& textBox::getColor()const{return color_;}

//////////////////////////

bool textBoxes::debug=false;

#include <cstdlib>

void textBoxes::readFromFileInCMSSW(const std::string & filename, const std::string & markername){
	std::string cmssw=getenv("CMSSW_BASE");
	cmssw+="/";
	cmssw+=filename;
	readFromFile(cmssw,markername);
}

/**
 * entries:
 * [textBoxes - <markername>]
 * <x coord>, <ycoord>, <text>, <size>
 * [end textBoxes]
 */
void textBoxes::readFromFile(const std::string & filename, const std::string & markername){

	if(((TString )filename).EndsWith("none"))
		return;

	fileReader fr;


	fr.setStartMarker("[textBoxes - "+markername+']');
	fr.setEndMarker("[end - textBoxes]");
	fr.readFile(filename);
	fr.setRequireValues(false);
	TString externalfile= fr.getValue<TString>("externalFile","");
	TString externalmarker= fr.getValue<TString>("externalMarker","");

	if(externalfile.Length() > 0){
		if(externalfile.BeginsWith("CMSSW_BASE")){
			TString str=externalfile;
			str.Remove(0,((TString)"CMSSW_BASE").Length());
			readFromFileInCMSSW(str.Data(),externalmarker.Data());
		}
		else{
			readFromFile(externalfile.Data(),externalmarker.Data());
		}
	}

	fr.setComment("$");
	fr.setDelimiter(",");
	fr.readFile(filename);
	//read lines
	/*    if(fr.nLines()<1){
        std::cout << "textBoxes::readFromFile: did not find boxes "  << markername <<std::endl;
        throw std::runtime_error("textBoxes::readFromFile: no boxes found");
    }*/
	//clear();



	for(size_t i=0 ;i < fr.nLines();i++){
		if(fr.nEntries(i)<4) continue; //line doesnt contain all info needed
		if(debug) std::cout << "textBoxes::readFromFile: add "
				<< fr.getData<float>(i,0)<<" "<<fr.getData<float>(i,1)
				<<" "<<fr.getData<std::string>(i,2)<<" "<<fr.getData<float>(i,3) << std::endl;
		if(fr.nEntries(i)==(size_t)4)
			add(fr.getData<float>(i,0),fr.getData<float>(i,1),fr.getData<std::string>(i,2),fr.getData<float>(i,3));
		else if(fr.nEntries(i)==(size_t)5)
			add(fr.getData<float>(i,0),fr.getData<float>(i,1),fr.getData<std::string>(i,2),fr.getData<float>(i,3),fr.getData<int>(i,4));
		else if(fr.nEntries(i)==(size_t)6)
			add(fr.getData<float>(i,0),fr.getData<float>(i,1),fr.getData<std::string>(i,2),fr.getData<float>(i,3),fr.getData<int>(i,4),fr.getData<int>(i,5));
		else if(fr.nEntries(i)==(size_t)7)
			add(fr.getData<float>(i,0),fr.getData<float>(i,1),fr.getData<std::string>(i,2),fr.getData<float>(i,3),fr.getData<int>(i,4),fr.getData<int>(i,5),fr.getData<int>(i,6));
	}

}

void textBoxes::drawToPad(TVirtualPad * p,bool NDC){

	TLatex * tb=0;

	for(size_t i=0;i<size();i++){
		tb=addObject(new TLatex(at(i).getX(), at(i).getY(), at(i).getText()));
		tb->SetTextSize(at(i).getTextSize());
		tb->SetTextFont(at(i).getFont());
		tb->SetTextAlign(at(i).getAlign());
		tb->SetTextColor(at(i).getColor());
		//if(tbndc_)
		tb->SetNDC(NDC);
		tb->Draw("same");
	}

}


histoStyle::histoStyle(): markerSize(0),markerStyle(0),markerColor(0),lineSize(0),lineStyle(0),
		lineColor(0),fillStyle(0),fillColor(0),sysFillStyle(0),sysFillColor(0) {
}

histoStyle::~histoStyle(){}


void histoStyle::multiplySymbols(float val){
	markerSize*=val;
	lineSize= (int)((float)lineSize * val);

}


void histoStyle::readFromFile(const std::string & filename, const std::string& stylename,bool requireall){
	fileReader fr;
	fr.setComment("$");
	fr.setDelimiter(",");
	fr.setStartMarker("[containerStyle - "+stylename+']');
	fr.setEndMarker("[end containerStyle]");
	fr.readFile(filename);
	if(fr.nLines()<1 && requireall){
		std::cout << "containerStyle::readFromFile: did not find style "  << stylename <<std::endl;
		throw std::runtime_error("containerStyle::readFromFile: no containerStyle found");
	}
	fr.setRequireValues(requireall);

	markerColor = fr.getValue<int>("markerColor",markerColor);
	markerStyle = fr.getValue<int>("markerStyle",markerStyle);
	markerSize  = fr.getValue<float>("markerSize",markerSize);

	lineSize    = fr.getValue<float>("lineSize",lineSize);
	lineStyle   = fr.getValue<int>("lineStyle",lineStyle);
	lineColor   = fr.getValue<int>("lineColor",lineColor);

	fillStyle   = fr.getValue<int>("fillStyle",fillStyle);
	fillColor   = fr.getValue<int>("fillColor",fillColor);
	sysFillStyle   = fr.getValue<int>("sysFillStyle",sysFillStyle);
	sysFillColor   = fr.getValue<int>("sysFillColor",sysFillColor);

	rootDrawOpt   = fr.getValue<TString>("rootDrawOpt",rootDrawOpt);
	sysRootDrawOpt   = fr.getValue<TString>("sysRootDrawOpt",sysRootDrawOpt);
	drawStyle     = fr.getValue<TString>("drawStyle",drawStyle);

	legendDrawStyle  = fr.getValue<TString>("legendDrawStyle",legendDrawStyle);

}
/**
 * does not add draw style!
 */
void histoStyle::applyContainerStyle(TH1*h,bool sys)const{
	h->SetMarkerSize(markerSize);
	h->SetMarkerStyle(markerStyle);
	h->SetMarkerColor(markerColor);
	h->SetLineWidth(lineSize);
	h->SetLineStyle(lineStyle);
	h->SetLineColor(lineColor);
	if(sys){
		h->SetFillColor(sysFillColor);
		h->SetFillStyle(sysFillStyle);
	}
	else{
		h->SetFillColor(fillColor);
		h->SetFillStyle(fillStyle);
	}
}
/**
 * does not add draw style!
 */
void histoStyle::applyContainerStyle(TGraphAsymmErrors*h,bool sys)const{
	h->SetMarkerSize(markerSize);
	h->SetMarkerStyle(markerStyle);
	h->SetMarkerColor(markerColor);
	h->SetLineWidth(lineSize);
	h->SetLineStyle(lineStyle);
	h->SetLineColor(lineColor);
	if(sys){
		h->SetFillColor(sysFillColor);
		h->SetFillStyle(sysFillStyle);
	}
	else{
		h->SetFillColor(fillColor);
		h->SetFillStyle(fillStyle);
	}
}
void histoStyle::applyContainerStyle(plot*p)const{
	applyContainerStyle(p->getStatGraph(),false);
	applyContainerStyle(p->getSystGraph(),true);
}
}
