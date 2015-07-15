/*
 * plotterMultiStack.cc
 *
 *  Created on: Jun 2, 2015
 *      Author: kiesej
 */


#include "../interface/plotterMultiStack.h"

namespace ztop{

void plotterMultiStack::addStyleForAllPlots(const std::string & f,const std::string &sm,const std::string &em){
	for(size_t idx=0;idx<plotter_.size();idx++){
		plotterControlPlot * pl = (plotterControlPlot *)plotter_.at(idx);
		pl->addStyleFromFile(f,sm,em);
	}
}

void plotterMultiStack::addStack(const histoStack *s){
	if(!s)
		throw std::logic_error("plotterMultiColumn::addStack: pointer NULL");
	if(stacks_.size() >= plotter_.size())
		throw std::logic_error("plotterMultiColumn::addStack: not set up for that many plots (check style file)");

	stacks_.push_back(s);

}


void  plotterMultiStack::addPlotData(const size_t & idx){
	if(idx>=stacks_.size())
		throw std::out_of_range("plotterMultiStack::addPlotData: index out of range");
	plotterControlPlot * pl = (plotterControlPlot *)plotter_.at(idx); //cast
	pl->setStack(stacks_.at(idx));

}

float plotterMultiStack::getYMax(const size_t& idx,bool divbw)const{
	return stacks_.at(idx)->getYMax(divbw);
}
float plotterMultiStack::getYMin(const size_t& idx,bool divbw)const{
	float min= stacks_.at(idx)->getYMin(divbw,true);
	if(min < 0) min=0; //this is a stacked histo! but maybe the syst sum may go below
	return min;
}
void plotterMultiStack::readStylePriv(const std::string & file, bool requireall){
	plotterMultiColumn::readStylePrivMarker(file,requireall,"plotterMultiStack");
}

}
