/*
 * flatJetWrapper.h
 *
 *  Created on: 23 May 2016
 *      Author: kiesej
 */

#ifndef TTZANALYSIS_ANALYSIS_INTERFACE_FLATJETWRAPPER_H_
#define TTZANALYSIS_ANALYSIS_INTERFACE_FLATJETWRAPPER_H_

#include "flatBranchesWrapper.h"
#include "TtZAnalysis/DataFormats/interface/NTJet.h"
#include "TString.h"

namespace ztop{
class flatJetWrapper: public flatBranchesWrapperBase{
public:
	flatJetWrapper(tTreeHandler * t,
			const TString& pt_branch="",
			const TString& eta_branch="",
			const TString& phi_branch="",
			const TString& m_branch="",
			const TString& btag_branch="",
			const TString& id_branch="",
			const TString& genpt_branch="",
			const TString& geneta_branch="",
			const TString& genphi_branch="",
			const TString& genm_branch="");

	NTJet * content();

private:
	flatJetWrapper(){}
	NTJet content_;

};

}



#endif /* TTZANALYSIS_ANALYSIS_INTERFACE_FLATJETWRAPPER_H_ */
