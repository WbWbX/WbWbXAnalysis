/*
 * fileReader.h
 *
 *  Created on: Oct 10, 2013
 *      Author: kiesej
 */

#ifndef FILEREADER_H_
#define FILEREADER_H_


#include <string>
#include <vector>
#include <sstream>

namespace ztop{

class fileReader{
public:
	fileReader():trim_(" \t"),comment_("#"),delimiter_(","){}
	~fileReader(){clear();}
	void setTrim(const std::string& tr){trim_=tr;}
	void setComment(const std::string& c){comment_=c;}
	void setDelimiter(const std::string& d){delimiter_=d;}

	std::string & ltrim(std::string & str) const;
	std::string & rtrim(std::string & str) const;
	std::string & trim(std::string & str) const;
	std::string & trimcomments(std::string & str) const;

	void readFile(const std::string& filename);
	size_t nLines()const{return lines_.size();}
	size_t nEntries(const size_t &line)const{return getData(line).size();}
	std::vector<std::string> getData(const size_t &line) const;
	template<class T>
	T getData(const size_t &line,const size_t &entry) const{
		T out;
		std::stringstream ss(getData(line).at(entry));
		ss >> out;
		return out;
	}


	void clear(){lines_.clear();}

private:
	std::string trim_,comment_,delimiter_;
	std::vector<std::vector<std::string> > lines_;

};

template<>
bool fileReader::getData<bool>(const size_t &line,const size_t &entry) const{
	bool out;
	std::stringstream ss(getData(line).at(entry));
	ss >> std::boolalpha >> out;
	return out;
}

}



#endif /* FILEREADER_H_ */
