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

	absdeta=fabs(muon->eta() - jet->eta());

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
		if(jet->pt()<50) continue;
		if(dR(muon,jet)<0.01) continue;
		if(fabs(jet->eta())>2.5) continue;
		sjet=jet;
		break;
	}


	if(!sjet)
		return -99;
	absdeta=fabs(muon->eta() - sjet->eta());

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

	absdeta=(muon->eta() - jet->eta());

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
	if(!use()) return;

	if(etaRanges_.size()<2)
		throw std::out_of_range("analysisPlotsW::bookPlots: no deta range defined!");

	std::vector<float> asymmbins=histo1D::createBinning(18,-1.0,1.0);
	asymmbins.insert(asymmbins.begin(),-1.2);
	asymmbins.push_back(1.2); //make larger first and last bin to capture resolution effects
	std::vector<float> mTbins      =histo1D::createBinning(50,10,130);

	for(size_t i=0;i<etaRanges_.size()-1;i++){
		TString etastring=produceDEtaString(i);
		asymmiso_.push_back(addPlot1D(asymmbins,"pttrans"+etastring,"p_{T}^{#perp}/M_{W}","Events"));
		asymmnoniso_.push_back(addPlot1D(asymmbins,"pttrans_noiso"+etastring,"p_{T}^{#perp}/M_{W}","Events"));
	}

	for(size_t i=0;i<etaRanges_.size()-1;i++){
		TString etastring=produceDEtaString(i);
		mTnoniso_.push_back(addPlot1D(mTbins,"mT"+etastring,"M_{T} [GeV]","Events"));
	}
	bool tmpcm=histo1DUnfold::c_makelist;
	histo1DUnfold::c_makelist=true;
	detaresolution_=new histo1DUnfold(etaRanges_,etaRanges_,"DeltaEtaResolution","#Delta#eta(W,j)","Events");

	std::vector<float> etabins=histo1D::createBinning(20, -4, 4);
	jetetaresolution_=new histo1DUnfold(etabins,etabins,"JetEtaResolution","#eta(j)","Events");
	lepetaresolution_=new histo1DUnfold(etabins,etabins,"LepEtaResolution","#eta(l)","Events");
	histo1DUnfold::c_makelist=tmpcm;


	asymmisofull_ = addPlot1D(asymmbins,"pttrans full deta", "p_{T}^{#perp}/M_{W}","Events");
	asymmnonisofull_ = addPlot1D(asymmbins,"pttrans_noiso full deta", "p_{T}^{#perp}/M_{W}","Events");
	mTnonisofull_ = addPlot1D(mTbins,"mT full deta","M_{T} [GeV]","Events") ;
	//std::vector<float> detabins=histo1D::createBinning(23,-4.5,4.5);
	simpleDataAsymm_=addPlot1D(etaRanges_,"asymmdata","#Delta#eta(l,j)","<2 p_{T}^{#perp}/M_{W}>");


}

void analysisPlotsW::fillPlotsReco(){

	if(!use()) return;
	if(!event()) return;

	float deta=0;
	float asymmval=calcAsymmReco(deta);
	size_t detabin=getEtaIndex(deta );
	if(isiso_){
		if(asymmval>-98){
			asymmiso_.at(detabin)->fill(asymmval, puweight());
			asymmisofull_->fill(asymmval, puweight());
		}
	}
	else{
		float deta=0;
		float mT=calcMTReco(deta);
		if(mT>0){
			mTnoniso_.at(detabin)->fill(mT,puweight());
			asymmnoniso_.at(detabin)->fill(asymmval, puweight());
			asymmnonisofull_->fill(asymmval, puweight());
		}
	}
	if(event()->leadinglep && event()->leadingjet){
		float deta=fabs(event()->leadinglep->eta() - event()->leadingjet->eta());
		detaresolution_->fillReco(deta, puweight() );
		jetetaresolution_->fillReco(event()->leadingjet->eta(), puweight() );
	}
	if(event()->leadinglep)
		lepetaresolution_->fillReco(event()->leadinglep->eta(), puweight() );

}

void analysisPlotsW::fillPlotsGen(){
	if(!use()) return;
	if(!event()) return;

	if(event()->genW && event()->genjets && event()->genjets->size()>0){
		float deta=fabs(event()->genW->eta() - event()->genjets->at(0)->eta());
		detaresolution_->fillGen(deta, puweight() );
		jetetaresolution_->fillGen(event()->genjets->at(0)->eta(), puweight() );
	}
	if(event()->genlepton)
		lepetaresolution_->fillGen(event()->genlepton->eta(), puweight() );
	//no unfilding histos
	/*
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

	}*/

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

