#include "../../DataFormats/interface/NTElectron.h"
#include "../../DataFormats/interface/NTMuon.h"
#include "../../DataFormats/interface/NTSuClu.h"
#include "../../DataFormats/interface/NTEvent.h"
#include "../../DataFormats/interface/NTTrack.h"
#include "../../leptonSelector.h"
#include <vector>
#include <TString.h>

#include "../../plugins/reweightPU.h"
#include "../../plugins/histo1D.h"

#include <iostream>

#include "../../plugins/miscUtils.h"
#include "effHistoCreater.h"





void createFitHistos(){
  using namespace std;
  using namespace top;

  //allf iles JUST for testing purposes!!! recheck if it gets real!

  TString PuFile="/afs/naf.desy.de/user/k/kieseler/public/dataPileUp_23_06.root";


  vector<float> etabins;etabins.push_back(-2.5);etabins.push_back(-1.5);etabins.push_back(-0.8);etabins.push_back(0.8);etabins.push_back(1.5);
etabins.push_back(2.5);
  vector<float> ptbins;ptbins.push_back(20);ptbins.push_back(30);ptbins.push_back(40);ptbins.push_back(50);ptbins.push_back(800);

  effHistoCreater elecHistos("InvMass_Elecs_SuCluID.root");
  elecHistos.setIsMuons(false);
  elecHistos.setUseRhoIso(true);
  elecHistos.setUseSuClus(true);
  elecHistos.setPtBins(ptbins);
  elecHistos.setEtaBins(etabins);
  elecHistos.setMCFile("/scratch/hh/current/cms/user/kieseler/2012/trees0704/tree_dyee50inf.root.root"); // 
  elecHistos.setDataFile("/scratch/hh/current/cms/user/kieseler/2012/trees0704/tree_singlee_runA_prompt.root.root"); //

  elecHistos.setPuFile(PuFile);
  elecHistos.doIDIsoBoth(0);
  elecHistos.makeFitHistos();

  elecHistos.setOutFile("InvMass_Elecs_ID.root");
  elecHistos.setUseSuClus(false);
  elecHistos.makeFitHistos();

  elecHistos.setOutFile("InvMass_Elecs_IsoRho.root");
  elecHistos.doIDIsoBoth(1);
  elecHistos.makeFitHistos();

  elecHistos.setOutFile("InvMass_Elecs_IsoDB.root");
  elecHistos.setUseRhoIso(false);
  elecHistos.doIDIsoBoth(1);
  elecHistos.makeFitHistos();

  /*
  std::vector<float> binsmumueta;
  binsmumueta.push_back(-2.4);binsmumueta.push_back(-2.1);binsmumueta.push_back(-1.2);binsmumueta.push_back(-0.9);binsmumueta.push_back(0.9);binsmumueta.push_back(1.2);binsmumueta.push_back(2.1);binsmumueta.push_back(2.4);

  effHistoCreater muonHistos("InvMass_Muons_TrackID.root");
  muonHistos.setUseTracks(true);
  muonHistos.setIsMuons(true);
  muonHistos.setPtBins(ptbins);
  muonHistos.setEtaBins(binsmumueta);
  muonHistos.setMCFile("");
  muonHistos.setDataFile("");
  muonHistos.setPuFile(PuFile);
  muonHistos.doIDIsoBoth(0);
  muonHistos.makeFitHistos();
  muonHistos.setOutFile("InvMass_Muons_ID.root");
  muonHistos.setUseTracks(false);
  muonHistos.makeFitHistos();
  muonHistos.setOutFile("InvMass_Muons_Iso.root");
  muonHistos.doIDIsoBoth(1);
  muonHistos.makeFitHistos();

  */

}
