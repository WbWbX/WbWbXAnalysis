/*
 * wA7Extractor.h
 *
 *  Created on: 20 Jun 2016
 *      Author: jkiesele
 */

#ifndef TTZANALYSIS_ANALYSIS_INTERFACE_WA7EXTRACTOR_H_
#define TTZANALYSIS_ANALYSIS_INTERFACE_WA7EXTRACTOR_H_

#include <string>
#include "TtZAnalysis/Tools/interface/tObjectList.h"
#include "TtZAnalysis/Tools/interface/histoStack.h"
#include "TtZAnalysis/Tools/interface/variateHisto1D.h"
#include <algorithm>
#include "TtZAnalysis/Tools/interface/simpleFitter.h"

namespace ztop{

class wA7Extractor: public tObjectList{//just for garbage collection
public:
	wA7Extractor():tObjectList(){}
	~wA7Extractor(){}

	void readConfig(const std::string& infile);

	void loadPlots(const std::string& infile);


	//get some results



	//make some plots


	void fitAll();


private:
	void fit(int sysindex, bool up);

	void createVariates(const std::vector<histoStack>& in);


	std::pair<std::string, size_t> parameterasso_;

	//std::vector<histo1D>
	std::vector<histoStack> orig_inputstacks_;
	std::vector<histoStack> orig_inputstacks_noniso_;
	//! these have QCD from data
	std::vector<histoStack> inputstacks_;

	std::vector<variateHisto1D> mcdependencies_;
	std::vector<histo1D> datahistos_;

	simpleFitter fitter_;
	ROOT::Math::Functor functor_;
	double toBeMinimized(const double * variations);
	std::vector<double> fittedparas_;
};

}



#endif /* TTZANALYSIS_ANALYSIS_INTERFACE_WA7EXTRACTOR_H_ */
