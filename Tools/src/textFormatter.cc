/*
 * textFormatter.cc
 *
 *  Created on: May 6, 2014
 *      Author: kiesej
 */


#include "../interface/textFormatter.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

namespace ztop{

bool textFormatter::debug=false;


/**
 * cuts on right side of the string
 * example:
 *     setTrim(#)
 *     string="#######this is a string####"
 *     rtrim(string)
 *     now: string="#######this is a string"
 */
std::string & textFormatter::rtrim(std::string & str) const{
	if(trim_.length()<1) return str;
	size_t endpos = str.find_last_not_of(trim_);
	if( std::string::npos != endpos )
	{
		str = str.substr( 0, endpos+1 );
	}
	else
		str.clear(); //only contains trim chars
	return str;
}
/**
 * cuts on right side of the string
 * example:
 *     setTrim(#)
 *     string="#######this is a string####"
 *     ltrim(string)
 *     now: string="this is a string####"
 */
std::string & textFormatter::ltrim(std::string & str) const{
	if(trim_.length()<1) return str;

	size_t startpos = str.find_first_not_of(trim_);
	if( std::string::npos != startpos )
	{
		str = str.substr(startpos );
	}
	else
		str.clear(); //only contains trim chars
	return str;
}
/**
 * cuts everything that follows the comment marker (one char)
 * example:
 *     setComment("&")
 *     string="this is a string &with a comment"
 *     trimcomments(string)
 *     now: string=="this is a string "
 */
std::string & textFormatter::trimcomments(std::string & str) const{
	if(comment_.length()<1) return str;
	if(str.length() <2 && str==comment_) str="";
	size_t endpos = str.find(comment_);
	if( std::string::npos != endpos)
	{
		str = str.substr( 0, endpos);
	}

	return str;
}

// trim from both ends
std::string &textFormatter::trim(std::string &s) const {
	ltrim(rtrim(s));

	return s;
}



//static member functions

std::string textFormatter::getFilename(const std::string& pathtofile){
	std::string out;
	using namespace std;
	string s=pathtofile;
	istringstream ss(s);
	while (ss)
	{
		string s2;
		if (!getline( ss, s2, *"/" )) break;
		if(debug)
			std::cout << "got \"" << s2 << "\""<<std::endl;
		if(s2.size()>0)
			out = s2;
	} //just keep last

	return out;
}


std::string textFormatter::getFileExtension(const std::string& pathtofile){
	std::string out=getFilename(pathtofile);
	using namespace std;
	string s=out;
	istringstream ss(s);
	while (ss)
	{
		string s2;
		if (!getline( ss, s2, *"." )) break;
		if(debug)
			std::cout << "got \"" << s2 << "\""<<std::endl;
		if(s2.size()>0)
			out = s2;
	} //just keep last

	//in case no extension
	if(out==s)
		return "";
	return out;
}
std::string textFormatter::stripFileExtension(const std::string& pathtofile){
	std::string temp=getFilename(pathtofile);
	using namespace std;
	string s=temp;
	string out="";
	istringstream ss(s);
	size_t endpos = pathtofile.find_last_of(".");
	if( std::string::npos != endpos )
	{
		out = pathtofile.substr( 0, endpos);
	}


	return out;
}
std::string textFormatter::stripFileDir(const std::string& pathtofile){
	std::string temp=getFilename(pathtofile);
	using namespace std;
	string s=temp;
	string out="";
	istringstream ss(s);
	size_t endpos = pathtofile.find_last_of("/");
	if( std::string::npos != endpos )
	{
		out = pathtofile.substr( endpos+1, pathtofile.length());
	}

	return out;
}

std::string textFormatter::getFileDir(const std::string& pathtofile){
	using namespace std;
	string str=pathtofile;
	size_t endpos = str.find_last_of("/");
	if( std::string::npos != endpos )
	{
		str = str.substr( 0, endpos+1 );
	}
	return str;

}
std::string textFormatter::addFilenameSuffix(const std::string& pathtofile, const std::string& suffix){

	std::string onlyname=stripFileExtension(pathtofile);

	std::string extension=getFileExtension(pathtofile);
	return onlyname+suffix+"."+extension;

}

std::string textFormatter::makeCompatibleFileName(const std::string &in){
	std::string out=in;
	std::replace( out.begin(), out.end(), '#', '_');
	std::replace( out.begin(), out.end(), '/', '_');
	std::replace( out.begin(), out.end(), '{', '_');
	std::replace( out.begin(), out.end(), '}', '_');
	std::replace( out.begin(), out.end(), ' ', '_');
	std::replace( out.begin(), out.end(), '\\', '_');
	std::replace( out.begin(), out.end(), '-', '_');
	return out;
}

}//ns
