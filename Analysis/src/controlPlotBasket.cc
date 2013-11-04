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
		if(!event()->puweight){
			std::cout << "controlPlotBasket::addPlot: puweight needs to be set" << std::endl;
			throw std::logic_error("controlPlotBasket::addPlot: puweight needs to be set");
		}
		container1D * cont = new container1D(tempbins_,name+" step "+toTString(tmpstep_), xaxisname, yaxisname, mergeufof);
		cplots_.at(tmpstep_).push_back(cont);
	}
	//else just return pointer
	lcont_=cplots_[tmpstep_][tmpidx_++];
	return lcont_;//; //not safe but fast
}

void controlPlotBasket::initStep(const size_t & step){
	//check if already created

	tmpidx_=0;
	tmpstep_=step;
	if(step<size_){
		tmpnewstep_=false;
		return;
	}
	else if(step==size_){//create step
		tmpnewstep_=true;
		cplots_.push_back(std::vector<container1D *>());
		size_=cplots_.size();
		return;
	}

	std::cout << "controlPlotBasket::initStep: new step has to have index (oldsteps)+1" << std::endl;
	throw std::logic_error("controlPlotBasket::initStep: new step has to have index (oldsteps)+1");


}


}//namespace
