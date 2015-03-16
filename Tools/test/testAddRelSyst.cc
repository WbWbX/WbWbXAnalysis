/*
 * testAddRelSyst.cc
 *
 *  Created on: Aug 6, 2014
 *      Author: kiesej
 */



#include "TtZAnalysis/Tools/interface/histo1D.h"
#include "TCanvas.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"


int main(){
	using namespace ztop;

	TCanvas *c = new TCanvas();


	std::vector<float> bins;
	bins << 0 << 1 << 2 << 3 << 4 << 5;

	histo1D nominal(bins);


	for(size_t i=0;i<=nominal.getNBins();i++){
		nominal.setBinContent(i,1.+i);
		nominal.setBinStat(i,0);
	}
	nominal.addErrorContainer("sys_up",nominal*1.05);
	nominal.addErrorContainer("sys_down",nominal*0.95);

	nominal.drawFullPlot("");
	c->Print("testRelSys.pdf(");

	histo1D nominal2=nominal;
	nominal.removeAllSystematics();
	nominal.addRelSystematicsFrom(nominal2);
	nominal.drawFullPlot("");
	c->Print("testRelSys.pdf");
	histo1D::debug=true;
	nominal2.renameSyst("sys","sys2");
	nominal.addRelSystematicsFrom(nominal2);
	for(size_t i=0;i<=nominal.getNBins();i++){
		nominal2.setBinContent(i,3);
		nominal2.setBinStat(i,0.1);
	}
	nominal2.addErrorContainer("sys3_up",nominal2*1.10);
	nominal2.addErrorContainer("sys3_down",nominal2*0.99);
	nominal.addRelSystematicsFrom(nominal2);
	nominal.drawFullPlot("");
	c->Print("testRelSys.pdf");
	nominal *= 2.;
	nominal.drawFullPlot("");
	c->Print("testRelSys.pdf)");
	return 0;
}
