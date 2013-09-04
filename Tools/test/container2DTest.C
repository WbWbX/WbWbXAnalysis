/*
 * container2DTest.C
 *
 *  Created on: Jul 25, 2013
 *      Author: kiesej
 */

#include "../interface/container2D.h"
//#include "../src/container2D.cc"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "TRandom.h"
#include "TCanvas.h"

void container2DTest(){
	using namespace std;
	using namespace ztop;

	vector<float> binsx,binsy;
	for(float i=-30;i<31;i++){
		binsx << i/10;
		binsy << i/10+5;
	}
	binsx << 8;
	container2D c1(binsx,binsy,"testcont","xaxis","yaxis");
	container2D c2(binsx,binsy,"testcont2","xaxis","yaxis");

	TRandom * r = new TRandom(12093821);

	std::cout << "filling..." << std::endl;
	for(int i=0;i<40000;i++){
		c1.fill(r->Gaus(0,2),r->Gaus(5,1));
		c2.fill(r->Gaus(0,1),r->Gaus(5,2));
	}
	std::cout << "done filling" << std::endl;

	container2D c3 = c1;///(c2*0.001);

	c3.getTH2D()->Draw("colz");

	std::cout << "projecting" << std::endl;
	TCanvas *c=new TCanvas();
	c->cd();



	container1D xprojection=c3.projectToX(false);

	xprojection.getTH1D("xproj")->Draw("");

	container1D yprojection= c3.projectToY(false);
	TH1D * h=yprojection.getTH1D("yproj");
	h->SetMarkerColor(kRed);
	h->Draw("same");

	cout << "done" <<endl;
}

