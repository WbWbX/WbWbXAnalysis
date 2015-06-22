/*
 * testMultiCompare.cc
 *
 *  Created on: Jun 3, 2015
 *      Author: kiesej
 */




#include "../interface/plotterMultiCompare.h"
#include "../interface/applicationMainMacro.h"
#include "../interface/histoStackVector.h"
#include "../interface/fileReader.h"
#include "TCanvas.h"

invokeApplication(){

	using namespace ztop;

	histoStackVector hsv;hsv.readFromFile("emu_8TeV_172.5_nominal_invertedIsoQCDWjets.ztop");

	std::cout << "File read" <<std::endl;

	std::vector<std::vector<histoStack> > allstacks;

	const TString comparecontributiontosignal= parser->getOpt<TString>("c","t#bar{t}bg","compare to");
	const TString nomcontr= parser->getOpt<TString>("n","signal","nominal");

	float totsignorm=0,totothernorm=0,totall=0;
	for(size_t i=0;i<3;i++){
		std::vector<histoStack> stackstmp;


		stackstmp.push_back(hsv.getStack("total "+toTString(i) +",0 b-jets step 8"));
		stackstmp.push_back(hsv.getStack("lead jet pt "+toTString(i) +",1 b-jets step 8"));
		stackstmp.push_back(hsv.getStack("second jet pt "+toTString(i) +",2 b-jets step 8"));
		stackstmp.push_back(hsv.getStack("third jet pt "+toTString(i) +",3 b-jets step 8"));
		for(size_t j=0;j<stackstmp.size();j++){
			//stackstmp.at(j).mergeVariationsFromFileInCMSSW("/src/TtZAnalysis/Analysis/configs/fitTtBarXsec/combine_syst.txt");
			//stackstmp.at(j).mergeVariationsFromFileInCMSSW("/src/TtZAnalysis/Analysis/configs/fitTtBarXsec/combine_syst.txt","merge for plots");

			if(nomcontr=="signal")
				totsignorm+=stackstmp.at(j).getSignalContainer().integral(false);
			else
				totsignorm+=stackstmp.at(j).getContribution1DUnfold(nomcontr).getRecoContainer().integral(false);
			totothernorm+=stackstmp.at(j).getContribution1DUnfold(comparecontributiontosignal).getRecoContainer().integral(false);

			totall+=stackstmp.at(j).getFullMCContainer1DUnfold().getRecoContainer().integral(false);
		}


		allstacks.push_back(stackstmp);

	}

	std::cout << "Total events "  << " :" << totall <<std::endl;
	std::cout << "Total events " << nomcontr << " :" << totsignorm <<std::endl;
	std::cout << "Total events " << comparecontributiontosignal << " :" << totothernorm <<std::endl;

	std::vector<TString> sys=allstacks.at(0).at(0).getSystNameList();

	//for(size_t j=0;j<sys.size();j++){

	for(size_t j=0;j<allstacks.size();j++){
		TString sysname=comparecontributiontosignal+"_to_"+nomcontr+ "_shapes_"+toTString(j)+"_bjets";//sys.at(j);
		sysname.ReplaceAll("#","");
		sysname.ReplaceAll("{","");
		sysname.ReplaceAll("}","");
		std::vector<histoStack> & stacks=allstacks.at(j);


		std::vector<histo1D> nominals;
		std::vector<histo1D> ups;
		std::vector<histo1D> downs;

		for(size_t i=0;i<stacks.size();i++){

			/*std::vector<histo1D> tmp=stacks.at(i).getFullMCContainer().produceVariations(sysname);
			nominals.push_back(tmp.at(0));
			ups.push_back(tmp.at(1));
			downs.push_back(tmp.at(2)); */
			histo1D tmp;
			if(nomcontr=="signal")
				tmp=stacks.at(i).getSignalContainer();
			else
				tmp=stacks.at(i).getContribution1DUnfold(nomcontr).getRecoContainer();


			tmp.removeAllSystematics();
			tmp*= (1/totsignorm);
			tmp.setYAxisName("1/N_{tot} "+tmp.getYAxisName());
			tmp.setName(nomcontr);
			nominals.push_back(tmp);

			tmp=stacks.at(i).getContribution1DUnfold(comparecontributiontosignal).getRecoContainer();
			tmp.removeAllSystematics();
			tmp*= (1/totothernorm);
			tmp.setName(comparecontributiontosignal);
			ups.push_back(tmp);
		}
		plotterMultiCompare pl;
		pl.readStyleFromFileInCMSSW("/src/TtZAnalysis/Analysis/configs/fitTtBarXsec/plotterMultiCompare_standard.txt");
		std::cout << "nominals " <<std::endl;
		pl.setNominalPlots(&nominals);
		std::cout << "comps " <<std::endl;
		pl.setComparePlots(&ups,0);
		//pl.setComparePlots(&downs,1);
		//plotterCompare::debug=true;
		std::cout << "print " <<std::endl;
		pl.printToPdf((std::string)"out/"+sysname.Data());
		TCanvas cv;
		pl.usePad(&cv);
		pl.draw();
		cv.Print("out/"+sysname+".png");
	}
	return 0;
}
