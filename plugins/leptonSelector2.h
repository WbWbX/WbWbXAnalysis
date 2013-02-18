#ifndef leptonSelector2_h
#define leptonSelector2_h

#include "../DataFormats/interface/NTElectron.h"
#include "../DataFormats/interface/NTLepton.h"
#include "../DataFormats/interface/NTEvent.h"
#include "../DataFormats/interface/NTMuon.h"
#include "../DataFormats/interface/NTTrack.h"
#include "../DataFormats/interface/NTSuClu.h"

#include <algorithm>
#include "TString.h"
#include "../Tools/interface/miscUtils.h"


//selectpassingProbes does NOT include the isolation cut!!!! be careful

namespace top{
  

  std::pair<std::vector<top::NTElectron* > , std::vector<top::NTMuon* > > getOppoQHighestPtPair(const std::vector<top::NTElectron*> &elecs, const std::vector<top::NTMuon*> & muons){
    double sumpt=0;
    //double emupt=0;
    std::vector<top::NTElectron*> outelecs;
    std::vector<top::NTMuon*> outmuons;
    int channel=99; // ee,mumu,emu
    unsigned int a=999;
    unsigned int b=999;

    for(unsigned int i=0;i<elecs.size();i++){
      for(unsigned int j=i;j<elecs.size();j++){
	if(elecs[i]->q() * elecs[j]->q() < 0 && sumpt < (elecs[i]->pt() + elecs[j]->pt())){
	  sumpt=elecs[i]->pt() + elecs[j]->pt();
	  a=i;
	  b=j;
	  channel=0;
	}
      }
    }
    for(unsigned int i=0;i<muons.size();i++){
      for(unsigned int j=i;j<muons.size();j++){
	if(muons[i]->q() * muons[j]->q() < 0 && sumpt < (muons[i]->pt() + muons[j]->pt())){
	  sumpt=muons[i]->pt() + muons[j]->pt();
	  a=i;
	  b=j;
	  channel=1;
	}
      }
    }
    for(unsigned int i=0;i<elecs.size();i++){
      for(unsigned int j=0;j<muons.size();j++){
	if(elecs[i]->q() * muons[j]->q() < 0 && sumpt < (elecs[i]->pt() + muons[j]->pt())){
	  sumpt=elecs[i]->pt() + muons[j]->pt();
	  a=i;
	  b=j;
	  channel=2;
	}
      }
    }
    if(channel == 0){
      outelecs << elecs[a] << elecs[b];
    }
    else if(channel == 1){
      outmuons << muons[a] << muons[b];
    }
    else if(channel == 2){
      outelecs << elecs[a];
      outmuons << muons[b];
    }
    std::pair<std::vector<top::NTElectron*>, std::vector<top::NTMuon*> > out(outelecs,outmuons);
    return out;
  }

}
  

#endif
