/*
 * containerStyle.cc
 *
 *  Created on: Sep 30, 2013
 *      Author: kiesej
 */

#include "../interface/containerStyle.h"
#include "../interface/fileReader.h"
#include "TH1.h"
#include "TGraphAsymmErrors.h"
#include <stdexcept>
#include <iostream>
#include "../interface/plot.h"

namespace ztop{

///////HELPER CLASS//////
axisStyle::axisStyle(): titleSize(0), labelSize(0), titleOffset(0), labelOffset(0), tickLength(0), ndiv(0), max(-1), min(1),log(false){}
axisStyle::~axisStyle(){}

bool axisStyle::applyAxisRange() const{
    if(max<=min) return false;
    return true;
}


textBox::textBox(): x_(0),y_(0),text_(""),textsize_(0){}
textBox::textBox(float x,float y,const TString &text,float textsize):  x_(x),y_(y),text_(text),textsize_(textsize){}
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

bool textBoxes::debug=false;

/**
 * entries:
 * [textBoxes - <markername>]
 * <x coord>, <ycoord>, <text>, <size>
 * [end textBoxes]
 */
void textBoxes::readFromFile(const std::string & filename, const std::string & markername){
    fileReader fr;
    fr.setComment("$");
    fr.setDelimiter(",");
    fr.setStartMarker("[textBoxes - "+markername+']');
    fr.setEndMarker("[end textBoxes]");
    fr.readFile(filename);
    if(fr.nLines()<1){
        std::cout << "textBoxes::readFromFile: did not find boxes "  << markername <<std::endl;
        throw std::runtime_error("textBoxes::readFromFile: no boxes found");
    }
    clear();
    for(size_t i=0 ;i < fr.nLines();i++){
        if(fr.nEntries(i)<4) continue; //line doesnt contain all info needed
        if(debug) std::cout << "textBoxes::readFromFile: add "
                << fr.getData<float>(i,0)<<" "<<fr.getData<float>(i,1)
                <<" "<<fr.getData<std::string>(i,2)<<" "<<fr.getData<float>(i,3) << std::endl;
        add(fr.getData<float>(i,0),fr.getData<float>(i,1),fr.getData<std::string>(i,2),fr.getData<float>(i,3));
    }

}


containerStyle::containerStyle(): markerSize(0),markerStyle(0),markerColor(0),lineSize(0),lineStyle(0),
        lineColor(0),fillStyle(0),fillColor(0),sysFillStyle(0),sysFillColor(0) {
}

containerStyle::~containerStyle(){}


void containerStyle::multiplySymbols(float val){
    markerSize*=val;
    lineSize= (int)((float)lineSize * val);

}


void containerStyle::readFromFile(const std::string & filename, const std::string& stylename,bool requireall){
    fileReader fr;
    fr.setComment("$");
    fr.setDelimiter(",");
    fr.setStartMarker("[containerStyle - "+stylename+']');
    fr.setEndMarker("[end containerStyle]");
    fr.readFile(filename);
    if(fr.nLines()<1){
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

}
/**
 * does not add draw style!
 */
void containerStyle::applyContainerStyle(TH1*h,bool sys)const{
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
void containerStyle::applyContainerStyle(TGraphAsymmErrors*h,bool sys)const{
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
void containerStyle::applyContainerStyle(plot*p)const{
    applyContainerStyle(p->getStatGraph(),false);
    applyContainerStyle(p->getSystGraph(),true);
}
}
