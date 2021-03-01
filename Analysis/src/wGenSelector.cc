/*
 * wGenSelector.cc
 *
 *  Created on: 15 Jun 2016
 *      Author: jkiesele
 */

#include "../interface/wGenSelector.h"
#include "WbWbXAnalysis/DataFormats/interface/NTGenParticle.h"

namespace ztop{
bool wGenSelector::debug=false;

void wGenSelector::addVeto(int pdgid){
	if(debug)
		std::cout << "wGenSelector::addVeto: " << pdgid << std::endl;
	vetos_.push_back(pdgid);
}


bool wGenSelector::pass()const{
	if(vetos_.size()<1) return true;
	if(!coll_) throw std::logic_error("wGenSelector::pass: no collection assigned");

	for(size_t i=0;i<coll_->size();i++){
		const NTGenParticle * p=coll_->at(i);
		for(size_t j=0;j<vetos_.size();j++){
			if(p->pdgId() == vetos_.at(j)){
				if(debug)
					std::cout << "wGenSelector::pass: rejected " << p->pdgId() << std::endl;
				return false;
			}
		}
	}
	return true;
}
}
