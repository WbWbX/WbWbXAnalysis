/*
 * wReweightingPlots.h
 *
 *  Created on: 30 May 2016
 *      Author: jkiesele
 */

#ifndef TTZANALYSIS_ANALYSIS_INTERFACE_WREWEIGHTINGPLOTS_H_
#define TTZANALYSIS_ANALYSIS_INTERFACE_WREWEIGHTINGPLOTS_H_

#include "TtZAnalysis/Tools/interface/histo2D.h"
#include <stdexcept>
#include <vector>
#include "NTFullEvent.h"
#include "HTransformToCS.h"

namespace ztop{

class wReweightingPlots {
public:
	wReweightingPlots():evt_(0),enabled_(false),signal_(false),transf_(8000){}
	~wReweightingPlots(){
		clearHists();
	}


	void bookPlots();
	void fillPlots();

	void setEvent(NTFullEvent * evt){evt_=evt;}

	void enable(bool en=true){enabled_=en;}

	void setSignal(bool sig=true){signal_=sig;}

private:

	histo2D*  getHisto(const float & vara,const float & varb,const float & varc);
	histo1D* getHisto1D(const float & vara,const float & varb,const float & varc);

	void createBinning( const std::vector<float>& a, const std::vector<float>& b, const std::vector<float>& c);
	std::vector<float>  makeBinning(std::vector<float> a)const;

	NTFullEvent * event(){if(evt_) return evt_; throw std::runtime_error("wControlPlots_technical: event NULL");}

	NTFullEvent * evt_;
	bool enabled_,signal_;

	std::vector< std::vector< std::vector<histo2D* > > >hists_;
	std::vector< std::vector< std::vector<histo1D* > > >hists1D_;
	std::vector<float> binsa_,binsb_,binsc_;

	size_t get1DIndex(const float & val, const std::vector<float> &)const;

	std::vector<float> makeBins(float lowtha, float lowthb, float lowthc)const;

	void clearHists();

	HTransformToCS transf_;

};




}//namespace

#endif /* TTZANALYSIS_ANALYSIS_INTERFACE_WREWEIGHTINGPLOTS_H_ */
