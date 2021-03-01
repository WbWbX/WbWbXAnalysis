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
#include "WbWbXAnalysis/DataFormats/interface/NTJECUncertainties.h"
#include "wChargeFlipGenerator.h"
#include "wNLOReweighter.h"

namespace ztop{

class tTreeHandler;

class wAnalyzer : public basicAnalyzer{
public:
	wAnalyzer():basicAnalyzer(),genonly_(false){

	}
	~wAnalyzer(){}

	fileForker::fileforker_status  start();

	void addWeightIndex(size_t idx){weightindicies_.push_back(idx);}

	NTJECUncertainties * getJecUnc(){return &jecunc_;}
	const NTJECUncertainties * getJecUnc()const{return &jecunc_;}

	wChargeFlipGenerator * getChargeFlip(){return &chargeflip_;}
	const wChargeFlipGenerator * getChargeFlip()const {return &chargeflip_;}

	wNLOReweighter * getNLOReweighter(){return &nlorew_;}
	const wNLOReweighter * getNLOReweighter()const {return &nlorew_;}

	scalefactors * getMuonSF(){return &muonSF_;}
	const scalefactors * getMuonSF()const {return &muonSF_;}

	scalefactors * getMuonESF(){return &muonESF_;}
	const scalefactors * getMuonESF()const {return &muonESF_;}

	void setGenOnly(bool set){genonly_=set;}


private:

	void pairLeptons( std::vector<NTGenParticle*> * v, NTGenParticle*& lep, NTGenParticle*& neutr, bool isdy=false)const;

	fileForker::fileforker_status writeOutput();

	void analyze(size_t id);

	void createNormalizationInfo(tTreeHandler* );

	std::vector<size_t> weightindicies_;

	NTJECUncertainties jecunc_;
	wChargeFlipGenerator chargeflip_;
	wNLOReweighter nlorew_;
	scalefactors muonSF_,muonESF_;
	bool genonly_;
};

}


#endif /* TTZANALYSIS_ANALYSIS_INTERFACE_WANALYZER_H_ */
