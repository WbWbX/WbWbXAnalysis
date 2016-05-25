/*
 * flatBranchesWrapper.h
 *
 *  Created on: 23 May 2016
 *      Author: kiesej
 */

#ifndef TTZANALYSIS_ANALYSIS_INTERFACE_FLATBRANCHESWRAPPER_H_
#define TTZANALYSIS_ANALYSIS_INTERFACE_FLATBRANCHESWRAPPER_H_

#include "tTreeHandler.h"
#include "tBranchHandler.h"

namespace ztop{
class flatBranchesWrapperBase {
public:
	flatBranchesWrapperBase(tTreeHandler * t);
	~flatBranchesWrapperBase();


	static bool debug;
protected:
	template<class T>
	tBranchHandler<T>* addBranch(tTreeHandler*, const TString& branchname);

	template<class T>
	T * getBranchContent(const size_t& idx){
		if(associatedBranches_.at(idx))
		return
			(static_cast< tBranchHandler<T> * > (associatedBranches_.at(idx)))->content();
		else
			throw std::logic_error("flatBranchesWrapperBase:getBranchContent: branch not associated");;}

	bool isNewEntry()const;

	const std::vector<tBranchHandlerBase*>& associatedBranches()const{return associatedBranches_;}
	flatBranchesWrapperBase():read_(false){}
private:

	bool read_;

	std::vector<tBranchHandlerBase*> associatedBranches_;


};


template<class T>
tBranchHandler<T>* flatBranchesWrapperBase::addBranch(tTreeHandler*t, const TString& branchname){
	tBranchHandler<T>* p;
	if(branchname.Length()){
		p=new tBranchHandler<T>(t,branchname);
	}
	else
		p=0;
	associatedBranches_.push_back(p);
	return p;
}



}



#endif /* TTZANALYSIS_ANALYSIS_INTERFACE_FLATBRANCHESWRAPPER_H_ */
