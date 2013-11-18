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
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"


//selectpassingProbes does NOT include the isolation cut!!!! be careful

namespace ztop{


std::pair<std::vector<ztop::NTElectron* > , std::vector<ztop::NTMuon* > >
getOppoQHighestPtPair(const std::vector<ztop::NTElectron*> &elecs, const std::vector<ztop::NTMuon*> & muons, int qmulti=1){
	double sumpt=0;
	//double emupt=0;
	std::vector<ztop::NTElectron*> outelecs;
	std::vector<ztop::NTMuon*> outmuons;
	int channel=99; // ee,mumu,emu
	unsigned int a=999;
	unsigned int b=999;

	for(unsigned int i=0;i<elecs.size();i++){
		for(unsigned int j=i;j<elecs.size();j++){
			if(i==j) continue;
			if(qmulti*elecs[i]->q() * elecs[j]->q() < 0 && sumpt < (elecs[i]->pt() + elecs[j]->pt())){
				sumpt=elecs[i]->pt() + elecs[j]->pt();
				a=i;
				b=j;
				channel=0;
			}
		}
	}
	for(unsigned int i=0;i<muons.size();i++){
		for(unsigned int j=i;j<muons.size();j++){
			if(i==j) continue;
			if(qmulti*muons[i]->q() * muons[j]->q() < 0 && sumpt < (muons[i]->pt() + muons[j]->pt())){
				sumpt=muons[i]->pt() + muons[j]->pt();
				a=i;
				b=j;
				channel=1;
			}
		}
	}
	for(unsigned int i=0;i<elecs.size();i++){
		for(unsigned int j=0;j<muons.size();j++){
			//if(i==j) continue;
			if(qmulti*elecs[i]->q() * muons[j]->q() < 0 && sumpt < (elecs[i]->pt() + muons[j]->pt())){
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
	std::pair<std::vector<ztop::NTElectron*>, std::vector<ztop::NTMuon*> > out(outelecs,outmuons);
	return out;
}

}


#endif
