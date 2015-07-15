// -*- C++ -*-
//
// Package:    EventPreFilter
// Class:      EventPreFilter
// 
/**\class EventPreFilter EventPreFilter.cc TtZAnalysis/EventPreFilter/src/EventPreFilter.cc

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

class EventPreFilter : public edm::EDFilter {
public:
	explicit EventPreFilter(const edm::ParameterSet&);
	~EventPreFilter();

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

	std::vector<size_t> selectevents_;
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
EventPreFilter::EventPreFilter(const edm::ParameterSet& iConfig)
{
	//now do what ever initialization is needed
	//	selectevents_    = iConfig.getParameter<std::vector<size_t> > ("SelectEventNo");
	selectevents_.push_back(28488750);
	selectevents_.push_back(28488761);
	selectevents_.push_back(28488787);
	selectevents_.push_back(28488793);
	selectevents_.push_back(28488802);
	selectevents_.push_back(28488804);
	selectevents_.push_back(28488804);
	selectevents_.push_back(28488811);
	selectevents_.push_back(191120528);
	selectevents_.push_back(98530953);

}


EventPreFilter::~EventPreFilter()
{

	// do anything here that needs to be done at desctruction time
	// (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool
EventPreFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{

	using namespace edm;

	size_t thisevent=iEvent.id().event();
	if(std::find(selectevents_.begin(),selectevents_.end() ,thisevent) != selectevents_.end())
		return true;
	return false;
}

// ------------ method called once each job just before starting event loop  ------------
void
EventPreFilter::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void
EventPreFilter::endJob() {

}

// ------------ method called when starting to processes a run  ------------
bool
EventPreFilter::beginRun(edm::Run&, edm::EventSetup const&)
{
	return true;
}

// ------------ method called when ending the processing of a run  ------------
bool
EventPreFilter::endRun(edm::Run&, edm::EventSetup const&)
{
	return true;
}

// ------------ method called when starting to processes a luminosity block  ------------
bool
EventPreFilter::beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
	return true;
}

// ------------ method called when ending the processing of a luminosity block  ------------
bool
EventPreFilter::endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
	return true;
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
EventPreFilter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
	//The following says we do not know what parameters are allowed so do no validation
	// Please change this to state exactly what you do use, even if it is no parameters
	edm::ParameterSetDescription desc;
	desc.setUnknown();
	descriptions.addDefault(desc);
}
//define this as a plug-in
DEFINE_FWK_MODULE(EventPreFilter);
