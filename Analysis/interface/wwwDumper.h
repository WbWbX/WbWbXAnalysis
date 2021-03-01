/*
 * wwwDumper.h
 *
 *  Created on: Feb 21, 2014
 *      Author: kiesej
 */

#ifndef WWWDUMPER_H_
#define WWWDUMPER_H_
#include "WbWbXAnalysis/Tools/interface/histoStackVector.h"
#include "TString.h"

namespace ztop{

class wwwDumper{
public:
    wwwDumper(){}
    ~wwwDumper(){}

    void makePlotHTML(TString title, TString descr, TString where , ztop::histoStackVector& csv);
    int appendToIndex(TString title, TString description,TString plotdir, TString indexfile);
    int dumpToWWW(TString infile, TString title,TString description);

private:


};



}


#endif /* WWWDUMPER_H_ */
