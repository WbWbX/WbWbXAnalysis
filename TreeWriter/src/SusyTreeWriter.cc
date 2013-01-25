// -*- C++ -*-
//
// Package:    SusyTreeWriter
// Class:      SusyTreeWriter
// 
/**\class SusyTreeWriter SusyTreeWriter.cc DOSS/SusyTreeWriter/src/SusyTreeWriter.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Jan Kieseler,,,DESY
//         Created:  Fri May 11 14:22:43 CEST 2012
// $Id: SusyTreeWriter.cc,v 1.8 2013/01/17 19:03:10 jkiesele Exp $
//
//


// system include files
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

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "../../DataFormats/interface/NTInflatedMuon.h"
#include "../../DataFormats/interface/NTInflatedElectron.h"
#include "../../DataFormats/interface/NTJet.h"
#include "../../DataFormats/interface/NTMet.h"
#include "../../DataFormats/interface/NTEvent.h"
#include "../../DataFormats/interface/NTTrack.h"
#include "../../DataFormats/interface/NTSuClu.h"
#include "../../DataFormats/interface/elecRhoIsoAdder.h"
#include "../../DataFormats/interface/NTTrigger.h"
#include "../../DataFormats/interface/NTGenLepton.h"


#include "DataFormats/Candidate/interface/CompositeCandidate.h"

#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "FWCore/Common/interface/TriggerNames.h"

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

//
// class declaration
//

class SusyTreeWriter : public edm::EDAnalyzer {
   public:
      explicit SusyTreeWriter(const edm::ParameterSet&);
      ~SusyTreeWriter();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
 
      virtual void beginJob() ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      virtual void beginRun(edm::Run const&, edm::EventSetup const&);
      virtual void endRun(edm::Run const&, edm::EventSetup const&);
      virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
      virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);

      // ----------member data ---------------------------

  bool checkJetID(std::vector<pat::Jet>::const_iterator);

  bool pfElecCands_;
  bool pfMuonCands_;
  bool rho2011_;
  bool usegsf_;

  edm::InputTag muons_, recomuons_, pfelecs_, gsfelecs_,recoelecs_, jets_, met_, vertices_, trigresults_, puinfo_, recotracks_, recosuclus_,rhojetsiso_,rhojetsisonopu_,rhoiso_,pdfweights_;
  //rhojets_,rhojetsiso_,rhojetsnopu_,rhojetsisonopu_,rhoiso_;

  bool includereco_, includetrigger_, pfinput_,includepdfweights_,includegen_;
  TTree* Ntuple;
  std::vector<top::NTInflatedMuon> ntmuons;
  std::vector<top::NTLepton> ntleptons;
  std::vector<top::NTInflatedElectron> ntpfelectrons;
  std::vector<top::NTInflatedElectron> ntgsfelectrons;
  std::vector<top::NTJet> ntjets;
  std::vector<top::NTTrack> nttracks;
  std::vector<top::NTSuClu> ntsuclus;
  top::NTMet ntmet;
  top::NTEvent ntevent;
  top::NTTrigger nttrigger;

  std::vector<top::NTGenLepton> ntgenmuons, ntgenelecs;
  bool viaTau_;

  std::string treename_, btagalgo_;

  std::vector<float> stopMass,chiMass;
  bool firstevent;

  edm::Service<TFileService> fs;

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
SusyTreeWriter::SusyTreeWriter(const edm::ParameterSet& iConfig)
{
  //now do what ever initialization is needed
  treename_    =iConfig.getParameter<std::string>        ("treeName");
  muons_       =iConfig.getParameter<edm::InputTag>    ( "muonSrc" );
  gsfelecs_       =iConfig.getParameter<edm::InputTag>    ( "elecGSFSrc" );
  pfelecs_       =iConfig.getParameter<edm::InputTag>    ( "elecPFSrc" );
  jets_        =iConfig.getParameter<edm::InputTag>    ( "jetSrc" );
  btagalgo_    =iConfig.getParameter<std::string>       ("btagAlgo");
  met_         =iConfig.getParameter<edm::InputTag>    ( "metSrc" );
  vertices_    =iConfig.getParameter<edm::InputTag>    ( "vertexSrc" );

  includereco_  =iConfig.getParameter<bool>              ( "includeReco" );
  recomuons_    =iConfig.getParameter<edm::InputTag>    ( "recoMuonSrc" );
  pfMuonCands_  =iConfig.getParameter<bool>             ( "isPFMuonCand" );
  recoelecs_    =iConfig.getParameter<edm::InputTag>    ( "recoElecSrc" );
  pfElecCands_  =iConfig.getParameter<bool>             ( "isPFElecCand" );
  recotracks_  =iConfig.getParameter<edm::InputTag>             ( "recoTrackSrc" );
  recosuclus_  =iConfig.getParameter<edm::InputTag>             ( "recoSuCluSrc" );


  includetrigger_  =iConfig.getParameter<bool>               ( "includeTrigger" );
  trigresults_     =iConfig.getParameter<edm::InputTag>    ( "triggerResults" );

  puinfo_          =iConfig.getParameter<edm::InputTag>         ( "PUInfo" );


  rho2011_  =iConfig.getParameter<bool>             ( "includeRho2011" );

  rhojetsiso_       =iConfig.getParameter<edm::InputTag>    ( "rhoJetsIso" );
  rhojetsisonopu_       =iConfig.getParameter<edm::InputTag>    ( "rhoJetsIsoNoPu" );

  rhoiso_       =iConfig.getParameter<edm::InputTag>    ( "rhoIso" );


  includepdfweights_ = iConfig.getParameter<bool>             ( "includePDFWeights" );
  pdfweights_  =iConfig.getParameter<edm::InputTag>    ( "pdfWeights"          );


  includegen_ = iConfig.getParameter<bool>             ( "includeGen" );

   std::cout << "n\n################## Tree writer ########################" 
             <<  "\n#" << treename_
             <<  "\n#                                                     #" 
             <<  "\n#     includes trigger    : " << includetrigger_ <<"                         #"
             <<  "\n#     includes reco       : " << includereco_          <<"                         #" 
             <<  "\n#     includes pdfWeights : " << includepdfweights_    <<"                         #" 
             <<  "\n#                                                     #"
             <<  "\n#    WARNING!!! PF ELECS ARE COMMENTED! NO PF OUTPUT! #"
             <<  "\n#    WARNING!!! Elec collections get 2012 rho corrections by default #"
             <<  "\n#######################################################" << std::endl;


   firstevent=true;

}


SusyTreeWriter::~SusyTreeWriter()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
SusyTreeWriter::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   

   std::string addForPF;
   if(pfinput_) addForPF="bla";
   
   ntmuons.clear();
   ntleptons.clear();
   ntpfelectrons.clear();
   ntgsfelectrons.clear();
   ntjets.clear();
   nttracks.clear();
   ntsuclus.clear();
   ntgenelecs.clear();
   ntgenmuons.clear();

   top::NTTrigger clear;
   nttrigger=clear;

  bool IsRealData = false;
  edm::Handle <reco::GenParticleCollection> genParticles;
  try {
    iEvent.getByLabel("genParticles", genParticles);
    int aux = genParticles->size();
    aux = 0+aux;
  }
  catch(...) {IsRealData = true;} 


   Handle<std::vector<pat::Electron > > pfelecs;
   iEvent.getByLabel(pfelecs_,pfelecs);
   Handle<std::vector<pat::Electron > > gsfelecs;
   iEvent.getByLabel(gsfelecs_,gsfelecs);


   Handle<std::vector<reco::Muon> > recomuons;
   Handle<std::vector<reco::GsfElectron> > recoelecs;
   Handle<std::vector<reco::PFCandidate> > recopfmuons;
   Handle<std::vector<reco::PFCandidate> > recopfelecs;
   Handle<std::vector<reco::Track> > recotracks;
   Handle<std::vector<reco::SuperCluster> > recosuclus;


   if(includereco_){
   
     if(pfElecCands_)
       iEvent.getByLabel(recoelecs_,recopfelecs);
     else
       iEvent.getByLabel(recoelecs_,recoelecs);

     if(pfMuonCands_)
       iEvent.getByLabel(recomuons_,recopfmuons);
     else
       iEvent.getByLabel(recomuons_,recomuons);

     iEvent.getByLabel(recotracks_,recotracks);
     iEvent.getByLabel(recosuclus_,recosuclus);

   }

   Handle<std::vector<pat::Muon > > muons;
   iEvent.getByLabel(muons_,muons);

   Handle<std::vector<pat::Jet> > jets;
   iEvent.getByLabel(jets_,jets);

   Handle<std::vector<pat::MET> > mets;
   iEvent.getByLabel(met_,mets);

   Handle<std::vector<reco::Vertex> > vertices;
   iEvent.getByLabel(vertices_, vertices);
   const reco::VertexCollection vtxs  = *(vertices.product());


   
   if(vtxs.size()>0){


     ///////////////////////////////////////////////E L E C T R O N S//////////
     //recent changes: stares two collections (gsf and pf), ecaldrivenmomentum in BOTH BE CAREFUL WHEN CHANGING
     /*
     for(std::vector<pat::Electron >::const_iterator electron=pfelecs->begin(); electron<pfelecs->end() ; ++electron){
     
       top::NTIsolation Iso;
       
       Iso.setChargedHadronIso(electron->chargedHadronIso());
       Iso.setNeutralHadronIso(electron->neutralHadronIso());
       Iso.setPhotonIso(electron->photonIso());
       Iso.setPuChargedHadronIso(electron->puChargedHadronIso());
	 

       top::NTInflatedElectron tempelec;
       //if(electron->ecalDrivenMomentum())
       tempelec.setECalP4(electron->ecalDrivenMomentum());

       tempelec.setMember("WP80Id");    /// ##CHECK##
       double ipfe_d=0;
       if(electron->isPF()) ipfe_d=1;
       tempelec.setMember("isPF",ipfe_d);
       
       
       tempelec.setP4(electron->p4());
       tempelec.setQ(electron->charge());
       double vz=-9999;
       double vzerr=-9999;
       double dbs=100;
       int mhits=-1;
       if(!(electron->gsfTrack().isNull())){
	 vz=electron->gsfTrack()->dz(vtxs[0].position());                   //
	 vzerr=electron->gsfTrack()->dzError();  
	 dbs=fabs(electron->gsfTrack()->dxy(vtxs[0].position()));

	 mhits=electron->gsfTrack()->trackerExpectedHitsInner().numberOfHits();
       }              //
       else if((electron->closestCtfTrackRef()).isNull()){
	 vz=electron->closestCtfTrackRef()->dz(vtxs[0].position());                   //
	 vzerr=electron->closestCtfTrackRef()->dzError();  
	 dbs=fabs(electron->closestCtfTrackRef()->dxy(vtxs[0].position()));
       }
       tempelec.setVertexZ(vz);                   //
       tempelec.setVertexZErr(vzerr);  

       tempelec.setDbs(dbs);                  //
       tempelec.setNotConv(electron->passConversionVeto());    
       tempelec.setId(electron->electronIDs());
       tempelec.setIso03(Iso);                   //
       //tempelec.setIso04(iso04);                   //
       tempelec.setMHits(mhits);
		       
       if(includereco_){
	 if(electron->triggerObjectMatches().size() ==1){ // no ambiguities
	   tempelec.setMatchedTrig(electron->triggerObjectMatches().begin()->pathNames());
	 }
	 else{
	   std::vector<std::string> def;
	 def.push_back("NoUnamTrigMatch");
	 tempelec.setMatchedTrig(def);
	 }
       }
       double suclue=0;
       if(includereco_ && !(electron->superCluster().isNull())){
	 suclue=electron->superCluster()->rawEnergy();
	 math::XYZPoint suclupoint=electron->superCluster()->position();
	 double magnitude=sqrt(suclupoint.mag2());
	 top::LorentzVector suclup4(suclue*suclupoint.x() / magnitude,suclue*suclupoint.y() / magnitude,suclue*suclupoint.z() / magnitude,suclue);
	 top::NTSuClu suclu;
	 suclu.setP4(suclup4);
	 tempelec.setSuClu(suclu);
       }
       // otherwise (0,0,0,0) taken  care of by NTInflatedElectron Constructor

       if(electron->genParticleRef().isNonnull()) tempelec.setGenP4(electron->genParticleRef()->p4());
     
       ntpfelectrons.push_back(tempelec);


     }

     */
     ////gsf electron/////


     for(std::vector<pat::Electron >::const_iterator electron=gsfelecs->begin(); electron<gsfelecs->end() ; ++electron){
     
       top::NTIsolation Iso;
       
       Iso.setChargedHadronIso(electron->chargedHadronIso());
       Iso.setNeutralHadronIso(electron->neutralHadronIso());
       Iso.setPhotonIso(electron->photonIso());
       Iso.setPuChargedHadronIso(electron->puChargedHadronIso());
	 

       top::NTInflatedElectron tempelec;
       //  if(electron->ecalDrivenMomentum())
       tempelec.setECalP4(electron->ecalDrivenMomentum());

       tempelec.setMember("vbtf11WP80",(double) electron->electronID("vbtf11WP80"));    /// ##CHECK##
       double ipfe_d=0;
       if(electron->isPF()) ipfe_d=1;
       tempelec.setMember("isPFElectron",ipfe_d);
       ;
       
       tempelec.setP4(electron->p4());
       double vz=-9999;
       double vzerr=-9999;
       double dbs=100;

       int mhits=-1;

       if(!(electron->gsfTrack().isNull())){
	 vz=electron->gsfTrack()->dz(vtxs[0].position());                   //
	 vzerr=electron->gsfTrack()->dzError();  
	 dbs=fabs(electron->gsfTrack()->dxy(vtxs[0].position()));

	 mhits=electron->gsfTrack()->trackerExpectedHitsInner().numberOfHits();
       }              //
       else if((electron->closestCtfTrackRef()).isNull()){
	 vz=electron->closestCtfTrackRef()->dz(vtxs[0].position());                   //
	 vzerr=electron->closestCtfTrackRef()->dzError();  
	 dbs=fabs(electron->closestCtfTrackRef()->dxy(vtxs[0].position()));
       }
       tempelec.setVertexZ(vz);                   //
       tempelec.setVertexZErr(vzerr);  

       tempelec.setDbs(dbs);                  //
       tempelec.setNotConv(electron->passConversionVeto());    
       tempelec.setId(electron->electronIDs());
       tempelec.setIso03(Iso);                   //
       //tempelec.setIso04(iso04);                   //
       tempelec.setMHits(mhits);


		       
       if(includereco_){
	 if(electron->triggerObjectMatches().size() ==1){ // no ambiguities
	   tempelec.setMatchedTrig(electron->triggerObjectMatches().begin()->pathNames());
	 }
	 else{
	   std::vector<std::string> def;
	 def.push_back("NoUnamTrigMatch");
	 tempelec.setMatchedTrig(def);
	 }
       }
       double suclue=0;
       if(includereco_ && !(electron->superCluster().isNull())){
	 suclue=electron->superCluster()->rawEnergy();
	 math::XYZPoint suclupoint=electron->superCluster()->position();
	 double magnitude=sqrt(suclupoint.mag2());
	 top::LorentzVector suclup4(suclue*suclupoint.x() / magnitude,suclue*suclupoint.y() / magnitude,suclue*suclupoint.z() / magnitude,suclue);
	 top::NTSuClu suclu;
	 suclu.setP4(suclup4);
	 tempelec.setSuClu(suclu);
       }
       // otherwise (0,0,0,0) taken  care of by NTInflatedElectron Constructor

       if(electron->genParticleRef().isNonnull()) tempelec.setGenP4(electron->genParticleRef()->p4());
     
       ntgsfelectrons.push_back(tempelec);


     }


     /////////////////////////////////// M U O N S//////////////////
   
     for(std::vector<pat::Muon >::const_iterator muon=muons->begin(); muon<muons->end() ; muon++){
       
//        

        top::NTIsolation Iso04;
       // Iso03.setChargedHadronIso(muon->pfIsolationR03().sumChargedHadronPt);
       // Iso03.setNeutralHadronIso(muon->pfIsolationR03().sumNeutralHadronEt); ///
       // Iso03.setPhotonIso(muon->pfIsolationR03().sumPhotonEt);
       // Iso03.setPuChargedHadronIso(muon->pfIsolationR03().sumPUPt);

       top::NTIsolation Iso03;
       Iso03.setChargedHadronIso(muon->chargedHadronIso());
       Iso03.setNeutralHadronIso(muon->neutralHadronIso());
       Iso03.setPhotonIso(muon->photonIso());
       Iso03.setPuChargedHadronIso(muon->puChargedHadronIso());
       // Iso04.setChargedHadronIso(muon->pfIsolationR04().sumChargedHadronPt);
       // Iso04.setNeutralHadronIso(muon->pfIsolationR04().sumNeutralHadronEt); ///
       // Iso04.setPhotonIso(muon->pfIsolationR04().sumPhotonEt);
       // Iso04.setPuChargedHadronIso(muon->pfIsolationR04().sumPUPt);

       top::NTInflatedMuon tempmuon;

       tempmuon.setMember("dB", muon->dB());
       tempmuon.setMember("vPixelHits");
       tempmuon.setMember("matchedStations", muon->numberOfMatchedStations());
       double pfm_d=0;
       if(muon->isPFMuon()) pfm_d=1;
       tempmuon.setMember("isPF",pfm_d);

       //double iso03=0;
       //double iso04=0;

       tempmuon.setP4   (muon->p4());
       tempmuon.setQ   (muon->charge());
       double vz=-9999;
       double vzerr=-9999;
       if(includereco_ && !(muon->innerTrack()).isNull()){
	 top::LorentzVector trkp4(muon->innerTrack()->px(),muon->innerTrack()->py(),muon->innerTrack()->pz(),muon->innerTrack()->p());
	 tempmuon.setTrackP4(trkp4);
       }
       else{
	 top::LorentzVector dummy(0,0,0,0);
	 tempmuon.setTrackP4(dummy);
       }

       if(!(muon->globalTrack()).isNull()){
	 vz=muon->globalTrack()->dz(vtxs[0].position());
	 vzerr=muon->globalTrack()->dzError();
       }
       else if(!(muon->innerTrack()).isNull()){
	 vz=muon->innerTrack()->dz(vtxs[0].position());
	 vzerr=muon->innerTrack()->dzError();
	 
       }
       else if(!(muon->outerTrack()).isNull()){
	 vz=muon->outerTrack()->dz(vtxs[0].position());
	 vzerr=muon->outerTrack()->dzError();
       }
       if(!(muon->innerTrack()).isNull())
	 tempmuon.setMember("vPixelHits",muon->innerTrack()->hitPattern().numberOfValidPixelHits());

       tempmuon.setVertexZ   (vz);
       tempmuon.setVertexZErr   (vzerr);

       tempmuon.setIsGlobal   (muon->isGlobalMuon());
       tempmuon.setIsTracker (muon->isTrackerMuon());

       if(muon->isGlobalMuon()){
	 tempmuon.setNormChi2   (muon->globalTrack()->normalizedChi2());
	 if(!(muon->innerTrack().isNull())) 
	   tempmuon.setTrkHits  (muon->innerTrack()->hitPattern().trackerLayersWithMeasurement());
	 else
	   tempmuon.setTrkHits (0);

	 tempmuon.setMuonHits   (muon->globalTrack()->hitPattern().numberOfValidMuonHits());
	 tempmuon.setDbs   (fabs(muon->globalTrack()->dxy(vtxs[0].position())));
	 tempmuon.setIso03   (Iso03);
	 tempmuon.setIso04   (Iso04);
       }
       else{
	 tempmuon.setNormChi2   (100);
	 tempmuon.setTrkHits    (0);
	 tempmuon.setMuonHits    (0);
	 tempmuon.setDbs  (100);
	 tempmuon.setIso03    (Iso03);
	 tempmuon.setIso04    (Iso04);
       }
       if(includereco_){
	 if(muon->triggerObjectMatches().size() ==1){ // no ambiguities
	   tempmuon.setMatchedTrig(muon->triggerObjectMatches().begin()->pathNames());
	 }
	 else{
	 std::vector<std::string> def;
	 def.push_back("NoUnamTrigMatch");
	 tempmuon.setMatchedTrig(def);
	 }
       }
       if(muon->genParticleRef().isNonnull()) tempmuon.setGenP4(muon->genParticleRef()->p4());

       ntmuons.push_back(tempmuon);
     }
   
     if(includereco_){
       if(!pfMuonCands_){
	 for(std::vector<reco::Muon>::const_iterator recomuon=recomuons->begin(); recomuon<recomuons->end() ; recomuon++){
	   top::NTLepton templep;
	   templep.setP4(recomuon->p4());
	   templep.setQ(recomuon->charge());	 
	   double vz=-9999;
	   double vzerr=-9999;
	   if(!(recomuon->globalTrack().isNull())){
	     vz=recomuon->globalTrack()->dz(vtxs[0].position());
	     vzerr=recomuon->globalTrack()->dzError();
	   }
	   else if(!(recomuon->innerTrack().isNull())){
	     vz=recomuon->innerTrack()->dz(vtxs[0].position());
	     vzerr=recomuon->innerTrack()->dzError();
	   }
	   else if(!(recomuon->outerTrack()).isNull()){
	     vz=recomuon->outerTrack()->dz(vtxs[0].position());
	     vzerr=recomuon->outerTrack()->dzError();
	   }
	   templep.setVertexZ(vz);
	   templep.setVertexZErr(vzerr);

	   ntleptons.push_back(templep);
	 }
       }
       else{
	 for(std::vector<reco::PFCandidate>::const_iterator recomuon=recopfmuons->begin(); recomuon<recopfmuons->end() ; recomuon++){
	   top::NTLepton templep;
	   templep.setP4(recomuon->p4());
	   templep.setQ(recomuon->charge());	 
	   double vz=-9999;
	   double vzerr=-9999;
	   if(!(recomuon->trackRef().isNull())){
	     vz=recomuon->trackRef()->dz(vtxs[0].position());
	     vzerr=recomuon->trackRef()->dzError();
	   }
	   templep.setVertexZ(vz);
	   templep.setVertexZErr(vzerr);

	   ntleptons.push_back(templep);

	 }
       }

       if(!pfElecCands_){
	 for(std::vector<reco::GsfElectron>::const_iterator recoelectron=recoelecs->begin(); recoelectron<recoelecs->end() ; recoelectron++){
	   top::NTLepton templep;
	   templep.setP4(recoelectron->p4());
	   templep.setQ(recoelectron->charge());
	   double vz=-9999;
	   double vzerr=-9999;
	   if(!(recoelectron->gsfTrack().isNull())){
	     vz=recoelectron->gsfTrack()->dz(vtxs[0].position());
	     vzerr=recoelectron->gsfTrack()->dzError();
	   }
	   else if(!(recoelectron->closestCtfTrackRef().isNull())){
	     vz=recoelectron->closestCtfTrackRef()->dz(vtxs[0].position());
	     vzerr=recoelectron->closestCtfTrackRef()->dzError();
	   }
	   templep.setVertexZ(vz);
	   templep.setVertexZErr(vzerr);
	   ntleptons.push_back(templep);
	 }
       }
       else{
	 for(std::vector<reco::PFCandidate>::const_iterator recoelec=recopfelecs->begin(); recoelec<recopfelecs->end() ; recoelec++){
	   top::NTLepton templep;
	   templep.setP4(recoelec->p4());
	   templep.setQ(recoelec->charge());	 
	   double vz=-9999;
	   double vzerr=-9999;
	   if(!(recoelec->trackRef().isNull())){
	     vz=recoelec->trackRef()->dz(vtxs[0].position());
	     vzerr=recoelec->trackRef()->dzError();
	   }
	   templep.setVertexZ(vz);
	   templep.setVertexZErr(vzerr);

	   ntleptons.push_back(templep);
       }
     }

     }
     for(std::vector<pat::Jet>::const_iterator jet=jets->begin(); jet<jets->end() ; jet++){

       top::NTJet tempjet;
       tempjet.setId(checkJetID(jet));
       tempjet.setP4(jet->p4());
       if(jet->genJet()){
	 tempjet.setGenP4(jet->genJet()->p4());
	 tempjet.setGenPartonFlavour(jet->partonFlavour());
       }
       else{
	 tempjet.setGenP4(jet->p4());
	 tempjet.setGenPartonFlavour(0);
       }
       tempjet.setBtag(jet->bDiscriminator(btagalgo_));    //
       double emEnergyfraction=0;
       if(jet->isPFJet()) emEnergyfraction=jet->chargedEmEnergyFraction()+jet->neutralEmEnergyFraction();
       else emEnergyfraction=jet->emEnergyFraction();
       tempjet.setEmEnergyFraction(emEnergyfraction);
       ntjets.push_back(tempjet);
     }

     
     for(std::vector<pat::MET>::const_iterator met=mets->begin(); met<mets->end() ; met++){
       ntmet.setP4(met->p4());
       break;
     
     }
     if(includereco_){
       for(std::vector<reco::Track>::const_iterator track=recotracks->begin();track<recotracks->end();track++){
	 if(track->pt() < 17) continue;
	 if(fabs(track->eta()) > 2.6) continue;
	 for(std::vector<reco::Track>::const_iterator track2=recotracks->begin();track2<recotracks->end();track2++){
	   if(track2->pt() < 17) continue;
	   if(fabs(track2->eta()) > 2.6) continue;
	   if(track->charge() == track2->charge()) continue;
	   top::NTTrack nttrack;
	   top::LorentzVector p1(track->px(), track->py(), track->pz(), track->p());
	   top::LorentzVector p2(track2->px(), track2->py(), track2->pz(), track2->p());
	   if((p1+p2).M() > 55 && (p1+p2).M() < 125){
	     nttrack.setP4(p1);
	     nttrack.setQ(track->charge());
	     nttrack.setVertexZ(track->dz(vtxs[0].position()));
	     nttrack.setVertexZErr(track->dzError());

	     nttracks.push_back(nttrack);
	     break;
	   }
	 }
       }

       for(std::vector<reco::SuperCluster>::const_iterator suclu=recosuclus->begin(); suclu<recosuclus->end(); suclu++){

	 double suclue=suclu->rawEnergy();
	 math::XYZPoint suclupoint=suclu->position();
	 double magnitude=sqrt(suclupoint.mag2());
	 top::LorentzVector suclup4(suclue*suclupoint.x() / magnitude,suclue*suclupoint.y() / magnitude,suclue*suclupoint.z() / magnitude,suclue);
	 if(suclup4.Pt() < 10) continue;

	 for(std::vector<reco::SuperCluster>::const_iterator suclu2=recosuclus->begin(); suclu2<recosuclus->end(); suclu2++){

	   if(suclu == suclu2) continue;

	   double suclue2=suclu2->rawEnergy();
	   math::XYZPoint suclupoint2=suclu2->position();
	   double magnitude2=sqrt(suclupoint2.mag2());
	   top::LorentzVector suclup42(suclue2*suclupoint2.x() / magnitude2,suclue2*suclupoint2.y() / magnitude2,suclue2*suclupoint2.z() / magnitude2,suclue2);

	   if(suclup42.Pt() < 10) continue;

	   if((suclup4+suclup42).M() > 55 && (suclup4+suclup42).M()<125 ){
	     top::NTSuClu tempsuclu;
	     tempsuclu.setP4(suclup4);
	     ntsuclus.push_back(tempsuclu);
	     break;
	   }
	 }
       }
     }
     
   } //end vtxs.size()>0


   std::vector<std::string> firedtriggers;

   if(includetrigger_){ 

     Handle<TriggerResults> trigresults;
     iEvent.getByLabel(trigresults_, trigresults);

     if(!trigresults.failedToGet()){
       int n_Triggers = trigresults->size();
       TriggerNames trigName = iEvent.triggerNames(*trigresults);

       for(int i_Trig = 0; i_Trig<n_Triggers; ++i_Trig){
	 if(trigresults.product()->accept(i_Trig)){
	   firedtriggers.push_back(trigName.triggerName(i_Trig));
	   nttrigger.insert(trigName.triggerName(i_Trig));
	   // if(((TString)trigName.triggerName(i_Trig)).Contains("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v")){
	   // std::cout << trigName.triggerName(i_Trig) << std::endl;
	   //} 
	 }
	  
       }
     }

   }
   ntevent.setRunNo(iEvent.id().run());
   ntevent.setLumiBlock(iEvent.id().luminosityBlock());
   ntevent.setEventNo(iEvent.id().event());
   ntevent.setVertexMulti(vertices->size());
   ntevent.setFiredTriggers(firedtriggers);



   float BXminus=0;
   float BXzero=0;
   float BXplus=0;

   edm::Handle<std::vector<PileupSummaryInfo> > PUSInfo;
   try{
     iEvent.getByLabel(puinfo_, PUSInfo);
     for(std::vector<PileupSummaryInfo>::const_iterator PUI=PUSInfo->begin(); PUI<PUSInfo->end();PUI++){
       if(PUI->getBunchCrossing()==-1)
	 BXminus=PUI->getTrueNumInteractions();
       else if(PUI->getBunchCrossing()==0)
	 BXzero=PUI->getTrueNumInteractions();
       else if(PUI->getBunchCrossing()==1)
	 BXplus=PUI->getTrueNumInteractions();
     }
   }catch(...){}
   
   ntevent.setTruePU(BXminus,BXzero,BXplus);

   std::vector<double> temprhos;


   //rhoiso,,rhojetsiso,,rhojetsisonopu
   edm::Handle<double> rho;

   iEvent.getByLabel(rhoiso_,rho);
   temprhos.push_back(*rho);
   //if(rho2011_){
   iEvent.getByLabel(rhojetsisonopu_,rho);
   temprhos.push_back(*rho);
   iEvent.getByLabel(rhojetsiso_,rho);
   temprhos.push_back(*rho);
     //}
   ntevent.setIsoRho(temprhos);

   //add rhoiso to electrons (if uses 2011 corrections (second argument set to false)); ##RHO2012##
   top::elecRhoIsoAdder addrho(!IsRealData, !rho2011_);
   if(rho2011_) addrho.setRho(temprhos[2]);
   else         addrho.setRho(temprhos[0]);
   addrho.addRhoIso(ntpfelectrons);
   addrho.addRhoIso(ntgsfelectrons);

   /////pdf weights///////
   if(includepdfweights_ && !IsRealData){
     edm::Handle<std::vector<double> > weightHandle;
     iEvent.getByLabel(pdfweights_, weightHandle);

     ntevent.setPDFWeights(*weightHandle);
   }

   ///////fill gen info and SUSY generator info////

   if(!IsRealData && includegen_){
     //    edm::Handle<GenEventInfoProduct> genEvtInfo;
     // iEvent.getByLabel("generator", genEvtInfo);
     //T_Event_PtHat =  genEvtInfo->hasBinningValues() ? (genEvtInfo->binningValues())[0] : 0.0;
     // T_Event_processID= genEvtInfo->signalProcessID();
  
     std::vector<const reco::GenParticle *> allgen;
  
     for (size_t i = 0; i < genParticles->size(); ++i){

       allgen.push_back(&(genParticles->at(i)));

       const reco::GenParticle & p = (*genParticles)[i];
       int id = p.pdgId();
       //  int st = p.status();
    
       if (abs(id) == 1000006) {
	 stopMass.push_back(p.mass());
      
	 size_t ndau = p.numberOfDaughters();
	 bool foundTop = false;
      
	 for( size_t j = 0; j < ndau; ++ j ) {
	
	   if( abs( p.daughter(j)->pdgId() ) == 6 ) { // if the i-th daughter is a top or an anti-top
	     foundTop = true;
	  
	     top::LorentzVector LVstop( p.px(), p.py(), p.pz(), p.energy() );
	     top::LorentzVector LVtop( p.daughter(j)->px(), p.daughter(j)->py(), p.daughter(j)->pz(), p.daughter(j)->energy() );
	  
	     chiMass.push_back( (LVstop-LVtop).M() );
	     //std::cout << "chi0 mass = " << (LVstop-LVtop).M() << endl; 
	     break;
	   }

	   if( !foundTop )// stop/anti-stop has no top/anti-top daughter! m_chi0 set to an unphysical value
	     chiMass.push_back(-99.);
	 }
       }
     }	

     //  const std::vector<reco::GenParticle> * allgen = &(*genParticles);


     std::vector<int> elecpdg; elecpdg.push_back(11);elecpdg.push_back(-11);
     std::vector<int> muonpdg; muonpdg.push_back(13);muonpdg.push_back(-13);

     std::vector<const reco::GenParticle *> genElecsZ = top::getGenLepsFromZ(allgen, elecpdg, viaTau_);
     std::vector<const reco::GenParticle *> genMuonsZ = top::getGenLepsFromZ(allgen, muonpdg, viaTau_);

     //both empty for top

     std::vector<const reco::GenParticle *> genElecsTop = top::getGenLepsFromTop(allgen, elecpdg, viaTau_);
     std::vector<const reco::GenParticle *> genMuonsTop = top::getGenLepsFromTop(allgen, muonpdg, viaTau_);

     std::vector<const reco::GenParticle *> genElecs=genElecsTop;
     std::vector<const reco::GenParticle *> genMuons=genMuonsTop;

     genElecs.insert(genElecs.end(),genElecsZ.begin(),genElecsZ.end());
     genMuons.insert(genMuons.end(),genMuonsZ.begin(),genMuonsZ.end());


     top::NTGenLepton tempgenlep;
     const reco::GenParticle * genP;
     for(unsigned int i=0;i<genElecs.size();i++){
       genP=genElecs.at(i);
       tempgenlep.setP4(genP->p4());
       tempgenlep.setPdgId(genP->pdgId());
       ntgenelecs.push_back(tempgenlep);
     } 
     for(unsigned int i=0;i<genMuons.size();i++){
       genP=genMuons.at(i);
       tempgenlep.setP4(genP->p4());
       tempgenlep.setPdgId(genP->pdgId());
       ntgenmuons.push_back(tempgenlep);
     }



   } // isMc end



   firstevent=false;

   Ntuple ->Fill();

   }



// ------------ method called once each job just before starting event loop  ------------
void 
SusyTreeWriter::beginJob()
{
 
  if( !fs ){
    throw edm::Exception( edm::errors::Configuration,
                          "TFile Service is not registered in cfg file" );
  }

  viaTau_=false;
  if(((TString)fs->file().GetName()).Contains("tau")){
    viaTau_=true;
    std::cout << "\n\n\n################## applying viatau gencut on NTGenLeptons! #########\n\n\n" << std::endl; 
  }

  char * tempname = new char[treename_.length()];
  strcpy(tempname, treename_.c_str());
  Ntuple=fs->make<TTree>(tempname ,tempname );
  //Ntuple->Branch("elePt",&elecpts);
  //TBranch *branch = Ntuple->Branch("Triggers",&triggers);
  Ntuple->Branch("NTInflatedMuons", "std::vector<top::NTInflatedMuon>", &ntmuons);
  if(includereco_){
    Ntuple->Branch("NTLeptons", "std::vector<top::NTLepton>", &ntleptons);
    Ntuple->Branch("NTTracks", "std::vector<top::NTTrack>", &nttracks);
    Ntuple->Branch("NTSuClu", "std::vector<top::NTSuClu>", &ntsuclus);
  }
  Ntuple->Branch("NTInflatedPFElectrons", "std::vector<top::NTInflatedElectron>", &ntpfelectrons);
  Ntuple->Branch("NTInflatedElectrons", "std::vector<top::NTInflatedElectron>", &ntgsfelectrons);
  Ntuple->Branch("NTJets", "std::vector<top::NTJet>", &ntjets);
  Ntuple->Branch("NTMet", "top::NTMet", &ntmet);
  Ntuple->Branch("NTEvent", "top::NTEvent", &ntevent);
  Ntuple->Branch("NTTrigger", "top::NTTrigger", &nttrigger);

  Ntuple->Branch("stopMass",  &stopMass);
  Ntuple->Branch("chiMass", &chiMass);

  Ntuple->Branch("NTGenElectrons", "std::vector<top::NTGenLepton>", &ntgenelecs);
  Ntuple->Branch("NTGenMuons",     "std::vector<top::NTGenLepton>", &ntgenmuons);

  //std::vector<std::string>
}

// ------------ method called once each job just after ending the event loop  ------------
void 
SusyTreeWriter::endJob() 
{

  if( !fs ){
    throw edm::Exception( edm::errors::Configuration,
                          "TFile Service is not registered in cfg file" );
  }
  TTree * TT;
  TT=fs->make<TTree>("TriggerMaps" ,"TriggerMaps" );
  nttrigger.writeMapToTree(TT);
}

// ------------ method called when starting to processes a run  ------------
void 
SusyTreeWriter::beginRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a run  ------------
void 
SusyTreeWriter::endRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block  ------------
void 
SusyTreeWriter::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void 
SusyTreeWriter::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
SusyTreeWriter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}
bool SusyTreeWriter::checkJetID(std::vector<pat::Jet>::const_iterator jet)
{
  bool hasjetID=false;
  if(jet->numberOfDaughters() > 1 &&
     jet->neutralHadronEnergyFraction() < 0.99 &&
     jet->neutralEmEnergyFraction() < 0.99){
    if(fabs(jet->eta())<2.4){
      if( jet->chargedMultiplicity() > 0 &&
          jet->chargedHadronEnergyFraction() > 0&&
          jet->chargedEmEnergyFraction() < 0.99)
	hasjetID=true;
    }
    else{
      hasjetID=true;
    }
  }

  
  return hasjetID;
}

//define this as a plug-in
DEFINE_FWK_MODULE(SusyTreeWriter);
