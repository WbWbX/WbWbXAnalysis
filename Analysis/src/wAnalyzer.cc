/*
 * wAnalyzer.cc
 *
 *  Created on: 23 May 2016
 *      Author: kiesej
 */

#include "../interface/wAnalyzer.h"
#include "../interface/tTreeHandler.h"
#include "../interface/tBranchHandler.h"
#include "../interface/simpleReweighter.h"

#include "../interface/wNTJetInterface.h"
#include "../interface/wNTLeptonsInterface.h"
#include "../interface/NTFullEvent.h"
#include "../interface/wReweighterInterface.h"

#include "../interface/wControlPlots.h"
#include "../interface/analysisPlotsW.h"
#include "TtZAnalysis/DataFormats/interface/helpers.h"

namespace ztop{

fileForker::fileforker_status  wAnalyzer::start(){
	allplotsstackvector_.setName(getOutFileName());
	allplotsstackvector_.setSyst(getSyst());
	setOutputFileName((getOutPath()+".ztop").Data());//add extension
	return runParallels(5);
}


fileForker::fileforker_status wAnalyzer::writeOutput(){
	return writeHistos();
}


void wAnalyzer::analyze(size_t id){



	tTreeHandler t( datasetdirectory_+inputfile_ ,"treeProducerA7W");
	createNormalizationInfo(&t);



	NTFullEvent evt;
	wControlPlots c_plots;
	c_plots.linkEvent(evt);
	analysisPlotsW anaplots(4);
	anaplots.setEvent(evt);
	anaplots.enable();
	anaplots.bookPlots();
	histo1DUnfold::setAllListedMC(isMC_);
	histo1DUnfold::setAllListedLumi((float)lumi_);



	wNTJetInterface b_Jets
	(&t, "nJet", "Jet_pt", "Jet_eta", "Jet_phi","Jet_mass","Jet_btagCSV","Jet_id");

	wNTLeptonsInterface b_goodLeptons=wNTLeptonsInterface
			(&t,"nLepGood","LepGood_pdgId","LepGood_pt","LepGood_eta","LepGood_phi","LepGood_mass",
					"LepGood_charge","LepGood_tightId","LepGood_relIso04");


	wReweighterInterface mcweights=wReweighterInterface(&t,"ntheoryWeightsValue","theoryWeightsValue_weight");
	if(signal_)
		for(size_t i=0;i<weightindicies_.size();i++)
			mcweights.addWeightIndex(weightindicies_.at(i));

	if(!isMC_)
		mcweights.enable(false);
	tBranchHandler<float> b_puweight(&t, "puWeight");
	tBranchHandler<float> b_lep1weight(&t, "LepEff_1lep");
	tBranchHandler<float> b_lep2weight(&t, "LepEff_2lep");

	tBranchHandler<int> b_trigger(&t, "HLT_SingleMu");





	Long64_t entries=t.entries();
	if(testmode_)
		entries/=10;

	for(Long64_t event=0;event<entries;event++){
		/*
		 * Init event
		 */
		size_t step=0;
		evt.reset();
		t.setEntry(event);
		reportStatus(event,entries);
		histo1DUnfold::flushAllListed();


		/*
		 * Link collections to event
		 */
		float puweight=1;
		if(isMC_)puweight*=* b_puweight.content();
		evt.puweight=&puweight;
		mcweights.reWeight(puweight);

		std::vector<NTMuon*>     allgoodmuons=produceCollection<NTMuon>    (b_goodLeptons.mu_content());
		std::vector<NTElectron*> allgoodelecs=produceCollection<NTElectron>(b_goodLeptons.e_content());
		evt.allmuons=&allgoodmuons;
		evt.allelectrons=&allgoodelecs;

		std::vector<NTMuon*> kinmuons,idmuons,isomuons,vetomuons;
		evt.kinmuons=&kinmuons;
		evt.idmuons=&idmuons;
		evt.isomuons=&isomuons;
		evt.vetomuons=&vetomuons;

		std::vector<NTJet*> idjets, hardjets;
		evt.idjets=&idjets;
		evt.hardjets=&hardjets;
		/*
		 * Generator part
		 */

		//(at the end)
		anaplots.fillPlotsGen();

		/*
		 * Lepton selection
		 */
		for(size_t i=0;i<allgoodmuons.size();i++){
			NTMuon* muon=allgoodmuons.at(i);
			if(muon->pt()<15) continue;
			if(fabs(muon->eta())>2.4)continue;
			vetomuons << muon;
			if(muon->pt()<25) continue;
			if(fabs(muon->eta())>2.1) continue;
			kinmuons << muon;

			if(! muon->isTightID()) continue;
			idmuons << muon;

			if(muon->isoVal() > 0.12) continue; ///INVERT SWITCH
			isomuons << muon;

		}

		c_plots.makeControlPlots(step++);

		//trigger requirement: step 1
		if(*b_trigger.content()<1) continue;
		c_plots.makeControlPlots(step++);

		//at least one iso muon: step 2
		if(isomuons.size()<1) continue;
		if(isMC_) puweight*= *b_lep1weight.content();
		c_plots.makeControlPlots(step++);

		//veto step: step 3
		if(vetomuons.size()>1)continue; //one is the iso muon

		/*
		 * Jet selection
		 */
		for(size_t i=0;i<b_Jets.content()->size();i++){
			NTJet* jet=&b_Jets.content()->at(i);
			//if(! jet->id()) continue;
			idjets << jet;

			if(jet->pt()<25)continue;
			if(fabs(jet->eta())>2.4)continue;
			hardjets << jet;

		}

		c_plots.makeControlPlots(step++);

		//at least one jet: step 4
		if(hardjets.size()<1) continue;
		c_plots.makeControlPlots(step++);

		// final step. create the observables
		anaplots.fillPlotsReco();



	}
	////////////////////// Event loop end ///////////////////
	histo1DUnfold::flushAllListed();


	/*
	 * get norm right
	 */

	norm_*=mcweights.getRenormalization();


	processEndFunction();
}


void wAnalyzer::createNormalizationInfo(tTreeHandler* t){

	if(!isMC_){ norm_=1; return;}

	size_t idx=ownChildIndex();
	double xsec=norms_.at(idx);
	double entries=t->entries();
	norm_= lumi_ * xsec / entries;

}

}//ns


