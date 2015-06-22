/*
 * analysisPlotsTtbarXsecFit.cc
 *
 *  Created on: Feb 12, 2014
 *      Author: kiesej
 */


#include "../interface/analysisPlotsTtbarXsecFit.h"
#include "../interface/AnalysisUtils.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "TtZAnalysis/DataFormats/src/classes.h"
#include "TtZAnalysis/DataFormats/interface/helpers.h"

namespace ztop{

void analysisPlotsTtbarXsecFit::bookPlots(){
	if(!use()) return;
	using namespace std;
	using namespace ztop;


	std::vector<float> bins=histo1D::createBinning(1,0,1);
	//leadjetpt_plots, secjetpt_plots, thirdjetpt_plots, total_plots;
	total_plots.at(cat_0bjet0jet) = addPlot(bins,bins,"total 0,0 b-jets","","Events");
	total_plots.at(cat_0bjet1jet) = addPlot(bins,bins,"total 0,1 b-jets","","Events");
	total_plots.at(cat_0bjet2jet) = addPlot(bins,bins,"total 0,2 b-jets","","Events");
	total_plots.at(cat_0bjet3jet) = addPlot(bins,bins,"total 0,3 b-jets","","Events");
	total_plots.at(cat_1bjet0jet) = addPlot(bins,bins,"total 1,0 b-jets","","Events");
	total_plots.at(cat_1bjet1jet) = addPlot(bins,bins,"total 1,1 b-jets","","Events");
	total_plots.at(cat_1bjet2jet) = addPlot(bins,bins,"total 1,2 b-jets","","Events");
	total_plots.at(cat_1bjet3jet) = addPlot(bins,bins,"total 1,3 b-jets","","Events");
	total_plots.at(cat_2bjet0jet) = addPlot(bins,bins,"total 2,0 b-jets","","Events");
	total_plots.at(cat_2bjet1jet) = addPlot(bins,bins,"total 2,1 b-jets","","Events");
	total_plots.at(cat_2bjet2jet) = addPlot(bins,bins,"total 2,2 b-jets","","Events");
	total_plots.at(cat_2bjet3jet) = addPlot(bins,bins,"total 2,3 b-jets","","Events");

	bins.clear();
	bins << 30 << 35  << 40 << 50 << 60 << 70 << 80 << 90 << 100 << 120 << 140 << 160 << 200;
	leadjetpt_plots.at(cat_0bjet0jet) = addPlot(bins,bins,"lead jet pt 0,0 b-jets","p_{T}} [GeV]","Events/GeV");
	leadjetpt_plots.at(cat_0bjet1jet) = addPlot(bins,bins,"lead jet pt 0,1 b-jets","p_{T}} [GeV]","Events/GeV");
	leadjetpt_plots.at(cat_0bjet2jet) = addPlot(bins,bins,"lead jet pt 0,2 b-jets","p_{T}} [GeV]","Events/GeV");
	leadjetpt_plots.at(cat_0bjet3jet) = addPlot(bins,bins,"lead jet pt 0,3 b-jets","p_{T}} [GeV]","Events/GeV");
	leadjetpt_plots.at(cat_1bjet0jet) = addPlot(bins,bins,"lead jet pt 1,0 b-jets","p_{T}} [GeV]","Events/GeV");
	leadjetpt_plots.at(cat_1bjet1jet) = addPlot(bins,bins,"lead jet pt 1,1 b-jets","p_{T}} [GeV]","Events/GeV");
	leadjetpt_plots.at(cat_1bjet2jet) = addPlot(bins,bins,"lead jet pt 1,2 b-jets","p_{T}} [GeV]","Events/GeV");
	leadjetpt_plots.at(cat_1bjet3jet) = addPlot(bins,bins,"lead jet pt 1,3 b-jets","p_{T}} [GeV]","Events/GeV");
	bins.clear();
	bins << 30    << 40 << 50 << 60 << 70 << 80 << 90 << 120 << 160;
	leadjetpt_plots.at(cat_2bjet0jet) = addPlot(bins,bins,"lead jet pt 2,0 b-jets","p_{T}} [GeV]","Events/GeV");
	leadjetpt_plots.at(cat_2bjet1jet) = addPlot(bins,bins,"lead jet pt 2,1 b-jets","p_{T}} [GeV]","Events/GeV");
	leadjetpt_plots.at(cat_2bjet2jet) = addPlot(bins,bins,"lead jet pt 2,2 b-jets","p_{T}} [GeV]","Events/GeV");
	leadjetpt_plots.at(cat_2bjet3jet) = addPlot(bins,bins,"lead jet pt 2,3 b-jets","p_{T}} [GeV]","Events/GeV");


	bins.clear();
	bins << 30 << 35  << 40 << 50 << 60 << 70 << 80 << 90 << 100 << 120 << 140  << 200;
	secondjetpt_plots.at(cat_0bjet0jet) = addPlot(bins,bins,"second jet pt 0,0 b-jets","p_{T}} [GeV]","Events/GeV");
	secondjetpt_plots.at(cat_0bjet1jet) = addPlot(bins,bins,"second jet pt 0,1 b-jets","p_{T}} [GeV]","Events/GeV");
	secondjetpt_plots.at(cat_0bjet2jet) = addPlot(bins,bins,"second jet pt 0,2 b-jets","p_{T}} [GeV]","Events/GeV");
	secondjetpt_plots.at(cat_0bjet3jet) = addPlot(bins,bins,"second jet pt 0,3 b-jets","p_{T}} [GeV]","Events/GeV");
	bins.clear();
	bins      << 30 << 35  << 40 << 50 << 60 << 70 << 80 << 90 << 100 << 120 << 140  << 200;
	secondjetpt_plots.at(cat_1bjet0jet) = addPlot(bins,bins,"second jet pt 1,0 b-jets","p_{T}} [GeV]","Events/GeV");
	secondjetpt_plots.at(cat_1bjet1jet) = addPlot(bins,bins,"second jet pt 1,1 b-jets","p_{T}} [GeV]","Events/GeV");
	secondjetpt_plots.at(cat_1bjet2jet) = addPlot(bins,bins,"second jet pt 1,2 b-jets","p_{T}} [GeV]","Events/GeV");
	bins.clear();
	bins      << 30 << 35  << 40 << 50 << 60 << 70 << 80 << 90 << 100 << 120 << 200;
	secondjetpt_plots.at(cat_1bjet3jet) = addPlot(bins,bins,"second jet pt 1,3 b-jets","p_{T}} [GeV]","Events/GeV");
	bins.clear();
	bins  << 30  << 45 <<  65  << 80 << 100 << 120 << 160 << 300;
	secondjetpt_plots.at(cat_2bjet0jet) = addPlot(bins,bins,"second jet pt 2,0 b-jets","p_{T}} [GeV]","Events/GeV");
	secondjetpt_plots.at(cat_2bjet1jet) = addPlot(bins,bins,"second jet pt 2,1 b-jets","p_{T}} [GeV]","Events/GeV");
	bins.clear();
	bins  << 30  << 45 <<  65  << 80 << 100 << 120  << 300;
	secondjetpt_plots.at(cat_2bjet2jet) = addPlot(bins,bins,"second jet pt 2,2 b-jets","p_{T}} [GeV]","Events/GeV");
	secondjetpt_plots.at(cat_2bjet3jet) = addPlot(bins,bins,"second jet pt 2,3 b-jets","p_{T}} [GeV]","Events/GeV");

	bins.clear();
	bins<< 30 << 35  << 40 << 50 << 60 << 70 << 80 << 90 << 100 << 120 << 140 << 200;
	thirdjetpt_plots.at(cat_0bjet0jet) = addPlot(bins,bins,"third jet pt 0,0 b-jets","p_{T}} [GeV]","Events/GeV");
	thirdjetpt_plots.at(cat_0bjet1jet) = addPlot(bins,bins,"third jet pt 0,1 b-jets","p_{T}} [GeV]","Events/GeV");
	thirdjetpt_plots.at(cat_0bjet2jet) = addPlot(bins,bins,"third jet pt 0,2 b-jets","p_{T}} [GeV]","Events/GeV");
	bins.clear();
	bins << 30 << 35  << 40 << 50 << 60 << 70 << 80 << 90 << 100 << 200;
	thirdjetpt_plots.at(cat_0bjet3jet) = addPlot(bins,bins,"third jet pt 0,3 b-jets","p_{T}} [GeV]","Events/GeV");
	bins.clear();
	bins << 30 << 35  << 40 << 50 << 60 << 70 << 80 << 90 << 100 << 200;
	thirdjetpt_plots.at(cat_1bjet0jet) = addPlot(bins,bins,"third jet pt 1,0 b-jets","p_{T}} [GeV]","Events/GeV");
	thirdjetpt_plots.at(cat_1bjet1jet) = addPlot(bins,bins,"third jet pt 1,1 b-jets","p_{T}} [GeV]","Events/GeV");
	thirdjetpt_plots.at(cat_1bjet2jet) = addPlot(bins,bins,"third jet pt 1,2 b-jets","p_{T}} [GeV]","Events/GeV");
	bins.clear();
	bins << 30 << 35  << 40 << 50 << 60 << 80 << 200;
	thirdjetpt_plots.at(cat_1bjet3jet) = addPlot(bins,bins,"third jet pt 1,3 b-jets","p_{T}} [GeV]","Events/GeV");
	bins.clear();
	bins << 30    << 55  << 70  << 90  << 200;
	thirdjetpt_plots.at(cat_2bjet0jet) = addPlot(bins,bins,"third jet pt 2,0 b-jets","p_{T}} [GeV]","Events/GeV");
	thirdjetpt_plots.at(cat_2bjet1jet) = addPlot(bins,bins,"third jet pt 2,1 b-jets","p_{T}} [GeV]","Events/GeV");
	bins.clear();
	bins << 30    << 55  << 70  <<  200;
	thirdjetpt_plots.at(cat_2bjet2jet) = addPlot(bins,bins,"third jet pt 2,2 b-jets","p_{T}} [GeV]","Events/GeV");
	bins.clear();
	bins << 30    << 50  <<  200;
	thirdjetpt_plots.at(cat_2bjet3jet) = addPlot(bins,bins,"third jet pt 2,3 b-jets","p_{T}} [GeV]","Events/GeV");


}


void analysisPlotsTtbarXsecFit::fillPlotsGen(){
	if(!use()) return;
	if(!event()) return;
	using namespace std;
	using namespace ztop;
	//define vis PS here
	/*
	 * Following the ATLAS definition, taking into account leptons after FSR/PS (Pythia stat 1)
	 * Including intermediate tau decays (or not)
	 */

	std::vector<NTGenParticle*> genvisleptons1=produceCollection(event()->genleptons1,20,2.4);
	totevts_++;
	totevtsw_+=puweight();
	//only fill one bin in some visible part of the histogram to get the total
	// n_gen and a nice display of PS migrations
	if(genvisleptons1.size()>1){
		for(size_t i=0;i<total_plots.size();i++){
			total_plots.at(i)->fillGen(0.5,puweight());
			leadjetpt_plots.at(i)->fillGen(20.5,puweight());
			secondjetpt_plots.at(i)->fillGen(20.5,puweight());
			thirdjetpt_plots.at(i)->fillGen(20.5,puweight());
		}
		vispsevts_++;
		vispsevtsw_+=puweight();
	}


}

void analysisPlotsTtbarXsecFit::fillPlotsReco(){
	if(!use()) return;
	if(!event()) return;
	using namespace std;
	using namespace ztop;
	size_t nbjets=0;
	size_t naddjets=0;
	if(event()->selectedbjets && event()->selectedjets){
		nbjets=event()->selectedbjets->size();
		naddjets=event()->selectedjets->size() - nbjets;

		setJetCategory(nbjets,naddjets);
		if(naddjets==0)
			total_plots.at(jetcategory)->fillReco(0.5,puweight());
		else if(naddjets==1)
			leadjetpt_plots.at(jetcategory)->fillReco(event()->selectedjets->at(0)->pt(),puweight());
		else if(naddjets==2)
			secondjetpt_plots.at(jetcategory)->fillReco(event()->selectedjets->at(1)->pt(),puweight());
		else if(naddjets>2){
			thirdjetpt_plots.at(jetcategory)->fillReco(event()->selectedjets->at(event()->selectedjets->size()-1)->pt(),puweight());
		}
	}
}


void analysisPlotsTtbarXsecFit::setJetCategory(size_t nbjets,size_t njets){
	if(nbjets < 1 || nbjets>2){
		if(njets<1)
			jetcategory=cat_0bjet0jet;
		else if(njets<2)
			jetcategory=cat_0bjet1jet;
		else if(njets<3)
			jetcategory=cat_0bjet2jet;
		else if(njets>2)
			jetcategory=cat_0bjet3jet;
	}
	else if(nbjets < 2){
		if(njets<1)
			jetcategory=cat_1bjet0jet;
		else if(njets<2)
			jetcategory=cat_1bjet1jet;
		else if(njets<3)
			jetcategory=cat_1bjet2jet;
		else if(njets>2)
			jetcategory=cat_1bjet3jet;
	}
	else if(nbjets < 3){
		if(njets<1)
			jetcategory=cat_2bjet0jet;
		else if(njets<2)
			jetcategory=cat_2bjet1jet;
		else if(njets<3)
			jetcategory=cat_2bjet2jet;
		else if(njets>2)
			jetcategory=cat_2bjet3jet;
	}
}


}//ns
