#ifndef leptonSelector2_h
#define leptonSelector2_h

#include "TtZAnalysis/DataFormats/interface/NTElectron.h"
#include "TtZAnalysis/DataFormats/interface/NTLepton.h"
#include "TtZAnalysis/DataFormats/interface/NTEvent.h"
#include "TtZAnalysis/DataFormats/interface/NTMuon.h"
#include "TtZAnalysis/DataFormats/interface/NTTrack.h"
#include "TtZAnalysis/DataFormats/interface/NTSuClu.h"

#include <algorithm>
#include "TString.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"



namespace ztop{


std::pair<std::vector<ztop::NTElectron* > , std::vector<ztop::NTMuon* > >
getOppoQHighestPtPair(const std::vector<ztop::NTElectron*> &elecs, const std::vector<ztop::NTMuon*> & muons, int qmulti=1);


}


#endif
