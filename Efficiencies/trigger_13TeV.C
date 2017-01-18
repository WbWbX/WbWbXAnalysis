#include "triggerAnalyzer_base_13TeV.h"
#include "TtZAnalysis/Analysis/interface/leptonSelector2.h"
#include "TtZAnalysis/Tools/TUnfold/TUnfold.h"
#include "TTreePerfStats.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"

#define DEF_ELEC_PT 15
#define DEF_MUON_PT 15

double 
triggerAnalyzer::selectDileptons(std::vector<ztop::NTMuon> * inputMuons, std::vector<ztop::NTElectron> * inputElectrons){

	using namespace std;
	using namespace ztop;

	std::vector<ztop::NTElectron* > tempelecs;
	std::vector<ztop::NTMuon* > tempmuons;

	for(size_t i=0;i<inputMuons->size();i++){
		ztop::NTMuon * muon = &inputMuons->at(i);


		if(muon->pt() < DEF_ELEC_PT)       continue;
		if(fabs(muon->eta())>2.4) continue;

		//tight muon selection: https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMuonId#Tight_Muon
		//isPF is implicit

		if(muon->isGlobal()
				&& muon->normChi2()<10.
				&& muon->muonHits()>0
				&& muon->matchedStations() >1  //not in trees
				&& fabs(muon->d0V())<0.2
				&& fabs(muon->dzV())<0.5
				&& muon->pixHits()>0
				&& muon->trkHits()>5
				&& fabs(muon->isoVal()) < 0.15){
			tempmuons << muon;

		}
	}



	for(size_t i=0;i<inputElectrons->size();i++){
		ztop::NTElectron * elec = &inputElectrons->at(i);
		/*	if(elec->pt() < 20)  continue;
		float abseta=fabs(elec->eta());
		float suclueta = fabs(elec->ECalP4().eta());
		if(abseta > 2.4) continue;
		if(suclueta > 1.4442 && suclueta < 1.5660) continue; //transistion region
		if(fabs(elec->d0V()) < 0.02
				&& elec->isNotConv()
				&& elec->mvaId() > 0.9
				&& elec->mHits() <= 0){
			if(fabs(elec->rhoIso())<0.1){
				tempelecs << elec;

			}
		} */
		//
		if(elec->pt() < DEF_ELEC_PT)  continue;
		float abseta=fabs(elec->eta());
                if(elec->storedId() < 0.9) continue;
		float suclueta = fabs(elec->ECalP4().eta());
		if(abseta > 2.4) continue;
		if(suclueta > 1.4442 && suclueta < 1.5660) continue; //transistion region
                //if(elec->getMember(0)> 0.15) continue;
		//kinelectrons  << elec;
                if(suclueta < 1.4442 && (fabs(elec->d0V()) > 0.05 ||fabs(elec->dzV())>0.10) ) continue;
                if(suclueta > 1.5660 && (fabs(elec->d0V()) > 0.10 ||fabs(elec->dzV())>0.20) ) continue;
	//	if(fabs(elec->d0V()) < 0.02
	//			&& elec->isNotConv()
	//			&& elec->storedId() > 0.9
	//			&& elec->mHits() <= 0
	//			&& elec->isPf()){

		//	idelectrons <<  elec;
		//	if(fabs(elec->rhoIso())<0.1){
				//isoelectrons <<  elec;
				tempelecs << elec;
			//}

		//}


	}

	double mass=0;

	std::pair<std::vector<ztop::NTElectron* > , std::vector<ztop::NTMuon* > > leppair=getOppoQHighestPtPair(tempelecs,tempmuons);

	selectedElecs_=leppair.first;
	selectedMuons_=leppair.second;


	if(mode_<0){ //ee
		if(selectedElecs_.size() < 2) return 0;
                if(!(selectedElecs_.at(0)->pt() > 25.)) return 0;
		mass=(selectedElecs_.at(0)->p4() + selectedElecs_.at(1)->p4()).M();
	}
	else if(mode_==0){ //emu
		if(selectedElecs_.size() < 1 || selectedMuons_.size() <1) return 0;
                if(!(selectedElecs_.at(0)->pt() > 25.||selectedMuons_.at(0)->pt() > 25.)) return 0;
		mass=(selectedElecs_.at(0)->p4() + selectedMuons_.at(0)->p4()).M();

	}
	else{ //mumu
		if(selectedMuons_.size() <2) return 0;
                if(!(selectedMuons_.at(0)->pt() > 25.)) return 0;
                //if(((selectedMuons_.at(0)->eta() < -1.2 && selectedMuons_.at(1)->eta() < -1.2) || (selectedMuons_.at(0)->eta() >1.2 && selectedMuons_.at(1)->eta() > 1.2)) && deltaPhi(selectedMuons_.at(0)->phi(),selectedMuons_.at(1)->phi()< 1.04 )) return 0;
		mass=(selectedMuons_.at(0)->p4() + selectedMuons_.at(1)->p4()).M();
	}



	return mass;

}



void trigger_tightLeptons(){

	triggerAnalyzer::testmode=false;

	AutoLibraryLoader::enable();

	using namespace std;
	using namespace ztop;

	std::vector<float> binsmumueta, bins2dee, bins2dmue,binsptmu, binspte, bins2dmumu,binseeeta;

	binsmumueta << -2.4 << -2.1 << -1.2 << -0.9 << 0.9 << 1.2 << 2.1 << 2.4;
	bins2dmumu << 0<<0.4 << 0.9 << 1.2 << 2.1 << 2.4;
	bins2dee <<0 << 1.47 << 2.4;
	bins2dmue << 0 << 1.2 << 2.4;
      
        binseeeta<< -2.5<< -2.1<< -1.56<< -1.44<< -1.0<< -0.6<< -0.3<< -0.1<< 0.1<< 0.3<< 0.6<< 1.0<< 1.44<< 1.56<< 2.1<< 2.5;


	bool includecorr=true;

	triggerAnalyzer ta_eed;
	triggerAnalyzer ta_mumud;
	triggerAnalyzer ta_emud;

	ta_eed.setMode("ee");
	ta_eed.setMassCutLow(20);
	ta_eed.setIncludeCorr(includecorr);
	//ta_eed.setDileptonTrigger("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v");

	ta_emud.setMode("emu");
	ta_emud.setMassCutLow(20);
	ta_emud.setIncludeCorr(includecorr);
	//ta_emud.setDileptonTrigger("HLT_Mu23_TrkIsoVVL_Ele12_Gsf_CaloId_TrackId_Iso_MediumWP_v");
	//ta_emud.setDileptonTrigger("HLT_Mu8_TrkIsoVVL_Ele23_Gsf_CaloId_TrackId_Iso_MediumWP_v"); //HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v


	ta_mumud.setMode("mumu");
	//  ta_mumud.setDileptonTrigger("HLT_Mu17_Mu8_v");
	ta_mumud.setMassCutLow(20);
	ta_mumud.setIncludeCorr(includecorr);
        std::vector<std::string> triggerObjects;
        //triggerObjects <<"HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v"<<"HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v"<<"hltL3fL1sDoubleMu114L1f0L2f10OneMuL3Filtered17"<<"hltDiMuonGlb17Glb8RelTrkIsoFiltered0p4"<<"hltDiMuonGlb17Trk8RelTrkIsoFiltered0p4"<<"hltL3pfL1sDoubleMu114L1f0L2pf0L3PreFiltered8"<<"hltL2pfL1sDoubleMu114ORDoubleMu125L1f0L2PreFiltered0"<<"hltDiMuonGlb17Trk8RelTrkIsoFiltered0p4"<<"hltDiMuonGlbFiltered17TrkFiltered8";
        ta_mumud.setTriggerObjects(triggerObjects);

	ta_mumud.setBinsEta(binsmumueta);
	ta_mumud.setBinsEta2dX(bins2dmumu);
	ta_mumud.setBinsEta2dY(bins2dmumu);

        ta_eed.setBinsEta(binseeeta);
	ta_eed.setBinsEta2dX(bins2dee);
	ta_eed.setBinsEta2dY(bins2dee);
	ta_emud.setBinsEta2dX(bins2dee);
	ta_emud.setBinsEta2dY(bins2dmue);


	triggerAnalyzer ta_eeMC=ta_eed;
	triggerAnalyzer ta_mumuMC=ta_mumud;
	triggerAnalyzer ta_emuMC=ta_emud;

	ta_eeMC.setIsMC(true);
	ta_mumuMC.setIsMC(true);
	ta_emuMC.setIsMC(true);
        
        //ta_eeMC.setIncludeCorr(false);
        //ta_mumuMC.setIncludeCorr(false);
        //ta_emuMC.setIncludeCorr(false);

	TString dir="/nfs/dust/cms/user/tarndt/NTuples/Prod_8020/";
        //TString dir="/nfs/dust/cms/group/topcmsdesy/TTH_Trigger/";

	TString cmssw_base=getenv("CMSSW_BASE");




	std::vector<TString> mumumcfiles, eemcfiles, emumcfiles, datafilesFull,datafilesRunB,datafilesRunAB,datafilesRunC, datafilesRunA, datafilesRunD;

	mumumcfiles << dir+"tree_13TeV_emuttbar_Mor17_trig.root"<< dir+"tree_13TeV_emuttbarbg_Mor17_trig.root" ;
//
	eemcfiles << dir+"tree_13TeV_emuttbar_Mor17_trig.root"<< dir+"tree_13TeV_emuttbarbg_Mor17_trig.root" ;

	emumcfiles << dir+"tree_13TeV_emuttbar_Mor17_trig.root"<< dir+"tree_13TeV_emuttbarbg_Mor17_trig.root" ;

	datafilesFull  << dir + "tree_13Tev_data_MET_RunC_golden.root"<< dir + "tree_13Tev_data_MET_RunB_golden.root"<< dir + "tree_13Tev_data_MET_RunD_golden.root"<< dir + "tree_13Tev_data_MET_RunE_golden.root"<< dir + "tree_13Tev_data_MET_RunF_golden.root"<< dir + "tree_13Tev_data_MET_RunG_golden.root";
 
        //datafilesFull  << dir + "tree_13Tev_data_MET_RunG.root";
      
        //datafilesFull << dir + "tree_13TeV_dy50inf_lo.root";

	TString pileuproot = cmssw_base+"/src/TtZAnalysis/Data/ReRecoJan13RunD.json.txt_PU.root";

	//for others just copy data and mc, change input and pu file


	//ta_eeMC.setPUFile(pileuproot);
	//ta_mumuMC.setPUFile(pileuproot);
	//ta_emuMC.setPUFile(pileuproot);

	ta_eeMC.setChain(eemcfiles);
	ta_mumuMC.setChain(mumumcfiles);
	ta_emuMC.setChain(emumcfiles);

	//ta_eed.setPUFile(pileuproot);
	//ta_mumud.setPUFile(pileuproot);
	//ta_emud.setPUFile(pileuproot);


	ta_eed.setChain(datafilesFull);
	ta_mumud.setChain(datafilesFull);
	ta_emud.setChain(datafilesFull);



    std::cout<<"before Analyze"<<std::endl;
	analyzeAll( ta_eed,  ta_eeMC,  ta_mumud,  ta_mumuMC, ta_emud,ta_emuMC,  "direct_comb","direct 19 fb^{-1}");
    std::cout<<"after Analyze"<<std::endl;




	triggerAnalyzer emudFull = ta_emud;
	triggerAnalyzer emumcFull = ta_emuMC;
	TString emustring=makeFullOutput(emudFull, emumcFull, "emu_Full", "emu Full", 0.0);
        TString eestring=makeFullOutput(ta_eed,ta_eeMC,"ee_Full","ee Full",0.0);
        TString mumustring=makeFullOutput(ta_mumud,ta_mumuMC,"mumu_Full","mumu_Full",0.0);

	std::cout << "\n\nweighted output summary: " << std::endl;

	std::cout << "channel  & $\\epsilon_{data}$ & $\\epsilon_{MC}$ & SF & $\\alpha$ \\\\ " << std::endl;
	std::cout << eestring<< std::endl;
	std::cout << mumustring<< std::endl;
	std::cout << emustring<< std::endl;

}

int main(){
	trigger_tightLeptons();
	return 0;
}

