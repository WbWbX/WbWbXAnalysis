/*
 * controlPlotBasket.cc
 *
 *  Created on: Oct 29, 2013
 *      Author: kiesej
 */

#include "../interface/controlPlotBasket.h"
#include "../interface/NTFullEvent.h"
#include <stdexcept>
#include <iostream>
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"


namespace ztop{
std::vector<TString> controlPlotBasket::namelist;

controlPlotBasket::~controlPlotBasket(){
    for(size_t i=0;i<cplots_.size();i++)
        for(size_t j=0;j<cplots_.at(i).size();j++)
            if(cplots_.at(i).at(j)) delete cplots_.at(i).at(j);
}


/**
 * returns pointer to container if already init
 */
container1D * controlPlotBasket::addPlot(const TString & name, const TString & xaxisname,const TString & yaxisname, const bool & mergeufof){
    if(tmpnewstep_){ //make new container
        if(!event()){
            throw std::logic_error("controlPlotBasket::addPlot: Not linked to any NTFullEvent");
        }
        if(!initphase_ && !event()->puweight){
            std::cout << "controlPlotBasket::addPlot: puweight needs to be set" << std::endl;
            throw std::logic_error("controlPlotBasket::addPlot: puweight needs to be set");
        }
        TString newname=name+" step "+toTString(tmpstep_);
        for(size_t i=0;i<namelist.size();i++){
            if(namelist.at(i) == newname){
                std::cout <<"controlPlotBasket::addPlot: plots must not have same names! ("<< newname << ")"<< std::endl;
                throw std::logic_error("controlPlotBasket::addPlot: plots must not have same names!");
            }
        }
        bool tmp=container1D::c_makelist;
        container1D::c_makelist =true;
        container1D * cont = new container1D(tempbins_,newname, xaxisname, yaxisname, mergeufof);
        container1D::c_makelist=tmp;
        namelist.push_back(cont->getName());
        cplots_.at(tmpstep_).push_back(cont);
    }
    //else just return pointer
    lcont_=cplots_[tmpstep_][tmpidx_++];//at(tmpstep_).at(tmpidx_++);//
    return lcont_;//; //not safe but fast
}

void controlPlotBasket::initStep(const size_t & step){
    //check if already created

    tmpidx_=0;
    tmpstep_=step;
    tmpnewstep_=false;
    if(step<size_){
        return;
    }
    else if(step==size_){//create step
        if(!event()){
            throw std::logic_error("controlPlotBasket::initStep: Not linked to any NTFullEvent");
        }
        tmpnewstep_=true;
        std::vector<container1D *> tmp;
        cplots_.push_back(tmp);
        //scplots_.push_back(std::vector<container1D >());

        size_=cplots_.size();
        return;
    }

    std::cout << "controlPlotBasket::initStep: new step has to have index (oldsteps)+1" << std::endl;
    throw std::logic_error("controlPlotBasket::initStep: new step has to have index (oldsteps)+1");


}
void controlPlotBasket::initSteps(size_t steps){
    initphase_=true; //removes some checks
    for(size_t i=0;i<=steps;i++)
        makeControlPlots(i);
    initphase_=false;
}


}//namespace
