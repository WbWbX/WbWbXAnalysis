/*
 * corrMatrix.cc
 *
 *  Created on: Apr 13, 2015
 *      Author: kiesej
 */


#include "../interface/corrMatrix.h"

namespace ztop{
const double corrMatrix::one_=1.;
corrMatrix::corrMatrix(const std::vector<TString>& names){
	for(size_t i=0;i<names.size();i++){
		names_.push_back(names.at(i));
	}
	const size_t & N=names_.size();
	size_t size=(N*N-N);
	size/=2;
	entries_.resize(size,0);
}

const TString& corrMatrix::getEntryName(const size_t &idx)const{
	return names_.getData(idx);
}
const size_t& corrMatrix::getEntryIndex(const TString& name)const{
	return names_.getIndex(name);
}

size_t corrMatrix::linearize(const int & col,const int & row)const{
	const size_t & N=names_.size();
	if (row<col)
		return row*(N-1) - (row-1)*((row-1) + 1)/2 + col - row - 1;
	else if (col<row)
		return col*(N-1) - (col-1)*((col-1) + 1)/2 + row - col - 1;
	else
		return std::string::npos;
}

const double& corrMatrix::getEntry(const size_t & row, const size_t & col)const{
	if(col > entries_.size() || row> entries_.size()){
		throw std::out_of_range("corrMatrix::getEntry: index out of range");
	}
	if(row==col)return one_;
	return entries_.at(linearize(row,col));
}


void  corrMatrix::setEntry(const size_t & row,const size_t & col,const double & entr){
	if(col > entries_.size() || row> entries_.size()){
		throw std::out_of_range("corrMatrix::setEntry: index out of range");
	}
	if(row==col)return;
	entries_.at(linearize(row,col))=entr;
}

}
