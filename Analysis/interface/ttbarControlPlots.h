/*
 * ttbarControlPlots.h
 *
 *  Created on: Nov 1, 2013
 *      Author: kiesej
 */

#ifndef TTBARCONTROLPLOTS_H_
#define TTBARCONTROLPLOTS_H_

#include "controlPlotBasket.h"

namespace ztop{

class ttbarControlPlots : public controlPlotBasket{
public:
	ttbarControlPlots():controlPlotBasket(),jetcategory(cat_0bjet0jet){}
	~ttbarControlPlots(){}

	void makeControlPlots(const size_t& step);

protected:
	enum bjetnjetscategories{
		cat_0bjet0jet,cat_0bjet1jet,cat_0bjet2jet,cat_0bjet3jet,
		cat_1bjet0jet,cat_1bjet1jet,cat_1bjet2jet,cat_1bjet3jet,
		cat_2bjet0jet,cat_2bjet1jet,cat_2bjet2jet,cat_2bjet3jet}
	jetcategory;

	void setJetCategory(size_t nbjets,size_t njets);

};
}//namespace

#endif /* TTBARCONTROLPLOTS_H_ */
