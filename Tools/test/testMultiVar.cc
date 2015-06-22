/*
 * testMultiVar.cc
 *
 *  Created on: May 8, 2015
 *      Author: kiesej
 */




#include "../interface/extendedVariable.h"


int main(){

	using namespace ztop;
	extendedVariable v1,v2,v3,v4;
	v1.addDependence(0.5,1,1,"bla");
	v2.addDependence(0.,1,2,"bla");
	v3.addDependence(0.1,1,3,"bla");
	v4.addDependence(0.1,1,4,"bla");

	extendedVariable v5 = v4+v1-v2*v3 ;// +v;//+(v -v) ;


	v2*=v2/v3;
	v2+=v4;
	v2*=v3;

	std::vector<double> var(1,1);

	extendedVariable::debugoperations=true;

	std::cout << v2.getMultiplicationFactor(var) <<std::endl;
	std::cout << std::endl;
	std::cout << v5.getValue(var) <<std::endl;

	return 0;
}
