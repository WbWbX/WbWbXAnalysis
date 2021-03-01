/*
 * listSystematics.cc
 *
 *  Created on: Sep 3, 2014
 *      Author: kiesej
 */




#include "WbWbXAnalysis/Tools/interface/histoStackVector.h"
#include "WbWbXAnalysis/Tools/interface/optParser.h"

#include "WbWbXAnalysis/Tools/interface/applicationMainMacro.h"

invokeApplication(){
	using namespace ztop;
	using namespace std;

	//bool debug=true;

	// TString namescontain = parser.getOpt<TString>("-s","","");
	std::vector<TString> instrings=parser->getRest<TString>();

	if(instrings.size()!=1)
		return -1;

	histoStackVector csv;

	try{
		csv.readFromFile(instrings.at(0).Data());
	}
	catch(...){
		try{
			csv.loadFromTFile(instrings.at(0));
		}
		catch(...){
			std::cout << "Input file format not supported!" <<std::endl;
			return -1;
		}
	}

	if(csv.size()<1)
		return -2;

	for(size_t i=0;i<csv.size();i++){

		if(csv.getStack(i).is1D()){
			std::cout << "nsyst 1D: "   << csv.getStack(i).getSystSize() <<std::endl;
		}
		if(csv.getStack(i).is2D()){
			std::cout << "nsyst 2D: "   << csv.getStack(i).getSystSize() <<std::endl;

		}
		if(csv.getStack(i).is1DUnfold()){
			std::cout << "nsyst 1DUnf: " << csv.getStack(i).getSystSize() <<std::endl;

		}
	}
	return 0;
}
