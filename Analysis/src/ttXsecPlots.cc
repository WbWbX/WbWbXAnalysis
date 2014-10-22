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


	/*
	 *
	 * NJets inclusive
	 *
	 */
	SETBINSRANGE(1,0,1);
	addPlot("total 0,0 b-jets","m_{ll} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_0bjet0jet && event()->puweight)
		last()->fill(0.5,*event()->puweight);
	addPlot("total 0,1 b-jets","m_{ll} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_0bjet1jet && event()->puweight)
		last()->fill(0.5,*event()->puweight);
	addPlot("total 0,2 b-jets","m_{ll} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_0bjet2jet && event()->puweight)
		last()->fill(0.5,*event()->puweight);
	addPlot("total 0,3 b-jets","m_{ll} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_0bjet3jet && event()->puweight)
		last()->fill(0.5,*event()->puweight);

	addPlot("total 1,0 b-jets","m_{ll} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_1bjet0jet && event()->puweight)
		last()->fill(0.5,*event()->puweight);
	addPlot("total 1,1 b-jets","m_{ll} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_1bjet1jet && event()->puweight)
		last()->fill(0.5,*event()->puweight);
	addPlot("total 1,2 b-jets","m_{ll} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_1bjet2jet && event()->puweight)
		last()->fill(0.5,*event()->puweight);
	addPlot("total 1,3 b-jets","m_{ll} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_1bjet3jet && event()->puweight)
		last()->fill(0.5,*event()->puweight);

	addPlot("total 2,0 b-jets","m_{ll} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_2bjet0jet && event()->puweight)
		last()->fill(0.5,*event()->puweight);
	addPlot("total 2,1 b-jets","m_{ll} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_2bjet1jet && event()->puweight)
		last()->fill(0.5,*event()->puweight);
	addPlot("total 2,2 b-jets","m_{ll} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_2bjet2jet && event()->puweight)
		last()->fill(0.5,*event()->puweight);
	addPlot("total 2,3 b-jets","m_{ll} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_2bjet3jet && event()->puweight)
		last()->fill(0.5,*event()->puweight);


	/*
	 *
	 *  MLL
	 *
	 */
	SETBINSRANGE(15,20,300);
	addPlot("mll 0,0 b-jets","m_{ll} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_0bjet0jet)
		FILLSINGLE(mll);
	addPlot("mll 0,1 b-jets","m_{ll} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_0bjet1jet)
		FILLSINGLE(mll);
	addPlot("mll 0,2 b-jets","m_{ll} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_0bjet2jet)
		FILLSINGLE(mll);
	addPlot("mll 0,3 b-jets","m_{ll} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_0bjet3jet)
		FILLSINGLE(mll);

	addPlot("mll 1,0 b-jets","m_{ll} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_1bjet0jet)
		FILLSINGLE(mll);
	addPlot("mll 1,1 b-jets","m_{ll} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_1bjet1jet)
		FILLSINGLE(mll);
	addPlot("mll 1,2 b-jets","m_{ll} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_1bjet2jet)
		FILLSINGLE(mll);
	addPlot("mll 1,3 b-jets","m_{ll} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_1bjet3jet)
		FILLSINGLE(mll);

	addPlot("mll 2,0 b-jets","m_{ll} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_2bjet0jet)
		FILLSINGLE(mll);
	addPlot("mll 2,1 b-jets","m_{ll} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_2bjet1jet)
		FILLSINGLE(mll);
	SETBINSRANGE(10,20,300);
	addPlot("mll 2,2 b-jets","m_{ll} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_2bjet2jet)
		FILLSINGLE(mll);
	SETBINSRANGE(5,20,300);
	addPlot("mll 2,3 b-jets","m_{ll} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_2bjet3jet)
		FILLSINGLE(mll);

	/*
	 *
	 *  LEADING LEPTON PT
	 *
	 */
	SETBINS << 20 << 25 << 30 << 35  << 40 << 50 << 60 << 70 << 90 << 120 << 200;
	addPlot("lead lep pt 0,0 b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_0bjet0jet)
		FILL(leadinglep,pt());
	addPlot("lead lep pt 0,1 b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_0bjet1jet)
		FILL(leadinglep,pt());
	addPlot("lead lep pt 0,2 b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_0bjet2jet)
		FILL(leadinglep,pt());
	addPlot("lead lep pt 0,3 b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_0bjet3jet)
		FILL(leadinglep,pt());
	addPlot("lead lep pt 1,0 b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_1bjet0jet)
		FILL(leadinglep,pt());
	addPlot("lead lep pt 1,1 b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_1bjet1jet)
		FILL(leadinglep,pt());
	addPlot("lead lep pt 1,2 b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_1bjet2jet)
		FILL(leadinglep,pt());
	addPlot("lead lep pt 1,3 b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_1bjet3jet)
		FILL(leadinglep,pt());
	addPlot("lead lep pt 2,0 b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_2bjet0jet)
		FILL(leadinglep,pt());
	addPlot("lead lep pt 2,1 b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_2bjet1jet)
		FILL(leadinglep,pt());
	SETBINS << 20  << 30   << 40 << 50 << 60 << 70 << 90 << 120 << 200;
	addPlot("lead lep pt 2,2 b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_2bjet2jet)
		FILL(leadinglep,pt());
	SETBINS << 20 << 35 << 50 << 60  << 90 << 120 << 200;
	addPlot("lead lep pt 2,3 b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_2bjet3jet)
		FILL(leadinglep,pt());


	/*
	 *
	 * Extras that might be interesting
	 *
	 */
	SETBINSRANGE(30,0,M_PI);
	addPlot("dphi ((ll),j) 0,1 b-jets","#Delta#phi(ll,j)","N_{evt}/GeV");
	if(jetcategory == cat_0bjet1jet)
		FILLSINGLE(dphillj);
	addPlot("dphi ((ll),j) 1,0 b-jets","#Delta#phi(ll,j)","N_{evt}/GeV");
	if(jetcategory == cat_1bjet0jet)
		FILLSINGLE(dphillj);

	SETBINS << -2.4 << -2.1 << -1.47 << -0.8 << 0.8 << 1.47 << 2.1 << 2.4;
	addPlot("second jet eta 0,2 b-jets", "#eta_{j}","N_{j}");
	if(jetcategory == cat_0bjet2jet)
		FILL(selectednonbjets->at(1),eta());
	addPlot("second jet eta 0,3 b-jets", "#eta_{j}","N_{j}");
	if(jetcategory == cat_0bjet3jet)
		FILL(selectednonbjets->at(1),eta());
	addPlot("second jet eta 1,2 b-jets", "#eta_{j}","N_{j}");
	if(jetcategory == cat_1bjet2jet)
		FILL(selectednonbjets->at(1),eta());
	addPlot("second jet eta 1,3 b-jets", "#eta_{j}","N_{j}");
	if(jetcategory == cat_1bjet3jet)
		FILL(selectednonbjets->at(1),eta());
	addPlot("second jet eta 2,2 b-jets", "#eta_{j}","N_{j}");
	if(jetcategory == cat_2bjet2jet)
		FILL(selectednonbjets->at(1),eta());
	addPlot("second jet eta 2,3 b-jets", "#eta_{j}","N_{j}");
	if(jetcategory == cat_2bjet3jet)
		FILL(selectednonbjets->at(1),eta());


	SETBINS << 20 << 25 << 30 << 35  << 40 << 50 << 60 << 70 << 80 << 90 << 100 << 120 << 140 << 160 << 200;
	addPlot("lead jet pt 0,1 b-jets", "p_{t}^{j}","N_{j}");
	if(jetcategory == cat_0bjet1jet)
		FILL(selectednonbjets->at(0),pt());
	addPlot("lead jet pt 0,2 b-jets", "p_{t}^{j}","N_{j}");
	if(jetcategory == cat_0bjet2jet)
		FILL(selectednonbjets->at(0),pt());
	addPlot("lead jet pt 0,3 b-jets", "p_{t}^{j}","N_{j}");
	if(jetcategory == cat_0bjet3jet)
		FILL(selectednonbjets->at(0),pt());
	addPlot("lead jet pt 1,1 b-jets", "p_{t}^{j}","N_{j}");
	if(jetcategory == cat_1bjet1jet)
		FILL(selectednonbjets->at(0),pt());
	addPlot("lead jet pt 1,2 b-jets", "p_{t}^{j}","N_{j}");
	if(jetcategory == cat_1bjet2jet)
		FILL(selectednonbjets->at(0),pt());
	addPlot("lead jet pt 1,3 b-jets", "p_{t}^{j}","N_{j}");
	if(jetcategory == cat_1bjet3jet)
		FILL(selectednonbjets->at(0),pt());
	SETBINS << 20  << 30  << 40 << 50 << 60 << 70 << 80 << 90 << 100 << 160 << 200;
	addPlot("lead jet pt 2,1 b-jets", "p_{t}^{j}","N_{j}");
	if(jetcategory == cat_2bjet1jet)
		FILL(selectednonbjets->at(0),pt());
	addPlot("lead jet pt 2,2 b-jets", "p_{t}^{j}","N_{j}");
	if(jetcategory == cat_2bjet2jet)
		FILL(selectednonbjets->at(0),pt());
	addPlot("lead jet pt 2,3 b-jets", "p_{t}^{j}","N_{j}");
	if(jetcategory == cat_2bjet3jet)
		FILL(selectednonbjets->at(0),pt());



	SETBINS << 20 << 25 << 30 << 35  << 40 << 50 << 60 << 70 << 80 << 90 << 100 << 120 << 140 << 160 << 200;
	addPlot("second jet pt 0,2 b-jets", "p_{t}^{j}","N_{j}");
	if(jetcategory == cat_0bjet2jet)
		FILL(selectednonbjets->at(1),pt());
	addPlot("second jet pt 0,3 b-jets", "p_{t}^{j}","N_{j}");
	if(jetcategory == cat_0bjet3jet)
		FILL(selectednonbjets->at(1),pt());
	addPlot("second jet pt 1,2 b-jets", "p_{t}^{j}","N_{j}");
	if(jetcategory == cat_1bjet2jet)
		FILL(selectednonbjets->at(1),pt());
	addPlot("second jet pt 1,3 b-jets", "p_{t}^{j}","N_{j}");
	if(jetcategory == cat_1bjet3jet)
		FILL(selectednonbjets->at(1),pt());
	SETBINS << 20  << 30  << 40 << 50 << 60 << 70 << 80 << 90 << 100 << 160 << 200;
	addPlot("second jet pt 2,2 b-jets", "p_{t}^{j}","N_{j}");
	if(jetcategory == cat_2bjet2jet)
		FILL(selectednonbjets->at(1),pt());
	addPlot("second jet pt 2,3 b-jets", "p_{t}^{j}","N_{j}");
	if(jetcategory == cat_2bjet3jet)
		FILL(selectednonbjets->at(1),pt());


	SETBINS << 20 << 25 << 30 << 35  << 40 << 50 << 60 << 70 << 80 << 90 << 100 << 120 << 140 << 160 << 200;
	addPlot("third jet pt 0,3 b-jets", "p_{t}^{j}","N_{j}");
	if(jetcategory == cat_0bjet3jet)
		FILL(selectednonbjets->at(2),pt());
	addPlot("third jet pt 1,3 b-jets", "p_{t}^{j}","N_{j}");
	if(jetcategory == cat_1bjet3jet)
		FILL(selectednonbjets->at(2),pt());
	SETBINS << 20  << 30  << 40 << 50 << 60 << 70 << 80 << 90 << 100 << 160 << 200;
	addPlot("third jet pt 2,3 b-jets", "p_{t}^{j}","N_{j}");
	if(jetcategory == cat_2bjet3jet)
		FILL(selectednonbjets->at(2),pt());



	SETBINS << 20 << 25 << 30 << 35  << 40 << 50 << 60 << 70 << 80 << 90 << 100 << 120 << 140 << 160 << 200;
	addPlot("lead b-jet pt 1,0 b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_1bjet0jet)
		FILL(selectedbjets->at(0),pt());
	addPlot("lead b-jet pt 1,1 b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_1bjet1jet)
		FILL(selectedbjets->at(0),pt());
	addPlot("lead b-jet pt 1,2 b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_1bjet2jet)
		FILL(selectedbjets->at(0),pt());
	addPlot("lead b-jet pt 1,3 b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_1bjet3jet)
		FILL(selectedbjets->at(0),pt());
	addPlot("lead b-jet pt 2,0 b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_2bjet0jet)
		FILL(selectedbjets->at(0),pt());
	SETBINS << 20  << 30  << 40 << 50 << 60 << 70 << 80 << 90 << 100 << 160 << 200;
	addPlot("lead b-jet pt 2,1 b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_2bjet1jet)
		FILL(selectedbjets->at(0),pt());
	addPlot("lead b-jet pt 2,2 b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_2bjet2jet)
		FILL(selectedbjets->at(0),pt());
	addPlot("lead b-jet pt 2,3 b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_2bjet3jet)
		FILL(selectedbjets->at(0),pt());


	SETBINS << 20 << 25 << 30 << 35  << 40 << 50 << 60 << 70 << 80 << 90 << 100 << 120 << 140 << 160 << 200;
	addPlot("second b-jet pt 2,0 b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_2bjet0jet)
		FILL(selectedbjets->at(1),pt());
	SETBINS << 20  << 30  << 40 << 50 << 60 << 70 << 80 << 90 << 100 << 160 << 200;
	addPlot("second b-jet pt 2,1 b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_2bjet1jet)
		FILL(selectedbjets->at(1),pt());
	addPlot("second b-jet pt 2,2 b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_2bjet2jet)
		FILL(selectedbjets->at(1),pt());
	addPlot("second b-jet pt 2,3 b-jets","p_{t}^{l1} [GeV]","N_{evt}/GeV");
	if(jetcategory == cat_2bjet3jet)
		FILL(selectedbjets->at(1),pt());


}

}
