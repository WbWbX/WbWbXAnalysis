/*
 * optParser.cc
 *
 *  Created on: Feb 27, 2014
 *      Author: kiesej
 */




#include "../interface/optParser.h"
#include <iostream>

namespace ztop{

bool optParser::debug=false;

void optParser::doneParsing(){
    getRest<TString>();
    if(getOpt<bool>("h",false,"display this help message") || getOpt<bool>("-help",false,"display this help message"))
        coutHelp();
}

void optParser::coutHelp()const{
    using namespace std;

    std::cout << progname_ << "\n\n" << adddescr_ << "\n usage: "<<std::endl;
    for(size_t i=0;i<help_.size();i++){
        if(help_.at(i).Length()>0){
            std::cout << "   " << help_.at(i) << std::endl;
        }
    }
    std::cout << std::endl;
    exit(EXIT_SUCCESS);

}

}
