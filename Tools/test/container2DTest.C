/*
 * container2DTest.C
 *
 *  Created on: Jul 25, 2013
 *      Author: kiesej
 */

#include "../interface/histo2D.h"
//#include "../src/histo2D.cc"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "TRandom.h"
#include "TCanvas.h"

void container2DTest(){
	using namespace std;
	using namespace ztop;

	vector<float> binsx,binsy, otherbins;
	for(float i=-8;i<12;i+=2){
		binsx << i;
		binsy << i;
	}

	otherbins << -8 << -4 << 0 << 4 << 8 << 12;

	//binsx << 4<< 6<< 7<<8;
	histo2D c1(binsx,binsy,"testcont","xaxis","yaxis");
	histo2D c2(binsx,binsy,"testcont2","xaxis","yaxis");

	TRandom * r = new TRandom(12093821);

	std::cout << "filling..." << std::endl;
	for(int i=0;i<400000;i++){
		c1.fill(r->Gaus(0,2),r->Gaus(5,1));
		c2.fill(r->Gaus(0,1),r->Gaus(5,2));
	}
	std::cout << "done filling" << std::endl;

	histo2D c3 = c1;///(c2*0.001);

	std::cout << "projecting" << std::endl;


	c3.addErrorContainer("sys_up",c2);
	c3.addErrorContainer("sys_down",c2*0.98);

	c3.getTH2D("",false,false)->Draw("colz,text,e");
	TCanvas *c=new TCanvas();
	c->cd();
	histo1D xprojection=c3.projectToX(false);

	xprojection.getTH1D("xproj")->Draw("");

	histo1D yprojection= c3.projectToY(false);
	TH1D * h=yprojection.getTH1D("yproj");
	h->SetMarkerColor(kRed);
	h->Draw("same");

	TCanvas *cc=new TCanvas();
	cc->cd();

	histo1D yslice=c3.getXSlice(c3.getBinNoX(0.5));
	yslice.getTH1D("",false)->Draw("text,e");
	c3.getXSlice(c3.getBinNoX(2.5)).getTH1D("",false)->Draw("same,text,e");


	cout << "done" <<endl;
}

