#include "TtZAnalysis/Analysis/triggerEfficiencies12/correlationChecker.h"
#include <iostream>



void checkCorrelations11(){

	using namespace ztop;
	using namespace std;
	TString mode="emu";

	vector<TString> allmettriggers, selectedmettriggers;;

	allmettriggers << "HLT_DiJet60_MET45_v7" << "HLT_MET65_HBHENoiseFiltered_v5" << "HLT_MET65_HBHENoiseFiltered_v5" << "HLT_CentralJet80_MET65_v7" << "HLT_MediumIsoPFTau35_Trk20_MET60_v1" << "HLT_DiCentralJet20_BTagIP_MET65_v7" << "HLT_MediumIsoPFTau35_Trk20_MET70_v1" << "HLT_DiCentralJet20_MET80_v5" << "HLT_CentralJet80_MET80_v6" << "HLT_MET100_HBHENoiseFiltered_v6" << "HLT_MET100_HBHENoiseFiltered_v6" << "HLT_DiCentralJet20_MET100_HBHENoiseFiltered_v1" << "HLT_CentralJet80_MET100_v7" << "HLT_MET120_HBHENoiseFiltered_v6" << "HLT_MET120_HBHENoiseFiltered_v6" << "HLT_CentralJet80_MET160_v7" << "HLT_MET200_HBHENoiseFiltered_v6" << "HLT_MET200_HBHENoiseFiltered_v6" << "HLT_L2Mu60_1Hit_MET40_v5" << "HLT_L2Mu60_1Hit_MET60_v5" << "HLT_MET400_v2";

	TString MCdir="/nfs/dust/cms/user/kiesej/trees_Oct14/";

	std::vector<TString> eemcfiles,mumumcfiles,emumcfiles;
	eemcfiles << MCdir+"tree_7TeV_eettbar.root"
			<< MCdir+"tree_7TeV_eettbarviatau.root" ;

	mumumcfiles << MCdir+"tree_7TeV_mumuttbar.root"
			<< MCdir+"tree_7TeV_mumuttbarviatau.root" ;

	emumcfiles << MCdir+"tree_7TeV_emuttbar.root"
			<< MCdir+"tree_7TeV_emuttbarviatau.root";




	correlationChecker trig(mode);

	if(mode=="ee"){
		trig.dileptonTriggers() << "HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v" << "Ele17_SW_TightCaloEleId_Ele8_HE_L1R_v" << "HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v";
		trig.inputfiles()    = eemcfiles;
	}

	if(mode == "mumu"){
		trig.dileptonTriggers() << "HLT_DoubleMu7" << "HLT_Mu13_Mu8_v";
		trig.inputfiles()    = mumumcfiles;
	}

	if(mode == "emu"){
			trig.dileptonTriggers() <<
					("HLT_Mu10_Ele10_CaloIdL_")<<
					("HLT_Mu8_Ele17_CaloIdL_")<<
					("HLT_Mu17_Ele8_CaloIdL_")<<
					("HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_v")<<
					("HLT_Mu17_Ele8_CaloIdT_CaloIsoVL_v")<<
					("HLT_Mu10_Ele10_CaloIdL_v");

			trig.inputfiles()    = mumumcfiles;
		}



	trig.crossTriggers() = allmettriggers;//allmettriggers;

	//  std::cout << "\nAll MET triggers in mode " << mode << "\n" << std::endl;
	//  trig.getTriggers("MET");
	trig.getTriggers("MET");
	std::cout << "\nCorrelations:\n" << std::endl;
	//trig.getCorrelations();

}

int main(){
	checkCorrelations11();
	return 0;
}
