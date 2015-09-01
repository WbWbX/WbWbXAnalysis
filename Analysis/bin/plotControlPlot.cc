/*
 * plotControlPlot.cc
 *
 *  Created on: Nov 24, 2014
 *      Author: kiesej
 */


#include "TtZAnalysis/Tools/interface/applicationMainMacro.h"
#include "TtZAnalysis/Tools/interface/plotterControlPlot.h"
#include "TtZAnalysis/Tools/interface/histoStackVector.h"
#include "TtZAnalysis/Tools/interface/fileReader.h"
#include "TtZAnalysis/Tools/interface/textFormatter.h"
#include "TCanvas.h"
#include "TFile.h"
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
	const std::string inlist=parser->getOpt<std::string>("-list","","specify a file that represents a list of all plots to be plotted.\n     one line for each plot or %% as delimiter");
	const std::string suffix=parser->getOpt<std::string>("s","","specify a suffix.");
	const std::string outdir=parser->getOpt<std::string>("d",".","specify an optional output directory.");
	const bool normToInt=parser->getOpt<bool>("int",false,"specify whether you want to normalize to the integral of the data or to the lumi.");

	const bool printToRoot = parser->getOpt<bool>("-root",false,"creates a root file with a TCanvas instead of a pdf");

	std::vector<TString> tmpv=parser->getRest<TString>();
	parser->doneParsing();
	if(tmpv.size()!=1){
		parser->coutHelp();
		return 0;
	}
	const TString infile = tmpv.at(0);
	tmpv.clear();

	/////////   parsing done

	histoStackVector * csv = new histoStackVector();

	try{
		csv->readFromFile(infile.Data());
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
	if(outdir.length()>0)
		system(("mkdir -p "+ outdir).data());


	for(size_t i=0;i<allnames.size();i++){
		histoStack  stack;
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
		if(normToInt){
			double integral_data = stack.getDataContainer().integral();
			double integral_mc   = stack.getFullMCContainer().integral();
			stack.multiplyAllMCNorms(integral_data/integral_mc);
		}




		TString outname=stack.getFormattedName();
		outname+=suffix;
		TCanvas cv(outname);
		outname=(TString)outdir.data()+"/"+outname;
		pl.usePad(&cv);
		pl.setStack(&stack);
		pl.draw();
		if(printToRoot){
			TFile *f=new TFile(outname+".root","RECREATE");
			cv.Write();
			f->Close();
			delete f;
		}
		else{
			outname+=".pdf";
			cv.Print(outname);
		}

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
