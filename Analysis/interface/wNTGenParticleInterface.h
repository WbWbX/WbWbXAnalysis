/*
 * wNTGenParticleInterface.h
 *
 *  Created on: 31 May 2016
 *      Author: jkiesele
 */

#ifndef TTZANALYSIS_ANALYSIS_INTERFACE_WNTGENPARTICLEINTERFACE_H_
#define TTZANALYSIS_ANALYSIS_INTERFACE_WNTGENPARTICLEINTERFACE_H_


#include "TtZAnalysis/DataFormats/interface/NTGenParticle.h"
#include "wNTBaseInterface.h"

namespace ztop{

class wNTGenParticleInterface: public wNTBaseInterface{
public:
	wNTGenParticleInterface(tTreeHandler * t,
			const TString& size_branch,
			const TString& pt_branch,
			const TString& eta_branch,
			const TString& phi_branch,
			const TString& m_branch,
			const TString& pdgid_branch,
			const TString& charge_branch,
			const TString& status_branch,
			const TString& motherpdgid_branch,
			const TString& grandmotherpdgid_branch);
	~wNTGenParticleInterface();

	std::vector<NTGenParticle> * content();


private:
	wNTGenParticleInterface(){}


	std::vector<NTGenParticle> content_;


};

}

#endif /* TTZANALYSIS_ANALYSIS_INTERFACE_WNTGENPARTICLEINTERFACE_H_ */
