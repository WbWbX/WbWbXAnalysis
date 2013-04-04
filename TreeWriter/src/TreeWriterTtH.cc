#include "../interface/TreeWriterBase.h"

class TreeWriterTtH : public TreeWriterBase{

public:

  explicit TreeWriterTtH(const edm::ParameterSet & ParSet) : TreeWriterBase(ParSet) {


    writeMatchedHLTObjects_    =ParSet.getParameter<std::vector<std::string> >        ("writeMatchedHLTObjects");

    //   writeMatchedHLTObjects_ << "HLT_IsoMu24_v" << "HLT_IsoMu24_eta2p1_v";
    std::cout << "Writing matched triggersObjects:" << std::endl;
    for(size_t i=0;i<writeMatchedHLTObjects_.size() ;i++)
      std::cout << writeMatchedHLTObjects_.at(i) << " " ;
    std::cout << std::endl;
    

  }
  ~TreeWriterTtH(){}






  ztop::NTMuon makeNTMuon(const pat::Muon &);
  ztop::NTElectron makeNTElectron(const pat::Electron &);
  ztop::NTJet makeNTJet(const pat::Jet &);


  std::vector<std::string> writeMatchedHLTObjects_;


};

ztop::NTMuon TreeWriterTtH::makeNTMuon(const pat::Muon & muon){
  
  ztop::LorentzVector p4zero(0,0,0,0);
  ztop::NTMuon tempmuon;

  tempmuon.setIsPf(muon.isPFMuon());

  ztop::NTIsolation Iso;
  Iso.setChargedHadronIso(muon.chargedHadronIso());
  Iso.setNeutralHadronIso(muon.neutralHadronIso());
  Iso.setPhotonIso(muon.photonIso());
  Iso.setPuChargedHadronIso(muon.puChargedHadronIso());

  tempmuon.setIso    (Iso);

  tempmuon.setP4   (muon.p4());
  tempmuon.setQ   (muon.charge());

  if(includereco_ && !(muon.innerTrack()).isNull()){
    ztop::LorentzVector trkp4(muon.innerTrack()->px(),muon.innerTrack()->py(),muon.innerTrack()->pz(),muon.innerTrack()->p());
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

    std::vector<std::string> matched;
    for(size_t i=0;i<writeMatchedHLTObjects_.size();i++){
      if(muon.triggerObjectMatchesByPath(writeMatchedHLTObjects_.at(i)).size() > 0){
	matched.push_back(writeMatchedHLTObjects_[i]);
      }
      if(muon.triggerObjectMatchesByCollection(writeMatchedHLTObjects_.at(i)).size() >0){
	matched.push_back(writeMatchedHLTObjects_[i]);
      }
      if(muon.triggerObjectMatchesByFilter(writeMatchedHLTObjects_.at(i)).size() >0){
	matched.push_back(writeMatchedHLTObjects_[i]);
      }
    }
    if(debugmode) std:: cout << "muon:matched HLTObjects size: " << matched.size() << std::endl;
    tempmuon.setMatchedTrig(matched);
    
  }
  ////////space for extra cuts






  return tempmuon;
}
ztop::NTElectron TreeWriterTtH::makeNTElectron(const pat::Electron & electron){
  
  ztop::LorentzVector p4zero(0,0,0,0);

   ztop::NTElectron tempelec;

  ztop::NTIsolation Iso;
       
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
    vzbs=electron.gsfTrack()->dz(beamSpotPosition);                   //
    vzerr=electron.gsfTrack()->dzError();  
    d0V=fabs(electron.gsfTrack()->dxy(vtxs[0].position()));
    d0Bs=fabs(electron.gsfTrack()->dxy(beamSpotPosition));

    mhits=electron.gsfTrack()->trackerExpectedHitsInner().numberOfHits();
  }              //
  else if(!(electron.closestCtfTrackRef()).isNull()){
    vz=electron.closestCtfTrackRef()->dz(vtxs[0].position());                   //
    vzbs=electron.closestCtfTrackRef()->dz(beamSpotPosition);                   //
    vzerr=electron.closestCtfTrackRef()->dzError();  
    d0V=fabs(electron.closestCtfTrackRef()->dxy(vtxs[0].position()));
    d0Bs=fabs(electron.closestCtfTrackRef()->dxy(beamSpotPosition));
  }
  tempelec.setDzV(vz);                   //
  tempelec.setDzVErr(vzerr);  
  tempelec.setDzBs(vzbs);     
  tempelec.setD0V(d0V); 
  tempelec.setD0Bs(d0Bs); 

  tempelec.setNotConv(electron.passConversionVeto());    
  tempelec.setId(electron.electronIDs());
  tempelec.setIso(Iso);                        //
  tempelec.setMHits(mhits);


		       
  if(includereco_){
    
    std::vector<std::string> matched;
    for(size_t i=0;i<writeMatchedHLTObjects_.size();i++){
      if(electron.triggerObjectMatchesByPath(writeMatchedHLTObjects_.at(i)).size() > 0)
	matched.push_back(writeMatchedHLTObjects_[i]);
      if(electron.triggerObjectMatchesByCollection(writeMatchedHLTObjects_.at(i)).size() >0)
	matched.push_back(writeMatchedHLTObjects_[i]);
      if(electron.triggerObjectMatchesByFilter(writeMatchedHLTObjects_.at(i)).size() >0)
	matched.push_back(writeMatchedHLTObjects_[i]);
    }
    if(debugmode) std:: cout << "electron:matched HLTObjects size: " << matched.size() << std::endl;
   
    tempelec.setMatchedTrig(matched);
  }
  double suclue=0;
  if(includereco_ && !(electron.superCluster().isNull())){
    suclue=electron.superCluster()->rawEnergy();
    math::XYZPoint suclupoint=electron.superCluster()->position();
    double magnitude=sqrt(suclupoint.mag2());
    ztop::LorentzVector suclup4(suclue*suclupoint.x() / magnitude,suclue*suclupoint.y() / magnitude,suclue*suclupoint.z() / magnitude,suclue);
    ztop::NTSuClu suclu;
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
ztop::NTJet TreeWriterTtH::makeNTJet(const pat::Jet & jet){
  
  ztop::LorentzVector p4zero(0,0,0,0);

  ztop::NTJet tempjet;

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
