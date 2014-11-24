/*
 * plotControlPlot.cc
 *
 *  Created on: Nov 24, 2014
 *      Author: kiesej
 */


#include "mainMacro.h"
#include "TtZAnalysis/Tools/interface/plotterControlPlot.h"
#include "TtZAnalysis/Tools/interface/containerStackVector.h"
#include "TCanvas.h"

/**
 * Small application to plot a control plot in a fast and efficient way. Returns a pdf file
 */
invokeApplication(){
	using namespace ztop;
	parser->setAdditionalDesciption("Small application to plot a control plot in a fast and efficient way.\nReturns a pdf file");

	const std::string stylefile=parser->getOpt<std::string>("i",
			getenv("CMSSW_BASE")+(std::string)"/src/TtZAnalysis/Tools/styles/controlPlots_standard.txt",
			"Specifiy (optional) control plot style file");
	const TString plotname=parser->getOpt<TString>("p","","specify plot name");

	std::vector<TString> tmpv=parser->getRest<TString>();
	if(tmpv.size()!=1){
		parser->coutHelp();
		return 0;
	}
	const TString infile = tmpv.at(0);
	tmpv.clear();

	/////////   parsing done

	containerStackVector * csv = new containerStackVector();
	containerStack  stack;
	try{
		csv->loadFromTFile(infile);
	}catch(...){
		std::cout << "No Plots found in file " << infile <<std::endl;
		delete csv;
		return -1;
	}
	try{
		stack= csv->getStack(plotname);//copy
	}catch(...){
		std::cout << "Plot " << plotname<< " not found in file " << infile <<std::endl;
		std::cout << "Available: " <<std::endl;
		csv->listStacks();
		delete csv;
		return -2;
	}
	delete csv;
	plotterControlPlot pl;
	try{
		pl.readStyleFromFile(stylefile);
	}catch(...){
		std::cout << "Style not found in file " << stylefile <<std::endl;
		return -3;
	}
	TCanvas cv;
	pl.usePad(&cv);
	pl.setStack(&stack);

	TString outname=stack.getName();
	outname.ReplaceAll(" ","_");
	outname+=".pdf";
	pl.draw();
	cv.Print(outname);


	return 0;
}
