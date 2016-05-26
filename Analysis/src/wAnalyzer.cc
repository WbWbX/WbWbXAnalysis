/*
 * wAnalyzer.cc
 *
 *  Created on: 23 May 2016
 *      Author: kiesej
 */

#include "../interface/wAnalyzer.h"
#include "../interface/tTreeHandler.h"
#include "../interface/tBranchHandler.h"
#include "TtZAnalysis/Tools/interface/histo1D.h"
#include "../interface/wNTJetInterface.h"
#include "../interface/NTFullEvent.h"

namespace ztop{

fileForker::fileforker_status  wAnalyzer::start(){
	allplotsstackvector_.setName(getOutFileName());
	allplotsstackvector_.setSyst(getSyst());
	setOutputFileName((getOutFileName()+".ztop").Data());//add extension
	return runParallels(5);
}


fileForker::fileforker_status wAnalyzer::writeOutput(){
	return writeHistos();
}


void wAnalyzer::analyze(size_t id){

	/*
	 * modify norm_ etc if ne
	 */


	//tTreeHandler::debug=true;
	tTreeHandler *t=new tTreeHandler ( datasetdirectory_+inputfile_ ,"treeProducerA7W");
	createNormalizationInfo(t);

	//tTreeHandler *t=new tTreeHandler ( "/afs/desy.de/user/k/kiesej/wwtoall.root","writeNTuple/NTuple");

	histo1D::c_makelist=true;
	histo1D h(histo1D::createBinning(20,0,300),"jet pt","p_{t}","Events");
	histo1D h2(histo1D::createBinning(20,-3,3),"jet eta","eta","Events");

	NTFullEvent evt;


	wNTJetInterface b_Jets
			(t, "nJet", "Jet_pt", "Jet_eta", "Jet_phi","Jet_mass","Jet_btagCSV","Jet_id");



	tBranchHandler<float> b_puweight(t, "puWeight");

	Long64_t entries=t->entries();
	if(testmode_)
		entries/=100;
	//t->setPreCache();

	for(Long64_t event=0;event<entries;event++){
		t->setEntry(event);
		reportStatus(event,entries);
		float puweight=* b_puweight.content();
		puweight*=1;





		for(size_t i=0;i<b_Jets.content()->size();i++){
			NTJet * jet=&b_Jets.content()->at(i);

			h.fill(jet->pt() ,puweight);
			h2.fill(jet->eta(),puweight);
		}
	}
	delete t;
	std::cout << "done loop" <<std::endl;
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


