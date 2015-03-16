/*
 * 2Dchi2.cc
 *
 *  Created on: Jan 20, 2015
 *      Author: kiesej
 */




#include "../interface/histoStackVector.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "../interface/plotter2D.h"

#include "TtZAnalysis/Analysis/app_src/mainMacro.h"
#include "TString.h"
#include "TStyle.h"

invokeApplication(){
	using namespace ztop;
	const std::vector<TString> files = parser->getRest<TString>();

	if(files.size()!=2)
		return 1;

	histoStackVector csv;
	csv.loadFromTFile(files.at(0));
	histoStack stack=csv.getStack("m_lb min step 8");
	histo1DUnfold cuf1=stack.getSignalContainer1DUnfold();

	csv.loadFromTFile(files.at(1));
	stack=csv.getStack("m_lb min step 8");
	histo1DUnfold cuf2=stack.getSignalContainer1DUnfold();

	std::vector<float> newbins;
	newbins << 0   << 75 << 100 << 125  << 200 << 350;

	cuf1=cuf1.rebinToBinning(newbins);
	cuf2=cuf2.rebinToBinning(newbins);

	histo2D resp1=cuf1.getNormResponseMatrix();
	histo2D resp2=cuf2.getNormResponseMatrix();

	histo2D diff=resp1-resp2;

	gStyle->SetOptStat(0);

	TCanvas c;
	plotter2D pl;
	pl.setPlot(&resp1,false);
	pl.printToPdf((files.at(0)+"_resp").Data());

	pl.cleanMem();
	pl.setPlot(&resp2,false);
	pl.printToPdf((files.at(1)+"_resp").Data());

	pl.cleanMem();
	pl.setPlot(&diff,false);
	pl.printToPdf((files.at(0)+"_"+ files.at(1) +"_diff").Data());

	size_t ndof;
	histo2D chi2=resp1.chi2container(resp2,&ndof);


	pl.cleanMem();
	pl.setPlot(&chi2,false);
	pl.printToPdf((files.at(0)+"_"+ files.at(1) +"_chi2").Data());


	float gchi2=resp1.chi2(resp2);

	std::cout << "chi2: " << gchi2 << " ndof: " << ndof <<std::endl;

	return 0;
}
