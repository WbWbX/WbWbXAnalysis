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

//this is really ugly.....

namespace ztop{

template <class T>
	std::vector<T*> produceCollection(std::vector<T> * t, const float & ptcut=-1,const float absetacut=-1,
			const std::vector<ztop::NTGenParticle*>* allowedmothers=0) ;




////////definitions




template <class T>
inline std::vector<T*> produceCollection(std::vector<T> * t, const float & ptcut,const float absetacut,
		const std::vector<ztop::NTGenParticle*>* allowedmothers){
	std::vector<T*> out;
	for(size_t i=0;i<t->size();i++){
		if(allowedmothers){ //only possible for NTGenParticles
			throw std::logic_error("produceCollection(): defining a mother collection only possible for gen Particles");
		}
		if(ptcut>t->at(i).pt()){
			continue;
		}
		if(absetacut>0){
			if(fabs(ptcut>t->at(i).eta())>absetacut)
				continue;
		}
		out.push_back(&(t->at(i)));
	}
	std::sort(out.begin(),out.end(), ztop::comparePt<T*> );
	return out;
}
template <>
inline std::vector<ztop::NTGenParticle*> produceCollection(std::vector<ztop::NTGenParticle> * t,
		const float & ptcut,const float absetacut,
		const std::vector<ztop::NTGenParticle*>* allowedmothers){
	std::vector<ztop::NTGenParticle*> out;
	for(size_t i=0;i<t->size();i++){
		if(allowedmothers){ //only possible for NTGenParticles
			bool hasrightmother=false;
			for(size_t j=0;j<allowedmothers->size();j++){
				const int & motherid=allowedmothers->at(j)->genId();
				if(t->at(i).genId() == motherid){
					hasrightmother=true;
					break;
				}
			}
			if(!hasrightmother) continue;
		}
		if(t->at(i).pt()<ptcut){
			continue;
		}
		if(absetacut>0){
			if(fabs(t->at(i).eta())>absetacut)
				continue;
		}
		out.push_back(&(t->at(i)));
	}
	std::sort(out.begin(),out.end(), ztop::comparePt<ztop::NTGenParticle*> );
	return out;
}




}
#endif /* DATAFORMATHELPERS_H_ */
