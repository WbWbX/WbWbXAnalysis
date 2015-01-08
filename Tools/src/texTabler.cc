/*
 * texTabler.cc
 *
 *  Created on: Jun 27, 2014
 *      Author: kiesej
 */



#include "../interface/texTabler.h"
#include "../interface/formatter.h"

#include "../interface/textFormatter.h"
#include <algorithm>
#include "TString.h"
#include <fstream>

#include <iostream>
#include <stdlib.h>


namespace ztop{


TString texLine::getTex()const{
    TString out;
    for(size_t i=0;i<multi_;i++)
        out += " \\hline";
    return out;
}


texTabler::texTabler():columns_(0),texformat_(""),linecount_(0),addnewrow_(false){

}

texTabler::texTabler(const TString& formats):columns_(0),texformat_(formats),linecount_(0),addnewrow_(false){

    format(formats);

}

void texTabler::format(const TString& str){

    TString fc=str;
    fc.ReplaceAll("r","#");
    fc.ReplaceAll("l","#");
    fc.ReplaceAll("c","#");

    std::string fs(fc.Data());
    columns_ = std::count(fs.begin(), fs.end(), '#');
}

texTabler& texTabler::operator << (const texLine&tl){

    rowboundaries_.at(rowboundaries_.size()-1)=tl;
    return *this;

}
void texTabler::newLine(){

    while(entries_.size() % columns_ !=0)
        entries_.push_back("$\\$SKIPENTRY$\\$");
    linecount_++;
    rowboundaries_.push_back(texLine(0));

}

TString texTabler::getTable()const{

    TString out;
    out += "\\begin{tabular}{"+texformat_+"}\n";
    out+=toplines_.getTex();
    for(size_t i=0;i<linecount_;i++){
        out+="\n";
        for(size_t j=0;j<columns_;j++){
            bool skip=entries_.at( i * columns_ +j) == "$\\$SKIPENTRY$\\$";
            if(!skip)
                out+=entries_.at( i * columns_ +j);
            if(j<columns_-1 && entries_.at( i * columns_ +j +1) != "$\\$SKIPENTRY$\\$")
                out+=" & ";
            else
                out+=" ";
        }

        out+="\\\\";
        out+=rowboundaries_.at(i).getTex();
    }
    out+=bottomlines_.getTex();
    out+="\n\\end{tabular}";

    return out;

}

void texTabler::writeToFile(const TString& filename)const{
    TString string=getTable();
    ofstream outs;
    outs.open(filename.Data());
    outs << string;
    outs.close();
}

void texTabler::writeToPdfFile(const TString& filename)const{
	char tmpfilename[]={'X','X','X','X','X','X','\0'};
	mkstemp(tmpfilename);
	TString namestr(tmpfilename);
	writeToFile(namestr);
	char * cmsswbase=getenv("CMSSW_BASE");
	TString syscall(cmsswbase);
	syscall+="/src/TtZAnalysis/Tools/bin/texToPdf "+namestr+" > /dev/null";
	system(syscall.Data());
	syscall="mv "+namestr+".pdf "+filename;
	system(syscall.Data());
	syscall="rm "+namestr;
	system(syscall.Data());
}

void texTabler::clear(){
    entries_.clear();
    rowboundaries_.clear();
    linecount_=0;

}
}
