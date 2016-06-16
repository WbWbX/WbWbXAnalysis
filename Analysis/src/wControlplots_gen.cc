/*
 * wControlplots_gen.cc
 *
 *  Created on: 15 Jun 2016
 *      Author: jkiesele
 */




#include "../interface/wControlplots_gen.h"
#include "TtZAnalysis/DataFormats/src/classes.h"

namespace ztop{
void wControlplots_gen::makeControlPlotsPriv(){
	using namespace std;

	bool isgood=  event()->genW && event()->puweight;
	NTGenParticle * W=event()->genW;


	SETBINSRANGE(60,-1.1,1.1);
	addPlot("A7 W pt 0-10","A7","Events");
	if(isgood && W->pt() < 10)
		FILLSINGLE(A7);

	addPlot("A7 W pt 10-20","A7","Events");
	if(isgood && W->pt()>=10 && W->pt() < 20)
		FILLSINGLE(A7);

	addPlot("A7 W pt 20-30","A7","Events");
	if(isgood && W->pt()>=20 && W->pt() < 30)
		FILLSINGLE(A7);

	addPlot("A7 W pt 30-40","A7","Events");
	if(isgood && W->pt()>=30 && W->pt() < 40)
		FILLSINGLE(A7);

	addPlot("A7 W pt 40-60","A7","Events");
	if(isgood && W->pt()>=40 && W->pt() < 60)
		FILLSINGLE(A7);

	isgood = isgood && event()->genjets;
	isgood = isgood && event()->genjets->size();

	NTGenJet * jet=0;
	if(isgood && event()->genjets && event()->genjets->size()>0) jet=event()->genjets->at(0);
	float deta=0;
	isgood = isgood && jet;
	if(isgood)
		deta=fabs(jet->eta()-W->eta());

	addPlot("A7 W deta 0-0.5","A7","Events");
	if(isgood && deta < 0.5)
		FILLSINGLE(A7);

	addPlot("A7 W deta 0.5-1","A7","Events");
	if(isgood && deta >= 0.5 && deta < 1)
		FILLSINGLE(A7);

	addPlot("A7 W deta 1-1.5","A7","Events");
	if(isgood && deta >= 1 && deta < 1.5)
		FILLSINGLE(A7);

	addPlot("A7 W deta 1.5-2","A7","Events");
	if(isgood && deta >= 1.5 && deta < 2)
		FILLSINGLE(A7);

	addPlot("A7 W deta 2-2.5","A7","Events");
	if(isgood && deta >= 2 && deta < 2.5)
		FILLSINGLE(A7);

	addPlot("A7 W deta 2.5-3","A7","Events");
	if(isgood && deta >= 2.5 && deta < 3)
		FILLSINGLE(A7);

	addPlot("A7 W deta 3-3.5","A7","Events");
	if(isgood && deta >= 3 && deta < 3.5)
		FILLSINGLE(A7);

	addPlot("A7 W deta 3.5-5","A7","Events");
	if(isgood && deta >= 3.5 && deta < 5)
		FILLSINGLE(A7);


	addPlot("pttrans gen","A7","Events");
	FILLSINGLE(pttrans_gen);


	SETBINSRANGE(10,-0.5,10.5);
	addPlot("gen jet multi","gen jet multiplicity","Events");
	FILL(genjets,size());



}


}
