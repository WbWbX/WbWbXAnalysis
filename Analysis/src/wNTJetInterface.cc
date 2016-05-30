/*
 * wNTJetInterface.cc
 *
 *  Created on: 24 May 2016
 *      Author: kiesej
 */

#include "../interface/wNTJetInterface.h"

namespace ztop{
wNTJetInterface::wNTJetInterface(tTreeHandler * t,
		const TString& size_branch,
		const TString& pt_branch,
		const TString& eta_branch,
		const TString& phi_branch,
		const TString& m_branch,
		const TString& btag_branch,
		const TString& id_branch): wNTBaseInterface(t){

	/*
	 * order defines the order the branches can be read later!
	 */
	addBranch<int>   (size_branch);
	addBranch<float*>(pt_branch,64);
	addBranch<float*>(eta_branch,64);
	addBranch<float*>(phi_branch,64);
	addBranch<float*>(m_branch,64);
	addBranch<float*>(btag_branch,64);
	addBranch<int*>  (id_branch,64);


}
/**
 * descr
 *
 * for performance reasons, wNTJetInterface is friend class of NTJet
 * allowing to set members directly
 */
std::vector<NTJet> * wNTJetInterface::content(){
	if(isNewEntry()){
		size_t njet=*getBranchContent<int>(0);
		if(njet>getBranchBuffer(1))njet=getBranchBuffer(1);
		content_.resize(njet);
		for(size_t i=0;i<njet;i++){
			NTJet & jet=content_.at(i);

			jet.p4_.setPt( (* getBranchContent<float*>(1))[i]);
			jet.p4_.setEta((* getBranchContent<float*>(2))[i]);
			jet.p4_.setPhi((* getBranchContent<float*>(3))[i]);
			jet.p4_.setM(  (* getBranchContent<float*>(4))[i]);
			jet.btag_ =    (* getBranchContent<float*>(5))[i];
			jet.id_   =  1 ==  (*getBranchContent<int*>(6))[i];

		}
	}
	return &content_;
}


wNTJetInterface::~wNTJetInterface(){

}

}

