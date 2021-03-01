/*
 * wNTGenJetInterface.h
 *
 *  Created on: 31 May 2016
 *      Author: jkiesele
 */

#ifndef TTZANALYSIS_ANALYSIS_INTERFACE_WNTGENJETINTERFACE_H_
#define TTZANALYSIS_ANALYSIS_INTERFACE_WNTGENJETINTERFACE_H_

#include "WbWbXAnalysis/DataFormats/interface/NTGenJet.h"
#include "wNTBaseInterface.h"

namespace ztop{

class wNTGenJetInterface: public wNTBaseInterface{
public:
	wNTGenJetInterface(tTreeHandler * t,
			bool enabled,
			const TString& size_branch="",
			const TString& pt_branch="",
			const TString& eta_branch="",
			const TString& phi_branch="",
			const TString& m_branch="");
	~wNTGenJetInterface();

	std::vector<NTGenJet> * content();


private:
	wNTGenJetInterface(){}


	std::vector<NTGenJet> content_;


};

}


#endif /* TTZANALYSIS_ANALYSIS_INTERFACE_WNTGENJETINTERFACE_H_ */
