/*
 * wAnalyzer.h
 *
 *  Created on: 23 May 2016
 *      Author: kiesej
 */

#ifndef TTZANALYSIS_ANALYSIS_INTERFACE_WANALYZER_H_
#define TTZANALYSIS_ANALYSIS_INTERFACE_WANALYZER_H_

#include "basicAnalyzer.h"
#include "TString.h"

namespace ztop{
class wAnalyzer : public basicAnalyzer{
public:
	wAnalyzer():basicAnalyzer(){}
	~wAnalyzer(){}


private:
	fileForker::fileforker_status writeOutput();

	void analyze(size_t id);



};

}


#endif /* TTZANALYSIS_ANALYSIS_INTERFACE_WANALYZER_H_ */
