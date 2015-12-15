#include "../interface/TreeWriterBase.h"

#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"

#include <algorithm>
TreeWriterBase::TreeWriterBase(const edm::ParameterSet& iConfig)
{

	debugmode                  =iConfig.getParameter<bool>                      ("debugmode");

 	//now do what ever initialization is needed
	runonaod_                  =iConfig.getParameter<bool>                      ("runOnAOD");                
	
	treename_                  =iConfig.getParameter<std::string>               ("treeName");
	muons_                     =iConfig.getParameter<edm::InputTag>             ("muonSrc");
	gsfelecs_                  =iConfig.getParameter<edm::InputTag>             ("elecGSFSrc");
	pfelecs_                   =iConfig.getParameter<edm::InputTag>             ("elecPFSrc");
	jets_                      =iConfig.getParameter<edm::InputTag>             ("jetSrc");
	btagalgo_                  =iConfig.getParameter<std::string>               ("btagAlgo");
	met_                       =iConfig.getParameter<edm::InputTag>             ("metSrc");
	mvamet_                    =iConfig.getParameter<edm::InputTag>             ("mvaMetSrc");
	t1met_                     =iConfig.getParameter<edm::InputTag>             ("metT1Src");
	t0t1txymet_                =iConfig.getParameter<edm::InputTag>             ("metT0T1TxySrc");
	t0t1met_                   =iConfig.getParameter<edm::InputTag>             ("metT0T1Src");
	t1txymet_                  =iConfig.getParameter<edm::InputTag>             ("metT1TxySrc");

	vertices_                  =iConfig.getParameter<edm::InputTag>             ("vertexSrc");

	includereco_               =iConfig.getParameter<bool>                      ("includeReco");
	recomuons_                 =iConfig.getParameter<edm::InputTag>             ("recoMuonSrc");
	pfMuonCands_               =iConfig.getParameter<bool>                      ("isPFMuonCand");
	recoelecs_                 =iConfig.getParameter<edm::InputTag>             ("recoElecSrc");
	pfElecCands_               =iConfig.getParameter<bool>                      ("isPFElecCand");
	recotracks_                =iConfig.getParameter<edm::InputTag>             ("recoTrackSrc");
	recosuclus_                =iConfig.getParameter<edm::InputTag>             ("recoSuCluSrc");


	includetrigger_            =iConfig.getParameter<bool>                      ("includeTrigger");
	trigresults_               =iConfig.getParameter<edm::InputTag>             ("triggerResults");
	pattriggerevent_           =iConfig.getParameter<edm::InputTag>             ("triggerEvent");

	puinfo_                    =iConfig.getParameter<edm::InputTag>             ("PUInfo");

	rhoiso_                    =iConfig.getParameter<edm::InputTag>             ("rhoIso");


	includepdfweights_         =iConfig.getParameter<bool>                      ("includePDFWeights");
	pdfweights_                =iConfig.getParameter<edm::InputTag>             ("pdfWeights"          );


	includegen_                =iConfig.getParameter<bool>                      ("includeGen");
	genparticles_              =iConfig.getParameter<edm::InputTag>             ("genParticles");
	genjets_                   =iConfig.getParameter<edm::InputTag>             ("genJets");




	//Input from GenLevelBJetProducer
	bHadJetIdx_                =iConfig.getParameter<edm::InputTag>             ("BHadJetIndex");
	antibHadJetIdx_            =iConfig.getParameter<edm::InputTag>             ("AntiBHadJetIndex");
	BHadrons_                  =iConfig.getParameter<edm::InputTag>             ("BHadrons");
	AntiBHadrons_              =iConfig.getParameter<edm::InputTag>             ("AntiBHadrons");
	BHadronFromTopB_           =iConfig.getParameter<edm::InputTag>             ("BHadronFromTopB");
	AntiBHadronFromTopB_       =iConfig.getParameter<edm::InputTag>             ("AntiBHadronFromTopB");
	BHadronVsJet_              =iConfig.getParameter<edm::InputTag>             ("BHadronVsJet");
	AntiBHadronVsJet_          =iConfig.getParameter<edm::InputTag>             ("AntiBHadronVsJet");

	genBHadPlusMothers_        =iConfig.getParameter<edm::InputTag>             ("genBHadPlusMothers");
	genBHadPlusMothersIndices_ =iConfig.getParameter<edm::InputTag>             ("genBHadPlusMothersIndices");
	genBHadIndex_              =iConfig.getParameter<edm::InputTag>             ("genBHadIndex");
	genBHadFlavour_            =iConfig.getParameter<edm::InputTag>             ("genBHadFlavour");
	genBHadJetIndex_           =iConfig.getParameter<edm::InputTag>             ("genBHadJetIndex");
	useBHadrons_               =iConfig.getParameter<bool>                      ("useBHadrons");

	triggerObjects_            =iConfig.getParameter<std::vector<std::string> > ("triggerObjects");

	keepelecidOnly_            =iConfig.getParameter<std::string>               ("keepElecIdOnly");
        
        partonShower_              =iConfig.getParameter<std::string>               ("partonShower");

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

	std::vector<std::string> tmpweightnames = iConfig.getParameter<std::vector<std::string> > ("additionalWeights");
	for(size_t i=0;i<tmpweightnames.size();i++){
		if(tmpweightnames.at(i).size() < 1) continue;
		weightnames_.push_back(tmpweightnames.at(i));
	}



	weights_.resize(weightnames_.size());
	if(weightnames_.size()>0){
		std::cout << "writing additional weights" << std::endl;
		for(size_t i=0;i<weightnames_.size();i++){
			std::cout << weightnames_.at(i) << " ";
		}
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

	if(debugmode) std::cout << "event loop started" << std::endl;

	std::string addForPF;
	if(pfinput_) addForPF="bla";

	ztop::NTLorentzVector<float> p4zero(0,0,0,0);

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
	ntradbs.clear();
	ntbhadrons.clear();
	ntnus.clear();
	ntleps3.clear();
	ntleps1.clear();
	ntallnus.clear();

	ntgenjets.clear();
	genmet_f=0;
	vstopmass.clear();
	vchimass.clear();

	triggerBools_.clear();
	alltriggerswithprescales_.clear();
	triggerPrescales_.clear();
	firedtriggers_.clear();
	
	for(size_t i=0;i<trigObjVec.size();i++)
		trigObjVec.at(i).clear();

	for(size_t i=0;i<weights_.size();i++)
		weights_.at(i) = NTWeight(1);

	ztop::NTTrigger clear;
	nttrigger=clear;

	runno_=0;
	lumiblock_=0;
	eventno_=0;

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
                std::vector<const reco::GenParticle *> wmothers, leps3mothers,bmothers,numothers,leps1mothers;

		// nttops,ntws,ntbs,ntbhadrons,ntnus,ntleps3,ntleps1, ntallnus;


                /*Please note that the Mother Daughter relations heavily depend on the generator!!
                * With the implementation below the Mother Daughter relations were only tested with PYTHIA 6.
                * Any Other Generator will likely require additional implementation
                */
		if(debugmode) std::cout << "filling all gen Particles" << std::endl;

		bool gotTop=false;

		int genidit=0;


		for(size_t i=0;i<genParticles->size();i++){
			allgen << &genParticles->at(i);
		}
                std::vector<const reco::GenParticle *>* allgenForDec(&allgen);
                ztop::topDecaySelector * topDecay = new topDecaySelector();
                topDecay->setGenCollection(allgenForDec);
                ztop::topDecaySelector::partonShowers ps;
                if(partonShower_ == "pythia6")  ps = ztop::topDecaySelector::ps_pythia6;
                else if (partonShower_ == "pythia8") ps = ztop::topDecaySelector::ps_pythia8;
                else if (partonShower_ == "herwig")  ps = ztop::topDecaySelector::ps_herwig;
                else if (partonShower_ == "herwigpp") ps= ztop::topDecaySelector::ps_herwigpp;
                else  throw std::logic_error("TreeWriterBase::analyze: Wrong partonShower set in Config.");      
                topDecay->setPartonShower(ps);
		if(debugmode) std::cout << "Filling tops" << std::endl;
                topDecay->process();
                tops = topDecay->getMETops();
		for(size_t i=0;i<tops.size();i++){
			ztop::NTGenParticle temp=makeNTGen(tops.at(i));
			temp.setGenId(genidit++);
			nttops << temp;
			gotTop=true;
		}
                bool gotWFromTop=false;
                Ws = topDecay->getMEWs();
                wmothers = topDecay->getMEWsMothers();
                for(size_t i=0;i<Ws.size();i++){
                        ztop::NTGenParticle temp=makeNTGen(Ws.at(i));
                        temp.setGenId(genidit++);
                        for(size_t k=0;k<tops.size();k++){
                                if(wmothers.at(i) == tops.at(k)){
                                        temp.addMotherIt(nttops.at(k).genId());
                                        nttops.at(k).addDaughterIt(temp.genId());
                                        break;
                                }
                        }
                        ntws << temp;
                        gotWFromTop=true;
                }
                leps3 = topDecay->getMELeptons();
                leps3mothers= topDecay->getMELeptonsMothers();
                for(size_t i=0;i<leps3.size();i++){
                        ztop::NTGenParticle temp=makeNTGen(leps3.at(i));
                        temp.setGenId(genidit++);
                        for(size_t k=0;k<Ws.size();k++){
                                if (leps3mothers.at(i)==Ws.at(k)){
                                        temp.addMotherIt(ntws.at(k).genId());
                                        ntws.at(k).addDaughterIt(temp.genId());
                                        break;
                                }
                        }
                        ntleps3 << temp;
                }
                nus=topDecay->getMENeutrinos();
                numothers=topDecay->getMENeutrinosMothers();
                for(size_t i=0;i<nus.size();i++){
                        ztop::NTGenParticle temp=makeNTGen(nus.at(i));
                        temp.setGenId(genidit++);
                        for(size_t k=0;k<Ws.size();k++){
                                if (numothers.at(i)==Ws.at(k)){
                                        temp.addMotherIt(ntws.at(k).genId());
                                        ntws.at(k).addDaughterIt(temp.genId());
                                        break;
                                }
                        }
                        ntnus << temp;
                }
                leps1=topDecay->getFinalStateLeptonsFromW();
                leps1mothers=topDecay->getFinalStateLeptonsFromWMothers();
                for(size_t i=0;i<leps1.size();i++){
                        ztop::NTGenParticle temp=makeNTGen(leps1.at(i));
                        temp.setGenId(genidit++);
                        for(size_t k=0;k<leps3.size();k++){
                                if (leps1mothers.at(i)==leps3.at(k)){
                                        temp.addMotherIt(ntleps3.at(k).genId());
                                        ntleps3.at(k).addDaughterIt(temp.genId());
                                        break;
                                }
                        }
                        ntleps1 << temp;
                }
                allnus=topDecay->getFinalStateNeutrinos();
                for(size_t i=0;i<allnus.size();i++){
                        ztop::NTGenParticle temp=makeNTGen(allnus.at(i));
                        temp.setGenId(genidit++);
                        //temp.addMotherIt(ntws.at(i).genId());
                        //ntws.at(i).addDaughterIt(temp.genId());
                        ntallnus << temp;
                        genmet_f+=allnus.at(i)->pt();
                }
                //FIXME:NTBRads is no longer implemented, needs to be added to topDecayselector 
                bs=topDecay->getMEBs();
                bmothers = topDecay->getMEBsMothers();
                for(size_t i=0;i<bs.size();i++){
                        ztop::NTGenParticle temp=makeNTGen(bs.at(i));
                        temp.setGenId(genidit++);
                        for(size_t k=0;k<tops.size();k++){
                                if (bmothers.at(i)==tops.at(k)){
                                        temp.addMotherIt(nttops.at(k).genId());
                                        nttops.at(k).addDaughterIt(temp.genId());
                                        break;
                                }
                        }
                        ntbs << temp;
                }                                                                        
                //FIXME:This only works for PYTHIA 6  in the moment !!!
                //FIXME:Especially not implemented for PYTHIA 8                       

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
                        //This only works for PYTHIA 6  in the moment !!!
                        //Especially not implemented for PYTHIA 8
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
		if(debugmode) std::cout << "filling genMet" << std::endl;


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

		if(debugmode) std::cout <<"includegen left" << std::endl;
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


	///met handles
	//t1met_,t0t1txymet_,t0t1ymet_,t1txymet_;
	Handle<std::vector<pat::MET> > mets,mvamets,t1mets,t0t1mets,t0t1txymets,t1txymets;
	iEvent.getByLabel(met_,mets);
	iEvent.getByLabel(mvamet_,mvamets);
	iEvent.getByLabel(t1met_,t1mets);
	iEvent.getByLabel(t0t1txymet_,t0t1txymets);
	iEvent.getByLabel(t0t1met_,t0t1mets);
	iEvent.getByLabel(t1txymet_,t1txymets);


	iEvent.getByLabel(vertices_, vertices);
	vtxs  = *(vertices.product());

	if(vtxs.size()>0){
		if(debugmode) std::cout << "vtxs.size()>0" << std::endl;
		
		///////////////////////////////////////////////E L E C T R O N S//////////
		//recent changes: stares two collections (gsf and pf), ecaldrivenmomentum in BOTH BE CAREFUL WHEN CHANGING

		math::XYZPoint beamSpotPosition;
		beamSpotPosition.SetCoordinates(0,0,0);

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

		for(size_t i=0;i<muons->size();i++){
			ztop::NTMuon tempmuon;
			tempmuon=makeNTMuon(muons->at(i));

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
					float vz=-9999;
					float vzerr=-9999;
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
					float vz=-9999;
					float vzerr=-9999;
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
					float vz=-9999;
					float vzerr=-9999;
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
					float vz=-9999;
					float vzerr=-9999;
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

			if(uncJet.pt() < 10) continue;

			ztop::NTJet tempjet;
			tempjet=makeNTJet(jets->at(i));

			int genidx=findGenMatchIdx((jets->at(i).genJet()),ntgenjets,0.1);
			tempjet.setGenMatch(genidx);

			ntjets.push_back(tempjet);
		}


		if(debugmode) std::cout << "met loops" << std::endl;

		for(std::vector<pat::MET>::const_iterator met=mets->begin(); met<mets->end() ; met++){
			ntmet.setP4(met->p4());
			break;
		}

		if(debugmode) std::cout <<"mvamets" << std::endl;
		for(std::vector<pat::MET>::const_iterator met=mvamets->begin(); met<mvamets->end() ; met++){
			ntmvamet.setP4(met->p4());
			break;
		}
		if(debugmode) std::cout <<"t1mets" << std::endl;
		for(std::vector<pat::MET>::const_iterator met=t1mets->begin(); met<t1mets->end() ; met++){
			ntt1met.setP4(met->p4());
			break;
		}
		if(debugmode) std::cout <<"t0t1txymets" << std::endl;
		for(std::vector<pat::MET>::const_iterator met=t0t1txymets->begin(); met<t0t1txymets->end() ; met++){
			ntt0t1txymet.setP4(met->p4());
			break;
		}
		if(debugmode) std::cout <<"t0t1mets" << std::endl;
		for(std::vector<pat::MET>::const_iterator met=t0t1mets->begin(); met<t0t1mets->end() ; met++){
			ntt0t1met.setP4(met->p4());
			break;
		}
		if(debugmode) std::cout <<"t1txymets" << std::endl;
		for(std::vector<pat::MET>::const_iterator met=t1txymets->begin(); met<t1txymets->end() ; met++){
			ntt1txymet.setP4(met->p4());
			break;
		}
		if(debugmode) std::cout <<"met loops end" << std::endl;

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
					ztop::NTLorentzVector<float> p1;
					p1.setPxPyPzE(track->px(), track->py(), track->pz(), track->p());
					ztop::NTLorentzVector<float> p2;
					p2.setPxPyPzE(track2->px(), track2->py(), track2->pz(), track2->p());
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

				float suclue=suclu->rawEnergy();
				math::XYZPoint suclupoint=suclu->position();
				float magnitude=sqrt(suclupoint.mag2());
				NTLorentzVector<float>  suclup4;
				suclup4.setPxPyPzE(suclue*suclupoint.x() / magnitude,suclue*suclupoint.y() / magnitude,suclue*suclupoint.z() / magnitude,suclue);
				if(suclup4.Pt() < 8) continue;

				for(std::vector<reco::SuperCluster>::const_iterator suclu2=recosuclus->begin(); suclu2<recosuclus->end(); suclu2++){

					if(suclu == suclu2) continue;

					float suclue2=suclu2->rawEnergy();
					math::XYZPoint suclupoint2=suclu2->position();
					float magnitude2=sqrt(suclupoint2.mag2());
					NTLorentzVector<float>  suclup42;
					suclup42.setPxPyPzE(suclue2*suclupoint2.x() / magnitude2,suclue2*suclupoint2.y() / magnitude2,suclue2*suclupoint2.z() / magnitude2,suclue2);

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
	
	/////process trigger info/////
	triggerBools_.clear();
	triggerBools_.resize(triggers_.size(),false);	
	alltriggerswithprescales_.clear();
	triggerPrescales_.clear();
	firedtriggers_.clear();
	
	if(debugmode) std::cout << "new trigger filling loop" << std::endl;
	
	// run different trigger routines for AODs and MiniAODs 
	if(runonaod_){
	    runTriggerAOD(iEvent);
	}else{
	    runTriggerMiniAOD(iEvent);
	}
	////////end process trigger info
	
	
	if(debugmode) std::cout << "event info" << std::endl;

	ntevent.setRunNo(iEvent.id().run());
	ntevent.setLumiBlock(iEvent.id().luminosityBlock());
	ntevent.setEventNo(iEvent.id().event());
	ntevent.setVertexMulti(vertices->size());
	//  ntevent.setFiredTriggers(firedtriggers_);

	runno_=iEvent.id().run();
	lumiblock_=iEvent.id().luminosityBlock();
	eventno_=iEvent.id().event();
        #ifndef CMSSW_LEQ_5 
        edm::Handle<bool>skim;
        iEvent.getByLabel("filterkinLeptons",skim);
        skim_= *skim;
        //catch (...) {skim_=1;}
        #endif
	/*ZTOP_COUTVAR(iEvent.id().run());
	ZTOP_COUTVAR(runno_);
	ZTOP_COUTVAR(iEvent.id().luminosityBlock());
	ZTOP_COUTVAR(lumiblock_);
	ZTOP_COUTVAR(iEvent.id().event());
	ZTOP_COUTVAR(eventno_);*/

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
	
	std::vector<float> temprhos;
	//agrohsje rho not available in mini aods, disable in aod corrections for 13 TeV (fixme!!!) 
	    //rhoiso,,rhojetsiso,,rhojetsisonopu
	edm::Handle<double> rho;
	    
	if(debugmode) std::cout << "add rho iso" << std::endl;
	    
	iEvent.getByLabel(rhoiso_,rho);
	temprhos.push_back(*rho);
	ntevent.setIsoRho(temprhos);
	if(runonaod_){    
	    //add rhoiso to electrons (uses 2011 corrections (second argument set to false));
	    ztop::elecRhoIsoAdder addrho(!IsRealData, true);
	    addrho.setRho(temprhos[0]);
	    addrho.addRhoIso(ntpfelectrons);
	    addrho.addRhoIso(ntgsfelectrons);
	}
	/////pdf weights///////
	if(includepdfweights_ && !IsRealData){


		if(debugmode) std::cout << "include pdf weights" << std::endl;
		edm::Handle<std::vector<double> > weightHandle;
		iEvent.getByLabel(pdfweights_, weightHandle);
		std::vector<float> pdfweights;
		for(size_t i=0;i<weightHandle->size();i++)
			pdfweights.push_back(weightHandle->at(i));

		ntevent.setPDFWeights(pdfweights);

	}

	if(weightnames_.size()>0){
		if(debugmode) std::cout << "processing additional weights " << std::endl;
		for(size_t i=0;i<weightnames_.size();i++){
			edm::Handle<double> weight;
			iEvent.getByLabel(edm::InputTag(weightnames_.at(i)), weight);
			weights_.at(i).setWeight(*weight);
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

	Ntuple->Branch("NTMvaMet", "ztop::NTMet", &ntt1met);
	Ntuple->Branch("NTT0T1TxyMet", "ztop::NTMet", &ntt0t1txymet);
	Ntuple->Branch("NTT0T1Met", "ztop::NTMet", &ntt0t1met);
	Ntuple->Branch("NTT1TxyMet", "ztop::NTMet", &ntt1txymet);

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

	for(size_t i=0;i<weightnames_.size();i++){
		Ntuple->Branch("NTWeight_"+(TString)weightnames_.at(i), "ztop::NTWeight", &weights_.at(i));
	}



	//gen branches

	Ntuple->Branch("NTGenTops",      "std::vector<ztop::NTGenParticle>", &nttops);
	Ntuple->Branch("NTGenWs",        "std::vector<ztop::NTGenParticle>", &ntws);
	Ntuple->Branch("NTGenZs",        "std::vector<ztop::NTGenParticle>", &ntzs);
	Ntuple->Branch("NTGenBs",        "std::vector<ztop::NTGenParticle>", &ntbs);
	Ntuple->Branch("NTGenBsRad",        "std::vector<ztop::NTGenParticle>", &ntradbs);
	Ntuple->Branch("NTGenBHadrons",  "std::vector<ztop::NTGenParticle>", &ntbhadrons);
	Ntuple->Branch("NTGenNeutrinos", "std::vector<ztop::NTGenParticle>", &ntnus);
	Ntuple->Branch("NTGenLeptons3",  "std::vector<ztop::NTGenParticle>", &ntleps3);
	Ntuple->Branch("NTGenLeptons1",  "std::vector<ztop::NTGenParticle>", &ntleps1);

	Ntuple->Branch("NTGenJets",      "std::vector<ztop::NTGenJet>",      &ntgenjets);

	Ntuple->Branch("genMet",           &genmet_f);

	Ntuple->Branch("NTGenParticles",      "std::vector<ztop::NTGenParticle>", &ntpart);
	
	// Ntuple->Branch("NTGenMEMuons",     "std::vector<ztop::NTGenParticle>", &ntgenmuons3);
	// Ntuple->Branch("NTGenElectrons", "std::vector<ztop::NTGenParticle>", &ntgenelecs1);
	// Ntuple->Branch("NTGenMuons",     "std::vector<ztop::NTGenParticle>", &ntgenmuons1);
	//  Ntuple->Branch("Channel",channel_);
	
	//simpel branches
	/*runno_,lumiblock_,eventno_*/

	Ntuple->Branch("RunNumber",      &runno_);
	Ntuple->Branch("LumiBlock",      &lumiblock_);
	Ntuple->Branch("EventNumber",   &eventno_);
        
        #ifndef CMSSW_LEQ_5
        Ntuple->Branch("Skim", &skim_);
        #endif

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


	// more 2011 triggers - some might be repeated! leave it as it is!
	// according to http://cms.cern.ch/iCMS/jsp/db_notes/noteInfo.jsp?cmsnoteid=CMS%20AN-2012/065

	//mumu

	triggers_.push_back("HLT_DoubleMu7_"); //12
	triggers_.push_back("HLT_Mu13_Mu8_v"); //13
	triggers_.push_back("HLT_Mu17_Mu8_v"); //14
	triggers_.push_back("HLT_DoubleMu6_"); //15
	triggers_.push_back("HLT_DoubleMu45_"); //16

	//ee

	triggers_.push_back("HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v"); //17
	triggers_.push_back("HLT_Ele17_CaloIdT_TrkIdVL_CaloIsoVL_TrkIsoVL_Ele8_CaloIdT_TrkIdVL_CaloIsoVL_TrkIsoVL_v"); //18
	triggers_.push_back("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v"); //19
	triggers_.push_back("HLT_DoubleEle45_CaloIdL_v"); //20

	//emu

	triggers_.push_back("HLT_Mu10_Ele10_CaloIdL_"); //21
	triggers_.push_back("HLT_Mu8_Ele17_CaloIdL_"); //22
	triggers_.push_back("HLT_Mu17_Ele8_CaloIdL_"); //23
	triggers_.push_back("HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_v"); //24
	triggers_.push_back("HLT_Mu17_Ele8_CaloIdT_CaloIsoVL_v"); //25
	triggers_.push_back("HLT_Mu10_Ele10_CaloIdL_v"); //26

	/// 7TeV single lep go here!!! only append - DONT change order (kills compat inbetween ntuples!)


	// agrohsje added triggers for upcoming 13 TeV run 
	//mumu
	triggers_.push_back("HLT_Mu17_Mu8_DZ_v"); //27
	triggers_.push_back("HLT_Mu17_TkMu8_DZ_v"); //28
	triggers_.push_back("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v"); //29
	triggers_.push_back("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v"); //30
        triggers_.push_back("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v");//31
        triggers_.push_back("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v");//32
	triggers_.push_back("HLT_DoubleMu18NoFiltersNoVtx_v"); //33
	//ee 
	triggers_.push_back("HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v"); //34
        triggers_.push_back("HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_v"); //35
        triggers_.push_back("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v"); //36
        triggers_.push_back("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v"); //37
	//emu
	triggers_.push_back("HLT_Mu17_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v"); //38
	triggers_.push_back("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v"); //39
        triggers_.push_back("HLT_Mu8_TrkIsoVVL_Ele17_CaloIdL_TrackIdL_IsoVL_v"); //40
        triggers_.push_back("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v"); //41

        //Single Mu 
        triggers_.push_back("HLT_IsoMu20_v"); //42
        triggers_.push_back("HLT_IsoTkMu20_v"); //43

        //Single Ele
        triggers_.push_back("HLT_Ele23_WPLoose_Gsf_v");//44
        triggers_.push_back("HLT_Ele22_eta2p1_WP75_Gsf_v");//45

        //MET Triggers
        triggers_.push_back("HLT_CaloMHTNoPU90_PFMET90_PFMHT90_IDTight_BTagCSV0p72_v");//46
        triggers_.push_back("HLT_CaloMHTNoPU90_PFMET90_PFMHT90_IDLoose_BTagCSV0p7_v");//47
        triggers_.push_back("HLT_PFMET120_PFMHT120_IDLoose_v");//48
        triggers_.push_back("HLT_PFMET90_PFMHT90_IDLoose_v");//49
        triggers_.push_back("HLT_PFMETNoMu90_JetIdCleaned_PFMHTNoMu90_IDTight_v");//50
        triggers_.push_back("HLT_PFMETNoMu90_NoiseCleaned_PFMHTNoMu90_IDTight_v");//51
        triggers_.push_back("HLT_MonoCentralPFJet80_PFMETNoMu90_JetIdCleaned_PFMHTNoMu90_IDTight_v");//52
        triggers_.push_back("HLT_MonoCentralPFJet80_PFMETNoMu120_JetIdCleaned_PFMHTNoMu120_IDTight_v");//53
        triggers_.push_back("HLT_MonoCentralPFJet80_PFMETNoMu90_NoiseCleaned_PFMHTNoMu90_IDTight_v");//54
        triggers_.push_back("HLT_PFMET90_PFMHT90_IDTight_v");//55
        triggers_.push_back("HLT_MET200_JetIdCleaned_v");//56
        triggers_.push_back("HLT_CaloMET200_NoiseCleaned_v");//57
                

	//triggers_.push_back("");

}

void TreeWriterBase::runTriggerAOD(const edm::Event& iEvent){

    edm::Handle< pat::TriggerEvent > triggerEvent;
    iEvent.getByLabel(pattriggerevent_ , triggerEvent );
    const std::vector< pat::TriggerPath > * paths=triggerEvent->paths();
    if(!paths){
	std::cout << "paths pointer=0; exit" << std::endl;
	std::exit(EXIT_FAILURE);
    }
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
		
		for(unsigned int k=0; k<keys.size(); ++k) {
		    const trigger::TriggerObject & to = toc[keys[k]];
		    ztop::NTTriggerObject tempobj;
		    ztop::NTLorentzVector<float> vec(to.pt(),to.eta(),to.phi(),to.mass());
		    
		    tempobj.setP4(vec);
		    trigObjVec.at(i).push_back(tempobj);
		    
		    if(debugmode) std::cout << "written triggerobject for id: " << triggerObjects_.at(i)  << std::endl;
		}
	    }
	}
	//firedtriggers: leave for comparison!
	if(debugmode) std::cout << "include trigger loop" << std::endl;
	
	edm::Handle<edm::TriggerResults> trigresults;
	iEvent.getByLabel(trigresults_, trigresults);
	
	if(!trigresults.failedToGet()){
	    int n_Triggers = trigresults->size();
	    edm::TriggerNames trigName = iEvent.triggerNames(*trigresults);
	    
	    for(int i_Trig = 0; i_Trig<n_Triggers; ++i_Trig){
		if(trigresults.product()->accept(i_Trig)){
		    firedtriggers_.push_back(trigName.triggerName(i_Trig));
		}
	    }
	}
    }	
}

void TreeWriterBase::runTriggerMiniAOD(const edm::Event& iEvent){
    #ifndef CMSSW_LEQ_5
    edm::Handle<edm::TriggerResults> trigresults;
    iEvent.getByLabel(trigresults_, trigresults);
    edm::Handle<pat::TriggerObjectStandAloneCollection> triggerObjects;
    iEvent.getByLabel("selectedPatTrigger", triggerObjects);
    edm::Handle<pat::PackedTriggerPrescales> triggerPrescales;
    iEvent.getByLabel("patTrigger", triggerPrescales);
    const edm::TriggerNames &names = iEvent.triggerNames(*trigresults);
    for (unsigned int i = 0, n = trigresults->size(); i < n; ++i) {
	unsigned int prescale=triggerPrescales->getPrescaleForIndex(i); 
	std::string pathname=names.triggerName(i);
	bool fired=trigresults->accept(i);
        if(fired && includetrigger_) {
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
    #endif
    //Needs to be fixed to check for the Triggers named in the Trigger Object string array. Otherwise it will crash
   /*if(includetrigger_){
        //Checks for All Triggerobjects, no further check, should propably be implemented
        for(pat::TriggerObjectStandAlone obj : *triggerObjects){
            obj.unpackPathNames(names);
            std::vector<std::string> pathNamesAll  = obj.pathNames(false);
            ztop::NTTriggerObject tempobj;
            ztop::NTLorentzVector<float> vec(obj.pt(),obj.eta(),obj.phi(),obj.mass());
            tempobj.setP4(vec);
            for (unsigned h = 0, n = pathNamesAll.size(); h < n; ++h){
                trigObjVec.at(h).push_back(tempobj);
                if(debugmode) std::cout << "written triggerobject for id: " << triggerObjects_.at(h)  << std::endl;
                }
            }            
    }*/
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
	out.setQ(p->charge());
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
