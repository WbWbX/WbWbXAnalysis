/*
 * addVariation.cc
 *
 *  Created on: Dec 17, 2014
 *      Author: kiesej
 */


/*
 * Simple program to add a systematic variation to a file containing
 * the nominal points (other systematics allowed)
 * Don't use this to add large bunches of systematics, for this purpose use
 * mergeSyst with the appropriate naming conventions.
 * This program is only meant for small checks.
 *
 * To execute: addVariation <var_name> <nominal file> <up_file> <down_file> (optional -o <output>)
 */

#include "TtZAnalysis/Tools/interface/applicationMainMacro.h"
#include <vector>
#include "TString.h"
#include "TtZAnalysis/Tools/interface/containerStackVector.h"

invokeApplication(){
	using namespace ztop;
	parser->setAdditionalDesciption(
			"* Simple program to add a systematic variation to a file containing \n\
* the nominal points (other systematics allowed) \n\
* Don\'t use this to add large bunches of systematics, for this purpose use \n\
* mergeSyst with the appropriate naming conventions. \n\
* This program is only meant for small checks. \n\
*  \n\
* To execute: addVariation <var_name> <nominal file> <up_file> <down_file>");
	TString output=parser->getOpt<TString>("o","","specify output file name, otherwise nominal will be overwritten.");

	std::vector<TString> input=parser->getRest<TString>();

	if(input.size()!=4 || !fileExists(input.at(1)) || !fileExists(input.at(2)) || !fileExists(input.at(3))){
		parser->coutHelp(); //exits
	}
	if(output=="")
		output=input.at(1);

	containerStackVector* csvnom=new containerStackVector();
	containerStackVector* csvup=new containerStackVector();
	containerStackVector* csvdown=new containerStackVector();

	//try to load variations first (prob smaller)
	try{
		csvup->loadFromTFile(input.at(2));
	}catch(...){
		std::cout << "could not read up variation file"<<std::endl;
		delete csvnom,csvup,csvdown;
		return -1;
	}
	try{
		csvnom->loadFromTFile(input.at(1));
	}catch(...){
		std::cout << "could not read nominal file"<<std::endl;
		delete csvnom,csvup,csvdown;
		return -1;
	}

	csvnom->addMCErrorStackVector(input.at(0)+"_up",*csvup);
	delete csvup;

	try{
		csvdown->loadFromTFile(input.at(3));
	}catch(...){
		std::cout << "could not read down variation file"<<std::endl;
		delete csvnom,csvdown;
		return -1;
	}

	csvnom->addMCErrorStackVector(input.at(0)+"_down",*csvdown);
	delete csvdown;

	csvnom->writeAllToTFile(output,true);
	delete csvnom;
	return 0;
}
