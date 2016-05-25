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

bool tBranchHandlerBase::debug=false;


void tBranchHandlerBase::addTreeAndBranch(tTreeHandler * t, const TString& branchname){

	if(std::find(branchesfortree_[t].begin(),branchesfortree_[t].end(),branchname) != branchesfortree_[t].end()){
		std::string err="tBranchHandlerBase::addTreeAndBranch: Only one handler per branch allowed! (";
		err+=branchname.Data();
		err+=")";
		throw std::logic_error(err);
	}
	branchesfortree_[t].push_back(branchname);
	if(t_)
		t->associate(this);
}
void tBranchHandlerBase::removeTreeAndBranch( tTreeHandler * t, const TString& branchname){

	std::vector<TString> & branches=branchesfortree_[t];
	std::vector<TString>::iterator it=std::find(branches.begin(),branches.end(),branchname);
	if(it != branches.end()){
		branchesfortree_[t].erase(it);
	}
	if(t_)
		t->removeAsso(this);
	removeTree(t);
}


//all inlined

}

