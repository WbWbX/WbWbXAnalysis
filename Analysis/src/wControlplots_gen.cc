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
	addPlot("asymm W pt 0-10","asymm","#DeltaEvents");
	if(isgood && W->pt() < 10)
		if(event()->pttrans_gen){
			if(* event()->pttrans_gen >0)
				last()->fill(* event()->pttrans_gen, *event()->puweight );
			else
				last()->fill(- * event()->pttrans_gen, - *event()->puweight );
		}

	addPlot("asymm W pt 10-20","asymm","#DeltaEvents");
	if(isgood && W->pt()>=10 && W->pt() < 20)
		if(event()->pttrans_gen){
			if(* event()->pttrans_gen >0)
				last()->fill(* event()->pttrans_gen, *event()->puweight );
			else
				last()->fill(- * event()->pttrans_gen, - *event()->puweight );
		}

	addPlot("asymm W pt 20-30","asymm","#DeltaEvents");
	if(isgood && W->pt()>=20 && W->pt() < 30)
		if(event()->pttrans_gen){
			if(* event()->pttrans_gen >0)
				last()->fill(* event()->pttrans_gen, *event()->puweight );
			else
				last()->fill(- * event()->pttrans_gen, - *event()->puweight );
		}

	addPlot("asymm W pt 30-40","asymm","#DeltaEvents");
	if(isgood && W->pt()>=30 && W->pt() < 40)
		if(event()->pttrans_gen){
			if(* event()->pttrans_gen >0)
				last()->fill(* event()->pttrans_gen, *event()->puweight );
			else
				last()->fill(- * event()->pttrans_gen, - *event()->puweight );
		}

	addPlot("asymm W pt 40-60","asymm","#DeltaEvents");
	if(isgood && W->pt()>=40 && W->pt() < 60)
		if(event()->pttrans_gen){
			if(* event()->pttrans_gen >0)
				last()->fill(* event()->pttrans_gen, *event()->puweight );
			else
				last()->fill(- * event()->pttrans_gen, - *event()->puweight );
		}

	isgood = isgood && event()->genjets;
	isgood = isgood && event()->genjets->size();

	NTGenJet * jet=0;
	if(isgood && event()->genjets && event()->genjets->size()>0) jet=event()->genjets->at(0);
	float deta=0;
	isgood = isgood && jet;
	if(isgood)
		deta=fabs(jet->eta()-W->eta());

	addPlot("asymm W deta 0-0.5","asymm","#DeltaEvents");
	if(isgood && deta < 0.5)
		if(event()->pttrans_gen){
			if(* event()->pttrans_gen >0)
				last()->fill(* event()->pttrans_gen, *event()->puweight );
			else
				last()->fill(- * event()->pttrans_gen, - *event()->puweight );
		}

	addPlot("asymm W deta 0.5-1","asymm","#DeltaEvents");
	if(isgood && deta >= 0.5 && deta < 1)
		if(event()->pttrans_gen){
			if(* event()->pttrans_gen >0)
				last()->fill(* event()->pttrans_gen, *event()->puweight );
			else
				last()->fill(- * event()->pttrans_gen, - *event()->puweight );
		}

	addPlot("asymm W deta 1-1.5","asymm","#DeltaEvents");
	if(isgood && deta >= 1 && deta < 1.5)
		if(event()->pttrans_gen){
			if(* event()->pttrans_gen >0)
				last()->fill(* event()->pttrans_gen, *event()->puweight );
			else
				last()->fill(- * event()->pttrans_gen, - *event()->puweight );
		}

	addPlot("asymm W deta 1.5-2","asymm","#DeltaEvents");
	if(isgood && deta >= 1.5 && deta < 2)
		if(event()->pttrans_gen){
			if(* event()->pttrans_gen >0)
				last()->fill(* event()->pttrans_gen, *event()->puweight );
			else
				last()->fill(- * event()->pttrans_gen, - *event()->puweight );
		}

	addPlot("asymm W deta 2-2.5","asymm","#DeltaEvents");
	if(isgood && deta >= 2 && deta < 2.5)
		if(event()->pttrans_gen){
			if(* event()->pttrans_gen >0)
				last()->fill(* event()->pttrans_gen, *event()->puweight );
			else
				last()->fill(- * event()->pttrans_gen, - *event()->puweight );
		}

	addPlot("asymm W deta 2.5-3","asymm","#DeltaEvents");
	if(isgood && deta >= 2.5 && deta < 3)
		if(event()->pttrans_gen){
			if(* event()->pttrans_gen >0)
				last()->fill(* event()->pttrans_gen, *event()->puweight );
			else
				last()->fill(- * event()->pttrans_gen, - *event()->puweight );
		}

	addPlot("asymm W deta 3-3.5","asymm","#DeltaEvents");
	if(isgood && deta >= 3 && deta < 3.5)
		if(event()->pttrans_gen){
			if(* event()->pttrans_gen >0)
				last()->fill(* event()->pttrans_gen, *event()->puweight );
			else
				last()->fill(- * event()->pttrans_gen, - *event()->puweight );
		}

	addPlot("asymm W deta 3.5-5","asymm","#DeltaEvents");
	if(isgood && deta >= 3.5 && deta < 5)
		if(event()->pttrans_gen){
			if(* event()->pttrans_gen >0)
				last()->fill(* event()->pttrans_gen, *event()->puweight );
			else
				last()->fill(- * event()->pttrans_gen, - *event()->puweight );
		}


	addPlot("pttrans gen","asymm","Events");
	FILLSINGLE(pttrans_gen);

	SETBINSRANGE(100,0,200);
	addPlot("W pt","p_{T}^{W} [GeV]","Events");
	FILL(genW,pt());


	SETBINSRANGE(10,-0.5,10.5);
	addPlot("gen jet multi","gen jet multiplicity","Events");
	FILL(genjets,size());



}


}
