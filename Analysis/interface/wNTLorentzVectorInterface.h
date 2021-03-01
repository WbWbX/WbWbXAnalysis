/*
 * wNTLorentzVectorInterface.h
 *
 *  Created on: 25 May 2016
 *      Author: kiesej
 */

#ifndef TTZANALYSIS_ANALYSIS_INTERFACE_WNTLORENTZVECTORINTERFACE_H_
#define TTZANALYSIS_ANALYSIS_INTERFACE_WNTLORENTZVECTORINTERFACE_H_

#include "WbWbXAnalysis/Analysis/interface/wNTBaseInterface.h"
#include "WbWbXAnalysis/DataFormats/interface/NTLorentzVector.h"
namespace ztop{
class tTreeHandler;


class wNTLorentzVectorInterface : public wNTBaseInterface{
public:
	wNTLorentzVectorInterface(tTreeHandler * t,
			const TString& pt_branch="",
			const TString& eta_branch="",
			const TString& phi_branch="",
			const TString& m_branch="");
	~wNTLorentzVectorInterface();

	NTLorentzVector<float> * content();


private:
	wNTLorentzVectorInterface(){}



	NTLorentzVector<float> content_;


};


}
#endif /* TTZANALYSIS_ANALYSIS_INTERFACE_WNTLORENTZVECTORINTERFACE_H_ */
