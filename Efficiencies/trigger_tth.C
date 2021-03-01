#include "triggerAnalyzer_base3.h"



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
    if(muon->muonHits()<=0) continue;
    if(fabs(muon->d0V()) > 0.2) continue;
    if(fabs(muon->dzV()) > 0.5) continue;

    if(fabs(muon->getMember(0)<=1)) continue; //matched stations

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
    if(elec->rhoIso() > 0.1) continue;
    if(fabs(elec->eta()) > 2.5) continue;

    if(fabs(elec->suClu().p4().Eta()) > 1.4442 && fabs(elec->suClu().p4().Eta()) < 1.5660) continue;

    if(elec->storedId()<0.5) continue;
    if(elec->mHits()>0)continue;

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

    if(fabs(elec->suClu().p4().Eta()) > 1.4442 && fabs(elec->suClu().p4().Eta()) < 1.5660) continue;

    if(elec->pt() < 10) continue;
    if(elec->rhoIso()>0.2) continue;
    if(fabs(elec->eta()) > 2.5) continue;
    if(elec->storedId() < 0.5) continue;
    if(!elec->isNotConv()) continue;
    if(elec->d0V() > 0.04) continue;
    if(elec->mHits()>0)continue;

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



void trigger_tth(){

  //triggerAnalyzer::testmode=true;

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
  ta_eed.setMassCutLow(12);
  ta_eed.setIncludeCorr(includecorr);
  ta_emud.setMode("emu");
  ta_emud.setMassCutLow(12);
  ta_eed.setIncludeCorr(includecorr);
  ta_mumud.setMode("mumu");
  ta_mumud.setMassCutLow(12);
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



  TString dir="/scratch/hh/dust/naf/cms/user/kieseler/trees_Jun13_04/";



TString cmssw_base=getenv("CMSSW_BASE");
  TString PURunA = cmssw_base+"/src/WbWbXAnalysis/Data/RunAComp.json.txt_PU.root";
  TString PURunB = cmssw_base+"/src/WbWbXAnalysis/Data/RunB13Jul.json.txt_PU.root";
  //  TString PURunAB = cmssw_base+"/src/WbWbXAnalysis/Data/RunABComp_PU.root";
  TString PURunC = cmssw_base+"/src/WbWbXAnalysis/Data/RunCComp.json.txt_PU.root";
  TString PURunD = cmssw_base+"/src/WbWbXAnalysis/Data/RunDprompt.json.txt_PU.root";

  TString pileuproot = cmssw_base+"/src/WbWbXAnalysis/Data/Full19.json.txt_PU.root";


  std::vector<TString> mumumcfiles, eemcfiles, emumcfiles, datafilesFull,datafilesRunB,datafilesRunAB,datafilesRunC, datafilesRunA, datafilesRunD;
 
  mumumcfiles << dir+"tree_8TeV_mumuttbar.root" 
	      << dir+"tree_8TeV_mumuttbarviatau.root" ;

  eemcfiles << dir+"tree_8TeV_eettbar.root" 
	    << dir+"tree_8TeV_eettbarviatau.root" ;

  emumcfiles << dir+"tree_8TeV_emuttbar.root" 
	     << dir+"tree_8TeV_emuttbarviatau.root" ;

  datafilesFull  << dir + "tree_8TeV_MET_runA_06Aug.root"  
		 << dir + "tree_8TeV_MET_runA_13Jul.root"  
		 << dir + "tree_8TeV_MET_runB_13Jul.root"  
		 << dir + "tree_8TeV_MET_runC_prompt.root"
		 << dir + "tree_8TeV_MET_runC_24Aug.root"  
    // << dir + "tree_8TeV_MET_runC_11Dec.root"  
		 << dir + "tree_8TeV_MET_runD_prompt.root";

  datafilesRunA  << dir + "tree_8TeV_MET_runA_06Aug.root"  
	 	 << dir + "tree_8TeV_MET_runA_13Jul.root";

  datafilesRunB  << dir + "tree_8TeV_MET_runB_13Jul.root";


  datafilesRunAB  << datafilesRunA << datafilesRunB;

  datafilesRunC  << dir + "tree_8TeV_MET_runC_prompt.root"
		 << dir + "tree_8TeV_MET_runC_24Aug.root"  ;
    //	 << dir + "tree_8TeV_MET_runC_11Dec.root";

  datafilesRunD  << dir + "tree_8TeV_MET_runD_prompt.root";
  


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




  //  analyzeAll( ta_eed,  ta_eeMC,  ta_mumud,  ta_mumuMC,  ta_emud,  ta_emuMC,"full","19 fb^{-1}");





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

    analyzeAll( ta_eedD,  ta_eeMCD,  ta_mumudD,  ta_mumuMCD,  ta_emudD,  ta_emuMCD,"RunD","Run D");



    double lumiA=0.890;
    double lumiB=4.430;
    double lumiC=7.026;
    double lumiD=7.272;

    double lumitotal=lumiA+lumiB+lumiC+lumiD;

    //for ee
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
    triggerAnalyzer eemcFull = ta_eeMCA + ta_eeMCB + ta_eeMCC + ta_eeMCD;
   TString eestring= makeFullOutput(eedFull, eemcFull, "ee_Full", "ee Full 19 fb^{-1}", 0.01);

    ////
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
    triggerAnalyzer mumumcFull = ta_mumuMCA + ta_mumuMCB + ta_mumuMCC + ta_mumuMCD;
   TString mumustring= makeFullOutput(mumudFull, mumumcFull, "mumu_Full", "mumu Full 19 fb^{-1}", 0.01);


    ///
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
    triggerAnalyzer emumcFull = ta_emuMCA + ta_emuMCB + ta_emuMCC + ta_emuMCD;
    TString emustring=makeFullOutput(emudFull, emumcFull, "emu_Full", "emu Full 19 fb^{-1}", 0.01);

    std::cout << "\n\nweighted output summary: " << std::endl;

    std::cout << "channel  & $\\epsilon_{data}$ & $\\epsilon_{MC}$ & SF & $\\alpha$ \\\\ " << std::endl;
    std::cout << eestring<< std::endl;
    std::cout << mumustring<< std::endl;
    std::cout << emustring<< std::endl;


}

  /*

  TString cmssw_base=getenv("CMSSW_BASE");
  TString pileuproot = cmssw_base+"/src/WbWbXAnalysis/Data/Full19.json.txt_PU.root";//HCP_PU.root";//HCP_5.3fb_PU.root";


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
  */ 

  /*
  ta_mumud.Eff();
  ta_mumuMC.Eff();
  */

  //  makeFullOutput(ta_mumud, ta_mumuMC, "testdir", "testRun", 0.01);


  // analyzeAll( ta_eed,  ta_eeMC,  ta_mumud,  ta_mumuMC,  ta_emud,  ta_emuMC);
  
  //miniscript();
  // } 


/*
plots now have appropriate style, analyzer can be run without doing all channels, new plots can be implemented easily

still missing: actual plotting step. exploit ordering in std::vector<histWrapper> sfs=getAllSFs(ta_eed,ta_eeMC,0.01) and the fact that isTH1D() can be required for overlay plots.

also possible: do plots overlaying run ranges etc just by combining the same entries etc...

*/
  
