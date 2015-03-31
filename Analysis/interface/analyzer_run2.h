/*
 * analyzer_run1.h
 *
 *  Created on: Mar 31, 2015
 *      Author: kiesej
 */

#ifndef ANALYZER_RUN2_H_
#define ANALYZER_RUN2_H_

#include "../interface/MainAnalyzer.h"

class analyzer_run2 : public MainAnalyzer{
public:


	analyzer_run2():MainAnalyzer(){}


	void analyze(size_t i);
	bool checkTrigger(std::vector<bool> * ,ztop::NTEvent * , bool , size_t);

	//no copy
private:
	analyzer_run2(const analyzer_run2 &);
	analyzer_run2& operator = (const analyzer_run2 &);



};



#endif /* ANALYZER_RUN2_H_ */
