/*
 * wA7Extractor.cc
 *
 *  Created on: 20 Jun 2016
 *      Author: jkiesele
 */

#include "../interface/wA7Extractor.h"
#include "TtZAnalysis/Tools/interface/histoStackVector.h" //all histos
#include "TtZAnalysis/Tools/interface/textFormatter.h"


namespace ztop{


void wA7Extractor::readConfig(const std::string& infile){/*TBI*/}


void wA7Extractor::loadPlots(const std::string& infile){
	histoStackVector * hsv=new histoStackVector();
	hsv->readFromFile(infile);

	const TString isoplots="pttrans_deta_";
	const TString nonisoplots="pttrans_noiso_deta_";



	//get lists;
	std::vector<TString> stacknames=hsv->getStackNames(false);

	std::vector<histoStack> selectedStacks;
	std::vector<histoStack> nonisoStacks;
	//dynamically produce binning
	/*
	 * actually, it doesn't matter...
	 */
	//search for plots
	histo1D::c_makelist=true;
	for(size_t i=0;i<stacknames.size();i++){
		if(stacknames.at(i).BeginsWith(isoplots)){
			selectedStacks.push_back(hsv->getStack(stacknames.at(i)));
			std::cout << stacknames.at(i) << std::endl;
		}
	}
	histo1D::c_makelist=false;
	//now search for corresponding non-iso plots
	for(size_t i=0;i<selectedStacks.size();i++){
		TString searchname=selectedStacks.at(i).getName();
		searchname.ReplaceAll(isoplots,nonisoplots);
		nonisoStacks.push_back(hsv->getStack(searchname));
	}
	hsv->clear();
	std::vector<TString>fitcontr;
	fitcontr.push_back("QCD");

	//replace QCD by noniso data-MC
	for(size_t i=0;i<selectedStacks.size();i++){
		histo1D nonisodata=nonisoStacks.at(i).getDataContainer();
		histo1D nonisoMCother=nonisoStacks.at(i).getContributionsBut("QCD");
		nonisodata-=nonisoMCother;
		histo1D & href=selectedStacks.at(i).getContainer(selectedStacks.at(i).getContributionIdx("QCD"));
		href=nonisodata;
		hsv->getVector().push_back(selectedStacks.at(i));

		//fitted shape
		histoStack st=selectedStacks.at(i).fitAllNorms(fitcontr,true);
		st.setName(st.getName()+"_fitted");
		hsv->getVector().push_back(st);

	}



	hsv->writeAllToTFile("testOut_plots.root",true,false);


	delete hsv;
}

}
