/*
 * indexMap.h
 *
 *  Created on: Aug 28, 2013
 *      Author: kiesej
 */

#ifndef INDEXMAP_H_
#define INDEXMAP_H_

#include <map>
#include <stdexcept>
#include <iostream>

namespace ztop{
template<class T>
class indexMap {
public:
	indexMap():size_(0){}
	~indexMap(){}

	const size_t &size() const{return size_;}
	/**
	 * returns index associated to data
	 * if element does not exist it returns indexMap::size()
	 */
	const size_t & getIndex(const T& t) const{
		typename std::map<T,size_t>::const_iterator mit=imap_.find(t);
		if(mit!=imap_.end())
			return mit->second;
		return size_;
	}
	/**
	 * returns data associated to index
	 * if index is out of range, exception is thrown
	 */
	const T& getData(const size_t &idx) const{
		typename std::map<size_t,T>::const_iterator mit=map_.find(idx);
		if(mit!=map_.end())
			return mit->second;
		std::cout << "indexMap::getData: out of range" << std::endl;
		std::out_of_range("indexMap::getData: out of range");
		return map_.begin()->second; //just to avoid warnings
	}

	indexMap & operator << (const T& t){
		 push_back(t);
		 return *this;
	}
	indexMap & operator << (const indexMap& m){
		for(size_t i=0;i<m.size_;i++){
			size_t idx=push_back(m.getData(i));
		}
		return *this;
	}

	/**
	 * costs performance
	 */
	void eraseByIdx(const size_t &idx){
		if(idx>=size_){
			std::cout << "indexMap::eraseByIdx: index out of range not doing anything" <<std::endl;
			return;
		}
		indexMap<T> newmap;
		for(size_t i=0;i<size();i++){
			if(i==idx) continue;
			newmap.push_back(getData(i));
		}
		*this=newmap;
	}
	/**
	 * costs performance
	 */
	void eraseByData(const T &t){
		typename std::map<T,size_t>::iterator imit=imap_.find(t);
		if(imit==imap_.end()){
			std::cout << "indexMap::eraseByData: data not found" << std::endl;
			return;
		}
		size_t eraseindex=imit->second;
		eraseByIdx(eraseindex);
	}
	/**
	 * not protected! idx not allowed to exceed size
	 * does not work due to iterator->first = permissions
	 */
	/*
	void setData(const size_t & idx, const T& t ){
	////
	}
	 */
	/**
	 * returns index of newly added data
	 * if data already exists, it returns its iterator position
	 */
	size_t push_back(const T& t){
		typename std::map<T,size_t>::iterator imit=imap_.find(t);
		if(imit!=imap_.end()){
			return imit->second;
		}
		map_[size_]=t;
		imap_[t]=size_;
		size_++;
		return size_-1;
	}

	void clear(){
		map_.clear();
		imap_.clear();
		size_=0;
	}

private:
	std::map<size_t,T> map_;
	std::map<T,size_t> imap_;
	size_t size_;
};

}//namespace

#endif /* INDEXMAP_H_ */
