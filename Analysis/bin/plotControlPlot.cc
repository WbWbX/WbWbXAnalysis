/*
 * plotControlPlot.cc
 *
 *  Created on: Nov 24, 2014
 *      Author: kiesej
 */


#include "TtZAnalysis/Tools/interface/applicationMainMacro.h"
#include "TtZAnalysis/Tools/interface/plotterControlPlot.h"
#include "TtZAnalysis/Tools/interface/containerStackVector.h"
#include "TtZAnalysis/Tools/interface/fileReader.h"
#include "TtZAnalysis/Tools/interface/textFormatter.h"
#include "TCanvas.h"

/**
 * Small application to plot a control plot in a fast and efficient way. Returns a pdf file
 */
invokeApplication(){
	using namespace ztop;
	parser->setAdditionalDesciption("Small application to plot control plots in a fast and efficient way.\nSeparate with \"%%\". Returns pdf files");

	const std::string stylefile=parser->getOpt<std::string>("i",
			getenv("CMSSW_BASE")+(std::string)"/src/TtZAnalysis/Tools/styles/controlPlots_standard.txt",
			"Specify (optional) control plot style file");
	const std::string plotnames=parser->getOpt<std::string>("p","","specify plot names as list plot1 %% plot2 %% ...");
	const std::string inlist=parser->getOpt<std::string>("-list","","specify a file that represents a list of all plots to be plotted.");

	std::vector<TString> tmpv=parser->getRest<TString>();
	parser->doneParsing();
	if(tmpv.size()!=1){
		parser->coutHelp();
		return 0;
	}
	const TString infile = tmpv.at(0);
	tmpv.clear();

	/////////   parsing done

	containerStackVector * csv = new containerStackVector();

	try{
		csv->loadFromTFile(infile);
	}catch(...){
		std::cout << "No Plots found in file " << infile <<std::endl;
		delete csv;
		return -1;
	}
	std::vector<TString> notfound;
	std::vector<std::string> allnames;
	//if(inlist.length()<1){
	textFormatter fmt;
	fmt.setDelimiter("%%");
	fmt.setComment("#");
	fmt.setTrim(" ");
	allnames=fmt.getFormatted(plotnames);
	//}
	if(inlist.length()>0){
		fileReader fr;
		fr.setDelimiter("%%");
		fr.setComment("#");
		fr.setTrim(" ");
		fr.readFile(inlist);
		for(size_t i=0;i<fr.nLines();i++){
			std::string str=fr.getData<std::string>(i,0);
			if(str.length()<1) continue;
			allnames.push_back(str);
		}
	}

	for(size_t i=0;i<allnames.size();i++){
		containerStack  stack;
		TString plotname=allnames.at(i);
		if(plotname.Length()<1) continue;
		try{
			stack= csv->getStack(plotname);//copy
		}catch(...){
			notfound.push_back(plotname);
			continue;
		}
		plotterControlPlot pl;
		try{
			pl.readStyleFromFile(stylefile);
		}catch(...){
			std::cout << "Style not found in file " << stylefile <<std::endl;
			delete csv;
			return -3;
		}
		TCanvas cv;
		pl.usePad(&cv);
		pl.setStack(&stack);

		TString outname=stack.getFormattedName();
		outname+=".pdf";
		pl.draw();
		cv.Print(outname);
	}
	if(notfound.size()>0){
		std::cout << "Plots not found:\n";
		for(size_t i=0;i<notfound.size();i++)
			std::cout << notfound.at(i) << "\n";
		std::cout << "\navailable:\n";
		csv->listStacks();
	}

	delete csv;
	return 0;
}
