/*
 * analyzer_run1.h
 *
 *  Created on: Mar 31, 2015
 *      Author: kiesej
 */

#ifndef ANALYZER_RUN1_H_
#define ANALYZER_RUN1_H_

#include "../interface/topAnalyzer.h"
#include "WbWbXAnalysis/DataFormats/interface/NTLepton.h"
class top_analyzer_run1 : public ztop::topAnalyzer{
public:

	top_analyzer_run1():topAnalyzer(){}

	void analyze(size_t i);
	bool checkTrigger(std::vector<bool> * ,ztop::NTEvent * , bool , size_t);

	//no copy
private:
	top_analyzer_run1(const top_analyzer_run1 &);
	top_analyzer_run1& operator = (const top_analyzer_run1 &);

	bool passesLeptonId(const ztop::NTLepton* , bool )const;

};



#endif /* ANALYZER_RUN1_H_ */
