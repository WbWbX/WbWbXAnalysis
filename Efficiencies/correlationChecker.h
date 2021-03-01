#ifndef CORRELATIONCHECKER_H
#define CORRELATIONCHECKER_H

#include <vector>
#include "TString.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "WbWbXAnalysis/DataFormats/interface/NTMuon.h"
#include "WbWbXAnalysis/DataFormats/interface/NTElectron.h"
#include "WbWbXAnalysis/DataFormats/interface/NTEvent.h"
#include "WbWbXAnalysis/DataFormats/interface/NTJet.h"
#include "WbWbXAnalysis/DataFormats/interface/NTMet.h"

#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"

//vector with triggers to be checked
//check function
//output
// MC input trees (vector<TString> and then add to chain)

namespace ztop{


class correlationChecker{
private:
	std::vector<TString> checkdiltriggers_;
	std::vector<TString> checkmettriggers_;
	std::vector<TString> inputfiles_;
	TString mode_;
	TString whichelectrons_;

public:


	correlationChecker(TString mode=""){mode_=mode; whichelectrons_="NTPFElectrons";};
	~correlationChecker(){};
	std::vector<TString> & dileptonTriggers(){return checkdiltriggers_;}
	std::vector<TString> & crossTriggers(){return checkmettriggers_;}
	std::vector<TString> & inputfiles(){return inputfiles_;}
	void setElectrons(TString elecs){whichelectrons_=elecs;}

	TChain * makeChain(std::vector<TString> paths){
		TChain * chain = new TChain(paths[0]);
		for(std::vector<TString>::iterator path=paths.begin();path<paths.end();++path){
			chain->Add((*path)+"/PFTree/PFTree");
			std::cout << "added " << *path << " to chain." << std::endl;
		}
		return chain;
	}
	///////////////
	void getCorrelations(TString mode=""){

		if(mode=="") mode=mode_;

		using namespace std;
		using namespace ztop;

		cout << "\nchecking " << checkmettriggers_.size() << " in total" <<endl;

		bool useRhoIso=false;

		cout << "warning! no PU corrections applied here. Use numbers only for first estimate!!!\n" <<endl;
		TChain * t1 = makeChain(inputfiles_);

		vector<NTMuon> * pMuons = 0;
		t1->SetBranchAddress("NTMuons",&pMuons);
		vector<NTElectron> * pElectrons = 0;
		t1->SetBranchAddress(whichelectrons_,&pElectrons);
		vector<NTJet> * pJets = 0;
		t1->SetBranchAddress("NTJets",&pJets);
		NTMet * pMet = 0;
		t1->SetBranchAddress("NTMet",&pMet);
		NTEvent * pEvent = 0;
		t1->SetBranchAddress("NTEvent",&pEvent);
		map<string,unsigned int> * pTriggersWithPrescales=0;
		t1->SetBranchAddress("AllTriggersWithPrescales",&pTriggersWithPrescales);

		float n = t1->GetEntries();

		vector<double> metfired,dilfired,bothfired;
		for(unsigned int i=0;i<checkmettriggers_.size();i++){
			metfired  << 0;
			dilfired  << 0;
			bothfired << 0;
		}
		double dileptonevents=0;


		double totaldil=0;
		double totalmet=0;
		double totalboth=0;

		for(float i=0;i<n;i++){  //main loop

			t1->GetEntry(i);
			displayStatusBar(i,n);

			if(pEvent->vertexMulti() <1) continue;
			if(mode=="ee" && pElectrons->size()<2) continue;
			if(mode=="mumu" && pMuons->size()<2) continue;

			//dilepton preselection ////////////////////
			bool b_dilepton=false;
			double puweight=1;

			vector<NTMuon> selectedMuons;
			for(vector<ztop::NTMuon>::iterator muon=pMuons->begin();muon<pMuons->end();muon++){
				if(!(fabs(muon->eta())<2.4) ) continue;
				if(!(muon->pt() >20))   continue;
				if(!(muon->isGlobal() || muon->isTracker()) ) continue;
				//  if(muon->trkHits()<3) continue;
				//  if(muon->trkHits() <= 5 ) continue;
				// if(muon->muonHits() <1) continue;
				// if(muon->dbs()> 0.2) continue;
				if(muon->isoVal() > 0.2 ) continue;
				//  if(muon->normChi2() >10) continue;
				selectedMuons.push_back(*muon);
			}
			if(mode == "mumu" && selectedMuons.size() < 2) continue;

			vector<NTElectron> selectedElecs;
			for(vector<NTElectron>::iterator elec=pElectrons->begin();elec<pElectrons->end();elec++){
				if(elec->pt()<20 ) continue;
				if(fabs(elec->eta())>2.5 ) continue;
				if(fabs(elec->d0V()) >0.04 ) continue;
				if(!(elec->isNotConv()) ) continue;
				if(elec->rhoIso()>0.15 ) continue;
				//if(!useRhoIso) continue;
				//  if(CiCId && (0x00 == (0x01 & (int) elec->id("cicTightMC"))) ) continue; //for CiC bit test
				if(elec->storedId() < -0.1) continue;
				//if(!noOverlap(elec,*pMuons,0.1)) continue;

				selectedElecs.push_back(*elec);
			}
			double mass=0;
			if(mode=="ee"){
				if(selectedElecs.size() <2) continue;
				if(selectedElecs[0].q() == selectedElecs[1].q()) continue;
				mass=(selectedElecs[0].p4() + selectedElecs[1].p4()).M();
				if(mass > 20)
					b_dilepton=true;
			}

			if(mode=="mumu"){
				if(selectedMuons.size() < 2) continue;
				if(selectedMuons[0].q() == selectedMuons[1].q()) continue;
				mass=(selectedMuons[0].p4() + selectedMuons[1].p4()).M();
				if(mass > 20)
					b_dilepton=true;
			}

			if(mode=="emu"){
				if(selectedMuons.size() < 1 || selectedElecs.size() < 1) continue;
				if(selectedMuons[0].q() * selectedElecs[0].q() == 1) continue;
				mass=(selectedMuons[0].p4() + selectedElecs[0].p4()).M();
				if(mass > 20)
					b_dilepton=true;
			}

			if(!b_dilepton) continue; ///////////////////////
			dileptonevents+=puweight;

			/////////check triggers/////////

			//vector<string> trigs;
			//trigs=pEvent->firedTriggers();
			bool dilfired_b=false;
			bool anymetfired_b=false;

			for(std::map<std::string, unsigned int>::iterator trigP=pTriggersWithPrescales->begin();trigP!=pTriggersWithPrescales->end();++trigP){
				const string * name= &(trigP->first);

				for(unsigned int dtrigit=0;dtrigit<checkdiltriggers_.size();dtrigit++){
					if(((TString)*name).Contains(checkdiltriggers_.at(dtrigit))){

						dilfired_b=true;
						break;
					}
				}
				if(dilfired_b)break;
			}
			for(unsigned int ctrigit=0;ctrigit<checkmettriggers_.size();ctrigit++){
				if(dilfired_b)
					dilfired.at(ctrigit)+= puweight;
				for(std::map<std::string, unsigned int>::iterator trigP=pTriggersWithPrescales->begin();trigP!=pTriggersWithPrescales->end();++trigP){
					const string * name= &(trigP->first);
					if(((TString)*name).Contains(checkmettriggers_.at(ctrigit))){
						metfired.at(ctrigit) += puweight;

						anymetfired_b=true;
						// cout << "bla" << endl;
						if(dilfired_b) bothfired.at(ctrigit) += puweight;
						break;
					}
				}
			}


			if(dilfired_b)                  totaldil +=puweight;
			if(anymetfired_b)               totalmet +=puweight;
			if(anymetfired_b && dilfired_b) totalboth+=puweight;

		}//main loop
		cout << endl;

		//  cout << dileptonevents << endl;

		vector<double> ratios;
		vector<double> ratioerrs;
		vector<double> signif;


		for(unsigned int ctrig=0;ctrig<checkmettriggers_.size();ctrig++){
			//	cout << checkmettriggers_.at(ctrig) <<endl;

			double ratio = (metfired.at(ctrig) * dilfired.at(ctrig))/(bothfired.at(ctrig) * dileptonevents);
			double effboth=bothfired.at(ctrig)/dileptonevents;

			double err= sqrt(effboth*(1-effboth))/sqrt(dileptonevents) * ratio/effboth;

			ratios << ratio;
			ratioerrs << err;
			signif << fabs(1-ratio)+fabs(err);
		}
		vector<unsigned int> order=getOrderAscending(signif);

		cout <<"\n\nList of triggers:\n\n" <<endl;

		cout.precision(4);
		for(unsigned int i=0;i<order.size();i++){
			unsigned int it=order.at(i);
			cout << "sig: " << signif.at(it)<<
					"\tR: " << ratios.at(it) <<
					"\tRerr: " << ratioerrs.at(it)<<
					"\t" << checkmettriggers_.at(it) << endl;

		}
		for(unsigned int i=0;i<order.size();i++){
			unsigned int it=order.at(i);
			cout << " << \"" <<  checkmettriggers_.at(it) <<"\""<< endl;
		}
		double totalratio = totalmet * totaldil/(totalboth * dileptonevents);
		double totalerr=sqrt(totalboth/dileptonevents * ( 1- totalboth/dileptonevents) / dileptonevents) * totalratio / (totalboth/dileptonevents);
		cout << "total dilepton events: " << dileptonevents<< endl;
		cout << "total met:             " <<  totalmet<< endl;
		cout << "total both:            " << totalboth<< endl;
		cout  << "\n\ntotal ratio:      " << totalratio << " +- "<< totalerr << endl;



	}

	void getTriggers(TString restr, TString mode=""){
		if(mode=="") mode=mode_;

		using namespace std;
		using namespace ztop;

		map<TString, double> triggerlist;

		cout << "\ngetting triggers with dilepton contribution restricted to " << restr <<endl;

		bool useRhoIso=false;

		cout << "warning! no PU corrections applied here. Use numbers only for first estimate!!!\n" <<endl;
		TChain * t1 = makeChain(inputfiles_);

		vector<NTMuon> * pMuons = 0;
		t1->SetBranchAddress("NTMuons",&pMuons);
		vector<NTElectron> * pElectrons = 0;
		t1->SetBranchAddress(whichelectrons_,&pElectrons);
		vector<NTJet> * pJets = 0;
		t1->SetBranchAddress("NTJets",&pJets);
		NTMet * pMet = 0;
		t1->SetBranchAddress("NTMet",&pMet);
		NTEvent * pEvent = 0;
		t1->SetBranchAddress("NTEvent",&pEvent);
		map<string,unsigned int> * pTriggersWithPrescales=0;
		t1->SetBranchAddress("AllTriggersWithPrescales",&pTriggersWithPrescales);

		float n = t1->GetEntries();

		vector<double> metfired,dilfired,bothfired;
		for(unsigned int i=0;i<checkmettriggers_.size();i++){
			metfired  << 0;
			dilfired  << 0;
			bothfired << 0;
		}
		//   double dileptonevents=0;

		for(float i=0;i<n;i++){  //main loop

			t1->GetEntry(i);
			displayStatusBar(i,n);

			if(pEvent->vertexMulti() <1) continue;
			if(mode=="ee" && pElectrons->size()<2) continue;
			if(mode=="mumu" && pMuons->size()<2) continue;

			//dilepton preselection ////////////////////
			bool b_dilepton=false;
			double puweight=1;

			vector<NTMuon> selectedMuons;
			for(vector<ztop::NTMuon>::iterator muon=pMuons->begin();muon<pMuons->end();muon++){
				if(!(fabs(muon->eta())<2.4) ) continue;
				if(!(muon->pt() >20))   continue;
				if(!(muon->isGlobal() || muon->isTracker()) ) continue;
				//  if(muon->trkHits()<3) continue;
				//  if(muon->trkHits() <= 5 ) continue;
				// if(muon->muonHits() <1) continue;
				// if(muon->dbs()> 0.2) continue;
				if(muon->isoVal() > 0.2 ) continue;
				//  if(muon->normChi2() >10) continue;
				selectedMuons.push_back(*muon);
			}
			if(mode == "mumu" && selectedMuons.size() < 2) continue;

			vector<NTElectron> selectedElecs;
			for(vector<NTElectron>::iterator elec=pElectrons->begin();elec<pElectrons->end();elec++){
				if(elec->pt()<20 ) continue;
				if(fabs(elec->eta())>2.5 ) continue;
				if(fabs(elec->d0V()) >0.04 ) continue;
				if(!(elec->isNotConv()) ) continue;
				if(useRhoIso && elec->rhoIso()>0.15 ) continue;
				//if(!useRhoIso && elec->isoVal03()>0.15) continue;
				//  if(CiCId && (0x00 == (0x01 & (int) elec->id("cicTightMC"))) ) continue; //for CiC bit test
				if(elec->storedId() < 0.1) continue;
				//if(!noOverlap(elec,*pMuons,0.1)) continue;

				selectedElecs.push_back(*elec);
			}
			double mass=0;
			if(mode=="ee"){
				if(selectedElecs.size() <2) continue;
				if(selectedElecs[0].q() == selectedElecs[1].q()) continue;
				mass=(selectedElecs[0].p4() + selectedElecs[1].p4()).M();
				if(mass > 20)
					b_dilepton=true;
			}

			if(mode=="mumu"){
				if(selectedMuons.size() < 2) continue;
				if(selectedMuons[0].q() == selectedMuons[1].q()) continue;
				mass=(selectedMuons[0].p4() + selectedMuons[1].p4()).M();
				if(mass > 20)
					b_dilepton=true;
			}

			if(mode=="emu"){
				if(selectedMuons.size() < 1 || selectedElecs.size() < 1) continue;
				if(selectedMuons[0].q() * selectedElecs[0].q() == 1) continue;
				mass=(selectedMuons[0].p4() + selectedElecs[0].p4()).M();
				if(mass > 20)
					b_dilepton=true;
			}

			if(!b_dilepton) continue; ///////////////////////
			vector<string> trigs;
			trigs=pEvent->firedTriggers();

			for(std::map<std::string, unsigned int>::iterator trigP=pTriggersWithPrescales->begin();trigP!=pTriggersWithPrescales->end();++trigP){
				const string * name= &(trigP->first);
				if(name->find(restr.Data())!=string::npos)
					triggerlist[*name] +=puweight;
			}

		}//
		cout <<endl;
		vector<TString> trigs;
		vector<double> trigcontr;
		for(map<TString,double>::iterator it=triggerlist.begin();it!=triggerlist.end();++it){
			trigs << it->first;
			trigcontr << it->second;
		}
		vector<unsigned int> order=getOrderAscending(trigcontr);
		for(unsigned int i=order.size(); i>0 ;i--){
			unsigned int it=order.at(i-1);
			cout << trigcontr.at(it)
	    																								 << "\t"
	    																								 << trigs.at(it)
	    																								 << endl;
		}

		for(unsigned int i=order.size(); i>0 ;i--){
			unsigned int it=order.at(i-1);
			cout << " << \"" << trigs.at(it) << "\""<<endl;
		}
		cout << endl;


	}

};


}//namespace

#endif
