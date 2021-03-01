/*
 * movecontribution.cc
 *
 *  Created on: Aug 24, 2015
 *      Author: kiesej
 */




#include "WbWbXAnalysis/Tools/interface/applicationMainMacro.h"
#include "WbWbXAnalysis/Tools/interface/histoStackVector.h"

invokeApplication(){
	using namespace ztop;
	parser->setAdditionalDesciption("simple program to replace a contribution in file1 from the one found in file0. Only works if present in both files. Output will be written to a new file.");

	const float scaler=parser->getOpt<float>("s",1,"scaler");
	const TString contribution=parser->getOpt<TString>("c","","contribution to be moved from first file to other.");

	std::vector<std::string> infiles=parser->getRest<std::string>();

	parser->doneParsing();

	if(infiles.size()!=2)
		return -1;

	std::cout << "reading..."<<std::endl;


	histoStackVector hsv1,hsv2;

	hsv1.readFromFile(infiles.at(0));
	hsv2.readFromFile(infiles.at(1));

	std::cout << "processing..."<<std::endl;
	for(size_t i=0;i<hsv1.size();i++){
		 histoStack  fromstack=hsv1.getStack(i);
		histoStack & stack=hsv2.getStack(fromstack.getName());

		size_t idx2=stack.getContributionIdx(contribution);
		size_t idx1=fromstack.getContributionIdx(contribution);

		if(stack.is1D()){
			stack.getContainer(idx2) = fromstack.getContainer(idx1) * scaler;
		}
		if(stack.is2D()){
			stack.getContainer2D(idx2) = fromstack.getContainer2D(idx1) * scaler;
		}
		if(stack.is1DUnfold()){
			stack.getContainer1DUnfold(idx2) = fromstack.getContainer1DUnfold(idx1) *scaler;
		}

	}

	hsv2.writeToFile(infiles.at(1)+"new.ztop");


	return 0;

}
