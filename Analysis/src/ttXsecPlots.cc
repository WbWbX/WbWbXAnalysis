/*
 * ttXsecPlots.cc
 *
 *  Created on: Oct 16, 2014
 *      Author: kiesej
 */


#include "../interface/ttXsecPlots.h"
#include "../interface/NTFullEvent.h"
#include "TtZAnalysis/DataFormats/interface/mathdefs.h"

#include "TtZAnalysis/DataFormats/src/classes.h"

/*
 * These plots here are NOT used to extract the cross section!
 * They are just additional control plots etc.
 */

namespace ztop{

void ttXsecPlots::makeControlPlots(const size_t& step){
	if(!switchedon_) return;

	initStep(step);
	if(limittostep_>-1 && (size_t)limittostep_!=step)
		return;

	size_t nbjets=0;
	size_t naddjets=0;
	if(event()->selectedbjets && event()->selectedjets){
		nbjets=event()->selectedbjets->size();
		naddjets=event()->selectedjets->size() - nbjets;
	}

	setJetCategory(nbjets,naddjets);


	SETBINSRANGE(15,20,300);
	addPlot("mll 0 b-jets","m_{ll} [GeV]","Events/GeV");
	if(nbjets==0 || nbjets>2)
		FILLSINGLE(mll);

	SETBINSRANGE(15,20,300);
	addPlot("mll 1 b-jets","m_{ll} [GeV]","Events/GeV");
	if(nbjets==1)
		FILLSINGLE(mll);

	SETBINSRANGE(15,20,300);
	addPlot("mll 2 b-jets","m_{ll} [GeV]","Events/GeV");
	if(nbjets==2)
		FILLSINGLE(mll);



	//end more dimensions

	SETBINS << 20 << 30 << 40 << 50 << 60 << 70 << 90 << 120 << 200;
	addPlot("lead lep pt 0 b-jets","leading lept. p_{T} [GeV]","Events/GeV");
	if(nbjets==0 || nbjets>2)
		FILL(leadinglep,pt());


	addPlot("lead lep pt 1 b-jets","leading lept. p_{T} [GeV]","Events/GeV");
	if(nbjets==1)
		FILL(leadinglep,pt());

	addPlot("lead lep pt 2 b-jets","leading lept. p_{T} [GeV]","Events/GeV");
	if(nbjets==2)
		FILL(leadinglep,pt());

	////end more dimensions

	SETBINS << 20 << 25 << 30 << 35 << 40 << 50 << 60  << 80 << 200;
	addPlot("sec lep pt 0 b-jets","2^{nd} lepton p_{T} [GeV]","Events/GeV");
	if(nbjets==0 || nbjets>2)
		FILL(secleadinglep,pt());

	SETBINS << 20 << 25 << 30 << 35 << 40 << 50 << 60  << 80 << 200;
	addPlot("sec lep pt 1 b-jets","2^{nd} lepton p_{T} [GeV]","Events/GeV");
	if(nbjets==1)
		FILL(secleadinglep,pt());

	SETBINS << 20 << 25 << 30 << 35 << 40 << 50 << 60  << 80 << 200;
	addPlot("sec lep pt 2 b-jets","2^{nd} lepton p_{T} [GeV]","Events/GeV");
	if(nbjets==2)
		FILL(secleadinglep,pt());

	SETBINS << 20 << 35 << 50 << 65 << 80 << 95 << 110 << 125 << 140 << 155 << 250;
	addPlot("m_lb min 0 b-jets","m_{lb}^{min} [GeV]","Events/GeV");
	if(nbjets==0 || nbjets>2)
		FILLSINGLE(mlbmin);
	addPlot("m_lb min 1 b-jets","m_{lb}^{min} [GeV]","Events/GeV");
	if(nbjets==1)
		FILLSINGLE(mlbmin);
	addPlot("m_lb min 2 b-jets","m_{lb}^{min} [GeV]","Events/GeV");
	if(nbjets==2)
		FILLSINGLE(mlbmin);

	SETBINSRANGE(4,-0.5,3.5);
	addPlot("selected b-jet multi 0 b-jets","b jet multiplicity","Events",true);
	if(nbjets==0 || nbjets>2)
		FILL(selectedbjets,size());
	addPlot("selected b-jet multi 1 b-jets","b jet multiplicity","Events",true);
	if(nbjets==1)
		FILL(selectedbjets,size());
	addPlot("selected b-jet multi 2 b-jets","b jet multiplicity","Events",true);
	if(nbjets==2)
		FILL(selectedbjets,size());

}

}
