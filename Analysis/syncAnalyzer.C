#include "../DataFormats/src/classes.h" //gets all the dataformats
#include "../DataFormats/interface/elecRhoIsoAdder.h"
#include "../plugins/reweightPU.h"
#include "../plugins/leptonSelector.h"
#include "../Tools/interface/miscUtils.h"
//#include "../plugins/containerStackVector.h"
//#include "../plugins/JERAdjuster.h"
//#include "../plugins/JECUncertainties.h"
#include "TTree.h"
#include "TFile.h"
#include <fstream>
#include <iostream>

namespace top{
  typedef std::vector<top::NTElectron>::iterator NTElectronIt;
  typedef std::vector<top::NTMuon>::iterator NTMuonIt;
  typedef std::vector<top::NTJet>::iterator NTJetIt;
  typedef std::vector<top::NTTrack>::iterator NTTrackIt;
  typedef std::vector<top::NTSuClu>::iterator NTSuCluIt;
}

//need function for find container in vector by name and create if not existing
void do_sync(const char * file){

  using namespace top;
  using namespace std;

  TFile * f = new TFile(file);
  TTree * t = (TTree*) f->Get("PFTree/pfTree");

  top::leptonSelector lepSel;
  lepSel.setUseRhoIsoForElectrons(false); ///FIRST TRY WITHOUT RHO ISO

  vector<NTMuon> * pMuons = 0;
  t->SetBranchAddress("NTMuons",&pMuons); 
  vector<NTElectron> * pElectrons = 0;
  t->SetBranchAddress("NTElectrons",&pElectrons);
  vector<NTJet> * pJets=0;
  t->SetBranchAddress("NTJets",&pJets);
  NTMet * pMet = 0;
  t->SetBranchAddress("NTMet",&pMet); 
  NTEvent * pEvent = 0;
  t->SetBranchAddress("NTEvent",&pEvent); 



  ////just some counters

  double totalkinelectrons=0;
  double totalkinmuons=0;

  double totalidelectrons=0;
  double totalidmuons=0;

  double totalisoelectrons=0;
  double totalisomuons=0;


  double totalhardjets=0;


  // start main loop /////////////////////////////////////////////////////////
  Long64_t nEntries=t->GetEntries();

  Long64_t maxevents=50000;

  if(maxevents < nEntries) nEntries=maxevents;

  cout << "\n\nrunning on maximum " << nEntries << " entries." <<endl;

  for(Long64_t entry=0;entry<nEntries;entry++){

    displayStatusBar(entry,nEntries);
    t->GetEntry(entry);

    elecRhoIsoAdder rho(true,false);

    vector<NTElectron> kinelectrons;

    ////// clean electrons against muons and make kin cuts on the fly//////
    for(NTElectronIt elec=pElectrons->begin();elec<pElectrons->end();++elec){
      if(elec->pt() < 20) continue;
      if(fabs(elec->eta()) > 2.5) continue;
      if(!noOverlap(elec, *pMuons, 0.1)) continue;
      
      kinelectrons.push_back(*elec);
    }
    
    rho.setRho(pEvent->isoRho(2));
    rho.addRhoIso(kinelectrons);
    vector<NTMuon> kinmuons         = lepSel.selectKinMuons(*pMuons);
    
    vector<NTElectron> idelectrons   =lepSel.selectIDElectrons(kinelectrons);
    vector<NTMuon> idmuons           =lepSel.selectIDMuons(kinmuons);

    vector<NTElectron> isoelectrons  =lepSel.selectIsolatedElectrons(idelectrons);
    vector<NTMuon> isomuons          =lepSel.selectIsolatedMuons(idmuons);

    totalkinelectrons+=kinelectrons.size();
    totalkinmuons+=kinmuons.size();
    totalidelectrons+=idelectrons.size();
    totalidmuons+=idmuons.size();
    totalisoelectrons+=isoelectrons.size();
    totalisomuons+=isomuons.size();



    vector<NTJet> hardjets;



    for(NTJetIt jet=pJets->begin();jet<pJets->end();++jet){ //some kin cuts plus  id overlaps commented!
      if(jet->pt()<30) continue;
      if(fabs(jet->eta())>2.5) continue;
      if(!(jet->id())) continue;
      // if(!noOverlap(jet, isomuons, 0.3)) continue;
      // if(!noOverlap(jet, isoelectrons, 0.3)) continue;
      hardjets.push_back(*jet);
    }

    totalhardjets+=hardjets.size();

  }

  cout << "\nkinelecs: " << totalkinelectrons << "\n"
       << "idelectrons: " << totalidelectrons << "\n"
       << "isoelectrons: " << totalisoelectrons << "\n\n"
       << "kinmuons: " << totalkinmuons << "\n"
       << "idmuons: " << totalidmuons << "\n"
       << "isomuons: " << totalisomuons << "\n\n" 
       << "hardjets: " << totalhardjets << "\n"
       << endl;
  cout << "hardjets definition might be the same as in real analysis! doesn't matter for sync." << endl;


  f->Close(); //deletes all pointers
  delete f;

}




void syncAnalyzer(){

  do_sync("/scratch/hh/dust/naf/cms/user/kieseler/trees0822/tree_8TeV_eettbar.root");

  // do_sync("");

}
