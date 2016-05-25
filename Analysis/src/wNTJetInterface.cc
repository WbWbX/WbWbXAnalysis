/*
 * wNTJetInterface.cc
 *
 *  Created on: 24 May 2016
 *      Author: kiesej
 */

#include "../interface/wNTJetInterface.h"

namespace ztop{
wNTJetInterface::wNTJetInterface(tTreeHandler * t,
		const TString& pt_branch,
		const TString& eta_branch,
		const TString& phi_branch,
		const TString& m_branch,
		const TString& btag_branch,
		const TString& id_branch): wNTBaseInterface(t){

	addBranch<float>(pt_branch);
	addBranch<float>(eta_branch);
	addBranch<float>(phi_branch);
	addBranch<float>(m_branch);
	addBranch<float>(btag_branch);
	addBranch<int>  (id_branch);


}

NTJet * wNTJetInterface::content(){
	if(isNewEntry()){
		content_.p4_.setPt( * getBranchContent<float>(0));
		content_.p4_.setEta(* getBranchContent<float>(1));
		content_.p4_.setPhi(* getBranchContent<float>(2));
		content_.p4_.setM(  * getBranchContent<float>(3));
		content_.setBtag(   * getBranchContent<float>(4));
		if(* getBranchContent<int>(5) == 1)
			content_.setId(true);
		else
			content_.setId(false);

	}
	return &content_;
}


wNTJetInterface::~wNTJetInterface(){

}

}

