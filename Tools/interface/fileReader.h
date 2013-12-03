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
	fileReader():trim_(" \t"),comment_("#"),delimiter_(","),start_(""),end_(""),blindmode_(false){}
	~fileReader(){clear();}
	void setTrim(const std::string& tr){trim_=tr;}
	void setComment(const std::string& c){comment_=c;}
	void setDelimiter(const std::string& d){delimiter_=d;}
	void setStartMarker(const std::string& d){start_=d;}
	void setEndMarker(const std::string& d){end_=d;}

	void setBlindMode(bool blind){blindmode_=blind;}

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

	std::string getValueString(const std::string &, bool checkdoubles=true);

	template<class T>
	T getValue(const std::string & str, bool checkdoubles=true){
		T out;
		std::stringstream ss(getValueString(str,checkdoubles));
		ss >> out;
		return out;
	}

	void clear(){lines_.clear();}
	static bool debug;

private:
	std::string trim_,comment_,delimiter_,start_,end_;
	std::vector<std::vector<std::string> > lines_;
	bool blindmode_;
};


template<>
inline bool fileReader::getData<bool>(const size_t &line,const size_t &entry) const{
	bool out;
	std::stringstream ss(getData(line).at(entry));
	ss >> std::boolalpha >> out;
	return out;
}
template<>
inline bool fileReader::getValue<bool>(const std::string & str, bool checkdoubles){
	bool out;
	std::stringstream ss(getValueString(str,checkdoubles));
	ss >> std::boolalpha >> out;
	return out;
}


}



#endif /* FILEREADER_H_ */
