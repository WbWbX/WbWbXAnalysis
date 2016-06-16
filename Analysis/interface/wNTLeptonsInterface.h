/*
 * wNTLeptonsInterface.h
 *
 *  Created on: 30 May 2016
 *      Author: jkiesele
 */

#ifndef TTZANALYSIS_ANALYSIS_INTERFACE_WNTLEPTONSINTERFACE_H_
#define TTZANALYSIS_ANALYSIS_INTERFACE_WNTLEPTONSINTERFACE_H_

#include "wNTBaseInterface.h"
#include "TtZAnalysis/DataFormats/interface/NTMuon.h"
#include "TtZAnalysis/DataFormats/interface/NTElectron.h"

namespace ztop{

class wNTLeptonsInterface: public wNTBaseInterface{
public:
	wNTLeptonsInterface(tTreeHandler * t,
			const TString& size_branch,
			const TString& pdgid_branch,
			const TString& pt_branch,
			const TString& eta_branch,
			const TString& phi_branch,
			const TString& m_branch,
			const TString& charge_branch,
			const TString& tightID_branch,
			const TString& relIso04_branch,
			const TString& dxy_branch,
			const TString& dz_branch);

	std::vector<NTMuon>     * mu_content();
	std::vector<NTElectron> * e_content();
	std::vector<NTLepton*>  * l_content();

private:
	wNTLeptonsInterface(){}

	void readContents();

	std::vector<NTMuon>     mu_content_;
	std::vector<NTElectron> e_content_;
	std::vector<NTLepton*>  l_content_;
};

}//ns

#endif /* TTZANALYSIS_ANALYSIS_INTERFACE_WNTLEPTONSINTERFACE_H_ */
