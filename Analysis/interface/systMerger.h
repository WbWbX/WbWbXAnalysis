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

class histoStackVector;


class systMerger{
public:
    systMerger():infileadd_(""),outfileadd_(""),indir_(""),nominalid_("_nominal")
,standardnominal_("_nominal"),drawcanv_(true),ignorestring_("_sysnominal"),mergerel_(false){}
    ~systMerger(){}

    void setInputStrings(const std::vector<std::string>& instrings);

    //from input
    void setInFileAdd(const TString & ofa){infileadd_=ofa;}

    void setOutFileAdd(const TString & ofa){outfileadd_=ofa;}

    void setNominalID(const TString & nomid){nominalid_=nomid;}

    void setDrawCanvases(bool set){drawcanv_=set;}

    void setIgnoreName(const TString &str){ignorestring_=str;}

    void setMergeRelative(bool set){mergerel_=set;}

    std::vector<TString>  start();
    /**
     * returns list of indicies of files that have been merged to output files
     */
    std::vector<size_t> getMergedFiles()const;

   // std::vector<TString> getNominalStartstrings()const;


    static bool debug;

private:
    //prepare strings

   // void setInFiles(std::vector<TString> ifls){instrings_=ifls;}
    void searchNominals();
    void searchSystematics();


    ztop::histoStackVector * getFromFileToMem( TString dir, const TString& name)const;

    std::vector<TString> mergeAndSafe();

    void mergeBTags()const;

    TString infileadd_,outfileadd_;
    std::string indir_;
    std::vector<TString> instrings_;

    std::vector<size_t> nominals_;
    std::vector< std::vector<size_t> > syst_;

    TString nominalid_;
    TString standardnominal_;

    bool drawcanv_;

    std::vector<std::string> addrelative_;
    std::string ignorestring_;

    bool mergerel_;

    std::string removedext_;

};

}

#endif /* SYSTMERGER_H_ */
