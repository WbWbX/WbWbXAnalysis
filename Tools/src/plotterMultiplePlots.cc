/*
 * plotterMultiplePlots.cc
 *
 *  Created on: Feb 26, 2014
 *      Author: kiesej
 */

#include "../interface/plotterMultiplePlots.h"
#include <stdexcept>
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include <iostream>
#include "TLegend.h"
#include "TStyle.h"
#include "TH1.h"
#include "../interface/fileReader.h"


namespace ztop{

void plotterMultiplePlots::addPlot(const graph *c){
    plots_.push_back(plot(c));
    lastplotidx_=plots_.size()-1;
}
void plotterMultiplePlots::addPlot(const container1D *c,bool divbw){
    plots_.push_back(plot(c,divbw));
    lastplotidx_=plots_.size()-1;
}
void plotterMultiplePlots::removePlot(size_t idx){
    if(idx>=size()){
        throw std::out_of_range("plotterMultiplePlots::removePlot index out of range");
    }
    plots_.erase(plots_.begin()+idx);
    std::vector<size_t>::iterator it=std::find(nolegendidx_.begin(),nolegendidx_.end(),idx);
    if(it != nolegendidx_.end())
    	nolegendidx_.erase(it);
}

void plotterMultiplePlots::addStyleFromFile(const std::string& infile){
    readStylePriv(infile,false);
}
void plotterMultiplePlots::readStyleFromFile(const std::string& infile){
    readStylePriv(infile,true);
}
void plotterMultiplePlots::readStylePriv(const std::string& infile,bool requireall){
    /*
     * style format:
     * expects entries:
     * [containerStyle - Default]
     * [containerStyle - <N=0,1,...>] <- only differences wrt to default
     * [plotStyle - multiPlots]
     * TBI:!! instead of N it is possible to use identifiers (e.g. MC, data)
     */
    containerStyle defaults;
    //read required part

    pstyle_.readFromFile(infile,"MultiPlots",requireall);
    defaults.readFromFile(infile,"Default",requireall);

    fileReader fr;
    fr.setDelimiter(",");
    fr.setStartMarker("[plotterMultiplePlots]");
    fr.setEndMarker("[end plotterMultiplePlots]");
    fr.setComment("$");

    fr.setRequireValues(false);
    fr.readFile(infile);
    drawlegend_ = fr.getValue<bool>("drawLegend", drawlegend_);
    tightxaxis_ = fr.getValue<bool>("tightXaxis",tightxaxis_);
    tightyaxis_ = fr.getValue<bool>("tightYaxis",tightyaxis_);


    fr.setStartMarker("");
    fr.setEndMarker("");
    fr.readFile(infile);
    //count number of available containerStyles
    size_t ncstyles=0;
    for(size_t i=0;i<fr.nLines();i++){
        for(size_t no=0;no<fr.nLines();no++){
            if(fr.getData<TString>(i,0) == "[containerStyle - " + toTString(no) +"]"){
                ncstyles++;
                break;
            }
        }
    }

    for(size_t i=0;i<ncstyles;i++){ //read until nothing found anymore (exception thrown) - should be done differently..
        containerStyle temp=defaults;
        std::string add=toString(i);
        if(debug) std::cout << "plotterMultiplePlots::readStyleFromFile: reading " << add <<std::endl;
        // try{
        temp.readFromFile(infile,add,false);
        /*    }catch(std::runtime_error &e){
            if(debug) std::cout << "plotterMultiplePlots::readStyleFromFile: read "
                    << i<< " styles" <<std::endl;
            break;
        } */
        cstyles_.push_back(temp);
    }


    textboxes_.readFromFile(infile,"boxes");
    legstyle_.readFromFile(infile,"",true);



}


void plotterMultiplePlots::clear(){
    plots_.clear();
    cleanMem();


}

void plotterMultiplePlots::preparePad(){
    if(debug) std::cout <<"plotterMultiplePlots::preparePad" << std::endl;
    cleanMem();
    TVirtualPad * c = getPad();
    c->cd();
    gStyle->SetOptStat(0);
    c->Clear();
    pstyle_.applyPadStyle(c);
}
void plotterMultiplePlots::drawPlots(){
    if(debug) std::cout <<"plotterMultiplePlots::drawPlots" << std::endl;
    TVirtualPad * c=getPad();
    c->cd();
    if(cstyles_.size()<plots_.size()){
        throw std::out_of_range("plotterMultiplePlots::draw: Less styles set than plots added. exit");
    }
    if(plots_.size()<1){
        throw std::logic_error("plotterMultiplePlots::draw: cannot draw 0 plots");
    }


    plotStyle tempstyle=pstyle_;
    if(!tempstyle.yAxisStyle()->applyAxisRange()){ //recompute range, otherwise take from input style
        float min=getMinimum();
        float max=getMaximum();

        max=max+(fabs(max-min)*0.05);
        if(fabs(min)/(fabs(max-min)) < 0.01) //minimum is  close enough to 0
            min=0+(fabs(max-min)*0.000001);
        else //is far away from 0
            min=min-(fabs(max-min)*0.05);

        if(tempstyle.forceYAxisZero())
            min=0+0.00000001;

        tempstyle.yAxisStyle()->max=max;
        tempstyle.yAxisStyle()->min=min;
    }
    TH1 * axish=0;

    axish=addObject(plots_.at(0).getInputGraph().getAxisTH1(tightyaxis_,tightxaxis_));
  //  axish->GetYaxis() -> SetRangeUser(ymin,axish->GetYaxis() -> GetXmax()));
    tempstyle.applyAxisStyle(axish);
    axish->Draw("AXIS");
    TG * g=0,*gs=0;
    for(size_t inv=plots_.size();inv>0;inv--){ //draw from bottom to top
        size_t i=inv-1;
        g=plots_.at(i).getStatGraph();
        gs=plots_.at(i).getSystGraph();
        cstyles_.at(i).applyContainerStyle(g,false);
        cstyles_.at(i).applyContainerStyle(gs,true);
        if(cstyles_.at(i).sysRootDrawOpt != "none")
            gs->Draw(cstyles_.at(i).sysRootDrawOpt+"same");
        g->Draw(cstyles_.at(i).rootDrawOpt+"same");
    }

    getPad()->RedrawAxis();

}
void plotterMultiplePlots::drawLegends(){
    if(!drawlegend_)  return;
    getPad()->cd(1);
    TLegend *leg = addObject(new TLegend((Double_t)0.65,(Double_t)0.50,(Double_t)0.95,(Double_t)0.90));
    leg->Clear();
    leg->SetFillStyle(0);
    leg->SetBorderSize(0);
    for(size_t i=0;i<plots_.size() ; i++){
    	if(std::find(nolegendidx_.begin(),nolegendidx_.end(),i)!=nolegendidx_.end()) continue;
        leg->AddEntry(plots_.at(i).getStatGraph(),plots_.at(i).getName(),"pel");
    }
    legstyle_.applyLegendStyle(leg);
    leg->Draw("same");
    getPad()->RedrawAxis();
}

float plotterMultiplePlots::getMaximum()const{
    float max=-1e20;
    for(size_t i=0;i<plots_.size();i++){
        if(max<plots_.at(i).getInputGraph().getYMax(true)) max=plots_.at(i).getInputGraph().getYMax(true);
    }
    return max;
}


float plotterMultiplePlots::getMinimum()const{
    float min=1e20;
    for(size_t i=0;i<plots_.size();i++){
        if(min>plots_.at(i).getInputGraph().getYMin(true)) min=plots_.at(i).getInputGraph().getYMin(true);
    }
    return min;
}


}
