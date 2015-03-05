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


	std::vector<float> bins=container1D::createBinning(1,0,1);
	//leadjetpt_plots, secjetpt_plots, thirdjetpt_plots, total_plots;
	total_plots.at(cat_0bjet0jet) = addPlot(bins,bins,"total 0,0 b-jets","","N_{evt}");
	total_plots.at(cat_0bjet1jet) = addPlot(bins,bins,"total 0,1 b-jets","","N_{evt}");
	total_plots.at(cat_0bjet2jet) = addPlot(bins,bins,"total 0,2 b-jets","","N_{evt}");
	total_plots.at(cat_0bjet3jet) = addPlot(bins,bins,"total 0,3 b-jets","","N_{evt}");
	total_plots.at(cat_1bjet0jet) = addPlot(bins,bins,"total 1,0 b-jets","","N_{evt}");
	total_plots.at(cat_1bjet1jet) = addPlot(bins,bins,"total 1,1 b-jets","","N_{evt}");
	total_plots.at(cat_1bjet2jet) = addPlot(bins,bins,"total 1,2 b-jets","","N_{evt}");
	total_plots.at(cat_1bjet3jet) = addPlot(bins,bins,"total 1,3 b-jets","","N_{evt}");
	total_plots.at(cat_2bjet0jet) = addPlot(bins,bins,"total 2,0 b-jets","","N_{evt}");
	total_plots.at(cat_2bjet1jet) = addPlot(bins,bins,"total 2,1 b-jets","","N_{evt}");
	total_plots.at(cat_2bjet2jet) = addPlot(bins,bins,"total 2,2 b-jets","","N_{evt}");
	total_plots.at(cat_2bjet3jet) = addPlot(bins,bins,"total 2,3 b-jets","","N_{evt}");

	bins.clear();
	bins << 20 << 25 << 30 << 35  << 40 << 50 << 60 << 70 << 80 << 90 << 100 << 120 << 140 << 160 << 200;
	leadjetpt_plots.at(cat_0bjet0jet) = addPlot(bins,bins,"lead jet pt 0,0 b-jets","p_{t}^{1^{st}j} [GeV]","N_{j}");
	leadjetpt_plots.at(cat_0bjet1jet) = addPlot(bins,bins,"lead jet pt 0,1 b-jets","p_{t}^{1^{st}j} [GeV]","N_{j}");
	leadjetpt_plots.at(cat_0bjet2jet) = addPlot(bins,bins,"lead jet pt 0,2 b-jets","p_{t}^{1^{st}j} [GeV]","N_{j}");
	leadjetpt_plots.at(cat_0bjet3jet) = addPlot(bins,bins,"lead jet pt 0,3 b-jets","p_{t}^{1^{st}j} [GeV]","N_{j}");
	leadjetpt_plots.at(cat_1bjet0jet) = addPlot(bins,bins,"lead jet pt 1,0 b-jets","p_{t}^{1^{st}j} [GeV]","N_{j}");
	leadjetpt_plots.at(cat_1bjet1jet) = addPlot(bins,bins,"lead jet pt 1,1 b-jets","p_{t}^{1^{st}j} [GeV]","N_{j}");
	leadjetpt_plots.at(cat_1bjet2jet) = addPlot(bins,bins,"lead jet pt 1,2 b-jets","p_{t}^{1^{st}j} [GeV]","N_{j}");
	leadjetpt_plots.at(cat_1bjet3jet) = addPlot(bins,bins,"lead jet pt 1,3 b-jets","p_{t}^{1^{st}j} [GeV]","N_{j}");
	bins.clear();
	bins << 20  << 30  << 40 << 50 << 60 << 70 << 80 << 90 << 100 << 160 << 200;
	leadjetpt_plots.at(cat_2bjet0jet) = addPlot(bins,bins,"lead jet pt 2,0 b-jets","p_{t}^{1^{st}j} [GeV]","N_{j}");
	leadjetpt_plots.at(cat_2bjet1jet) = addPlot(bins,bins,"lead jet pt 2,1 b-jets","p_{t}^{1^{st}j} [GeV]","N_{j}");
	leadjetpt_plots.at(cat_2bjet2jet) = addPlot(bins,bins,"lead jet pt 2,2 b-jets","p_{t}^{1^{st}j} [GeV]","N_{j}");
	leadjetpt_plots.at(cat_2bjet3jet) = addPlot(bins,bins,"lead jet pt 2,3 b-jets","p_{t}^{1^{st}j} [GeV]","N_{j}");


	bins.clear();
	bins << 20 << 25 << 30 << 35  << 40 << 50 << 60 << 70 << 80 << 90 << 100 << 120 << 140 << 160 << 200;
	secondjetpt_plots.at(cat_0bjet0jet) = addPlot(bins,bins,"second jet pt 0,0 b-jets","p_{t}^{2^{nd}j} [GeV]","N_{j}");
	secondjetpt_plots.at(cat_0bjet1jet) = addPlot(bins,bins,"second jet pt 0,1 b-jets","p_{t}^{2^{nd}j} [GeV]","N_{j}");
	secondjetpt_plots.at(cat_0bjet2jet) = addPlot(bins,bins,"second jet pt 0,2 b-jets","p_{t}^{2^{nd}j} [GeV]","N_{j}");
	secondjetpt_plots.at(cat_0bjet3jet) = addPlot(bins,bins,"second jet pt 0,3 b-jets","p_{t}^{2^{nd}j} [GeV]","N_{j}");
	secondjetpt_plots.at(cat_0bjet3jet) ->setAllowMultiRecoFill(true); //little twist
	secondjetpt_plots.at(cat_1bjet0jet) = addPlot(bins,bins,"second jet pt 1,0 b-jets","p_{t}^{2^{nd}j} [GeV]","N_{j}");
	secondjetpt_plots.at(cat_1bjet1jet) = addPlot(bins,bins,"second jet pt 1,1 b-jets","p_{t}^{2^{nd}j} [GeV]","N_{j}");
	secondjetpt_plots.at(cat_1bjet2jet) = addPlot(bins,bins,"second jet pt 1,2 b-jets","p_{t}^{2^{nd}j} [GeV]","N_{j}");
	secondjetpt_plots.at(cat_1bjet3jet) = addPlot(bins,bins,"second jet pt 1,3 b-jets","p_{t}^{2^{nd}j} [GeV]","N_{j}");
	secondjetpt_plots.at(cat_1bjet3jet) ->setAllowMultiRecoFill(true); //little twist
	bins.clear();
	bins << 20  << 30  << 40 << 50 << 60 << 70 << 80 << 90 << 100 << 160 << 200;
	secondjetpt_plots.at(cat_2bjet0jet) = addPlot(bins,bins,"second jet pt 2,0 b-jets","p_{t}^{2^{nd}j} [GeV]","N_{j}");
	secondjetpt_plots.at(cat_2bjet1jet) = addPlot(bins,bins,"second jet pt 2,1 b-jets","p_{t}^{2^{nd}j} [GeV]","N_{j}");
	secondjetpt_plots.at(cat_2bjet2jet) = addPlot(bins,bins,"second jet pt 2,2 b-jets","p_{t}^{2^{nd}j} [GeV]","N_{j}");
	secondjetpt_plots.at(cat_2bjet3jet) = addPlot(bins,bins,"second jet pt 2,3 b-jets","p_{t}^{2^{nd}j} [GeV]","N_{j}");
	secondjetpt_plots.at(cat_2bjet3jet) ->setAllowMultiRecoFill(true); //little twist

	bins.clear();
	bins << 20 << 25 << 30 << 35  << 40 << 50 << 60 << 70 << 80 << 90 << 100 << 120 << 140 << 160 << 200;
	thirdjetpt_plots.at(cat_0bjet0jet) = addPlot(bins,bins,"third jet pt 0,0 b-jets","p_{t}^{3^{rd}j} [GeV]","N_{j}");
	thirdjetpt_plots.at(cat_0bjet1jet) = addPlot(bins,bins,"third jet pt 0,1 b-jets","p_{t}^{3^{rd}j} [GeV]","N_{j}");
	thirdjetpt_plots.at(cat_0bjet2jet) = addPlot(bins,bins,"third jet pt 0,2 b-jets","p_{t}^{3^{rd}j} [GeV]","N_{j}");
	thirdjetpt_plots.at(cat_0bjet3jet) = addPlot(bins,bins,"third jet pt 0,3 b-jets","p_{t}^{3^{rd}j} [GeV]","N_{j}");
	thirdjetpt_plots.at(cat_0bjet3jet) ->setAllowMultiRecoFill(true); //little twist
	thirdjetpt_plots.at(cat_1bjet0jet) = addPlot(bins,bins,"third jet pt 1,0 b-jets","p_{t}^{3^{rd}j} [GeV]","N_{j}");
	thirdjetpt_plots.at(cat_1bjet1jet) = addPlot(bins,bins,"third jet pt 1,1 b-jets","p_{t}^{3^{rd}j} [GeV]","N_{j}");
	thirdjetpt_plots.at(cat_1bjet2jet) = addPlot(bins,bins,"third jet pt 1,2 b-jets","p_{t}^{3^{rd}j} [GeV]","N_{j}");
	thirdjetpt_plots.at(cat_1bjet3jet) = addPlot(bins,bins,"third jet pt 1,3 b-jets","p_{t}^{3^{rd}j} [GeV]","N_{j}");
	thirdjetpt_plots.at(cat_1bjet3jet) ->setAllowMultiRecoFill(true); //little twist
	bins.clear();
	bins << 20  << 30  << 40 << 50 << 60 << 70 << 80 << 90 << 100 << 160 << 200;
	thirdjetpt_plots.at(cat_2bjet0jet) = addPlot(bins,bins,"third jet pt 2,0 b-jets","p_{t}^{3^{rd}j} [GeV]","N_{j}");
	thirdjetpt_plots.at(cat_2bjet1jet) = addPlot(bins,bins,"third jet pt 2,1 b-jets","p_{t}^{3^{rd}j} [GeV]","N_{j}");
	thirdjetpt_plots.at(cat_2bjet2jet) = addPlot(bins,bins,"third jet pt 2,2 b-jets","p_{t}^{3^{rd}j} [GeV]","N_{j}");
	thirdjetpt_plots.at(cat_2bjet3jet) = addPlot(bins,bins,"third jet pt 2,3 b-jets","p_{t}^{3^{rd}j} [GeV]","N_{j}");
	thirdjetpt_plots.at(cat_2bjet3jet) ->setAllowMultiRecoFill(true); //little twist


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

	std::vector<NTGenParticle*> genvisleptons1=produceCollection(event()->genleptons1,30,2.4);

	//only fill one bin in some visible part of the histogram to get the total
	// n_gen and a nice display of PS migrations
	if(genvisleptons1.size()>1){
		for(size_t i=0;i<total_plots.size();i++){
			total_plots.at(i)->fillGen(0.5,puweight());
			leadjetpt_plots.at(i)->fillGen(20.5,puweight());
			secondjetpt_plots.at(i)->fillGen(20.5,puweight());
			thirdjetpt_plots.at(i)->fillGen(20.5,puweight());
		}
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

		total_plots.at(jetcategory)->fillReco(0.5,puweight());
		if(naddjets>0)
			leadjetpt_plots.at(jetcategory)->fillReco(event()->selectedjets->at(0)->pt(),puweight());
		if(naddjets>1)
			secondjetpt_plots.at(jetcategory)->fillReco(event()->selectedjets->at(1)->pt(),puweight());
		if(naddjets>2){
			for(size_t i=2;i<naddjets;i++) //multi-fill
				thirdjetpt_plots.at(jetcategory)->fillReco(event()->selectedjets->at(i)->pt(),puweight());
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
