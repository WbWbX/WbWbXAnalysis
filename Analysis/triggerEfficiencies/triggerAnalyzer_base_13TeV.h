#ifndef TRIGGERANALYZER_BASE3_H
#define TRIGGERANALYZER_BASE3_H

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TtZAnalysis/DataFormats/src/classes.h"
// #include "TtZAnalysis/DataFormats/interface/NTElectron.h"
// #include "TtZAnalysis/DataFormats/interface/NTEvent.h"
// #include "TtZAnalysis/DataFormats/interface/NTJet.h"
// #include "TtZAnalysis/DataFormats/interface/NTMet.h"
#include <vector>
#include <algorithm>
#include <string>
#include <utility>
#include "TCanvas.h"
#include "TROOT.h"
#include "TChain.h"
//#include "makeplotsnice.h"
#include "TtZAnalysis/Tools/interface/container.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "TopAnalysis/ZTopUtils/interface/PUReweighter.h"
#include <map>
#include <stdlib.h>
#include "histoStyle.h"

#include "TtZAnalysis/Tools/interface/effTriple.h"
#include "TtZAnalysis/Tools/interface/namedPairs.h" 


using namespace ztop;
namespace top{using namespace ztop;}
using namespace std;


//ratiomulti affects plots AND syst error in tables
// NOOOOO relative paths!!

double ratiomultiplier=0;

double jetptcut=30;

TChain * makeChain(std::vector<TString> paths){

	TChain * chain = new TChain("PFTree/PFTree");

	for(std::vector<TString>::iterator path=paths.begin();path<paths.end();++path){
		int h=chain->AddFile((*path));
		std::cout << h << " added " << *path << " to chain."<< std::endl;
	}

	return chain;
}

TChain * makeChain(TString path){
	TChain * chain = new TChain(path);

	chain->Add(path+"/PFTree/PFTree");
	std::cout << "added " << path << " to chain." << std::endl;
	return chain;
}


class triggerAnalyzer{

public:
	triggerAnalyzer(){
		binseta_.push_back(-2.5);binseta_.push_back(-1.5);binseta_.push_back(-0.8);binseta_.push_back(0.8);binseta_.push_back(1.5);binseta_.push_back(2.5); // ee standard
		binspt_ << 20 << 25 << 30 << 35 << 40 << 50 << 60 << 100 << 200;
		binseta2dx_ << 0 << 0.9 << 1.2 << 2.1 << 2.4; //mu standard
		binseta2dy_=binseta2dx_;
		whichelectrons_="NTElectrons";

		checktriggerpaths_=false;
		coutalltriggerpaths_=false;
		TH1::AddDirectory(kFALSE);
		mode_=0;
		isMC_=false;

		statusbar_=false;
		prescaleCutHigh_=20;

		includecorr_=true;
		masscutlow_=20;
		masscuthigh_=0;

		match_=false;

		runcutlow_=0;
		runcuthigh_=0;
		prescaleweight_=true;
		selectionstep_=0;

		globalDen_=0;

		tempp0_=0;
		tempp1_=0;
		tempp2_=0;
		tempp3_=0;

		mcpufile_="";
		t_=0;
		skip=false;
		//  std::cout << "Don't forget to check if all trigger Prescales are 1. If they are its either MC or an error in prescaletable. then use the one from event before or skip event - maybe checking one known prescale is sufficient" << std::endl;

	}
	~triggerAnalyzer(){};

	//virtual
	double selectDileptons(std::vector<ztop::NTMuon> * , std::vector<ztop::NTElectron> * );
	//! fills selectedElecs_ and std::vector<ztop::NTMuon* > selectedMuons_

	void setElectrons(TString elecs){whichelectrons_=elecs;}

	void setDisplayStatus(bool disp){statusbar_=disp;}

	void setBinsEta(std::vector<float> binseta){binseta_.clear();binseta_=binseta;};
	void setBinsEta2dX(std::vector<float> binseta2dx){binseta2dx_.clear();binseta2dx_=binseta2dx;};
	void setBinsEta2dY(std::vector<float> binseta2dy){binseta2dy_.clear();binseta2dy_=binseta2dy;};
	void setBinsPt(std::vector<float> binspt){binspt_.clear();binspt_=binspt;}

	std::vector<ztop::effTriple> getTriples(){return alltriples_;}

	void checkTriggerPaths(bool check){checktriggerpaths_=check;}

	void setPrescaleCut(unsigned int cut){prescaleCutHigh_=cut;}
	void setChain(TChain * t1){t_=t1;}
	void setChain(std::vector<TString> data){TChain * c=makeChain(data);setChain(c);}

	void setIsMC(bool isMonteCarlo){isMC_=isMonteCarlo;}

	void setMode(TString mode){if(mode=="ee") mode_=-1;if(mode=="emu") mode_=0;if(mode=="mumu") mode_=1;}

	TString getModeS(){if(mode_==-1) return "ee";if(mode_==0) return "emu";else return "mumu";}

	void setPUFile(TString file){pufile_=file;}
	void setMCPUFile(TString file){mcpufile_=file;}

	void setIncludeCorr(bool inc){includecorr_=inc;}

	void setMassCutLow(double cut){masscutlow_=cut;}
	void setMassCutHigh(double cut){masscuthigh_=cut;}

	void setRunCutLow(double cut){runcutlow_=cut;}
	void setRunCutHigh(double cut){runcuthigh_=cut;}

	void setDileptonTriggers(std::vector<std::string> trigs){trigs_=trigs;}
	void setDileptonTriggersMC(std::vector<std::string> trigs){trigsMC_=trigs;}
	void setDileptonTrigger(string trig){trigs_.clear(); trigs_.push_back(trig);}

	void setTriggerObjects(std::vector<std::string> trigobjs){trigsObj_=trigobjs;}
	void setTriggerObjects(std::string trigobj){trigsObj_.clear();trigsObj_.push_back(trigobj);}
	void addTriggerObjects(std::string trigobj){trigsObj_.push_back(trigobj);}

	void setSelectionStep(int step){/*selectionstep_=step; for future use. needs to affect the plots, too*/ }
	int selectionStep(){return selectionstep_;}

	void setUseMatching(bool match){match_=match;}

	void setUsePrescaleWeight(bool doit){prescaleweight_=doit;}

	double getGlobalDen(){return globalDen_;}

	std::vector<double> getGlobalOutput(){return storedOut_;}

	void scale(double SF){
		for(size_t i=0;i<alltriples_.size();i++) alltriples_.at(i).scale(SF);
		for(size_t i=0;i<storedOut_.size();i++) storedOut_.at(i) *= SF;
		globalDen_ *= SF;
	}

	triggerAnalyzer operator + (const triggerAnalyzer & second){
		triggerAnalyzer out = second;
		if(alltriples_.size() != second.alltriples_.size()){
			std::cout << "triggerAnalyzer operator +: alltriples must be same size!" << std::endl;
			return out;
		}
		if(isMC_)
			histWrapper::addStatUncorr=false;
		else
			histWrapper::addStatUncorr=true;
		for(size_t i=0;i<alltriples_.size();i++) out.alltriples_.at(i) = out.alltriples_.at(i) + alltriples_.at(i);
		for(size_t i=0;i<storedOut_.size();i++) out.storedOut_.at(i) += storedOut_.at(i);
		out.globalDen_ += globalDen_;

		return out;
	}

	//////////////////////

private:
	std::vector<string> mettriggers_;

public:
	bool skip;
	void setMetTriggers(std::vector<string>  in){mettriggers_=in;}

	std::vector<double> Eff(){

		using namespace ztop;
		using namespace std;
		std::vector<string> mettriggers;
		if(mettriggers_.size()<1)
			mettriggers=initTriggers();
		else
			mettriggers=mettriggers_;

		TString MCadd="";
		if(isMC_) MCadd="MC";


		//define bins

		vector<float> binsdphi,binsvmulti,binsjetmulti,binsdrll;

		for(float i=0;i<=10;i++){
			binsdphi.push_back(2*3.1415 * i/10);
		}
		for(float i=0.5;i<=30.5;i++) binsvmulti << i;
		for(float i=-0.5;i<=7.5;i++) binsjetmulti << i;
		for(float i=0 ;i<15 ;i++) binsdrll << 2 * i /15;


		vector<float> binsallpt;
		binsallpt << 0 << 3 << 5 << 8 << 15 << 20 << 25 << 30 << 40 << 50 << 60 << 80 << 120 << 200 << 400;

		// in addition binseta_ binseta2dx_ binseta2dy_ binspt_

		vector<float> binsmass;

		for(float i=0;i<300;i++){
			float bin=0;
			if(mode_>0) bin=i*3; //more bins for mumu
			else bin=i*30;

			if(bin>=300) break;
			binsmass << bin;
		}

		vector<float> binslepmulti;
		binslepmulti << 1.5 << 2.5 << 3.5 << 4.5 << 5.5;
		vector<float> binsetafine;
		binsetafine << -2.4 << -2.1 << -1.7 << -1.2 << -0.9 << -0.6 << -0.3 << -0.1 << 0.1 << 0.3 << 0.6 << 0.9 << 1.2 << 1.7 << 2.1 << 2.4;

		vector<float> binseta2dfineX,binseta2dfineY;
		binseta2dfineX << 0 << 0.1 << 0.3 << 0.6 << 0.9 << 1.2 << 1.7 << 2.4;
		binseta2dfineY=binseta2dfineX;

		vector<float> binseta2jetmultiX,binseta2jetmultiY;
		binseta2jetmultiX << -0.5 << 0.5 << 1.5 << 2.5 << 3.5 << 4.5 << 5.5;
		binseta2jetmultiY << 0 << 0.9 << 1.2 << 2.1 << 2.4;

		vector<float> binsiso;
		for(float i=0;i<20;i++)
			binsiso << i/100;

		vector<float> selectionbins; selectionbins << -0.5 << 0.5 << 1.5 << 2.5 << 3.5 << 4.5;

		vector<float> dzbins;
		for(float i=0;i<40;i++)
			dzbins << i;
		vector<float> dzbins2;
		//for(float i=0;i<10;i++)
		dzbins2 << 0 << 0.025 << 0.05 << 0.3  << 1 << 2 << 3 << 4;

		vector<float> binsdrtrig;
		binsdrtrig << 0 << 0.05 << 0.1 << 0.15 << 0.2 << 0.3 << 0.4 << 0.6 << 1 << 2 << 5;

		//pt
		//eta
		//et2d
		//dphi
		//vmulti
		//drlep
		//jetmulti
		////jetmultieta2d


		alltriples_.clear();
		std::cout << "setting plots "<< std::endl;
		std::cout << effTriple::effTriple_list.size() << std::endl;

		effTriple::makelist=true;

		/////////PLOTS///////

		effTriple t_global  (selectionbins           , "global"         , "sel step"                    , "evts"   );
		effTriple t_global_woCorr  (selectionbins           , "global_woCorr"         , "sel step"                    , "evts"   );

		///don't change!! global needs to be the first/second entry!!

		effTriple t_pt      (binspt_                 , "lepton_pt"      , "p_{T,l} [GeV]"            , "evts"   );
		effTriple t_allpt   (binsallpt               , "all_lepton_pt"  , "p_{T,l} [GeV]"            , "evts"   );
		effTriple t_eta     (binseta_                , "lepton_eta"     , "#eta_{l}"           , "evts"   );
		effTriple t_eta2d   (binseta2dx_, binseta2dy_, "lepton_eta2d"   , "#eta_{l_{1}}"       , "#eta_{l_{2}}");

		///leading lep pt

		effTriple t_eta2dpt2030_2030   (binseta2dx_, binseta2dy_, "lepton_eta2d_pt2030_2030"   , "#eta_{l_{1}}"       , "#eta_{l_{2}}");
		effTriple t_eta2dpt2030_3040   (binseta2dx_, binseta2dy_, "lepton_eta2d_pt2030_3040"   , "#eta_{l_{1}}"       , "#eta_{l_{2}}");
		effTriple t_eta2dpt2030_4060   (binseta2dx_, binseta2dy_, "lepton_eta2d_pt2030_4060"   , "#eta_{l_{1}}"       , "#eta_{l_{2}}");
		effTriple t_eta2dpt2030_60     (binseta2dx_, binseta2dy_, "lepton_eta2d_pt2030_60"     , "#eta_{l_{1}}"       , "#eta_{l_{2}}");
		effTriple t_eta2dpt3040_2030   (binseta2dx_, binseta2dy_, "lepton_eta2d_pt3040_2030"   , "#eta_{l_{1}}"       , "#eta_{l_{2}}");
		effTriple t_eta2dpt3040_3040   (binseta2dx_, binseta2dy_, "lepton_eta2d_pt3040_3040"   , "#eta_{l_{1}}"       , "#eta_{l_{2}}");
		effTriple t_eta2dpt3040_4060   (binseta2dx_, binseta2dy_, "lepton_eta2d_pt3040_4060"   , "#eta_{l_{1}}"       , "#eta_{l_{2}}");
		effTriple t_eta2dpt3040_60     (binseta2dx_, binseta2dy_, "lepton_eta2d_pt3040_60"     , "#eta_{l_{1}}"       , "#eta_{l_{2}}");
		effTriple t_eta2dpt4060_2030   (binseta2dx_, binseta2dy_, "lepton_eta2d_pt4060_2030"   , "#eta_{l_{1}}"       , "#eta_{l_{2}}");
		effTriple t_eta2dpt4060_3040   (binseta2dx_, binseta2dy_, "lepton_eta2d_pt4060_3040"   , "#eta_{l_{1}}"       , "#eta_{l_{2}}");
		effTriple t_eta2dpt4060_4060   (binseta2dx_, binseta2dy_, "lepton_eta2d_pt4060_4060"   , "#eta_{l_{1}}"       , "#eta_{l_{2}}");
		effTriple t_eta2dpt4060_60     (binseta2dx_, binseta2dy_, "lepton_eta2d_pt4060_60"     , "#eta_{l_{1}}"       , "#eta_{l_{2}}");
		effTriple t_eta2dpt60_2030     (binseta2dx_, binseta2dy_, "lepton_eta2d_pt60_2030"     , "#eta_{l_{1}}"       , "#eta_{l_{2}}");
		effTriple t_eta2dpt60_3040     (binseta2dx_, binseta2dy_, "lepton_eta2d_pt60_3040"     , "#eta_{l_{1}}"       , "#eta_{l_{2}}");
		effTriple t_eta2dpt60_4060     (binseta2dx_, binseta2dy_, "lepton_eta2d_pt60_4060"     , "#eta_{l_{1}}"       , "#eta_{l_{2}}");
		effTriple t_eta2dpt60_60       (binseta2dx_, binseta2dy_, "lepton_eta2d_pt60_60"       , "#eta_{l_{1}}"       , "#eta_{l_{2}}");
                
                effTriple t_met     (binspt_,"MET","MET","evts");

		effTriple t_dphi    (binsdphi                , "leptonmet_dphi" , "#Delta#phi_{l,MET}" , "evts"   );
		effTriple t_vmulti  (binsvmulti              , "vertex_multi"   , "n_{vtx}"            , "evts"   );
		effTriple t_drll    (binsdrll                , "leptons_dR"     , "#Delta R_{l,l}"     , "evts"   );
		effTriple t_jetmulti(binsjetmulti            , "jet_multi"      , "n_{jets}"           , "evts"   );

		effTriple t_invmass (binsmass            , "dilepton_mll"      , "m_{ll} [GeV]"          , "evts"   );
		effTriple t_lepmulti(binslepmulti            , "lepton_multi"      , "n_{l}"           , "evts"   );
		effTriple t_alllepmulti(binslepmulti            , "alllepton_multi"      , "n_{l}"           , "evts"   );
		effTriple t_etafine     (binsetafine                , "lepton_etafine"     , "#eta_{l}"           , "evts"   );
		effTriple t_eta2jetmulti  (binseta2jetmultiX, binseta2jetmultiY, "leptonjet_eta2multi"   , "n_{jets}"       , "|#eta_{l}|");
		effTriple t_iso     (binsiso                , "lepton_iso"     , "Iso_{l}"           , "evts"   );
		effTriple t_eta2dfine   (binseta2dfineX, binseta2dfineY, "lepton_eta2dfine"   , "#eta_{l_{1}}"       , "#eta_{l_{2}}", "smallmarkers");
		effTriple t_dzlepton (dzbins2                 , "lepton_dzV"      , "dz(V)_{l} [cm]"            , "evts"   );
		effTriple t_dzbslepton (dzbins                 , "lepton_dzBs"      , "dz(Bs)_{l} [cm]"            , "evts"   );
		effTriple t_dzbsalllepton (dzbins2                 , "alllepton_dzBs"      , "dz(Bs)_{l} [cm]"            , "evts"   );
		effTriple t_dzleptonlepton (dzbins2                 , "leptonlepton_dz"      , "dz(ll) [cm]"            , "evts"   );
		effTriple t_drleptrig (binsdrtrig                 , "leptontrigger_dr"      , "dr(lt) [cm]"            , "evts"   );

		effTriple::makelist=false;

		//all this trigger stuff starts here

		TH1D *datapileup=0;
		if(isMC_){
			TFile f2(pufile_);
			datapileup=((TH1D*)f2.Get("pileup"));
		}

		vector<string> dileptriggers;

		//make triggers switchable from the outside in next iteration. leave right now as it is

		if(mode_<-0.1){
			//  pair<string, double> trig2("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v",999999.);
			dileptriggers.push_back("HLT_Ele23_Ele12_CaloId_TrackId_Iso_v");

		}
		if(mode_>0.1){
			//   pair<string, double> trig3("HLT_Mu17_Mu8_v",999999);
			dileptriggers.push_back("HLT_Mu17_Mu8_v");
			// pair<string, double> trig4("HLT_Mu17_TkMu8_v",999999);
			dileptriggers.push_back("HLT_Mu17_TkMu8_v");
		}

		if(mode_==0){
			//  pair<string, double> trig3("HLT_Mu17_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v",999999);
			dileptriggers.push_back("HLT_Mu23_TrkIsoVVL_Ele12_Gsf_CaloId_TrackId_Iso_MediumWP_v");
			//  pair<string, double> trig4("HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v",999999);
			dileptriggers.push_back("HLT_Mu8_TrkIsoVVL_Ele23_Gsf_CaloId_TrackId_Iso_MediumWP_v");
		}

		if(trigs_.size()>0){   //manually set
			dileptriggers.clear();
			for(size_t i=0;i<trigs_.size();i++){
				dileptriggers.push_back(trigs_.at(i));
			}
		}


		vector<string> dileptriggersMC; // the version numbers where set as wildcards, so if statement obsolete!

		if(mode_<-0.1){
			dileptriggersMC.push_back("HLT_Ele23_Ele12_CaloId_TrackId_Iso_v");
		}
		if(mode_>0.1){
			dileptriggersMC.push_back("HLT_Mu17_Mu8_v");
			dileptriggersMC.push_back("HLT_Mu17_TkMu8_v");
		}
		if(mode_==0){
			dileptriggersMC.push_back("HLT_Mu23_TrkIsoVVL_Ele12_Gsf_CaloId_TrackId_Iso_MediumWP_v");
			dileptriggersMC.push_back("HLT_Mu8_TrkIsoVVL_Ele23_Gsf_CaloId_TrackId_Iso_MediumWP_v");
		}
		if(trigsMC_.size()>0){   //manually set
			dileptriggersMC.clear();
			for(size_t i=0;i<trigsMC_.size();i++){
				dileptriggersMC.push_back(trigsMC_.at(i));
			}
		}

		///////////PU reweighting

		PUReweighter PUweight;
		//if(isMC_) PUweight.setDataTruePUInput(datapileup);
		if(isMC_ && mcpufile_==""){
			PUweight.setMCDistrSum12();
		}
		else if(isMC_ && mcpufile_!=""){
			if(mcpufile_=="Summer11_Leg")
				PUweight.setMCDistrSummer11Leg();
			else
				PUweight.setMCTruePUInput(mcpufile_.Data());
		}

		vector<NTMuon> * pMuons = 0;
		t_->SetBranchAddress("NTMuons",&pMuons);//, &b_NTMuons);
		vector<NTElectron> * pElectrons = 0;
		t_->SetBranchAddress(whichelectrons_.Data(),&pElectrons);//, &b_NTElectrons);
		vector<NTJet> * pJets = 0;
		t_->SetBranchAddress("NTJets",&pJets);
		NTMet * pMet = 0;
		t_->SetBranchAddress("NTMet",&pMet);
		NTEvent * pEvent = 0;
		t_->SetBranchAddress("NTEvent",&pEvent);
		map<string,unsigned int> * pTriggersWithPrescales=0;
		t_->SetBranchAddress("AllTriggersWithPrescales",&pTriggersWithPrescales);

		vector<NTTriggerObject> * pTriggerobjects[]={0,0,0,0,0,0,0,0,0,0};
		const size_t pTriggerobjectsSize=10;

		/*pTriggerobjects.clear();
		for(size_t i=0;i<trigsObj_.size();i++){
			vector<NTTriggerObject> * temp = 0;
			pTriggerobjects.push_back(temp);
		}*/
		vector<size_t> invalidbranches;
		for(size_t i=0;i<trigsObj_.size();i++){
			if(i>=10){
				std::cout << "Branch adress: " << "NTTriggerObjects_"+(TString)trigsObj_.at(i) << " will not be set, exceeds limit of 10" << std::endl;
			}
			if(t_->GetBranch(("NTTriggerObjects_"+(TString)trigsObj_.at(i)))){
				t_->SetBranchAddress("NTTriggerObjects_"+(TString)trigsObj_.at(i),&pTriggerobjects[i]);
				if(testmode) std::cout << "Branch adress: " << "NTTriggerObjects_"+(TString)trigsObj_.at(i) << " set" << std::endl;
			}
			else{
				cout << "Branch " << "NTTriggerObjects_"+(TString)trigsObj_.at(i) 
				    																				 << " not found\n    will be ignored" << endl;
				invalidbranches << i;
			}
		}

		std::vector<double> * pStopMass=0;
		std::vector<double> * pChiMass=0;
		//try to get SUSY branches
		if(t_->GetBranch("StopMass")){
			t_->SetBranchAddress("StopMass",&pStopMass);
			t_->SetBranchAddress("ChiMass",&pChiMass);
		}


		pair<string,double> dilepton("dilepton", 0);
		pair<string,double> ZVeto   ("ZVeto   ", 0);
		pair<string,double> oneJet  ("oneJet  ", 0);
		pair<string,double> twoJets ("twoJets ", 0);
		pair<string,double> met     ("met     ", 0);


		vector<pair<string,double> >sel_woTrig;
		vector<pair<string,double> >sel_Trig;
		vector<pair<string,double> >sel_MetTrig;
		vector<pair<string,double> >sel_BothTrig;

		sel_woTrig.push_back(dilepton);
		sel_woTrig.push_back(ZVeto);
		sel_woTrig.push_back(oneJet);
		sel_woTrig.push_back(twoJets);
		sel_woTrig.push_back(met);

		sel_Trig.push_back(dilepton);
		sel_Trig.push_back(ZVeto);
		sel_Trig.push_back(oneJet);
		sel_Trig.push_back(twoJets);
		sel_Trig.push_back(met);

		sel_MetTrig.push_back(dilepton);
		sel_MetTrig.push_back(ZVeto);
		sel_MetTrig.push_back(oneJet);
		sel_MetTrig.push_back(twoJets);
		sel_MetTrig.push_back(met);

		sel_BothTrig.push_back(dilepton);
		sel_BothTrig.push_back(ZVeto);
		sel_BothTrig.push_back(oneJet);
		sel_BothTrig.push_back(twoJets);
		sel_BothTrig.push_back(met);


		unsigned int wrongtables=0;
		namedPairs summarylist,summarylistweighted;


		Long64_t n = t_->GetEntries();
		cout  << "Entries in tree: " << n ;


		if(testmode){
			n*=0.01;
			cout  << " reduced to: " << n;
		}
		if(lowMCStat && isMC_){
			n*=0.01;
			cout  << " reduced to: " << n;
		}
		if(lowDataStat && !isMC_){
			n*=0.001;
			cout  << " reduced to: " << n;
		}
		cout << std::endl;
		if(skip)
			n=1;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if(testmode) std::cout << "entering main loop" << std::endl;
		for(Long64_t i=0;i<n;i++){  //main loop

			selectedElecs_.clear();
			selectedMuons_.clear();

			bool firedDilepTrigger=false;
			bool firedMet=false;
			bool b_dilepton=false;
			bool b_ZVeto=false;
			bool b_oneJet=false;
			bool b_twoJets=false;
			bool b_met=false;


			t_->GetEntry(i);

			tempp0_=pStopMass;
			tempp1_=pChiMass;

			if(testmode && i<2)
				std::cout << "got entry "<<i << std::endl;

			if(statusbar_)
				displayStatusBar(i,n,10);


			double puweight=1;
			if(isMC_) puweight=PUweight.getPUweight(pEvent->truePU());
			//puweight=1;


			if(mode_>0.1 && pMuons->size()<2) continue;

			if(mode_<-0.1 && pElectrons->size()<2) continue;

			if(mode_==0 &&  (pElectrons->size() < 1 || pMuons->size()< 1)) continue;


			//select dileptons
			double mass=selectDileptons(pMuons,pElectrons);


			//cut on run ranges
			if(runcutlow_ >0  && pEvent->runNo() < runcutlow_ ) continue;
			if(runcuthigh_>0 && pEvent->runNo() > runcuthigh_) continue;

			//cut on mass ranges
			if(masscutlow_ >0  && mass < masscutlow_ ) continue;
			if(masscuthigh_>0 && mass > masscuthigh_) continue;




			b_dilepton=true;


			//check whether dilepton trigger has fired

			for(std::map<std::string, unsigned int>::iterator trigP=pTriggersWithPrescales->begin();trigP!=pTriggersWithPrescales->end();++trigP){
				const string * name= &(trigP->first);

				/*if(std::find(dileptriggers.begin(),dileptriggers.end(),*name) !=  dileptriggers.end())
					firedDilepTrigger=true; */
				//check dilepton trigger

				for(unsigned int ctrig=0;ctrig<dileptriggers.size(); ctrig++){
					if(name->find(dileptriggers.at(ctrig))!=string::npos && (name->size()-dileptriggers.at(ctrig).size()) < 5){ //allow something like "_v10"
						//if(*name == dileptriggers.at(ctrig)){
						firedDilepTrigger=true;
						break;
					}
				}

			}

			//check met trigger and use the one with lowest prescale


			unsigned int prescale=99999;
			unsigned int largestprescale=0;

			vector<size_t> firedidx;

			for(size_t fg=0;fg<mettriggers.size();fg++){
				string * name= &(mettriggers.at(fg));

				size_t tmpidx=0;
				for(std::map<std::string, unsigned int>::iterator trigP=pTriggersWithPrescales->begin();trigP!=pTriggersWithPrescales->end();++trigP){
					if(trigP->first.find(*name) != std::string::npos){

						firedMet=true; //any trigger fired

						firedidx.push_back(tmpidx);

						if(trigP->second < prescale)
							prescale=trigP->second;
						if(trigP->second > largestprescale)
							largestprescale=trigP->second;

					}
					tmpidx++;
				}
			}

			if(!firedMet)
				prescale = 1;

			if(largestprescale < 2 && !isMC_)
				wrongtables++;

			if(prescaleweight_){

				if(prescale > prescaleCutHigh_) continue; //get rid of high prescales

				puweight*=(double) prescale;
			}

			std::map<std::string, unsigned int>::iterator tit=pTriggersWithPrescales->begin();

			for(size_t k=0;k<firedidx.size();k++){
				tit=pTriggersWithPrescales->begin();
				std::advance(tit, firedidx.at(k));
				if(tit->second != prescale)
					continue;
				summarylist.fill(tit->first, tit->second);
				summarylistweighted.fill(tit->first, tit->second, puweight);
			}

			//////////trigger check ends

			//do matching on selectedMuons_ selectedElecs

			if(match_){
				firedDilepTrigger=false;

			}




			if(!isMC_ && !firedMet) continue;


			t_global_woCorr.fillDen(0,puweight);
			if(firedDilepTrigger)
				t_global_woCorr.fillNum(0,puweight);


			if(includecorr_ && !firedMet) continue;
			// lepton selection and event selection (at least two leptons etc)

			if(pEvent->vertexMulti() <1) continue;


			/////////trigger check/////////

			////////////////dilepton selection
			if((mode_== 1 || mode_<-0.1 ) && (mass > 106 || mass < 76)) b_ZVeto=true;
			if(mode_==0)  b_ZVeto=true;

			vector<NTJet> selected_jets;
			for(vector<NTJet>::iterator jet=pJets->begin();jet<pJets->end();jet++){
				if(jet->pt() < jetptcut) continue;
				if(fabs(jet->eta()) >2.5) continue;
				if(!noOverlap(&*jet,selectedMuons_,0.3)) continue; //cleaning  ##TRAP## changed to 0.4 /doesn't matter for final eff
				if(!noOverlap(&*jet,selectedElecs_,0.3)) continue;
				if((!jet->id())) continue;

				selected_jets.push_back(*jet);
			}
			if(selected_jets.size() >0) b_oneJet=true;
			if(selected_jets.size() >1) b_twoJets=true;
			if((mode_== -1 || mode_>0.1) && pMet->met() > 40) b_met=true;
			if(mode_== 0) b_met=true;


			vector<NTTriggerObject *> alltrigobj;
			/////// make trigger object collections ////
			for(size_t to=0;to<pTriggerobjectsSize;to++){ //merge collections
				if(pTriggerobjects[to]){
					for(size_t j=0;j<pTriggerobjects[to]->size();j++){
						alltrigobj.push_back(&pTriggerobjects[to]->at(j));
					}
				}
			}
			//  std::cout << puweight << std::endl;

			//define variables

			double vmulti=pEvent->vertexMulti();
			double jetmulti=selected_jets.size();
			double pt1,pt2;
			double eta1,eta2;
			double dphi;
			double dRll;
			double lepmulti;
			double alllepmulti;
			double iso1;
			double iso2;
			double dz1,dz2,dzbs1,dzbs2;
			double dRlt1,dRlt2;
                        double met;

			//mass already defined


			/////////filling stuff
			//

                        met=pMet->met();

			if(mode_<-0.1){ //ee
				eta1=selectedElecs_[0]->eta();
				eta2=selectedElecs_[1]->eta();
				pt1=selectedElecs_[0]->ECalP4().Pt();
				pt2=selectedElecs_[1]->ECalP4().Pt();
				dphi=selectedElecs_[0]->phi() - pMet->phi();
				dRll=dR(selectedElecs_[0],selectedElecs_[1]);
				lepmulti=selectedElecs_.size();
				alllepmulti=pElectrons->size();
				iso1=selectedElecs_[0]->isoVal();
				iso2=selectedElecs_[1]->isoVal();
				dz1=selectedElecs_[0]->dzV();
				dz2=selectedElecs_[1]->dzV();
				dzbs1=selectedElecs_[0]->dZBs();
				dzbs2=selectedElecs_[1]->dZBs();
			}
			else if(mode_>0.1){ //mumu
				eta1=selectedMuons_[0]->eta();
				eta2=selectedMuons_[1]->eta();
				pt1=selectedMuons_[0]->pt();
				pt2=selectedMuons_[1]->pt();
				dphi=selectedMuons_[0]->phi() - pMet->phi();
				dRll=dR(selectedMuons_[0],selectedMuons_[1]);
				lepmulti=selectedMuons_.size();
				alllepmulti=pMuons->size();
				iso1=selectedMuons_[0]->isoVal();
				iso2=selectedMuons_[1]->isoVal();
				dz1=selectedMuons_[0]->dzV();
				dz2=selectedMuons_[1]->dzV();
				dzbs1=selectedMuons_[0]->dZBs();
				dzbs2=selectedMuons_[1]->dZBs();
				dRlt1=0;dRlt2=0;
				getClosestInDR(selectedMuons_[0],alltrigobj,dRlt1);
				getClosestInDR(selectedMuons_[1],alltrigobj,dRlt2);
			}
			else{ //emu
				eta1=selectedElecs_[0]->eta();
				eta2=selectedMuons_[0]->eta();
				pt1=selectedElecs_[0]->ECalP4().Pt();
				pt2=selectedMuons_[0]->pt();
				dphi=selectedElecs_[0]->phi() - pMet->phi();
				dRll=dR(selectedElecs_[0],selectedMuons_[0]);
				lepmulti=selectedElecs_.size()+selectedMuons_.size();
				alllepmulti=pMuons->size() + pElectrons->size();
				iso1=selectedElecs_[0]->isoVal();
				iso2=selectedMuons_[0]->isoVal();
				dz1=selectedMuons_[0]->dzV();
				dz2=selectedElecs_[0]->dzV();
				dzbs1=selectedElecs_[0]->dZBs();
				dzbs2=selectedMuons_[0]->dZBs();
			}

			if(b_dilepton){// && b_twoJets){
				sel_woTrig[0].second      +=puweight; //global
				t_global.fillDen(0,puweight);

				t_pt.fillDen(pt1,puweight);
				t_pt.fillDen(pt2,puweight);

				//	t_allpt.fillDen();
				//	t_allpt.fillDen();


				if(pt1 <30){
					if(pt2<30)
						t_eta2dpt2030_2030.fillDen(fabs(eta1),fabs(eta2),puweight);
					else if(pt2<40)
						t_eta2dpt2030_3040.fillDen(fabs(eta1),fabs(eta2),puweight);
					else if(pt2<60)
						t_eta2dpt2030_4060.fillDen(fabs(eta1),fabs(eta2),puweight);
					else
						t_eta2dpt2030_60.fillDen(fabs(eta1),fabs(eta2),puweight);
				}
				else if(pt1<40){
					if(pt2<30)
						t_eta2dpt3040_2030.fillDen(fabs(eta1),fabs(eta2),puweight);
					else if(pt2<40)
						t_eta2dpt3040_3040.fillDen(fabs(eta1),fabs(eta2),puweight);
					else if(pt2<60)
						t_eta2dpt3040_4060.fillDen(fabs(eta1),fabs(eta2),puweight);
					else
						t_eta2dpt3040_60.fillDen(fabs(eta1),fabs(eta2),puweight);
				}
				else if(pt1<60){
					if(pt2<30)
						t_eta2dpt4060_2030.fillDen(fabs(eta1),fabs(eta2),puweight);
					else if(pt2<40)
						t_eta2dpt4060_3040.fillDen(fabs(eta1),fabs(eta2),puweight);
					else if(pt2<60)
						t_eta2dpt4060_4060.fillDen(fabs(eta1),fabs(eta2),puweight);
					else
						t_eta2dpt4060_60.fillDen(fabs(eta1),fabs(eta2),puweight);
				}
				else{
					if(pt2<30)
						t_eta2dpt60_2030.fillDen(fabs(eta1),fabs(eta2),puweight);
					else if(pt2<40)
						t_eta2dpt60_3040.fillDen(fabs(eta1),fabs(eta2),puweight);
					else if(pt2<60)
						t_eta2dpt60_4060.fillDen(fabs(eta1),fabs(eta2),puweight);
					else
						t_eta2dpt60_60.fillDen(fabs(eta1),fabs(eta2),puweight);
				}


				t_eta .fillDen(eta1,puweight);
				t_eta .fillDen(eta2,puweight);

				t_eta2d.fillDen(fabs(eta1),fabs(eta2),puweight);
				t_dphi.fillDen(dphi,puweight);
				t_vmulti.fillDen(vmulti,puweight);
				t_drll.fillDen(dRll,puweight);
				t_jetmulti.fillDen(jetmulti,puweight);

				t_invmass.fillDen(mass,puweight);
				t_lepmulti.fillDen(lepmulti,puweight);
				t_alllepmulti.fillDen(alllepmulti,puweight);
				t_etafine.fillDen(eta1,puweight);
				t_etafine.fillDen(eta2,puweight);
				t_eta2jetmulti.fillDen(jetmulti,fabs(eta1),puweight);
				t_eta2jetmulti.fillDen(jetmulti,fabs(eta2),puweight);
				t_iso.fillDen(iso1,puweight);
				t_iso.fillDen(iso2,puweight);
				t_eta2dfine.fillDen(fabs(eta1),fabs(eta2),puweight);
                                   
                                t_met.fillDen(met);
                                  
				t_dzlepton.fillDen(fabs(dz1),puweight);
				t_dzlepton.fillDen(fabs(dz2),puweight);

				t_dzbslepton.fillDen(fabs(dzbs1),puweight);
				t_dzbslepton.fillDen(fabs(dzbs2),puweight);

				t_dzleptonlepton.fillDen(fabs(dz1-dz2),puweight);

				if(mode_ > 0.1){
					for(size_t j=0;j<pMuons->size();j++){
						t_allpt.fillDen(pMuons->at(j).pt(),puweight);
						t_dzbsalllepton.fillDen(fabs(pMuons->at(j).dZBs()),puweight);
					}
				}
				t_drleptrig.fillDen(dRlt1,puweight);
				t_drleptrig.fillDen(dRlt2,puweight);


			}
			if(b_dilepton && b_ZVeto)                                   {sel_woTrig[1].second      +=puweight; t_global.fillDen(1,puweight);}
			if(b_dilepton && b_ZVeto && b_oneJet)                       {sel_woTrig[2].second      +=puweight; t_global.fillDen(2,puweight);}
			if(b_dilepton && b_ZVeto && b_oneJet && b_twoJets)          {sel_woTrig[3].second      +=puweight; t_global.fillDen(3,puweight);}
			if(b_dilepton && b_ZVeto && b_oneJet && b_twoJets && b_met) {sel_woTrig[4].second      +=puweight; t_global.fillDen(4,puweight);}

			if(firedDilepTrigger){
				if(b_dilepton){// && b_twoJets){
					sel_Trig[0].second  +=puweight;
					t_global.fillNum(0,puweight);

					t_pt.fillNum(pt1,puweight);
					t_pt.fillNum(pt2,puweight);

					//	t_allpt.fillNum();
					//	t_allpt.fillNum();

					if(pt1 <30){
						if(pt2<30)
							t_eta2dpt2030_2030.fillNum(fabs(eta1),fabs(eta2),puweight);
						else if(pt2<40)
							t_eta2dpt2030_3040.fillNum(fabs(eta1),fabs(eta2),puweight);
						else if(pt2<60)
							t_eta2dpt2030_4060.fillNum(fabs(eta1),fabs(eta2),puweight);
						else
							t_eta2dpt2030_60.fillNum(fabs(eta1),fabs(eta2),puweight);
					}
					else if(pt1<40){
						if(pt2<30)
							t_eta2dpt3040_2030.fillNum(fabs(eta1),fabs(eta2),puweight);
						else if(pt2<40)
							t_eta2dpt3040_3040.fillNum(fabs(eta1),fabs(eta2),puweight);
						else if(pt2<60)
							t_eta2dpt3040_4060.fillNum(fabs(eta1),fabs(eta2),puweight);
						else
							t_eta2dpt3040_60.fillNum(fabs(eta1),fabs(eta2),puweight);
					}
					else if(pt1<60){
						if(pt2<30)
							t_eta2dpt4060_2030.fillNum(fabs(eta1),fabs(eta2),puweight);
						else if(pt2<40)
							t_eta2dpt4060_3040.fillNum(fabs(eta1),fabs(eta2),puweight);
						else if(pt2<60)
							t_eta2dpt4060_4060.fillNum(fabs(eta1),fabs(eta2),puweight);
						else
							t_eta2dpt4060_60.fillNum(fabs(eta1),fabs(eta2),puweight);
					}
					else{
						if(pt2<30)
							t_eta2dpt60_2030.fillNum(fabs(eta1),fabs(eta2),puweight);
						else if(pt2<40)
							t_eta2dpt60_3040.fillNum(fabs(eta1),fabs(eta2),puweight);
						else if(pt2<60)
							t_eta2dpt60_4060.fillNum(fabs(eta1),fabs(eta2),puweight);
						else
							t_eta2dpt60_60.fillNum(fabs(eta1),fabs(eta2),puweight);
					}


					t_eta .fillNum(eta1,puweight);
					t_eta .fillNum(eta2,puweight);

					t_eta2d.fillNum(fabs(eta1),fabs(eta2),puweight);
					t_dphi.fillNum(dphi,puweight);
					t_vmulti.fillNum(vmulti,puweight);
					t_drll.fillNum(dRll,puweight);
					t_jetmulti.fillNum(jetmulti,puweight);


					t_invmass.fillNum(mass,puweight);
					t_lepmulti.fillNum(lepmulti,puweight);
					t_alllepmulti.fillNum(alllepmulti,puweight);
					t_etafine.fillNum(eta1,puweight);
					t_etafine.fillNum(eta2,puweight);
					t_eta2jetmulti.fillNum(jetmulti,fabs(eta1),puweight);
					t_eta2jetmulti.fillNum(jetmulti,fabs(eta2),puweight);
					t_iso.fillNum(iso1,puweight);
					t_iso.fillNum(iso2,puweight);
					t_eta2dfine.fillNum(fabs(eta1),fabs(eta2),puweight);

                                        t_met.fillNum(met);                                       
 
					t_dzlepton.fillNum(fabs(dz1),puweight);
					t_dzlepton.fillNum(fabs(dz2),puweight);

					t_dzbslepton.fillNum(fabs(dzbs1),puweight);
					t_dzbslepton.fillNum(fabs(dzbs2),puweight);

					t_dzleptonlepton.fillNum(fabs(dz1-dz2),puweight);

					if(mode_ > 0.1){
						for(size_t j=0;j<pMuons->size();j++){
							t_allpt.fillNum(pMuons->at(j).pt(),puweight);
							t_dzbsalllepton.fillNum(fabs(pMuons->at(j).dZBs()),puweight);
						}
					}

					t_drleptrig.fillNum(dRlt1,puweight);
					t_drleptrig.fillNum(dRlt2,puweight);

				}



				if(b_dilepton && b_ZVeto)                                   {sel_Trig[1].second    +=puweight; t_global.fillNum(1,puweight);}
				if(b_dilepton && b_ZVeto && b_oneJet)                       {sel_Trig[2].second    +=puweight; t_global.fillNum(2,puweight);}
				if(b_dilepton && b_ZVeto && b_oneJet && b_twoJets)          {sel_Trig[3].second    +=puweight; t_global.fillNum(3,puweight);}
				if(b_dilepton && b_ZVeto && b_oneJet && b_twoJets && b_met) {sel_Trig[4].second    +=puweight; t_global.fillNum(4,puweight);}
			}

			if(firedMet){
				if(b_dilepton){
					sel_MetTrig[0].second  +=puweight;
				}
				if(b_dilepton && b_ZVeto)                                   sel_MetTrig[1].second  +=puweight;
				if(b_dilepton && b_ZVeto && b_oneJet)                       sel_MetTrig[2].second +=puweight;
				if(b_dilepton && b_ZVeto && b_oneJet && b_twoJets)          sel_MetTrig[3].second +=puweight;
				if(b_dilepton && b_ZVeto && b_oneJet && b_twoJets && b_met) sel_MetTrig[4].second    +=puweight;
			}

			if(firedMet && firedDilepTrigger){
				if(b_dilepton){
					sel_BothTrig[0].second  +=puweight;
				}
				if(b_dilepton && b_ZVeto)                                   sel_BothTrig[1].second  +=puweight;
				if(b_dilepton && b_ZVeto && b_oneJet)                       sel_BothTrig[2].second +=puweight;
				if(b_dilepton && b_ZVeto && b_oneJet && b_twoJets)          sel_BothTrig[3].second +=puweight;
				if(b_dilepton && b_ZVeto && b_oneJet && b_twoJets && b_met) sel_BothTrig[4].second    +=puweight;
			}



		}//eventloop

		cout << endl;

		string MCdata="data";

		if(isMC_) MCdata="MC";
		cout << "\n\nIn channel " << mode_<< " (ee,emu,mumu)  " << MCdata << endl;
		cout << "triggers: " <<endl;
		if(!isMC_){
		for(unsigned int i=0;i<dileptriggers.size();i++){
			cout << dileptriggers[i] << endl;
		}
		}
		else{
			for(unsigned int i=0;i<dileptriggersMC.size();i++)
				cout << dileptriggersMC[i] << endl;
		}
		/*
    cout << "triggers MC: " <<endl;
    // if(!isMC_){
    for(unsigned int i=0;i<dileptriggersMC.size();i++){
      cout << dileptriggersMC[i] << endl;
    }
		 */
		// }

		cout << "probably wrong prescale tables: " << wrongtables << endl;


		// set up output
		vector<double> output;

		storedOut_.clear();
		for(unsigned int i=0; i< sel_woTrig.size();i++){

			storedOut_.push_back(sel_Trig[i].second);
			storedOut_.push_back(sel_woTrig[i].second);
			storedOut_.push_back(sel_MetTrig[i].second);
			storedOut_.push_back(sel_BothTrig[i].second);

			double eff=sel_Trig[i].second / sel_woTrig[i].second;
			double erreff=sqrt(eff*(1-eff)/sel_Trig[i].second);

			double ratio= (eff * sel_MetTrig[i].second/sel_woTrig[i].second ) /(sel_BothTrig[i].second/ sel_woTrig[i].second) -1;
			//double ratiostat=sel_BothTrig[i].second ;

			output.push_back(eff);
			output.push_back(erreff);
			output.push_back(ratio);

			cout << "selected " << sel_woTrig[i].first << ":  \t" << sel_woTrig[i].second << "  vs. \t" << sel_Trig[i].second << "  \teff: " << eff  << " +- " << erreff << "\tCorrR-1: " <<  ratio << "\t stat: " << sel_BothTrig[i].second  <<endl;
			//if(i==0) output=effdeffR;

		}
		//output=effdeffR;

		////////////////////////////////////check MC triggers

		/// show contribution of each trigger in data


		// cout << "\ncontribution of different triggers (unweighted!!):" << endl;
		//  summarylist.coutList();
		cout << "\ncontribution of different triggers (weighted!!). If several triggers fired, each gets an entry:" << endl;
		summarylistweighted.coutList();

		cout << "Total number of probably wrong prescale tables (may not be a problem, check cmssw output!): " << wrongtables << endl;


		if(checktriggerpaths_){



			///  triggersummary mettriggers
			/*
      int trigin=0;
      vector<unsigned int> notinc;
      for(unsigned int i=0; i<mettriggersMC.size(); i++){
	bool inc=false;
	//	if(!(mettriggersMC[i].Contains("HLT"))) continue;
	for(unsigned int j=0; j<triggersummary.size();j++){
	  // cout << mettriggers[i] << " vs " << triggersummary[j].first << endl;
	  if(triggersummary[j].first.Contains(mettriggersMC[i])){
	    trigin++;
	    inc=true;
	    break;
	  }
	}
	if(!inc) notinc.push_back(i);
      }


      cout << trigin << " total; fraction of found triggers wrt datamettriggers: " << trigin/(double)mettriggers.size() << endl;
      cout << "\ntriggers in metMCtrigger list and not found in triggered triggers:\n" << endl;

      for(unsigned int i=0; i<notinc.size();i++){
	cout << mettriggersMC[notinc[i]] << endl;
      }
			 */

		}



		if(!isMC_) cout <<  '\n' << mode_<< " \t\t& $N_{sel}$ \t& $N_{sel}^{trig}$ \t& $\\epsilon_d$ \t& $\\delta\\epsilon$ (stat.) \\\\ \\hline" << endl;
		if(isMC_) cout <<  '\n' << mode_<< " MC" << " \t\t& $N_{sel}$ \t& $N_{sel}^{trig}$ \t& $\\epsilon_{MC}$  \t& $\\delta\\epsilon$ (stat.) \t&R$_c$ - 1 \\\\ \\hline" << endl;
		for(unsigned int i=0; i< sel_woTrig.size();i++){
			double eff=sel_Trig[i].second / sel_woTrig[i].second;
			double erreff=sqrt(eff*(1-eff)/sel_woTrig[i].second);
			double ratio= (eff * sel_MetTrig[i].second/sel_woTrig[i].second ) /(sel_BothTrig[i].second/ sel_woTrig[i].second) -1;
			cout.setf(ios::fixed,ios::floatfield);
			cout.precision(0);
			cout <<  sel_woTrig[i].first << "\t& " << sel_woTrig[i].second << "\t\t& " << sel_Trig[i].second << "\t\t\t& ";
			//cout.unsetf(ios::floatfield);
			cout.precision(3);
			if(isMC_) cout << eff  << " \t& " << erreff << " \t& " << ratio << " \\\\" << endl;
			else cout << eff << " \t& " << erreff <<" \\\\" << endl;
		}


		TString add="emu";
		if(mode_<0) add="ee";
		if(mode_>0) add="mumu";

		alltriples_.clear();

		for(size_t i=0;i<effTriple::effTriple_list.size();i++){
			//  std::cout << "adding " << effTriple::effTriple_list.at(i)->getName() << std::endl;
			alltriples_.push_back(*(effTriple::effTriple_list.at(i)));
		}

		//all histograms saved
		globalDen_=sel_woTrig[0].second;

		return output;


	}


	void writeAll(TString add=""){
		for(size_t i=0; i< alltriples_.size();i++){
			// std::cout << alltriples_.at(i).getName() << std::endl;
			histWrapper a=alltriples_.at(i).getNum();
			histWrapper b=alltriples_.at(i).getDen();
			histWrapper c=alltriples_.at(i).getEff();
			if(add !=""){
				a.setName(a.getName()+add);
				b.setName(b.getName()+add);
				c.setName(c.getName()+add);
				applyNumStyle(a);
				applyDenStyle(b);
				applyEfficiencyStyleData(c);
			}
			a.write();
			b.write();
			c.write();
		}
	}



	std::vector<string> initTriggers(){
		std::vector<string> mettriggers;


		mettriggers.push_back("HLT_PFHT350_PFMET120_NoiseCleaned_v1");
		mettriggers.push_back("HLT_PFMET170_NoiseCleaned_v1");
		mettriggers.push_back("HLT_LooseIsoPFTau50_Trk30_eta2p1_MET120_v1");
		mettriggers.push_back("HLT_PFMET120_NoiseCleaned_BTagCSV07_v1");
		mettriggers.push_back("HLT_Photon250_NoHE_PFMET40_v1");
		mettriggers.push_back("HLT_Photon300_NoHE_PFMET40_v1");
		mettriggers.push_back("HLT_Photon160_PFMET40_v1");
		mettriggers.push_back("HLT_Photon150_PFMET40_v1");
		mettriggers.push_back("HLT_Photon135_PFMET40_v1");
		mettriggers.push_back("HLT_Photon22_R9Id90_HE10_Iso40_EBOnly_PFMET40_v1");
		mettriggers.push_back("HLT_Photon36_R9Id90_HE10_Iso40_EBOnly_PFMET40_v1");
		mettriggers.push_back("HLT_Photon50_R9Id90_HE10_Iso40_EBOnly_PFMET40_v1");
		mettriggers.push_back("HLT_Photon75_R9Id90_HE10_Iso40_EBOnly_PFMET40_v1");
		mettriggers.push_back("HLT_Photon90_R9Id90_HE10_Iso40_EBOnly_PFMET40_v");


		//just for testing
		// mettriggers.clear();
		//mettriggers.push_back("HLT_MET120_v10");

		vector<string> notinMCtriggers;

		std::vector<std::string> out;

		for(unsigned int j=0; j< mettriggers.size();j++){
			bool isOk=true;
			for(unsigned int i=0;i<notinMCtriggers.size();i++){
				if( ((TString)mettriggers[j]).Contains(notinMCtriggers[i]) ){
					mettriggers[j]="notrig";
					isOk=false;
				}
			}
			if(isOk) out.push_back(mettriggers[j]);
		}

		if(checktriggerpaths_){
			cout << "used MET triggers: " << endl;
			for(unsigned int i=0;i<out.size();i++){
				std::cout << out.at(i) << std::endl;
			}
		}

		// mettriggers=notinMCtriggers;
		return out;
	}


	static bool testmode;
	static bool lowMCStat,lowDataStat;

protected:
	std::vector<float> binseta_;
	std::vector<float> binseta2dx_;
	std::vector<float> binseta2dy_;
	std::vector<float> binspt_;

	bool match_;

	double masscutlow_,masscuthigh_;
	double runcutlow_,runcuthigh_;

	bool statusbar_;

	std::vector<string> trigs_,trigsMC_,trigsObj_;

	TString whichelectrons_;
	bool isMC_;
	bool checktriggerpaths_;
	bool coutalltriggerpaths_;
	bool includecorr_;
	int mode_; // -1: ee 0: emu 1:mumu
	TString pufile_,mcpufile_;
	bool prescaleweight_;
	int selectionstep_;

	std::vector< ztop::NTElectron *> selectedElecs_;
	std::vector< ztop::NTMuon *> selectedMuons_;

	TChain * t_;

	std::vector<ztop::effTriple> alltriples_;
	// bool isMC;

	double globalDen_;
	vector<double> storedOut_;
	unsigned int prescaleCutHigh_;

	std::vector<double>  *tempp0_;
	std::vector<double>  *tempp1_;
	std::vector<double>  *tempp2_;
	std::vector<double>  *tempp3_;

};




//function analyze(std::vector<TString> datafiles, std::vector<TString> eemcfiles,std::vector<TString> mumumcfiles,std::vector<TString> emumcfiles)



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///

// so include this header define your dilepton selection virtual double selectDileptons(std::vector<ztop::NTMuon> * inputMuons, std::vector<ztop::NTElectron> * inputElectrons) and run  analyze(std::vector<TString> datafiles, std::vector<TString> eemcfiles,std::vector<TString> mumumcfiles,std::vector<TString> emumcfiles)



std::vector<histWrapper> getAllSFs(triggerAnalyzer & num, triggerAnalyzer & den , double relerror=0){ //also applies style
	using namespace ztop;
	using namespace std;
	if(num.getTriples().size() != den.getTriples().size()){
		cout << "getSFs: wrong sized triples! exit" << endl;
	}

	//search for same named _eff plots and divide them.
	std::vector<histWrapper> out;

	for(size_t i=0;i<num.getTriples().size();i++){
		histWrapper  firsteff=num.getTriples().at(i).getEff();
		for(size_t j=0;j<den.getTriples().size();j++){
			histWrapper seceff=den.getTriples().at(j).getEff();
			if(firsteff.getName() == seceff.getName()){ //found two matching ones
				firsteff.setDivideBinomial(false);
				seceff.setDivideBinomial(false);

				seceff.setName(seceff.getName() + "_mc");
				applyEfficiencyStyleData(firsteff);
				applyEfficiencyStyleMC(seceff);


				out.push_back(firsteff);
				out.push_back(seceff);

				histWrapper sf=firsteff / seceff;
				sf.setName(num.getTriples().at(i).getName() + "_sf");

				sf.addRelError(relerror);
				applySFStyle(sf);
				out.push_back(sf);
			}
		}
	}
	return out;
}




/////definitions here
TString makeFullOutput(triggerAnalyzer & data, triggerAnalyzer & mc , TString dirname, TString label,  double relerror=0){

	using namespace std;
	int selectionstep=data.selectionStep();
	if(data.selectionStep() != mc.selectionStep()){
		cout << "makeFullOutput: selection Step must be the same!" << endl;
		return "";
	}
	if(data.getTriples().size() < 1 || mc.getTriples().size() < 1){
		cout << "makeFullOutput: run Eff() first!" << endl;
		return "";
	}

	std::vector<histWrapper> sfs=getAllSFs(data,mc,relerror);

	system(("mkdir -p "+dirname).Data());

	TFile *f = new TFile(dirname+"/scalefactors.root","RECREATE");
	for(size_t i=0;i<sfs.size();i++)
		sfs.at(i).write();
	f->Close();

	TFile *f2 = new TFile(dirname+"/raw.root","RECREATE");
	//data.writeAll();
	//mc.writeAll("MC");
	plotRaw(data.getTriples(),"",label,dirname+"/");
	plotRaw(mc.getTriples(),"_mc",label+" MC",dirname+"/");
	f2->Close();

	//make plots

	TFile *f3 = new TFile(dirname+"/sf_plots.root","RECREATE");
	plotAll(sfs,label,dirname+"/");
	f3->Close();

	delete f;
	delete f2;
	delete f3;

	std::cout << "to get subset of plots: " << std::endl;
	std::cout << "root -l -b -q 'getSubSetRoot.C++(\"" << dirname << "\")'" << std::endl;

	effTriple globaldata=data.getTriples().at(0);
	effTriple globalmc=mc.getTriples().at(0);
	effTriple globaldatanoC=data.getTriples().at(1);
	effTriple globalmcnoC=mc.getTriples().at(1);

	if(!globaldata.getNum().isTH1D()){
		cout << "makeFullOutput: global SF histo needs to be TH1D!!" <<endl;
		return "";
	}
	int bin=globaldata.getNum().getTH1D().FindBin(selectionstep);
	/*
  double datanum=globaldata.getNum().getTH1D().GetBinContent(bin);
  double dataden=globaldata.getDen().getTH1D().GetBinContent(bin);
  double datanumerr=globaldata.getNum().getTH1D().GetBinError(bin);
  double datadenerr=globaldata.getDen().getTH1D().GetBinError(bin);
	 */
	double dataeff=globaldata.getEff().getTH1D().GetBinContent(bin);
	double dataefferr=globaldata.getEff().getTH1D().GetBinError(bin);



	/*
  double mcnum=globalmc.getNum().getTH1D().GetBinContent(bin);
  double mcden=globalmc.getDen().getTH1D().GetBinContent(bin);
  double mcnumerr=globalmc.getNum().getTH1D().GetBinError(bin);
  double mcdenerr=globalmc.getDen().getTH1D().GetBinError(bin);
	 */
	double mceff=globalmc.getEff().getTH1D().GetBinContent(bin);
	double mcefferr=globalmc.getEff().getTH1D().GetBinError(bin);

	double mceffnoC=globalmcnoC.getEff().getTH1D().GetBinContent(bin);

	double alpha=mceffnoC/mceff;

	histWrapper hwdataeff=globaldata.getEff();
	hwdataeff.setDivideBinomial(false);
	histWrapper hwmceff=globalmc.getEff();
	hwmceff.setDivideBinomial(false);

	histWrapper hwscaleF=hwdataeff/hwmceff;
	hwscaleF.addRelError(relerror);
	double sfstaterr=hwscaleF.getTH1D().GetBinError(bin);
	double sf=hwscaleF.getTH1D().GetBinContent(bin);
	double sferr=hwscaleF.getTH1D().GetBinError(bin);

	std::cout << "***********\nTeX table Efficiencies for  \n" << label << std::endl;
	cout.setf(ios::fixed,ios::floatfield);
	cout.precision(3);
	std::cout << "  & $\\epsilon_{data}$ & $\\epsilon_{MC}$ & SF & $\\alpha$ \\\\ \n"
			<<  dataeff << " $\\pm$ " << dataefferr << " & "
			<<   mceff << " $\\pm$ " << mcefferr << " & "
			<<  sf << " $\\pm$ " << sferr << " " << alpha << "\\\\" << endl;


	std::cout << "\n***********\nnumbers for copy-paste in tables(data,mc,sf,alpha)(stat,stat,stat;syst)\n"
			<< dataeff << " " << dataefferr << "\n"
			<< mceff << " " << mcefferr << "\n"
			<< sf << " " << sfstaterr << " " << sf*0.01 << " " << alpha
			<< "\n************\n" << std::endl;


	std::ostringstream s;
	s.setf(ios::fixed,ios::floatfield);
	s.precision(3);
	s    << data.getModeS() << " & " << dataeff << " $\\pm$ " << dataefferr << " & "
			<<   mceff << " $\\pm$ " << mcefferr << " & "
			<<  sf << " $\\pm$ " << sferr << " & " << alpha << "\\\\";

	TString out= s.str();
	return out;

}


void analyzeAll(triggerAnalyzer &ta_eed, triggerAnalyzer &ta_eeMC, triggerAnalyzer &ta_mumud, triggerAnalyzer &ta_mumuMC, triggerAnalyzer &ta_emud, triggerAnalyzer &ta_emuMC, TString dir="", TString label=""){

	using namespace ztop;
	using namespace std;


	cout << "Doing efficiencies for " << dir << " " << label << endl;

	vector<double> eed;eed=ta_eed.Eff();
	vector<double> mumud;mumud=ta_mumud.Eff();
	vector<double> emud;emud= ta_emud.Eff();


	vector<double> eeMC=ta_eeMC.Eff();
	vector<double> mumuMC=ta_mumuMC.Eff();
	vector<double> emuMC=ta_emuMC.Eff();


	cout.precision(3);
	// get correction factors

	cout << "Output Summary for " << dir << " " << label << endl;

	cout << "\n\n TeX table:" <<endl;

	cout << "\\begin{table}\n\\center\n\\begin{tabular}{c | c | c | c}" << endl;
	cout << " & $\\epsilon_{data}$ & $\\epsilon_{MC}$ & $\\epsilon_{data}/\\epsilon_{MC}$ \\\\ \\hline" << endl;

	cout << "$ee$ & " << eed[0] <<" $\\pm$ " << eed[1] << " (stat.) & "
			<< eeMC[0] << " $\\pm$ " << eeMC[1] << " (stat.) & " << eed[0]/eeMC[0]
			                                                                    << " $\\pm$ " << sqrt((eed[1]/eeMC[0])*(eed[1]/eeMC[0]) + (eed[0]/(eeMC[0]*eeMC[0]))*eeMC[1]*eeMC[1]*(eed[0]/(eeMC[0]*eeMC[0]))) << " (stat.)  "
			                                                                    <<  " $\\pm$ " << sqrt(0.01*eed[0]/eeMC[0]*eed[0]/eeMC[0]*0.01  +fabs(ratiomultiplier*eed[0]/eeMC[0] * eeMC[2])*fabs(ratiomultiplier*eed[0]/eeMC[0] * eeMC[2])) << " (syst.) \\\\" << endl;


	cout << "$\\mu\\mu$ & " << mumud[0] <<" $\\pm$ " << mumud[1] << " (stat.) & "
			<< mumuMC[0] << " $\\pm$ " << mumuMC[1] << " (stat.) & " << mumud[0]/mumuMC[0]
			                                                                            << " $\\pm$ " << sqrt((mumud[1]/mumuMC[0])*(mumud[1]/mumuMC[0]) + (mumud[0]/(mumuMC[0]*mumuMC[0]))*mumuMC[1]*mumuMC[1]*(mumud[0]/(mumuMC[0]*mumuMC[0]))) << " (stat.)  "
			                                                                            <<  " $\\pm$ " << sqrt(0.01*mumud[0]/mumuMC[0]*mumud[0]/mumuMC[0]*0.01+fabs(ratiomultiplier*mumud[0]/mumuMC[0] * mumuMC[2])*fabs(ratiomultiplier*mumud[0]/mumuMC[0] * mumuMC[2])) << " (syst.) \\\\" << endl;

	cout << "$e\\mu$ & " << emud[0] <<" $\\pm$ " << emud[1] << " (stat.) & "
			<< emuMC[0] << " $\\pm$ " << emuMC[1] << " (stat.) & " << emud[0]/emuMC[0]
			                                                                        << " $\\pm$ " << sqrt((emud[1]/emuMC[0])*(emud[1]/emuMC[0]) + (emud[0]/(emuMC[0]*emuMC[0]))*emuMC[1]*emuMC[1]*(emud[0]/(emuMC[0]*emuMC[0]))) << " (stat.)  "
			                                                                        <<  " $\\pm$ " << sqrt(0.01*emud[0]/emuMC[0]*emud[0]/emuMC[0]*0.01+fabs(ratiomultiplier*emud[0]/emuMC[0] * emuMC[2])*fabs(ratiomultiplier*emud[0]/emuMC[0] * emuMC[2])) << " (syst.) \\\\" << endl;

	cout << "\\end{tabular}\n\\caption{Dilepton trigger efficiencies for data and MC and resulting scaling factors}\n\\end{table}" << endl;

	cout << "\n\n\\begin{table}\n\\center\n\\begin{tabular}{c | c | c | c}" << endl;
	cout  << " & $ee$ & $\\mu\\mu$ & $e\\mu$ \\\\ \\hline" <<endl;

	cout.precision(3);
	cout.setf(ios::fixed,ios::floatfield);

	//make summarizing table
	for(unsigned int i=0; i<eed.size();i++){
		TString step;
		if(i==0) step="dilepton";
		if(i==3) step="Z-Veto";
		if(i==6) step="$\\geq1$jet";
		if(i==9) step="$\\geq2$jets";
		if(i==12) step="MET";
		cout << step << " & " << eed[i]/eeMC[i] << " $\\pm$ " << sqrt((eed[i+1]/eeMC[i])*(eed[1]/eeMC[i]) + (eed[i]/(eeMC[i]*eeMC[i]))*eeMC[i+1]*eeMC[i+1]*(eed[i]/(eeMC[i]*eeMC[i])) + fabs(ratiomultiplier*eed[i]/eeMC[i] * eeMC[i+2])*fabs(ratiomultiplier*eed[i]/eeMC[i] * eeMC[i+2]) + 0.01*0.01*eed[i]/eeMC[i]*eed[i]/eeMC[i]) << " & "
				<< mumud[i]/mumuMC[i] << " $\\pm$ " << sqrt((mumud[i+1]/mumuMC[i])*(mumud[1]/mumuMC[i]) + (mumud[i]/(mumuMC[i]*mumuMC[i]))*mumuMC[i+1]*mumuMC[i+1]*(mumud[i]/(mumuMC[i]*mumuMC[i])) + fabs(ratiomultiplier*mumud[i]/mumuMC[i] * mumuMC[i+2])*fabs(ratiomultiplier*mumud[i]/mumuMC[i] * mumuMC[i+2]) + 0.01*0.01*mumud[i]/mumuMC[i]*mumud[i]/mumuMC[i] ) << "  & "
				<< emud[i]/emuMC[i] << " $\\pm$ " << sqrt((emud[i+1]/emuMC[i])*(emud[1]/emuMC[i]) + (emud[i]/(emuMC[i]*emuMC[i]))*emuMC[i+1]*emuMC[i+1]*(emud[i]/(emuMC[i]*emuMC[i])) + fabs(ratiomultiplier*emud[i]/emuMC[i] * emuMC[i+2])*fabs(ratiomultiplier*emud[i]/emuMC[i] * emuMC[i+2]) + 0.01*0.01*emud[i]/emuMC[i]*emud[i]/emuMC[i]) << " \\\\" << endl;
		i++;i++;
	}

	cout << "\\end{tabular}\n\\caption{Trigger scale factors for different ttbar selection steps}\n\\end{table}" << endl;


	cout << "\n\nsummary tablenew style" << endl;

	TString eestring= makeFullOutput(ta_eed, ta_eeMC, "ee_"+dir, "ee"+label, 0.01);
	TString mumustring= makeFullOutput(ta_mumud, ta_mumuMC, "mumu_"+dir, "#mu#mu"+label, 0.01);
	TString emustring= makeFullOutput(ta_emud, ta_emuMC, "emu_"+dir, "e#mu"+label, 0.01);

	std::cout << "channel  & $\\epsilon_{data}$ & $\\epsilon_{MC}$ & SF & $\\alpha$ \\\\ " << std::endl;
	std::cout << eestring<< std::endl;
	std::cout << mumustring<< std::endl;
	std::cout << emustring<< std::endl;


}


bool triggerAnalyzer::testmode=false;
bool triggerAnalyzer::lowMCStat=false;
bool triggerAnalyzer::lowDataStat=false;

#endif
