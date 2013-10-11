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
namespace ztop{
std::string & fileReader::ltrim(std::string & str) const{
	size_t endpos = str.find_last_not_of(trim_);
	if( std::string::npos != endpos )
	{
		str = str.substr( 0, endpos+1 );
	}
	return str;
}

std::string & fileReader::trimcomments(std::string & str) const{
	size_t endpos = str.find(comment_);
	if( std::string::npos != endpos)
	{
		str = str.substr( 0, endpos);
	}
	return str;
}
std::string & fileReader::rtrim(std::string & str) const{
	size_t startpos = str.find_first_not_of(trim_);
	if( std::string::npos != startpos )
	{
		str = str.substr(startpos );
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
		while ( myfile.good() )
		{
			string s;

			if (!getline( myfile, s )) break;
			trimcomments(s);
			trim(s);
			if(s.size()<1)
				continue;
			istringstream ss( s );
			vector <string> record;

			while (ss)
			{
				string s2;
				if (!getline( ss, s2, *delimiter_.data() )) break;
				trim(s2);
				if(s2.size()<1)
					continue;
				record.push_back( s2 );
			}
			lines_.push_back(record);
			record.clear();
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
}//ztop
