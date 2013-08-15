#include "../interface/TreeWriterBase.h"

#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"

TreeWriterBase::TreeWriterBase(const edm::ParameterSet& iConfig)
{

	debugmode  =iConfig.getParameter<bool>              ( "debugmode" );

	//now do what ever initialization is needed
	treename_    =iConfig.getParameter<std::string>        ("treeName");
	muons_       =iConfig.getParameter<edm::InputTag>    ( "muonSrc" );
	gsfelecs_       =iConfig.getParameter<edm::InputTag>    ( "elecGSFSrc" );
	pfelecs_       =iConfig.getParameter<edm::InputTag>    ( "elecPFSrc" );
	jets_        =iConfig.getParameter<edm::InputTag>    ( "jetSrc" );
	btagalgo_    =iConfig.getParameter<std::string>       ("btagAlgo");
	met_         =iConfig.getParameter<edm::InputTag>    ( "metSrc" );
	mvamet_         =iConfig.getParameter<edm::InputTag>    ( "mvaMetSrc" );
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
	pattriggerevent_     =iConfig.getParameter<edm::InputTag>    ( "triggerEvent" );

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




	//Input from GenLevelBJetProducer
	bHadJetIdx_ = iConfig.getParameter<edm::InputTag> ("BHadJetIndex");
	antibHadJetIdx_= iConfig.getParameter<edm::InputTag> ("AntiBHadJetIndex");
	BHadrons_= iConfig.getParameter<edm::InputTag> ("BHadrons");
	AntiBHadrons_= iConfig.getParameter<edm::InputTag> ("AntiBHadrons");
	BHadronFromTopB_ = iConfig.getParameter<edm::InputTag> ("BHadronFromTopB");
	AntiBHadronFromTopB_= iConfig.getParameter<edm::InputTag> ("AntiBHadronFromTopB");
	BHadronVsJet_= iConfig.getParameter<edm::InputTag> ("BHadronVsJet");
	AntiBHadronVsJet_= iConfig.getParameter<edm::InputTag> ("AntiBHadronVsJet");

	genBHadPlusMothers_= iConfig.getParameter<edm::InputTag> ("genBHadPlusMothers");
	genBHadPlusMothersIndices_= iConfig.getParameter<edm::InputTag> ("genBHadPlusMothersIndices");
	genBHadIndex_= iConfig.getParameter<edm::InputTag> ("genBHadIndex");
	genBHadFlavour_= iConfig.getParameter<edm::InputTag> ("genBHadFlavour");
	genBHadJetIndex_= iConfig.getParameter<edm::InputTag> ("genBHadJetIndex");


	useBHadrons_ = iConfig.getParameter<bool> ("useBHadrons");

	susy_= iConfig.getParameter<bool> ("isSusy");
	jpsi_= iConfig.getParameter<bool> ("isJPsi");

	triggerObjects_ = iConfig.getParameter<std::vector<std::string> > ("triggerObjects");


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

	std::cout << "writing Muons: " << muons_ << std::endl;
	std::cout << "writing GSF: " << gsfelecs_ << std::endl;
	std::cout << "writing PFElecs: " << pfelecs_ << std::endl;

	if(triggerObjects_.size() > 0 && includetrigger_){
		std::cout << "writing trigger objects: " << std::endl;
		for(size_t i=0;i<triggerObjects_.size();i++)
			std::cout << triggerObjects_.at(i) << " ";
		std::cout << std::endl;
	}

	//set trigger bools
	setTriggers();

}


TreeWriterBase::~TreeWriterBase()
{

	// do anything here that needs to be done at desctruction time
	// (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
TreeWriterBase::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
	using namespace edm;
	using namespace ztop;
	using namespace std;
	//using namespace zztop;


	if(debugmode) std::cout << "event loop started" << std::endl;

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

	goodvtx.clear();
	jetvtx.clear();
	dimuonvtx.clear();

	nttops.clear();
	ntws.clear();
	ntzs.clear();
	ntbs.clear();
	ntbhadrons.clear();
	ntnus.clear();
	ntleps3.clear();
	ntleps1.clear();
	ntallnus.clear();

	ntgenjets.clear();

	vstopmass.clear();
	vchimass.clear();

	triggerBools_.clear();
	alltriggerswithprescales_.clear();
	triggerPrescales_.clear();

	for(size_t i=0;i<trigObjVec.size();i++)
		trigObjVec.at(i).clear();

	ztop::NTTrigger clear;
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

	std::map<const reco::GenParticle *  , size_t> smallIdMap; ////

	if(!IsRealData && includegen_){ ///



		///couts//// all tagged with ##cout##



		const reco::GenParticle * daughter;
		const reco::GenParticle * mother;


		std::vector<const reco::GenParticle *> allgen, tops, Zs, Ws, bs, Bhadrons, nus, leps1, leps3, allnus;



		// nttops,ntws,ntbs,ntbhadrons,ntnus,ntleps3,ntleps1, ntallnus;

		if(debugmode) std::cout << "filling all gen Particles" << std::endl;

		bool gotTop=false;

		int genidit=0;


		for(size_t i=0;i<genParticles->size();i++){
			allgen << &genParticles->at(i);
		}

		if(debugmode) std::cout << "Filling tops" << std::endl;

		for(size_t i=0;i<allgen.size();i++){
			if(fabs(allgen.at(i)->pdgId() -6) < 0.1){ // 6
				ztop::NTGenParticle temp=makeNTGen(allgen.at(i));
				temp.setGenId(genidit++);
				nttops << temp;
				tops << allgen.at(i);
				gotTop=true;
			}
			else if(fabs(allgen.at(i)->pdgId() +6) < 0.1){ // -6
				ztop::NTGenParticle temp=makeNTGen(allgen.at(i));
				temp.setGenId(genidit++);
				nttops << temp;
				tops << allgen.at(i);
			}
			if(tops.size() > 1) break;
		}

		//makes shure the right ordering is done
		///////////////

		bool gotWFromTop=false;
		////////////to be used for other generator types!

		if(debugmode) std::cout << "filling Ws" << std::endl;

		if(tops.size()>1){ //should always be the case for ttbar sample
			for(size_t i=0;i<tops.size();i++){
				mother=tops.at(i);
				for(size_t j=0;j<mother->numberOfDaughters();j++){
					daughter = dynamic_cast<const reco::GenParticle*>(mother->daughter(j));
					if(fabs(daughter->pdgId() - 24) < 0.1){
						ztop::NTGenParticle temp=makeNTGen(daughter);
						temp.setGenId(genidit++);
						temp.addMotherIt(nttops.at(i).genId());
						nttops.at(i).addDaughterIt(temp.genId());
						ntws << temp;
						Ws   << daughter;
						gotWFromTop=true;
					}
					if(fabs(daughter->pdgId() + 24) < 0.1){
						ztop::NTGenParticle temp=makeNTGen(daughter);
						temp.setGenId(genidit++);
						temp.addMotherIt(nttops.at(i).genId());
						nttops.at(i).addDaughterIt(temp.genId());
						ntws << temp;
						Ws   << daughter;
					}
				}
			}
		}

		//////direct W decay leps3 and nus

		if(debugmode) std::cout << "filling neutrinoes and stat3 leps from W" << std::endl;

		for(size_t i=0;i<Ws.size();i++){
			mother=Ws.at(i);

			for(size_t j=0;j<mother->numberOfDaughters();j++){
				daughter = dynamic_cast<const reco::GenParticle*>(mother->daughter(j));
				if(isAbsApprox(daughter->pdgId(), 11) || isAbsApprox(daughter->pdgId(), 13) || isAbsApprox(daughter->pdgId(), 15)){
					ztop::NTGenParticle temp=makeNTGen(daughter);
					temp.setGenId(genidit++);
					temp.addMotherIt(ntws.at(i).genId());
					ntws.at(i).addDaughterIt(temp.genId());
					ntleps3 << temp;
					leps3 << daughter;
				}
				if(isAbsApprox(daughter->pdgId(), 12) || isAbsApprox(daughter->pdgId(), 14) || isAbsApprox(daughter->pdgId(), 16)){
					ztop::NTGenParticle temp=makeNTGen(daughter);
					temp.setGenId(genidit++);
					temp.addMotherIt(ntws.at(i).genId());
					ntws.at(i).addDaughterIt(temp.genId());
					ntnus << temp;
					nus << daughter;
				}
			}
		}

		if(!gotWFromTop && !gotTop){ //must b Z or something else

			if(debugmode) std::cout << "Filling Z" << std::endl;

			for(size_t i=0;i<allgen.size();i++){
				if(fabs(allgen.at(i)->pdgId() -23) < 0.1){ // 23
					ztop::NTGenParticle temp=makeNTGen(allgen.at(i));
					temp.setGenId(genidit++);
					ntzs << temp;
					Zs << allgen.at(i);
					break;
				}
			}

			if(debugmode) std::cout << "Fillig Z daughters stat 3" << std::endl;

			for(size_t i=0;i<Zs.size();i++){
				mother=Zs.at(i);
				for(size_t j=0;j<mother->numberOfDaughters();j++){
					daughter = dynamic_cast<const reco::GenParticle*>(mother->daughter(j));
					if(isAbsApprox(daughter->pdgId(), 11) || isAbsApprox(daughter->pdgId(), 13) || isAbsApprox(daughter->pdgId(), 15)){
						ztop::NTGenParticle temp=makeNTGen(daughter);
						temp.setGenId(genidit++);
						temp.addMotherIt(ntzs.at(i).genId());
						ntzs.at(i).addDaughterIt(temp.genId());
						ntleps3 << temp;
						leps3 << daughter;
					}
					if(isAbsApprox(daughter->pdgId(), 12) || isAbsApprox(daughter->pdgId(), 14) || isAbsApprox(daughter->pdgId(), 16)){
						ztop::NTGenParticle temp=makeNTGen(daughter);
						temp.setGenId(genidit++);
						temp.addMotherIt(ntws.at(i).genId());
						ntzs.at(i).addDaughterIt(temp.genId());
						ntnus << temp;
						nus << daughter;
					}
				}
			}
		}

		/////further lep decay to stat 1 leps; the above can be stat 1, too
		if(debugmode) std::cout << "Filling stat 1 leptons" << std::endl;

		int newid=0;
		for(size_t j=0;j<leps3.size();j++){
			mother=leps3[j];
			size_t i=0;
			size_t ndaugh=mother->numberOfDaughters();
			while(i<ndaugh){
				daughter = dynamic_cast<const reco::GenParticle*>(mother->daughter(i));
				i++;
				if(isAbsApprox(daughter->pdgId(),11) || isAbsApprox(daughter->pdgId(),13) || isAbsApprox(daughter->pdgId(),15)){
					if(isAbsApprox(daughter->status(),1)){
						ztop::NTGenParticle temp=makeNTGen(daughter);
						temp.setGenId(genidit++);
						temp.addMotherIt(ntleps3.at(j).genId());
						ntleps3.at(j).addDaughterIt(temp.genId());
						leps1 << daughter;
						smallIdMap[daughter]=newid;
						ntleps1 << temp;
						newid++;
						break;
					}
					else{
						mother=daughter;
						i=0;
						ndaugh=mother->numberOfDaughters();
					}
				}
			}
		}
		//all nus (for met or other stuff)

		if(debugmode) std::cout << "filling all neutrinos" << std::endl;

		for(size_t i=0;i<allgen.size();i++){
			if(isAbsApprox(allgen.at(i)->pdgId(), 12)
					|| isAbsApprox(allgen.at(i)->pdgId(), 14)
					|| isAbsApprox(allgen.at(i)->pdgId(), 16)){
				ztop::NTGenParticle temp=makeNTGen(allgen.at(i));
				// temp.setGenId(genidit++);
				ntallnus << temp;
				allnus << allgen.at(i);
			}
		}


		////////////////////////////////////// 1st entry of (nt)bs should contain a b quark
		////////////////////////////
		//leptonic part finished, add jet part
		///////////// GEN JETS AND HADRONS

		edm::Handle<reco::GenJetCollection> genJets;
		iEvent.getByLabel(genjets_, genJets);

		/*  edm::Handle<std::vector<int> > BHadJetIndex;
     edm::Handle<std::vector<int> > AntiBHadJetIndex;
     edm::Handle<std::vector<reco::GenParticle> > BHadrons;
     edm::Handle<std::vector<reco::GenParticle> > AntiBHadrons;
     edm::Handle<std::vector<bool> > BHadronFromTopB;
     edm::Handle<std::vector<bool> > AntiBHadronFromTopB;
     edm::Handle<std::vector<int> > BHadronVsJet;
     edm::Handle<std::vector<int> > AntiBHadronVsJet; */
		edm::Handle<std::vector<std::vector<int> > > genBHadPlusMothersIndices;
		edm::Handle<std::vector<int> > genBHadFlavour;
		edm::Handle<std::vector<int> > genBHadJetIndex;
		edm::Handle<std::vector<reco::GenParticle> > genBHadPlusMothers;
		edm::Handle<std::vector<int> > genBHadIndex;

		if(useBHadrons_){

			iEvent.getByLabel(genBHadPlusMothersIndices_, genBHadPlusMothersIndices);
			iEvent.getByLabel(genBHadFlavour_, genBHadFlavour);
			iEvent.getByLabel(genBHadJetIndex_, genBHadJetIndex);
			iEvent.getByLabel(genBHadPlusMothers_, genBHadPlusMothers);
			iEvent.getByLabel(genBHadIndex_, genBHadIndex);


		}


		/////////




		if(debugmode) std::cout << "Filling b- quarks" << std::endl;

		for(size_t i=0;i<tops.size();i++){
			mother=tops[i];
			for(size_t j=0;j<mother->numberOfDaughters();j++){
				daughter = dynamic_cast<const reco::GenParticle*>(mother->daughter(j));
				if(isAbsApprox(daughter->pdgId(),5)){
					ztop::NTGenParticle temp=makeNTGen(daughter);
					temp.setGenId(genidit++);
					temp.addMotherIt(nttops.at(i).genId());
					nttops.at(i).addDaughterIt(temp.genId());
					ntbs << temp;
					bs << daughter;
				}
			}
		}

		/////B hadrons
		vector<int> tempBDaugh;
		if(useBHadrons_){
			if(debugmode) std::cout << "Filling b-hadrons" << std::endl;

			for(size_t i=0;i<genBHadFlavour->size();i++){
				const reco::GenParticle * B = &genBHadPlusMothers->at(i);
				int motherFlavour     =  genBHadFlavour->at(i);
				int assoJetIdx        = genBHadJetIndex->at(i);

				ztop::NTGenParticle tmpB=makeNTGen(B);
				tmpB.setGenId(genidit++);
				tempBDaugh.push_back(assoJetIdx);
				//  tmpB.addDaughterIt(assoJetIdx);
				//  size_t mother=999;
				if(isAbsApprox(motherFlavour,6)){ //from top
					//   loop only for security reasons.
					for(size_t h=0;h<ntbs.size();h++){
						ztop::NTGenParticle * b=& ntbs.at(h);
						if(b->pdgId() * motherFlavour > 0){ // top-b match or antitop-antib
							tmpB.addMotherIt(b->genId());
							b->addDaughterIt(tmpB.genId());
							break;
						}
					}
				}
				ntbhadrons << tmpB;
			}

		}
		if(debugmode) std::cout << "Filling genjets" << std::endl;

		for(size_t i=0;i<genJets->size();i++){
			const reco::GenJet * jet=&genJets->at(i);
			ztop::NTGenJet tempjet=makeNTGenJet(jet);
			tempjet.setGenId(genidit++);

			for(size_t h=0;h<tempBDaugh.size();h++){ //only > 0 if bhadrons were filled so safe
				ztop::NTGenParticle * B= & ntbhadrons.at(h);
				if((size_t)tempBDaugh.at(h) == i){
					tempjet.addMotherIt(B->genId());
					B->addDaughterIt(tempjet.genId());

					if(debugmode) std::cout << "associated Hadron (id): " << B->genId() << " to jet (id): " << tempjet.genId() <<  std::endl;
					//no break there might be some hadrons merged in one jet
				}
			}
			ntgenjets << tempjet;
		}

		//////susy

		if(susy_){
			for(size_t i=0;i<allgen.size();i++){
				if(fabs(fabs(allgen.at(i)->pdgId()) -1000006) < 0.1){ // +-1000006
					// ztop::NTGenParticle temp=makeNTGen(allgen.at(i));

					vstopmass.push_back(allgen.at(i)->mass());

					size_t ndau = allgen.at(i)->numberOfDaughters();
					bool foundTop = false;

					for( size_t j = 0; j < ndau; ++ j ) {

						if( fabs( fabs(allgen.at(i)->daughter(j)->pdgId())-6 ) <0.1 ) { // if the i-th daughter is a top or an anti-top
							foundTop = true;

							LorentzVector LVstop( allgen.at(i)->px(), allgen.at(i)->py(), allgen.at(i)->pz(), allgen.at(i)->energy() );
							LorentzVector LVtop( allgen.at(i)->daughter(j)->px(), allgen.at(i)->daughter(j)->py(), allgen.at(i)->daughter(j)->pz(), allgen.at(i)->daughter(j)->energy() );

							vchimass.push_back( (LVstop-LVtop).M() );
							//std::cout << "chi0 mass = " << (LVstop-LVtop).M() << endl;
							break;
						}
					}
					if( !foundTop )// stop/anti-stop has no top/anti-top daughter! m_chi0 set to an unphysical value
						vchimass.push_back(-99999.);
				} //if
			}

			//do gen selection on tree writer level
			if(vstopmass.size() < 1)
				return;
			if(vstopmass.at(0) > 350)
				return;
			if(vstopmass.at(0) < 250)
				return;
			float diff=vstopmass.at(0) - vchimass.at(0);
			if(diff > 250)
				return;
			if(diff < 200)
				return;
		}



	}//isMC and includegen end

	if(debugmode){
		std::cout <<"One full decay path for bs: " << std::endl;
		for(size_t f=0;f<nttops.size();f++){
			std::cout << "top: "  << nttops.at(f).pdgId() << "(" << nttops.at(f).genId() << ")";
			for(size_t i=0;i<nttops.at(f).daughterIts().size();i++){
				for(size_t j=0;j<ntbs.size();j++){
					if(ntbs.at(j).genId() == nttops.at(f).daughterIts().at(i)){
						std::cout << " -> b-quark: " << ntbs.at(j).pdgId() << "(" << ntbs.at(j).genId() << ")";
						for(size_t k=0;k<ntbs.at(j).daughterIts().size();k++){
							for(size_t l=0;l<ntbhadrons.size();l++){
								if(ntbs.at(j).daughterIts().at(k) == ntbhadrons.at(l).genId()){
									std::cout << " -> B-hadron: " << ntbhadrons.at(l).pdgId() << "(" << ntbhadrons.at(l).genId() << ")" << std::endl;
								}
							}
						}
					}
				}
			}
		}
	}
	if(debugmode) std::cout <<"includegen left" << std::endl;

	//////////starting RECO part




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

	Handle<std::vector<pat::MET> > mvamets;
	iEvent.getByLabel(mvamet_,mvamets);

	Handle<std::vector<reco::Vertex> > vertices;
	iEvent.getByLabel(vertices_, vertices);
	vtxs  = *(vertices.product());

	if(vtxs.size()>0){
		if(debugmode) std::cout << "vtxs.size()>0" << std::endl;
		if(jpsi_){
			if(debugmode) std::cout << "filling jpsi good vertices" << std::endl;
			for(size_t i=0;i<vtxs.size();i++){
				reco::Vertex * vtx=&vtxs.at(i);
				math::XYZPoint vpoint=vtx->position();
				double vchi2=vtx->chi2();
				float vndof=vtx->ndof();
				TVector3 vpos(vpoint.X(),vpoint.Y(),vpoint.Z());
				TVector3 vposerr(vtx->covariance(0,0),vtx->covariance(1,1),vtx->covariance(2,2));
				ztop::NTVertex tempvtx;
				tempvtx.setPos(vpos);
				tempvtx.setPosErr(vposerr);
				tempvtx.setNDof(vndof);
				tempvtx.setChi2(vchi2);
				tempvtx.setIdx(i);
				goodvtx.push_back(tempvtx);
			}
		}



		///////////////////////////////////////////////E L E C T R O N S//////////
		//recent changes: stares two collections (gsf and pf), ecaldrivenmomentum in BOTH BE CAREFUL WHEN CHANGING


		math::XYZPoint beamSpotPosition;
		beamSpotPosition.SetCoordinates(0,0,0);

		edm::Handle<reco::BeamSpot> bsHandle;
		iEvent.getByLabel("offlineBeamSpot",bsHandle);

		//double BSx=0,BSy=0,BSz=0;
		if( (bsHandle.isValid()) ){
			/*reco::BeamSpot bs = *bsHandle;
			BSx = bs.x0();
			BSy = bs.y0();
			BSz = bs.z0();*/
			beamSpotPosition = bsHandle->position();
		}



		if(debugmode) std::cout << "electron loops" << std::endl;


		////Electrons/////


		edm::Handle<std::vector<pat::Electron> >  temppatelecs=gsfelecs;

		for(size_t vecs=0;vecs<2;vecs++){

			for(size_t i=0;i<temppatelecs->size();i++){

				ztop::NTElectron tempelec;
				tempelec=makeNTElectron(temppatelecs->at(i));

				int genidx=-1;

				if(temppatelecs->at(i).genParticle()){ // pat genmatch exists;

					if(smallIdMap.find(temppatelecs->at(i).genParticle()) != smallIdMap.end())
						genidx=smallIdMap.find(temppatelecs->at(i).genParticle())->second;                // use pat match
					else
						genidx=findGenMatchIdx(temppatelecs->at(i).genParticle(),ntleps1,0.1); //match pat match to closest from ntgen collection

					if(genidx>=0){
						tempelec.setGenMatch(genidx);
						tempelec.setGenP4(ntleps1.at(genidx).p4());
					}
					// else: particle is not coming (directly) coming from signal process (or higher order - not reflected in ntallgen)

				}
				else{
					tempelec.setGenP4(p4zero);
					tempelec.setGenMatch(-1);
				}
				if(vecs ==0)
					ntgsfelectrons.push_back(tempelec);
				if(vecs ==1)
					ntpfelectrons.push_back(tempelec);
			}

			temppatelecs=pfelecs;

		}


		/////////////////////////////////// M U O N S//////////////////


		if(debugmode) std::cout << "muon loops" << std::endl;

		edm::ESHandle<TransientTrackBuilder> theTTBuilder;
		KalmanVertexFitter vtxFitter;
		if(jpsi_){
			iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",theTTBuilder);
		}

		for(size_t i=0;i<muons->size();i++){
			const pat::Muon * patmuon=&muons->at(i);
			ztop::NTMuon tempmuon;
			tempmuon=makeNTMuon(muons->at(i));



			////////JPSi specific///////
			if(jpsi_ && theTTBuilder.isValid()){
				reco::PFCandidateRef pfmuptr=patmuon->pfCandidateRef();
				{
					for(size_t j=i+1;j<muons->size();j++){
						if(muons->size()>2)
							break;
						reco::PFCandidateRef pfmuptr2=muons->at(j).pfCandidateRef();
						//split to function here
						vector<reco::TransientTrack> t_tks;
						t_tks.push_back(theTTBuilder->build(*(pfmuptr->trackRef())));
						t_tks.push_back(theTTBuilder->build(*(pfmuptr2->trackRef())));
						TransientVertex myVertex = vtxFitter.vertex(t_tks);
						NTVertex tempvtx;
						tempvtx.setChi2(999999); //default
						tempvtx.setIdx(i); //associate with first muon
						if (myVertex.isValid()) {
							tempvtx.setPos(TVector3(myVertex.position().x(),
									myVertex.position().y(),
									myVertex.position().z()));
							tempvtx.setPosErr(TVector3(myVertex.positionError().cxx(),
									myVertex.positionError().cyy(),
									myVertex.positionError().czz()));
							tempvtx.setChi2(myVertex.totalChiSquared());
							tempvtx.setNDof(myVertex.degreesOfFreedom());
						}
						dimuonvtx << tempvtx;
					}
				}
				LorentzVector ptrack(pfmuptr->trackRef()->px(),
						pfmuptr->trackRef()->py(),
						pfmuptr->trackRef()->pz(),
						pfmuptr->trackRef()->p());

				tempmuon.setTrackP4(ptrack);
			}

			int genidx=-1;

			if(muons->at(i).genParticle()){ // pat genmatch exists;

				if(smallIdMap.find(muons->at(i).genParticle()) != smallIdMap.end())
					genidx=smallIdMap.find(muons->at(i).genParticle())->second;                // use pat match
				else
					genidx=findGenMatchIdx(muons->at(i).genParticle(),ntleps1,0.1); //match pat match to closest from ntgen collection

				if(genidx>=0){
					tempmuon.setGenMatch(genidx);
					tempmuon.setGenP4(ntleps1.at(genidx).p4());
				}
				// else: particle is not coming (directly) coming from signal process (or higher order - not reflected in ntallgen)
			}
			else{
				tempmuon.setGenP4(p4zero);
				tempmuon.setGenMatch(-1);
			}
			ntmuons.push_back(tempmuon);
		}



		if(includereco_){


			if(debugmode) std::cout <<"includereco entered" << std::endl;

			if(!pfMuonCands_){
				for(std::vector<reco::Muon>::const_iterator recomuon=recomuons->begin(); recomuon<recomuons->end() ; recomuon++){
					ztop::NTLepton templep;
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
					templep.setDzV(vz);
					templep.setDzVErr(vzerr);

					ntleptons.push_back(templep);
				}
			}
			else{
				for(std::vector<reco::PFCandidate>::const_iterator recomuon=recopfmuons->begin(); recomuon<recopfmuons->end() ; recomuon++){
					ztop::NTLepton templep;
					templep.setP4(recomuon->p4());
					templep.setQ(recomuon->charge());
					double vz=-9999;
					double vzerr=-9999;
					if(!(recomuon->trackRef().isNull())){
						vz=recomuon->trackRef()->dz(vtxs[0].position());
						vzerr=recomuon->trackRef()->dzError();
					}
					templep.setDzV(vz);
					templep.setDzVErr(vzerr);

					ntleptons.push_back(templep);

				}
			}

			if(!pfElecCands_){
				for(std::vector<reco::GsfElectron>::const_iterator recoelectron=recoelecs->begin(); recoelectron<recoelecs->end() ; recoelectron++){
					ztop::NTLepton templep;
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
					templep.setDzV(vz);
					templep.setDzVErr(vzerr);
					ntleptons.push_back(templep);
				}
			}
			else{
				for(std::vector<reco::PFCandidate>::const_iterator recoelec=recopfelecs->begin(); recoelec<recopfelecs->end() ; recoelec++){
					ztop::NTLepton templep;
					templep.setP4(recoelec->p4());
					templep.setQ(recoelec->charge());
					double vz=-9999;
					double vzerr=-9999;
					if(!(recoelec->trackRef().isNull())){
						vz=recoelec->trackRef()->dz(vtxs[0].position());
						vzerr=recoelec->trackRef()->dzError();
					}
					templep.setDzV(vz);
					templep.setDzVErr(vzerr);

					ntleptons.push_back(templep);
				}
			}

		}


		if(debugmode) std::cout << "jet loop" << std::endl;

		for(size_t i=0;i<jets->size();i++){

			pat::Jet uncJet=jets->at(i).correctedJet("Uncorrected");
			const pat::Jet * patjet=&jets->at(i);

			if(uncJet.pt() < 10) continue;

			ztop::NTJet tempjet;
			tempjet=makeNTJet(jets->at(i));

			int genidx=findGenMatchIdx((jets->at(i).genJet()),ntgenjets,0.1);
			tempjet.setGenMatch(genidx);
			if(jpsi_){ //add associated jet vertex
				if(debugmode)
					std::cout << "entering jpsi jet-vertex association" << std::endl;

				const reco::SecondaryVertexTagInfo *svTagInfo = patjet->tagInfoSecondaryVertex();

				double vchi2=99999;
				float vndof=1;

				math::XYZPoint vpoint;
				const reco::Vertex * patjetvtx;
				if(svTagInfo && svTagInfo->nVertices()>0){
					patjetvtx=&svTagInfo->secondaryVertex(0);
					vpoint=patjetvtx->position();
					chi2=patjetvtx->chi2();
					ndof=patjetvtx->ndof();
				}
				if(debugmode)
					std::cout << vpoint << std::endl;

				TVector3 vpos(vpoint.X(),vpoint.Y(),vpoint.Z());
				TVector3 vposerr;//(patjet->vertexCovariance(0,0),patjet->vertexCovariance(1,1),patjet->vertexCovariance(2,2));
				ztop::NTVertex tempvtx;
				tempvtx.setPos(vpos);
				tempvtx.setPosErr(vposerr);
				tempvtx.setNDof(vndof);
				tempvtx.setChi2(vchi2);
				tempvtx.setIdx(i);
				jetvtx.push_back(tempvtx);
				tempjet.setMember(99,i);
			}


			ntjets.push_back(tempjet);
		}


		if(debugmode) std::cout << "met loops" << std::endl;

		for(std::vector<pat::MET>::const_iterator met=mets->begin(); met<mets->end() ; met++){
			ntmet.setP4(met->p4());
			break;

		}

		for(std::vector<pat::MET>::const_iterator met=mvamets->begin(); met<mvamets->end() ; met++){
			ntmvamet.setP4(met->p4());
			break;

		}


		if(includereco_){


			if(debugmode) std::cout <<"reco track&suclu  loop" << std::endl;

			for(std::vector<reco::Track>::const_iterator track=recotracks->begin();track<recotracks->end();track++){
				if(track->pt() < 8) continue;
				if(fabs(track->eta()) > 2.6) continue;
				for(std::vector<reco::Track>::const_iterator track2=recotracks->begin();track2<recotracks->end();track2++){
					if(track2->pt() < 8) continue;
					if(fabs(track2->eta()) > 2.6) continue;
					if(track->charge() == track2->charge()) continue;
					ztop::NTTrack nttrack;
					ztop::LorentzVector p1(track->px(), track->py(), track->pz(), track->p());
					ztop::LorentzVector p2(track2->px(), track2->py(), track2->pz(), track2->p());
					if((p1+p2).M() > 55 && (p1+p2).M() < 125){
						nttrack.setP4(p1);
						nttrack.setQ(track->charge());
						nttrack.setDzV(track->dz(vtxs[0].position()));
						nttrack.setDzVErr(track->dzError());

						nttracks.push_back(nttrack);
						break;
					}
				}
			}

			for(std::vector<reco::SuperCluster>::const_iterator suclu=recosuclus->begin(); suclu<recosuclus->end(); suclu++){

				double suclue=suclu->rawEnergy();
				math::XYZPoint suclupoint=suclu->position();
				double magnitude=sqrt(suclupoint.mag2());
				ztop::LorentzVector suclup4(suclue*suclupoint.x() / magnitude,suclue*suclupoint.y() / magnitude,suclue*suclupoint.z() / magnitude,suclue);
				if(suclup4.Pt() < 8) continue;

				for(std::vector<reco::SuperCluster>::const_iterator suclu2=recosuclus->begin(); suclu2<recosuclus->end(); suclu2++){

					if(suclu == suclu2) continue;

					double suclue2=suclu2->rawEnergy();
					math::XYZPoint suclupoint2=suclu2->position();
					double magnitude2=sqrt(suclupoint2.mag2());
					ztop::LorentzVector suclup42(suclue2*suclupoint2.x() / magnitude2,suclue2*suclupoint2.y() / magnitude2,suclue2*suclupoint2.z() / magnitude2,suclue2);

					if(suclup42.Pt() < 8) continue;

					if((suclup4+suclup42).M() > 55 && (suclup4+suclup42).M()<125 ){
						ztop::NTSuClu tempsuclu;
						tempsuclu.setP4(suclup4);
						ntsuclus.push_back(tempsuclu);
						break;
					}
				}
			}
		}

	} //end vtxs.size()>0

	/////////triggers as boolians and if switched on as strings



	std::vector<std::string> firedtriggers;
	//leave for comparison!
	if(includetrigger_){


		if(debugmode) std::cout << "include trigger loop" << std::endl;

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

	/////all trigger stuffff/////new

	// triggerBools_=checkTriggers(iEvent);


	triggerBools_.clear();
	alltriggerswithprescales_.clear();
	triggerPrescales_.clear();

	if(debugmode) std::cout << "new trigger filling loop" << std::endl;

	edm::Handle< pat::TriggerEvent > triggerEvent;
	iEvent.getByLabel(pattriggerevent_ , triggerEvent );

	const std::vector< pat::TriggerPath > * paths=triggerEvent->paths();

	if(!paths){
		std::cout << "paths pointer=0; exit" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	std::vector<unsigned int> prescales;
	std::vector<std::string> pathnames;
	std::vector<bool> fireds;

	triggerBools_.resize(triggers_.size(),false);

	for(size_t i=0;i<paths->size();i++){
		unsigned int prescale=paths->at(i).prescale();
		std::string pathname= paths->at(i).name();
		bool fired=           paths->at(i).wasAccept();
		if(fired && includetrigger_){
			alltriggerswithprescales_[pathname]=prescale;
			if(debugmode) std::cout << "prescale "<< prescale << " trigger: " << pathname << std::endl;
		}
		for(size_t j=0;j<triggers_.size();j++){
			if(pathname.find(triggers_[j]) != std::string::npos){
				triggerBools_.at(j)=fired;
				if(debugmode) std::cout << "fired trigger: " << pathname << std::endl;
			}
		}
	}


	////////end trigger stuff new



	if(debugmode) std::cout << "event info" << std::endl;

	ntevent.setRunNo(iEvent.id().run());
	ntevent.setLumiBlock(iEvent.id().luminosityBlock());
	ntevent.setEventNo(iEvent.id().event());
	ntevent.setVertexMulti(vertices->size());
	//  ntevent.setFiredTriggers(firedtriggers);

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


	if(debugmode) std::cout << "add rho iso" << std::endl;

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
	ztop::elecRhoIsoAdder addrho(!IsRealData, !rho2011_);
	if(rho2011_) addrho.setRho(temprhos[2]);
	else         addrho.setRho(temprhos[0]);
	addrho.addRhoIso(ntpfelectrons);
	addrho.addRhoIso(ntgsfelectrons);

	/////pdf weights///////
	if(includepdfweights_ && !IsRealData){


		if(debugmode) std::cout << "include pdf weights" << std::endl;
		edm::Handle<std::vector<double> > weightHandle;
		iEvent.getByLabel(pdfweights_, weightHandle);

		ntevent.setPDFWeights(*weightHandle);
	}

	//////////////// TRIGGER OBJECTS!!!! ////////

	if(includetrigger_){

		std::string triggerProcess_="HLT";

		if(debugmode) std::cout << "starting trigger object loops (HLT)" << std::endl;
		edm::Handle<trigger::TriggerEvent> triggerEvent;
		iEvent.getByLabel(edm::InputTag("hltTriggerSummaryAOD", "", triggerProcess_), triggerEvent);

		if(!triggerEvent.isValid()) {
			std::cout << "TriggerEvent not valid" << std::endl;
			return;
		}
		const trigger::TriggerObjectCollection & toc = triggerEvent->getObjects();
		unsigned int filterIndex = triggerEvent->sizeFilters();

		//start loop over collections
		for(size_t i=0;i<triggerObjects_.size();i++){
			//might throw exception if not in right path
			filterIndex = triggerEvent->filterIndex(edm::InputTag(triggerObjects_.at(i), "", triggerProcess_));
			if(debugmode) std::cout << "filter index for " << triggerObjects_.at(i) << ": " << filterIndex <<std::endl;
			if( filterIndex<triggerEvent->sizeFilters() ) { //just a safety net
				//  const trigger::Vids & vids( triggerEvent->filterIds(filterIndex) );
				const trigger::Keys & keys( triggerEvent->filterKeys(filterIndex) );

				/*
				 * module type not needed for L3 matching
				 */

				for(unsigned int k=0; k<keys.size(); ++k) {
					const trigger::TriggerObject & to = toc[keys[k]];
					NTTriggerObject tempobj;
					PolarLorentzVector vec(to.pt(),to.eta(),to.phi(),to.mass());

					tempobj.setP4(vec);
					trigObjVec.at(i).push_back(tempobj);

					if(debugmode) std::cout << "written triggerobject for id: " << triggerObjects_.at(i)  << std::endl;

				}
			}
		}
	}

	if(debugmode) std::cout << "fill ntuple" << std::endl;

	if(debugmode && !Ntuple) std::cout << "ntuple pointer broken" << std::endl;

	Ntuple->Fill();



	if(debugmode) std::cout << "event loop finished" << std::endl;

}


// ------------ method called once each job just before starting event loop  ------------
void 
TreeWriterBase::beginJob()
{
	// edm::Service<TFileService> fs;

	if( !fs ){
		throw edm::Exception( edm::errors::Configuration,
				"TFile Service is not registered in cfg file" );
	}


	if(debugmode) std::cout << "setbranches" << std::endl;

	//  char * tempname = new char[treename_.length()];
	//strcpy(tempname, treename_.c_str());
	Ntuple=(fs->make<TTree>("PFTree" ,"PFTree" ));
	//Ntuple->Branch("elePt",&elecpts);
	//TBranch *branch = Ntuple->Branch("Triggers",&triggers);


	Ntuple->Branch("NTMuons", "std::vector<ztop::NTMuon>", &ntmuons);

	if(includereco_){
		Ntuple->Branch("NTLeptons", "std::vector<ztop::NTLepton>", &ntleptons);
		Ntuple->Branch("NTTracks", "std::vector<ztop::NTTrack>", &nttracks);
		Ntuple->Branch("NTSuClu", "std::vector<ztop::NTSuClu>", &ntsuclus);
	}

	//if(includetrigger_)
	Ntuple->Branch("AllTriggersWithPrescales",   "std::map<std::string,unsigned int>", &alltriggerswithprescales_);

	Ntuple->Branch("NTPFElectrons", "std::vector<ztop::NTElectron>", &ntpfelectrons);
	Ntuple->Branch("NTElectrons", "std::vector<ztop::NTElectron>", &ntgsfelectrons);
	Ntuple->Branch("NTJets", "std::vector<ztop::NTJet>", &ntjets);

	Ntuple->Branch("NTMet", "ztop::NTMet", &ntmet);

	Ntuple->Branch("NTMvaMet", "ztop::NTMet", &ntmvamet);
	Ntuple->Branch("NTEvent", "ztop::NTEvent", &ntevent);

	Ntuple->Branch("TriggerBools",   "std::vector<bool>", &triggerBools_);
	Ntuple->Branch("TriggerPrescales",   "std::vector<unsigned int>", &triggerPrescales_);


	for(size_t i=0;i<triggerObjects_.size();i++){ //set vector beforehand (pointers change when increasing vector size)
		std::vector<ztop::NTTriggerObject> temp;
		trigObjVec.push_back(temp);
	}
	for(size_t i=0;i<triggerObjects_.size();i++){ //make own branch for each trigger object, indices coincide
		if(triggerObjects_.at(i)=="")
			continue;

		Ntuple->Branch("NTTriggerObjects_"+(TString)(triggerObjects_.at(i)),      "std::vector<ztop::NTTriggerObject>",&(trigObjVec[i]));
		if(debugmode) std::cout << "added branch for TriggerObjects: " << "NTTriggerObjects_"+(TString)triggerObjects_.at(i) << std::endl;
	}





	//gen branches

	Ntuple->Branch("NTGenTops",      "std::vector<ztop::NTGenParticle>", &nttops);
	Ntuple->Branch("NTGenWs",        "std::vector<ztop::NTGenParticle>", &ntws);
	Ntuple->Branch("NTGenZs",        "std::vector<ztop::NTGenParticle>", &ntzs);
	Ntuple->Branch("NTGenBs",        "std::vector<ztop::NTGenParticle>", &ntbs);
	Ntuple->Branch("NTGenBHadrons",  "std::vector<ztop::NTGenParticle>", &ntbhadrons);
	Ntuple->Branch("NTGenNeutrinos", "std::vector<ztop::NTGenParticle>", &ntnus);
	Ntuple->Branch("NTGenLeptons3",  "std::vector<ztop::NTGenParticle>", &ntleps3);
	Ntuple->Branch("NTGenLeptons1",  "std::vector<ztop::NTGenParticle>", &ntleps1);

	Ntuple->Branch("NTGenJets",      "std::vector<ztop::NTGenJet>",      &ntgenjets);


	Ntuple->Branch("NTGenParticles",      "std::vector<ztop::NTGenParticle>", &ntpart);
	if(susy_){
		Ntuple->Branch("StopMass", "std::vector<float>", &vstopmass);
		Ntuple->Branch("ChiMass", "std::vector<float>", &vchimass);
	}
	if(jpsi_){
		Ntuple->Branch("NTJetVtx", "std::vector<ztop::NTVertex>", &jetvtx);
		Ntuple->Branch("NTDiMuonVtx", "std::vector<ztop::NTVertex>", &dimuonvtx);
		Ntuple->Branch("NTGoodVtx", "std::vector<ztop::NTVertex>", &goodvtx);
	}

	// Ntuple->Branch("NTGenMEMuons",     "std::vector<ztop::NTGenParticle>", &ntgenmuons3);
	// Ntuple->Branch("NTGenElectrons", "std::vector<ztop::NTGenParticle>", &ntgenelecs1);
	// Ntuple->Branch("NTGenMuons",     "std::vector<ztop::NTGenParticle>", &ntgenmuons1);
	//  Ntuple->Branch("Channel",channel_);


	if(debugmode) std::cout <<"branches set" << std::endl;

}

// ------------ method called once each job just after ending the event loop  ------------
void 
TreeWriterBase::endJob() 
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
TreeWriterBase::beginRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a run  ------------
void 
TreeWriterBase::endRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block  ------------
void 
TreeWriterBase::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void 
TreeWriterBase::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
TreeWriterBase::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
	//The following says we do not know what parameters are allowed so do no validation
	// Please change this to state exactly what you do use, even if it is no parameters
	edm::ParameterSetDescription desc;
	desc.setUnknown();
	descriptions.addDefault(desc);
}
bool TreeWriterBase::checkJetID(const pat::Jet & jet)
{
	bool hasjetID=false;
	if(jet.numberOfDaughters() > 1 &&
			(jet.neutralHadronEnergyFraction() + jet.HFHadronEnergy() / jet.energy() ) < 0.99 &&
			jet.neutralEmEnergyFraction() < 0.99){
		if(fabs(jet.eta())<2.4){
			if( jet.chargedMultiplicity() > 0 &&
					jet.chargedHadronEnergyFraction() > 0&&
					jet.chargedEmEnergyFraction() < 0.99)
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
std::vector<bool> TreeWriterBase::checkTriggers(const edm::Event& iEvent){
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




void TreeWriterBase::setTriggers(){

	triggers_.push_back("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v");
	triggers_.push_back("HLT_Mu17_Mu8_v");
	triggers_.push_back("HLT_Mu17_TkMu8_v");
	//
	triggers_.push_back("HLT_Ele17_SW_TightCaloEleId_Ele8_HE_L1R_v");              //3
	triggers_.push_back("HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v");   //4
	triggers_.push_back("HLT_DoubleMu7");
	triggers_.push_back("HLT_Mu13_Mu8_v");

	//single lep for efficiencies
	triggers_.push_back("HLT_IsoMu24_v");      //7
	triggers_.push_back("HLT_IsoMu24_eta2p1_v");
	triggers_.push_back("HLT_Ele27_WP80_v");

	//emu triggers

	triggers_.push_back("HLT_Mu17_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v"); //10
	triggers_.push_back("HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v"); //11
	/*

  triggers_.push_back();
  triggers_.push_back();
  triggers_.push_back();
	 */

}


ztop::NTGenParticle TreeWriterBase::makeNTGen(const reco::GenParticle * p, const std::map<const reco::GenParticle * , int> & idmap){
	ztop::NTGenParticle out;
	out.setP4(p->p4());
	out.setPdgId(p->pdgId());
	out.setStatus(p->status());
	out.setQ(p->charge());
	out.setGenId(idmap.at(p));
	return out;
}

ztop::NTGenParticle TreeWriterBase::makeNTGen(const reco::GenParticle * p){
	ztop::NTGenParticle out;
	out.setP4(p->p4());
	out.setPdgId(p->pdgId());
	out.setStatus(p->status());
	// out.setGenId(idmap.at(p));
	return out;
}

ztop::NTGenJet TreeWriterBase::makeNTGenJet(const reco::GenJet * p){
	ztop::NTGenJet out;
	out.setP4(p->p4());
	return out;
}

// searchdaughter(particle *, vectorparticle * daughters)
// searchmother(particle * , vector * possiblemothers)

bool  TreeWriterBase::isInGenCollection(const reco::GenParticle * p, const std::vector<const reco::GenParticle * > & coll){
	for(size_t i=0;i<coll.size();i++){
		if(coll.at(i) == p)
			return true;
	}
	return false;
}
bool  TreeWriterBase::isInGenCollection(const reco::GenJet * p, const std::vector<const reco::GenJet * > & coll){
	for(size_t i=0;i<coll.size();i++){
		if(coll.at(i) == p)
			return true;
	}
	return false;
}


template<class t, class u>
int TreeWriterBase::findGenMatchIdx(t * patinput , std::vector<u> & gen, double dR){

	double Dptmax=0.5;

	if(!patinput)
		return -1;

	double drmin2=100;
	int idx=-1;
	for(size_t i=0;i<gen.size();i++){
		double DRs=(patinput->eta() - gen.at(i).eta())*(patinput->eta() - gen.at(i).eta()) + (patinput->phi() - gen.at(i).phi())*(patinput->phi() - gen.at(i).phi());
		if(drmin2 > DRs){
			drmin2 = DRs;
			idx=i;
		}
	}
	if(drmin2 < dR*dR && Dptmax > fabs(patinput->pt() - gen.at(idx).pt())/patinput->pt())
		return idx;
	else
		return -1;
}

