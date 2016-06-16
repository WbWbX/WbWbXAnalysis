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
#include "../interface/wNTGenJetInterface.h"
#include "../interface/wNTGenParticleInterface.h"

#include "../interface/wControlPlots.h"
#include "../interface/wControlplots_gen.h"
#include "../interface/analysisPlotsW.h"
#include "TtZAnalysis/DataFormats/interface/helpers.h"
#include "TtZAnalysis/DataFormats/interface/NTEvent.h"

#include "../interface/wReweightingPlots.h"

#include "../interface/HTransformToCS.h"
#include "../interface/wGenSelector.h"
#include "../interface/wNTGoodEventInterface.h"

//small helper



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



	tTreeHandler t;
	if(inputfile_.Contains("treeProducerSusySoftlepton") || inputfile_.Contains("SingleMu"))
		t.load( datasetdirectory_+inputfile_ ,"treeProducerSusySoftlepton");
	else
		t.load( datasetdirectory_+inputfile_ ,"treeProducerA7W");



	createNormalizationInfo(&t);

	const bool isNLO=  signal_ && inputfile_.Contains("new/");
	//	const bool isInclusiveW=inputfile_.Contains("Wjets.root");

	wGenSelector selectGen;
	if(isNLO){
		if(0 && inputfile_.Contains("LN_")){ //FIXME
			selectGen.addVeto(15);
			selectGen.addVeto(-15);
		}
	}


	NTFullEvent evt;
	wControlPlots c_plots;
	c_plots.linkEvent(evt);
	c_plots.initSteps(6);

	wControlplots_gen c_plots_gen;
	c_plots_gen.linkEvent(evt);

	analysisPlotsW anaplots(5);
	anaplots.setEvent(evt);
	anaplots.enable(false); //FIXME not used right now
	anaplots.bookPlots();
	histo1DUnfold::setAllListedMC(isMC_);
	histo1DUnfold::setAllListedLumi((float)lumi_);

	wReweightingPlots rewplots;
	rewplots.enable(true);
	rewplots.setSignal(signal_);
	rewplots.setEvent(&evt);
	rewplots.bookPlots();


	wNTJetInterface b_Jets
	(&t, "nJet", "Jet_pt", "Jet_eta", "Jet_phi","Jet_mass","Jet_btagCSV","Jet_puId");

	wNTLeptonsInterface b_goodLeptons=wNTLeptonsInterface
			(&t,"nLepGood","LepGood_pdgId","LepGood_pt","LepGood_eta","LepGood_phi","LepGood_mass",
					"LepGood_charge","LepGood_tightId","LepGood_relIso04","LepGood_dxy","LepGood_dz");

	wNTLeptonsInterface b_otherLeptons=wNTLeptonsInterface
			(&t,"nLepOther","LepOther_pdgId","LepOther_pt","LepOther_eta","LepOther_phi","LepOther_mass",
					"LepOther_charge","LepOther_tightId","LepOther_relIso04","LepOther_dxy","LepOther_dz");


	wNTGenJetInterface b_genjets(&t,signal_, "nGenJet", "GenJet_pt", "GenJet_eta", "GenJet_phi", "GenJet_mass");
	wNTGenParticleInterface b_genparticles(&t,signal_, "nGenP6StatusThree", "GenP6StatusThree_pt", "GenP6StatusThree_eta", "GenP6StatusThree_phi",
			"GenP6StatusThree_mass","GenP6StatusThree_pdgId","GenP6StatusThree_charge","GenP6StatusThree_status", "GenP6StatusThree_motherId",
			"GenP6StatusThree_grandmaId");


	wNTGoodEventInterface b_goodevent(&t,isMC_);

	wReweighterInterface mcweights=wReweighterInterface(&t,signal_&& isNLO,"ntheoryWeightsValue","theoryWeightsValue_weight");
	if(signal_)
		for(size_t i=0;i<weightindicies_.size();i++)
			mcweights.addWeightIndex(weightindicies_.at(i));
	simpleReweighter lheReweighter;

	if(!isMC_)
		tBranchHandler<float>::allow_missing=true;
	tBranchHandler<float> b_puweight(&t, "puWeight");
	tBranchHandler<float> b_lep1weight(&t, "LepEff_1lep");
	tBranchHandler<float> b_lep2weight(&t, "LepEff_2lep");
	tBranchHandler<float>::allow_missing=false;

	tBranchHandler<int>::allow_missing=!(signal_ && isNLO);
	tBranchHandler<int> b_lheweight(&t, "LHE_originalWeight");
	tBranchHandler<int>::allow_missing=false;

	tBranchHandler<int> b_trigger(&t, "HLT_SingleMu");
	tBranchHandler<int> b_vertices(&t, "nVert");




	Long64_t entries=t.entries();


	//////individual histos
	histo1D::c_makelist=true;
	histo1D h(histo1D::createBinning(6,-.5,6.5),"Steps","","");
	histo1D::c_makelist=false;

	histo2D::c_makelist=true;
	histo2D ptttransA7corr (histo1D::createBinning(100,-1.1,1.1), histo1D::createBinning(100,-1.1,1.1),
			"ptttransA7corr gen", "2 p_{T}^{#perp}/ M_{W} (gen)", "sin(#phi*)sin(#theta*)","Events");
	histo2D ptttransA7corrrec (histo1D::createBinning(100,-1.1,1.1), histo1D::createBinning(100,-1.1,1.1),
			"ptttransA7corr reco", "2 p_{T}^{#perp}/ M_{W} (reco)", "2 p_{T}^{#perp}/ M_{W} (gen)","Events");
	histo2D ptttransA7corrgenrec (histo1D::createBinning(100,-1.1,1.1), histo1D::createBinning(100,-1.1,1.1),
			"ptttransA7corr genreco", "2 p_{T}^{#perp}/ M_{W} (reco)", "sin(#phi*)sin(#theta*)","Events");
	histo2D::c_makelist=false;

	HTransformToCS cs_transformer(8000);

	if(testmode_)
		entries/=20;//skip=(float) 1./0.05; //only consider 5 % of the events
	/*else
		entries/=2;
	std::cout << "also full mode is restricted to 1/2 of the events because of problem with RunC sample"<<std::endl;
	 */
	//t.setPreCache();//better performance for large samples

	///////////////////////////////// Event loop //////////////////////////////

	for(Long64_t event=0;event<entries;event++){
		reportStatus(event,entries);

		/*
		 * Init event
		 */
		size_t step=0;
		evt.reset();
		t.setEntry(event);
		histo1DUnfold::flushAllListed();


		/*
		 * Link collections to event
		 */
		float puweight=1;
		if(isMC_)puweight*=* b_puweight.content();
		evt.puweight=&puweight;
		if(signal_ && isNLO){
			float lheweight=(float) *b_lheweight.content() / (10000. * 4.3446); //only sign is important scale to make the weight plots look nice
			lheReweighter.setNewWeight(lheweight);
			lheReweighter.reWeight(puweight);
		}
		mcweights.reWeight(puweight);
		NTEvent ntevt;
		evt.event=&ntevt;
		/*
		 * Generator part
		 */
		std::vector<NTGenJet *> genjets;
		genjets=produceCollection(b_genjets.content() );
		evt.genjets=&genjets;
		std::vector<NTGenParticle*> allgenparticles;
		allgenparticles=produceCollection(b_genparticles.content());
		evt.allgenparticles=&allgenparticles;

		////first gen selection step (problem with tau decays)
		selectGen.setCollection(&allgenparticles);
		if(!selectGen.pass()) continue;

		NTGenParticle * lepton=0, *neutrino=0, Wbos;
		float phi_cs=0,costheta_cs=0,pttrans_gen=0,A7=0;
		evt.genlepton=lepton;
		evt.genneutrino=neutrino;

		for(size_t i=0;i<allgenparticles.size();i++){
			NTGenParticle * p = allgenparticles.at(i);
			if(p->status() != 23 && p->status() != -23) continue; //only hard interaction. Also if it decays later

			if(p->pdgId() == 13 || p->pdgId() == -13){ //should we also use taus and elecs here?!->more stat
				if(p->pt()>25 && fabs(p->eta())<2.1){
					lepton=p;

					//remove from genjets
					for(size_t j=0;j<genjets.size();j++){
						if(dR(genjets.at(j),lepton)<0.0001) //FIXME see if necessary
							genjets.erase(genjets.begin()+j);
					}
				}
			}
			else if(p->pdgId() == 14 || p->pdgId() == -14)
				neutrino=p;
		}
		if(lepton && neutrino){//construct W
			Wbos.setP4( lepton->p4() + neutrino->p4() );
			Wbos.setQ(lepton->q());
			evt.genW=&Wbos;
			cs_transformer.TransformToCS(neutrino,lepton);
			phi_cs=cs_transformer.GetPhi();
			costheta_cs=cs_transformer.GetCosTheta();
			evt.phi_cs=&phi_cs;
			evt.costheta_cs=&costheta_cs;
			pttrans_gen=analysisPlotsW::asymmValue(lepton ,&Wbos,false); //dont invert W direction
			evt.pttrans_gen=&pttrans_gen;
			evt.A7=&A7;
			A7=std::sin(phi_cs)*std::sqrt(1-costheta_cs*costheta_cs);
			if(genjets.size() && genjets.at(0)->pt()>25 && fabs(genjets.at(0)->eta())<2.5)
				ptttransA7corr.fill(pttrans_gen ,A7 ,puweight);
		}


		c_plots_gen.makeControlPlots(0);
		rewplots.fillPlots();
		//(at the end)
		anaplots.fillPlotsGen();



		if(! b_goodevent.pass())continue;


		//only event requirements: step 0
		h.fill(step,puweight);
		c_plots.makeControlPlots(step++);


		//trigger requirement: step 1
		if(*b_trigger.content()<1) continue;

		ntevt.setVertexMulti((float) (* b_vertices.content()) );

		NTMuon leadinglep;
		evt.leadinglep=&leadinglep;


		h.fill(step,puweight);
		c_plots.makeControlPlots(step++);


		std::vector<NTMuon*>     allgoodmuons=produceCollection<NTMuon>    (b_goodLeptons.mu_content());
		std::vector<NTElectron*> allgoodelecs=produceCollection<NTElectron>(b_goodLeptons.e_content());
		evt.allmuons=&allgoodmuons;
		evt.allelectrons=&allgoodelecs;

		std::vector<NTMuon*> kinmuons,idmuons,isomuons,vetomuons, nonisomuons;
		evt.kinmuons=&kinmuons;
		evt.idmuons=&idmuons;
		evt.isomuons=&isomuons;
		evt.vetomuons=&vetomuons;
		evt.nonisomuons=&nonisomuons;


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
			if(muon->d0V() > 0.2) continue;
			//if(muon->dzV() > 00) continue;
			idmuons << muon;

			if(muon->isoVal() < 0.12) { ///INVERT SWITCH
				isomuons << muon;
			}
			else if(muon->isoVal() < 0.5){
				nonisomuons << muon;
			}
		}


		//at least one id muon: step 2
		if(idmuons.size()<1) continue;
		if(isMC_) puweight*= *b_lep1weight.content();
		h.fill(step,puweight);
		c_plots.makeControlPlots(step++);

		std::vector<NTMuon*>     allothermuons=produceCollection<NTMuon>   (b_otherLeptons.mu_content());
		for(size_t i=0;i<allothermuons.size();i++){
			NTMuon* muon=allothermuons.at(i);
			if(muon->pt()<15) continue;
			if(fabs(muon->eta())>2.4)continue;
			vetomuons << muon;
		}



		//veto step: step 3
		//the veto muons include one iso/noniso
		if(vetomuons.size()>1)continue;
		if(isomuons.size()+nonisomuons.size()>1) continue; //should be obsolete
		//now there is exactly one muon
		bool isiso=true;
		if(nonisomuons.size()>0){
			isiso=false;
			leadinglep=*nonisomuons.at(0);
		}
		else{
			leadinglep=*isomuons.at(0);
		}


		std::vector<NTJet*> idjets, hardjets;
		evt.idjets=&idjets;
		evt.hardjets=&hardjets;


		/*
		 * Jet selection
		 */
		for(size_t i=0;i<b_Jets.content()->size();i++){
			NTJet* jet=&b_Jets.content()->at(i);

			if(isMC_){
				getJecUnc()->applyToJet(jet);
			}
			if(! jet->id()) continue;
			idjets << jet;


			if(jet->pt()<25)continue;
			if(fabs(jet->eta())>2.4)continue;
			hardjets << jet;

		}
		if(isiso){
			h.fill(step,puweight);
			c_plots.makeControlPlots(step);
		}
		step++;

		//at least one jet: step 4
		if(hardjets.size()<1) continue;
		evt.leadingjet=hardjets.at(0);

		float pttrans=analysisPlotsW::asymmValue(&leadinglep,hardjets.at(0));
		evt.pttrans=&pttrans;
		if(isiso && evt.genW){
			ptttransA7corrrec.fill(pttrans, pttrans_gen, puweight );
			ptttransA7corrgenrec.fill(pttrans, A7, puweight );
		}
		if(isiso){
			h.fill(step,puweight);
			c_plots.makeControlPlots(step);
		}
		step++;


		//add control plots for iso muon and noniso muon areas
		if(isomuons.size()>0){
			anaplots.setIsIso(true);
			//step 5
			h.fill(step,puweight);
			c_plots.makeControlPlots(step);
		}
		else if(nonisomuons.size()>0){
			anaplots.setIsIso(false);
			step++;
			//step 6
			h.fill(step,puweight);
			c_plots.makeControlPlots(step);
		}
		else{
			continue;
		}

		//the class is protected internally against the latter case
		anaplots.fillPlotsReco();

	}

	////////////////////// Event loop end ///////////////////



	/*
	 * get norm right
	 */


	std::cout << inputfile_<< ": before renorm " << norm_ << std::endl;

	norm_*=lheReweighter.getRenormalization();
	norm_*=mcweights.getRenormalization();

	std::cout <<  inputfile_<< ": end norm " << norm_ << std::endl;

	//	std::cout << "end norm for " << inputfile_ << ": "<< norm_<<std::endl;
	processEndFunction();
	//	tBranchHandlerBase::debug=true;
	//	tTreeHandler::debug=true;

}


void wAnalyzer::createNormalizationInfo(tTreeHandler* t){

	if(!isMC_){ norm_=1; return;}

	size_t idx=ownChildIndex();
	double xsec=norms_.at(idx);
	double entries=t->entries();

	norm_= lumi_ * xsec / entries;
}

}//ns


