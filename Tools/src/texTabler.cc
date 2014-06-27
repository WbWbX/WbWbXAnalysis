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
#include <stdexcept>

namespace ztop{


TString texLine::getTex()const{
    TString out;
    for(size_t i=0;i<multi_;i++)
        out += " \\hline";
    return out;
}


texTabler::texTabler(){
    throw std::runtime_error("texTabler::texTabler: don't use default constr");
}

texTabler::texTabler(const TString& format):columns_(0),texformat_(format),linecount_(0){


    TString fc=format;
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


TString texTabler::getTable()const{

    TString out;
    out += "\\begin{tabular}{"+texformat_+"}\n";
    out+=toplines_.getTex();
    for(size_t i=0;i<linecount_;i++){
        out+="\n";
        for(size_t j=0;j<columns_;j++){
            out+=entries_.at( i * columns_ +j);
            if(j<columns_-1)
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

void texTabler::clear(){
    entries_.clear();
    rowboundaries_.clear();
    linecount_=0;

}
}
