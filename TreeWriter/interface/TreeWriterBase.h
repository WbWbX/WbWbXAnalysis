// -*- C++ -*-
//
// Package:    TreeWriterBase
// Class:      TreeWriterBase
// 
/**\class TreeWriterBase TreeWriterBase.cc DOSS/TreeWriterBase/src/TreeWriterBase.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Jan Kieseler,,,DESY
//         Created:  Fri May 11 14:22:43 CEST 2012
// $Id: TreeWriterBase.h,v 1.5 2013/06/05 13:20:24 jkiesele Exp $
//
//


// system include files

#ifndef TREEWRITERBASE_H
#define TREEWRITERBASE_H

#include <memory>

// user include files
#include "PhysicsTools/SelectorUtils/interface/JetIDSelectionFunctor.h"
#include "PhysicsTools/SelectorUtils/interface/PFJetIDSelectionFunctor.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include <vector>
#include <string>
#include "DataFormats/Common/interface/TriggerResults.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/PatCandidates/interface/TriggerObject.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "DataFormats/JetReco/interface/GenJet.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "../../DataFormats/interface/NTMuon.h"
#include "../../DataFormats/interface/NTElectron.h"
#include "../../DataFormats/interface/NTJet.h"
#include "../../DataFormats/interface/NTMet.h"
#include "../../DataFormats/interface/NTEvent.h"
#include "../../DataFormats/interface/NTTrack.h"
#include "../../DataFormats/interface/NTSuClu.h"
#include "../../DataFormats/interface/elecRhoIsoAdder.h"
#include "../../DataFormats/interface/NTTrigger.h"
#include "../../DataFormats/interface/NTGenLepton.h"
#include "../../DataFormats/interface/NTGenParticle.h"
#include "../../DataFormats/interface/NTGenJet.h"
#include "../../DataFormats/interface/NTTriggerObject.h"
#include "../../DataFormats/interface/mathdefs.h"
#include "../../DataFormats/interface/NTVertex.h"


#include "DataFormats/Candidate/interface/CompositeCandidate.h"

#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/PatCandidates/interface/TriggerEvent.h"
#include "DataFormats/Common/interface/RefVector.h"
#include <string>
#include <map>

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenRunInfoProduct.h"

#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
//#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"
//#include "PhysicsTools/Utilities/interface/Lumi3DReWeighting.h"

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"

#include "DataFormats/TrackReco/interface/Track.h"

#include <algorithm>

#include <cstring>
#include "../interface/genTools.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"


#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/Common/interface/TriggerResultsByName.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"

#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "HLTrigger/HLTcore/interface/HLTEventAnalyzerAOD.h"

//
// class declaration
//

class TreeWriterBase : public edm::EDAnalyzer {
public:
  // explicit TreeWriterBase(){};
  explicit TreeWriterBase(const edm::ParameterSet&);
      ~TreeWriterBase();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


  virtual ztop::NTMuon makeNTMuon(const pat::Muon &)=0;
  virtual ztop::NTElectron makeNTElectron(const pat::Electron &)=0;
  virtual ztop::NTJet makeNTJet(const pat::Jet &)=0;


  //  protected: //nasty
 
       virtual void beginJob() ;
       virtual void analyze(const edm::Event&, const edm::EventSetup&);
       virtual void endJob() ;

       virtual void beginRun(edm::Run const&, edm::EventSetup const&);
       virtual void endRun(edm::Run const&, edm::EventSetup const&);
       virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
       virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);


       // void beginJob() ;
       // void analyze(const edm::Event&, const edm::EventSetup&);
       // void endJob() ;

       // void beginRun(edm::Run const&, edm::EventSetup const&);
       // void endRun(edm::Run const&, edm::EventSetup const&);
       // void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
       // void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);

      // ----------member data ---------------------------

  bool checkJetID(const pat::Jet &);

  std::vector<bool> checkTriggers(const edm::Event&);
  std::vector<std::string> triggers_;
  std::vector<bool> triggerBools_;
  std::vector<unsigned int> triggerPrescales_;
  std::map<std::string,unsigned int> alltriggerswithprescales_;
  void setTriggers();
  ztop::NTGenParticle makeNTGen(const reco::GenParticle *, const std::map<const reco::GenParticle*, int>&);
  ztop::NTGenParticle  makeNTGen(const reco::GenParticle * p);
  ztop::NTGenJet  makeNTGenJet(const reco::GenJet * p);
  bool isInGenCollection(const reco::GenParticle * , const std::vector<const reco::GenParticle * > &);
  bool isInGenCollection(const reco::GenJet * , const std::vector<const reco::GenJet * > &);
  template<typename t,typename u>
  int findGenMatchIdx(t * , std::vector<u> & , double dR=0.4);
 
  std::vector<std::string> triggerObjects_;
  std::vector<std::vector<ztop::NTTriggerObject> > trigObjVec;
  //HLTConfigProvider hltConfig_;
  

  bool pfElecCands_;
  bool pfMuonCands_;
  bool rho2011_;
  bool usegsf_;

  edm::InputTag muons_, recomuons_, pfelecs_, gsfelecs_,recoelecs_, jets_, met_, vertices_, trigresults_, puinfo_, recotracks_, recosuclus_,rhojetsiso_,rhojetsisonopu_,rhoiso_,pdfweights_, genparticles_, genjets_,mvamet_,pattriggerevent_;
  //rhojets_,rhojetsiso_,rhojetsnopu_,rhojetsisonopu_,rhoiso_;

  bool includereco_, includetrigger_, pfinput_,includepdfweights_,includegen_,susy_,jpsi_;
  TTree * Ntuple;
  std::vector<ztop::NTMuon> ntmuons;
  std::vector<ztop::NTLepton> ntleptons;
  std::vector<ztop::NTElectron> ntpfelectrons;
  std::vector<ztop::NTElectron> ntgsfelectrons;
  std::vector<ztop::NTJet> ntjets;
  std::vector<ztop::NTTrack> nttracks;
  std::vector<ztop::NTSuClu> ntsuclus;
  ztop::NTMet ntmet;
  ztop::NTMet ntmvamet;
  ztop::NTEvent ntevent;
  ztop::NTTrigger nttrigger;

  std::vector<ztop::NTVertex> goodvtx,jetvtx,dimuonvtx;

  std::vector<ztop::NTGenParticle> nttops,ntws,ntzs,ntbs,ntbhadrons,ntnus,ntleps3,ntleps1,ntallnus,ntpart;
  std::vector<ztop::NTGenJet> ntgenjets;
  int channel_; // 11 for ee 13 for mumu 1113 for emu 151113 etc for via tau


  std::vector<float> vstopmass,vchimass;

  bool viaTau_;

  std::string treename_, btagalgo_;

  edm::Service<TFileService> fs;

  reco::VertexCollection vtxs;
  math::XYZPoint beamSpotPosition;


  bool debugmode;

  ///b-hardon stuff

  edm::InputTag bHadJetIdx_, antibHadJetIdx_;
  edm::InputTag BHadrons_, AntiBHadrons_;
  edm::InputTag BHadronFromTopB_, AntiBHadronFromTopB_;
  edm::InputTag BHadronVsJet_, AntiBHadronVsJet_;
  
  edm::InputTag genBHadPlusMothers_, genBHadPlusMothersIndices_;
  edm::InputTag genBHadIndex_, genBHadFlavour_, genBHadJetIndex_;

  bool useBHadrons_;

};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//

#endif
