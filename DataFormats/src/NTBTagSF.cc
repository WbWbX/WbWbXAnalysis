#include "../interface/NTBTagSF.h"
#include "TFile.h"
#include "TTree.h"
#include "TtZAnalysis/Configuration/interface/version.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"


namespace ztop{


/**
 * changes the discriminator values of the jets using random technique.
 * After this step do not expect the discriminator distrubtions in data MC to match!
 *  if()
            return;
 */

void NTBTagSF::changeNTJetTags( std::vector<ztop::NTJet *> *jets)const{
	if(!isMC_ || mode_!=randomtagging_mode || makesEff()) return;
	for(size_t i=0;i<jets->size();i++){
		ztop::NTJet * jet=jets->at(i);
		if(std::abs(jet->genPartonFlavour()) == 0) continue;
		bool shouldbetagged=false;
		shouldbetagged= jetIsTagged(jet->pt(),jet->eta(),jet->genPartonFlavour(),jet->btag(),
					    std::abs(int(1.e6 * sin(jet->phi()))));

		// throw std::logic_error(" NTBTagSF::changeNTJetTags wrong type (undefined");

		if(!shouldbetagged){
			if(jet->btag()>getWPDiscrValue())
				jet->setBtag(-0.1);
		}
		else if(jet->btag()<getWPDiscrValue()){
			jet->setBtag(1.1);
		}
	}
}
void NTBTagSF::changeNTJetTags( std::vector<ztop::NTJet *> &jets)const{
	NTBTagSF::changeNTJetTags(&jets);
}





/**
 * recreates file!
 */
void NTBTagSF::writeToFile(const std::string& filename){
	TFile  f(filename.data(),"RECREATE");
	bTagSFBase::writeToTFile(&f);
	f.Close();
}




void NTBTagSF::readFromFile(const std::string& filename){
	if(!fileExists(filename.data())){
		std::string err="NTBTagSF::readFromTFile: "+filename;
		err+=" does not exist";
		throw std::runtime_error(err);
	}
	TFile * f = new TFile(filename.data(),"READ");
	bTagSFBase::readFromTFile(f);
	f->Close();
	delete f;

}

}//namespace
