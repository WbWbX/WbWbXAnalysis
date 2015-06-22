/*
 * plotterMultiCompare.cc
 *
 *  Created on: Jun 2, 2015
 *      Author: kiesej
 */



#include "../interface/plotterMultiCompare.h"


namespace ztop{


void plotterMultiCompare::setNominalPlots(const std::vector<histo1D> *c,bool divbw){
	if(c->size()!=plotter_.size())
		throw std::out_of_range("plotterMultiCompare::setNominalPlots: plotter not configured for required number of input plots");

	for(size_t i=0;i<c->size();i++){
		((plotterCompare*)plotter_.at(i))->setNominalPlot(&c->at(i),divbw);

	}

}
void plotterMultiCompare::setComparePlots(const std::vector<histo1D> *c,size_t idx,bool divbw){
	if(c->size()!=plotter_.size())
		throw std::out_of_range("plotterMultiCompare::setNominalPlots: plotter not configured for required number of input plots");

	for(size_t i=0;i<c->size();i++){
		((plotterCompare*)plotter_.at(i))->setComparePlot(&c->at(i),idx,divbw);

	}
}
void plotterMultiCompare::setNominalPlots(const std::vector<graph> *c){
	if(c->size()!=plotter_.size())
		throw std::out_of_range("plotterMultiCompare::setNominalPlots: plotter not configured for required number of input plots");

	for(size_t i=0;i<c->size();i++){
		((plotterCompare*)plotter_.at(i))->setNominalPlot(&c->at(i));

	}
}
void plotterMultiCompare::setComparePlots(const std::vector<graph> *c,size_t idx){
	if(c->size()!=plotter_.size())
		throw std::out_of_range("plotterMultiCompare::setNominalPlots: plotter not configured for required number of input plots");

	for(size_t i=0;i<c->size();i++){
		((plotterCompare*)plotter_.at(i))->setComparePlot(&c->at(i),idx);

	}
}



float  plotterMultiCompare::getYMax(const size_t& idx,bool divbw)const{
	if(plotter_.size()<1)
		throw std::out_of_range("plotterMultiCompare::getYMax: no plotter defined yet");
	return ((plotterCompare*)plotter_.at(idx))->getMaxMinUpper(true);
}
float  plotterMultiCompare::getYMin(const size_t& idx,bool divbw)const{
	if(plotter_.size()<1)
		throw std::out_of_range("plotterMultiCompare::getYMin: no plotter defined yet");
	return ((plotterCompare*)plotter_.at(idx))->getMaxMinUpper(false);
}

void plotterMultiCompare::readStylePriv(const std::string & file, bool requireall){
	plotterMultiColumn::readStylePrivMarker(file,requireall,"plotterMultiCompare");
}


}
