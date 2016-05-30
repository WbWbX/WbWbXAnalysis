/*
 * wControlPlots.cc
 *
 *  Created on: 30 May 2016
 *      Author: jkiesele
 */




#include "../interface/wControlPlots.h"
#include "TtZAnalysis/DataFormats/src/classes.h"

namespace ztop{

void wControlPlots::makeControlPlotsPriv(){

	using namespace std;

	SETBINSRANGE(500,-1000,1000);
	addPlot("event weight","Weight", "Events");
	FILLSINGLE(puweight);

	SETBINSRANGE(500,-2,2);
	addPlot("event weight narrow","Weight", "Events");
	FILLSINGLE(puweight);

	SETBINSRANGE(50,0,400);
	addPlot("good muon pt","p_{T} [GeV]", "N_{#mu}");
	FILLFOREACH(allmuons,pt());


	SETBINSRANGE(5,-0.5,4.5);
	addPlot("good muon size","N_{#mu}", "Events");
	FILL(allmuons,size());


	SETBINSRANGE(50,0,400);
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

	SETBINSRANGE(50,0,400);
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

	SETBINSRANGE(50,0,300);
	addPlot("veto muon pt","Muon p_{T} [GeV]", "N_{#mu}");
	FILLFOREACH(vetomuons,pt());

	SETBINSRANGE(8,-.5,7.5);
	addPlot("id jet multi","N_{j}", "Events");
	FILL(idjets,size());

	SETBINSRANGE(50,0,300);
	addPlot("id jet pt","p_{T} [GeV]", "N_{j}");
	FILLFOREACH(idjets,pt());

	SETBINSRANGE(40,-4,4);
	addPlot("id jet eta","#eta", "N_{j}");
	FILLFOREACH(idjets,eta());

	SETBINSRANGE(8,-.5,7.5);
	addPlot("hard jet multi","Jet multiplicity", "Events");
	FILL(hardjets,size());

	SETBINSRANGE(50,0,300);
	addPlot("hard jet pt","Jet p_{T} [GeV]", "N_{j}");
	FILLFOREACH(hardjets,pt());

	SETBINSRANGE(40,-4,4);
	addPlot("hard jet eta","Jet #eta", "N_{j}");
	FILLFOREACH(hardjets,eta());

	SETBINSRANGE(40,-3.1415,3.1415);
	addPlot("hard jet phi","Jet #phi", "N_{j}");
	FILLFOREACH(hardjets,phi());



}


}
