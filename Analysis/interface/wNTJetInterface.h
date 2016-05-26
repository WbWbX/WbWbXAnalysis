/*
 * wNTJetInterface.h
 *
 *  Created on: 24 May 2016
 *      Author: kiesej
 */

#ifndef TTZANALYSIS_FLATNTUPLEINTERFACE_INTERFACE_WNTJETINTERFACE_H_
#define TTZANALYSIS_FLATNTUPLEINTERFACE_INTERFACE_WNTJETINTERFACE_H_

#include "TtZAnalysis/Analysis/interface/wNTBaseInterface.h"
#include "TtZAnalysis/DataFormats/interface/NTJet.h"
#include <vector>

namespace ztop{
class tTreeHandler;

class wNTJetInterface: public wNTBaseInterface{
public:
	wNTJetInterface(tTreeHandler * t,
			const TString& size_branch="",
			const TString& pt_branch="",
			const TString& eta_branch="",
			const TString& phi_branch="",
			const TString& m_branch="",
			const TString& btag_branch="",
			const TString& id_branch="");
	~wNTJetInterface();

	std::vector<NTJet> * content();


private:
	wNTJetInterface(){}


	std::vector<NTJet> content_;


};



}
#endif /* TTZANALYSIS_FLATNTUPLEINTERFACE_INTERFACE_WNTJETINTERFACE_H_ */
