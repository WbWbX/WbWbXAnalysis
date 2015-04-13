/*
 * testCorrM.cc
 *
 *  Created on: Apr 13, 2015
 *      Author: kiesej
 */




#include "../interface/corrMatrix.h"

int main(){
	using namespace ztop;

	std::vector<TString> names;
	names.push_back("bla1");
	names.push_back("bla2");
	names.push_back("bla3");
	names.push_back("bla4");
	names.push_back("bla5");

	corrMatrix m(names);

	for(size_t i=0;i<5;i++)
		for(size_t j=0;j<5;j++)
			 m.setEntry(i,j,3.*j*i) ;

	for(size_t i=0;i<5;i++){
		std::cout <<  m.getEntryName(i) << "   " ;
		for(size_t j=0;j<5;j++)
			std::cout <<  m.getEntry(i,j) << "   " ;
		std::cout<<std::endl;
	}

	return 0;
}
