/*
 * corrMatrix.h
 *
 *  Created on: Apr 13, 2015
 *      Author: kiesej
 */

#ifndef CORRMATRIX_H_
#define CORRMATRIX_H_

#include <vector>
#include "TString.h"
#include "indexMap.h"

namespace ztop{

class corrMatrix{
public:
	corrMatrix(){}
	corrMatrix(const std::vector<TString>&);

	~corrMatrix(){}

	const double& getEntry(const size_t & , const size_t &)const;
	void setEntry(const size_t &,const size_t &, const double &);

	const TString& getEntryName(const size_t &)const;
	const size_t& getEntryIndex(const TString&)const;

	const size_t& size()const{return names_.size();}

private:
	indexMap<TString> names_;
	std::vector<double> entries_;
	static const double one_;
	size_t linearize(const int &,const int &)const;

};

}


#endif /* CORRMATRIX_H_ */
