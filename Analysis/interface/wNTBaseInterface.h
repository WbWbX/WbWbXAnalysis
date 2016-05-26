/*
 * wNTBaseInterface.h
 *
 *  Created on: 25 May 2016
 *      Author: kiesej
 */

#ifndef TTZANALYSIS_ANALYSIS_INTERFACE_WNTBASEINTERFACE_H_
#define TTZANALYSIS_ANALYSIS_INTERFACE_WNTBASEINTERFACE_H_

#include "tBranchHandler.h"
#include "TString.h"

namespace ztop{

class tTreeHandler;

class wNTBaseInterface {
public:
	wNTBaseInterface();
	wNTBaseInterface(tTreeHandler * t);
	~wNTBaseInterface();

	void setTree(tTreeHandler * t){t_=t;}

	static bool debug;
protected:
	template<class T>
	tBranchHandler<T>* addBranch(const TString& branchname, size_t buffer=1);

	template<class T>
	T * getBranchContent(const size_t& idx){
		if(associatedBranches_.size()>idx){
			if(associatedBranches_.at(idx)){
				if(firstread_){// cast was already checked at least once. can do fast and dirty C-style now
					return (( tBranchHandler<T> * ) (associatedBranches_.at(idx)))->content();}
				else{
					tBranchHandler<T> * p=dynamic_cast<tBranchHandler<T> *>(associatedBranches_.at(idx));
					if(!p)
						throw std::logic_error
						("wNTBaseInterface:getBranchContent: wrong cast. Check branch initialization\
								 and call of getBranchContent");
					firstread_=true;
					return p->content();
				}
			}
		}
		throw std::logic_error("wNTBaseInterface:getBranchContent: branch not associated");;}

	bool isNewEntry()const;

	const std::vector<tBranchHandlerBase*>& associatedBranches()const{return associatedBranches_;}

private:

	bool read_;
	bool firstread_;

	std::vector<tBranchHandlerBase*> associatedBranches_;

	tTreeHandler * t_;

};


template<class T>
tBranchHandler<T>* wNTBaseInterface::addBranch(const TString& branchname, size_t buffer){
	if(!t_)
		throw std::logic_error("wNTBaseInterface::addBranch: first associate tree");
	tBranchHandler<T>* p;
	if(branchname.Length()){
		p=new tBranchHandler<T>(t_,branchname,buffer);
	}
	else
		p=0;
	associatedBranches_.push_back(p);
	return p;
}


}







#endif /* TTZANALYSIS_ANALYSIS_INTERFACE_WNTBASEINTERFACE_H_ */
