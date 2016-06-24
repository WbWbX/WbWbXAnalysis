/*
 * wControlPlots.cc
 *
 *  Created on: 30 May 2016
 *      Author: jkiesele
 */




#include "../interface/wControlPlots.h"
#include "../interface/analysisPlotsW.h"
#include "TtZAnalysis/DataFormats/src/classes.h"

namespace ztop{

void wControlPlots::makeControlPlotsPriv(){

	using namespace std;




//reco plots
	SETBINSRANGE(41,-0.5,40.5);
	addPlot("vertex multi","Vertex multiplicity", "Events");
	FILL(event,vertexMulti());

	SETBINSRANGE(120,-30,30);
	addPlot("event weight","Weight", "Events");
	if(event()->puweight)last()->fill(*(event()->puweight),fabs(*event()->puweight));

	SETBINSRANGE(200,-3,3);
	addPlot("event weight narrow","Weight", "Events");
	if(event()->puweight)last()->fill(*(event()->puweight),fabs(*event()->puweight));

	SETBINSRANGE(40,0,400);
	addPlot("good muon pt","p_{T} [GeV]", "N_{#mu}");
	FILLFOREACH(allmuons,pt());


	SETBINSRANGE(5,-0.5,4.5);
	addPlot("good muon size","N_{#mu}", "Events");
	FILL(allmuons,size());


	SETBINSRANGE(40,0,400);
	addPlot("kin muon pt","p_{T} [GeV]", "N_{#mu}");
	FILLFOREACH(kinmuons,pt());

	SETBINSRANGE(40,-2.5,2.5);
	addPlot("kin muon eta","#eta", "N_{#mu}");
	FILLFOREACH(kinmuons,eta());

	SETBINSRANGE(2,-.5,1.5);
	addPlot("kin muon id","ID", "N_{#mu}");
	FILLFOREACH(kinmuons,isTightID());

	SETBINSRANGE(50,0,3);
	addPlot("kin muon iso","Muon isolation", "N_{#mu}");
	FILLFOREACH(kinmuons,isoVal());


	SETBINSRANGE(100,0,0.1);
	addPlot("kin muon d0","d_{0} [cm]", "N_{#mu}");
	FILLFOREACH(kinmuons,d0V());


	SETBINSRANGE(5,-0.5,4.5);
	addPlot("iso muon size","N_{#mu}", "Events");
	FILL(isomuons,size());

	SETBINSRANGE(40,0,400);
	addPlot("iso muon pt","Muon p_{T} [GeV]", "N_{#mu}");
	FILLFOREACH(isomuons,pt());

	SETBINSRANGE(40,-2.5,2.5);
	addPlot("iso muon eta","Muon #eta", "N_{#mu}");
	FILLFOREACH(isomuons,eta());

	SETBINSRANGE(40,-3.1415,3.1415);
	addPlot("iso muon phi","Muon #phi", "N_{#mu}");
	FILLFOREACH(isomuons,phi());

	SETBINSRANGE(5,-1.5,1.5);
	addPlot("iso muon charge","Charge", "N_{#mu}");
	FILLFOREACH(isomuons,q());

	SETBINSRANGE(2,-.5,1.5);
	addPlot("iso muon id","ID", "N_{#mu}");
	FILLFOREACH(isomuons,isTightID());

	SETBINSRANGE(50,0,3);
	addPlot("iso muon iso","Muon isolation", "N_{#mu}");
	FILLFOREACH(isomuons,isoVal());

	SETBINSRANGE(5,-0.5,4.5);
	addPlot("noniso muon size","N_{#mu}", "Events");
	FILL(nonisomuons,size());

	SETBINSRANGE(50,0,3);
	addPlot("noniso muon iso","Muon isolation", "N_{#mu}");
	FILLFOREACH(nonisomuons,isoVal());

	SETBINSRANGE(5,-0.5,4.5);
	addPlot("veto muon size","N_{#mu}", "Events");
	FILL(vetomuons,size());

	SETBINSRANGE(60,0,300);
	addPlot("veto muon pt","Muon p_{T} [GeV]", "N_{#mu}");
	FILLFOREACH(vetomuons,pt());

	/*
	SETBINSRANGE(8,-.5,7.5);
	addPlot("id jet multi","N_{j}", "Events");
	FILL(idjets,size());

	SETBINSRANGE(60,0,300);
	addPlot("id jet pt","p_{T} [GeV]", "N_{j}");
	FILLFOREACH(idjets,pt());

	SETBINSRANGE(40,-4,4);
	addPlot("id jet eta","#eta", "N_{j}");
	FILLFOREACH(idjets,eta());
	 */

	SETBINSRANGE(8,-.5,7.5);
	addPlot("hard jet multi","Jet multiplicity", "Events");
	FILL(hardjets,size());

	SETBINSRANGE(60,0,300);
	addPlot("hard jet pt","Jet p_{T} [GeV]", "N_{j}");
	FILLFOREACH(hardjets,pt());

	SETBINSRANGE(40,-4,4);
	addPlot("hard jet eta","Jet #eta", "N_{j}");
	FILLFOREACH(hardjets,eta());

	SETBINSRANGE(40,-3.1415,3.1415);
	addPlot("hard jet phi","Jet #phi", "N_{j}");
	FILLFOREACH(hardjets,phi());

	SETBINSRANGE(60,0,300);
	addPlot("leading jet pt","Leading jet p_{T} [GeV]", "Events");
	FILL(leadingjet,pt());

	SETBINSRANGE(40,-4,4);
	addPlot("leading jet eta","Leading et #eta", "Events");
	FILL(leadingjet,eta());

	SETBINSRANGE(40,-1.4,1.4);
	addPlot("pttrans full deta cp","2 p_{T}^{T} / M_{W}", "Events");
	if(event()->leadinglep && event()->leadingjet){
		float val=analysisPlotsW::asymmValue(event()->leadinglep,event()->leadingjet);
		last()->fill(val,* event()->puweight);
	}

	SETBINS << -1.5 << -0.5 << 0.5 << 1.5;
	addPlot("pttrans full deta coarse","2 p_{T}^{T} / M_{W}", "Events");
	if(event()->leadinglep && event()->leadingjet){
		float val=analysisPlotsW::asymmValue(event()->leadinglep,event()->leadingjet);
		last()->fill(val,* event()->puweight);
	}
	float deta=0;
	if(event()->leadinglep && event()->leadingjet){
		deta=fabs(event()->leadinglep->eta()-event()->leadingjet->eta());
	}
	addPlot("pttrans deta geq 1 coarse","2 p_{T}^{T} / M_{W}", "Events");
	if(deta > 1.){
		float val=analysisPlotsW::asymmValue(event()->leadinglep,event()->leadingjet);
		last()->fill(val,* event()->puweight);
	}
	addPlot("pttrans full geq 2 coarse","2 p_{T}^{T} / M_{W}", "Events");
	if(deta > 2.){
		float val=analysisPlotsW::asymmValue(event()->leadinglep,event()->leadingjet);
		last()->fill(val,* event()->puweight);
	}
	addPlot("pttrans full geq 2.5 coarse","2 p_{T}^{T} / M_{W}", "Events");
	if(deta > 2.5){
		float val=analysisPlotsW::asymmValue(event()->leadinglep,event()->leadingjet);
		last()->fill(val,* event()->puweight);
	}

	bool isgood=event()->puweight && event()->pttrans && deta;
	bool pos=false;
	if(isgood)pos=*event()->pttrans>0;

	float adaptedweight=0;
	if(isgood) adaptedweight=*event()->puweight;
	if(!pos)
		adaptedweight*=-1;
	float fabspttrans=0;
	if(isgood) fabspttrans= fabs(*event()->pttrans);

	SETBINS << 0 << 0.5 << 2 << 2.5 << 4 << 4.5 << 6 << 6.5 << 8 << 8.5 << 10;
	addPlot("asymm full deta","#pm2p_{T}^{T} / M_{W}", "#DeltaEvents");
	if(isgood){
		if(deta>= 0. && deta < 1.)
			last()->fill(fabspttrans,adaptedweight);
		else if( deta < 2.)
			last()->fill(fabspttrans+2,adaptedweight);
		else if( deta < 3.)
			last()->fill(fabspttrans+4,adaptedweight);
		else if( deta < 4.)
			last()->fill(fabspttrans+6,adaptedweight);
		else if( deta < 6.)
			last()->fill(fabspttrans+8,adaptedweight);

	}

	SETBINS << 0 << 2;
	addPlot("asymm full inclusive","", "#DeltaEvents");
	if(isgood)
		last()->fill(fabspttrans,adaptedweight);




}


}
