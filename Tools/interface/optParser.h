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
#include <vector>
#include <iostream>
#include <stdexcept>

namespace ztop{

////options parser
/**
 * class to parse options in a handy way. Not much of error checking etc.
 * that would have to be done in main code
 *
 * CAUTION does not handle the following in a proper way:
 * program -<somebool> <this is an input file>
 * getInput() would not return anything here, because there is a switch right before the input
 *
 * change to "getreset" or something (safe full strings in vector or something when setIn()
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
    void reload(){
        temp_.clear();
        for(int i=1;i<argc_;i++){
            temp_.push_back((TString)argv_[i]);
        }
    }


    template <class T>
    T getOpt(const TString& opt,const T & def){
        for(size_t i=0;i<temp_.size();i++){
            if(temp_.at(i) == "-"+opt){
                if (i + 1 != temp_.size()){
                    T out=temp_.at(i+1);
                    temp_.erase(temp_.begin()+i, temp_.begin()+i+1);
                    return out;
                }
            }
        }
        return def;
    }
    /**
     * get everything that is not parsed with "-" and was not parsed yet
     */
    template<class T>
    std::vector<T> getRest(){
        std::vector<T> out;
        for(size_t i=0;i<temp_.size();i++){
            if(((TString)temp_.at(i)).BeginsWith("-")){
                std::cout << "optParser: warning option "<<temp_.at(i) << " set but not parsed, yet" << std::endl;
                if(bepicky)
                    throw std::runtime_error("optParser: begin picky: not all options parsed");
                continue;
            }
            out.push_back((T)temp_.at(i));
        }
        return out;
    }

    /**
     * call this function after getting all options
     */
    void doneParsing(){
        getRest<TString>();
    }

    /**
     * throws exception if one of the options given to the program was not parsed before getRest() is called
     */
    bool bepicky;

private:
    int argc_;
    char** argv_;
    std::vector<TString> temp_;

};

template<>
inline bool optParser::getOpt<bool>(const TString& opt,const bool & def){
    for(size_t i=0;i<temp_.size();i++){
        if((TString)temp_.at(i) == "-"+opt){
            temp_.erase(temp_.begin()+i);
            if(!def)return true;
            else return false;
        }
    }
    return def;
}
template<>
inline float optParser::getOpt<float>(const TString& opt,const float & def){
    for(size_t i=0;i<temp_.size();i++){
        if((TString)temp_.at(i) == "-"+opt){
            if (i + 1 != temp_.size()){
                float out=atof(temp_.at(i+1).Data());
                temp_.erase(temp_.begin()+i, temp_.begin()+i+1);
                return out;
            }
        }
    }
    return def;
}
template<>
inline std::vector<float> optParser::getRest<float>(){
    std::vector<float> out;
    for(size_t i=0;i<temp_.size();i++){
        if(((TString)temp_.at(i)).BeginsWith("-")){
            std::cout << "optParser: warning option "<<temp_.at(i) << " set but not parsed, yet" << std::endl;
            if(bepicky)
                throw std::runtime_error("optParser: begin picky: not all options parsed");
            continue;
        }
        out.push_back(atof(temp_.at(i).Data()));
    }
    return out;
}


}


#endif /* OPTPARSER_H_ */
