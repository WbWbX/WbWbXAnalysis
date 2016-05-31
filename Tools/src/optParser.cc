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


std::string optParser::optionParsed(const std::string& opt, bool& found,bool noerase){
	std::string out="";
	found=false;
	for(size_t i=0;i<temp_.size();i++){
		if(temp_.at(i) == "-"+opt){
			found=true;
			if (i + 1 != temp_.size() && !noerase){
				out=temp_.at(i+1);
				if(out.find("-")!=0) //also an option
					temp_.erase(temp_.begin()+i, temp_.begin()+i+2);
				else
					temp_.erase(temp_.begin()+i);
				break;
			}
			else{
				temp_.erase(temp_.begin()+i);
				break;
			}
		}
	}
	if(debug){
		std::cout << "parsed " << opt <<": "<< out << " "<< toStdString(found) <<std::endl;
		std::cout << "remaining to parse: ";
		for(size_t i=0;i<temp_.size();i++)
			std::cout << " " <<temp_.at(i);
		std::cout << std::endl;
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
	out+=textFormatter::splitIntoLines(in,65,15,1);

	if(def.length()){
		std::string desc="default: "+def;
		out+="\n";
		out+=textFormatter::splitIntoLines(desc,65,15,0);
	}
	return out;

}
}

