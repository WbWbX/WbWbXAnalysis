#include "triggerAnalyzer_base3.h"
#include "TtZAnalysis/plugins/leptonSelector2.h"
#include "TTreePerfStats.h"

namespace TAhelper{

bool muonId(ztop::NTMuon * muon){
	if(!muon->isGlobal()) return false;
	if(!muon->isPf()) return false;
	if(fabs(muon->eta()) > 2.4) return false;
	if(muon->isoVal() > 0.15) return false; //0.15!!!!but with r=03
	if(muon->normChi2() > 10) return false;

	if(muon->muonHits() <1) return false;

	if(muon->pixHits() <1) return false;
	if(muon->trkHits() < 6) return false;
	if(fabs(muon->d0V()) > 0.02) return false;
	if(fabs(muon->dzV()) > 0.5) return false;

	//members: 0: matchedstations, 1: ptdiff, 2: dB //def: -9999999
	if(muon->getMember(0)  <2) return false;
	if(fabs(muon->getMember(1)) > 5) return false;

	return true;

}

bool elecId(ztop::NTElectron * elec){

	if(fabs(elec->eta())>2.5) return false;
	if(fabs(elec->eta()) > 1.4442 && fabs(elec->eta()) < 1.566)return false;
	//if(fabs(elec->suClu().eta()) > 1.4442 && fabs(elec->suClu().eta()) < 1.566)return false;
	if(elec->rhoIso() > 0.15) return false;

	if(fabs(elec->d0V()) >= 0.02) return false;

	if(!elec->isNotConv()) return false;


	////10: pass iD (>0)

	if(!elec->isPf()) return false;
	if(fabs(elec->dzV())>0.1) return false;
	if(elec->getMember(10) < 0.5) return false;
	return true;
}

}

double 
triggerAnalyzer::selectDileptons(std::vector<ztop::NTMuon> * inputMuons, std::vector<ztop::NTElectron> * inputElectrons){

	using namespace std;
	using namespace ztop;
	float stopMass=0;
	float chiMass=0;


	std::vector<double> * pStopMass=tempp0_;
	std::vector<double>  * pChiMass=tempp1_;
	//do susy gen cuts

	if(isMC_){

		if(pStopMass->size()>0 && pChiMass->size() >0){

			stopMass=(pStopMass->at(0));
			for(size_t j=0;j<pChiMass->size();j++){
				chiMass=(pChiMass->at(j));
				if(chiMass >0)
					break;
			}
			bool rightmass=false;
			if(stopMass > 305 && stopMass < 355 && (stopMass - chiMass) > 195 && (stopMass - chiMass) < 255)
				rightmass=true;
			if(!rightmass)
				return -1;
		}
	}

	//lepton selection

	std::vector<ztop::NTElectron* > tempelecs,lowkinelecs;
	std::vector<ztop::NTMuon* > tempmuons,lowkinmuons;
	size_t highkinidx=1000000;
	for(size_t i=0;i<inputMuons->size();i++){
		ztop::NTMuon * muon = &inputMuons->at(i);
		//select
		if(muon->pt() < 20) continue;

		if(!TAhelper::muonId(muon)) continue;

		tempmuons << muon;
		highkinidx=i;
		break;
	}

	for(size_t i=0;i<inputMuons->size();i++){
		if(i==highkinidx) continue;
		ztop::NTMuon * muon = &inputMuons->at(i);
		if(!TAhelper::muonId(muon)) continue;
		if(muon->pt() < 10) continue;
		lowkinmuons << muon;
	}

	highkinidx=1000000;
	for(size_t i=0;i<inputElectrons->size();i++){
		ztop::NTElectron * elec = &inputElectrons->at(i);
		if(elec->pt() < 20) continue;
		if(!TAhelper::elecId(elec)) continue;
		tempelecs << elec;
		highkinidx=i;
		break;
	}

	for(size_t i=0;i<inputElectrons->size();i++){
		if(i==highkinidx) continue;
		ztop::NTElectron * elec = &inputElectrons->at(i);
		if(elec->pt() < 10) continue;
		if(!TAhelper::elecId(elec)) continue;
		lowkinelecs << elec;
	}

	double mass=0;

	if(mode_<0){ //ee
		if(tempelecs.size()<1) return 0;
	}
	else if(mode_==0){ //emu
		if(tempelecs.size()<1 && tempmuons.size()<1) return 0; //neither highpt elec nor highptmu
	}
	else{ //mumu
		if(tempmuons.size()<1) return 0;
	}

	tempelecs << lowkinelecs; //merge again
	tempmuons << lowkinmuons;

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



void trigger_SUSY(){

	//triggerAnalyzer::testmode=true;
	//triggerAnalyzer::lowMCStat=true;

	bool susymc=false;
	//if(susymc)
	//	triggerAnalyzer::lowMCStat=true;

	bool displayStatus=true;
	bool sepruns=false;


	enum channels{ee,emu,mumu,all};
	channels usechan=mumu;

	using namespace std;
	using namespace ztop;

	std::vector<float> binsmumueta, bins2dee, bins2dmue,binsptmu, binspte, bins2dmumu;

	binsmumueta << -2.4 << -2.1 << -1.2 << -0.9 << 0.9 << 1.2 << 2.1 << 2.4;
	bins2dmumu << 0 << 0.9 << 1.2 << 2.1 << 2.4;//2.1 << 2.4;
	bins2dee <<0 << 1.4442 << 1.566 << 2.5;
	bins2dmue << 0 << 1.2 << 2.4;
	binsptmu << 10 << 15 << 20 << 40 << 70 << 200;
	binspte << 10 << 20 << 40 << 70 << 100 << 200;

	bool includecorr=true;

	triggerAnalyzer ta_eed;
	triggerAnalyzer ta_mumud;
	triggerAnalyzer ta_emud;

	ta_eed.setMode("ee");
	ta_eed.setMassCutLow(20);
	ta_eed.setIncludeCorr(includecorr);

	ta_emud.setMode("emu");
	ta_emud.setMassCutLow(20);
	ta_emud.setIncludeCorr(includecorr);
	//   ta_emud.setDileptonTrigger("HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v");

	ta_mumud.setMode("mumu");
	//  ta_mumud.setDileptonTrigger("HLT_Mu17_Mu8_v");
	ta_mumud.setMassCutLow(20);
	ta_mumud.setIncludeCorr(includecorr);

	ta_mumud.setBinsEta(binsmumueta);
	ta_mumud.setBinsEta2dX(bins2dmumu);
	ta_mumud.setBinsEta2dY(bins2dmumu);
	ta_mumud.setBinsPt(binsptmu);

	ta_eed.setBinsEta2dX(bins2dee);
	ta_eed.setBinsEta2dY(bins2dee);
	ta_eed.setBinsPt(binspte);
	ta_emud.setBinsEta2dX(bins2dee);
	ta_emud.setBinsEta2dY(bins2dmue);
	ta_emud.setBinsPt(binspte);


	//displayStatus
	ta_eed.setDisplayStatus(displayStatus);
	ta_emud.setDisplayStatus(displayStatus);
	ta_mumud.setDisplayStatus(displayStatus);

	triggerAnalyzer ta_eeMC=ta_eed;
	triggerAnalyzer ta_mumuMC=ta_mumud;
	triggerAnalyzer ta_emuMC=ta_emud;

	ta_eeMC.setIsMC(true);
	ta_mumuMC.setIsMC(true);
	ta_emuMC.setIsMC(true);


	TString dir="/scratch/hh/dust/naf/cms/user/kieseler/trees_SUSYES_Dec13/"; //"/scratch/hh/dust/naf/cms/user/kieseler/trees_ES_Jul13/";
	TString mcpufile= dir+ "SUSY_FS_PU.root";
	if(susymc){
		ta_eeMC.setMCPUFile(mcpufile);
		ta_mumuMC.setMCPUFile(mcpufile);
		ta_emuMC.setMCPUFile(mcpufile);
	}


	TString cmssw_base=getenv("CMSSW_BASE");


	TString PURunA = cmssw_base+"/src/TtZAnalysis/Data/ReRecoJan13RunA.json.txt_PU.root";
	TString PURunB = cmssw_base+"/src/TtZAnalysis/Data/ReRecoJan13RunB.json.txt_PU.root";
	//  TString PURunAB = cmssw_base+"/src/TtZAnalysis/Data/RunABComp_PU.root";
	TString PURunC = cmssw_base+"/src/TtZAnalysis/Data/ReRecoJan13RunC.json.txt_PU.root";
	TString PURunD = cmssw_base+"/src/TtZAnalysis/Data/ReRecoJan13RunD.json.txt_PU.root";

	TString pileuproot = cmssw_base+"/src/TtZAnalysis/Data/ReRecoJan13.json.txt_PU.root";


	std::vector<TString> mumumcfiles, eemcfiles, emumcfiles, datafilesFull,datafilesRunB,datafilesRunAB,datafilesRunC, datafilesRunA, datafilesRunD;

	if(!susymc){ mumumcfiles << dir+"tree_8TeV_mumuttbar.root"
		<< dir+"tree_8TeV_mumuttbarviatau.root" ;
	//mumumcfiles <<dir +"tree_8TeV_susy.root";
	eemcfiles << dir+"tree_8TeV_eettbar.root"
			<< dir+"tree_8TeV_eettbarviatau.root" ;
	//<<dir +"tree_8TeV_susy.root";
	emumcfiles << dir+"tree_8TeV_emuttbar.root"
			<< dir+"tree_8TeV_emuttbarviatau.root" ;
	//<<dir +"tree_8TeV_susy.root";
	}
	else{//susymc==true
		mumumcfiles //<< dir+"tree_8TeV_mumuttbar.root"
		//<< dir+"tree_8TeV_mumuttbarviatau.root" ;
		<<dir +"tree_8TeV_susy.root";
		eemcfiles// << dir+"tree_8TeV_eettbar.root"
		//<< dir+"tree_8TeV_eettbarviatau.root" ;
		<<dir +"tree_8TeV_susy.root";
		emumcfiles //<< dir+"tree_8TeV_emuttbar.root"
		//<< dir+"tree_8TeV_emuttbarviatau.root" ;
		<<dir +"tree_8TeV_susy.root";
	}
	datafilesFull //  << dir + "tree_8TeV_MET_runA_06Aug.root"
	// 		 << dir + "tree_8TeV_MET_runA_13Jul.root"
	// 		 << dir + "tree_8TeV_MET_runB_13Jul.root"
	// 		 << dir + "tree_8TeV_MET_runC_prompt.root"
	// 		 << dir + "tree_8TeV_MET_runC_24Aug.root"
	// 		 << dir + "tree_8TeV_MET_runC_11Dec.root"
	// 		 << dir + "tree_8TeV_MET_runD_prompt.root";

	<< dir + "tree_8TeV_MET_runA_22Jan.root"
	<< dir + "tree_8TeV_MET_runB_22Jan.root"
	<< dir + "tree_8TeV_MET_runC_22Jan.root"
	<< dir + "tree_8TeV_MET_runD_22Jan.root";

	datafilesRunA  << dir + "tree_8TeV_MET_runA_22Jan.root";

	datafilesRunB  <<  dir + "tree_8TeV_MET_runB_22Jan.root";


	datafilesRunAB  << datafilesRunA << datafilesRunB;

	datafilesRunC  <<  dir + "tree_8TeV_MET_runC_22Jan.root";

	datafilesRunD  <<  dir + "tree_8TeV_MET_runD_22Jan.root";



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




	//analyzeAll( ta_eed,  ta_eeMC,  ta_mumud,  ta_mumuMC,  ta_emud,  ta_emuMC,"direct_comb","direct 19 fb^{-1}");





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

	if(sepruns)
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

	if(sepruns)
		analyzeAll( ta_eedB,  ta_eeMCB,  ta_mumudB,  ta_mumuMCB,  ta_emudB,  ta_emuMCB,"RunB","Run B");

	triggerAnalyzer ta_eedC=ta_eed;
	triggerAnalyzer ta_mumudC=ta_mumud;
	triggerAnalyzer ta_emudC=ta_emud;

	ta_eedC.setChain(datafilesRunC);
	ta_mumudC.setChain(datafilesRunC);
	ta_emudC.setChain(datafilesRunC);


	triggerAnalyzer ta_eeMCC=ta_eeMC;
	triggerAnalyzer ta_mumuMCC=ta_mumuMC;
	triggerAnalyzer ta_emuMCC=ta_emuMC;


	ta_eeMCC.setPUFile(PURunC);
	ta_mumuMCC.setPUFile(PURunC);
	ta_emuMCC.setPUFile(PURunC);

	if(sepruns)
		analyzeAll( ta_eedC,  ta_eeMCC,  ta_mumudC,  ta_mumuMCC,  ta_emudC,  ta_emuMCC,"RunC","Run C");


	triggerAnalyzer ta_eedD=ta_eed;
	triggerAnalyzer ta_mumudD=ta_mumud;
	triggerAnalyzer ta_emudD=ta_emud;

	ta_eedD.setChain(datafilesRunD);
	ta_mumudD.setChain(datafilesRunD);
	ta_emudD.setChain(datafilesRunD);


	triggerAnalyzer ta_eeMCD=ta_eeMC;
	triggerAnalyzer ta_mumuMCD=ta_mumuMC;
	triggerAnalyzer ta_emuMCD=ta_emuMC;


	ta_eeMCD.setPUFile(PURunD);
	ta_mumuMCD.setPUFile(PURunD);
	ta_emuMCD.setPUFile(PURunD);

	if(sepruns){
		analyzeAll( ta_eedD,  ta_eeMCD,  ta_mumudD,  ta_mumuMCD,  ta_emudD,  ta_emuMCD,"RunD","Run D");
	}
	else{

		if(usechan==ee || usechan==all){
			ta_eedA.Eff();
			ta_eedB.Eff();
			ta_eedC.Eff();
			ta_eedD.Eff();
		}

		if(usechan==emu || usechan==all){
			ta_emudA.Eff();
			ta_emudB.Eff();
			ta_emudC.Eff();
			ta_emudD.Eff();

		}
		if(usechan==mumu || usechan==all){
			ta_mumudA.Eff();
			ta_mumudB.Eff();
			ta_mumudC.Eff();
			ta_mumudD.Eff();

		}


		//MC

	}



	double lumiA=0.890;
	double lumiB=4.430;
	double lumiC=7.026;
	double lumiD=7.272;

	double lumitotal=lumiA+lumiB+lumiC+lumiD;

	//for ee
	TString eestring;
	if(usechan==ee|| usechan==all){
		double eetotal=ta_eedA.getGlobalDen() + ta_eedB.getGlobalDen() + ta_eedC.getGlobalDen() + ta_eedD.getGlobalDen();
		double eeMCtotal=ta_eeMCA.getGlobalDen() + ta_eeMCB.getGlobalDen() + ta_eeMCC.getGlobalDen() + ta_eeMCD.getGlobalDen();

		double eeAscale=eetotal/ta_eedA.getGlobalDen() * lumiA/lumitotal;
		ta_eedA.scale(eeAscale);
		double eeMCAscale=eeMCtotal/ta_eeMCA.getGlobalDen() * lumiA/lumitotal;
		ta_eeMCA.scale(eeMCAscale);
		double eeBscale=eetotal/ta_eedB.getGlobalDen() * lumiB/lumitotal;
		ta_eedB.scale(eeBscale);
		double eeMCBscale=eeMCtotal/ta_eeMCB.getGlobalDen() * lumiB/lumitotal;
		ta_eeMCB.scale(eeMCBscale);
		double eeCscale=eetotal/ta_eedC.getGlobalDen() * lumiC/lumitotal;
		ta_eedC.scale(eeCscale);
		double eeMCCscale=eeMCtotal/ta_eeMCC.getGlobalDen() * lumiC/lumitotal;
		ta_eeMCC.scale(eeMCCscale);
		double eeDscale=eetotal/ta_eedD.getGlobalDen() * lumiD/lumitotal;
		ta_eedD.scale(eeDscale);
		double eeMCDscale=eeMCtotal/ta_eeMCD.getGlobalDen() * lumiD/lumitotal;
		ta_eeMCD.scale(eeMCDscale);


		triggerAnalyzer eedFull = ta_eedA + ta_eedB + ta_eedC + ta_eedD;
		triggerAnalyzer eemcFull;
		if(sepruns){
			eemcFull = ta_eeMCA + ta_eeMCB + ta_eeMCC + ta_eeMCD;
		}
		else{
			eemcFull=ta_eeMC;
			eemcFull.Eff();;
		}
		eestring= makeFullOutput(eedFull, eemcFull, "ee_Full", "ee Full 19 fb^{-1}", 0.01);
	}
	////
	TString mumustring;
	if(usechan==mumu||usechan==all){
		double mumutotal=ta_mumudA.getGlobalDen() + ta_mumudB.getGlobalDen() + ta_mumudC.getGlobalDen() + ta_mumudD.getGlobalDen();
		double mumuMCtotal=ta_mumuMCA.getGlobalDen() + ta_mumuMCB.getGlobalDen() + ta_mumuMCC.getGlobalDen() + ta_mumuMCD.getGlobalDen();

		double mumuAscale=mumutotal/ta_mumudA.getGlobalDen() * lumiA/lumitotal;
		ta_mumudA.scale(mumuAscale);
		double mumuMCAscale=mumuMCtotal/ta_mumuMCA.getGlobalDen() * lumiA/lumitotal;
		ta_mumuMCA.scale(mumuMCAscale);
		double mumuBscale=mumutotal/ta_mumudB.getGlobalDen() * lumiB/lumitotal;
		ta_mumudB.scale(mumuBscale);
		double mumuMCBscale=mumuMCtotal/ta_mumuMCB.getGlobalDen() * lumiB/lumitotal;
		ta_mumuMCB.scale(mumuMCBscale);
		double mumuCscale=mumutotal/ta_mumudC.getGlobalDen() * lumiC/lumitotal;
		ta_mumudC.scale(mumuCscale);
		double mumuMCCscale=mumuMCtotal/ta_mumuMCC.getGlobalDen() * lumiC/lumitotal;
		ta_mumuMCC.scale(mumuMCCscale);
		double mumuDscale=mumutotal/ta_mumudD.getGlobalDen() * lumiD/lumitotal;
		ta_mumudD.scale(mumuDscale);
		double mumuMCDscale=mumuMCtotal/ta_mumuMCD.getGlobalDen() * lumiD/lumitotal;
		ta_mumuMCD.scale(mumuMCDscale);


		triggerAnalyzer mumudFull = ta_mumudA + ta_mumudB + ta_mumudC + ta_mumudD;
		triggerAnalyzer mumumcFull;// = ta_mumuMCA + ta_mumuMCB + ta_mumuMCC + ta_mumuMCD;
		if(sepruns){
			mumumcFull = ta_mumuMCA + ta_mumuMCB + ta_mumuMCC + ta_mumuMCD;
		}
		else{
			mumumcFull=ta_mumuMC;
			mumumcFull.Eff();;
		}
		mumustring= makeFullOutput(mumudFull, mumumcFull, "mumu_Full", "mumu Full 19 fb^{-1}", 0.01);
	}

	///
	TString emustring;
	if(usechan==emu || usechan==all){
		double emutotal=ta_emudA.getGlobalDen() + ta_emudB.getGlobalDen() + ta_emudC.getGlobalDen() + ta_emudD.getGlobalDen();
		double emuMCtotal=ta_emuMCA.getGlobalDen() + ta_emuMCB.getGlobalDen() + ta_emuMCC.getGlobalDen() + ta_emuMCD.getGlobalDen();

		double emuAscale=emutotal/ta_emudA.getGlobalDen() * lumiA/lumitotal;
		ta_emudA.scale(emuAscale);
		double emuMCAscale=emuMCtotal/ta_emuMCA.getGlobalDen() * lumiA/lumitotal;
		ta_emuMCA.scale(emuMCAscale);
		double emuBscale=emutotal/ta_emudB.getGlobalDen() * lumiB/lumitotal;
		ta_emudB.scale(emuBscale);
		double emuMCBscale=emuMCtotal/ta_emuMCB.getGlobalDen() * lumiB/lumitotal;
		ta_emuMCB.scale(emuMCBscale);
		double emuCscale=emutotal/ta_emudC.getGlobalDen() * lumiC/lumitotal;
		ta_emudC.scale(emuCscale);
		double emuMCCscale=emuMCtotal/ta_emuMCC.getGlobalDen() * lumiC/lumitotal;
		ta_emuMCC.scale(emuMCCscale);
		double emuDscale=emutotal/ta_emudD.getGlobalDen() * lumiD/lumitotal;
		ta_emudD.scale(emuDscale);
		double emuMCDscale=emuMCtotal/ta_emuMCD.getGlobalDen() * lumiD/lumitotal;
		ta_emuMCD.scale(emuMCDscale);



		triggerAnalyzer emudFull = ta_emudA + ta_emudB + ta_emudC + ta_emudD;
		triggerAnalyzer emumcFull;// = ta_emuMCA + ta_emuMCB + ta_emuMCC + ta_emuMCD;
		if(sepruns){
			emumcFull = ta_emuMCA + ta_emuMCB + ta_emuMCC + ta_emuMCD;
		}
		else{
			emumcFull=ta_emuMC;
			emumcFull.Eff();;
		}
		emustring=makeFullOutput(emudFull, emumcFull, "emu_Full", "emu Full 19 fb^{-1}", 0.01);
	}
	std::cout << "\n\nweighted output summary: " << std::endl;

	std::cout << "channel  & $\\epsilon_{data}$ & $\\epsilon_{MC}$ & SF & $\\alpha$ \\\\ " << std::endl;
	std::cout << eestring<< std::endl;
	std::cout << mumustring<< std::endl;
	std::cout << emustring<< std::endl;

}
