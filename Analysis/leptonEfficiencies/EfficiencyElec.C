#include "../TtZAnalysis/DataFormats/interface/NTEvent.h"
#include "../TtZAnalysis/DataFormats/interface/NTMuon.h"
#include "../TtZAnalysis/DataFormats/interface/NTElectron.h"
#include "../TtZAnalysis/DataFormats/interface/NTSuClu.h"
#include "../TtZAnalysis/DataFormats/interface/NTLepton.h"
#include "../TtZAnalysis/DataFormats/interface/NTMet.h"
#include "../TtZAnalysis/DataFormats/interface/NTJet.h"
#include "../TtZAnalysis/DataFormats/interface/NTIsolation.h"
#include "../TtZAnalysis/DataFormats/interface/elecRhoIsoAdder.h"
#include "../TopAnalysis/ZTopUtils/interface/PUReweighter.h"



#include "TH1D.h"
#include "TH2D.h"
#include "TTree.h"
#include "TBranch.h"
#include "TFile.h"
#include "TString.h"
#include <TH1.h>
#include <TH2.h>
#include <TStyle.h>
#include "TLorentzVector.h"
#include <fstream>
#include <iostream>
#include <TMath.h>
#include <TSystem.h>
#include <set>


#include <map>

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <vector>
#include "TCanvas.h"
#include "TLegend.h"
#include "TExec.h"
#include "TROOT.h"
#include <sstream>
#include <cstdio>

//#include "HHStyle.h"
#include "TGraphAsymmErrors.h"


class TFile;
class TH1D;
class TH2D;
class TTree;
class TBranch;

class NTEvent;
class NTMuon;
class NTLepton;
class NTMet;
class NTJet;

struct t_probe{
  double pt, eta, d0, InvMass, id, iso;
  int isPF, mHits, isNotConv;
};

namespace top{using namespace ztop;}
using namespace std;

void EfficiencyElec(){
  bool doRhoIso = true;
  double mvaCut = 0.9;
  double EtaCut = 2.4;
  double PtCut = 20.;
  double d0Cut = 0.02;
  double IsoCut = 0.1;
  int maxmHits = 0;

  double minMass = 60.0;
  double maxMass = 120.0;
  TH1D * h_IsoJetsMC = new TH1D("NumIsoJetsMC","NumIsoJetsMC;Jets",50,0,50);
  TH1D * h_NUMJetsMC = new TH1D("DenIsoJetsMC","DenIsoJetsMC;Jets",50,0,50);

  TH1D * h_IsoJets = new TH1D("NumIsoJets","NumIsoJets;Jets",50,0,50);
  TH1D * h_NUMJets = new TH1D("DenIsoJets","DenIsoJets;Jets",50,0,50);

  double bins[9]={-2.4,-1.556,-1.4442,-0.8,0.,0.8,1.4442,1.556,2.4};
  int etabin = 8;
  double ptbins[5]={20.,30.,40.,50.,500};
  int ptbin = 4;
  int NumMC = 1;

  TH2D * h2_DEN = new TH2D("Ddenom","D_denom, #eta, #p_{T} (GeV)",etabin,bins,ptbin,ptbins);

  TH2D * h2_NUMIdEff = new TH2D("DIdnum","DId_num, #eta, #p_{T} (GeV)",etabin,bins,ptbin,ptbins);
  TH2D * h2_NUMIsoEff = new TH2D("DIsonum","DIso_num, #eta, #p_{T} (GeV)",etabin,bins,ptbin,ptbins);

  TH2D * histo_IdEff_Data = new TH2D("DataIdEff","Id Eff. Data, #eta, #p_{T} (GeV)",etabin,bins,ptbin,ptbins);
  TH2D * histo_IsoEff_Data = new TH2D("DataIsoEff","Iso Eff. Data, #eta, #p_{T} (GeV)",etabin,bins,ptbin,ptbins);

  TH2D * h2_DENMC = new TH2D("MCdenom","MC_denom; #eta; p_{T} (GeV)",etabin,bins,ptbin,ptbins);
  TH2D * h2_NUMIdEffMC = new TH2D("MCIdnum","MCId_num; #eta; p_{T} (GeV)",etabin,bins,ptbin,ptbins); 
  TH2D * h2_NUMIsoEffMC = new TH2D("MCIsonum","MCIso_num; #eta; p_{T} (GeV)",etabin,bins,ptbin,ptbins);

  TH2D * histo_IdEff_MC = new TH2D("MCIdEff","Id Eff. MC; #eta; p_{T} (GeV)",etabin,bins,ptbin,ptbins);
  TH2D * histo_IsoEff_MC = new TH2D("MCIsoEff","Id Eff. MC; #eta; p_{T} (GeV)",etabin,bins,ptbin,ptbins);

  TH2D * h2_IsoScaleFactor = new TH2D("IsoScaleFactor","IsoSF; #eta; p_{T} (GeV)",etabin,bins,ptbin,ptbins);
  TH2D * h2_IdScaleFactor = new TH2D("IdScaleFactor","IdSF; #eta; p_{T} (GeV)",etabin,bins,ptbin,ptbins);
  TH2D * h2_GlobalScaleFactor = new TH2D("GlobalSF","GlobalSF; #eta; p_{T} (GeV)",etabin,bins,ptbin,ptbins);


  t_probe probeLeptonData;
  t_probe probeLeptonMC;  


  TFile *fMC = new TFile("/nfs/dust/cms/user/kiesej/trees_Oct14/tree_7TeV_dyee60120_eff.root");

  TFile * fout= new TFile("ElecEff_gsfRho_eta24Tight76106MET402Elec.root","RECREATE");


  TTree * ProbeElecVar = new TTree("ProbeElecVar", "probe Elec variables Data");
  ProbeElecVar->Branch("ProbeLeptonData",&probeLeptonData.pt,"pt/D:eta:d0:InvMass:id:iso:isPF/I:mHits:isNotConv");
  TTree * ProbeElecVarMC = new TTree("ProbeElecVarMC", "probe Elec variables MC");
  ProbeElecVarMC->Branch("ProbeLeptonMC",&probeLeptonMC.pt,"pt/D:eta:d0:InvMass:id:iso:isPF/I:mHits:isNotConv");
 
  TString files[2]={"A","B"};
  char datafile[225];
  for(int k = 0; k<1;k++) { // run over all trees
    sprintf(datafile,"/nfs/dust/cms/user/kiesej/trees_Oct14/tree_7TeV_singlee_run%s.root",files[k].Data());

    TFile *fdata = TFile::Open(datafile);
    TTree * t1 = (TTree*) fdata->Get("/PFTree/PFTree");

    vector<ztop::NTElectron> * pElecs = 0;
    t1->SetBranchAddress("NTElectrons",&pElecs); //sets the pointer to the vector<NTElec>

    int n = (int) t1->GetEntries();
    std::cout << " Tree Entries " << n << std::endl;

    std::vector<top::NTJet> * pJet = 0;
    t1->SetBranchAddress("NTJets",&pJet); //sets the pointer to the vector<NTElec>

    top::NTEvent * pNTEventData = 0;
    t1->SetBranchAddress("NTEvent",&pNTEventData);

    top::NTMet * pMET = 0;
    t1->SetBranchAddress("NTMet",&pMET);

    std::vector<bool> *trig=0;
    t1->SetBranchAddress("TriggerBools",&trig);

    //n = 1000000;
    //"real" part starts here
    double EvWeight = 1.0;
    for (int i = 0; i < n; i++) {
      t1->GetEntry(i);
      if(!trig->at(9)) continue;
      //    if(pMET->met() < 40) continue;
      int numjet = 0, haybjet = 0; 
      /*   for(std::vector<top::NTJet>::iterator jet=pJet->begin(); jet < pJet->end(); jet++) {
	   if(jet->pt()>30. && fabs(jet->eta())<2.4 && jet->id() >0 ) {
	   numjet++;
	   //if(jet->btag() > 0.244) haybjet++;
	   }
	   //    cout<<i<<" " << jet->pt() << " " << numjet<<endl;

	   }
      */
      //    if(numjet<2 || pElecs->size()>2 ) continue; 

      //    if(numjet<2) continue;
      if(pMET->met() > 40 || pElecs->size()>2) continue;
      for(vector<ztop::NTElectron>::iterator Elec=pElecs->begin(); Elec != pElecs->end(); Elec++){
      
	if(Elec->mHits() > maxmHits || !Elec->isNotConv() || fabs(Elec->d0V()) >= d0Cut || (doRhoIso && Elec->rhoIso()>= IsoCut) || (!doRhoIso && Elec->isoVal() >= IsoCut ) || Elec->pt() < 30.0 || fabs(Elec->eta()) > EtaCut || !Elec->isPf() || Elec->mvaId() <= mvaCut) continue; 

	int tag = 0;

	for(size_t j=0; j<Elec->matchedTrig().size(); j++){
	  TString checkName(Elec->matchedTrig()[j]); 
	  if(checkName.Contains("HLT_Ele17_CaloIdVT_CaloIsoVT_TrkIdT_TrkIsoVT_SC8_Mass3") || checkName.Contains("HLT_Ele27")){tag++; break;}//std::cout<< "hay tag " <<Elec->matchedTrig()[j]<<std::endl;break;}
 
	  //    if(checkName.Contains("HLT_Ele27_WP80_v")) {tag++; break;}//std::cout<< "hay tag" <<Elec->matchedTrig()[j]<<std::endl;   
	  //    if(checkName.Contains("HLT_Ele2")) {tag++; break;}//std::cout<< "hay tag" <<endl; 
	  //    if(checkName.Contains("HLT_Ele25") || checkName.Contains("HLT_Ele27")){tag++;std::cout<< "hay tag" <<Elec->matchedTrig()[j]<<std::endl; break;}
	}
	if(tag <1 ) continue;
	int foundProbe = 0; 
	for(vector<ztop::NTElectron>::iterator Elec2=pElecs->begin(); Elec2 < pElecs->end(); Elec2++){   
	  if (foundProbe>0) break;
	  probeLeptonData.pt =  Elec2->ECalP4().Pt();
	  probeLeptonData.eta = Elec2->eta();
	  probeLeptonData.d0 = Elec2->d0V();
	  probeLeptonData.isPF = Elec2->isPf();
	  probeLeptonData.mHits = Elec2->mHits();
	  probeLeptonData.InvMass = (Elec->p4()+ Elec2->p4()).M();
	  probeLeptonData.id = Elec2->mvaId();
	  probeLeptonData.iso = Elec2->rhoIso();
	  probeLeptonData.isNotConv = Elec2->isNotConv();

                       
	  if((fabs(Elec2->ECalP4().Eta()) > 1.4442 && fabs(Elec2->ECalP4().Eta()) < 1.5660) ||  Elec2->ECalP4().Pt() <= PtCut || fabs(Elec2->eta()) > EtaCut || Elec->q()*Elec2->q()>0 || (Elec->p4()+ Elec2->p4()).M() > maxMass || (Elec->p4()+ Elec2->p4()).M() < minMass ) continue;//|| fabs(Elec2->vertexZ()-Elec->vertexZ())/sqrt(pow(Elec2->vertexZErr(),2)+pow(Elec->vertexZErr(),2)) > 9) continue;  
	  ++foundProbe;
	  h2_DEN->Fill(Elec2->p4().eta(),Elec2->ECalP4().Pt(),EvWeight);

	  if(Elec2->mvaId() > mvaCut && Elec2->isNotConv() && fabs(Elec2->d0V()) < d0Cut && Elec2->isPf() && Elec2->mHits() <= maxmHits ) { 
	    h2_NUMIdEff->Fill(Elec2->p4().eta(),Elec2->ECalP4().Pt(),EvWeight);
	    //h_NUMJets->Fill(pNTEventData->vertexMulti(),EvWeight);
	    h_NUMJets->Fill(numjet,EvWeight);
	    if(doRhoIso && Elec2->rhoIso()< IsoCut) {h_IsoJets->Fill(numjet,EvWeight);h2_NUMIsoEff->Fill(Elec2->p4().eta(),Elec2->ECalP4().Pt(),EvWeight); } 
	    if(!doRhoIso && Elec2->isoVal() < IsoCut) {h_IsoJets->Fill(pNTEventData->vertexMulti(),EvWeight);h2_NUMIsoEff->Fill(Elec2->p4().eta(),Elec2->ECalP4().Pt(),EvWeight); }
	  }
                     
	  ProbeElecVar->Fill(); 

	} 
      }

    }
  }//allfiles
  h2_DEN->Sumw2(); h2_NUMIdEff->Sumw2();h2_NUMIsoEff->Sumw2(); 
  histo_IdEff_Data->Divide(h2_NUMIdEff,h2_DEN,1,1,"B");
  histo_IsoEff_Data->Divide(h2_NUMIsoEff,h2_NUMIdEff,1,1,"B");  
  
  TTree * tMC = (TTree*) fMC->Get("/PFTree/PFTree");

  top::NTEvent * pNTEvent = 0;
  tMC->SetBranchAddress("NTEvent",&pNTEvent);
  
  ztop::PUReweighter test;

  test.setMCDistrSummer11Leg();

  test.setDataTruePUInput("/afs/desy.de/user/k/kiesej/public/Legacy2011.json_PU.root");


  vector<ztop::NTElectron> * pElecs2 = 0;
  tMC->SetBranchAddress("NTElectrons",&pElecs2); //sets the pointer to the vector<NTElec>

  int nMC = (int) tMC->GetEntries();
  std::cout << " MC Tree Entries " << nMC << std::endl;

  //nMC = 100000;
  std::vector<top::NTJet> * pJetMC = 0;
  tMC->SetBranchAddress("NTJets",&pJetMC); //sets the pointer to the vector<NTElec>

  top::NTMet * pMET2 = 0;
  tMC->SetBranchAddress("NTMet",&pMET2);

  std::vector<bool> *trigMC=0;
  tMC->SetBranchAddress("TriggerBools",&trigMC);

  double EvWeightMC = 1.0;
  for (int i = 0; i < nMC; i++) {
    tMC->GetEntry(i);
    if(!trigMC->at(9)) continue;
 
    EvWeightMC = test.getPUweight(pNTEvent->truePU());

    if(pMET2->met() > 40.0 || pElecs2->size()>2) continue; 

    int numjet = 0, haybjet = 0;
    /*   for(std::vector<top::NTJet>::iterator jet=pJetMC->begin(); jet < pJetMC->end(); jet++) {
	 if(jet->pt()>30. && fabs(jet->eta()) < 2.4 && jet->id() >0) {
	 numjet++;
	 //        if(jet->btag() > 0.244) haybjet++;
	 }
	 }*/
    //    if(numjet < 2 || !haybjet) continue;
    //    if(numjet < 2 || pElecs2->size()>2) continue;


    for(std::vector<top::NTElectron>::iterator Elec=pElecs2->begin(); Elec != pElecs2->end(); Elec++){
      if(Elec->mHits() > maxmHits || !Elec->isNotConv() || fabs(Elec->d0V()) >= d0Cut || (doRhoIso && Elec->rhoIso()>= IsoCut) || (!doRhoIso && Elec->isoVal() >= IsoCut ) || Elec->pt() < 30.0 || fabs(Elec->eta()) > EtaCut || !Elec->isPf() || Elec->mvaId() <= mvaCut) continue; 
 
      int tag = 0;
      for(size_t j=0; j<Elec->matchedTrig().size(); j++){
	TString checkName(Elec->matchedTrig()[j]); //std::cout<< "hay tag" <<Elec->matchedTrig()[j]<<std::endl;
	if(checkName.Contains("HLT_Ele17_CaloIdVT_CaloIsoVT_TrkIdT_TrkIsoVT_SC8_Mass3") || checkName.Contains("HLT_Ele27")){
	  //    if(checkName.Contains("HLT_Ele27_WP80_v")){
	  tag++; break;} //std::cout<< "hay tag" <<endl;   
      }
      if(tag <1 ) continue;
 
      //TRIGGER
      int foundProbe = 0;
      for(std::vector<top::NTElectron>::iterator Elec2=pElecs2->begin(); Elec2 < pElecs2->end(); Elec2++){
	if (foundProbe>0) break;
	probeLeptonMC.pt =  Elec2->ECalP4().Pt();
	probeLeptonMC.eta = Elec2->eta();
	probeLeptonMC.d0 = Elec2->d0V();
	probeLeptonMC.isPF = Elec2->isPf();
	probeLeptonMC.mHits = Elec2->mHits();
	probeLeptonMC.InvMass = (Elec->p4()+ Elec2->p4()).M();
	probeLeptonMC.id = Elec2->mvaId();
	probeLeptonMC.iso = Elec2->rhoIso();
	probeLeptonMC.isNotConv = Elec2->isNotConv();

	if((fabs(Elec2->ECalP4().Eta()) > 1.4442 && fabs(Elec2->ECalP4().Eta()) < 1.5660) ||  Elec2->ECalP4().Pt() <= PtCut || fabs(Elec2->eta()) > EtaCut || Elec->q()*Elec2->q()>0 || (Elec->p4()+ Elec2->p4()).M() > maxMass || (Elec->p4()+ Elec2->p4()).M() < minMass ) continue;
	++foundProbe;

	h2_DENMC->Fill(Elec2->p4().eta(),Elec2->ECalP4().Pt(),EvWeightMC);
	if(Elec2->mvaId() > mvaCut && Elec2->isNotConv() && fabs(Elec2->d0V()) < d0Cut && Elec2->isPf() && Elec2->mHits() <= maxmHits ) {
	  h2_NUMIdEffMC->Fill(Elec2->p4().eta(),Elec2->ECalP4().Pt(),EvWeightMC);
	  //                          h_NUMJetsMC->Fill(pJetMC->size(),EvWeightMC);
	  h_NUMJetsMC->Fill(numjet,EvWeightMC);
	  if(doRhoIso && Elec2->rhoIso()< IsoCut) {h2_NUMIsoEffMC->Fill(Elec2->p4().eta(),Elec2->ECalP4().Pt(),EvWeightMC);
	    h_IsoJetsMC->Fill(numjet,EvWeightMC);}
	  if(!doRhoIso && Elec2->isoVal() < IsoCut){h2_NUMIsoEffMC->Fill(Elec2->p4().eta(),Elec2->ECalP4().Pt(),EvWeightMC);
	    h_IsoJetsMC->Fill(pNTEvent->vertexMulti(),EvWeightMC);}
	}

	ProbeElecVarMC->Fill(); 
      } 
    }
  } 
  h2_DENMC->Sumw2(); h2_NUMIdEffMC->Sumw2();h2_NUMIsoEffMC->Sumw2(); 
  histo_IdEff_MC->Divide(h2_NUMIdEffMC,h2_DENMC,1,1,"B");
  histo_IsoEff_MC->Divide(h2_NUMIsoEffMC,h2_NUMIdEffMC,1,1,"B");
  histo_IdEff_MC->Sumw2(); histo_IsoEff_MC->Sumw2();

  h2_IdScaleFactor->Divide(histo_IdEff_Data,histo_IdEff_MC,1,1,"B");
  h2_IsoScaleFactor->Divide(histo_IsoEff_Data,histo_IsoEff_MC,1,1,"B");
  h2_GlobalScaleFactor->Multiply(h2_IsoScaleFactor,h2_IdScaleFactor,1,1);  
  

  fout->cd();
  TObjArray Hlist(0);
  Hlist.Add(ProbeElecVar);
  Hlist.Add(ProbeElecVarMC);
  Hlist.Add(histo_IdEff_Data);
  Hlist.Add(histo_IsoEff_Data);
  Hlist.Add(h2_DEN);
  Hlist.Add(h2_NUMIdEff);
  Hlist.Add(h2_NUMIsoEff);
 
  Hlist.Add(histo_IdEff_MC);
  Hlist.Add(histo_IsoEff_MC);
  Hlist.Add(h2_DENMC);
  Hlist.Add(h2_NUMIdEffMC);
  Hlist.Add(h2_NUMIsoEffMC);

  Hlist.Add(h2_IdScaleFactor);
  Hlist.Add(h2_IsoScaleFactor);
  Hlist.Add(h2_GlobalScaleFactor);
 
  Hlist.Add(h_IsoJetsMC);
  Hlist.Add(h_NUMJetsMC);

  Hlist.Add(h_IsoJets);
  Hlist.Add(h_NUMJets); 
 
 
  Hlist.Write();  fout->Close();   Hlist.Clear();

  double ideff = h2_NUMIdEff->Integral()/h2_DEN->Integral();
  double isoeff = h2_NUMIsoEff->Integral()/h2_NUMIdEff->Integral();
  double errideff = sqrt(ideff*(1-ideff)/h2_DEN->Integral());
  double errisoeff = sqrt(isoeff*(1-isoeff)/h2_NUMIdEff->Integral());

  double ideffMC = h2_NUMIdEffMC->Integral()/h2_DENMC->Integral();
  double isoeffMC = h2_NUMIsoEffMC->Integral()/h2_NUMIdEffMC->Integral();
  double errideffMC = sqrt(ideffMC*(1-ideffMC)/h2_DENMC->Integral());
  double errisoeffMC = sqrt(isoeffMC*(1-isoeffMC)/h2_NUMIdEffMC->Integral());

  double idSF = ideff/ideffMC;
  double erridSF = idSF*(errideff/ideff+errideffMC/ideffMC);

  double isoSF = isoeff/isoeffMC;
  double errisoSF = isoSF*(errisoeff/isoeff+errisoeffMC/isoeffMC);


  char globalEffs[255];
  sprintf(globalEffs,"ElecEff_gsfRho_eta24TightMET402Elec76106.txt"); 
  ofstream myfile;

  myfile.open(globalEffs);
  myfile << "\t &Data \t& Simulation \t& Scale Factor \\\\ \\\hline" <<endl;
  myfile << "Id Efficiency \t&"<< ideff <<"$\\\pm$"<< errideff  <<"\t&"<< ideffMC <<"$\\\pm$"<< errideffMC <<"\t&"<< idSF << "$\\\pm$"<< erridSF<<"\\\\ \\\hline" <<endl;
  myfile << "Iso Efficiency \t&"<< isoeff <<"$\\\pm$"<< errisoeff  <<"\t&"<< isoeffMC <<"$\\\pm$"<< errisoeffMC <<"\t&"<< isoSF << "$\\\pm$"<< errisoSF<<"\\\\ \\\hline" <<endl;
  myfile << "Global Efficiency \t&"<< isoeff*ideff <<"$\\\pm$"<< sqrt((1-isoeff*ideff)*isoeff*ideff/h2_DEN->Integral())  <<"\t&"<< isoeffMC*ideffMC <<"$\\\pm$"<< sqrt((1-isoeffMC*ideffMC)*isoeffMC*ideffMC/h2_DENMC->Integral()) <<"\t&"<< isoSF*idSF << "$\\\pm$"<< errisoSF<<"\\\\ \\\hline" <<endl;
  myfile.close();

}




