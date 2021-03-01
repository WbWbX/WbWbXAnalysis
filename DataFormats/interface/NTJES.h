#ifndef NTJES_H
#define NTJES_H

#include "WbWbXAnalysis/Configuration/interface/version.h"
#ifndef CMSSW_LEQ_5
#include "TopAnalysis/ZTopUtils/interface/JESBase.h"
#else

class JESBase{
public:

	void setFile(std::string pathToFile){}
	void setSystematics(std::string syst){}
	void configureFactorizedJetCorrector(const TString* filePathL1, const TString* filePathL2, const TString* filePathL3, const TString* filePathL2L3, const bool& isMC){}
	void setIs2012(bool is){}
	std::vector<std::string> getSourceNames(){return std::vector<std::string>();}
	void restrictToFlavour(int flav){}
	void clearRestrictToFlavour(){}
	void applyUncertainties(float a,float b,float c,float d, int f){}
	float correctionForUncorrectedJet(float a,float b,float c,float d){return 0.;}
	void setSource(const std::string& str){}
};

#endif
#include "WbWbXAnalysis/DataFormats/interface/NTJet.h"

#include "NTLorentzVector.h"
#include <iostream>

namespace ztop{

class NTJES{

public:
	NTJES(){}
	~NTJES(){}

	void setFile(std::string pathToFile){JesBase_.setFile(pathToFile);}
	void setSystematics(std::string syst){JesBase_.setSystematics(syst);} //! up, down, no
	void setIs2012(bool is){JesBase_.setIs2012(is);}
        
        void setFilesCorrection (const TString* filePathL1, const TString* filePathL2, const TString* filePathL3, const TString* filePathL2L3, const bool& isMC) {
	        JesBase_.configureFactorizedJetCorrector( filePathL1, filePathL2, filePathL3, filePathL2L3, isMC);
        }
	/**
	 * Adds a source with name to the sources to be varied
	 */
	void setSource(const std::string& str){JesBase_.setSource(str);}

	std::vector<std::string> getSourceNames(){return JesBase_.getSourceNames();}

	/**
	 * Applies uncertainties only if they match the flavour specified here
	 * This function ADDS and entry! to clear, use clearRestrictToFlavour()
	 */
	void restrictToFlavour(int flav){
		std::cout << "NTJES::restrictToFlavour " <<flav << std::endl;
		JesBase_.restrictToFlavour(flav);
	}

	/**
	 * Clears all flavour restrictions added before
	 */
	void clearRestrictToFlavour(){JesBase_.clearRestrictToFlavour();}


	void applyToJet(ztop::NTJet * jet ){
		float recopt=jet->pt();
		float recoeta=jet->eta();
		float recophi=jet->phi();
		float recom=jet->p4().M();
		JesBase_.applyUncertainties(recopt,recoeta,recophi,recom,jet->genPartonFlavour());
		jet->setP4(ztop::NTLorentzVector<float>(recopt,recoeta,recophi,recom));
	}

        void correctJet(ztop::NTJet * jet,float jetArea, float rho){
                jet->setP4(jet->p4Uncorr());
                float jetInitialPt = jet->pt();
                float jetInitialEta = jet->eta(); 
                float jetCorr = JesBase_.correctionForUncorrectedJet(jetArea, jetInitialEta, jetInitialPt, rho);
		jet->setP4(jet->p4() * jetCorr);
        }
        void correctJets(std::vector<ztop::NTJet *> jets, float rho){
                for(size_t i=0;i<jets.size();i++) correctJet(jets.at(i),jets.at(i)->getMember(0), rho);
        }
	void applyToJets(std::vector<ztop::NTJet *> jets){
		for(size_t i=0;i<jets.size();i++)
			applyToJet(jets.at(i));
	}

protected:
	JESBase JesBase_;


};

}
#endif
