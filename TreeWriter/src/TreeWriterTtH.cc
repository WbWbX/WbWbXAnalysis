#include "../interface/TreeWriterBase.h"

class TreeWriterTtH : public TreeWriterBase{

public:

  explicit TreeWriterTtH(const edm::ParameterSet & ParSet) : TreeWriterBase(ParSet) {}
  ~TreeWriterTtH(){}






  top::NTMuon makeNTMuon(const pat::Muon &);
  top::NTElectron makeNTElectron(const pat::Electron &);
  top::NTJet makeNTJet(const pat::Jet &);





};

top::NTMuon TreeWriterTtH::makeNTMuon(const pat::Muon & muon){
  
  top::LorentzVector p4zero(0,0,0,0);
  top::NTMuon tempmuon;

  tempmuon.setIsPf(muon.isPFMuon());

  top::NTIsolation Iso;
  Iso.setChargedHadronIso(muon.chargedHadronIso());
  Iso.setNeutralHadronIso(muon.neutralHadronIso());
  Iso.setPhotonIso(muon.photonIso());
  Iso.setPuChargedHadronIso(muon.puChargedHadronIso());

  tempmuon.setIso    (Iso);

  tempmuon.setP4   (muon.p4());
  tempmuon.setQ   (muon.charge());

  if(includereco_ && !(muon.innerTrack()).isNull()){
    top::LorentzVector trkp4(muon.innerTrack()->px(),muon.innerTrack()->py(),muon.innerTrack()->pz(),muon.innerTrack()->p());
    tempmuon.setTrackP4(trkp4);
  }
  else{
    tempmuon.setTrackP4(p4zero);
  }
  
  double vz=-9999;
  double vzerr=-9999;
  double vzbs=-9999;
  if(!(muon.globalTrack()).isNull()){
    vz=muon.globalTrack()->dz(vtxs[0].position());
    vzerr=muon.globalTrack()->dzError();
    vzbs=muon.globalTrack()->dz(beamSpotPosition);
  }
  else if(!(muon.innerTrack()).isNull()){
    vz=muon.innerTrack()->dz(vtxs[0].position());
    vzerr=muon.innerTrack()->dzError();
    vzbs=muon.innerTrack()->dz(beamSpotPosition);
	 
  }
  else if(!(muon.outerTrack()).isNull()){
    vz=muon.outerTrack()->dz(vtxs[0].position());
    vzerr=muon.outerTrack()->dzError();
    vzbs=muon.outerTrack()->dz(beamSpotPosition);
  }

  tempmuon.setPixHits(-9999);
  if(!(muon.innerTrack()).isNull())
    tempmuon.setPixHits(muon.innerTrack()->hitPattern().numberOfValidPixelHits());

  tempmuon.setDzV   (vz);
  tempmuon.setDzVErr   (vzerr);
  tempmuon.setDzBs (vzbs);

  tempmuon.setIsGlobal   (muon.isGlobalMuon());
  tempmuon.setIsTracker (muon.isTrackerMuon());

  if(muon.isGlobalMuon()){
    tempmuon.setNormChi2   (muon.globalTrack()->normalizedChi2());
    if(!(muon.innerTrack().isNull())) 
      tempmuon.setTrkHits  (muon.innerTrack()->hitPattern().trackerLayersWithMeasurement());
    else
      tempmuon.setTrkHits (0);
    tempmuon.setMuonHits   (muon.globalTrack()->hitPattern().numberOfValidMuonHits());
    tempmuon.setD0V   (fabs(muon.globalTrack()->dxy(vtxs[0].position())));
    tempmuon.setD0Bs   (fabs(muon.globalTrack()->dxy(beamSpotPosition)));
    
  }
  else{
    tempmuon.setNormChi2   (100);
    tempmuon.setTrkHits    (0);
    tempmuon.setMuonHits    (0);
    tempmuon.setD0V  (-9999);
    tempmuon.setD0Bs  (-9999);
    
  }
  if(includereco_){
    if(muon.triggerObjectMatches().size() ==1){ // no ambiguities
      tempmuon.setMatchedTrig(muon.triggerObjectMatches().begin()->pathNames());
    }
    else{
      std::vector<std::string> def;
      def.push_back("NoUnamTrigMatch");
      tempmuon.setMatchedTrig(def);
    }
  }
  ////////space for extra cuts






  return tempmuon;
}
top::NTElectron TreeWriterTtH::makeNTElectron(const pat::Electron & electron){
  
  top::LorentzVector p4zero(0,0,0,0);

   top::NTElectron tempelec;

  top::NTIsolation Iso;
       
  Iso.setChargedHadronIso(electron.chargedHadronIso());
  Iso.setNeutralHadronIso(electron.neutralHadronIso());
  Iso.setPhotonIso(electron.photonIso());
  Iso.setPuChargedHadronIso(electron.puChargedHadronIso());
	 

  //  if(electron.ecalDrivenMomentum())


  tempelec.setECalP4(electron.ecalDrivenMomentum());
  tempelec.setIsPf(electron.isPF());
  tempelec.setP4(electron.p4());
  tempelec.setQ(electron.charge());
  double vz=-9999;
  double vzerr=-9999;
  double vzbs=-9999;
  double d0V=-9999;
  double d0Bs=-9999;

  int mhits=99;



  if(!(electron.gsfTrack().isNull())){
    vz=electron.gsfTrack()->dz(vtxs[0].position());                   //
    vzerr=electron.gsfTrack()->dzError();  
    d0V=fabs(electron.gsfTrack()->dxy(vtxs[0].position()));
    d0Bs=fabs(electron.gsfTrack()->dxy(beamSpotPosition));

    mhits=electron.gsfTrack()->trackerExpectedHitsInner().numberOfHits();
  }              //
  else if(!(electron.closestCtfTrackRef()).isNull()){
    vz=electron.closestCtfTrackRef()->dz(vtxs[0].position());                   //
    vzerr=electron.closestCtfTrackRef()->dzError();  
    d0V=fabs(electron.closestCtfTrackRef()->dxy(vtxs[0].position()));
    d0Bs=fabs(electron.closestCtfTrackRef()->dxy(beamSpotPosition));
  }
  tempelec.setDzV(vz);                   //
  tempelec.setDzVErr(vzerr);  
  tempelec.setDzBs(vzbs);  
  tempelec.setD0Bs(d0Bs);   
  tempelec.setD0V(d0V);     
             //
  tempelec.setNotConv(electron.passConversionVeto());    
  tempelec.setId(electron.electronIDs());
  tempelec.setIso(Iso);                        //
  tempelec.setMHits(mhits);


		       
  if(includereco_){
    if(electron.triggerObjectMatches().size() ==1){ // no ambiguities
      tempelec.setMatchedTrig(electron.triggerObjectMatches().begin()->pathNames());
    }
    else{
      std::vector<std::string> def;
      def.push_back("NoUnamTrigMatch");
      tempelec.setMatchedTrig(def);
    }
  }
  double suclue=0;
  if(includereco_ && !(electron.superCluster().isNull())){
    suclue=electron.superCluster()->rawEnergy();
    math::XYZPoint suclupoint=electron.superCluster()->position();
    double magnitude=sqrt(suclupoint.mag2());
    top::LorentzVector suclup4(suclue*suclupoint.x() / magnitude,suclue*suclupoint.y() / magnitude,suclue*suclupoint.z() / magnitude,suclue);
    top::NTSuClu suclu;
    suclu.setP4(suclup4);
    tempelec.setSuClu(suclu);
  }
  ////extra stuff set member by id

  if(!(electron.gsfTrack().isNull())){
    tempelec.setMember(1, electron.gsfTrack()->charge());              //1: gsfCharge
    tempelec.setMember(2, electron.gsfTrack()->pt());                  //2: tkPT
    tempelec.setMember(3, electron.gsfTrack()->dxy(beamSpotPosition)); //3: correctedD0
  }
  if(!(electron.closestCtfTrackRef()).isNull()){
    tempelec.setMember(4, electron.closestCtfTrackRef()->charge());    //4: tkCharge
  }
  
  tempelec.setMember(5,electron.dB(pat::Electron::PV3D));              //5: IP

  if(electron.superCluster().isAvailable()) {
    tempelec.setMember(6,electron.caloEnergy() * sin( electron.superCluster()->position().theta() )); //6: scEt
  }

  return tempelec;
}
top::NTJet TreeWriterTtH::makeNTJet(const pat::Jet & jet){
  
  top::LorentzVector p4zero(0,0,0,0);

  top::NTJet tempjet;

  pat::Jet uncJet=jet.correctedJet("Uncorrected");
 
  double corr_factor=jet.pt() / uncJet.pt();
  tempjet.setCorrFactor(corr_factor);

  tempjet.setId(checkJetID(jet));
  tempjet.setP4(jet.p4());
  if(jet.genJet()){
    tempjet.setGenP4(jet.genJet()->p4());
    tempjet.setGenPartonFlavour(jet.partonFlavour());
  	 
  }
  else{
    tempjet.setGenP4(p4zero);
    tempjet.setGenPartonFlavour(0);
  }
  tempjet.setBtag(jet.bDiscriminator(btagalgo_));    //
  double emEnergyfraction=0;
  if(jet.isPFJet()) emEnergyfraction=jet.chargedEmEnergyFraction()+jet.neutralEmEnergyFraction();
  else emEnergyfraction=jet.emEnergyFraction();
  tempjet.setEmEnergyFraction(emEnergyfraction);

  return tempjet;
}



DEFINE_FWK_MODULE(TreeWriterTtH);
