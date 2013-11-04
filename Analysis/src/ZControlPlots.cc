/*
 * ZControlPlots.cc
 *
 *  Created on: Nov 1, 2013
 *      Author: kiesej
 */

#include "../interface/ZControlPlots.h"
#include "../interface/NTFullEvent.h"

#include "TtZAnalysis/DataFormats/src/classes.h"

namespace ztop{
/**
 * setbins, addplot, FILL
 */
void ZControlPlots::makeControlPlots(const size_t & step){
	initStep(step);
	using namespace std;


	///LEPTONS
	SETBINSRANGE(120,0,240);
	addPlot("mll Z Range","m_{ll}[GeV]","N_{evt}/bw",true);
	FILLSINGLE(mll);




}

}//namespace

