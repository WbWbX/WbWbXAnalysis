#include "triggerAnalyzer_base3.h"
#include "TtZAnalysis/plugins/leptonSelector2.h"
#include "TTreePerfStats.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"

#define DEF_ELEC_PT 30
#define DEF_MUON_PT 30

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
				&& fabs(muon->isoVal()) < 0.12){
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

		float suclueta = fabs(elec->ECalP4().eta());
		if(abseta > 2.4) continue;
		if(suclueta > 1.4442 && suclueta < 1.5660) continue; //transistion region
		//kinelectrons  << elec;

		if(fabs(elec->d0V()) < 0.02
				&& elec->isNotConv()
				&& elec->mvaId() > 0.9
				&& elec->mHits() <= 0
				&& elec->isPf()){

		//	idelectrons <<  elec;
			if(fabs(elec->rhoIso())<0.1){
				//isoelectrons <<  elec;
				tempelecs << elec;
			}

		}


	}

	double mass=0;

	std::pair<std::vector<ztop::NTElectron* > , std::vector<ztop::NTMuon* > > leppair=getOppoQHighestPtPair(tempelecs,tempmuons);

	selectedElecs_=leppair.first;
	selectedMuons_=leppair.second;


	if(mode_<0){ //ee
		if(selectedElecs_.size() < 2) return 0;
		mass=(selectedElecs_.at(0)->p4() + selectedElecs_.at(1)->p4()).M();
	}
	else if(mode_==0){ //emu
		if(selectedElecs_.size() < 1 || selectedMuons_.size() <1) return 0;
		mass=(selectedElecs_.at(0)->p4() + selectedMuons_.at(0)->p4()).M();

	}
	else{ //mumu
		if(selectedMuons_.size() <2) return 0;
		mass=(selectedMuons_.at(0)->p4() + selectedMuons_.at(1)->p4()).M();
	}



	return mass;

}



void trigger_tightLeptons_7TeV(){

	triggerAnalyzer::testmode=false;

	AutoLibraryLoader::enable();

	using namespace std;
	using namespace ztop;

	std::vector<float> binsmumueta, bins2dee, bins2dmue,binsptmu, binspte, bins2dmumu;

	binsmumueta << -2.4 << -2.1 << -1.2 << -0.9 << 0.9 << 1.2 << 2.1 << 2.4;
	bins2dmumu << 0 << 0.9 << 1.2 << 2.1 << 2.4;
	bins2dee <<0 << 1.47 << 2.4;
	bins2dmue << 0 << 1.2 << 2.4;

	bool includecorr=true;

	triggerAnalyzer ta_eed;
	triggerAnalyzer ta_mumud;
	triggerAnalyzer ta_emud;


	///7 TeV met triggers
	std::vector<string> mettriggers;
	mettriggers<< "HLT_CentralJet80_MET65_v11"
			 << "HLT_DiJet60_MET45_v11"
			 << "HLT_DiCentralJet20_MET80_v9"
			 << "HLT_CentralJet80_MET80_v10"
			 << "HLT_CentralJet80_MET95_v4"
			 << "HLT_DiCentralJet20_BTagIP_MET65_v12"
			 << "HLT_DiCentralJet20_MET100_HBHENoiseFiltered_v5"
			 << "HLT_CentralJet80_MET110_v4"
			 << "HLT_MET120_HBHENoiseFiltered_v7"
			 << "HLT_MET120_HBHENoiseFiltered_v7"
			 << "HLT_MET200_HBHENoiseFiltered_v7"
			 << "HLT_MET200_HBHENoiseFiltered_v7"
			 << "HLT_MET400_v3";



	std::vector<string> emutriggers,eetriggers,mumutriggers;
	emutriggers<<
			("HLT_Mu10_Ele10_CaloIdL_")<<
			("HLT_Mu8_Ele17_CaloIdL_")<<
			("HLT_Mu17_Ele8_CaloIdL_")<<
			("HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_v")<<
			("HLT_Mu17_Ele8_CaloIdT_CaloIsoVL_v")<<
			("HLT_Mu10_Ele10_CaloIdL_v");

	ta_eed.setMetTriggers(mettriggers);
	ta_mumud.setMetTriggers(mettriggers);
	ta_emud.setMetTriggers(mettriggers);

	ta_emud.setDileptonTriggers(emutriggers);
	ta_emud.setDileptonTriggersMC(emutriggers);
	ta_eed.setDileptonTriggers(eetriggers);
	ta_eed.setDileptonTriggersMC(eetriggers);
	ta_mumud.setDileptonTriggers(mumutriggers);
	ta_mumud.setDileptonTriggersMC(mumutriggers);

	ta_emud.setMode("emu");
	ta_emud.setMassCutLow(20);
	ta_emud.setIncludeCorr(includecorr);

	ta_emud.setBinsEta2dX(bins2dee);
	ta_emud.setBinsEta2dY(bins2dmue);


	triggerAnalyzer ta_eeMC=ta_eed;
	triggerAnalyzer ta_mumuMC=ta_mumud;
	triggerAnalyzer ta_emuMC=ta_emud;

	ta_eeMC.setIsMC(true);
	ta_mumuMC.setIsMC(true);
	ta_emuMC.setIsMC(true);



	TString dir="/nfs/dust/cms/user/kiesej/trees_ES_Jul14/";

	TString cmssw_base=getenv("CMSSW_BASE");


	TString PURunA = cmssw_base+"/src/TtZAnalysis/Data/ReRecoJan13RunA.json.txt_PU.root";
	TString PURunB = cmssw_base+"/src/TtZAnalysis/Data/ReRecoJan13RunB.json.txt_PU.root";

	TString pileuproot = cmssw_base+"/src/TtZAnalysis/Data/Legacy2011.json_PU.root";


	std::vector<TString> mumumcfiles, eemcfiles, emumcfiles, datafilesFull,datafilesRunB, datafilesRunA;

	mumumcfiles << dir+"tree_8TeV_mumuttbar_trig.root"
			<< dir+"tree_8TeV_mumuttbarviatau_trig.root" ;

	eemcfiles << dir+"tree_8TeV_eettbar_trig.root"
			<< dir+"tree_8TeV_eettbarviatau_trig.root" ;

	emumcfiles << dir+"tree_8TeV_emuttbar_trig.root"
			<< dir+"tree_8TeV_emuttbarviatau_trig.root" ;

	datafilesFull

	<< dir + "tree_8TeV_MET_runA_22Jan.root"
	<< dir + "tree_8TeV_MET_runB_22Jan.root";

	datafilesRunA  << dir + "tree_8TeV_MET_runA_22Jan.root";

	datafilesRunB  <<  dir + "tree_8TeV_MET_runB_22Jan.root";

	//for others just copy data and mc, change input and pu file


	ta_eeMC.setPUFile(pileuproot);
	ta_mumuMC.setPUFile(pileuproot);
	ta_emuMC.setPUFile(pileuproot);

	ta_eeMC.setChain(eemcfiles);
	ta_mumuMC.setChain(mumumcfiles);
	ta_emuMC.setChain(emumcfiles);

	ta_eed.setChain(datafilesFull);
	ta_mumud.setChain(datafilesFull);
	ta_emud.setChain(datafilesFull);




	analyzeAll( ta_eed,  ta_eeMC,  ta_mumud,  ta_mumuMC,  ta_emud,  ta_emuMC,"direct_comb","direct 5 fb^{-1}");





	///put in run dependent stuff own ana for each and then scale and combine (functions now available)
	triggerAnalyzer ta_eedA=ta_eed;
	triggerAnalyzer ta_mumudA=ta_mumud;
	triggerAnalyzer ta_emudA=ta_emud;

	ta_eedA.setChain(datafilesRunA);
	ta_mumudA.setChain(datafilesRunA);
	ta_emudA.setChain(datafilesRunA);

	triggerAnalyzer ta_eeMCA=ta_eeMC;
	triggerAnalyzer ta_mumuMCA=ta_mumuMC;
	triggerAnalyzer ta_emuMCA=ta_emuMC;

	ta_eeMCA.setPUFile(PURunA);
	ta_mumuMCA.setPUFile(PURunA);
	ta_emuMCA.setPUFile(PURunA);

	analyzeAll( ta_eedA,  ta_eeMCA,  ta_mumudA,  ta_mumuMCA,  ta_emudA,  ta_emuMCA,"RunA","Run A");

	triggerAnalyzer ta_eedB=ta_eed;
	triggerAnalyzer ta_mumudB=ta_mumud;
	triggerAnalyzer ta_emudB=ta_emud;

	ta_eedB.setChain(datafilesRunB);
	ta_mumudB.setChain(datafilesRunB);
	ta_emudB.setChain(datafilesRunB);


	triggerAnalyzer ta_eeMCB=ta_eeMC;
	triggerAnalyzer ta_mumuMCB=ta_mumuMC;
	triggerAnalyzer ta_emuMCB=ta_emuMC;

	ta_eeMCB.setPUFile(PURunB);
	ta_mumuMCB.setPUFile(PURunB);
	ta_emuMCB.setPUFile(PURunB);

	analyzeAll( ta_eedB,  ta_eeMCB,  ta_mumudB,  ta_mumuMCB,  ta_emudB,  ta_emuMCB,"RunB","Run B");



	double lumiA=0.890;
	double lumiB=4.430;

	double lumitotal=lumiA+lumiB;

	//for ee
	double eetotal=ta_eedA.getGlobalDen() + ta_eedB.getGlobalDen() ;
	double eeMCtotal=ta_eeMCA.getGlobalDen() + ta_eeMCB.getGlobalDen() ;

	double eeAscale=eetotal/ta_eedA.getGlobalDen() * lumiA/lumitotal;
	ta_eedA.scale(eeAscale);
	double eeMCAscale=eeMCtotal/ta_eeMCA.getGlobalDen() * lumiA/lumitotal;
	ta_eeMCA.scale(eeMCAscale);
	double eeBscale=eetotal/ta_eedB.getGlobalDen() * lumiB/lumitotal;
	ta_eedB.scale(eeBscale);
	double eeMCBscale=eeMCtotal/ta_eeMCB.getGlobalDen() * lumiB/lumitotal;
	ta_eeMCB.scale(eeMCBscale);



	triggerAnalyzer eedFull = ta_eedA + ta_eedB;
	triggerAnalyzer eemcFull = ta_eeMCA + ta_eeMCB;
	TString eestring= makeFullOutput(eedFull, eemcFull, "ee_Full", "ee Full 5 fb^{-1}", 0.01);

	////
	double mumutotal=ta_mumudA.getGlobalDen() + ta_mumudB.getGlobalDen() ;
	double mumuMCtotal=ta_mumuMCA.getGlobalDen() + ta_mumuMCB.getGlobalDen() ;

	double mumuAscale=mumutotal/ta_mumudA.getGlobalDen() * lumiA/lumitotal;
	ta_mumudA.scale(mumuAscale);
	double mumuMCAscale=mumuMCtotal/ta_mumuMCA.getGlobalDen() * lumiA/lumitotal;
	ta_mumuMCA.scale(mumuMCAscale);
	double mumuBscale=mumutotal/ta_mumudB.getGlobalDen() * lumiB/lumitotal;
	ta_mumudB.scale(mumuBscale);
	double mumuMCBscale=mumuMCtotal/ta_mumuMCB.getGlobalDen() * lumiB/lumitotal;
	ta_mumuMCB.scale(mumuMCBscale);


	triggerAnalyzer mumudFull = ta_mumudA + ta_mumudB;
	triggerAnalyzer mumumcFull = ta_mumuMCA + ta_mumuMCB;
	TString mumustring= makeFullOutput(mumudFull, mumumcFull, "mumu_Full", "mumu Full 5 fb^{-1}", 0.01);


	///
	double emutotal=ta_emudA.getGlobalDen() + ta_emudB.getGlobalDen();
	double emuMCtotal=ta_emuMCA.getGlobalDen() + ta_emuMCB.getGlobalDen();

	double emuAscale=emutotal/ta_emudA.getGlobalDen() * lumiA/lumitotal;
	ta_emudA.scale(emuAscale);
	double emuMCAscale=emuMCtotal/ta_emuMCA.getGlobalDen() * lumiA/lumitotal;
	ta_emuMCA.scale(emuMCAscale);
	double emuBscale=emutotal/ta_emudB.getGlobalDen() * lumiB/lumitotal;
	ta_emudB.scale(emuBscale);
	double emuMCBscale=emuMCtotal/ta_emuMCB.getGlobalDen() * lumiB/lumitotal;
	ta_emuMCB.scale(emuMCBscale);



	triggerAnalyzer emudFull = ta_emudA + ta_emudB;
	triggerAnalyzer emumcFull = ta_emuMCA + ta_emuMCB;
	TString emustring=makeFullOutput(emudFull, emumcFull, "emu_Full", "emu Full 5 fb^{-1}", 0.01);

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

