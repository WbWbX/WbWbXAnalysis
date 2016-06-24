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

	bool isgood=  event()->genW && event()->puweight && event()->genjets && event()->genjets->size()>0
			&& event()->genjets->at(0)->pt()>50;
	NTGenParticle * W=event()->genW;


	SETBINSRANGE(4,0,1.1);
	addPlot("asymm dsig dA7 W pt 0-10","asymm A7","#DeltaEvents");
	if(isgood && W->pt() < 10)
		if(event()->A7){
			if(* event()->A7 >0)
				last()->fill(* event()->A7, *event()->puweight );
			else
				last()->fill(- * event()->A7, - *event()->puweight );
		}

	addPlot("asymm dsig dA7 W pt 10-20","asymm A7","#DeltaEvents");
	if(isgood && W->pt()>=10 && W->pt() < 20)
		if(event()->A7){
			if(* event()->A7 >0)
				last()->fill(* event()->A7, *event()->puweight );
			else
				last()->fill(- * event()->A7, - *event()->puweight );
		}

	addPlot("asymm dsig dA7 W pt 20-30","asymm A7","#DeltaEvents");
	if(isgood && W->pt()>=20 && W->pt() < 30)
		if(event()->A7){
			if(* event()->A7 >0)
				last()->fill(* event()->A7, *event()->puweight );
			else
				last()->fill(- * event()->A7, - *event()->puweight );
		}

	addPlot("asymm dsig dA7 W pt 30-40","asymm A7","#DeltaEvents");
	if(isgood && W->pt()>=30 && W->pt() < 40)
		if(event()->A7){
			if(* event()->A7 >0)
				last()->fill(* event()->A7, *event()->puweight );
			else
				last()->fill(- * event()->A7, - *event()->puweight );
		}

	addPlot("asymm dsig dA7 W pt 40-60","asymm A7","#DeltaEvents");
	if(isgood && W->pt()>=40 && W->pt() < 60)
		if(event()->A7){
			if(* event()->A7 >0)
				last()->fill(* event()->A7, *event()->puweight );
			else
				last()->fill(- * event()->A7, - *event()->puweight );
		}

	isgood = isgood && event()->genjets;
	isgood = isgood && event()->genjets->size();

	NTGenJet * jet=0;
	if(isgood && event()->genjets && event()->genjets->size()>0) jet=event()->genjets->at(0);
	isgood=isgood && jet->pt()>50;
	float deta=0;
	isgood = isgood && jet;
	if(isgood)
		deta=fabs(jet->eta()-W->eta());

	addPlot("asymm dsig dA7 W deta 0-0.5","asymm A7","#DeltaEvents");
	if(isgood && deta < 0.5)
		if(event()->A7){
			if(* event()->A7 >0)
				last()->fill(* event()->A7, *event()->puweight );
			else
				last()->fill(- * event()->A7, - *event()->puweight );
		}

	addPlot("asymm dsig dA7 W deta 0.5-1","asymm A7","#DeltaEvents");
	if(isgood && deta >= 0.5 && deta < 1)
		if(event()->A7){
			if(* event()->A7 >0)
				last()->fill(* event()->A7, *event()->puweight );
			else
				last()->fill(- * event()->A7, - *event()->puweight );
		}

	addPlot("asymm dsig dA7 W deta 1-1.5","asymm A7","#DeltaEvents");
	if(isgood && deta >= 1 && deta < 1.5)
		if(event()->A7){
			if(* event()->A7 >0)
				last()->fill(* event()->A7, *event()->puweight );
			else
				last()->fill(- * event()->A7, - *event()->puweight );
		}

	addPlot("asymm dsig dA7 W deta 1.5-2","asymm A7","#DeltaEvents");
	if(isgood && deta >= 1.5 && deta < 2)
		if(event()->A7){
			if(* event()->A7 >0)
				last()->fill(* event()->A7, *event()->puweight );
			else
				last()->fill(- * event()->A7, - *event()->puweight );
		}

	addPlot("asymm dsig dA7 W deta 2-2.5","asymm A7","#DeltaEvents");
	if(isgood && deta >= 2 && deta < 2.5)
		if(event()->A7){
			if(* event()->A7 >0)
				last()->fill(* event()->A7, *event()->puweight );
			else
				last()->fill(- * event()->A7, - *event()->puweight );
		}

	addPlot("asymm dsig dA7 W deta 2.5-3","asymm A7","#DeltaEvents");
	if(isgood && deta >= 2.5 && deta < 3)
		if(event()->A7){
			if(* event()->A7 >0)
				last()->fill(* event()->A7, *event()->puweight );
			else
				last()->fill(- * event()->A7, - *event()->puweight );
		}

	addPlot("asymm dsig dA7 W deta 3-3.5","asymm A7","#DeltaEvents");
	if(isgood && deta >= 3 && deta < 3.5)
		if(event()->A7){
			if(* event()->A7 >0)
				last()->fill(* event()->A7, *event()->puweight );
			else
				last()->fill(- * event()->A7, - *event()->puweight );
		}

	addPlot("asymm dsig dA7 W deta 3.5-5","asymm A7","#DeltaEvents");
	if(isgood && deta >= 3.5 && deta < 5)
		if(event()->A7){
			if(* event()->A7 >0)
				last()->fill(* event()->A7, *event()->puweight );
			else
				last()->fill(- * event()->A7, - *event()->puweight );
		}

	SETBINSRANGE(20,-1.1,1.1);
	addPlot("dsig dA7 W deta 0-1","sin#phising#theta","Events");
	if(isgood && deta < 1)
		FILLSINGLE(A7);
	addPlot("dsig dA7 W deta 1-2","sin#phisin#theta","Events");
	if(isgood && deta >=1. && deta < 2.)
		FILLSINGLE(A7);
	addPlot("dsig dA7 W deta 2-3","sin#phisin#theta","Events");
	if(isgood && deta >=2. && deta < 3.)
		FILLSINGLE(A7);
	addPlot("dsig dA7 W deta 3-4","sin#phisin#theta","Events");
	if(isgood && deta >=3. && deta < 4.)
		FILLSINGLE(A7);
	addPlot("dsig dA7 W deta 4-5","sin#phisin#theta","Events");
	if(isgood && deta >=4. && deta < 5.)
		FILLSINGLE(A7);

	addPlot("dsig dA7 W all deta","sin#phisin#theta","Events");
	FILLSINGLE(A7);

	addPlot("pttrans gen","pttrans(gen)","Events");
	FILLSINGLE(pttrans_gen);

	SETBINSRANGE(100,0,200);
	addPlot("W pt","p_{T}^{W} [GeV]","Events");
	FILL(genW,pt());


	SETBINSRANGE(10,-0.5,10.5);
	addPlot("gen jet multi","gen jet multiplicity","Events");
	FILL(genjets,size());

	jet=0;
	if(event()->genjets && event()->genjets->size()>0)
		jet=event()->genjets->at(0);

	SETBINSRANGE(120,0,5);
	addPlot("dr lep genjet","#DeltaR(l,j)","Events");
	if(jet && event()->genlepton && event()->puweight)
		last()->fill(dR(jet,event()->genlepton),* event()->puweight );

	SETBINSRANGE(100,0,5);
	addPlot("W mass","m_{l#nu} [GeV]","Events");
	FILL(genW,m());


	SETBINSRANGE(120,0,10);
	addPlot("W mass low","m_{l#nu} [GeV]","Events",false);
	FILL(genW,m());

}


}
