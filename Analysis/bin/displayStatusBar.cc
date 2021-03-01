/*
 * diplayStatusBar.cc
 *
 *  Created on: Apr 23, 2015
 *      Author: kiesej
 */




#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "WbWbXAnalysis/Tools/interface/applicationMainMacro.h"
#include "WbWbXAnalysis/Tools/interface/formatter.h"

invokeApplication(){
	using namespace ztop;
	std::vector<float> inputs=parser->getRest<float>();

	if(inputs.size() != 2){
		std::cerr << "provide exactly two arguments: current and max" <<std::endl;
		return -1;
	}
	formatter fmt;

	Long64_t all=fmt.round(inputs.at(1)*1000,1);
	Long64_t pos=fmt.round(inputs.at(0)*1000,all/100);
	std::cout << std::endl;
	displayStatusBar(pos,all,100,true);


	return 0;
}
