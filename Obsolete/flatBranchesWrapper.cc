/*
 * flatBranchesWrapper.cc
 *
 *  Created on: 23 May 2016
 *      Author: kiesej
 */


#include "../interface/flatBranchesWrapper.h"

namespace ztop{

bool flatBranchesWrapperBase::debug=false;

flatBranchesWrapperBase::flatBranchesWrapperBase(tTreeHandler * t):read_(false){

}


flatBranchesWrapperBase::~flatBranchesWrapperBase(){
	for(size_t i=0;i<associatedBranches_.size();i++)
		if(associatedBranches_.at(i)){
			if(debug)
				std::cout << "flatBranchesWrapperBase::~flatBranchesWrapperBase: deleting branch" <<std::endl;
			delete associatedBranches_.at(i);
			if(debug)
				std::cout << "flatBranchesWrapperBase::~flatBranchesWrapperBase: branch deleted" <<std::endl;
			associatedBranches_.at(i)=0;
		}
}

bool flatBranchesWrapperBase::isNewEntry()const{
	if(associatedBranches_.size()<1)
		throw std::logic_error("flatBranchesWrapper: can't read entry with no branches associated");
	for(size_t i=0;i<associatedBranches_.size();i++)
		if(associatedBranches_.at(i)){
			if(associatedBranches_.at(i)->gotEntry())
				return false;
			else
				return true;
		}
	return true;
}

}
