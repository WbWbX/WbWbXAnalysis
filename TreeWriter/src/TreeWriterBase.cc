#include "../interface/TreeWriterBase.h"



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
   using namespace top;


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


     if(debugmode) std::cout << "includegen enetered" << std::endl;

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
	    tempntgen.setDaughter(smallIdMap.find(daughter)->second);
	    gotdaugh=true;
	  }
	}
	if(!gotdaugh && mothdaugh.find(mother) != mothdaugh.end()){
	  tempntgen.setDaughter(smallIdMap.find(mothdaugh.at(mother))->second);
	}
	for(size_t  i=0;i<mother->numberOfMothers();i++){
	  const reco::GenParticle* mothmoth = dynamic_cast<const reco::GenParticle*>(mother->mother(i));
	  if(isInGenCollection(mothmoth, mergedgen)){
	    tempntgen.setMother(smallIdMap.find(mothmoth)->second);
	    gotmoth=true;
	  }
	}
	if(!gotmoth && daughmoth.find(mother) != daughmoth.end()){
	  tempntgen.setMother(smallIdMap.find(daughmoth.at(mother))->second);
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
       ntgenjets.push_back( tempgenjet);
     }




   }//isMC and includegen end


   if(debugmode) std::cout <<"includegen left" << std::endl;

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

   Handle<std::vector<pat::MET> > mvamets;
   iEvent.getByLabel(mvamet_,mvamets);

   Handle<std::vector<reco::Vertex> > vertices;
   iEvent.getByLabel(vertices_, vertices);
   vtxs  = *(vertices.product());


   
   if(vtxs.size()>0){


   if(debugmode) std::cout << "vtxs.size()>0" << std::endl;

     ///////////////////////////////////////////////E L E C T R O N S//////////
     //recent changes: stares two collections (gsf and pf), ecaldrivenmomentum in BOTH BE CAREFUL WHEN CHANGING


     math::XYZPoint beamSpotPosition;
     beamSpotPosition.SetCoordinates(0,0,0);

     edm::Handle<reco::BeamSpot> bsHandle;
     iEvent.getByLabel("offlineBeamSpot",bsHandle);

     double BSx=0,BSy=0,BSz=0;
     if( (bsHandle.isValid()) ){
       reco::BeamSpot bs = *bsHandle;
       BSx = bs.x0();
       BSy = bs.y0();
       BSz = bs.z0();
       beamSpotPosition = bsHandle->position();
     }



   if(debugmode) std::cout << "electron loops" << std::endl;

 
     ////Electrons/////


     edm::Handle<std::vector<pat::Electron> >  temppatelecs=gsfelecs;

     for(size_t vecs=0;vecs<2;vecs++){

       for(size_t i=0;i<temppatelecs->size();i++){
     
	 top::NTElectron tempelec;
	 tempelec=makeNTElectron(temppatelecs->at(i));

	 int genidx=-1;

	 if(temppatelecs->at(i).genParticle()){ // pat genmatch exists;

	   if(smallIdMap.find(temppatelecs->at(i).genParticle()) != smallIdMap.end())
	     genidx=smallIdMap.find(temppatelecs->at(i).genParticle())->second;                // use pat match
	   else
	     genidx=findGenMatchIdx(temppatelecs->at(i).genParticle(),allntgen,0.1); //match pat match to closest from ntgen collection

	   if(genidx>=0){
	     tempelec.setGenMatch(genidx);
	     tempelec.setGenP4(allntgen.at(genidx).p4());
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

     for(size_t i=0;i<muons->size();i++){
   
       top::NTMuon tempmuon;
       tempmuon=makeNTMuon(muons->at(i));

       int genidx=-1;

	 if(muons->at(i).genParticle()){ // pat genmatch exists;

	   if(smallIdMap.find(muons->at(i).genParticle()) != smallIdMap.end())
	     genidx=smallIdMap.find(muons->at(i).genParticle())->second;                // use pat match
	   else
	     genidx=findGenMatchIdx(muons->at(i).genParticle(),allntgen,0.1); //match pat match to closest from ntgen collection

	   if(genidx>=0){
	     tempmuon.setGenMatch(genidx);
	     tempmuon.setGenP4(allntgen.at(genidx).p4());
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
	   templep.setDzV(vz);
	   templep.setDzVErr(vzerr);

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
	   templep.setDzV(vz);
	   templep.setDzVErr(vzerr);

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
	   templep.setDzV(vz);
	   templep.setDzVErr(vzerr);
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

       top::NTJet tempjet;
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
	   top::NTTrack nttrack;
	   top::LorentzVector p1(track->px(), track->py(), track->pz(), track->p());
	   top::LorentzVector p2(track2->px(), track2->py(), track2->pz(), track2->p());
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
	 top::LorentzVector suclup4(suclue*suclupoint.x() / magnitude,suclue*suclupoint.y() / magnitude,suclue*suclupoint.z() / magnitude,suclue);
	 if(suclup4.Pt() < 8) continue;

	 for(std::vector<reco::SuperCluster>::const_iterator suclu2=recosuclus->begin(); suclu2<recosuclus->end(); suclu2++){

	   if(suclu == suclu2) continue;

	   double suclue2=suclu2->rawEnergy();
	   math::XYZPoint suclupoint2=suclu2->position();
	   double magnitude2=sqrt(suclupoint2.mag2());
	   top::LorentzVector suclup42(suclue2*suclupoint2.x() / magnitude2,suclue2*suclupoint2.y() / magnitude2,suclue2*suclupoint2.z() / magnitude2,suclue2);

	   if(suclup42.Pt() < 8) continue;

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


   if(debugmode) std::cout << "event info" << std::endl;

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
   top::elecRhoIsoAdder addrho(!IsRealData, !rho2011_);
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

   /////// Fill generator info


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
  
  Ntuple->Branch("NTMvaMet", "top::NTMet", &ntmvamet);
  Ntuple->Branch("NTEvent", "top::NTEvent", &ntevent);

  Ntuple->Branch("TriggerBools", "std::vector<bool>", &triggerBools_);

  //  Ntuple->Branch("NTTrigger", "top::NTTrigger", &nttrigger);

  Ntuple->Branch("NTGenParticles", "std::vector<top::NTGenParticle>", &allntgen);
  Ntuple->Branch("NTGenJets", "std::vector<top::NTGenJet>", &ntgenjets);

  // Ntuple->Branch("NTGenMEMuons",     "std::vector<top::NTGenParticle>", &ntgenmuons3);
  // Ntuple->Branch("NTGenElectrons", "std::vector<top::NTGenParticle>", &ntgenelecs1);
  // Ntuple->Branch("NTGenMuons",     "std::vector<top::NTGenParticle>", &ntgenmuons1);
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
  triggers_.push_back("HLT_Ele17_SW_TightCaloEleId_Ele8_HE_L1R_v");
  triggers_.push_back("HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v");
  triggers_.push_back("HLT_DoubleMu7");
  triggers_.push_back("HLT_Mu13_Mu8_v");

  //single lep for efficiencies
  triggers_.push_back("HLT_IsoMu24_v");
  triggers_.push_back("HLT_IsoMu24_eta2p1_v");
  triggers_.push_back("HLT_Ele27_WP80_v");
  /*
  triggers_.push_back();
  triggers_.push_back();
  triggers_.push_back();
  triggers_.push_back();
  triggers_.push_back();
*/

}


top::NTGenParticle TreeWriterBase::makeNTGen(const reco::GenParticle * p, const std::map<const reco::GenParticle * , int> & idmap){
  top::NTGenParticle out;
  out.setP4(p->p4());
  out.setPdgId(p->pdgId());
  out.setStatus(p->status());
  out.setGenId(idmap.at(p));
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

