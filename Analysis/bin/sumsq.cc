/*
 * sumsq.cc
 *
 *  Created on: Mar 19, 2014
 *      Author: kiesej
 */

#include "WbWbXAnalysis/Tools/interface/optParser.h"
#include <cmath>
#include <vector>
#include <iostream>

#include "WbWbXAnalysis/Tools/interface/applicationMainMacro.h"

invokeApplication(){
	using namespace ztop;
	parser->setAdditionalDesciption("Just sums or subtracts numbers in quadrature.");
	parser->bepicky=false;

	std::vector<float> nums=parser->getRest<float>();
	parser->doneParsing();
	float sum2=0;
	for(size_t i=0;i<nums.size();i++)
		if(nums.at(i)>0)
			sum2+=nums.at(i)*nums.at(i);
		else
			sum2-=nums.at(i)*nums.at(i);

	std::cout <<  sqrt(sum2) <<std::endl;

	return 0;
}
