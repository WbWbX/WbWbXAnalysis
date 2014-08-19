/*
 * tBranchHandler.h
 *
 *  Created on: May 28, 2014
 *      Author: kiesej
 */

#ifndef TBRANCHHANDLER_H_
#define TBRANCHHANDLER_H_

#include "TString.h"
#include "TBranch.h"
#include "TTree.h"

namespace ztop{

/**
 * This is just a small wrapper to make TBranch reading and access less ambiguous
 * and easier
 */
template<class T>
class tBranchHandler{
public:
    tBranchHandler():content_(0),branch_(0),branchname_(""){
        // doesn't do anything
    }
    tBranchHandler(TTree * t, const TString& branchname):content_(0),branch_(0),branchname_(branchname){
        t->SetBranchAddress(branchname_,&content_,&branch_);
    }
    ~tBranchHandler(){if(content_) delete content_;}

    void getEntry(const Long64_t& entry){
        branch_->GetEntry(entry);
    }
    T * content(){return content_;}


private:
    T* content_;
    TBranch * branch_;
    TString branchname_;

};


}

#endif /* TBRANCHHANDLER_H_ */
