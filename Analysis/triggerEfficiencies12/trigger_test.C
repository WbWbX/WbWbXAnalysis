#include "triggerAnalyzer_base2.h"



double 
triggerAnalyzer::selectDileptons(std::vector<ztop::NTMuon> * inputMuons, std::vector<ztop::NTElectron> * inputElectrons){

  using namespace std;
  using namespace ztop;

  size_t tightidx=99999;

  std::vector<ztop::NTMuon*> tightmuons,loosemuons;

  for(size_t i=0;i<inputMuons->size();i++){
    ztop::NTMuon * muon = &inputMuons->at(i);

    if(muon->pt() < 20) continue;
    if(muon->isoVal() > 0.12) continue;
    if(fabs(muon->eta()) > 2.1) continue;
    if(!muon->isGlobal()) continue;
    if(!muon->isPf()) continue;

    if(muon->trkHits()<=5) continue;
    if(muon->normChi2()>10) continue;
    if(muon->pixHits()<=0) continue;
    if(muon->muonHits()<=1) continue;
    if(fabs(muon->d0V()) > 0.2) continue;
    if(fabs(muon->dzV()) > 0.5) continue;

    tightidx=i;
    tightmuons  << muon;
    break;
  }

  for(size_t i=0;i<inputMuons->size();i++){
    ztop::NTMuon * muon = &inputMuons->at(i);


    if(i==tightidx) continue; //avoid double-counting

    if(muon->pt() < 10) continue;
    if(muon->isoVal() > 0.2) continue;
    if(fabs(muon->eta()) > 2.5) continue;
    if(!(muon->isGlobal() || muon->isTracker())) continue;
    if(!muon->isPf()) continue;
    
    loosemuons << muon;    
  }

  // the tight muon is NOT included in the loosemuon vector
  // selectedMuons_

  vector<NTElectron*> tightelecs,looseelecs;
  tightidx=99999;

  for(size_t i=0;i<inputElectrons->size();i++){
    ztop::NTElectron * elec = &inputElectrons->at(i);

    if(elec->pt() < 20) continue;
    if(elec->isoVal() > 0.1) continue;
    if(fabs(elec->eta()) > 2.5) continue;
    if(elec->mvaId()<0) continue;
    if(!elec->isNotConv()) continue;
    if(fabs(elec->d0V()) > 0.02) continue;
    if(fabs(elec->dzV()) > 1) continue;

    tightelecs << elec;
    tightidx=i;
    break;
  }

  for(size_t i=0;i<inputElectrons->size();i++){
    ztop::NTElectron * elec = &inputElectrons->at(i);

    if(tightidx == i) continue;

    if(elec->pt() < 15) continue;
    if(elec->isoVal()>0.2) continue;
    if(fabs(elec->eta()) > 2.5) continue;
    if(elec->mvaId() < 0) continue;
    if(!elec->isNotConv()) continue;

    looseelecs << elec;
  }
  //do filling only for plots
  selectedMuons_ << tightmuons << loosemuons;
  selectedElecs_ << tightelecs << looseelecs;
  //make sure they are ordered by pt!! right now NOT the case!!

  //FIX

  //require one tight and exactly one loose lepton in addition


  double mass=0;

  LorentzVector dilp4;
  if(mode_<0){ //ee
    if(tightelecs.size() != 1 || looseelecs.size() != 1) return 0;
    if(tightelecs.at(0)->q() == looseelecs.at(0)->q())   return 0;
    if(!noOverlap(tightelecs.at(0), looseelecs.at(0) , 0.2)) return 0;
    mass=(tightelecs.at(0)->p4() + looseelecs.at(0)->p4()).M();
  }
  else if(mode_==0){ //emu
    std::vector<ztop::NTMuon*> allselmuons;
    allselmuons << tightmuons << loosemuons;
    std::vector<ztop::NTElectron*> allselelecs;
    allselelecs << tightelecs << looseelecs;

    bool tightmutighte = tightmuons.size() == 1 && tightelecs.size() == 1;
    bool tightmuloosee = tightmuons.size() == 1 && looseelecs.size() == 1;
    bool loosemutighte = loosemuons.size() == 1 && tightelecs.size() == 1;

    if(!(tightmutighte || tightmuloosee || loosemutighte)) return 0;
    //require exactly one of each
    if(allselmuons.size() != 1 || allselelecs.size() != 1) return 0;

    //oppo charge
    if(allselmuons.at(0)->q() == allselelecs.at(0)->q()) return 0;
    if(!noOverlap(allselmuons.at(0), allselelecs.at(0) , 0.2)) return 0;

    mass=(allselmuons.at(0)->p4() + allselelecs.at(0)->p4()).M();

  }
  else{ //mumu
    if(tightmuons.size() != 1 || loosemuons.size() != 1) return 0;
    if(tightmuons.at(0)->q() == loosemuons.at(0)->q())   return 0;
    if(!noOverlap(tightmuons.at(0), loosemuons.at(0) , 0.2)) return 0;
    mass=(tightmuons.at(0)->p4() + loosemuons.at(0)->p4()).M();
  }



  return mass;

}



void trigger_test(){

  

  using namespace std;
  using namespace ztop;

  std::vector<float> binsmumueta, bins2dee, bins2dmue,binsptmu, binspte, bins2dmumu;
  
  binsmumueta.push_back(-2.5);binsmumueta.push_back(-2.1);binsmumueta.push_back(-1.2);binsmumueta.push_back(-0.9);binsmumueta.push_back(0.9);binsmumueta.push_back(1.2);binsmumueta.push_back(2.1);binsmumueta.push_back(2.5);

  bins2dee << 0 << 1.479 << 2.5;
  bins2dmue << 0 << 0.9 << 2.5;
  bins2dmumu << 0 << 0.9 << 1.2 << 2.1 << 2.5;

  binsptmu << 10 << 20 << 25 << 30 << 35 << 40 << 50 << 60 << 100 << 200;
  binspte << 15 << 20 << 25 << 30 << 35 << 40 << 50 << 60 << 100 << 200;

  bool includecorr=true;

  triggerAnalyzer ta_eed;
  triggerAnalyzer ta_mumud;
  triggerAnalyzer ta_emud;

  ta_eed.setMode("ee");
  ta_eed.setMassCut(12);
  ta_eed.setIncludeCorr(includecorr);
  ta_emud.setMode("emu");
  ta_emud.setMassCut(12);
  ta_eed.setIncludeCorr(includecorr);
  ta_mumud.setMode("mumu");
  ta_mumud.setMassCut(12);
  ta_eed.setIncludeCorr(includecorr);

  ta_mumud.setBinsEta(binsmumueta);
  ta_mumud.setBinsEta2dX(bins2dmumu);
  ta_mumud.setBinsEta2dY(bins2dmumu);
  ta_mumud.setBinsPt(binsptmu);

  ta_eed.setBinsPt(binspte);
  ta_eed.setBinsEta2dX(bins2dee);
  ta_eed.setBinsEta2dY(bins2dee);

  ta_emud.setBinsEta2dX(bins2dee);
  ta_emud.setBinsEta2dY(bins2dmue);
  ta_emud.setBinsPt(binsptmu);


  triggerAnalyzer ta_eeMC=ta_eed;
  triggerAnalyzer ta_mumuMC=ta_mumud;
  triggerAnalyzer ta_emuMC=ta_emud;

  ta_eeMC.setIsMC(true);
  ta_mumuMC.setIsMC(true);
  ta_emuMC.setIsMC(true);



  TString dir="/scratch/hh/dust/naf/cms/user/kieseler/trees_ES_tth/";

  TString cmssw_base=getenv("CMSSW_BASE");
  TString pileuproot = cmssw_base+"/src/TtZAnalysis/Data/ttH.json_PU.root";//HCP_PU.root";//HCP_5.3fb_PU.root";


  std::vector<TString> eemcfiles,mumumcfiles,emumcfiles, datafiles;
  eemcfiles << dir+"tree_8TeV_eettbar.root"
    	    << dir+"tree_8TeV_eettbarviatau.root" ;

  mumumcfiles << dir+"tree_8TeV_mumuttbar.root" 
	      << dir+"tree_8TeV_mumuttbarviatau.root" ;

  emumcfiles << dir+"tree_8TeV_emuttbar.root"
    	     << dir+"tree_8TeV_emuttbarviatau.root";


  datafiles  << dir + "tree_8TeV_MET_runA_06Aug.root"  
	     << dir + "tree_8TeV_MET_runB_13Jul.root"  
	     << dir + "tree_8TeV_MET_runC_prompt.root"
	     << dir + "tree_8TeV_MET_runA_13Jul.root"  
	     << dir + "tree_8TeV_MET_runC_24Aug.root"  
	     << dir + "tree_8TeV_MET_runD_prompt.root";
  

  
  ta_eeMC.setPUFile(pileuproot);
  ta_mumuMC.setPUFile(pileuproot);
  ta_emuMC.setPUFile(pileuproot);

  TChain * datachain=makeChain(datafiles);

  TChain * eechain=makeChain(eemcfiles);
  TChain * mumuchain=makeChain(mumumcfiles);
  TChain * emuchain=makeChain(emumcfiles);

  ta_eeMC.setChain(eechain);
  ta_mumuMC.setChain(mumuchain);
  ta_emuMC.setChain(emuchain);

  ta_eed.setChain(datachain);
  ta_mumud.setChain(datachain);
  ta_emud.setChain(datachain);

  ta_mumud.Eff();
  ta_mumuMC.Eff();


  makeFullOutput(ta_mumud, ta_mumuMC, "testdir", "testRun", 0.01);

  // std::vector<histWrapper> sfs=getAllSFs(ta_mumud,ta_mumuMC,0.01); //last relerror

 //  TFile *f = new TFile("trigger_mumu_sf.root","RECREATE");
 //  for(size_t i=0;i<sfs.size();i++)
 //    sfs.at(i).write();
 //  f->Close();
  
 //  TFile *f2 = new TFile("trigger_mumu_raw.root","RECREATE");
 //  ta_mumud.writeAll();
 //  ta_mumuMC.writeAll("MC");
 //  f2->Close();

 // //make plots

 //  TFile *f2 = new TFile("trigger_plots_mumu.root","RECREATE");
 
 

 //  plotAll(sfs,"testRun","plots_test/");
 //  f2->Close();

  


  // analyze( ta_eed,  ta_eeMC,  ta_mumud,  ta_mumuMC,  ta_emud,  ta_emuMC);
  
  //miniscript();
}


/*
plots now have appropriate style, analyzer can be run without doing all channels, new plots can be implemented easily

still missing: actual plotting step. exploit ordering in std::vector<histWrapper> sfs=getAllSFs(ta_eed,ta_eeMC,0.01) and the fact that isTH1D() can be required for overlay plots.

also possible: do plots overlaying run ranges etc just by combining the same entries etc...

*/
  
