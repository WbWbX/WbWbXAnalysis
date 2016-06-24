/*
 * binFinder.h
 *
 *  Created on: 23 Jun 2016
 *      Author: jkiesele
 */

#ifndef TTZANALYSIS_TOOLS_INTERFACE_BINFINDER_H_
#define TTZANALYSIS_TOOLS_INTERFACE_BINFINDER_H_


#include <vector>
#include <algorithm>

namespace ztop{


/**
 * Class to find a bin in a vector
 * Using this class is much faster than a simple loop to find the bin!
 *
 * The convention is as follows (follows root histogram convention):
 *
 * The binning is provided as a vector describing the bin boundaries.
 * In addition, there is an entry for an underflow bin:
 *
 * <anynumber=underflow>, <bin left bound>, <next bin left bound>, ..., <last bin right bound>
 *
 * Example of what findBin then returns:
 *
 * bins={<any value, eg. 99>, 0, 1, 2}
 *
 * findBin( -1)-> 0
 * findBin(0.5)-> 1
 * findBin(1.5)-> 2
 * findBin(2.5)-> 3
 *
 * The boundaries are inclusive to larger values:
 * findBin(1)-> 2
 * findBin(1+epsilon)-> 2
 *
 * if setMergeUFOF is set, findBin will never return 0, nor the largest index. In this example:
 *
 * findBin( -1)-> 1
 * findBin(0.5)-> 1
 * findBin(1.5)-> 2
 * findBin(2.5)-> 2
 *
 */
template<class T>
class binFinder{
public:
	binFinder():mergeufof_(false),bins_(0){}
	binFinder(const std::vector<T>& v):mergeufof_(false),bins_(&v){}
	~binFinder(){}

	void setBinning(const std::vector<T>& v){bins_=&v;}

	void setMergeUFOF(bool set){mergeufof_=set;}

	size_t findBin(const T& val)const;

private:
	bool mergeufof_;
	const std::vector<T>* bins_;
};


template<class T>
size_t binFinder<T>::findBin(const T& var)const{
	if(bins_->size()<1){
		return 0;
	}
	if(bins_->size() <2){
		return 0;
	}
	size_t binno=0;
	typename std::vector<T>::const_iterator it=std::lower_bound(bins_->begin()+1, bins_->end(), var);
	if(var==*it)
		binno= it-bins_->begin();
	else
		binno= it-bins_->begin()-1;
	if(mergeufof_){
		if(binno==0){
			binno++;
		}
		else if(binno == bins_->size()-1){
			binno--;
		}
	}
	return binno;
}

}


#endif /* TTZANALYSIS_TOOLS_INTERFACE_BINFINDER_H_ */
