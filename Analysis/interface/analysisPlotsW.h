/*
 * analysisPlotsW.h
 *
 *  Created on: 30 May 2016
 *      Author: jkiesele
 */

#ifndef TTZANALYSIS_ANALYSIS_INTERFACE_ANALYSISPLOTSW_H_
#define TTZANALYSIS_ANALYSIS_INTERFACE_ANALYSISPLOTSW_H_

#include "analysisPlots.h"

namespace ztop{
class analysisPlotsW: public analysisPlots{
public:
	analysisPlotsW(size_t step):analysisPlots(step),
	asymm(0){}


	void bookPlots();
	void fillPlotsReco();
	void fillPlotsGen();


private:
	histo1DUnfold*
	asymm;
	static double WMASS;

	float calcAsymmReco()const;
	float calcAsymmGen()const;

	template <class T, class U>
	float asymmValue(T,U)const;


};


template <class T, class U>
float analysisPlotsW::asymmValue(T muon,U jet)const{
	double deltaphi=deltaPhi(muon->phi(),jet->phi() );
	double sindphi=std::sin(deltaphi);

	double signeta=1;
	if(muon->eta()<0)
		signeta=-1;
	float chargefloat=muon->q();
	double asymm= muon->pt()*2 / WMASS * sindphi * chargefloat * signeta ;
	return asymm;
}


}



#endif /* TTZANALYSIS_ANALYSIS_INTERFACE_ANALYSISPLOTSW_H_ */
