/*
 * wAnalyzer.h
 *
 *  Created on: 23 May 2016
 *      Author: kiesej
 */

#ifndef TTZANALYSIS_ANALYSIS_INTERFACE_WANALYZER_H_
#define TTZANALYSIS_ANALYSIS_INTERFACE_WANALYZER_H_

#include "basicAnalyzer.h"
#include "TString.h"
#include "TtZAnalysis/DataFormats/interface/NTJECUncertainties.h"
#include "wChargeFlipGenerator.h"
#include "wNLOReweighter.h"

namespace ztop{

class tTreeHandler;

class wAnalyzer : public basicAnalyzer{
public:
	wAnalyzer():basicAnalyzer(){}
	~wAnalyzer(){}

	fileForker::fileforker_status  start();

	void addWeightIndex(size_t idx){weightindicies_.push_back(idx);}

	NTJECUncertainties * getJecUnc(){return &jecunc_;}
	const NTJECUncertainties * getJecUnc()const{return &jecunc_;}

	wChargeFlipGenerator * getChargeFlip(){return &chargeflip_;}
	const wChargeFlipGenerator * getChargeFlip()const {return &chargeflip_;}

	wNLOReweighter * getNLOReweighter(){return &nlorew_;}
	const wNLOReweighter * getNLOReweighter()const {return &nlorew_;}

private:

	void pairLeptons( std::vector<NTGenParticle*> * v, NTGenParticle*& lep, NTGenParticle*& neutr)const;

	fileForker::fileforker_status writeOutput();

	void analyze(size_t id);

	void createNormalizationInfo(tTreeHandler* );

	std::vector<size_t> weightindicies_;

	NTJECUncertainties jecunc_;
	wChargeFlipGenerator chargeflip_;
	wNLOReweighter nlorew_;
};

}


#endif /* TTZANALYSIS_ANALYSIS_INTERFACE_WANALYZER_H_ */
