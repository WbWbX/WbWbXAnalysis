/*
 * helpers.cc
 *
 *  Created on: Jan 14, 2015
 *      Author: kiesej
 */




#include "../interface/helpers.h"

namespace ztop{

std::vector<ztop::NTGenParticle*> produceCollection(std::vector<ztop::NTGenParticle> * t,
		std::vector<ztop::NTGenParticle*>* mothercollection){
	std::vector<ztop::NTGenParticle*> out;
	if(!t)
		return out;
	for(size_t i=0;i<t->size();i++){
		if(mothercollection){ //only possible for NTGenParticles
			for(size_t j=0;j<mothercollection->size();j++){
				const int & motherid=mothercollection->at(j)->genId();
				if(std::find(t->at(i).motherIts().begin(), t->at(i).motherIts().end(), motherid) != t->at(i).motherIts().end()){
					//connect both on pointer level
					//mothercollection->at(j)->addDaughter(&t->at(i));
					t->at(i).relateWithMother(mothercollection->at(j));
				}
			}
		}

		out.push_back(&(t->at(i)));
	}
	std::sort(out.begin(),out.end(), ztop::comparePt<ztop::NTGenParticle*> );
	return out;
}
std::vector<ztop::NTGenParticle*> produceCollection(std::vector<ztop::NTGenParticle*> * t,
		std::vector<ztop::NTGenParticle*>* mothercollection){
	std::vector<ztop::NTGenParticle*> out;
	if(!t)
		return out;
	for(size_t i=0;i<t->size();i++){
		if(!t->at(i)) continue;
		if(mothercollection){ //only possible for NTGenParticles
			for(size_t j=0;j<mothercollection->size();j++){
				const int & motherid=mothercollection->at(j)->genId();
				if(std::find(t->at(i)->motherIts().begin(), t->at(i)->motherIts().end(), motherid) != t->at(i)->motherIts().end()){
					//connect one
					t->at(i)->relateWithMother(mothercollection->at(j));
				}
			}
		}

		out.push_back((t->at(i)));
	}
	std::sort(out.begin(),out.end(), ztop::comparePt<ztop::NTGenParticle*> );
	return out;
}


}//ns
