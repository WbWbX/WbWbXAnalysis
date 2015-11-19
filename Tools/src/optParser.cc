/*
 * optParser.cc
 *
 *  Created on: Feb 27, 2014
 *      Author: kiesej
 */




#include "../interface/optParser.h"
#include "../interface/textFormatter.h"
#include <iostream>

namespace ztop{

bool optParser::debug=false;


std::string optParser::optionParsed(const std::string& opt, bool& found){
	std::string out="";
	for(size_t i=0;i<temp_.size();i++){
		if(temp_.at(i) == "-"+opt){
			found=true;
			if (i + 1 != temp_.size()){
				out=temp_.at(i+1);
				if(debug) std::cout << "optparser::getOpt: got "<< temp_.at(i) << " " << temp_.at(i+1) <<std::endl;
				temp_.erase(temp_.begin()+i, temp_.begin()+i+2);
			}
		}
	}
	return out;
}

void optParser::doneParsing(){
	getRest<std::string>();
	bool helpa=getOpt<bool>("h",false,"display this help message");
	bool helpb=getOpt<bool>("-help",false,"display this help message");
	if(helpa || helpb)
		coutHelp();
}

void optParser::coutHelp()const{
	using namespace std;
	std::cout << '\n'<< progname_ << ":\n" << adddescr_ << "\n\nOPTIONS: "<<std::endl;
	for(size_t i=0;i<help_.size();i++){
		std::cout  << std::endl;
			std::cout  << help_.at(i) << std::endl;
	}
	std::cout  << std::endl;
	exit(EXIT_SUCCESS);

}

void optParser::setAdditionalDesciption(std::string desc){
	adddescr_=textFormatter::splitIntoLines(desc,80,0);
}

std::string optParser::createDescription(const std::string& opt, const std::string& in, const std::string& def)const{
	//if(in.Length() < 60) return in;
	std::string out="  -";
	out+=opt;
	if((int)15-(int)out.length()>0)
		out+= std::string(15-out.length(),' ');
	else
		out+=" ";
	//out+=in;
	std::string desc;
	if(def.length())
		desc+="default: "+def;
	out+=textFormatter::splitIntoLines(in,65,15,1);
	out+="\n";
	out+=textFormatter::splitIntoLines(desc,65,15,0);
	return out;

}
}

