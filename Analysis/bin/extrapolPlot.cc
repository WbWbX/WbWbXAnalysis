/*
 * extrapolPlot.cc
 *
 *  Created on: 2 Sep 2016
 *      Author: jkiesele
 */



#include "TtZAnalysis/Tools/interface/applicationMainMacro.h"
#include "TtZAnalysis/Tools/interface/graph.h"
#include "TtZAnalysis/Tools/interface/plotterMultiplePlots.h"
#include "TtZAnalysis/Tools/interface/binFinder.h"
#include <vector>
#include <math.h>
#include "TFile.h"
#include "TCanvas.h"

class stepdependence{
public:
	stepdependence();
	void addPoint(float x, float y);


	ztop::graph createGraph(TString name)const;

private:
	float evaluate(float x)const;
	std::vector<float> stepx,stepy;
	float interpolate(float x, float x1, float x2, float y1, float y2, float smoothness=.15)const;
};
stepdependence::stepdependence(){
	stepx.push_back(0);
	stepy.push_back(0);//underflow
}

void stepdependence::addPoint(float x, float y){
	//add check for ordering maybe
	stepx.push_back(x);
	stepy.push_back(y);
}
float stepdependence::evaluate(float x)const{
	using namespace ztop;
	if(stepx.size()<2)return 0;
	if(x < stepx.at(1)) return stepy.at(1);
	if(x>= stepx.at(stepx.size()-1)) return stepy.at(stepy.size()-1);

	binFinder<float> bf(stepx);

	size_t pos=bf.findBin(x);

	float x1=stepx.at(pos);
	float x2=stepx.at(pos+1);
	float y1=stepy.at(pos);
	float y2=stepy.at(pos+1);

	return interpolate(x,x1,x2,y1,y2);

}
float stepdependence::interpolate(float x, float x1, float x2, float y1, float y2, float smoothness)const{
	float stretch=3/smoothness;

	float range=(x2-x1);
	float erf_x= (2*(x-x1)/range -1)*stretch;

	float inter= ( erf(erf_x) +1)/2;
	inter*= (y2-y1);
	return y1+inter;
}

ztop::graph stepdependence::createGraph(TString name)const{
	ztop::graph g(100);
	g.setName(name);
	size_t point=0;
	for(float x=0.75;x<=3.25;x+=0.025){
		g.setPointContents(point,true, x, evaluate(x));
		point++;
	}

	return g;

}


invokeApplication(){
	using namespace ztop;

	float firstx=0.8;
	float secondx=1.85;
	float thirdx=2.9;

	stepdependence ljets;
	ljets.addPoint(firstx, 0.51);
	ljets.addPoint(secondx, 0.31);
	ljets.addPoint(thirdx, 0.17);

	stepdependence singletop;
	singletop.addPoint(firstx, 1.24);
	singletop.addPoint(secondx, 0.62);
	singletop.addPoint(thirdx, 0.45);


	stepdependence mlsv;
	mlsv.addPoint(firstx, 1.6);
	mlsv.addPoint(secondx, 0.95);
	mlsv.addPoint(thirdx, 0.62);

	stepdependence jpsi;
	jpsi.addPoint(firstx, 3.16);
	jpsi.addPoint(secondx, 1.0);
	jpsi.addPoint(thirdx, 0.58);

	stepdependence fromxsec;
	fromxsec.addPoint(firstx, 2);
	//fromxsec.addPoint(0.8, 2);
	//fromxsec.addPoint(1.2, 2.7);
	fromxsec.addPoint(secondx, 1.4);
	fromxsec.addPoint(thirdx, 1.2);
/*
	graph gljets=ljets.createGraph("l+jets");
	graph gsingletop=singletop.createGraph("single t");
	graph gmlsv=mlsv.createGraph("sec. vtx");
	graph gjpsi=jpsi.createGraph("J/#Psi");
	graph gfromxsec=fromxsec.createGraph("#sigma (t#bar{t})");
	*/

	graph gljets=ljets.createGraph("l+jets, PRD 93(2016)2004");
	graph gsingletop=singletop.createGraph("single t, PAS-TOP-15-001");
	graph gmlsv=mlsv.createGraph("sec. vtx, PRD 93(2016)2006 ");
	graph gjpsi=jpsi.createGraph("J/#Psi, arXiv:1608.03560");
	graph gfromxsec=fromxsec.createGraph("#sigma (t#bar{t}), JHEP08 (2016) 029");


	plotterMultiplePlots pl;
	TFile outfile("extrapolplot.root","RECREATE");
	TCanvas cv("","",800,700);
	pl.usePad(&cv);
	pl.readStyleFromFileInCMSSW("/src/TtZAnalysis/Analysis/configs/massextra/extraplot.txt");

	pl.addPlot(&gjpsi);
	pl.addPlot(&gfromxsec);
	pl.addPlot(&gmlsv);
	pl.addPlot(&gsingletop);
	pl.addPlot(&gljets);

	pl.addTextBox(0.275-0.02,0.10,"Run I",0.05,21,kBlack).setFont(22);
	pl.addTextBox(0.525-0.02,0.10,"0.3/ab, 14 TeV",0.05,21,kBlack).setFont(22);
	pl.addTextBox(0.775+0.02,0.10,"3/ab, 14 TeV",0.05,21,kBlack).setFont(22);


	pl.draw();

	cv.Write();
	cv.Print("extrapolplot.pdf");
	outfile.Close();

	return 1;
}
