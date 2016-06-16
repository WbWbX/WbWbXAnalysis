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
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"

namespace ztop{

double analysisPlotsW::WMASS=80.39;


float analysisPlotsW::calcAsymmReco(float& absdeta)const{

	if(!checkEventContentReco())
		return -99;

	NTMuon * muon=0;
	if(isiso_)
		muon=event()->isomuons->at(0);
	else
		muon=event()->nonisomuons->at(0);

	NTJet * jet = event()->hardjets->at(0);

	absdeta=std::abs(muon->eta() - jet->eta());

	return asymmValue(muon,jet);

}
float analysisPlotsW::calcAsymmGen(float& absdeta)const{
	if(!((event()->genjets && event()->genjets->size())
			&& event()->allgenparticles ))
		return -99;
	//what do we exactly want? jets, W on truth level?
	NTGenParticle * muon=0;
	//get the muon, and maybe other stuff
	for(size_t i=0;i<event()->allgenparticles->size();i++){
		NTGenParticle * p=event()->allgenparticles->at(i);
		if(!(p->pdgId() == 13 || p->pdgId() == -13)) continue; //only muons
		// matching to W? --> doensnt work like this
		//if( p->motherPdgID() != -24 && p->motherPdgID() !=24
		//	&& p->grandMotherPdgID() != 24 && p->grandMotherPdgID() !=-24 ) continue;
		//here is status code stuff here

		if(p->pt()<25)continue;
		if(fabs(p->eta())>2.1)continue;
		if(p->status() != 23 && p->status() != 3) continue; //?

		muon=p; //directly take the first one
		break;
	}
	if(!muon)
		return -99;

	NTGenJet *     sjet=0;
	for(size_t i=0;i<event()->genjets->size();i++){
		NTGenJet *     jet=event()->genjets->at(i);
		if(jet->pt()<25) continue;
		if(dR(muon,jet)<0.1) continue;
		if(fabs(jet->eta())>2.5) continue;
		sjet=jet;
		break;
	}


	if(!sjet)
		return -99;
	absdeta=std::abs(muon->eta() - sjet->eta());

	return asymmValue(muon,sjet);
}

float analysisPlotsW::calcMTReco(float& absdeta)const{
	if(!checkEventContentReco())
		return -99;


	NTMuon * muon=0;
	if(isiso_)
		muon=event()->isomuons->at(0);
	else
		muon=event()->nonisomuons->at(0);

	NTJet * jet = event()->hardjets->at(0);

	absdeta=std::abs(muon->eta() - jet->eta());

	return 60;

}

TString analysisPlotsW::produceDEtaString(size_t i)const{
	return "_deta_"+toTString(etaRanges_.at(i))+"-"+toTString(etaRanges_.at(i+1))+"_";
}

bool analysisPlotsW::checkEventContentReco()const{

	bool jetsok=event()->hardjets && event()->hardjets->size();
	bool isomuonsok=event()->isomuons && event()->isomuons->size() ;
	bool nonisomuonsok=event()->nonisomuons && event()->nonisomuons->size();

	bool muonsok= (isiso_ && isomuonsok) || (!isiso_ && nonisomuonsok);

	return muonsok && jetsok;
}

void analysisPlotsW::bookPlots(){
	if(etaRanges_.size()<2)
		throw std::out_of_range("analysisPlotsW::bookPlots: no deta range defined!");

	std::vector<float> asymmgenbins=histo1D::createBinning(40,-1.4,1.4);
	std::vector<float> mTbins      =histo1D::createBinning(50,10,130);

	for(size_t i=0;i<etaRanges_.size()-1;i++){
		TString etastring=produceDEtaString(i);
		asymmiso_.push_back(addPlot(asymmgenbins,asymmgenbins,"asymm"+etastring,"asymm","Events"));
		asymmnoniso_.push_back(addPlot(asymmgenbins,asymmgenbins,"asymm_noiso"+etastring,"asymm","Events"));
	}

	for(size_t i=0;i<etaRanges_.size()-1;i++){
		TString etastring=produceDEtaString(i);
		mTnoniso_.push_back(addPlot(mTbins,mTbins,"mT"+etastring,"M_{T} [GeV]","Events"));
	}

	std::vector<float> etabins=etaRanges_;
	asymmisofull_ = addPlot(asymmgenbins,asymmgenbins,"asymm full deta", "asymm","Events");
	asymmnonisofull_ = addPlot(asymmgenbins,asymmgenbins,"asymm_noiso full deta", "asymm","Events");
	mTnonisofull_ = addPlot(mTbins,mTbins,"mT full deta","M_{T} [GeV]","Events") ;
}

void analysisPlotsW::fillPlotsReco(){
	if(!use()) return;
	if(!event()) return;

	float deta=0;
	float asymmval=calcAsymmReco(deta);
	size_t detabin=getEtaIndex(deta );
	if(isiso_){
		if(asymmval>-98){
			asymmiso_.at(detabin)->fillReco(asymmval, puweight());
			asymmisofull_->fillReco(asymmval, puweight());
		}
	}
	else{
		float deta=0;
		float mT=calcMTReco(deta);
		if(mT>0){
			mTnoniso_.at(detabin)->fillReco(mT,puweight());
			asymmnoniso_.at(detabin)->fillReco(asymmval, puweight());
			asymmnonisofull_->fillReco(asymmval, puweight());
		}
	}
}

void analysisPlotsW::fillPlotsGen(){
	if(!use()) return;
	if(!event()) return;

	float deta;
	float asymmval=calcAsymmGen(deta);
	size_t detabin=getEtaIndex(deta );
	if(isiso_){
		if(asymmval>-98){
			asymmiso_.at(detabin)->fillGen(asymmval, puweight());
			asymmisofull_->fillGen(asymmval, puweight());
		}
	}
	else{
		//float deta=0;
		//float mT=calcMTReco(deta);

		//	mTnoniso_.at(detabin)->fillGen(mT,puweight());
		asymmnoniso_.at(detabin)->fillGen(asymmval, puweight());
		asymmnonisofull_->fillGen(asymmval, puweight());
		//	mTnonisofull_->fillGen(mT, puweight());

	}
}


size_t analysisPlotsW::getEtaIndex(const float & var)const{
	if(etaRanges_.size()<2)
		throw std::out_of_range("analysisPlotsW::getEtaIndex: no range defined!");

	size_t binno=0;
	std::vector<float>::const_iterator it=std::lower_bound(etaRanges_.begin()+1, etaRanges_.end(), var);
	//this mimics root bin behaviour which is different from lower_bound
	if(var==*it)
		binno= it-etaRanges_.begin();
	else
		binno= it-etaRanges_.begin()-1;

	if(binno == etaRanges_.size()-1)
		binno--; //merge overflow in last bin

	return binno; //not optimal but tested code
}


}//ns

