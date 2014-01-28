// -*- C++ -*-
//
// Package:    PUInfo
// Class:      PUInfo
// 
/**\class PUInfo PUInfo.cc DOSS/PUInfo/src/PUInfo.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Jan Kieseler,,,DESY
//         Created:  Fri May 11 14:22:43 CEST 2012
// $Id: PUInfo.cc,v 1.2 2012/07/12 12:50:33 jkiesele Exp $
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
#include "../../DataFormats/interface/NTMuon.h"
#include "../../DataFormats/interface/NTElectron.h"
#include "../../DataFormats/interface/NTJet.h"
#include "../../DataFormats/interface/NTMet.h"
#include "../../DataFormats/interface/NTEvent.h"

#include "DataFormats/Candidate/interface/CompositeCandidate.h"

#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "FWCore/Common/interface/TriggerNames.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenRunInfoProduct.h"


#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
//#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"
//#include "PhysicsTools/Utilities/interface/Lumi3DReWeighting.h"


#include <algorithm>

#include <cstring>

//
// class declaration
//

class PUInfo : public edm::EDAnalyzer {
   public:
      explicit PUInfo(const edm::ParameterSet&);
      ~PUInfo();

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


  edm::InputTag puinfo_, vertices_,pdfweights_;
  TTree* Ntuple;
 
  bool includepdfweights_;

  ztop::NTEvent ntevent;

  std::string treename_;


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
PUInfo::PUInfo(const edm::ParameterSet& iConfig)
{
   //now do what ever initialization is needed
  treename_  =iConfig.getParameter<std::string> ("treeName");
  puinfo_       =iConfig.getParameter<edm::InputTag>    ( "PUInfo" );
  vertices_       =iConfig.getParameter<edm::InputTag>    ( "vertexSrc" );
  includepdfweights_ = iConfig.getParameter<bool>             ( "includePDFWeights" );
  pdfweights_  =iConfig.getParameter<edm::InputTag>    ( "pdfWeights"          );

   std::cout << "n\n################## PUInfo writer ######################" 
             <<  "\n#" << treename_
             <<  "\n#     includes pdfWeights : " << includepdfweights_    <<"                         #" 
             <<  "\n#######################################################" << std::endl;




}


PUInfo::~PUInfo()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
PUInfo::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

  bool IsRealData = false;
  edm::Handle <reco::GenParticleCollection> genParticles;
  try {
    iEvent.getByLabel("genParticles", genParticles);
    int aux = genParticles->size();
    aux = 0+aux;
  }
  catch(...) {IsRealData = true;} 



   Handle<std::vector<reco::Vertex> > vertices;
   iEvent.getByLabel(vertices_, vertices);
   const reco::VertexCollection vtxs  = *(vertices.product());


 
   std::vector<std::string> firedtriggers;

   ntevent.setRunNo(iEvent.id().run());
   ntevent.setLumiBlock(iEvent.id().luminosityBlock());
   ntevent.setEventNo(iEvent.id().event());
   ntevent.setVertexMulti(vertices->size());
   ntevent.setFiredTriggers(firedtriggers);

   float BXminus=0;
   float BXzero=0;
   float BXplus=0;

   //   if(!IsRealData){
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
   // }
   ntevent.setTruePU(BXminus,BXzero,BXplus);
   if(includepdfweights_ && !IsRealData){
     edm::Handle<std::vector<double> > weightHandle;
     iEvent.getByLabel(pdfweights_, weightHandle);
     std::vector<float> pdfweights;
     for(size_t i=0;i<weightHandle->size();i++)
     pdfweights.push_back(weightHandle->at(i));
     
     ntevent.setPDFWeights(pdfweights);
   }

   Ntuple ->Fill();

}


// ------------ method called once each job just before starting event loop  ------------
void 
PUInfo::beginJob()
{
  edm::Service<TFileService> fs;

  if( !fs ){
    throw edm::Exception( edm::errors::Configuration,
                          "TFile Service is not registered in cfg file" );
  }
  char * tempname = new char[treename_.length()];
  strcpy(tempname, treename_.c_str());
  Ntuple=fs->make<TTree>(tempname ,tempname );
  
  Ntuple->Branch("NTEvent", "ztop::NTEvent", &ntevent);

}

// ------------ method called once each job just after ending the event loop  ------------
void 
PUInfo::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------
void 
PUInfo::beginRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a run  ------------
void 
PUInfo::endRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block  ------------
void 
PUInfo::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void 
PUInfo::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
PUInfo::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}


//define this as a plug-in
DEFINE_FWK_MODULE(PUInfo);
