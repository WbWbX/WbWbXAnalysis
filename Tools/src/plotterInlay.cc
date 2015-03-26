/*
 * plotterInlay.cc
 *
 *  Created on: Jun 17, 2014
 *      Author: kiesej
 */

#include "../interface/plotterInlay.h"
#include <vector>
#include <stdexcept>
#include <iostream>
#include "TLegend.h"
#include "TStyle.h"
#include "TH1.h"
#include "../interface/fileReader.h"

namespace ztop{

TVirtualPad * plotterInlay::getInlayPad()const{
    if(inlaypad_) return inlaypad_;
    throw std::logic_error("plotterInlay::getInlayPad: no inlay pad to return");
}
TVirtualPad * plotterInlay::getInlayPad(){
    if(inlaypad_) return inlaypad_;
    throw std::logic_error("plotterInlay::getInlayPad: no inlay pad to return");
}

void plotterInlay::addInlayPlot(const graph *c){
    plotsinlay_.push_back(plot(c));
}
void plotterInlay::addInlayPlot(const histo1D *c,bool divbw){
    plotsinlay_.push_back(plot(c,divbw));
}


void plotterInlay::removeInlayPlot(size_t idx){
    if(idx>=plotsinlay_.size()){
        throw std::out_of_range("plotterMultiplePlots::removePlot index out of range");
    }
    plotsinlay_.erase(plots_.begin()+idx);
}



void plotterInlay::drawPlots(){
    //draw main plots
    plotterMultiplePlots::drawPlots();

    //draw inlay plots
    if(cstylesinlay_.size()<plotsinlay_.size()){
        throw std::out_of_range("plotterInlay::draw: Less styles set than inlay plots added. exit");
    }

    plotStyle tempstyle=pstyleinlay_;
    if(!tempstyle.yAxisStyle()->applyAxisRange()){ //recompute range, otherwise take from input style
        float min=getInlayMinimum();
        float max=getInlayMaximum();
        max=max+(fabs(max-min)*0.05);
        if(min>0 && min/(fabs(max-min)) < 0.05) //minimum is  close enough to 0 and positive
            min=0-(fabs(max-min)*0.000001);
        else //is close enough to 0
            min=min-(fabs(max-min)*0.05);


        tempstyle.yAxisStyle()->max=max;
        tempstyle.yAxisStyle()->min=min;
    }


    setInlayPad(inlayx0_,inlayy0_,inlayx1_,inlayx1_);
    pstyleinlay_.applyPadStyle(inlaypad_);
    inlaypad_->Draw();

    inlaypad_->cd();

    TH1 * axish=0;
    axish=addObject(plotsinlay_.at(0).getInputGraph().getAxisTH1(tightyaxis_,tightxaxis_));
    tempstyle.applyAxisStyle(axish);
    axish->Draw("AXIS");
    TG * g=0,*gs=0;
    for(size_t inv=plotsinlay_.size();inv>0;inv--){
        size_t i=inv-1;
        g=plotsinlay_.at(i).getStatGraph();
        gs=plotsinlay_.at(i).getSystGraph();
        cstylesinlay_.at(i).applyContainerStyle(g,false);
        cstylesinlay_.at(i).applyContainerStyle(gs,true);
        if(cstylesinlay_.at(i).sysRootDrawOpt != "none")
            gs->Draw(cstylesinlay_.at(i).sysRootDrawOpt+"same");
        g->Draw(cstylesinlay_.at(i).rootDrawOpt+"same");

    }
    inlaypad_->RedrawAxis();



}


void plotterInlay::clear(){
    plots_.clear();
    plotsinlay_.clear();
    cleanMem();
}



void plotterInlay::addStyleFromFile(const std::string& infile){
    readStylePriv(infile,false);
}
void plotterInlay::readStyleFromFile(const std::string& infile){
    readStylePriv(infile,true);
}

void plotterInlay::readStylePriv(const std::string& infile,bool requireall){



    histoStyle defaults;
    //read required part

    pstyle_.readFromFile(infile,"MainPlots",requireall);
    defaults.readFromFile(infile,"MainDefault",requireall);

    fileReader fr;
    fr.setDelimiter(",");
    fr.setComment("$");


    fr.setRequireValues(requireall);
    fr.setStartMarker("[plotterInlay]");
    fr.setEndMarker("[end plotterInlay]");
    fr.readFile(infile);


    if(fr.nLines()<1){
        std::cout << "plotterInlay::readStyleFromFile: no [plotterInlay] found in " << infile  <<std::endl;
        throw std::runtime_error("plotterInlay::readStyleFromFile: no [plotterInlay] found");
    }


    drawlegend_ = fr.getValue<bool>("drawLegend", drawlegend_);
    inlayx0_ = fr.getValue<float>("inlayX0", inlayx0_);
    inlayy0_ = fr.getValue<float>("inlayY0", inlayy0_);
    inlayx1_ = fr.getValue<float>("inlayX1", inlayx1_);
    inlayy1_ = fr.getValue<float>("inlayY1", inlayy1_);



    fr.setStartMarker("");
    fr.setEndMarker("");
    fr.readFile(infile);
    //count number of available containerStyles
    size_t ncstyles=0;
    for(size_t i=0;i<fr.nLines();i++){
        for(size_t no=0;no<fr.nLines();no++){
            if(fr.getData<TString>(i,0) == "[containerStyle - Main" + toTString(no) +"]"){
                ncstyles++;
                break;
            }
        }
    }

    for(size_t i=0;i<ncstyles;i++){
        histoStyle temp=defaults;
        std::string add="Main"+toString(i);
        if(debug) std::cout << "plotterInlay::readStyleFromFile: reading " << add <<std::endl;

        temp.readFromFile(infile,add,false);

        cstyles_.push_back(temp);
    }

    ///inlay plots
    pstyleinlay_.readFromFile(infile,"InlayPlots",requireall);
    defaults.readFromFile(infile,"InlayDefault",requireall);
    //count number of available containerStyles and check for textboxes

    ncstyles=0;
    for(size_t i=0;i<fr.nLines();i++){
        //  if(std::find(fr.getData(i).begin(),fr.getData(i).end(),(std::string)"[textBoxes - boxes]") !=  fr.getData(i).end())
        //    boxes=true;
        for(size_t no=0;no<fr.nLines();no++){
            if(fr.getData<TString>(i,0) == "[containerStyle - Inlay" + toTString(no) +"]"){
                ncstyles++;
                break;
            }
        }
    }

    for(size_t i=0;i<ncstyles;i++){
        histoStyle temp=defaults;
        std::string add="Inlay"+toString(i);
        if(debug) std::cout << "plotterInlay::readStyleFromFile: reading " << add <<std::endl;

        temp.readFromFile(infile,add,false);

        cstylesinlay_.push_back(temp);
    }

    textboxes_.readFromFile(infile,"boxes");
    legstyle_.readFromFile(infile,"",requireall);

}

void plotterInlay::setInlayPad(float x0, float y0, float x1, float y1){
    if(inlaypad_) delete inlaypad_;
    getPad()->cd();
    inlaypad_ = addObject(new TPad("inlay_"+title_,"inlay_"+title_,x0,y0,x1,y1));

}


void plotterInlay::copyFrom(const plotterInlay& rhs){
    if(this == &rhs) return;

    pstyleinlay_=rhs.pstyleinlay_;
    cstylesinlay_=rhs.cstylesinlay_;
    plotsinlay_=rhs.plotsinlay_;

    if(inlaypad_) delete inlaypad_;
    inlaypad_ = (TVirtualPad*)(rhs.inlaypad_)->Clone();
    if(rhs.padAssociated())
        usePad(rhs.getPad());

}

plotterInlay & plotterInlay::operator = (const plotterInlay& rhs ){
    copyFrom(rhs);
    return *this;
}
plotterInlay::plotterInlay(const plotterInlay&rhs){
    copyFrom(rhs);
}


float plotterInlay::getInlayMaximum()const{
    float max=-1e20;
    for(size_t i=0;i<plotsinlay_.size();i++){
        if(max<plotsinlay_.at(i).getInputGraph().getYMax(true)) max=plotsinlay_.at(i).getInputGraph().getYMax(true);
    }
    return max;
}


float plotterInlay::getInlayMinimum()const{
    float min=1e20;
    for(size_t i=0;i<plotsinlay_.size();i++){
        if(min>plotsinlay_.at(i).getInputGraph().getYMin(true)) min=plotsinlay_.at(i).getInputGraph().getYMin(true);
    }
    return min;
}

}


