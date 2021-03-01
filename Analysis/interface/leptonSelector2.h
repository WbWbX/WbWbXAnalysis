#ifndef leptonSelector2_h
#define leptonSelector2_h

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
getOppoQHighestPtPair(const std::vector<ztop::NTElectron*> &elecs, const std::vector<ztop::NTMuon*> & muons, int qmulti=1);


}


#endif
