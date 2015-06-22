/*
 * testPMS.cc
 *
 *  Created on: May 4, 2015
 *      Author: kiesej
 */




#include <TtZAnalysis/Tools/interface/plotterMultiColumn.h>
#include "../interface/histoStack.h"
#include "fillContainerRandom.h"
#include "TRandom3.h"
#include "TFile.h"
#include  "../interface/histoStackVector.h"
#include "TPostScript.h"


int main(){
	using namespace ztop;
	/*
	histoStack stack,stack2;
	histo1D h1d,h2d;
	for(int i=0;i<5;i++){
		histo1D h(histo1D::createBinning(10,0,80), "blub", "x-Axis [GeV]","y-Axis/GeV");
		histo1D h2(histo1D::createBinning(1,0,1), "blub", "x-Axis [GeV]","y-Axis/GeV");
		fillRandom(&h2,(en_functions)i,1,30,10000);
		fillRandom(&h,(en_functions)i,50,30,10000);
		h1d+=h;
		h2d+=h2;
		stack.push_back(h,"bla "+toTString(i),636-i,1,i+1);
		stack2.push_back(h2,"bla "+toTString(i),636-i,1,i+1);
		h.clear();
	}
	TRandom3* r=new TRandom3();
	histo1D h=h1d;
	h=h.createPseudoExperiment(r,&h1d, histo1D::pseudodata_poisson);
	stack.push_back(h,"data",0,1);

	histo1D h2=h2d;
	h2=h2.createPseudoExperiment(r,&h2d, histo1D::pseudodata_poisson);
	stack2.push_back(h2,"data",0,1);

	plotterControlPlot pl;
	pl.addStyleFromFile((std::string)getenv("CMSSW_BASE")+"/src/TtZAnalysis/Tools/styles/controlPlots_standard.txt");
	pl.setStack(&stack);
	pl.printToPdf("test_single");

	//plotterBase::debug=true;

	plotterMultiStack plm;
	plm.readStyleFromFileInCMSSW("/src/TtZAnalysis/Analysis/configs/fitTtBarXsec/plotterMultiStack_standard.txt");
	plm.addStack(&stack2);
	histoStack stack3=stack*0.2;
	plm.addStack(&stack);
	plm.addStack(&stack3);
	histoStack stack4 = stack3*0.2;
	plm.addStack(&stack4);
	plm.printToPdf("test_multi");
	 */
	histoStackVector hsv;
	hsv.readFromFile("/afs/desy.de/user/k/kiesej/recentProject/src/TtZAnalysis/Analysis/output/emu_7TeV_172.5_nominal.ztop");

	histoStack tot=hsv.getStack("total 0,0 b-jets step 8");
	histoStack j1=hsv.getStack("lead jet pt 0,1 b-jets step 8");
	histoStack j2=hsv.getStack("second jet pt 0,2 b-jets step 8");
	histoStack j3=hsv.getStack("third jet pt 0,3 b-jets step 8");


	plotterMultiStack plm;
	plm.readStyleFromFileInCMSSW("/src/TtZAnalysis/Analysis/configs/fitTtBarXsec/plotterMultiStack_standard.txt");

	TCanvas cv;
	plm.readTextBoxesInCMSSW("/src/TtZAnalysis/Analysis/configs/fitTtBarXsec/plotterMultiStack_standard.txt","0btag7TeV");
	plm.addStack(&tot);
	plm.addStack(&j1);
	plm.addStack(&j2);
	plm.addStack(&j3);
	plm.printToPdf("test_multivc2pdf");
	plm.usePad(&cv);
	plm.draw();
	cv.Print("test_multivc2.bmp");
	return 0;
}
