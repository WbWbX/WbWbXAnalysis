/*
 * scanSeveralMtExtractions.cc
 *
 *  Created on: Jul 29, 2014
 *      Author: kiesej
 */

#include "WbWbXAnalysis/Tools/interface/optParser.h"
#include <iostream>
#include "WbWbXAnalysis/Tools/interface/textFormatter.h"
#include "WbWbXAnalysis/Tools/interface/formatter.h"
#include <string>
#include "WbWbXAnalysis/Tools/interface/fileReader.h"
#include "TString.h"
#include "WbWbXAnalysis/Tools/interface/plotterMultiplePlots.h"
#include "TCanvas.h"
#include <sys/param.h>
#include <unistd.h>
#include "TLine.h"

/*
 * just utilizes mtFromXSec2 application and organizes output a bit
 * can make calibration plots - very specific stuff. requires lots of
 * formats etc.. anyway...
 *
 *
 */

#include "WbWbXAnalysis/Tools/interface/applicationMainMacro.h"

invokeApplication(){
	using namespace ztop;

	bool makecalib = parser->getOpt<bool>("c",false,"enables calibration plot");
	TString opts = parser->getOpt<TString>("o","","specify additional options parsed to each mtFromXSec2");
	float mtopin = parser->getOpt<float>("mt",172.5,"specify reference top mass (only for pulls)");
	std::vector<std::string> dirnames = parser->getRest<std::string>();

	parser->doneParsing();
	graph sumgraph;//(dirnames.size());
	graph zerograph;
	graph gstatup=sumgraph;
	graph gstatdown=sumgraph;
	textFormatter fmt;
	formatter formt;

	TString thisdir=getenv("PWD");

	for(size_t i=0;i<dirnames.size();i++){
		std::cout << dirnames.at(i)  << std::endl;
		//continue;
		TString workdir=dirnames.at(i)+"/output/";
		//TString syscall="cd "+dirnames.at(i);
		chdir(workdir.Data());

		TString tmpout="tmpout"+formt.toTString(i)+".txt";
		TString syscall="mtFromXsec2 *_syst.root -o calib --extract --nosyst "+opts+" > " +tmpout;
		system(syscall);
		fileReader fr; //this one reads the input
		fr.setStartMarker("[Mtop feedback]");
		fr.setEndMarker("[end Mtop feedback]");
		fr.readFile(tmpout.Data());
		float mtop=fr.getValue<float>("__MTOP__");
		float stup=fr.getValue<float>("__MStatUp__");
		float stdown=fr.getValue<float>("__MStatDown__");

		if(makecalib){
			fmt.setDelimiter("_");
			fmt.setTrim("/");
			std::vector<std::string> choppeddirname=fmt.getFormatted(dirnames.at(i));
			for(size_t cdn=0;cdn<choppeddirname.size();cdn++){
				if(choppeddirname.at(cdn) == "mt"){
					mtopin=atof(choppeddirname.at(cdn+1).data());
				}
			}

		}

		std::cout << "top mass: " << mtop << " vs "<<mtopin << "\n"<<std::endl;
		float xpoint=i;
		if(makecalib)
			xpoint=mtopin;
		zerograph.addPoint(xpoint,0);
		sumgraph.addPoint(xpoint,mtop-mtopin);
		gstatup.addPoint(xpoint, mtop+stup-mtopin);
		gstatdown.addPoint(xpoint,mtop-stdown-mtopin);
		//syscall="rm "+tmpout;
		chdir(thisdir.Data());
	}


	sumgraph.addErrorGraph("stat_up",gstatup);
	sumgraph.addErrorGraph("stat_down",gstatdown);

	plotterMultiplePlots pl;

	sumgraph.setYAxisName("m_{t}^{out}-m_{t}^{in} [GeV]");
	if(!makecalib){
		sumgraph.setXAxisName("Subset");
		pl.readStyleFromFileInCMSSW("src/WbWbXAnalysis/Analysis/configs/topmass/multiplePlots_pull.txt");
	}
	else{
		sumgraph.setXAxisName("m_{t}^{in} [GeV]");
		pl.readStyleFromFileInCMSSW("src/WbWbXAnalysis/Analysis/configs/topmass/multiplePlots_calib.txt");
	}


	TCanvas cv;
	pl.usePad(&cv);
	pl.addPlot(&sumgraph);

	opts.ReplaceAll(" ","_");
	opts.ReplaceAll("\"","_");
	opts.ReplaceAll("-","_");
	float chi2sum=0;
	float mean=0;
	for(size_t i=0;i<sumgraph.getNPoints();i++){
		chi2sum+= sumgraph.getPointYContent(i)*sumgraph.getPointYContent(i)/(sumgraph.getPointYError(i,false)*sumgraph.getPointYError(i,false));
		mean+=sumgraph.getPointYContent(i);
	}
	mean/=sumgraph.getNPoints();

	float chi2ndof=chi2sum/(sumgraph.getNPoints());
	if(!makecalib)
		pl.addTextBox(0.1,0.055,"#Deltam_{t}^{avg} = "+formt.toTString(formt.round(mean,0.01))+ ",  #chi^{2}/N_{dof} = "+formt.toTString(formt.round(chi2ndof,0.01)));
	//pl.addTextBox(0.1,0.055,"#chi^{2}/N_{dof}="+formt.toTString(formt.round(chi2ndof,0.01)));

	pl.draw();
	if(fabs(mtopin-172.5)>0.1 && !makecalib){
		TString tmpstr=toTString(mtopin);
		tmpstr.ReplaceAll(".","_");
		opts+=tmpstr;
	}
	if(!makecalib){
		TLine line(-1,0,10,0);
		line.Draw("same");
		cv.Print("pull_"+opts+".pdf");
	}
	else{
		TLine line(166,0,179,0);
		line.Draw("same");
		cv.Print("calib_"+opts+".pdf");
	}

	return 1;

}
