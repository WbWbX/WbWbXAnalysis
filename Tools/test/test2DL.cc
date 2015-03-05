/*
 * test2DL.cc
 *
 *  Created on: Feb 17, 2015
 *      Author: kiesej
 */




#include "TtZAnalysis/Tools/interface/applicationMainMacro.h"
#include "TtZAnalysis/Tools/interface/container2D.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "TCanvas.h"
#include "TtZAnalysis/Tools/interface/plotter2D.h"
#include "TtZAnalysis/Tools/interface/plotterMultiplePlots.h"
#include "TStyle.h"
#include "TtZAnalysis/Tools/interface/graphFitter.h"

ztop::container2D lhtochi2(const ztop::container2D& in){
	using namespace ztop;
	container2D out=in;
	out.setAllZero();

	for(size_t i=1;i<=in.getNBinsX()-1;i++){
		for(size_t j=1;j<=in.getNBinsY()-1;j++){
			out.getBin(i,j).setContent(-2 * log(in.getBin(i,j).getContent()));
		}
	}
	return out;
}

double sq(double a){
	return a*a;
}



double LH_Exp(float mtop,float xsec,ztop::graphFitter & gf, const float& errup,const float& errdown){

	double xsecmeas=gf.getFitOutput(mtop);
	double xsecerr=errup*xsecmeas;
	if(xsec<xsecmeas)
		xsecerr=xsecmeas*errdown;
	//double mtoperr=0.5;

	double lnLHxsec = sq((xsecmeas-xsec)/xsecerr)/2;
	double lnLHmt = 0;//sq((mtopmeas-mtop)/mtoperr)/2;

	double combined = exp( - lnLHxsec - lnLHmt);
	return combined;

}


/*
double LH_Exp(float mtop,float xsec){
	double xsecmeas= (240-270)/(178-166)*(mtop-172.5)   +247 ;
	double mtopmeas=172.5;
	double xsecerr=0.04*xsecmeas; //in pb
	double mtoperr=1.4+1; //in GeV

	//now likelihood
	double lnLHxsec = sq((xsecmeas-xsec)/xsecerr)/2;
	double lnLHmt = sq((mtopmeas-mtop)/mtoperr)/2;

	double combined = exp( - lnLHxsec - lnLHmt);
	return combined;
}*/

invokeApplication(){

	using namespace ztop;
	const std::vector<TString> in=parser->getRest<TString>();
	if(in.size()!=1)
		return -1;
	const TString infile=in.at(0);

	float energy=8;
	if(infile.Contains("7TeV"))
		energy=7;

	graph expg;
	expg.loadFromTFile(infile,"");
	graphFitter gf;
	gf.setFitMode(graphFitter::fm_pol2);
	gf.readGraph(&expg);
	gf.fit();

	std::cout << "Fitted dep: " << gf.getParameter(0) << "+ mt * " << gf.getParameter(1) << "+ mt*mt*" << gf.getParameter(2) <<std::endl;

	float xsecerrup=0.039;
	float xsecerrdown=0.035;
	if(isApprox(energy,(float)7.,0.1)){
		xsecerrup=0.04;
		xsecerrdown=0.036;
	}


	const float mintopmass=166;
	const float maxtopmass=178;
	const float ndiv=500;
	const float minxsec=getTtbarXsec(maxtopmass,energy);
	const float maxxsec=getTtbarXsec(mintopmass,energy);


	std::vector<float> bins=container1D::createBinning(ndiv,mintopmass,maxtopmass);
	std::vector<float> binsy=container1D::createBinning(ndiv,minxsec,maxxsec);
	container2D c=  container2D(bins,binsy,"","m_{t} [GeV]","#sigma_{t#bar{t}} [pb]");
	container2D contexp=  container2D(bins,binsy,"","m_{t} [GeV]","#sigma_{t#bar{t}} [pb]");
	graph countourexp;
	graph countourcomb;
	float scaleerrth,pdferrth;
	getTtbarXsec((maxtopmass-mintopmass)/2,energy,&scaleerrth,&pdferrth);
	double tottheoerr=sqrt(scaleerrth*scaleerrth+pdferrth*pdferrth);

	for(size_t i=1;i<=c.getNBinsX();i++){
		for(size_t j=1;j<=c.getNBinsY();j++){
			float centery,centerx;
			c.getBinCenter(i,j,centerx,centery);
			double xsec= getTtbarXsec(centerx,energy);
			long double lnL = xsec - centery;
			lnL/=(tottheoerr*xsec);
			lnL=sq(lnL)/2;//lnL/1e6;
			lnL=exp(-lnL);
			c.getBin(i,j).setContent(lnL);
			//centerx+=1;
			double lhexp=LH_Exp(centerx,centery,gf,xsecerrup,xsecerrdown);

			contexp.getBin(i,j).setContent(lhexp);
			if(isApprox(lhexp,exp(-0.5),0.01) )
				countourexp.addPoint(centerx,centery);
			double comb=(lhexp*lnL);
			if(isApprox(comb,exp(-0.5),0.01))
				countourcomb.addPoint(centerx,centery);


		}
	}

	//add maxima

	TString nrgstr="_"+toTString(energy)+"TeV";
	TCanvas cv;
	gStyle->SetOptStat(0);
	plotter2D pl;
	textBoxes tb2d;
	tb2d.readFromFileInCMSSW("/src/TtZAnalysis/Analysis/configs/general/CMS_boxes.txt","CMSnoSplitRight2D");
	for(size_t i=0;i<tb2d.size();i++)
		tb2d.at(i).setColor(0);


	pl.readStyleFromFileInCMSSW("src/TtZAnalysis/Analysis/configs/mtFromXsec2/plot2D_0to1.txt");
	pl.setZAxis("L_{pred}(m_{t},#sigma_{t#bar{t}})");
	pl.setPlot(&c);
	pl.printToPdf(("theoOnly" +nrgstr).Data());
	pl.cleanMem();



	pl.setPlot(&contexp);
	pl.usePad(&cv);
	pl.setZAxis("L_{exp}(m_{t},#sigma_{t#bar{t}})");
	pl.draw();
	TGraphAsymmErrors * g=countourexp.getTGraph("",true);
	g->SetMarkerSize(0.15);
	g->Draw("P,same");
	expg.getTGraph()->Draw("PX");
	tb2d.drawToPad(&cv);
	cv.Print("expOnly" +nrgstr+".pdf");
	pl.cleanMem();

	contexp*=c;

	pl.setPlot(&contexp);
	pl.usePad(&cv);
	pl.setZAxis("L_{exp} L_{pred}");
	pl.draw();
	TGraphAsymmErrors* g2=countourcomb.getTGraph("",true);
	g2->SetMarkerSize(0.15);
	g2->Draw("P,same");
	//g->Draw("P,same");
	tb2d.drawToPad(&cv);
	cv.Print("expTheo" +nrgstr+".pdf");
	pl.cleanMem();

	//integrate:
/*
	container2D loglh=lhtochi2(contexp);
	container1D marginalized = loglh.projectToX(false);
	plotterMultiplePlots plm;
	plm.usePad(&cv);
	plm.addPlot(&marginalized);
	plm.setLastNoLegend();
	plm.readTextBoxesInCMSSW("/src/TtZAnalysis/Analysis/configs/general/CMS_boxes.txt","CMSnoSplitRight2D");
	plm.draw();
	cv.Print("expTheoMarg" +nrgstr+".pdf");
*/

	//brute force scan for max
	float maxmt=0,maxlh=0,dummy;
	for(size_t i=1;i<=c.getNBinsX();i++){
		for(size_t j=1;j<=c.getNBinsY();j++){
			if(contexp.getBinContent(i,j)>maxlh){
				maxlh=contexp.getBinContent(i,j);
				contexp.getBinCenter(i,j,maxmt,dummy);
			}
		}
	}
	float minx=1000,maxx=0;
	for(size_t i=0;i<countourcomb.getNPoints();i++){
		if(countourcomb.getPointXContent(i) < minx) minx=countourcomb.getPointXContent(i);
		if(countourcomb.getPointXContent(i) > maxx) maxx=countourcomb.getPointXContent(i);
	}

	std::cout << "mt: "<< maxmt << "+ " <<  maxx-maxmt << "- " << maxmt-minx;

	return 0;
}
