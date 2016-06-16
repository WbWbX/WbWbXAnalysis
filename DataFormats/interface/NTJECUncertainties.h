#ifndef NTJECUNCERTAINTIES_H
#define NTJECUNCERTAINTIES_H

#include "TopAnalysis/ZTopUtils/interface/JECBase.h"
#include "TtZAnalysis/DataFormats/interface/NTJet.h"

#include "NTLorentzVector.h"
#include <iostream>

namespace ztop{

class NTJECUncertainties{

public:
	NTJECUncertainties():enabled_(false){}
	~NTJECUncertainties(){}

	void setFile(std::string pathToFile){JecBase_.setFile(pathToFile);}
	void setSystematics(std::string syst){
		JecBase_.setSystematics(syst);
	if(syst.length() ==2 && syst.find("no") != std::string::npos)
		enabled_=false;
	else
		enabled_=true;
	} //! up, down, no

	void setIs2012(bool is){JecBase_.setIs2012(is);}//use to distinguish run i and ii
    
	/**
	 * Adds a source with name to the sources to be varied
	 */
	void setSource(const std::string& str){JecBase_.setSource(str);enabled_=true;}

	std::vector<std::string> getSourceNames(){return JecBase_.getSourceNames();}

	/**
	 * Applies uncertainties only if they match the flavour specified here
	 * This function ADDS and entry! to clear, use clearRestrictToFlavour()
	 */
	void restrictToFlavour(int flav){
		std::cout << "NTJECUncertainties::restrictToFlavour " <<flav << std::endl;
		JecBase_.restrictToFlavour(flav);
	}

	/**
	 * Clears all flavour restrictions added before
	 */
	void clearRestrictToFlavour(){JecBase_.clearRestrictToFlavour();}


	void applyToJet(ztop::NTJet * jet ){
		if(!enabled_) return;
		float recopt=jet->pt();
		float recoeta=jet->eta();
		float recophi=jet->phi();
		float recom=jet->p4().M();
		JecBase_.applyJECUncertainties(recopt,recoeta,recophi,recom,jet->genPartonFlavour());
		jet->setP4(ztop::NTLorentzVector<float>(recopt,recoeta,recophi,recom));
	}
	void applyToJets(std::vector<ztop::NTJet *> jets){
		for(size_t i=0;i<jets.size();i++)
			applyToJet(jets.at(i));
	}

protected:
	JECBase JecBase_;
	bool enabled_;

};

}
#endif
