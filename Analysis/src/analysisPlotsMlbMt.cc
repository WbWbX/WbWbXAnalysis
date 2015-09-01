/*
 * analysisPlotsJan.cc
 *
 *  Created on: Feb 12, 2014
 *      Author: kiesej
 */


#include "../interface/analysisPlotsMlbMt.h"
#include "../interface/AnalysisUtils.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "TtZAnalysis/DataFormats/src/classes.h"
#include "TtZAnalysis/DataFormats/interface/helpers.h"

namespace ztop{


void analysisPlotsMlbMt::setJetCategory(size_t nbjets,size_t njets){
	if(nbjets < 1 || nbjets>2){
		if(njets<1)
			jetcategory=cat_0bjet0jet;
		else if(njets<2)
			jetcategory=cat_0bjet1jet;
		else if(njets<3)
			jetcategory=cat_0bjet2jet;
		else if(njets>2)
			jetcategory=cat_0bjet3jet;
	}
	else if(nbjets < 2){
		if(njets<1)
			jetcategory=cat_1bjet0jet;
		else if(njets<2)
			jetcategory=cat_1bjet1jet;
		else if(njets<3)
			jetcategory=cat_1bjet2jet;
		else if(njets>2)
			jetcategory=cat_1bjet3jet;
	}
	else if(nbjets < 3){
		if(njets<1)
			jetcategory=cat_2bjet0jet;
		else if(njets<2)
			jetcategory=cat_2bjet1jet;
		else if(njets<3)
			jetcategory=cat_2bjet2jet;
		else if(njets>2)
			jetcategory=cat_2bjet3jet;
	}
}

void analysisPlotsMlbMt::bookPlots(){
	if(!use()) return;
	using namespace std;
	using namespace ztop;


	vector<float> genmlbmin_bins,genmlb_bins;
	genmlbmin_bins<<0 ;
	genmlb_bins<<0;
	for(float i=20;i<=165;i+=5){
		genmlbmin_bins <<i;
		genmlb_bins << i;
	}
	genmlb_bins << 180 <<350;
	genmlbmin_bins << 180 << 200 << 350;

	vector<float> ivangen_mlbbins;
	ivangen_mlbbins << 0 << 70 << 116 << 150 <<400;
	//vector<float> mlb_bins=ztop::subdivide<float>(genmlb_bins,2);
	vector<float> ivan_mlbbins=subdivide<float>(ivangen_mlbbins,5);
	vector<float> semicoarsebins;
	//if(finemlbbinning)
	semicoarsebins=histo1D::createBinning(10,20,160);
	vector<float> finebins=histo1D::createBinning(18,20,160);
	//extraplots2_
	//else
	//	semicoarsebins=histo1D::createBinning(10,20,160);
	vector<float> coarsebins=histo1D::createBinning(5,20,160);
	vector<float> verycoarsebins=histo1D::createBinning(3,20,160);

	Mlb=addPlot(genmlb_bins,genmlb_bins,"m_lb leading unfold","M_{lb} [GeV]", "Events/GeV");

	mlb=addPlot(genmlb_bins,genmlb_bins,"m_lb","m_{lb}* [GeV]", "Events/GeV");

	mlbmin=addPlot(genmlbmin_bins,genmlbmin_bins,"m_lb min","m_{lb}^{min} [GeV]", "Events/GeV");

	for(size_t nbjet=0;nbjet<3;nbjet++){
		for(size_t nadd=0;nadd<4;nadd++){
			setJetCategory(nbjet,nadd);
			if(nbjet>0){
				if(nbjet<2 && nadd < 3){
					extraplots_.at(jetcategory)=addPlot(semicoarsebins,semicoarsebins,"m_lb min "+toTString(nbjet)+","+toTString(nadd)+ " b-jets","m_{lb}^{min} [GeV]", "Events/GeV");
					extraplots2_.at(jetcategory)=addPlot(finebins,finebins,"m_lb min fine "+toTString(nbjet)+","+toTString(nadd)+ " b-jets","m_{lb}^{min} [GeV]", "Events/GeV");
				}
				else if(nadd < 2){
					extraplots_.at(jetcategory)=addPlot(coarsebins,coarsebins,"m_lb min "+toTString(nbjet)+","+toTString(nadd)+ " b-jets","m_{lb}^{min} [GeV]", "Events/GeV");
					extraplots2_.at(jetcategory)=addPlot(semicoarsebins,semicoarsebins,"m_lb min fine "+toTString(nbjet)+","+toTString(nadd)+ " b-jets","m_{lb}^{min} [GeV]", "Events/GeV");
				}
				else{
					extraplots_.at(jetcategory)=addPlot(verycoarsebins,verycoarsebins,"m_lb min "+toTString(nbjet)+","+toTString(nadd)+ " b-jets","m_{lb}^{min} [GeV]", "Events/GeV");
					extraplots2_.at(jetcategory)=addPlot(semicoarsebins,semicoarsebins,"m_lb min fine "+toTString(nbjet)+","+toTString(nadd)+ " b-jets","m_{lb}^{min} [GeV]", "Events/GeV");
				}
			}
		}
	}
	mlbivansbins=addPlot(ivangen_mlbbins,ivan_mlbbins,"m_lb ivansbins","m_{lb}^{ivan} [GeV]", "Events/GeV");

	mlbminbsrad=addPlot(genmlbmin_bins,genmlbmin_bins,"m_lb min bsrad","m_{lb}^{min} [GeV]", "Events/GeV");

	vector<float> tmpbins=makebins(40,20,200);

	leadleppt = addPlot(tmpbins,tmpbins,"leading top-lepton pt","p_{t}^{l} [GeV]", "Events/GeV");

	tmpbins=makebins(40,20,350);
	mll  = addPlot(tmpbins,tmpbins,"dilepton mass","m_{ll} [GeV]", "Events/GeV");



	vector<float> inclbins; inclbins << 0.5 << 1.5; //vis PS, fullPS


	total=addPlot(inclbins,inclbins,"total","bin","Events");
	total->setBinByBin(true); //independent bins
	total->addTag(taggedObject::dontDivByBW_tag);


	vistotal=addPlot(inclbins,inclbins,"vis total","bin","Events");
	vistotal->setBinByBin(true); //independent bins
	vistotal->addTag(taggedObject::dontDivByBW_tag);

	mll0b=addPlot(tmpbins,tmpbins,"dilepton mass 0,0 b-jets ","m_{ll} [GeV]", "Events/GeV");
}


void analysisPlotsMlbMt::fillPlotsGen(){
	if(!use()) return;
	if(!event()) return;
	using namespace std;
	using namespace ztop;

	if(event()->gentops && event()->gentops->size()>0)
		total->fillGen(1.,puweight());

	//////////// mlb analysis //////////////

	//produce visible phase space collections
	std::vector<NTGenParticle*> genvisleptons3=produceCollection(event()->genleptons3,20,2.4);
	std::vector<NTGenJet*> genvisjets=produceCollection(event()->genjets,30,2.4);
	std::vector<NTGenParticle*> genvisbs=produceCollection(event()->genbs,30,2.4);

	if(requireNumber(2,genvisleptons3) && requireNumber(2,genvisjets)){ //vis PS
		vistotal->fillGen(1.,puweight());
	}


	//calculate mlbs based on ME lepton
	if(requireNumber(2,genvisleptons3)){
		// no check needed if from top because only those are stored!
		if(requireNumber(1,genvisbs) ){
			//vis ps cuts on b-jets
			NTGenParticle* leadvisb=genvisbs.at(0);

			if(leadvisb){
				//	if(event()->genbs->at(0)->pt()>30){
				//	leadvisb=event()->genbs->at(0);

				NTLorentzVector<float> bjetp4=leadvisb->p4();
				NTLorentzVector<float> llepp4=genvisleptons3.at(0)->p4();
				NTLorentzVector<float> slepp4=genvisleptons3.at(1)->p4();

				NTGenParticle* pairedlep=genvisleptons3.at(0);

				float fMlb=(bjetp4+llepp4).m();
				float fm2lb=(bjetp4+slepp4).m();
				float fmlb=fMlb;
				float fmlbmin=fMlb;
				if(fMlb>mlbcombthresh_ && fm2lb<mlbcombthresh_)  fmlb=fm2lb;
				if(fMlb>fm2lb){
					fmlbmin=fm2lb;
					pairedlep=genvisleptons3.at(1);
				}

				if(pairedlep->q() * leadvisb->pdgId() > 0)
					rightassocounter_++;
				totalcounter_++;

				Mlb->fillGen(fMlb,puweight());
				mlb->fillGen(fmlb,puweight());
				mlbmin->fillGen(fmlbmin,puweight());



				mlbivansbins->fillGen(fmlb,puweight());
			}



		}

		std::vector<NTGenParticle*> genvisbsrad=produceCollection(event()->genbsrad,30,2.4);

		if(requireNumber(1,genvisbsrad) && event()->genbs){
			//check if its the right one
			//same charge as mother b-quark
			//and highest pt (if more have same charge)
			NTGenParticle* leadvisb=0;
			for(size_t i=0;i<genvisbsrad.size();i++){
				for(size_t j=0;j<genvisbsrad.at(i)->mothers().size();j++){
					if(genvisbsrad.at(i)->mothers().at(j)->q() == genvisbsrad.at(i)->q()){
						leadvisb=genvisbsrad.at(i);
						break;
					}
					if(leadvisb)
						break;
				}
			}
			if(leadvisb){

				NTLorentzVector<float> bjetp4=leadvisb->p4();
				NTLorentzVector<float> llepp4=genvisleptons3.at(0)->p4();
				NTLorentzVector<float> slepp4=genvisleptons3.at(1)->p4();

				float fMlb=(bjetp4+llepp4).m();
				float fm2lb=(bjetp4+slepp4).m();

				float fmlbmin=fMlb;
				if(fMlb>fm2lb) fmlbmin=fm2lb;

				mlbminbsrad->fillGen(fmlbmin,puweight());
			}
		}
	}

	if(requireNumber(1,genvisleptons3))
		leadleppt->fillGen(genvisleptons3.at(0)->pt(),puweight());

	if(requireNumber(2,genvisleptons3)){
		NTLorentzVector<float> mllgen = genvisleptons3.at(0)->p4() + genvisleptons3.at(1)->p4();
		mll->fillGen(mllgen.m(),puweight());
	}


	std::vector<NTGenParticle*> genvisleptons1=produceCollection(event()->genleptons1,20,2.4);

	if(genvisleptons1.size()>1){
		for(size_t i=0;i<extraplots_.size();i++){
			if(extraplots_.at(i)){
				extraplots_.at(i)->fillGen(20.5,puweight()); //same as for incl xsec
				extraplots2_.at(i)->fillGen(20.5,puweight());
			}
		}
		mll0b->fillGen(20.5,puweight());
	}


}

void analysisPlotsMlbMt::fillPlotsReco(){
	if(!use()) return;
	if(!event()) return;
	using namespace std;
	using namespace ztop;

	total->fillReco(1,puweight());

	vistotal->fillReco(1,puweight());

	size_t nbjets=0;
	size_t naddjets=0;
	if(event()->selectedbjets && event()->selectedjets){
		nbjets=event()->selectedbjets->size();
		naddjets=event()->selectedjets->size() - nbjets;
		setJetCategory(nbjets,naddjets);
	}

	/*
	 * this is not an event selection. just safety measures
	 */
	if(event()->leadinglep && event()->secleadinglep && event()->selectedbjets ){
		if(event()->selectedbjets->size()>0 ){
			NTLorentzVector<float> bjetp4=event()->selectedbjets->at(0)->p4();
			NTLorentzVector<float> llepp4=event()->leadinglep->p4();
			NTLorentzVector<float> slepp4=event()->secleadinglep->p4();

			float fMlb=(bjetp4+llepp4).m();
			float fm2lb=(bjetp4+slepp4).m();
			float fmlb=fMlb;
			float fmlbmin=fMlb;
			if(fMlb>mlbcombthresh_ && fm2lb<mlbcombthresh_)  fmlb=fm2lb;
			if(fMlb>fm2lb) fmlbmin=fm2lb;

			Mlb->fillReco(fMlb,puweight());
			mlb->fillReco(fmlb,puweight());
			mlbmin->fillReco(fmlbmin,puweight());
			mlbminbsrad->fillReco(fmlbmin,puweight());


			mlbivansbins->fillReco(fmlb,puweight());
			if(extraplots_.at(jetcategory)){
				extraplots_.at(jetcategory)->fillReco(fmlbmin,puweight());
				extraplots2_.at(jetcategory)->fillReco(fmlbmin,puweight());
			}
		}
	}
	if(event()->leadinglep)
		leadleppt->fillReco(event()->leadinglep->pt(),puweight());

	if(event()->mll){
		mll->fillReco(*event()->mll,puweight());
		if(jetcategory==cat_0bjet0jet)
			mll0b->fillReco(*event()->mll,puweight());
	}
}

}//ns
