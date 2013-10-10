/*
 * fillsomecontainers.C
 *
 *  Created on: Oct 2, 2013
 *      Author: kiesej
 */

{
#include <vector>
#include "TRandom.h"
#include "../interface/canvasStyle.h"
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


	c1.addErrorContainer("syst_up",c2);
	c1.addErrorContainer("syst_down",c3);
	c1.addErrorContainer("syst2_up",c2*0.99);
	c1.addErrorContainer("syst2_down",c3*0.99);
	c1.addErrorContainer("syst3_up",c2*1.01);
	c1.addErrorContainer("syst3_down",c3*1.02);




}


