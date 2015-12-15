/*
 * optParser.h
 *
 *  Created on: Feb 4, 2014
 *      Author: kiesej
 */

#ifndef OPTPARSER_H_
#define OPTPARSER_H_

#include "TString.h"
#include <cstdlib>
#include <string>
#include <vector>
#include <queue>
#include <iostream>
#include <stdexcept>
#include <sstream>

namespace ztop{

////options parser
/**
 * class to parse options in a handy way. Not much of error checking etc.
 * that would have to be done in main code
 *
 *
 * change to "getreset" or something (safe full strings in vector or something when setIn()
 *
 * The options -h and --help are reserved and will display a help message!
 *
 * Combining switch options can not be handled.
 *  E.g. executable -f -g would work,
 *  whereas executable -fg wouldn't give the same result
 *
 */
class optParser{
public:


	optParser():bepicky(false),argc_(0),argv_(0){}
	optParser(int ac,char** av):bepicky(false),argc_(ac),argv_(av){reload();}
	void setIn(int ac,char** av){
		argc_=ac;argv_=av;
		reload();
	}

	void setAdditionalDesciption(std::string desc);


	void reload(){
		temp_.clear();
		if(argv_[0])
			progname_=argv_[0];
		for(int i=1;i<argc_;i++){
			temp_.push_back((std::string)argv_[i]);
		}
	}


	template <class T>
	T getOpt(const std::string& opt,const T & def, const std::string & description="");

	/**
	 * get everything that is not parsed with "-" and was not parsed yet
	 * ignores -h and --help!
	 */
	template<class T>
	std::vector<T> getRest(){
		std::vector<T> out;
		for(size_t i=0;i<temp_.size();i++){
			if((temp_.at(i)).find("-")==0 ){
				if( bepicky && (std::string)temp_.at(i) != "-h" && (std::string)temp_.at(i) != "--help"){
					std::cout << "optParser: warning option "<<temp_.at(i) << " set but not parsed, yet" << std::endl;
					coutHelp(); //exits
					continue;
				}
			}
			if(debug) std::cout << "optparser::getRest: got "<< temp_.at(i) <<std::endl;
			out.push_back((T)temp_.at(i));
		}
		return out;
	}
	template<class T>
	T getRest(size_t idx){
		T out;
		std::vector<T> vec=getRest<T>();
		if(idx<vec.size())
			return vec.at(idx);
		else
			coutHelp();
		return out;
	}

	/**
	 * call this function after getting all options
	 */
	void doneParsing();
	void coutHelp()const;
	/**
	 * throws exception if one of the options given to the program was not parsed before getRest() is called
	 */
	bool bepicky;
	static bool debug;

private:

	int argc_;
	char** argv_;
	std::vector<std::string> temp_;
	std::vector<std::string> help_;
	std::string progname_;
	std::string adddescr_;

	std::string optionParsed(const std::string& opt, bool& found);
	std::string createDescription(const std::string& opt, const std::string& desc, const std::string& def)const;

	template <class T>
	T transform(const std::string& opt){
		return (T)opt;
	}
	template<class t>
	std::string toStdString(const t& in) {
		std::ostringstream s;
		s << in;
		std::string out = s.str();
		return out;
	}

};


template<>
std::string optParser::toStdString<bool>(const bool& in) {
	if(in)
		return "true";
	else
		return "false";
}

template<>
inline bool optParser::getOpt<bool>(const std::string& opt,const bool & def, const std::string & description){
	help_.push_back(createDescription(opt, description, toStdString(def)));
	bool dummy;
	std::string optval=optionParsed(opt,dummy);
	if(dummy)
		return !def;
	else
		return def;
}
template<>
inline float optParser::transform<float>(const std::string& opt){
	return atof(opt.data());
}
template<>
inline double optParser::transform<double>(const std::string& opt){
	return atof(opt.data());
}
template<>
inline int optParser::transform<int>(const std::string& opt){
	return atoi(opt.data());
}
template<>
inline TString optParser::transform<TString>(const std::string& opt){
	return (TString)(opt.data());
}

template <class T>
T optParser::getOpt(const std::string& opt,const T & def, const std::string & description){
	//search for option
	help_.push_back(createDescription(opt, description, toStdString(def)));
	bool dummy;
	std::string optval=optionParsed(opt,dummy);
	if(optval.length())
		return transform<T>(optval);
	else
		return def;
}



template<>
inline std::vector<float> optParser::getRest<float>(){
	std::vector<float> out;
	for(size_t i=0;i<temp_.size();i++){
		if(((std::string)temp_.at(i)).find("-") ==0){
			if( bepicky && (std::string)temp_.at(i) != "-h" && (std::string)temp_.at(i) != "--help"){
				std::cout << "optParser: warning option "<<temp_.at(i) << " set but not parsed, yet" << std::endl;

				throw std::runtime_error("optParser: begin picky: not all options parsed");
				continue;
			}

		}
		out.push_back(atof(temp_.at(i).data()));
	}
	return out;
}
template<>
inline std::vector<TString> optParser::getRest<TString>(){
	std::vector<TString> out;
	for(size_t i=0;i<temp_.size();i++){
		if(((std::string)temp_.at(i)).find("-")==0){
			if( bepicky && (std::string)temp_.at(i) != "-h" && (std::string)temp_.at(i) != "--help"){
				std::cout << "optParser: warning option "<<temp_.at(i) << " set but not parsed, yet" << std::endl;

				throw std::runtime_error("optParser: begin picky: not all options parsed");
				continue;
			}

		}
		out.push_back((temp_.at(i).data()));
	}
	return out;
}


}


#endif /* OPTPARSER_H_ */
