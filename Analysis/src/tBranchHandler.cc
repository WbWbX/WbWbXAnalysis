/*
 * tBranchHandler.cc
 *
 *  Created on: May 28, 2014
 *      Author: kiesej
 */


#include "../interface/tBranchHandler.h"

#include <algorithm>

namespace ztop{



std::map< TTree* ,std::vector<TString> > tBranchHandlerBase::branchesfortree_;

void tBranchHandlerBase::addTreeAndBranch(TTree * t, const TString& branchname){

	if(std::find(branchesfortree_[t].begin(),branchesfortree_[t].end(),branchname) != branchesfortree_[t].end()){
		throw std::logic_error("tBranchHandlerBase::addTreeAndBranch: Only one handler per branch allowed!");
	}
	branchesfortree_[t].push_back(branchname);

}
void tBranchHandlerBase::removeTreeAndBranch(TTree * t, const TString& branchname){

	std::vector<TString> & branches=branchesfortree_[t];
	std::vector<TString>::iterator it=std::find(branches.begin(),branches.end(),branchname);
	if(it != branches.end()){
		branchesfortree_[t].erase(it);
	}
}


//all inlined

}

