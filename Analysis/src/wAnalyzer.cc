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

namespace ztop{

fileForker::fileforker_status wAnalyzer::writeOutput(){
	return writeHistos();
}


void wAnalyzer::analyze(size_t id){

	/*
	 * modify norm_ etc if ne
	 */

	datasetdirectory_="/afs/desy.de/user/k/kiesej/xxl-af-cms/recProjects/CMSSW_5_3_27/src/TtZAnalysis/Analysis/test/";

	//tTreeHandler::debug=true;
	tTreeHandler *t=new tTreeHandler ( datasetdirectory_+inputfile_ ,"treeProducerA7W");
	//tTreeHandler *t=new tTreeHandler ( "/afs/desy.de/user/k/kiesej/wwtoall.root","writeNTuple/NTuple");

	histo1D::c_makelist=true;
	histo1D h(histo1D::createBinning(20,0,300),"jet pt","p_{t}","Events");
	histo1D h2(histo1D::createBinning(20,-3,3),"jet eta","eta","Events");

	//tBranchHandlerBase::debug=true;
//	tBranchHandler<int> b_JetEta(t,"vertMulti");
//	tBranchHandler<double> b_JetPt(t,"rho");

	wNTJetInterface b_firstjet
			(t, "Jet_pt", "Jet_eta", "Jet_phi","Jet_mass","Jet_btagCSV","Jet_id");



	std::cout<< "starting loop"<<std::endl;
	for(Long64_t event=0;event<t->entries();event++){
		t->setEntry(event);
		if((event +1)* 100 % t->entries() <100){
			int status=(event+1) * 100 / t->entries();
			reportBusyStatus(status);
		}
		//std::cout << b_firstJet->content()->pt() << std::endl;
		h.fill(b_firstjet.content()->pt() ,1);
		h2.fill(b_firstjet.content()->eta(),1);
	}
	delete t;
	std::cout << "done loop" <<std::endl;
	processEndFunction();
}


}//ns


