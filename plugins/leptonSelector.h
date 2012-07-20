#ifndef leptonSelector_h
#define leptonSelector_h

#include "TtZAnalysis/DataFormats/interface/NTElectron.h"
#include "TtZAnalysis/DataFormats/interface/NTLepton.h"
#include "TtZAnalysis/DataFormats/interface/NTEvent.h"
#include "TtZAnalysis/DataFormats/interface/NTMuon.h"
#include "TtZAnalysis/DataFormats/interface/NTTrack.h"
#include "TtZAnalysis/DataFormats/interface/NTSuClu.h"

#include <algorithm>
#include "TString.h"
#include "miscUtils.h"


//selectpassingProbes does NOT include the isolation cut!!!! be careful

namespace top{
  class effTriple{
  public:
    effTriple(){mass_=0;passed_=false;};
    ~effTriple(){};
    effTriple(top::LorentzVector P4, bool Passed, double Mass){p4_=P4;passed_=Passed;mass_=Mass;};
    top::LorentzVector p4(){return p4_;}
    bool passed(){return passed_;}
    double mass(){return mass_;}

  private:
    top::LorentzVector p4_;
    bool passed_;
    double mass_;

  };


  class leptonSelector {

  public:

    leptonSelector(){massrange_[0]=60;massrange_[1]=120;};
    ~leptonSelector(){};
    
    void setMassRange(double Massrangemin, double Massrangemax){massrange_[0]=Massrangemin;massrange_[1]=Massrangemax;userhoisoforelectrons_=true;}

    void setUseRhoIsoForElectrons(bool userhoisoforelectrons){userhoisoforelectrons_=userhoisoforelectrons;}
    
    std::vector<top::NTElectron> selectIDElectrons(std::vector<top::NTElectron> & treeElectrons){

      std::vector<top::NTElectron> passingelecs;
      for(std::vector<top::NTElectron>::iterator elec=treeElectrons.begin();elec<treeElectrons.end();++elec){
	if(elec->mvaId() <= -0.1)    continue;
	if(elec->pt() <= 20)         continue;
	if(fabs(elec->eta()) >= 2.5) continue;
	if(elec->dbs() >= 0.04)      continue;
	if(!(elec->isNotConv()))     continue;
	passingelecs.push_back(*elec);
      }
      return passingelecs;
    }

    std::vector<top::NTElectron> selectIsolatedElectrons(std::vector<top::NTElectron> & treeElectrons){

      std::vector<top::NTElectron> passingelecs;

      for(std::vector<top::NTElectron>::iterator elec=treeElectrons.begin();elec<treeElectrons.end();++elec){
	if(userhoisoforelectrons_ && elec->rhoIso03() >= 0.15) continue;
	if(!userhoisoforelectrons_ && elec->isoVal03() >=0.15) continue;
	passingelecs.push_back(*elec);
      }
      return passingelecs;
    }

    std::vector<top::NTElectron> selectTagElectrons(std::vector<top::NTElectron> & treeElectrons){

      std::vector<top::NTElectron> passingelecs;
      
      for(std::vector<top::NTElectron>::iterator elec=treeElectrons.begin();elec<treeElectrons.end();++elec){
	/////perform additional cuts TRIGGERERR!!
	bool passedtrig=false;
	for(unsigned int i=0;i<elec->matchedTrig().size();++i){
	  TString trig = elec->matchedTrig()[i];
	  if(trig.Contains("HLT_Ele27_WP80_v")){
	    passedtrig=true;
	    break;
	  }
	}
	if(!passedtrig) continue;
	passingelecs.push_back(*elec);
      }
      return passingelecs;
    }
    
    std::vector<top::NTMuon> selectIDMuons(std::vector<top::NTMuon> & treeMuons){
      
      std::vector<top::NTMuon> passingmuons;
      
      for(std::vector<top::NTMuon>::iterator muon=treeMuons.begin(); muon<treeMuons.end();++muon){
	if(muon->pt() <= 20)         continue;
	if(fabs(muon->eta()) >= 2.4) continue;
	passingmuons.push_back(*muon);
      }
      return passingmuons;
    }
    

    std::vector<top::NTMuon> selectIsolatedMuons(std::vector<top::NTMuon> & treeMuons){
      
      std::vector<top::NTMuon> passingmuons;
      
      for(std::vector<top::NTMuon>::iterator muon=treeMuons.begin(); muon<treeMuons.end();++muon){
	if(muon->isoVal04() >= 0.2)  continue;
	passingmuons.push_back(*muon);
      }
      return passingmuons;
    }

    std::vector<top::NTMuon> selectTagMuons(std::vector<top::NTMuon> & treeMuons){

      std::vector<top::NTMuon> passing;
      for(std::vector<top::NTMuon>::iterator muon=treeMuons.begin();muon<treeMuons.end();++muon){

	bool passedtrig=false;
	for(unsigned int i=0;i<muon->matchedTrig().size();i++){
	  TString trig = muon->matchedTrig()[i];
	  if(trig.Contains("HLT_IsoMu24_v") || trig.Contains("HLT_IsoMu24_eta2p1_v")){
	    passedtrig=true;
	    break;
	  }
	}
	if(!passedtrig) continue;

	//additional cuts?

	passing.push_back(*muon);
      }
      return passing;
    }
    
    top::effTriple getMatchingProbeElec(std::vector<top::NTSuClu> & probeSuClus, std::vector<top::NTElectron> & tagElecs, std::vector<top::NTElectron> & passingProbes){
      
      bool passes=false;
      bool foundpair=false;
      double mass=0;
      top::LorentzVector probevec(0,0,0,0);
      
      for(std::vector<top::NTElectron>::iterator tagelec=tagElecs.begin(); tagelec < tagElecs.end(); ++tagelec){
	for(std::vector<top::NTSuClu>::iterator suclu=probeSuClus.begin(); suclu < probeSuClus.end();++suclu){
	  if((tagelec->p4() + suclu->p4()).M() > massrange_[0] && (tagelec->p4() + suclu->p4()).M() < massrange_[1]){ // in Z range
	    mass=(tagelec->p4() + suclu->p4()).M();
	    foundpair=true;
	    for(std::vector<top::NTElectron>::iterator pprobeelec=passingProbes.begin(); pprobeelec<passingProbes.end();++pprobeelec){ //check for reconstr elec
	      if(suclu->p4() == pprobeelec->suClu().p4()){
		passes=true;
		probevec=pprobeelec->p4();
		break;
	      }
	    }
	    if(!passes){
	      probevec=suclu->p4();
	    }
	    break; //break after first found suclu fitting to tagelec
	  }
	}
	if(foundpair) break;
      }
      top::effTriple out(probevec, passes, mass);
      return out;
    }
    
    top::effTriple getMatchingProbeElec(std::vector<top::NTElectron> & probeElectrons, std::vector<top::NTElectron> & tagElecs, std::vector<top::NTElectron> & passingProbes){
      
      bool passes=false;
      bool foundpair=false;
      double mass=0;
      top::LorentzVector probevec(0,0,0,0);
      
      for(std::vector<top::NTElectron>::iterator tagelec=tagElecs.begin(); tagelec < tagElecs.end(); ++tagelec){
	for(std::vector<top::NTElectron>::iterator elec=probeElectrons.begin(); elec < probeElectrons.end();++elec){
	  if((tagelec->p4() + elec->p4()).M() > massrange_[0] && (tagelec->p4() + elec->p4()).M() < massrange_[1]){ // in Z range
	    mass=(tagelec->p4() + elec->p4()).M();
	    foundpair=true;
	    for(std::vector<top::NTElectron>::iterator pprobeelec=passingProbes.begin(); pprobeelec<passingProbes.end();++pprobeelec){ //check for reconstr elec
	      if(elec->p4() == pprobeelec->p4()){
		passes=true;
		probevec=pprobeelec->p4();
		break;
	      }
	    }
	    if(!passes){
	      probevec=elec->p4();
	    }
	    break; //break after first found suclu fitting to tagelec
	  }
	}
	if(foundpair) break;
      }
      top::effTriple out(probevec, passes, mass);
      return out;
    }

    top::effTriple getMatchingProbeMuon(std::vector<top::NTTrack> & probeTracks, std::vector<top::NTMuon> & tagMuons, std::vector<top::NTMuon> & passingProbes){
      
      bool passes=false;
      bool foundpair=false;
      top::LorentzVector probevec(0,0,0,0);
      double mass=0;
      
      for(std::vector<top::NTMuon>::iterator tagmuon=tagMuons.begin(); tagmuon < tagMuons.end(); ++tagmuon){
	for(std::vector<top::NTTrack>::iterator track=probeTracks.begin(); track < probeTracks.end();++track){

	  double dzvert=tagmuon->vertexZ() - track->vertexZ();
	  double dzerror=sqrt(pow(tagmuon->vertexZErr(),2) + pow(track->vertexZErr(),2));

	  if((tagmuon->p4() + track->p4()).M() > massrange_[0] 
	     && (tagmuon->p4() + track->p4()).M() < massrange_[1] 
	     && tagmuon->q() != track->q()
	     && dzvert/dzerror < 9){ // in Z range + oppocharge + Z vertex

	    mass=(tagmuon->p4() + track->p4()).M();
	    foundpair=true;
	    for(std::vector<top::NTMuon>::iterator pprobemuon=passingProbes.begin(); pprobemuon<passingProbes.end();++pprobemuon){ //check for reconstr muon
	      /////////////////////////////////////////
	      if(!noOverlap(track, tagmuon, 0.1)){   //which deltaR??
		passes=true;
		probevec=pprobemuon->p4();
		break;
	      }
	    }
	    if(!passes){
	      probevec=track->p4();
	    }
	    break; //break after first found track fitting to tagmuon
	  }
	}
	if(foundpair) break;
      }
      top::effTriple out(probevec, passes, mass);
      return out;
    }
       
    top::effTriple getMatchingProbeMuon(std::vector<top::NTMuon> & probeMuons, std::vector<top::NTMuon> & tagMuons, std::vector<top::NTMuon> & passingProbes){
      
      bool passes=false;
      bool foundpair=false;
      top::LorentzVector probevec(0,0,0,0);
      double mass=0;
      
      for(std::vector<top::NTMuon>::iterator tagmuon=tagMuons.begin(); tagmuon < tagMuons.end(); ++tagmuon){
	for(std::vector<top::NTMuon>::iterator muon=probeMuons.begin(); muon < probeMuons.end();++muon){
	  if((tagmuon->p4() + muon->p4()).M() > massrange_[0] && (tagmuon->p4() + muon->p4()).M() < massrange_[1] && tagmuon->q() != muon->q()){ // in Z range + oppocharge

	    mass=(tagmuon->p4() + muon->p4()).M();
	    foundpair=true;
	    for(std::vector<top::NTMuon>::iterator pprobemuon=passingProbes.begin(); pprobemuon<passingProbes.end();++pprobemuon){ //check for reconstr muon
	      /////////////////////////////////////////
	      if(muon->p4() == pprobemuon->p4()){   //maybe there is matching needed???
		passes=true;
		probevec=pprobemuon->p4();
		break;
	      }
	    }
	    if(!passes){
	      probevec=muon->p4();
	    }
	    break; //break after first found track fitting to tagmuon
	  }
	}
	if(foundpair) break;
      }
      top::effTriple out(probevec, passes, mass);
      return out;
    } 
  private:
    double massrange_[2];
    bool userhoisoforelectrons_;
    
  };
}
  

#endif
