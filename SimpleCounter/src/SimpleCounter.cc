// -*- C++ -*-
//
// Package:    SimpleCounter
// Class:      SimpleCounter
// 
/**\class SimpleCounter SimpleCounter.cc TtZAnalysis/SimpleCounter/src/SimpleCounter.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
 */
//
// Original Author:  Jan Kieseler,,,DESY
//         Created:  Thu Oct  4 16:04:20 CEST 2012
// $Id$
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Candidate/interface/CompositeCandidate.h"

#include <iostream>

//
// class declaration
//

class SimpleCounter : public edm::EDFilter {
public:
    explicit SimpleCounter(const edm::ParameterSet&);
    ~SimpleCounter();

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
    virtual void beginJob() ;
    virtual bool filter(edm::Event&, const edm::EventSetup&);
    virtual void endJob() ;

    virtual bool beginRun(edm::Run&, edm::EventSetup const&);
    virtual bool endRun(edm::Run&, edm::EventSetup const&);
    virtual bool beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);
    virtual bool endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);

    // ----------member data ---------------------------

    std::vector<edm::InputTag> inputs_;
    unsigned int minnumber_;
    bool oppoQ_;
    bool debug_;
    size_t sfc_;

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
SimpleCounter::SimpleCounter(const edm::ParameterSet& iConfig)
{
    //now do what ever initialization is needed
    inputs_    = iConfig.getParameter<std::vector<edm::InputTag> > ("src");
    minnumber_ = iConfig.getParameter<unsigned int> ("minNumber");
    oppoQ_ = iConfig.getParameter<bool> ("requireOppoQ");
    debug_ = iConfig.getParameter<bool> ("debug");
    sfc_=0;
    #ifndef CMSSW_LEQ_5
    produces<bool>();
    #endif
}


SimpleCounter::~SimpleCounter()
{

    // do anything here that needs to be done at desctruction time
    // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool
SimpleCounter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{

    bool enough=false;
    using namespace edm;
    Handle<reco::CandidateView> pIn;

    for(std::vector<edm::InputTag>::const_iterator input=inputs_.begin(); input<inputs_.end();++input){
        iEvent.getByLabel(*input,pIn);

        if(pIn->size() >= minnumber_){
            if(oppoQ_){
                int q0=pIn->at(0).charge();
                for(size_t i=1;i<pIn->size();i++){
                    /*
                    if((pIn->at(i).isMuon() && pIn->at(0).isMuon())
                            || (pIn->at(i).isElectron() && pIn->at(0).isElectron()))
                            continue;
                     */
                    if(pIn->at(i).charge() * q0 < 0){
                        if(debug_){
                            if((pIn->at(i).isMuon() && pIn->at(0).isMuon())
                                    || (pIn->at(i).isElectron() && pIn->at(0).isElectron())){
                                std::cout << "selected same flavour" <<std::endl;
                                sfc_++;
                            }
                        }
                        enough=true;
                        break;
                    }
                }
            }
            else{
                enough=true;
            }
        }
        if(enough) break;
    }
    #ifndef CMSSW_LEQ_5
    std::auto_ptr<bool> skim =std::auto_ptr<bool>(new bool);
    *skim=enough;
    iEvent.put(skim);
    #endif
    return enough;
}

// ------------ method called once each job just before starting event loop  ------------
void 
SimpleCounter::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
SimpleCounter::endJob() {
    if(debug_) std::cout << "same flavour count: " << sfc_ << std::endl;
}

// ------------ method called when starting to processes a run  ------------
bool 
SimpleCounter::beginRun(edm::Run&, edm::EventSetup const&)
{ 
    return true;
}

// ------------ method called when ending the processing of a run  ------------
bool 
SimpleCounter::endRun(edm::Run&, edm::EventSetup const&)
{
    return true;
}

// ------------ method called when starting to processes a luminosity block  ------------
bool 
SimpleCounter::beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
    return true;
}

// ------------ method called when ending the processing of a luminosity block  ------------
bool 
SimpleCounter::endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
    return true;
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
SimpleCounter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}
//define this as a plug-in
DEFINE_FWK_MODULE(SimpleCounter);
