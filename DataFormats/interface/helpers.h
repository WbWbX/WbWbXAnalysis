/*
 * dataFormatHelpers.h
 *
 *  Created on: Oct 9, 2014
 *      Author: kiesej
 */

#ifndef DATAFORMATHELPERS_H_
#define DATAFORMATHELPERS_H_

#include "NTGenParticle.h"
#include <vector>
#include <stdexcept>
#include <algorithm>
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"

//this is really ugly.....

namespace ztop{

/**
 * produces collections and dependencies between gen particles
 */
std::vector<ztop::NTGenParticle*> produceCollection(std::vector<ztop::NTGenParticle> * t,
		 std::vector<ztop::NTGenParticle*>* mothercollection) ;
/**
 * produces collections and dependencies between gen particles
 */
std::vector<ztop::NTGenParticle*> produceCollection(std::vector<ztop::NTGenParticle*> * t,
		 std::vector<ztop::NTGenParticle*>* mothercollection) ;


template <class T>
std::vector< T*> produceCollection( std::vector<T> * t, float minpt=-1, float maxabseta=-1);
template <class T>
std::vector< T*> produceCollection( std::vector<T*> * t, float minpt=-1, float maxabseta=-1);



template <class T>
bool comparePt(T a, T b);

template <class T>
bool removeEntry( T*, std::vector< T*>& );


////////definitions

/**
 * requires the collection to contain between (min and max) elements
 * min and max itself are also allowed
 */
template <class T>
bool requireNumber(size_t min,size_t max, const std::vector<T> & collection);

template <class T>
bool requireNumber(size_t min, const std::vector<T> & collection);



template <class T>
inline std::vector< T*> produceCollection( std::vector<T> * t, float minpt, float maxabseta){
	std::vector<T*> out;
	if(!t) return out;
	for(size_t i=0;i<t->size();i++){
		if(maxabseta>0 && fabs(t->at(i).eta())>maxabseta) continue;
		if(t->at(i).pt()<minpt) continue;
		out.push_back(&t->at(i));
	}
	return out;
}

template <class T>
inline std::vector< T*> produceCollection( std::vector<T*> * t, float minpt, float maxabseta){
	std::vector<T*> out;
	if(!t) return out;
	for(size_t i=0;i<t->size();i++){
		if(! t->at(i)) continue;
		if(maxabseta>0 && fabs(t->at(i)->eta())>maxabseta) continue;
		if(t->at(i)->pt()<minpt) continue;
		out.push_back(t->at(i));
	}
	return out;
}


template <class T>
inline bool requireNumber(size_t min,size_t max, const std::vector<T> & collection){
	if(collection.size()<min) return false;
	if(collection.size()>max) return false;
	return true;
}
template <class T>
inline bool requireNumber(size_t min, const std::vector<T> & collection){
	if(collection.size()<min) return false;
	return true;
}

template <class T>
inline bool comparePt(T a, T b){
    return (a->pt() > b->pt());
}

template <class T>
inline bool removeEntry( T* e, std::vector< T*>& v){
	typename std::vector< T*>::iterator p=std::find(v.begin(),v.end(),e);
	if(p!=v.end()){
		v.erase(p);
		return true;
	}
	return false;
}


}
#endif /* DATAFORMATHELPERS_H_ */
