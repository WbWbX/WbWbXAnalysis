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

	/*
	SETBINSRANGE(100,-.2,.2);
	addPlot("delta pttrans A7","#Delta(sin#phisin#theta, 2 p_{T}^{T} / M_{W})","Events");
	if(event()->A7 && event()->pttrans_gen && event()->puweight){
		last()->fill(*event()->A7-*event()->pttrans_gen , *event()->puweight);
	}

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
	 */

	isgood = isgood && event()->genjets;
	isgood = isgood && event()->genjets->size();

	NTGenJet * jet=0;
	if(isgood && event()->genjets && event()->genjets->size()>0) jet=event()->genjets->at(0);
	isgood=isgood && jet->pt()>50;
	//float deta=0;
	isgood = isgood && jet;
	//if(isgood)
	//	deta=(jet->eta()-W->eta());
	/*
	addPlot("asymm dsig dA7 W deta leq -4","asymm A7","#DeltaEvents");
	if(isgood && deta < -4 && deta > -50)
		if(event()->A7){
			if(* event()->A7 >0)
				last()->fill(* event()->A7, *event()->puweight );
			else
				last()->fill(- * event()->A7, - *event()->puweight );
		}

	addPlot("asymm dsig dA7 W deta -4 -3","asymm A7","#DeltaEvents");
	if(isgood && deta >= -4 && deta < -3)
		if(event()->A7){
			if(* event()->A7 >0)
				last()->fill(* event()->A7, *event()->puweight );
			else
				last()->fill(- * event()->A7, - *event()->puweight );
		}

	addPlot("asymm dsig dA7 W deta -3 -2","asymm A7","#DeltaEvents");
	if(isgood && deta >= -3 && deta < -2)
		if(event()->A7){
			if(* event()->A7 >0)
				last()->fill(* event()->A7, *event()->puweight );
			else
				last()->fill(- * event()->A7, - *event()->puweight );
		}

	addPlot("asymm dsig dA7 W deta -2 -1","asymm A7","#DeltaEvents");
	if(isgood && deta >= -2 && deta < -1)
		if(event()->A7){
			if(* event()->A7 >0)
				last()->fill(* event()->A7, *event()->puweight );
			else
				last()->fill(- * event()->A7, - *event()->puweight );
		}

	addPlot("asymm dsig dA7 W deta -1 1","asymm A7","#DeltaEvents");
	if(isgood && deta >= -1 && deta < 1)
		if(event()->A7){
			if(* event()->A7 >0)
				last()->fill(* event()->A7, *event()->puweight );
			else
				last()->fill(- * event()->A7, - *event()->puweight );
		}

	addPlot("asymm dsig dA7 W deta 1 2","asymm A7","#DeltaEvents");
	if(isgood && deta >= 1 && deta < 2)
		if(event()->A7){
			if(* event()->A7 >0)
				last()->fill(* event()->A7, *event()->puweight );
			else
				last()->fill(- * event()->A7, - *event()->puweight );
		}

	addPlot("asymm dsig dA7 W deta 2 3","asymm A7","#DeltaEvents");
	if(isgood && deta >= 2 && deta < 3.)
		if(event()->A7){
			if(* event()->A7 >0)
				last()->fill(* event()->A7, *event()->puweight );
			else
				last()->fill(- * event()->A7, - *event()->puweight );
		}

	addPlot("asymm dsig dA7 W deta 3 4","asymm A7","#DeltaEvents");
	if(isgood && deta >= 3 && deta < 4)
		if(event()->A7){
			if(* event()->A7 >0)
				last()->fill(* event()->A7, *event()->puweight );
			else
				last()->fill(- * event()->A7, - *event()->puweight );
		}
	addPlot("asymm dsig dA7 W deta geq 4 ","asymm A7","#DeltaEvents");
	if(isgood && deta >= 4 && deta < 50)
		if(event()->A7){
			if(* event()->A7 >0)
				last()->fill(* event()->A7, *event()->puweight );
			else
				last()->fill(- * event()->A7, - *event()->puweight );
		}

	float weight=0;
	float adja7=0;
	if(isgood&& event()->A7){
		weight =*event()->puweight ;
		adja7=* event()->A7;
		if(* event()->A7 <0){
			weight*=-1;
			adja7*=-1;
		}
	}


	addPlot("asymm dsig dA7 W deta>0","asymm A7","#DeltaEvents");
	if(weight)
		if(event()->A7){
			if(* event()->A7 >0)
				last()->fill(* event()->A7, *event()->puweight );
			else
				last()->fill(- * event()->A7, - *event()->puweight );
		}
	addPlot("asymm dsig dA7 W deta<0","asymm A7","#DeltaEvents");
	if(isgood && deta < 0)
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

	 */

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

	SETBINSRANGE(160,0,160);
	addPlot("W mass","m_{l#nu} [GeV]","Events");
	FILL(genW,m());

	/*

	SETBINSRANGE(120,0,10);
	addPlot("W mass low","m_{l#nu} [GeV]","Events",false);
	FILL(genW,m());

	SETBINSRANGE(200,0,200);
	addPlot("gen lepton pt","p_{T} [GeV]","Events",true);
	FILL(genlepton, pt());

	addPlot("gen neutrino pt","p_{T} [GeV]","Events",true);
	FILL(genneutrino,pt());

	SETBINSRANGE(120,-8,8);
	addPlot("gen lepton eta","#eta","Events",true);
	FILL(genlepton, eta());

	addPlot("gen neutrino eta","#eta","Events",true);
	FILL(genneutrino, eta());

	SETBINSRANGE(120,-M_PI,M_PI);
	addPlot("gen lepton phi","#phi","Events",true);
	FILL(genlepton, phi());

	addPlot("gen neutrino phi","#phi","Events",true);
	FILL(genneutrino, phi());

	SETBINSRANGE(120,-1,2);
	addPlot("gen lepton m","m [GeV]","Events",true);
	FILL(genlepton, m());

	addPlot("gen neutrino m","m [GeV]","Events",true);
	FILL(genneutrino, m());

	 */

	SETBINSRANGE(11,-.5,10.5);
	addPlot("gen jets10 multi","Number of jets","Events",true);
	float count10=0;
	float count20=0;
	float count30=0;
	float count40=0;
	if(event()->genjets){
		for(size_t i=0;i<event()->genjets->size();i++){
			if(event()->genjets->at(i)->pt()<10)continue;
			count10++;
			if(event()->genjets->at(i)->pt()<20)continue;
			count20++;
			if(event()->genjets->at(i)->pt()<30)continue;
			count30++;
			if(event()->genjets->at(i)->pt()<40)continue;
			count40++;
		}
	}
	if(event()->puweight)
		last()->fill(count10,* event()->puweight);

	addPlot("gen jets20 multi","Number of jets","Events",true);
	if(event()->puweight)
		last()->fill(count20,* event()->puweight);

	addPlot("gen jets30 multi","Number of jets","Events",true);
	if(event()->puweight)
		last()->fill(count30,* event()->puweight);

	addPlot("gen jets40 multi","Number of jets","Events",true);
	if(event()->puweight)
		last()->fill(count40,* event()->puweight);



	addPlot("gen jets10 Wpt30 multi","Number of jets","Events",true);
	if(W && W->pt()>30 && event()->puweight)
		last()->fill(count10,* event()->puweight);


	addPlot("gen jets20 Wpt30 multi","Number of jets","Events",true);
	if(W && W->pt()>30 && event()->puweight)
		last()->fill(count20,* event()->puweight);

	addPlot("gen jets30 Wpt30 multi","Number of jets","Events",true);
	if(W && W->pt()>30 && event()->puweight)
		last()->fill(count30,* event()->puweight);

	addPlot("gen jets40 Wpt30 multi","Number of jets","Events",true);
	if(W && W->pt()>30 && event()->puweight)
		last()->fill(count40,* event()->puweight);


	addPlot("gen jets10 Wpt40 multi","Number of jets","Events",true);
	if(W && W->pt()>40 && event()->puweight)
		last()->fill(count10,* event()->puweight);


	addPlot("gen jets20 Wpt40 multi","Number of jets","Events",true);
	if(W && W->pt()>40 && event()->puweight)
		last()->fill(count20,* event()->puweight);

	addPlot("gen jets30 Wpt40 multi","Number of jets","Events",true);
	if(W && W->pt()>40 && event()->puweight)
		last()->fill(count30,* event()->puweight);

	addPlot("gen jets40 Wpt40 multi","Number of jets","Events",true);
	if(W && W->pt()>40 && event()->puweight)
		last()->fill(count40,* event()->puweight);





	addPlot("gen jets10 Wpt50 multi","Number of jets","Events",true);
	if(W && W->pt()>50 && event()->puweight)
		last()->fill(count10,* event()->puweight);


	addPlot("gen jets20 Wpt50 multi","Number of jets","Events",true);
	if(W && W->pt()>50 && event()->puweight)
		last()->fill(count20,* event()->puweight);

	addPlot("gen jets30 Wpt50 multi","Number of jets","Events",true);
	if(W && W->pt()>50 && event()->puweight)
		last()->fill(count30,* event()->puweight);

	addPlot("gen jets40 Wpt50 multi","Number of jets","Events",true);
	if(W && W->pt()>50 && event()->puweight)
		last()->fill(count40,* event()->puweight);




	addPlot("gen jets10 Wpt60 multi","Number of jets","Events",true);
	if(W && W->pt()>60 && event()->puweight)
		last()->fill(count10,* event()->puweight);


	addPlot("gen jets20 Wpt60 multi","Number of jets","Events",true);
	if(W && W->pt()>60 && event()->puweight)
		last()->fill(count20,* event()->puweight);

	addPlot("gen jets30 Wpt60 multi","Number of jets","Events",true);
	if(W && W->pt()>60 && event()->puweight)
		last()->fill(count30,* event()->puweight);

	addPlot("gen jets40 Wpt60 multi","Number of jets","Events",true);
	if(W && W->pt()>60 && event()->puweight)
		last()->fill(count40,* event()->puweight);



}


}
