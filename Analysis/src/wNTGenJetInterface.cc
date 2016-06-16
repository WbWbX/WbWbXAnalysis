/*
 * wNTGenJetInterface.cc
 *
 *  Created on: 31 May 2016
 *      Author: jkiesele
 */

#include "../interface/wNTGenJetInterface.h"

namespace ztop{
wNTGenJetInterface::wNTGenJetInterface(tTreeHandler * t,
		bool enable,
		const TString& size_branch,
		const TString& pt_branch,
		const TString& eta_branch,
		const TString& phi_branch,
		const TString& m_branch): wNTBaseInterface(t,enable){

	/*
	 * order defines the order the branches can be read later!
	 */

	addBranch<int>   (size_branch);
	addBranch<float*>(pt_branch,64);
	addBranch<float*>(eta_branch,64);
	addBranch<float*>(phi_branch,64);
	addBranch<float*>(m_branch,64);


}
/**
 * descr
 *
 * for performance reasons, wNTGenJetInterface is friend class of NTGenJet
 * allowing to set members directly
 */
std::vector<NTGenJet> * wNTGenJetInterface::content(){

	if(isNewEntry()){
		size_t njet=*getBranchContent<int>(0);
		if(njet>getBranchBuffer(1))njet=getBranchBuffer(1);
		content_.resize(njet);
		for(size_t i=0;i<njet;i++){
			NTGenJet & jet=content_.at(i);

			jet.p4_.setPt( (* getBranchContent<float*>(1))[i]);
			jet.p4_.setEta((* getBranchContent<float*>(2))[i]);
			jet.p4_.setPhi((* getBranchContent<float*>(3))[i]);
			jet.p4_.setM(  (* getBranchContent<float*>(4))[i]);

		}
	}
	return &content_;
}


wNTGenJetInterface::~wNTGenJetInterface(){

}

}

