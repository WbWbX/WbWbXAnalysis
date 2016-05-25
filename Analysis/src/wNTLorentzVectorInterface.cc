/*
 * wNTLorentzVectorInterface.cc
 *
 *  Created on: 25 May 2016
 *      Author: kiesej
 */





#include "../interface/wNTLorentzVectorInterface.h"

namespace ztop{
wNTLorentzVectorInterface::wNTLorentzVectorInterface(tTreeHandler * t,
		const TString& pt_branch,
		const TString& eta_branch,
		const TString& phi_branch,
		const TString& m_branch): wNTBaseInterface(t){

	addBranch<float>(pt_branch);
	addBranch<float>(eta_branch);
	addBranch<float>(phi_branch);
	addBranch<float>(m_branch);

}

wNTLorentzVectorInterface::~wNTLorentzVectorInterface(){
	//taken care of by base class
}

NTLorentzVector<float> * wNTLorentzVectorInterface::content(){
	if(isNewEntry()){
		content_.setPt (* getBranchContent<float>(0));
		content_.setEta(* getBranchContent<float>(1));
		content_.setPhi(* getBranchContent<float>(2));
		content_.setM  (* getBranchContent<float>(3));
	}
	return &content_;
}

}

