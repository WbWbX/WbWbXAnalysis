/*
 * analysisPlotsJan.h
 *
 *  Created on: Feb 12, 2014
 *      Author: kiesej
 */

#ifndef ANALYSISPLOTSMLBMT_H_
#define ANALYSISPLOTSMLBMT_H_

#include "analysisPlots.h"

namespace ztop{

class analysisPlotsMlbMt : public analysisPlots{
public:
	analysisPlotsMlbMt(size_t step):analysisPlots(step),
	Mlb(0),
	mlb(0),
	mlbmin(0),
	mlbivansbins(0),
	mlbminbsrad(0),
	leadleppt(0),
	mll(0),
	total(0),
	vistotal(0),
	mlbcombthresh_(165),
	rightassocounter_(0),totalcounter_(0)
{extraplots_.resize(cat_bjetjetmax,0);}

	~analysisPlotsMlbMt(){/* destruction is handled in base class! */
		if(totalcounter_)
			std::cout << "fraction of properly associated combinations for mlbmin: " << rightassocounter_/totalcounter_
			<< std::endl;

	}

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

	std::vector<histo1DUnfold*> extraplots_;

	histo1DUnfold
	*Mlb,
	*mlb,
	*mlbmin,
	*mlbivansbins,
	*mlbminbsrad,

	*leadleppt,
	*mll,

	*total,
	*vistotal;

	float mlbcombthresh_;
	float rightassocounter_,totalcounter_;

};

}
#endif /* ANALYSISPLOTSJAN_H_ */
