/*
 * analysisPlotsW.cc
 *
 *  Created on: 30 May 2016
 *      Author: jkiesele
 */


#include "../interface/analysisPlotsW.h"
#include "TtZAnalysis/DataFormats/interface/NTJet.h"
#include "TtZAnalysis/DataFormats/interface/NTMuon.h"
#include "TtZAnalysis/DataFormats/interface/NTGenParticle.h"
#include "TtZAnalysis/DataFormats/interface/NTGenJet.h"

namespace ztop{

double analysisPlotsW::WMASS=80.39;


float analysisPlotsW::calcAsymmReco()const{
	if(!((event()->hardjets && event()->hardjets->size())
			&& event()->isomuons && event()->isomuons->size() ))
		return -99;
	NTMuon * muon=event()->isomuons->at(0);
	NTJet * jet = event()->hardjets->at(0);

	return asymmValue(muon,jet);

}
float analysisPlotsW::calcAsymmGen()const{
	if(!((event()->genjets && event()->genjets->size())
			&& event()->genleptons3 && event()->genleptons3->size() ))
		return -99;
	//what do we exactly want? jets, W on truth level?


	NTGenParticle * muon=event()->genleptons3->at(0);
	NTGenJet *     jet=event()->genjets->at(0);

	return asymmValue(muon,jet);
}


void analysisPlotsW::bookPlots(){

	std::vector<float> genbins=histo1D::createBinning(100,-1.4,1.4);

	asymm=addPlot(genbins,genbins,"aymm","asymm","Events");

}

void analysisPlotsW::fillPlotsReco(){
	if(!use()) return;
	if(!event()) return;

	float asymmval=calcAsymmReco();
	if(asymmval>-98)
		asymm->fillReco(calcAsymmReco(), puweight());

}

void analysisPlotsW::fillPlotsGen(){
	if(!use()) return;
	if(!event()) return;


	float asymmval=calcAsymmGen();
	if(asymmval>-98)
		asymm->fillGen(asymmval,puweight());
}



}//ns

