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
#include "TString.h"
#include <stdexcept>
#include <iostream>
#include "../interface/textFormatter.h"

namespace ztop{

class fileReader : public textFormatter{
public:
    fileReader(): textFormatter(), /*trim_(" \t"),comment_("#"),delimiter_(",") ,*/start_(""),end_(""),blindmode_(false),requirevalues_(true){}
    ~fileReader(){clear();}
    /*
    void setTrim(const std::string& tr){trim_=tr;}
    void setComment(const std::string& c){comment_=c;}
    void setDelimiter(const std::string& d){delimiter_=d;}
     */
    void setStartMarker(const std::string& d){start_=d;}
    void setEndMarker(const std::string& d){end_=d;}

    void setBlindMode(bool blind){blindmode_=blind;}
    void setRequireValues(bool req) {requirevalues_=req;}

    /*
    std::string & ltrim(std::string & str) const;
    std::string & rtrim(std::string & str) const;
    std::string & trim(std::string & str) const;
    std::string & trimcomments(std::string & str) const;
     */

    void readFile(const std::string& filename);
    bool isEmpty()const{return lines_.size()<1;}
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

    std::string getValueString(const std::string &str, bool checkdoubles=true);

    template<class T>
    T getValue(const std::string & str){
        T out;
        std::string s(getValueString(str,true));
        if(s.length()<1){
            std::cout << "fileReader::getValue: value for " << str << " not found in " << tempinfilename_<< std::endl;
            throw std::runtime_error("fileReader::getValue: no value found and no default defined!");
        }
        std::stringstream ss(s);
        ss >> out;
        return out;
    }
    template<class T>
    T getValue(const std::string & str,const T &def_val){
        if(requirevalues_)
            return getValue<T>(str);
        T out;
        std::string s(getValueString(str,true));
        if(s.length()>0){
            std::stringstream ss(s);
            ss >> out;
            return out;
        }
        else{
            return def_val;
        }
    }

    void clear(){lines_.clear();}
    static bool debug;

private:
    std::string /*trim_,comment_,delimiter_,*/start_,end_;
    std::vector<std::vector<std::string> > lines_;
    bool blindmode_;
    bool requirevalues_;
    std::string tempinfilename_;


//make inherited private
    std::vector<std::string> getFormatted(const std::string& in)const{return std::vector<std::string>();}

};


template<>
inline bool fileReader::getData<bool>(const size_t &line,const size_t &entry) const{
    bool out;
    std::stringstream ss(getData(line).at(entry));
    ss >> std::boolalpha >> out;
    return out;
}
template<>
inline bool fileReader::getValue<bool>(const std::string & str){
    bool out;
    std::string s(getValueString(str,true));
    if(s.length()<1){
        std::cout << "fileReader::getValue: value for " << str << " not found in " << tempinfilename_<< std::endl;
        throw std::runtime_error("fileReader::getValue: value required or no default defined!");
    }

    std::stringstream ss(s);
    ss >> std::boolalpha >> out;
    return out;
}
template<>
inline bool fileReader::getValue<bool>(const std::string & str,const bool& def_val){
    if(requirevalues_)
        return getValue<bool>(str);
    bool out;
    std::string s(getValueString(str,true));
    if(s.length()>0){
        std::stringstream ss(s);
        ss >> out;
        return out;
    }
    else{
        return def_val;
    }
}
template<>
inline std::string fileReader::getData<std::string>(const size_t &line,const size_t &entry) const{
    return (getData(line).at(entry));
}
template<>
inline std::string fileReader::getValue<std::string>(const std::string & str){
    std::string s(getValueString(str));
    if(s.length()<1){
        std::cout << "fileReader::getValue: value for " << str << " not found in " << tempinfilename_<< std::endl;
        throw std::runtime_error("fileReader::getValue: value required or no default defined!");
    }
    return s;
}
template<>
inline std::string fileReader::getValue<std::string>(const std::string & str, const std::string &  def_val){
    if(requirevalues_)
        return getValue<std::string>(str);
    std::string s(getValueString(str));
    if(s.length()<1)
        return def_val;
    return s;
}
template<>
inline TString fileReader::getData<TString>(const size_t &line,const size_t &entry) const{
    return (TString)(getData(line).at(entry));
}
template<>
inline TString fileReader::getValue<TString>(const std::string & str){
    std::string s(getValueString(str));
    if(s.length()<1){
        std::cout << "fileReader::getValue: value for " << str << " not found in " << tempinfilename_<< std::endl;
        throw std::runtime_error("fileReader::getValue: value required or no default defined!");
    }
    return (TString)s;
}
template<>
inline TString fileReader::getValue<TString>(const std::string & str, const TString & def_val){
    if(requirevalues_)
        return getValue<TString>(str);
    std::string s(getValueString(str));
    if(s.length()<1)
        return def_val;
    return (TString)s;
}

}



#endif /* FILEREADER_H_ */
