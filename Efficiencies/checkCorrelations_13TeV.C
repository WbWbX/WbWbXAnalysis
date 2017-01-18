#include "TtZAnalysis/Efficiencies/correlationChecker.h"
#include <iostream>



void checkCorrelations_13TeV(){

	using namespace ztop;
	using namespace std;
	TString mode="emu";

	vector<TString> allmettriggers, selectedmettriggers;;

	allmettriggers<< "HLT_LooseIsoPFTau50_Trk30_eta2p1_MET120_v1"
				<<"HLT_PFHT350_PFMET120_NoiseCleaned_v1"
				<<"HLT_PFMET120_NoiseCleaned_BTagCSV07_v1"
				<<"HLT_Photon22_R9Id90_HE10_Iso40_EBOnly_PFMET40_v1"
				<<"HLT_PFMET170_NoiseCleaned_v1"
				<<"HLT_Photon36_R9Id90_HE10_Iso40_EBOnly_PFMET40_v1"
				<<"HLT_Photon50_R9Id90_HE10_Iso40_EBOnly_PFMET40_v1"
				<<"HLT_Photon135_PFMET40_v1"
			    <<"HLT_Photon75_R9Id90_HE10_Iso40_EBOnly_PFMET40_v1"
				<<"HLT_Photon150_PFMET40_v1"
				<<"HLT_Photon160_PFMET40_v1"
				<<"HLT_Photon90_R9Id90_HE10_Iso40_EBOnly_PFMET40_v1"
				<<"HLT_Photon250_NoHE_PFMET40_v1"
				<<"HLT_Photon300_NoHE_PFMET40_v1";


	TString MCdir="/afs/desy.de/user/t/tarndt/xxl/TTZ/CMSSW_7_2_2_patch2/src/TtZAnalysis/createNTuples/";

	std::vector<TString> eemcfiles,mumumcfiles,emumcfiles;
/*	eemcfiles << MCdir+"tree_7TeV_eettbar_trig.root"
			<< MCdir+"tree_7TeV_eettbarviatau_trig.root" ;

	mumumcfiles << MCdir+"tree_7TeV_mumuttbar_trig.root"
			<< MCdir+"tree_7TeV_mumuttbarviatau_trig.root" ;
*/
	emumcfiles << MCdir+"def_out.root";




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
					"HLT_Mu23_TrkIsoVVL_Ele12_Gsf_CaloId_TrackId_Iso_MediumWP_v"<<
					"HLT_Mu8_TrkIsoVVL_Ele23_Gsf_CaloId_TrackId_Iso_MediumWP_v";

			trig.inputfiles()    = emumcfiles;
		}



	trig.crossTriggers() = allmettriggers;//allmettriggers;

	//  std::cout << "\nAll MET triggers in mode " << mode << "\n" << std::endl;
	//  trig.getTriggers("MET");
	//trig.getTriggers("MET");
	std::cout << "\nCorrelations:\n" << std::endl;
	trig.getCorrelations();

}

int main(){
	checkCorrelations_13TeV();
	return 0;
}

