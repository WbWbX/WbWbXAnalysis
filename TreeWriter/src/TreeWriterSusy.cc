#include "../interface/TreeWriterBase.h"
#include "EGamma/EGammaAnalysisTools/interface/EGammaCutBasedEleId.h"
#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"


class TreeWriterSusy : public TreeWriterBase{

public:

	explicit TreeWriterSusy(const edm::ParameterSet & ParSet) : TreeWriterBase(ParSet) {




	}
	~TreeWriterSusy(){}






	ztop::NTMuon makeNTMuon(const pat::Muon &);
	ztop::NTElectron makeNTElectron(const pat::Electron &);
	ztop::NTJet makeNTJet(const pat::Jet &);



};

ztop::NTMuon TreeWriterSusy::makeNTMuon(const pat::Muon & muon){

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

	if(includereco_ || includetrigger_){

		std::vector<std::string> matched;
		if(muon.triggerObjectMatches().size()>0)
			matched=muon.triggerObjectMatches().begin()->pathNames();
		if(debugmode) std:: cout << "muon:matched HLTObjects size: " << matched.size() << std::endl;
		if(debugmode){
			for(size_t k=0;k<matched.size();k++)
				std::cout << "matched: " << matched.at(k) << std::endl;
		}


		tempmuon.setMatchedTrig(matched);

	}
	////////space for extra cuts

	tempmuon.setMember(0,muon.numberOfMatchedStations());
	double ptdiff=9999999;
	if(tempmuon.isPf())
		ptdiff=muon.pt()-muon.pfP4().Pt();
	tempmuon.setMember(1,ptdiff);

	tempmuon.setMember(2,muon.dB());


	return tempmuon;
}
ztop::NTElectron TreeWriterSusy::makeNTElectron(const pat::Electron & electron){

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



	if(includereco_ || includetrigger_){

		std::vector<std::string> matched;
		if(electron.triggerObjectMatches().size()>0)
			matched=electron.triggerObjectMatches().begin()->pathNames();
		if(debugmode) std:: cout << "electron:matched HLTObjects size: " << matched.size() << std::endl;
		if(debugmode){
			for(size_t k=0;k<matched.size();k++)
				std::cout << "matched: " << matched.at(k) << std::endl;
		}

		tempelec.setMatchedTrig(matched);

	}

	double suclue=0;
	if(!(electron.superCluster().isNull())){
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

	/*
	 * lets do the electron ID by hand......
	 * from http://cmssw.cvs.cern.ch/cgi-bin/cmssw.cgi/UserCode/EGamma/EGammaAnalysisTools/src/EGammaCutBasedEleId.cc?annotate=1.10.2.2
	 * else if (workingPoint == EgammaCutBasedEleId::MEDIUM) {
264 :	 	 	        cut_dEtaIn[0]        = 0.004; cut_dEtaIn[1]        = 0.007;
265 :	 	 	        cut_dPhiIn[0]        = 0.060; cut_dPhiIn[1]        = 0.030;
266 :	 	 	        cut_sigmaIEtaIEta[0] = 0.010; cut_sigmaIEtaIEta[1] = 0.030;
267 :	 	 	        cut_hoe[0]           = 0.120; cut_hoe[1]           = 0.100;
268 :	 	 	        cut_ooemoop[0]       = 0.050; cut_ooemoop[1]       = 0.050;
269 :	 	 	        cut_d0vtx[0]         = 0.020; cut_d0vtx[1]         = 0.020;
270 :	 	 	        cut_dzvtx[0]         = 0.100; cut_dzvtx[1]         = 0.100;
271 :	 	 	        cut_vtxFit[0]        = true ; cut_vtxFit[1]        = true;
272 :	 	 	        cut_mHits[0]         = 1    ; cut_mHits[1]         = 1;
273 :	 	 	        if (pt >= 20.0) {
274 :	 	 	            cut_iso[0] = 0.150; cut_iso[1] = 0.150;
275 :	 	 	        }
276 :	 	 	        else {
277 :	 	 	            cut_iso[0] = 0.150; cut_iso[1] = 0.100;
278 :	 	 	        }
279 :	 	 	    }
	 */


	const reco::BeamSpot& beamspot=*(bsHandle.product());

	bool isEB           = electron.isEB() ? true : false;
	float pt            = electron.pt();
	float eta           = electron.superCluster()->eta();

	// id variables
	float dEtaIn        = electron.deltaEtaSuperClusterTrackAtVtx();
	float dPhiIn        = electron.deltaPhiSuperClusterTrackAtVtx();
	float sigmaIEtaIEta = electron.sigmaIetaIeta();
	float hoe           = electron.hadronicOverEm();
	float ooemoop       = (1.0/electron.ecalEnergy() - electron.eSuperClusterOverP()/electron.ecalEnergy());

	// impact parameter variables
	float d0vtx         = 0.0;
	float dzvtx         = 0.0;
	if (vertices->size() > 0) {
		reco::VertexRef vtx(vertices, 0);
		d0vtx = electron.gsfTrack()->dxy(vtx->position());
		dzvtx = electron.gsfTrack()->dz(vtx->position());
	} else {
		d0vtx = electron.gsfTrack()->dxy();
		dzvtx = electron.gsfTrack()->dz();
	}
	const float iso_ch=0;
	const float iso_em=0;
	const float iso_nh=0;

	reco::GsfElectron ele(electron.charge(),
			electron.chargeInfo(),
			electron.core(),
			electron.trackClusterMatching(),
			electron.trackExtrapolations(),
			electron.closestCtfTrack(),
			electron.fiducialFlags(),
			electron.showerShape(),
			electron.conversionRejectionVariables());



	bool vtxFitConversion = ConversionTools::hasMatchedConversion(ele, conversions, beamspot.position());
	float mHits = ele.gsfTrack()->trackerExpectedHitsInner().numberOfHits();
	const double rho=0;



	bool pass =  EgammaCutBasedEleId::PassWP(EgammaCutBasedEleId::MEDIUM,  isEB,  pt,  eta,
			dEtaIn,  dPhiIn,  sigmaIEtaIEta,  hoe,
			ooemoop,  d0vtx,  dzvtx,  iso_ch,  iso_em, iso_nh,
			vtxFitConversion, mHits, rho);

	double dpass=0;
	if(pass) dpass=1;
	tempelec.setMember(10,dpass);

	return tempelec;
}
ztop::NTJet TreeWriterSusy::makeNTJet(const pat::Jet & jet){

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



DEFINE_FWK_MODULE(TreeWriterSusy);
