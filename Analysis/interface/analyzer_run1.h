/*
 * analyzer_run1.h
 *
 *  Created on: Mar 31, 2015
 *      Author: kiesej
 */

#ifndef ANALYZER_RUN1_H_
#define ANALYZER_RUN1_H_

#include "../interface/MainAnalyzer.h"

class analyzer_run1 : public MainAnalyzer{
public:

	analyzer_run1():MainAnalyzer(){}

	void analyze(size_t i);
	bool checkTrigger(std::vector<bool> * ,ztop::NTEvent * , bool , size_t);

	//no copy
private:
	analyzer_run1(const analyzer_run1 &);
	analyzer_run1& operator = (const analyzer_run1 &);


};



#endif /* ANALYZER_RUN1_H_ */
