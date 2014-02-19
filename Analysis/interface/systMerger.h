/*
 * systMerger.h
 *
 *  Created on: Feb 14, 2014
 *      Author: kiesej
 */

#ifndef SYSTMERGER_H_
#define SYSTMERGER_H_
#include "TString.h"
#include <string>
#include <vector>

namespace ztop{

class containerStackVector;


class systMerger{
public:
    systMerger():infileadd_(""),outfileadd_(""),indir_(""){}
    ~systMerger(){}

    void setInputStrings(const std::vector<std::string>& instrings);

    //from input
    void setInFileAdd(const TString & ofa){infileadd_=ofa;}

    void setOutFileAdd(const TString & ofa){outfileadd_=ofa;}

    void start();

    static bool debug;

private:
    //prepare strings

   // void setInFiles(std::vector<TString> ifls){instrings_=ifls;}
    void searchNominals();
    void searchSystematics();


    ztop::containerStackVector * getFromFileToMem( TString dir, const TString& name)const;

    void mergeAndSafe();

    void mergeBTags()const;

    TString infileadd_,outfileadd_;
    std::string indir_;
    std::vector<TString> instrings_;

    std::vector<size_t> nominals_;
    std::vector< std::vector<size_t> > syst_;

};

}

#endif /* SYSTMERGER_H_ */
