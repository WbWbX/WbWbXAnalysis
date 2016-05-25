/*
 * flatJetWrapper.cc
 *
 *  Created on: 23 May 2016
 *      Author: kiesej
 */


#include "../interface/flatJetWrapper.h"
#include <stdexcept>

namespace ztop{

flatJetWrapper::flatJetWrapper(tTreeHandler * t,
		const TString& pt_branch,
		const TString& eta_branch,
		const TString& phi_branch,
		const TString& m_branch,
		const TString& btag_branch,
		const TString& id_branch,
		const TString& genpt_branch,
		const TString& geneta_branch,
		const TString& genphi_branch,
		const TString& genm_branch){

	addBranch<float>(t,pt_branch);
	addBranch<float>(t,eta_branch);
	addBranch<float>(t,phi_branch);
	addBranch<float>(t,m_branch);


}

NTJet * flatJetWrapper::content(){
	if(isNewEntry()){
		content_.setP4(NTLorentzVector<float>(
				*getBranchContent<float>(0),
				*getBranchContent<float>(1),
				*getBranchContent<float>(2),
				*getBranchContent<float>(3)
				));
	}
	return &content_;
}

}//ns
