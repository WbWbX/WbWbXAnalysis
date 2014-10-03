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
#include <stdexcept>
#include <iostream>

namespace ztop{

/**
 * This is just a small wrapper to make TBranch reading and access less ambiguous
 * and easier
 */
template<class T>
class tBranchHandler{
public:
	tBranchHandler():content_(0),branch_(0),branchname_(""),missingbranch_(true){
		// doesn't do anything
		throw std::logic_error("tBranchHandler: default constructor should not be used");
	}
	tBranchHandler(TTree * t, const TString& branchname):content_(0),branch_(0),branchname_(branchname),missingbranch_(false){
		int ret=t->SetBranchAddress(branchname_,&content_,&branch_);

		// Error handling

		if(ret == -2 || ret == -1){
			std::cout << "tBranchHandler: Class type given for branch " << branchname_
					<< " does not match class type in tree. (root CheckBranchAddressType returned " << ret << ")" <<std::endl;
			throw std::runtime_error("tBranchHandler: Class type does not match class type in branch");
		}
		else if(ret == -4 || ret == -3){
			std::cout << "tBranchHandler: Internal error in branch " << branchname_
					<< " (root CheckBranchAddressType returned " << ret << ")" <<std::endl;
			throw std::runtime_error("tBranchHandler: Internal error in branch");
		}
		else if( ret == -5){
			if(allow_missing){
				content_      = new T();
				missingbranch_= true;
				std::cout << "tBranchHandler: branch " << branchname_ << " does not exists - created empty content" << std::endl;
			}
			else{
				std::cout << "tBranchHandler: branch " << branchname_ << " does not exists!" << std::endl;
				throw std::runtime_error("tBranchHandler: branch does not exists!");
			}
		}

	}
	~tBranchHandler(){if(content_) delete content_;content_=0; /*obsolete but in case some sharing is done at some point*/ }

	void getEntry(const Long64_t& entry){
		if(!missingbranch_)
			branch_->GetEntry(entry);
	}
	T * content(){
		if(content_) return content_;
		else
			throw std::runtime_error("tBranchHandler::content() pointer NULL!");
	}

	/**
	 * Allows missing and just returns an empty vector
	 */
	static bool allow_missing;

private:
	T* content_;
	TBranch * branch_;
	TString branchname_;
	bool missingbranch_;

};

template <typename T>
bool tBranchHandler<T>::allow_missing=false;


}

#endif /* TBRANCHHANDLER_H_ */
