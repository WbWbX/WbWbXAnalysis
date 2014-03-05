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
 * The options -h and --help are reserved and will display a help message!
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
        if(argv_[0])
            progname_=argv_[0];
        for(int i=1;i<argc_;i++){
            temp_.push_back((TString)argv_[i]);
        }
    }


    template <class T>
    T getOpt(const TString& opt,const T & def, const TString & description=""){
        for(size_t i=0;i<temp_.size();i++){
            if(temp_.at(i) == "-"+opt){
                if (i + 1 != temp_.size()){
                    T out=temp_.at(i+1);
                    if(debug) std::cout << "optparser::getOpt: got "<< temp_.at(i) << " " << temp_.at(i+1) <<std::endl;
                    temp_.erase(temp_.begin()+i, temp_.begin()+i+2);
                    help_.push_back("-"+opt+"  \t"+description);
                    return out;
                }
            }
        }
        help_.push_back("-"+opt+"  \t"+description);
        return def;
    }
    /**
     * get everything that is not parsed with "-" and was not parsed yet
     * ignores -h and --help!
     */
    template<class T>
    std::vector<T> getRest(){
        std::vector<T> out;
        for(size_t i=0;i<temp_.size();i++){
            if(((TString)temp_.at(i)).BeginsWith("-")){
                if( (TString)temp_.at(i) != "-h" && (TString)temp_.at(i) != "--help"){
                    std::cout << "optParser: warning option "<<temp_.at(i) << " set but not parsed, yet" << std::endl;
                    if(bepicky)
                        throw std::runtime_error("optParser: begin picky: not all options parsed");
                }
                continue;
            }
            if(debug) std::cout << "optparser::getRest: got "<< temp_.at(i) <<std::endl;
            out.push_back((T)temp_.at(i));
        }
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
    std::vector<TString> temp_;
    std::vector<TString> help_;
    TString progname_;

};

template<>
inline bool optParser::getOpt<bool>(const TString& opt,const bool & def, const TString & description){
    for(size_t i=0;i<temp_.size();i++){
        if((TString)temp_.at(i) == "-"+opt){
            temp_.erase(temp_.begin()+i);
            help_.push_back("-"+opt+"  \t"+description);
            if(!def)return true;
            else return false;
        }
    }
    help_.push_back("-"+opt+"  \t"+description);
    return def;
}
template<>
inline float optParser::getOpt<float>(const TString& opt,const float & def, const TString & description){
    for(size_t i=0;i<temp_.size();i++){
        if((TString)temp_.at(i) == "-"+opt){
            if (i + 1 != temp_.size()){
                float out=atof(temp_.at(i+1).Data());
                temp_.erase(temp_.begin()+i, temp_.begin()+i+1);
                help_.push_back("-"+opt+"  \t"+description);
                return out;
            }
        }
    }
    help_.push_back("-"+opt+"  \t"+description);
    return def;
}
template<>
inline int optParser::getOpt<int>(const TString& opt,const int & def, const TString & description){
    for(size_t i=0;i<temp_.size();i++){
        if((TString)temp_.at(i) == "-"+opt){
            if (i + 1 != temp_.size()){
                float out=atoi(temp_.at(i+1).Data());
                temp_.erase(temp_.begin()+i, temp_.begin()+i+1);
                help_.push_back("-"+opt+"  \t"+description);
                return out;
            }
        }
    }
    help_.push_back("-"+opt+"  \t"+description);
    return def;
}
template<>
inline std::vector<float> optParser::getRest<float>(){
    std::vector<float> out;
    for(size_t i=0;i<temp_.size();i++){
        if(((TString)temp_.at(i)).BeginsWith("-")){
            if( (TString)temp_.at(i) != "-h" && (TString)temp_.at(i) != "--help"){
                std::cout << "optParser: warning option "<<temp_.at(i) << " set but not parsed, yet" << std::endl;
                if(bepicky)
                    throw std::runtime_error("optParser: begin picky: not all options parsed");
            }
            continue;
        }
        out.push_back(atof(temp_.at(i).Data()));
    }
    return out;
}


}


#endif /* OPTPARSER_H_ */
