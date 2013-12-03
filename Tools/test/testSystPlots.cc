/*
 * testSystPlots.cc
 *
 *  Created on: Oct 17, 2013
 *      Author: kiesej
 */

#include "../interface/container.h"
#include <vector>
#include "TRandom.h"
#include "../interface/canvasStyle.h"
#include "../interface/containerStack.h"
#include "../interface/plotter.h"

std::vector<TGraphAsymmErrors *> getVars(const ztop::container1D& c, size_t bin){

	c.coutBinContent(bin);
	std::vector<double> varsdown,varsup,statvarup,statvardown,zeros,increase;
	std::vector<TString> names;
	double idx=0;
	for(size_t i=0;i<c.getSystSize();i++){
		TString sysname=c.getSystErrorName(i);
		std::cout << "check name " << sysname <<std::endl;
		if(sysname.EndsWith("_down")){
			std::cout << c.getSystError(i,bin)/c.getBinContent(bin) << std::endl;
			varsdown.push_back(c.getSystError(i,bin)/c.getBinContent(bin));
			statvardown.push_back(c.getSystErrorStat(i,bin)/c.getSystError(i,bin)/c.getBinContent(bin));
			zeros.push_back(0);
			names.push_back(sysname.ReplaceAll("_down",""));
			increase.push_back(idx++);
		}
		else{
			std::cout << c.getSystError(i,bin)/c.getBinContent(bin) << std::endl;
			varsup.push_back(c.getSystError(i,bin)/c.getBinContent(bin));
			statvardown.push_back(c.getSystErrorStat(i,bin)/c.getSystError(i,bin)/c.getBinContent(bin));
		}

		//create graph vor var down

	}
	//new TGraphAsymmErrors(xbins,x,y,xel,xeh,yel,yeh);

	TGraphAsymmErrors * gup = new TGraphAsymmErrors(increase.size(),&zeros.at(0),&increase.at(0),&zeros.at(0),&varsup.at(0),&zeros.at(0),&zeros.at(0));
	gup->Draw("AP");
	return std::vector<TGraphAsymmErrors * >();


	TH1D h;
	h.f
}
/*
TGraphAsymmErrors * getVarUp(const container1D& c, size_t bin){



}


TGraphAsymmErrors * getStat(const container1D& c, size_t bin){



}
 */
void testSystPlots(){
	std::vector<float> bins;
	for(float i=0;i<20;i++)bins.push_back(i);

	using namespace ztop;
	container1D c1(bins),c2(bins),c3(bins),cerr;
	TRandom * r=new TRandom(123);

	for(int j=0;j<1000;j++){
		c1.fill(r->Gaus(8,4));
		c2.fill(r->Gaus(15,3));
		c3.fill(r->Gaus(14,1));
	}
	canvasStyle cstyle(canvasStyle::stackPlusRatioCanvas);
	plotter pl;
	pl.setStyle(cstyle);


	container1D fdata=c1+c2+c3;
	containerStack stack;
	// push_back(ztop::container1D, TString, int, double); //! adds container with, name, colour, norm to st
	stack.push_back(fdata,"data",kBlack,1);
	stack.push_back(c1,"signal",kRed,1);
	stack.push_back(c2,"bg1",kBlue,1);
	stack.push_back(c3,"bg2",kGreen,1);
	stack.addSignal("signal");
	stack.addSignal("bg2");

	containerStack::batchmode = false;


	c1.addErrorContainer("syst_up",c1+c2*0.1);
	c1.addErrorContainer("syst_down",c1-c2*0.1);
	c1.addErrorContainer("syst2_up",c1+c2*0.05);
	c1.addErrorContainer("syst2_down",c1-c2*0.04);
	c1.addErrorContainer("syst3_up",c1+c2*0.1);
	c1.addErrorContainer("syst3_down",c1-c2*0.08);

	c1.drawFullPlot();
	c1.getTGraph()->Draw("AP");
	/*a graph for:
	 * - variations low
	 * - variations high
	 * - stat low
	 * - stat high
	 */
	TCanvas *cv2=new TCanvas();
	getVars(c1,8);
	/*
	TGraphAsymmErrors * g = new TGraphAsymmErrors(xbins,x,y,xel,xeh,yel,yeh);

	g->Draw("AP"); */
}
