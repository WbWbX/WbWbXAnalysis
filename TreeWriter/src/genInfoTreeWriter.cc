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
// $Id: genInfoTreeWriter.cc,v 1.1 2013/06/14 15:45:36 jkiesele Exp $
//
//


// system include files
#include <memory>

// user include files

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenRunInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"

#include "TTree.h"

#include <iostream>

#include "TopAnalysis/ZTopUtils/interface/consumeTemplate.h"

// class declaration
//

class genInfoTreeWriter : public AnalyzerTemplate {
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
	consumeTemplate<GenEventInfoProduct>(edm::InputTag("generator"));




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


	edm::Handle<GenEventInfoProduct> genInfo;
	try {
		iEvent.getByLabel("generator", genInfo);
	}
	catch(...) {IsRealData = true;}

	if(!IsRealData){ //do MC stuff

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

	Ntuple->Branch("QScale", &qscale);
	Ntuple->Branch("PDFScale", &pdfscale);
	Ntuple->Branch("ID1", &id1);
	Ntuple->Branch("ID2", &id2);
	Ntuple->Branch("X1", &x1);
	Ntuple->Branch("X2", &x2);
	Ntuple->Branch("PDFX1", &pdfx1);
	Ntuple->Branch("PDFX2", &pdfx2);
	Ntuple->Branch("AlphaQCD", &aqcd);
	Ntuple->Branch("AlphaQED", &aqed);

}

// ------------ method called once each job just after ending the event loop  ------------
void 
genInfoTreeWriter::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------
void 
genInfoTreeWriter::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup)
{

	edm::Handle<LHERunInfoProduct> lheRunInfo;
	typedef std::vector<LHERunInfoProduct::Header>::const_iterator headers_const_iterator;
	iRun.getByLabel("externalLHEProducer", lheRunInfo);
	//catch (...) {;}
	if(lheRunInfo.isValid()){
		std::cout<<"Print LHE HEADER "<<std::endl;
		LHERunInfoProduct lheRunInfoProd = *(lheRunInfo.product());
		for (headers_const_iterator iter=lheRunInfoProd.headers_begin(); iter!=lheRunInfoProd.headers_end(); iter++){
			std::cout << iter->tag() << std::endl;
			std::vector<std::string> lines(iter->begin(),iter->end());
			for (unsigned int iLine = 0; iLine<lines.size(); iLine++) std::cout << lines.at(iLine);
		}
	}else{
		edm::LogWarning ("RivetAnalyzer::beginRun") << "Can't get LHE header!" ;
	}

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
