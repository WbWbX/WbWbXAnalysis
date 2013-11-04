/*
 * ttbarControlPlots.cc
 *
 *  Created on: Nov 1, 2013
 *      Author: kiesej
 */

#include "../interface/ttbarControlPlots.h"
#include "../interface/NTFullEvent.h"

#include "TtZAnalysis/DataFormats/src/classes.h"

namespace ztop{
/**
 * setbins, addplot, FILL
 */
void ttbarControlPlots::makeControlPlots(const size_t & step){
	initStep(step);
	using namespace std;


	///LEPTONS

	SETBINS << -2.5 << -2.1 << -1.47 << -0.8 << 0.8 << 1.47 << 2.1 << 2.5;
	addPlot("electron eta", "#eta_{l}","N_{e}");
	FILLFOREACH(isoelectrons,eta());


	SETBINSRANGE(50,0,1);
	addPlot("electron isolation", "Iso", "N_{e}");
	FILLFOREACH(idelectrons,rhoIso());

	SETBINS << 0 << 10 << 20 << 25 << 30 << 35 << 40 << 45 << 50 << 60 << 70 << 100 << 200;
	addPlot("electron pt", "p_{T} [GeV]", "N_{e}");
	FILLFOREACH(isoelectrons,pt());

	SETBINSRANGE(50,0,1);
	addPlot("electron mva id", "Id", "N_{e}");
	FILLFOREACH(kinelectrons,mvaId());


	SETBINSRANGE(10,0,10);
	addPlot("leadlep-secleadlep dR", "dR", "N_{e}*N_{#mu}/bw",true);
	FILLSINGLE(leplepdr);

	SETBINS << -2.4 << -2.1 << -1.47 << -0.8 << 0.8 << 1.47 << 2.1 << 2.4;
	addPlot("muon eta", "#eta_{l}","N_{#mu}");
	FILLFOREACH(isomuons,eta());

	SETBINS << 0 << 10 << 20 << 25 << 30 << 35 << 40 << 45 << 50 << 60 << 70 << 100 << 200;
	addPlot( "muon pt", "p_{T} [GeV]", "N_{#mu}");
	FILLFOREACH(isomuons,pt());

	SETBINSRANGE(50,0,1);
	addPlot("muon isolation", "Iso", "N_{#mu}");
	FILLFOREACH(idmuons,isoVal());

	SETBINS << 0 << 10 << 20 << 25 << 30 << 35 << 40 << 45 << 50 << 60 << 70 << 100 << 200;
	addPlot("leadLep pt", "p_{T} [GeV]", "N_{l}");
	FILL(leadinglep,pt());

	SETBINS << 0 << 10 << 20 << 25 << 30 << 35 << 40 << 45 << 50 << 60 << 70 << 100 << 200;
	addPlot("secLeadLep pt", "p_{T} [GeV]", "N_{l}");
	FILL(secleadinglep,pt());

	SETBINSRANGE(30,0,0.01);
	addPlot("lept d0V", "d0V [cm]", "N_{l}");
	FILLFOREACH(allleptons,d0V());


	////JETS & MET

	SETBINSRANGE(8,-0.5,7.5);
	addPlot("hard jet multi", "N_{jet}","N_{evt}");
	FILL(hardjets,size());
	addPlot("loose jet multi", "N_{jet}", "N_{evt}");
	FILL(medjets,size());

	SETBINSRANGE(5,-0.5,4.5);
	addPlot("hard b jet multi", "N_{bjet}","N_{evt}",true);
	FILL(hardbjets,size());
	addPlot("loose b jet multi","N{bjet}","N_{evt}",true);
	FILL(medbjets,size());

	SETBINSRANGE(40,0,200);
	addPlot("met adjusted","E_{T}^{miss}","N_{evt}/bw");
	FILL(adjustedmet,met());

	//Combined vars

	SETBINSRANGE(120,0,240);
	addPlot("mll Range","m_{ll}[GeV]","N_{evt}/bw",true);
	FILLSINGLE(mll);

	SETBINSRANGE(40,0,300);
	addPlot("HT","H_{T}","N_{evt}/bw");
	FILLSINGLE(ht);

	SETBINSRANGE(20,60,300);
	addPlot("pt llj","p_{T}(llj)","N_{evt}/bw");
	FILLSINGLE(ptllj);

	SETBINSRANGE(14,-6.283,6.283);
	addPlot("dphi ((ll),j)","#Delta#phi(ll,j)","N_{evt}/bw");
	FILLSINGLE(dphillj);

	SETBINSRANGE(70,0,2000);
	addPlot("top discr","D_{top}","N_{evt}/bw");
	FILLSINGLE(topdiscr);

	///EVT VARS

	SETBINSRANGE(50,0,50);
	addPlot("vertex multiplicity", "n_{vtx}", "N_{evt}");
	FILL(event,vertexMulti());

}

}//namespace

