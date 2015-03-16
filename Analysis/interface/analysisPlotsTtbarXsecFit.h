/*
 * analysisPlotsJan.h
 *
 *  Created on: Feb 12, 2014
 *      Author: kiesej
 */

#ifndef ANALYSISPLOTSTTXSECFIT_H_
#define ANALYSISPLOTSTTXSECFIT_H_

#include "analysisPlots.h"

namespace ztop{
/**
 * This is a misuse of the histo1DUnfold class
 */
class analysisPlotsTtbarXsecFit : public analysisPlots{
public:
	analysisPlotsTtbarXsecFit(size_t step):analysisPlots(step),jetcategory(cat_0bjet0jet)
	{
		leadjetpt_plots.resize(cat_bjetjetmax,0);
		secondjetpt_plots.resize(cat_bjetjetmax,0);
		thirdjetpt_plots.resize(cat_bjetjetmax,0);
		total_plots.resize(cat_bjetjetmax,0);
	}

	~analysisPlotsTtbarXsecFit(){/* destruction is handled in base class! */}

	void bookPlots();
	void fillPlotsReco();
	void fillPlotsGen();

private:
	enum bjetnjetscategories{
		cat_0bjet0jet,cat_0bjet1jet,cat_0bjet2jet,cat_0bjet3jet,
		cat_1bjet0jet,cat_1bjet1jet,cat_1bjet2jet,cat_1bjet3jet,
		cat_2bjet0jet,cat_2bjet1jet,cat_2bjet2jet,cat_2bjet3jet,cat_bjetjetmax}
	jetcategory;

	void setJetCategory(size_t nbjets,size_t njets);



	std::vector<histo1DUnfold*>
	leadjetpt_plots, secondjetpt_plots, thirdjetpt_plots, total_plots;

};

}
#endif /* analysisPlotsTtbarXsecFit_H_ */
