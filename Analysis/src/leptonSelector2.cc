
#include "WbWbXAnalysis/DataFormats/interface/NTElectron.h"
#include "WbWbXAnalysis/DataFormats/interface/NTLepton.h"
#include "WbWbXAnalysis/DataFormats/interface/NTEvent.h"
#include "WbWbXAnalysis/DataFormats/interface/NTMuon.h"
#include "WbWbXAnalysis/DataFormats/interface/NTTrack.h"
#include "WbWbXAnalysis/DataFormats/interface/NTSuClu.h"

#include <algorithm>
#include "TString.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"



namespace ztop{


std::pair<std::vector<ztop::NTElectron* > , std::vector<ztop::NTMuon* > >
getOppoQHighestPtPair(const std::vector<ztop::NTElectron*> &elecs, const std::vector<ztop::NTMuon*> & muons, int qmulti=1){
	double sumpt=0;
	//double emupt=0;
	std::vector<ztop::NTElectron*> outelecs;
	std::vector<ztop::NTMuon*> outmuons;
	enum channels {ee,mumu,emu,no};
	channels channel=no;
	unsigned int a=999;
	unsigned int b=999;

	for(unsigned int i=0;i<elecs.size();i++){
		for(unsigned int j=i;j<elecs.size();j++){
			if(i==j) continue;
			if(qmulti*elecs.at(i)->q() * elecs.at(j)->q() < 0 && sumpt < (elecs.at(i)->pt() + elecs.at(j)->pt())){
				sumpt=elecs.at(i)->pt() + elecs.at(j)->pt();
				a=i;
				b=j;
				channel=ee;
			}
		}
	}
	for(unsigned int i=0;i<muons.size();i++){
		for(unsigned int j=i;j<muons.size();j++){
			if(i==j) continue;
			if(qmulti*muons.at(i)->q() * muons.at(j)->q() < 0 && sumpt < (muons.at(i)->pt() + muons.at(j)->pt())){
				sumpt=muons.at(i)->pt() + muons.at(j)->pt();
				a=i;
				b=j;
				channel=mumu;
			}
		}
	}
	for(unsigned int i=0;i<elecs.size();i++){
		for(unsigned int j=0;j<muons.size();j++){
			//if(i==j) continue;
			if(qmulti* elecs.at(i)->q() * muons.at(j)->q() < 0 && sumpt < (elecs.at(i)->pt() + muons.at(j)->pt())){
				sumpt=elecs.at(i)->pt() + muons.at(j)->pt();
				a=i;
				b=j;
				channel=emu;
			}
		}
	}
	if(channel == ee){
		outelecs.push_back(elecs.at(a)); outelecs.push_back(elecs.at(b));
	}
	else if(channel == mumu){
		outmuons.push_back(muons.at(a)); outmuons.push_back(muons.at(b));
	}
	else if(channel == emu){
		outelecs.push_back(elecs.at(a));
		outmuons.push_back(muons.at(b));
	}
	std::pair<std::vector<ztop::NTElectron*>, std::vector<ztop::NTMuon*> > out(outelecs,outmuons);
	return out;
}

}

