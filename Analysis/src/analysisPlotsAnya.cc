/*
 * analysisPlotsAnya.cc
 *
 *  Created on: Feb 12, 2014
 *      Author: kiesej
 */


#include "../interface/analysisPlotsAnya.h"
#include "../interface/AnalysisUtils.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "TtZAnalysis/DataFormats/src/classes.h"

namespace ztop{

void analysisPlotsAnya::bookPlots(){
    if(!use()) return;
    using namespace std;
    using namespace ztop;

/*
    vector<float> inclbins; inclbins << 0.5 << 1.5; //vis PS, fullPS

    total=addPlot(inclbins,inclbins,"total","bin","N_{evt}");
    total->setBinByBin(true); //independent bins

*/

}


void analysisPlotsAnya::fillPlotsGen(){
    if(!use()) return;
    if(!event()) return;
    using namespace std;
    using namespace ztop;

   /* total->fillGen(1.,puweight()); */


}

void analysisPlotsAnya::fillPlotsReco(){
    if(!use()) return;
    if(!event()) return;
    using namespace std;
    using namespace ztop;

   /* total->fillReco(1,puweight()); */


}

}//ns
