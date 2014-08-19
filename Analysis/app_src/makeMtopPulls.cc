/*
 * makeMtopPulls.cc
 *
 *  Created on: Aug 15, 2014
 *      Author: kiesej
 */


#include "../interface/paraExtrPull.h"
#include "TtZAnalysis/Tools/interface/optParser.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "TtZAnalysis/Tools/interface/plotterMultiplePlots.h"
#include "TRandom3.h"
#include "TCanvas.h"
#include "TError.h"
#include "TFile.h"

int main(int argc, char* argv[]){

	using namespace ztop;

	optParser parse(argc,argv);
	const TString output=parse.getOpt<TString>("o","pullout","output file (without .pdf)");
	size_t niter= parse.getOpt<float>("i",100,"toys per pull");
	const float evalpoint=  parse.getOpt<float>("mt",172.5,"evalpoint");

	parse.doneParsing();

	//lets see what we are going to parse

	parameterExtractor * ex=new parameterExtractor();

	//smae config as for mtop
	ex->setLikelihoodMode(parameterExtractor::lh_fit);

	ex->setFitUncertaintyModeA(parameterExtractor::fitunc_statcorrgaus);
	ex->setFitUncertaintyModeB(parameterExtractor::fitunc_statuncorrpoisson);

	ex->setFitFunctions("pol2");
	ex->setConfidenceLevelFitInterval(0.68);
	ex->setIntersectionGranularity(500);

	std::vector<float> binning;
	for(float i=-5;i<=5;i+=0.05) binning<<i;

	size_t npulls=4;
	std::vector<container1D *> pulls;
	TRandom3* random=new TRandom3();

	gErrorIgnoreLevel = kInfo;
	TFile * f  = new TFile(output+".root","RECREATE");
	TCanvas cv1;

	//for a straight line
	float slope=1;

	float offsetatdefmtop=20;
	float offset=20-172*slope;

	for(size_t i=0;i<npulls;i++){

		//get functional form -> later you might want to fit some TBI inputs here
		//now just dummys


/////deprecaed
break;

		std::cout << "creating pull "<< i <<std::endl;
		container1D * ctemp= new container1D(binning);
		ctemp->setName(toTString(i));
		//do stuff
		paraExtrPull pex;
		pex.setExtractor(ex);
		pex.setFillContainer1D(ctemp);
		pex.setRandom(random);
		pex.setNIterations(niter);
//		pex.setFunctionalForm(-243.761,-614.668,5.90438);
		//pex.setFunctionalForm(offsetatdefmtop,slope,0.0001);
		pex.setMCScale((i+1));
		pex.setEvalPoint(evalpoint);

	/*		    graph data=pex.generateDataPoints();
	    data.getTGraph()->Draw("AP");

	    cv1.Print(output+"data.pdf");
	    graph mc=pex.generateMCPoints();
	    mc.getTGraph()->Draw("AP");

	    cv1.Print(output+"mc.pdf");
break;*/
		pex.fill();


		pulls.push_back(ctemp);
	}


	plotterMultiplePlots plotterm;
	plotterm.readStyleFromFileInCMSSW("src/TtZAnalysis/Tools/styles/multiplePlots.txt");
	plotterm.usePad(&cv1);

	for(size_t i=0;i<pulls.size();i++){

		plotterm.addPlot(pulls.at(i));

	}
	plotterm.draw();

	cv1.Write();

	cv1.Print(output+".pdf");
f->Close();
	//last
	for(size_t i=0;i<pulls.size();i++)
		if(pulls.at(i)) delete pulls.at(i);

}
