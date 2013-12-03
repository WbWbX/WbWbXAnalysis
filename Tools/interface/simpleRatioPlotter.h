/*
 * simpleRatioPlotter.h
 *
 *  Created on: Nov 8, 2013
 *      Author: kiesej
 */

#ifndef SIMPLERATIOPLOTTER_H_
#define SIMPLERATIOPLOTTER_H_
#include "simplePlotterBase.h"
#include "container.h"
#include "TString.h"
#include <vector>

class TCanvas;
namespace ztop{

class simpleRatioPlotter : public simplePlotterBase{

public:
	simpleRatioPlotter():simplePlotterBase(){}
	~simpleRatioPlotter();

	//void setStyle

	void drawPlots(const std::vector<container1D> & , const std::vector<int> &);
	void drawRatio(const std::vector<container1D> & , const std::vector<int> &);
	TCanvas * plotAll(const TString& , const std::vector<container1D> & , const std::vector<int> & colz = std::vector<int>());

	void setDrawOption(TString opt){drawopt_=opt;}


private:
	std::vector<int> checkSizes(const std::vector<container1D> & , const std::vector<int> &);
	TString drawopt_;

/**/

};


}



#endif /* SIMPLERATIOPLOTTER_H_ */
