/*
 * testRetsort.cc
 *
 *  Created on: Jul 31, 2014
 *      Author: kiesej
 */



#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"


int main(){
	using namespace ztop;

	std::vector<float> vec;

	vec << 9 << 8 << 7 << 5 << 6 << 2 << 1 << 0 << -3;

	std::vector<float> vec2=vec;

	std::vector<size_t> ids=retsort(vec.begin(),vec.end(),std::greater<int>());
	for(size_t i=0;i<vec.size();i++){
	std::cout << vec2.at(i) << " " << ids.at(i) << "    " << vec.at(i) << " " <<vec2.at(ids.at(i))<<std::endl;

	}
}
