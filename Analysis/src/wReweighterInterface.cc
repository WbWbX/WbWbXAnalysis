/*
 * wReweighterInterface.cc
 *
 *  Created on: 30 May 2016
 *      Author: jkiesele
 */

#include "../interface/wReweighterInterface.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"

namespace ztop{
wReweighterInterface::wReweighterInterface(
		tTreeHandler* t,
		const TString& size_branch,
		const TString& weightsbranch):wNTBaseInterface(t),enabled_(true){

	if(size_branch.Length()){
		addBranch<int>(size_branch);
		addBranch<float*>(weightsbranch,128);
	}
	else{
		enabled_=false;
	}

	//create the default one


}
void wReweighterInterface::addWeightIndex(const size_t index){
	indicies_.push_back(index);
	reweighter_.resize(indicies_.size());
}

void wReweighterInterface::reWeight(float & old){
	if(associatedBranches().size() <1 || !enabled_)
		return ;
	const size_t brsize = *getBranchContent<int>(0);
	if(brsize> associatedBranches().at(1)->getBufMax()){
		std::string err="wReweighterInterface::reWeight: buffer too small to store all weights: ";
		err+= toString(brsize);
		err+="/";
		err+=toString(associatedBranches().at(1)->getBufMax());
		throw std::out_of_range(err);

	}
	for(size_t i=0;i<indicies_.size();i++){
		size_t idx=indicies_.at(i);
		if(idx>=brsize)
			throw std::out_of_range("wReweighterInterface::reWeight ");
		reweighter_.at(i).setNewWeight((* getBranchContent<float*>(1))[idx]);
		reweighter_.at(i).reWeight(old);
	}
}

double wReweighterInterface::getRenormalization()const{
	if(associatedBranches().size() <1 || !enabled_)
		return 1.;
	double out=1;
	for(size_t i=0;i<reweighter_.size();i++)
		out*=reweighter_.at(i).getRenormalization();
	return out;
}

}
