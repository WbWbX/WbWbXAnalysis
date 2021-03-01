/*
 * testStatStuff.cc
 *
 *  Created on: Jul 17, 2014
 *      Author: kiesej
 */

#include "../interface/histo1D.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "TRandom.h"
#include <vector>
#include "TCanvas.h"
#include <cmath>
#include "../interface/plotterMultiplePlots.h"
#include "../interface/plotterCompare.h"
#include "TFile.h"
#include "TRandom3.h"


float newlogPois(float centre, float s, float evalpoint){

	//	float s=centre/(mcstat*mcstat);
	float o=evalpoint;
	float n=centre*s;

	int k=s*o;
	int lambda=n+centre;

	//float out=(int)k * log((int)lambda) - lgamma((int)k+1.) - (int)lambda;

	float out=0;
	//convolute by hand
	for(int i=0;i<centre*5;i++){

		out+=ztop::poisson(s*(o-i),n) * s*ztop::poisson(i,centre) ;
	}

	out=0;
	//out/=(s);
	return log(out);

}
/**
 * brute force:
 * produces prob density using toys.
 */
float toyMCPois(const float& centre, const float& s, const float& evalpoint, float iterations=5500){
	using namespace ztop;

	iterations*=sqrt(centre);

	int newmin=centre - 5*centre;
	if(newmin<0) newmin=0;



	float out=0;
	float nit=0;

	for(int mccentre=centre*s - 5*sqrt(centre*s);mccentre<centre*s +  5*sqrt(centre*s);mccentre++){
		if(mccentre<0)continue;
		double pois_c=ztop::poisson((float)mccentre ,centre*s);

		float newc=(mccentre/s);
		//newc=(floor(newc*(1.0f/1) + 0.5)/(1.0f/1));

		out+=ztop::poisson(evalpoint,newc) * pois_c;
		nit++;
	}

	return out;
}



int main(){

	using namespace ztop;

	std::cout << "go!" <<std::endl;
	//make pulls
	float bindiv=0.1;

	TRandom3 * random=new TRandom3();

	size_t genevents=1e5;
	double poismean=4;
	double s=10000;

	float minbin=0;
	if(poismean-5*sqrt(poismean) >0)minbin= (poismean-5*sqrt(poismean));
	float maxbin=poismean+5*sqrt(poismean);

	std::vector<float> bins;
	minbin=(int)(minbin-.2);
	maxbin=(int)maxbin;
	for(float i=minbin;i<=maxbin;i+=bindiv) bins<<i;
	for(size_t i=0;i<bins.size();i++)
	std::cout << bins.at(i) <<std::endl;

	histo1D generated(bins);
	histo1D sim(bins);
	histo1D onlypois(bins);
	histo1D newlogpois(bins);
	histo1D newtoypois(bins);
	std::vector<float> pullbins;
	pullbins<<-5<<-4<<-3<<-2<<-1<<0<<1<<2<<3<<4<<5;
	histo1D pull(pullbins);
	newtoypois.setName("P_{new} ");

	generated.setName("toys (obs==pred)");
	sim.setName("P_{#rho} (Caldwell)");
	onlypois.setName("pure poisson");



	//predict poismean
	std::vector<int> ps;
	displayStatusBar(1,bins.size(),100);
	for(float bin=0;bin<bins.size();bin++){
		float i=bins.at(bin);

		displayStatusBar(bin,bins.size(),100);
		//std::cout << bin << std::endl;
		for(size_t evt=0 ;evt<genevents;evt++){
			float trueval=random->Poisson(i);
			float mcexp=(poismean)*s;
			float mctrue=mcexp;//random->Poisson(mcexp);

			//float predoftrue=toyMCPois(trueval,s,mctrue);
			//pull.fill(mctrue,  exp(shiftedLnPoissonMCStat(poismean,sqrt(poismean), sqrt(mctrue),mctrue)) );
			if((int) (trueval) == (int) poismean)//mctrue/s)
				generated.fill(i);//,s);
		}
		//break;
	}
	bool useold=true;
	generated *= bindiv;///(float)bins.size();
	//generated*= (float)1/bins.size();
	std::cout << "making comp plots" <<std::endl;
	for(float bin=0;bin<bins.size();bin++){
		float i=bins.at(bin);
		if(useold)
			sim.fill(i,((float)genevents)*bindiv*exp(shiftedLnPoissonMCStat(poismean,sqrt(poismean),sqrt(poismean*s)/s,i)));
		else
			sim.fill(i,((float)genevents)*bindiv*exp(shiftedLnPoissonMCStat(poismean,1,sqrt(poismean*s)/s,i,false)));
		onlypois.fill(i,((float)genevents)*bindiv*ztop::poisson(i,poismean));
		newlogpois.fill(i, ((float)genevents)*bindiv*exp(newlogPois(poismean,s,i)));

		newtoypois.fill(i,((float)genevents)*toyMCPois(poismean,s,i));
	}
	sim.setAllErrorsZero();
	onlypois.setAllErrorsZero();
	newlogpois.setAllErrorsZero();
	newtoypois.setAllErrorsZero();

	plotterCompare genplotter;
	genplotter.readStyleFromFileInCMSSW("src/WbWbXAnalysis/Tools/styles/comparePlots_3MC.txt");
	TCanvas cvgen;
	genplotter.usePad(&cvgen);
	genplotter.setNominalPlot(&generated);
	genplotter.setComparePlot(&sim,0);
	//genplotter.addPlot(&onlypois);
	//	genplotter.addPlot(&pull);
	//genplotter.addPlot(&newlogpois);
	//genplotter.addPlot(&newtoypois);
	genplotter.draw();
	cvgen.Print("gen.pdf");

	histo1D c1(bins),c2(bins),c3(bins),c4(bins);
	c1.setMergeUnderFlowOverFlow(true);
	c2.setMergeUnderFlowOverFlow(true);
	c3.setMergeUnderFlowOverFlow(true);
	c4.setMergeUnderFlowOverFlow(true);
	//  c3.setMergeUnderFlowOverFlow(true);
	//  c4.setMergeUnderFlowOverFlow(true);



	//for(int j=0;j<100;j++)
	for(float bin=0;bin<bins.size();bin++){
			float i=bins.at(bin);
		/*  c1.fill(i,exp(shiftedLnPoisson(50,10,i)));
        c2.fill(i,exp(shiftedLnPoisson(50,100,i)));
        c3.fill(i,exp(shiftedLnPoisson(50,10000,i)));
        c4.fill(i,shiftedLnPoisson(50,1000000,i));
		 */
		c1.fill(i,exp(shiftedLnPoisson(5,sqrt(5),i)));
		c2.fill(i,exp(newlogPois(5,0.1,i)));
		//  c3.fill(i,exp(shiftedLnPoissonMCStat7,7,20,i)));
		c3.fill(i,exp(shiftedLnPoissonMCStat(5,1,0.0001,i,false)));
		//c4.fill(i,exp(shiftedLnPoissonMCStat(5,0.5,3.,i)));

	}


	TCanvas cv1;
	c1.setAllErrorsZero();
	c2.setAllErrorsZero();
	c3.setAllErrorsZero();
	c4.setAllErrorsZero();

	c1.setName("cont Poisson");
	c2.setName("P_{#rho} s>1");
	c3.setName("P_{#rho} s>>1");
	c4.setName("P_{#rho} s<1");

	std::cout << "1: " << c1.integral(true)*bindiv <<std::endl;
	std::cout << "2: " << c2.integral(true)*bindiv <<std::endl;
	std::cout << "3: " << c3.integral(true)*bindiv <<std::endl;
	std::cout << "4: " << c4.integral(true)*bindiv <<std::endl;
	plotterMultiplePlots::debug=false;
	plotterMultiplePlots plotter;
	plotter.readStyleFromFileInCMSSW("src/WbWbXAnalysis/Tools/styles/multiplePlots.txt");
	plotter.usePad(&cv1);

	plotter.addPlot(&c1,false);
	plotter.addPlot(&c2,false);
	plotter.addPlot(&c3,false);
	// plotter.addPlot(&c4,false);

	plotter.draw();
	TFile * f = new TFile("testStatStuff.root","RECREATE");
	cv1.Print("testStatStuff.pdf");
	cv1.Write();
	f->Close();



	//new toy stuff






















	return 0;
}
