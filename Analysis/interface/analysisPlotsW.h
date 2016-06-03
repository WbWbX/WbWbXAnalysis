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
	isiso_(false),asymdeta_(0)
	{
		etaRanges_ << 0 << 0.4 << 0.8 << 1.2 << 1.6 << 2. << 2.4 << 2.8 << 3.2 << 3.6; //just for testing use only one

	}


	void bookPlots();
	void fillPlotsReco();
	void fillPlotsGen();

	void setIsIso(const bool & is){isiso_=is;}

private:
	bool isiso_;

	std::vector< histo1DUnfold* >
	asymmiso_;
	std::vector< histo1DUnfold* >
	mTnoniso_;
	histo1DUnfold *
	asymdeta_;

	static double WMASS;

	float calcAsymmReco(float& absdeta)const;
	float calcAsymmGen(float& absdeta)const;

	float calcMTReco(float& absdeta)const;

	template <class T, class U>
	float asymmValue(T,U)const;

	size_t getEtaIndex(const float & deta)const;
	std::vector<float> etaRanges_;

	TString produceDEtaString(size_t i)const;

	bool checkEventContentReco()const;

};


template <class T, class U>
float analysisPlotsW::asymmValue(T muon,U jet)const{
	double deltaphi=deltaPhi(muon->phi(),jet->phi() );
	double sindphi=std::sin(deltaphi);

	double signeta=1;
	if(muon->eta()<0)
		signeta=-1;
	float chargefloat=muon->q();
	if(!muon->q())
		throw std::logic_error("analysisPlotsW::asymmValue: muon with 0 charge!");
	double asymm= muon->pt()*2 / WMASS * sindphi * chargefloat * signeta ;
	return asymm;
}


}

/*
 *
 *
 *
 *
 */


#endif /* TTZANALYSIS_ANALYSIS_INTERFACE_ANALYSISPLOTSW_H_ */
