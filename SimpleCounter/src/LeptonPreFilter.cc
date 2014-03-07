// -*- C++ -*-
//
// Package:    LeptonPreFilter
// Class:      LeptonPreFilter
// 
/**\class LeptonPreFilter LeptonPreFilter.cc TtZAnalysis/LeptonPreFilter/src/LeptonPreFilter.cc

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

class LeptonPreFilter : public edm::EDFilter {
public:
    explicit LeptonPreFilter(const edm::ParameterSet&);
    ~LeptonPreFilter();

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

    std::vector<edm::InputTag> inputsa_,inputsb_;
    double ptcut_;
    bool debug_,twokinds_;
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
LeptonPreFilter::LeptonPreFilter(const edm::ParameterSet& iConfig)
{
    //now do what ever initialization is needed
    inputsa_    = iConfig.getParameter<std::vector<edm::InputTag> > ("Asrc");
    inputsb_    = iConfig.getParameter<std::vector<edm::InputTag> > ("Bsrc");
    twokinds_ = false;
    if(inputsb_.size()>0)twokinds_=true;
    debug_ = iConfig.getParameter<bool> ("debug");

    ptcut_ = iConfig.getParameter<double> ("minPt");
    sfc_=0;
}


LeptonPreFilter::~LeptonPreFilter()
{

    // do anything here that needs to be done at desctruction time
    // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool
LeptonPreFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{

    using namespace edm;

    int chargemulti=-1;

    /*
     * require two opposite charge candidates within all inputs
     * if twokinds, check all combinations
     */

    std::vector<int> chargesa;
    int q0=100;


    for(std::vector<edm::InputTag>::const_iterator inputa=inputsa_.begin(); inputa<inputsa_.end();++inputa){
        Handle< reco::CandidateView >paIn;
        iEvent.getByLabel(*inputa,paIn);
        for(size_t i=0;i<paIn->size();i++){

            if(paIn->at(i).pt() < ptcut_) continue;
            if(!twokinds_){
                if(q0>90){ q0=paIn->at(i).charge(); continue;}
                if(q0 * paIn->at(i).charge() * chargemulti > 0) return true;
            }
            else{
                chargesa.push_back(paIn->at(i).charge());
            }

        }
    }//input a
    if(twokinds_){
        for(std::vector<edm::InputTag>::const_iterator inputb=inputsb_.begin(); inputb<inputsb_.end();++inputb){
            Handle<reco::CandidateView > pbIn;
            iEvent.getByLabel(*inputb,pbIn);
            for(size_t i=0;i<pbIn->size();i++){
                if(pbIn->at(i).pt() < ptcut_) continue;
                //size_t idx=0;
                int searchforcharge=chargemulti*(pbIn->at(i).charge());
                if(std::find(chargesa.begin(),chargesa.end(),searchforcharge) != chargesa.end()){

                    return true;
                }
            }
        }
    }

    return false;
}

// ------------ method called once each job just before starting event loop  ------------
void
LeptonPreFilter::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void
LeptonPreFilter::endJob() {
    if(debug_) std::cout << "same flavour count: " << sfc_ << std::endl;
}

// ------------ method called when starting to processes a run  ------------
bool
LeptonPreFilter::beginRun(edm::Run&, edm::EventSetup const&)
{
    return true;
}

// ------------ method called when ending the processing of a run  ------------
bool
LeptonPreFilter::endRun(edm::Run&, edm::EventSetup const&)
{
    return true;
}

// ------------ method called when starting to processes a luminosity block  ------------
bool
LeptonPreFilter::beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
    return true;
}

// ------------ method called when ending the processing of a luminosity block  ------------
bool
LeptonPreFilter::endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
    return true;
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
LeptonPreFilter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}
//define this as a plug-in
DEFINE_FWK_MODULE(LeptonPreFilter);
