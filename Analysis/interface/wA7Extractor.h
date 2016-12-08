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
#include "TtZAnalysis/Tools/interface/graph.h"

namespace ztop{

class wA7Extractor: public tObjectList{//just for garbage collection
public:
	wA7Extractor():tObjectList(),debug(false),wparaPrefix_("WparaA"),
	npseudo_(0),enhancestat_(true),mcstatthresh_(0.02){}
	~wA7Extractor(){}

	void readConfig(const std::string& infile);

	void loadPlots(const std::string& infile);

	void printVariations(const std::string& outdir)const;

	//get some results
	void setNPseude(size_t n){npseudo_=n;}


	void setEnhanceStat(bool set){enhancestat_=set;}
	void setMCStatThreshold(float thresh){mcstatthresh_=thresh;}

	//make some plots


	void fitAll();

	bool debug;

private:



	graph fit(int sysindex, bool up, bool fixallbuta7,bool fixa7=false);

	void createVariates(const std::vector<histoStack>& in, bool onlyMC,
			TRandom3* rand);

	/**
	 * returns new binning
	 */
	std::vector<float> enhanceStatistics(histoStack& stack)const;


	std::pair<std::string, size_t> parameterasso_;

	//std::vector<histo1D>
	std::vector<histoStack> orig_inputstacks_;
	std::vector<histoStack> orig_inputstacks_noniso_;
	std::vector<histoStack> normfitted_inputstacks_;
	//! these have QCD from data
	std::vector<histoStack> inputstacks_;

	std::vector<variateHisto1D> mcdependencies_;
	std::vector<histo1D> datahistos_;

	std::vector<float> detaranges_;

	simpleFitter fitter_;
	ROOT::Math::Functor functor_;
	double toBeMinimized(const double * variations);
	std::vector<double> fittedparas_;

	const TString wparaPrefix_;

	size_t npseudo_;
	bool enhancestat_;
	float mcstatthresh_;
};

}



#endif /* TTZANALYSIS_ANALYSIS_INTERFACE_WA7EXTRACTOR_H_ */
