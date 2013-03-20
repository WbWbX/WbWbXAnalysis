// -*- C++ -*-
//
// Package:    TreeWriter
// Class:      TreeWriter
// 
/**\class TreeWriter TreeWriter.cc DOSS/TreeWriter/src/TreeWriter.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Jan Kieseler,,,DESY
//         Created:  Fri May 11 14:22:43 CEST 2012
// $Id: TreeWriter.cc,v 1.19 2013/02/22 10:39:16 jkiesele Exp $
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
#include "../../DataFormats/interface/mathdefs.h"


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
#include "TtZAnalysis/Tools/interface/miscUtils.h"

//
// class declaration
//

class TreeWriter : public edm::EDAnalyzer {
   public:
      explicit TreeWriter(const edm::ParameterSet&);
      ~TreeWriter();

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

  std::vector<bool> checkTriggers(const edm::Event&);
  std::vector<std::string> triggers_;
  std::vector<bool> triggerBools_;
  void setTriggers();
  top::NTGenParticle makeNTGen(const reco::GenParticle *, const std::map<const reco::GenParticle*, int>&);
  bool isInGenCollection(const reco::GenParticle * , const std::vector<const reco::GenParticle * > &);
  bool isInGenCollection(const reco::GenJet * , const std::vector<const reco::GenJet * > &);
  template<typename t,typename u>
  int findGenMatchIdx(t * , std::vector<u> & , double dR=0.4);
 
  

  bool pfElecCands_;
  bool pfMuonCands_;
  bool rho2011_;
  bool usegsf_;

  edm::InputTag muons_, recomuons_, pfelecs_, gsfelecs_,recoelecs_, jets_, met_, vertices_, trigresults_, puinfo_, recotracks_, recosuclus_,rhojetsiso_,rhojetsisonopu_,rhoiso_,pdfweights_, genparticles_, genjets_;
  //rhojets_,rhojetsiso_,rhojetsnopu_,rhojetsisonopu_,rhoiso_;

  bool includereco_, includetrigger_, pfinput_,includepdfweights_,includegen_;
  TTree* Ntuple;
  std::vector<top::NTMuon> ntmuons;
  std::vector<top::NTLepton> ntleptons;
  std::vector<top::NTElectron> ntpfelectrons;
  std::vector<top::NTElectron> ntgsfelectrons;
  std::vector<top::NTJet> ntjets;
  std::vector<top::NTTrack> nttracks;
  std::vector<top::NTSuClu> ntsuclus;
  top::NTMet ntmet;
  top::NTEvent ntevent;
  top::NTTrigger nttrigger;

  std::vector<top::NTGenParticle> ntgenmuons3, ntgenelecs3,ntgentaus3,ntgenmuons1, ntgenelecs1,ntgentaus1, allntgen;
  std::vector<top::NTGenJet> ntgenjets;
  int channel_; // 11 for ee 13 for mumu 1113 for emu 151113 etc for via tau

  bool viaTau_;

  std::string treename_, btagalgo_;

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
TreeWriter::TreeWriter(const edm::ParameterSet& iConfig)
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
  genparticles_ = iConfig.getParameter<edm::InputTag>             ( "genParticles" );
  genjets_ = iConfig.getParameter<edm::InputTag>             ( "genJets" );

   std::cout << "n\n################## Tree writer ########################" 
             <<  "\n#" << treename_
             <<  "\n#                                                     #" 
             <<  "\n#     includes trigger    : " << includetrigger_ <<"                         #"
             <<  "\n#     includes reco       : " << includereco_          <<"                         #" 
             <<  "\n#     includes pdfWeights : " << includepdfweights_    <<"                         #" 
             <<  "\n#                                                     #"
             <<  "\n#                                                     #"
             <<  "\n#######################################################" << std::endl;


   std::cout << "\n\n JETS ARE REQUIRED TO HAVE AT LEAST 10GeV UNCORR PT\n\n" << std::endl;

   setTriggers();

}


TreeWriter::~TreeWriter()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
TreeWriter::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   using namespace top;

   std::string addForPF;
   if(pfinput_) addForPF="bla";

   LorentzVector p4zero(0,0,0,0);
   
   ntmuons.clear();
   ntleptons.clear();
   ntpfelectrons.clear();
   ntgsfelectrons.clear();
   ntjets.clear();
   nttracks.clear();
   ntsuclus.clear();
   ntgenmuons3.clear();
   ntgenelecs1.clear();
   ntgenelecs3.clear();
   ntgenmuons1.clear();
   ntgentaus3.clear();
   ntgentaus1.clear();

   allntgen.clear();
   ntgenjets.clear();

   triggerBools_.clear();


   top::NTTrigger clear;
   nttrigger=clear;

  bool IsRealData = false;
  edm::Handle <reco::GenParticleCollection> genParticles;
  try {
    iEvent.getByLabel(genparticles_, genParticles);
    IsRealData = false;
  }
  catch(...) {
    IsRealData = true;
  } 

  //////////////generator stuff/////////////////

  std::map<const reco::GenParticle *, int> genidmap,smallIdMap;
  std::map<const reco::GenJet *, int> jetidmap;


  std::vector<const reco::GenParticle *> allgen, tops, Zs, Ws, bs, leps3,leps1,nus;

  std::vector<const reco::GenParticle*>  mergedgen;
  std::vector<const reco::GenJet *> allgenjets;

   if(!IsRealData && includegen_){



     std::vector<const reco::GenParticle*> temp;

     const reco::GenParticle * daughter; 
     const reco::GenParticle * mother;
     //make allgen vector and fill id map

     //   std::cout << "bla" << std::endl;

     for (size_t i = 0; i < genParticles->size(); ++i){
       allgen.push_back(&(genParticles->at(i)));
       genidmap[&(genParticles->at(i))] = i;
     }
     

     // access genidmap.at(genparticlepointer) to get (int) id

     //now create mother/daughter links, getdecaychain etc

     //fill initial particles

     for(size_t i=0;i<allgen.size();i++){
       if(isAbsApprox(allgen.at(i)->pdgId(), 6)){ //(anti)top quark
	 tops << allgen.at(i);
       }

       if(isAbsApprox(allgen.at(i)->pdgId(), 23)){ //Z boson
	 Zs <<  allgen.at(i);
       }
     }


     std::map<const reco::GenParticle*, const reco::GenParticle*> mothdaugh; //mother daughter for leps3 leps1
     std::map<const reco::GenParticle*, const reco::GenParticle*> daughmoth; //mother daughter for leps3 leps1

     //get Z decay leptons stat 3
     for(size_t i=0;i<Zs.size();i++){
       //get leptonic decay chain of Z
       mother=Zs[i];
       leps3 << getGenFromMother(mother,11);
       leps3 << getGenFromMother(mother,-11);
       leps3 << getGenFromMother(mother,13);
       leps3 << getGenFromMother(mother,-13);
       leps3 << getGenFromMother(mother,15);
       leps3 << getGenFromMother(mother,-15);
     }
     //get top decay Ws
     for(size_t i=0;i<tops.size();i++){
       mother=tops[i];
       Ws << getGenFromMother(mother, 24);
       Ws << getGenFromMother(mother, -24);
       bs << getGenFromMother(mother, 5);
       bs << getGenFromMother(mother, -5);
     }


     //get leps3 from W decay
     for(size_t i=0;i<Ws.size();i++){
       mother=Ws[i];
       leps3 << getGenFromMother(mother,11);
       leps3 << getGenFromMother(mother,-11);
       leps3 << getGenFromMother(mother,13);
       leps3 << getGenFromMother(mother,-13);
       leps3 << getGenFromMother(mother,15);
       leps3 << getGenFromMother(mother,-15);
     }

     //status 1 particles 



     for(size_t j=0;j<leps3.size();j++){ 
       mother=leps3[j];
       
       size_t i=0;
       size_t ndaugh=mother->numberOfDaughters();


       //  std::cout << "\nchecking daughters of leps3" << std::endl;
       while(i<ndaugh){
	 
	 daughter = dynamic_cast<const reco::GenParticle*>(mother->daughter(i));

	 i++;

	 if(isAbsApprox(daughter->pdgId(),11) || isAbsApprox(daughter->pdgId(),13) || isAbsApprox(daughter->pdgId(),15)){
	   if(isAbsApprox(daughter->status(),1)){
	     leps1 << daughter;
	     mothdaugh[leps3[j]] = daughter;
	     daughmoth[daughter]=leps3[j];
	     break;
	   }
	   else{
	     mother=daughter;
	     //   std::cout << "switched mother "<< std::endl;
	     i=0;
	     ndaugh=mother->numberOfDaughters();
	   }
	 }

	
       }
     }
   

     /* 
	bs might change again etc...
	asso to b-hadrons, bjets etc will change, stop impl here until solved


     */

      mergedgen << tops << Zs << bs << Ws << leps3 << leps1;

      NTGenParticle tempntgen;
      //smallIdMap

      //make new id map for ordering
      //  std::cout << std::endl;
      for(size_t j=0;j<mergedgen.size();j++){
	//	std::cout << mergedgen.at(j)->pdgId() << "    " << mergedgen.at(j)->status() << std::endl;
	smallIdMap[mergedgen.at(j)]=j;
      }

      for(size_t j=0;j<mergedgen.size();j++){
	mother=mergedgen.at(j);
	tempntgen=makeNTGen(mother,smallIdMap);


	//put small function to get mothers until one of the particles in mergedgen is reached, same for daughters
	bool gotmoth=false;
	bool gotdaugh=false;

	for(size_t  i=0;i<mother->numberOfDaughters();i++){
	  daughter = dynamic_cast<const reco::GenParticle*>(mother->daughter(i));
	  if(isInGenCollection(daughter, mergedgen)){
	    tempntgen.setDaughter(smallIdMap.at(daughter));
	    gotdaugh=true;
	  }
	}
	if(!gotdaugh && mothdaugh.find(mother) != mothdaugh.end()){
	  tempntgen.setDaughter(smallIdMap.at(mothdaugh.at(mother)));
	}
	for(size_t  i=0;i<mother->numberOfMothers();i++){
	  const reco::GenParticle* mothmoth = dynamic_cast<const reco::GenParticle*>(mother->mother(i));
	  if(isInGenCollection(mothmoth, mergedgen)){
	    tempntgen.setMother(smallIdMap.at(mothmoth));
	    gotmoth=true;
	  }
	}
	if(!gotmoth && daughmoth.find(mother) != daughmoth.end()){
	  tempntgen.setMother(smallIdMap.at(daughmoth.at(mother)));
	}
	

	allntgen << tempntgen;
      }



      // would be some merging of bs etc... see nazars stuff

      // fill genJets


     edm::Handle<reco::GenJetCollection> genJets;
     iEvent.getByLabel(genjets_, genJets);


     NTGenJet tempgenjet;


// jetidmap


     for(size_t i=0;i<genJets->size();i++){
       allgenjets << & genJets->at(i);
       jetidmap[allgenjets[i]]=i;
       //fill map and pointer vector
     }

     for(size_t i=0;i<allgenjets.size();i++){
       //some requirement for jets

       tempgenjet.setP4(allgenjets.at(i)->p4());
       tempgenjet.setGenId(jetidmap.at(allgenjets[i])); //use a map here
       //  tempgenjet.setMother(int)
       ntgenjets << tempgenjet;
     }




   }//isMC and includegen end



 // a lot has to be done here!!!
	 // genJet b matching (get best! dr match cone 0.5
	 // associate b as mother of jet
	 // association via id int. e.g. e.g. some class variable that just counts futher in each fill
	 // first fill all the parton stuff, then the matched jets. 
	 // direct asso done on analysis level by checking. If has to be checked several times, make temp asso map
	 // also store neutrinos. just for fun. 12 14 16
	 // ids...: 
	 /*
	   one vector soulution: once filled properly, easy asso, fast, additional analysis level tools to get collections.. 
	   seperate vectors: complicated or lot of hardcoded asso, easy to handle on analysis level, but slower

	   how to make a nice asso? static member asso map?

	   first give "random" ids. at end of each event loop, make asso via vector links and fill one vector..
	   remaining prob: jet-b asso. reco-gen asso.. ok .. just associate in a "hard" way: 
	   b->always jet, genlep->stat3->recolep
	   genneutr->nothing

	   do selection as you like. step by step or in an enormous entangled loop

	   protoype loop:
	   int id
	   for each gen(i) in recogenpart:
	   do some selection
	   ntgen <- gen(i)
	   ntgen.setGenId(id)
	   id++ (at each filling step, also for daughters) !!! the important part.


	   at the end:
	   vector<ntgen> new=ntgenvec
	   for each ntgen(i) in ntgenvec
	   for each j in ntgenvec
	   moth=ntgenvec(i).mother
	   daught=ntgenvec.daught
	   search for corresponding entries j in ntgenvec
	   new.setMother/Duaghter(j)

	   use new 
	   do the same for ntgenjets<-recojets
	   do the same for genleptsstat3<-recoleps

	   the genStuff has to be filled BEFORE the reco stuff is done. To be able to do the asso (ids already created)
	   create temp map in tree writer of map<int id, reco:genParticle * asso genpart> for all gen<->reco asso


	   functions: reco: 
	   get:	   genMatch
	   set:    setGenMatch

	   gen:
	   get:    genId
	   set:    setGenId

	  */



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

     for(std::vector<pat::Electron >::const_iterator electron=pfelecs->begin(); electron<pfelecs->end() ; ++electron){
     
       top::NTIsolation Iso;
       
       Iso.setChargedHadronIso(electron->chargedHadronIso());
       Iso.setNeutralHadronIso(electron->neutralHadronIso());
       Iso.setPhotonIso(electron->photonIso());
       Iso.setPuChargedHadronIso(electron->puChargedHadronIso());
	 

       top::NTElectron tempelec;
       //if(electron->ecalDrivenMomentum())
       int genidx=findGenMatchIdx(&*electron,allntgen,0.4);
       //do gen asso
       if(genidx>=0){
	 tempelec.setGenMatch(genidx);
	 tempelec.setGenP4(allntgen.at(genidx).p4());
       }
       else{
	 tempelec.setGenP4(p4zero);
	 tempelec.setGenMatch(-1);
       }
       tempelec.setECalP4(electron->ecalDrivenMomentum());
       
       tempelec.setP4(electron->p4());
       tempelec.setQ(electron->charge());
       double vz=-9999;
       double vzerr=-9999;
       double dbs=100;
       int mhits=99;
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
       tempelec.setDz(vz);                   //
       tempelec.setDzErr(vzerr);  

       tempelec.setDbs(dbs);                  //
       tempelec.setNotConv(electron->passConversionVeto());    
       tempelec.setId(electron->electronIDs());
       tempelec.setIso(Iso);                   //
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
       // otherwise (0,0,0,0) taken  care of by NTElectron Constructor

       //  if(electron->genParticleRef().isNonnull()) tempelec.setGenP4(electron->genParticleRef()->p4());
     
       ntpfelectrons.push_back(tempelec);


     }
     ////gsf electron/////


     for(std::vector<pat::Electron >::const_iterator electron=gsfelecs->begin(); electron<gsfelecs->end() ; ++electron){
     
       top::NTIsolation Iso;
       
       Iso.setChargedHadronIso(electron->chargedHadronIso());
       Iso.setNeutralHadronIso(electron->neutralHadronIso());
       Iso.setPhotonIso(electron->photonIso());
       Iso.setPuChargedHadronIso(electron->puChargedHadronIso());
	 

       top::NTElectron tempelec;
       //  if(electron->ecalDrivenMomentum())

       //do gen asso
       int genidx=findGenMatchIdx(&*electron,allntgen,0.4);
       //do gen asso
       if(genidx>=0){
	 tempelec.setGenMatch(genidx);
	 tempelec.setGenP4(allntgen.at(genidx).p4());
       }
       else{
	 tempelec.setGenP4(p4zero);
	 tempelec.setGenMatch(-1);
       }

       tempelec.setECalP4(electron->ecalDrivenMomentum());
       tempelec.setIsPf(electron->isPF());
       tempelec.setP4(electron->p4());
       tempelec.setQ(electron->charge());
       double vz=-9999;
       double vzerr=-9999;
       double dbs=100;

       int mhits=99;

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
       tempelec.setDz(vz);                   //
       tempelec.setDzErr(vzerr);  

       tempelec.setDbs(dbs);                  //
       tempelec.setNotConv(electron->passConversionVeto());    
       tempelec.setId(electron->electronIDs());
       tempelec.setIso(Iso);                   //
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
       // otherwise (0,0,0,0) taken  care of by NTElectron Constructor

       //   if(electron->genParticleRef().isNonnull()) tempelec.setGenP4(electron->genParticleRef()->p4());
     
       ntgsfelectrons.push_back(tempelec);


     }


     /////////////////////////////////// M U O N S//////////////////
   
     for(std::vector<pat::Muon >::const_iterator muon=muons->begin(); muon<muons->end() ; muon++){
       
//        

       top::NTIsolation Iso;
       Iso.setChargedHadronIso(muon->chargedHadronIso());
       Iso.setNeutralHadronIso(muon->neutralHadronIso());
       Iso.setPhotonIso(muon->photonIso());
       Iso.setPuChargedHadronIso(muon->puChargedHadronIso());


       top::NTMuon tempmuon;

 //do gen asso
      int genidx=findGenMatchIdx(&*muon,allntgen,0.4);
       //do gen asso
       if(genidx>=0){
	 tempmuon.setGenMatch(genidx);
	 tempmuon.setGenP4(allntgen.at(genidx).p4());
       }
       else{
	 tempmuon.setGenP4(p4zero);
	 tempmuon.setGenMatch(-1);
       }


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
       tempmuon.setDz   (vz);
       tempmuon.setDzErr   (vzerr);

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
	 tempmuon.setIso   (Iso);
       }
       else{
	 tempmuon.setNormChi2   (100);
	 tempmuon.setTrkHits    (0);
	 tempmuon.setMuonHits    (0);
	 tempmuon.setDbs  (100);
	 tempmuon.setIso    (Iso);
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
       //   if(muon->genParticleRef().isNonnull()) tempmuon.setGenP4(muon->genParticleRef()->p4());

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
	   templep.setDz(vz);
	   templep.setDzErr(vzerr);

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
	   templep.setDz(vz);
	   templep.setDzErr(vzerr);

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
	   templep.setDz(vz);
	   templep.setDzErr(vzerr);
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
	   templep.setDz(vz);
	   templep.setDzErr(vzerr);

	   ntleptons.push_back(templep);
       }
     }

     }
     for(std::vector<pat::Jet>::const_iterator jet=jets->begin(); jet<jets->end() ; jet++){


       top::NTJet tempjet;

       pat::Jet uncJet=jet->correctedJet("Uncorrected");
       if(uncJet.pt() < 10) continue;
       double corr_factor=jet->pt() / uncJet.pt();
       tempjet.setCorrFactor(corr_factor);

       tempjet.setId(checkJetID(jet));
       tempjet.setP4(jet->p4());
       if(jet->genJet()){
	 tempjet.setGenP4(jet->genJet()->p4());
	 tempjet.setGenPartonFlavour(jet->partonFlavour());
	 int genidx=findGenMatchIdx((jet->genJet()),ntgenjets,0.1);
	 tempjet.setGenMatch(genidx);
	 
       }
       else{
	 tempjet.setGenP4(p4zero);
	 tempjet.setGenPartonFlavour(0);
	 tempjet.setGenMatch(-1);
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
	     nttrack.setDz(track->dz(vtxs[0].position()));
	     nttrack.setDzErr(track->dzError());

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

   /////////triggers as boolians and if switched on as strings

   triggerBools_=checkTriggers(iEvent);


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
   // if(rho2011_){
     iEvent.getByLabel(rhojetsisonopu_,rho);
     temprhos.push_back(*rho);
     iEvent.getByLabel(rhojetsiso_,rho);
     temprhos.push_back(*rho);
     // }
   ntevent.setIsoRho(temprhos);

   //add rhoiso to electrons (uses 2011 corrections (second argument set to false));
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

   /////// Fill generator info


   Ntuple ->Fill();

}


// ------------ method called once each job just before starting event loop  ------------
void 
TreeWriter::beginJob()
{
  edm::Service<TFileService> fs;

  if( !fs ){
    throw edm::Exception( edm::errors::Configuration,
                          "TFile Service is not registered in cfg file" );
  }
  char * tempname = new char[treename_.length()];
  strcpy(tempname, treename_.c_str());
  Ntuple=fs->make<TTree>(tempname ,tempname );
  //Ntuple->Branch("elePt",&elecpts);
  //TBranch *branch = Ntuple->Branch("Triggers",&triggers);
  Ntuple->Branch("NTMuons", "std::vector<top::NTMuon>", &ntmuons);
  if(includereco_){
    Ntuple->Branch("NTLeptons", "std::vector<top::NTLepton>", &ntleptons);
    Ntuple->Branch("NTTracks", "std::vector<top::NTTrack>", &nttracks);
    Ntuple->Branch("NTSuClu", "std::vector<top::NTSuClu>", &ntsuclus);
  }
  Ntuple->Branch("NTPFElectrons", "std::vector<top::NTElectron>", &ntpfelectrons);
  Ntuple->Branch("NTElectrons", "std::vector<top::NTElectron>", &ntgsfelectrons);
  Ntuple->Branch("NTJets", "std::vector<top::NTJet>", &ntjets);
  Ntuple->Branch("NTMet", "top::NTMet", &ntmet);
  Ntuple->Branch("NTEvent", "top::NTEvent", &ntevent);

  Ntuple->Branch("TriggerBools", "std::vector<bool>", &triggerBools_);

  //  Ntuple->Branch("NTTrigger", "top::NTTrigger", &nttrigger);

  Ntuple->Branch("NTGenParticles", "std::vector<top::NTGenParticle>", &allntgen);
  Ntuple->Branch("NTGenJets", "std::vector<top::NTGenJet>", &ntgenjets);
  // Ntuple->Branch("NTGenMEMuons",     "std::vector<top::NTGenParticle>", &ntgenmuons3);
  // Ntuple->Branch("NTGenElectrons", "std::vector<top::NTGenParticle>", &ntgenelecs1);
  // Ntuple->Branch("NTGenMuons",     "std::vector<top::NTGenParticle>", &ntgenmuons1);
  Ntuple->Branch("Channel",channel_);

}

// ------------ method called once each job just after ending the event loop  ------------
void 
TreeWriter::endJob() 
{
  if( !fs ){
    throw edm::Exception( edm::errors::Configuration,
                          "TFile Service is not registered in cfg file" );
  }
  TTree * TT;
  TT=fs->make<TTree>("TriggerMaps" ,"TriggerMaps" );
  TT->Branch("TriggerStrings",     "std::vector<std::string>", &triggers_);
  TT->Fill();
  
}

// ------------ method called when starting to processes a run  ------------
void 
TreeWriter::beginRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a run  ------------
void 
TreeWriter::endRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block  ------------
void 
TreeWriter::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void 
TreeWriter::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
TreeWriter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}
bool TreeWriter::checkJetID(std::vector<pat::Jet>::const_iterator jet)
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
/*
 * makes a contains(bla)
 */
std::vector<bool> TreeWriter::checkTriggers(const edm::Event& iEvent){
  using namespace edm;

  std::vector<bool> output;
  output.resize(triggers_.size(),false);
  

  Handle<TriggerResults> trigresults;
  iEvent.getByLabel(trigresults_, trigresults);

  if(!trigresults.failedToGet()){
    int n_Triggers = trigresults->size();
    TriggerNames trigName = iEvent.triggerNames(*trigresults);

    for(int i_Trig = 0; i_Trig<n_Triggers; ++i_Trig){
      if(trigresults.product()->accept(i_Trig)){
	std::string firedtrig=trigName.triggerName(i_Trig);
	for(size_t i=0;i<triggers_.size();i++){
	  if(firedtrig.find(triggers_[i]) != std::string::npos)
	    output[i]=true;
	}
      }	  
    }
  }
  return output;
}

void TreeWriter::setTriggers(){
  
  triggers_.push_back("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v");
  triggers_.push_back("HLT_Mu17_Mu8_v");
  triggers_.push_back("HLT_Mu17_TkMu8_v");
  //
  triggers_.push_back("HLT_Ele17_SW_TightCaloEleId_Ele8_HE_L1R_v");
  triggers_.push_back("HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v");
  triggers_.push_back("HLT_DoubleMu7");
  triggers_.push_back("HLT_Mu13_Mu8_v");
  /*
  triggers_.push_back();
  triggers_.push_back();
  triggers_.push_back();
  triggers_.push_back();
  triggers_.push_back();
*/

}


top::NTGenParticle TreeWriter::makeNTGen(const reco::GenParticle * p, const std::map<const reco::GenParticle * , int> & idmap){
  top::NTGenParticle out;
  out.setP4(p->p4());
  out.setPdgId(p->pdgId());
  out.setStatus(p->status());
  out.setGenId(idmap.at(p));
  return out;
}
// searchdaughter(particle *, vectorparticle * daughters)
// searchmother(particle * , vector * possiblemothers)
 
bool  TreeWriter::isInGenCollection(const reco::GenParticle * p, const std::vector<const reco::GenParticle * > & coll){
  for(size_t i=0;i<coll.size();i++){
    if(coll.at(i) == p)
      return true;
  }
  return false;
}
bool  TreeWriter::isInGenCollection(const reco::GenJet * p, const std::vector<const reco::GenJet * > & coll){
  for(size_t i=0;i<coll.size();i++){
    if(coll.at(i) == p)
      return true;
  }
  return false;
}


template<class t, class u>
int TreeWriter::findGenMatchIdx(t * patinput , std::vector<u> & gen, double dR){
  double drmin2=100;
  int idx=-1;
  for(size_t i=0;i<gen.size();i++){
    double DRs=(patinput->eta() - gen.at(i).eta())*(patinput->eta() - gen.at(i).eta()) + (patinput->phi() - gen.at(i).phi())*(patinput->phi() - gen.at(i).phi());
    if(drmin2 > DRs){
      drmin2 = DRs;
      idx=i;
    }
  }
  if(drmin2 < dR*dR)
    return idx;
  else
    return -1;
}

std::vector<top::NTGenParticle> getFullDecayChainTop(const reco::GenParticle * p){

  std::vector<top::NTGenParticle>  out;
  /*
  const reco::GenParticle * mother,daughter;
  mother=p;
  size_t ndaug=mother->numberOfDaughters();
  bool gotStatus3=false;
  bool gotB=false;

 for(size_t i;i<ndaug;i++){
    daughter=dynamic_cast<const reco::GenParticle*>(mother->daughter(i));
    //first check for bs
    if(!isAbsApprox(daughter->pdgId(),5)) 
      continue;
    if(gotStatus3 && !isAbsApprox(daughter->status(),1))
      continue;

    gotB=true;
    out << daughter;
    mother=daughter;
    ndaug=mother->numberOfDaughters();
    i=0;
    gotStatus3=true;
 }

 gotStatus3=false;
 mother=p;
 size_t ndaug=mother->numberOfDaughters();


  for(size_t i;i<ndaug;i++){
    daughter=dynamic_cast<const reco::GenParticle*>(mother->daughter(i));
    //first check for bs

    if(checkrest && isAbsApprox(daughter->pdgId(),24)){ //got a W, make rest of decay chain
      out << daughter;
      mother=daughter;
      ndaug=mother->numberOfDaughters();
      i=0;
      continue;
    }
    else if(isAbsApprox(daughter->pdgId(),11) || ){ //got leps status 3



  }
  */
  return out;
}

//define this as a plug-in
DEFINE_FWK_MODULE(TreeWriter);
