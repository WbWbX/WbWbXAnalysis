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


namespace ztop{

void plotterMultiplePlots::addPlot(const graph *c){
    plots_.push_back(plot(c));
}
void plotterMultiplePlots::addPlot(const container1D *c,bool divbw){
    plots_.push_back(plot(c,divbw));
}
void plotterMultiplePlots::removePlot(size_t idx){
    if(idx>=size()){
        throw std::out_of_range("plotterMultiplePlots::removePlot index out of range");
    }
    plots_.erase(plots_.begin()+idx);
}

void plotterMultiplePlots::readStyleFromFile(const std::string& infile){
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

    pstyle_.readFromFile(infile,"MultiPlots",true);
    defaults.readFromFile(infile,"Default",true);


    for(size_t i=0;true;i++){ //read until nothing found anymore (exception thrown) - should be done differently..
        containerStyle temp=defaults;
        std::string add=toString(i);
        if(debug) std::cout << "plotterMultiplePlots::readStyleFromFile: reading " << add <<std::endl;
        try{
            temp.readFromFile(infile,add,false);
        }catch(std::runtime_error &e){
            if(debug) std::cout << "plotterMultiplePlots::readStyleFromFile: read "
                    << i<< " styles" <<std::endl;
            break;
        }
        cstyles_.push_back(temp);
    }

    textboxes_.readFromFile(infile,"boxes");

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
        if(min>0 && min/(fabs(max-min)) < 0.05) //minimum is  close enough to 0 and positive
            min=0-(fabs(max-min)*0.000001);
        else //is close enough to 0
            min=min-(fabs(max-min)*0.05);


        tempstyle.yAxisStyle()->max=max;
        tempstyle.yAxisStyle()->min=min;
    }
    TH1 * axish=0;
    axish=addObject(plots_.at(0).getInputGraph().getAxisTH1(false,false));
    tempstyle.applyAxisStyle(axish);
    axish->Draw("AXIS");
    TG * g=0,*gs=0;
    for(size_t i=0;i<plots_.size();i++){
        g=plots_.at(i).getStatGraph();
        gs=plots_.at(i).getSystGraph();
        cstyles_.at(i).applyContainerStyle(g,false);
        cstyles_.at(i).applyContainerStyle(gs,true);
        gs->Draw(cstyles_.at(i).sysRootDrawOpt+"same");
        g->Draw(cstyles_.at(i).rootDrawOpt+"same");
    }


}
void plotterMultiplePlots::drawLegends(){
    TLegend *leg = addObject(new TLegend((Double_t)0.65,(Double_t)0.50,(Double_t)0.95,(Double_t)0.90));
    leg->Clear();
    leg->SetFillStyle(0);
    leg->SetBorderSize(0);
    legstyle_.applyLegendStyle(leg);
    for(size_t i=0;i<plots_.size() ; i++){
        leg->AddEntry(plots_.at(i).getStatGraph(),plots_.at(i).getName(),"pel");
    }
    getPad()->cd();
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
