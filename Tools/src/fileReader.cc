/*
 * fileReader.cc
 *
 *  Created on: Oct 10, 2013
 *      Author: kiesej
 */
#include "../interface/fileReader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace ztop{



bool fileReader::debug=false;




/**
 * cuts on right side of the string
 * example:
 * 	   setTrim(#)
 * 	   string="#######this is a string####"
 * 	   rtrim(string)
 *     now: string="#######this is a string"
 */
std::string & fileReader::rtrim(std::string & str) const{
	size_t endpos = str.find_last_not_of(trim_);
	if( std::string::npos != endpos )
	{
		str = str.substr( 0, endpos+1 );
	}
	return str;
}
/**
 * cuts on right side of the string
 * example:
 * 	   setTrim(#)
 * 	   string="#######this is a string####"
 * 	   ltrim(string)
 *     now: string="this is a string####"
 */
std::string & fileReader::ltrim(std::string & str) const{
	size_t startpos = str.find_first_not_of(trim_);
	if( std::string::npos != startpos )
	{
		str = str.substr(startpos );
	}
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
std::string & fileReader::trimcomments(std::string & str) const{
	size_t endpos = str.find(comment_);
	if( std::string::npos != endpos)
	{
		str = str.substr( 0, endpos);
	}
	return str;
}

// trim from both ends
std::string &fileReader::trim(std::string &s) const {
	return ltrim(rtrim(s));
}




void fileReader::readFile(const std::string &filename){

	using namespace std;
	lines_.clear();
	ifstream myfile (filename.data());
	if (myfile.is_open())
	{
		if(debug){
			std::cout << "fileReader::readFile: opened file.\n searching for start marker " << start_
					<<" until end marker " << end_ << "\n"
					<< "delimiter: " << delimiter_ << " comments: " << comment_ << std::endl;

		}
		if(blindmode_){
			vector <string> record;
			lines_.push_back(record);
		}
		bool started=false;
		bool ended=false;
		while ( myfile.good() )
		{
			if(ended)
				break;
			string s;

			if (!getline( myfile, s )) break;

			if(blindmode_){
				lines_.at(0).push_back(s);
				continue;
			}
			else{
				trimcomments(s);
				trim(s);
			}

			if(s.size()<1)
				continue;
			if(!blindmode_ && !started && start_.size() !=0 && s.find(start_) == string::npos) //wait for start marker on line basis
				continue;

			if(debug && !started)
				std::cout << "found start marker in line" << std::endl;

			//if(end_.size() !=0 && s.compare(end_) == 0) //wait for start marker
			//break;

			istringstream ss( s );
			vector <string> record;
			bool noentry=true;
			while (ss)
			{

				string s2;
				if (!getline( ss, s2, *delimiter_.data() )) break;
				if(!started && start_.size() !=0 && s2 != start_){ //wait for start marker on word by word basis
					if(debug)
						std::cout << s2 << " not yet startmarker" << std::endl;
					continue;
				}
				else if(!started){
					started=true;
					if(debug)
						std::cout << s2 << " is start marker, begin reading" << std::endl;
					continue; //dont read start marker itself
				}
				if(end_.size() >0 && s2 == end_){
					if(debug)
						std::cout << s2 << " is end marker, stop reading" << std::endl;
					ended=true;
					break;
				}
				if(debug)
				    std::cout << "read \"" << s2 << "\""<<std::endl;
				trim(s2);
                if(debug)
                    std::cout << "trimmed to \"" << s2 << "\"" << std::endl;
				if(s2.size()<1)
					continue;
				noentry=false;
				record.push_back( s2 );
			}
			if(!ended && !noentry){
				lines_.push_back(record);
				record.clear();
			}
		}

		myfile.close();
	}
	else{
		cout << "fileReader::readFile: could not read file" <<std::endl;
		return;
	}
}
std::vector<std::string> fileReader::getData(const size_t &line) const{
	if(line>=lines_.size()){
		std::cout << "fileReader::getData: line out of range" <<std::endl;
		return std::vector<std::string>();
	}
	return lines_.at(line);
}
/**
 * if file has entries like somevariable=blabla
 * getValue("somevariable") will return "blabla"
 * if value is definied several times, an exception is thrown
 * less performance but safer
 * can be switched off by bool
 * then last value is returned
 */
std::string fileReader::getValueString(const std::string & val, bool checkdoubles){
	std::string out;
	size_t count=0;
	//search all entries:
	for(size_t line=0;line<nLines();line++){
		for(size_t entr=0;entr<nEntries(line);entr++){
			std::string & s=lines_.at(line).at(entr);
			size_t pos=s.find(val);
			if(pos != std::string::npos){ //found
				if(checkdoubles && count > 0){//except
					throw std::logic_error("fileReader::getValue: value defined twice! Source of errors!");
				}
				pos=s.find("=");
				if(pos!= std::string::npos){
					if(pos+1 >= s.size())
						throw std::logic_error("fileReader::getValue: value definition empty");
					std::string s2(s.begin()+pos+1,s.end());
					trim(s2);
					if(s2.size()<1)
						throw std::logic_error("fileReader::getValue: value definition empty");
					out=s2;
					count++;
				}
			}
		}
	}
	return out;
}
}//ztop
