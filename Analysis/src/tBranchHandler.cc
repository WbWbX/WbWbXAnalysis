/*
 * tBranchHandler.cc
 *
 *  Created on: May 28, 2014
 *      Author: kiesej
 */


#include "../interface/tBranchHandler.h"

#include <algorithm>

namespace ztop{



std::map< tTreeHandler* ,std::vector<TString> > tBranchHandlerBase::branchesfortree_;

void tBranchHandlerBase::addTreeAndBranch(tTreeHandler * t, const TString& branchname){

	if(std::find(branchesfortree_[t].begin(),branchesfortree_[t].end(),branchname) != branchesfortree_[t].end()){
		throw std::logic_error("tBranchHandlerBase::addTreeAndBranch: Only one handler per branch allowed!");
	}
	branchesfortree_[t].push_back(branchname);
	t->associate(this);
}
void tBranchHandlerBase::removeTreeAndBranch( tTreeHandler * t, const TString& branchname){
	std::vector<TString> & branches=branchesfortree_[t];
	std::vector<TString>::iterator it=std::find(branches.begin(),branches.end(),branchname);
	if(it != branches.end()){
		branchesfortree_[t].erase(it);
	}
	t->removeAsso(this);
}


//all inlined

}

