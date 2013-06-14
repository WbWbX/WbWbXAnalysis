// -*- C++ -*-
//
// Package:    genInfoTreeWriter
// Class:      genInfoTreeWriter
// 
/**\class genInfoTreeWriter genInfoTreeWriter.cc DOSS/genInfoTreeWriter/src/genInfoTreeWriter.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Jan Kieseler,,,DESY
//         Created:  Fri May 11 14:22:43 CEST 2012
// $Id: genInfoTreeWriter.cc,v 1.3 2013/04/04 17:37:24 jkiesele Exp $
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
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"


#include <algorithm>

#include <cstring>

//
// class declaration
//

class genInfoTreeWriter : public edm::EDAnalyzer {
   public:
      explicit genInfoTreeWriter(const edm::ParameterSet&);
      ~genInfoTreeWriter();

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

  TTree* Ntuple;

  double qscale,pdfscale,x1,x2,pdfx1,pdfx2,aqcd,aqed;
  int id1,id2;
 
  


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
genInfoTreeWriter::genInfoTreeWriter(const edm::ParameterSet& iConfig)
{

   std::cout << "n\n################## genInfoTreeWriter writer ######################" 
	     << std::endl;




}


genInfoTreeWriter::~genInfoTreeWriter()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
genInfoTreeWriter::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
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

  if(!IsRealData){ //do MC stuff

    edm::Handle<GenEventInfoProduct> genInfo;
    iEvent.getByLabel("generator", genInfo);

    pdfscale = genInfo->pdf()->scalePDF;
 
    id1 = genInfo->pdf()->id.first;
    x1 = genInfo->pdf()->x.first;
    pdfx1 = genInfo->pdf()->xPDF.first;
 
    id2 = genInfo->pdf()->id.second;
    x2 = genInfo->pdf()->x.second;
    pdfx2 = genInfo->pdf()->xPDF.second; 

    qscale=genInfo->qScale();

    aqcd=genInfo->alphaQCD();
    aqed=genInfo->alphaQED();

  }
  Ntuple ->Fill();

}


// ------------ method called once each job just before starting event loop  ------------
void 
genInfoTreeWriter::beginJob()
{
  edm::Service<TFileService> fs;

  if( !fs ){
    throw edm::Exception( edm::errors::Configuration,
                          "TFile Service is not registered in cfg file" );
  }
 
  Ntuple=fs->make<TTree>("GenTree" ,"GenTree" );
  
  Ntuple->Branch("QScale", "double", &qscale);
  Ntuple->Branch("PDFScale", "double", &pdfscale);
  Ntuple->Branch("ID1", "double", &id1);
  Ntuple->Branch("ID2", "double", &id2);
  Ntuple->Branch("X1", "double", &x1);
  Ntuple->Branch("X2", "double", &x2);
  Ntuple->Branch("PDFX1", "double", &pdfx1);
  Ntuple->Branch("PDFX2", "double", &pdfx2);
  Ntuple->Branch("AlphaQCD", "double", &aqcd);
  Ntuple->Branch("AlphaQED", "double", &aqed);

}

// ------------ method called once each job just after ending the event loop  ------------
void 
genInfoTreeWriter::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------
void 
genInfoTreeWriter::beginRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a run  ------------
void 
genInfoTreeWriter::endRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block  ------------
void 
genInfoTreeWriter::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void 
genInfoTreeWriter::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
genInfoTreeWriter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}


//define this as a plug-in
DEFINE_FWK_MODULE(genInfoTreeWriter);
