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

namespace ztop{

class wA7Extractor: public tObjectList{//just for garbage collection
public:
	wA7Extractor():tObjectList(){}
	~wA7Extractor(){}

	void readConfig(const std::string& infile);

	void loadPlots(const std::string& infile);

	void fit(){}

	//get some results



	//make some plots





private:

	//std::vector<histo1D>


};

}



#endif /* TTZANALYSIS_ANALYSIS_INTERFACE_WA7EXTRACTOR_H_ */
