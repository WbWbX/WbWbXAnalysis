/*
 * listSystematics.cc
 *
 *  Created on: Sep 3, 2014
 *      Author: kiesej
 */




#include "TtZAnalysis/Tools/interface/containerStackVector.h"
#include "TtZAnalysis/Tools/interface/optParser.h"


int main(int argc, char* argv[]){
	using namespace ztop;
	using namespace std;

	bool debug=true;

	optParser parser(argc,argv);
	// TString namescontain = parser.getOpt<TString>("-s","","");
	std::vector<TString> instrings=parser.getRest<TString>();

	if(instrings.size()!=1)
		return -1;

	containerStackVector csv;
	csv.loadFromTFile(instrings.at(0));

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
}