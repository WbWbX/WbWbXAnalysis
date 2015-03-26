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

	Mlb=addPlot(genmlb_bins,genmlb_bins,"m_lb leading unfold","M_{lb} [GeV]", "N_{evt}/GeV");

	mlb=addPlot(genmlb_bins,genmlb_bins,"m_lb","m_{lb}* [GeV]", "N_{evt}/GeV");

	mlbmin=addPlot(genmlbmin_bins,genmlbmin_bins,"m_lb min","m_{lb}^{min} [GeV]", "N_{evt}/GeV");

	mlbivansbins=addPlot(ivangen_mlbbins,ivan_mlbbins,"m_lb ivansbins","m_{lb}^{ivan} [GeV]", "N_{evt}/GeV");

	mlbminbsrad=addPlot(genmlbmin_bins,genmlbmin_bins,"m_lb min bsrad","m_{lb}^{min} [GeV]", "N_{evt}/GeV");

	vector<float> tmpbins=makebins(40,20,200);

	leadleppt = addPlot(tmpbins,tmpbins,"leading top-lepton pt","p_{t}^{l} [GeV]", "N_{evt}/GeV");

	tmpbins=makebins(40,20,350);
	mll  = addPlot(tmpbins,tmpbins,"dilepton mass","m_{ll} [GeV]", "N_{evt}/GeV");



	vector<float> inclbins; inclbins << 0.5 << 1.5; //vis PS, fullPS


	total=addPlot(inclbins,inclbins,"total","bin","N_{evt}");
	total->setBinByBin(true); //independent bins
	total->addTag(taggedObject::dontDivByBW_tag);


	vistotal=addPlot(inclbins,inclbins,"vis total","bin","N_{evt}");
	vistotal->setBinByBin(true); //independent bins
	vistotal->addTag(taggedObject::dontDivByBW_tag);
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


}

void analysisPlotsMlbMt::fillPlotsReco(){
	if(!use()) return;
	if(!event()) return;
	using namespace std;
	using namespace ztop;

	total->fillReco(1,puweight());

	vistotal->fillReco(1,puweight());


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
		}
	}
	if(event()->leadinglep)
		leadleppt->fillReco(event()->leadinglep->pt(),puweight());

	if(event()->mll)
		mll->fillReco(*event()->mll,puweight());

}

}//ns
