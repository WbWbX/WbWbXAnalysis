/*
 * topDecaySelector.cc
 *
 *  Created on: Apr 7, 2015
 *      Author: kiesej
 */




#include "../interface/topDecaySelector.h"
#include <iostream>
#include <stdexcept>

namespace ztop{

bool topDecaySelector::debug=false;

//C++ initialize all members here that need an initial value. keep ordering
topDecaySelector::topDecaySelector(): partonshower_(ps_pythia6),incollectionp_(0),statTop_(3)
{
	// I usually put this kind of debug ouput on top of each function call, it makes debugging
	// much easier. Also in case it is done long after the class was developed
	if(debug)
		std::cout << "topDecaySelector::topDecaySelector(): constructor" << std::endl;

}

topDecaySelector::~topDecaySelector(){
	//nothing to be done here
}


//C++: initializer should be defined
topDecaySelector::topDecaySelector(const topDecaySelector&rhs){
	copyFrom(rhs);
}

//C++: equal operator should be defined. return *this (to allow a=b=c;)
topDecaySelector& topDecaySelector::operator = (const topDecaySelector&rhs){
	if(this==&rhs) return *this;
	copyFrom(rhs);
	return *this;
}

//this is where the magic happens
void  topDecaySelector::process(){
        
        //Vectors to save the tops, ws
        std::vector<const reco::GenParticle *> tops, ws;
        const reco::GenParticle * mother, * daughter, * secondDaughter;

	if(!incollectionp_){// pointer 0 -> real check, no example! ;)
		throw std::logic_error("topDecaySelector::process: input collection not defined");
	}
        
        if(debug) std::cout << "Filling tops" << std::endl;
        for(size_t i=0;i<incollectionp_->size();i++){
                mother = incollectionp_->at(i);
                if(std::abs(mother->pdgId()) ==6 && std::abs(mother->status())==statTop_ ){ 
                        metops_.push_back(mother);
                        daughter = findLastParticle(mother);
                        tops.push_back(daughter);
                        dectops_.push_back(daughter);
                        dectops_mothers_.push_back(mother);
                        metops_daughters_.push_back(daughter);
                }
        }
        if(tops.size()< 2){
                throw std::logic_error("topDecaySelector::process: less than two tops found");
        }
        if(debug) std::cout<<"Filling ws"<< std::endl;
        
        for(size_t i=0;i<tops.size();i++){
                mother=tops.at(i);
                for(size_t j=0;j<mother->numberOfDaughters();j++){
                        daughter = dynamic_cast<const reco::GenParticle*>(mother->daughter(j));
                        if(std::abs(daughter->pdgId())== 24){
                                mews_.push_back(daughter);
                                mews_mothers_.push_back(mother);
                                dectops_daughters_.push_back(daughter);
                                decws_mothers_.push_back(daughter);
                                secondDaughter = findLastParticle(daughter);
                                ws.push_back(secondDaughter);
                                decws_.push_back(secondDaughter);
                                mews_daughters_.push_back(secondDaughter);
                         }
                }
        }
        if(debug) std::cout<<"Filling Leps"<<std::endl;
        if(ws.size()< 2){
                throw std::logic_error("topDecaySelector::process: less than two ws found");
        }
        for(size_t i=0;i<ws.size();i++){
                mother=ws.at(i);
                for(size_t j=0;j<mother->numberOfDaughters();j++){
                        daughter = dynamic_cast<const reco::GenParticle*>(mother->daughter(j));
                        if (std::abs(daughter->pdgId())== 11 || std::abs(daughter->pdgId())== 13 || std::abs(daughter->pdgId())==15){
                               meleptons_.push_back(daughter);
                               decws_daughters_.push_back(daughter);
                               meleptons_mothers_.push_back(mother);
                               secondDaughter=findLastParticle(daughter);
                               if (secondDaughter->status()==1){
                                       finalstateleptonsfromw_.push_back(secondDaughter); 
                                       finalstateleptonsfromw_mothers_.push_back(daughter);
                                       meleptons_daughters_.push_back(secondDaughter);
                               }
                        }
                        else if (std::abs(daughter->pdgId())== 12 || std::abs(daughter->pdgId())== 14 || std::abs(daughter->pdgId())==16){
                               meneutrinos_.push_back(daughter);
                               decws_daughters_.push_back(daughter);
                               meneutrinos_mothers_.push_back(mother);
                        }

                }

        }
        if(debug) std::cout<<"Filling Bs"<<std::endl;
        for(size_t i=0;i<tops.size();i++){
                mother=tops.at(i);
                for(size_t j=0;j<mother->numberOfDaughters();j++){
                        daughter = dynamic_cast<const reco::GenParticle*>(mother->daughter(j));
                        if (std::abs(daughter->pdgId())== 5){
                               mebs_.push_back(daughter);
                               dectops_daughters_.push_back(daughter);
                               mebs_mothers_.push_back(mother);
                        }

                }

        }
        if(debug) std::cout<<"Filling Final State Leptons"<<std::endl;
        for(size_t i=0;i<incollectionp_->size();i++){
                mother= incollectionp_->at(i);
                if(mother->status()==1 && (std::abs(mother->pdgId())==11 || std::abs(mother->pdgId())==13 || std::abs(mother->pdgId())==15 )){
                        finalstateleptons_.push_back(mother);        
                }
                else if(mother->status()==1 && (std::abs(mother->pdgId())==12 || std::abs(mother->pdgId())==14 || std::abs(mother->pdgId())==16 )){
                        finalstateneutrinos_.push_back(mother);
                }

        }
}








////////////// private member functions //////////////////

void topDecaySelector::copyFrom(const topDecaySelector& rhs){
	partonshower_=rhs.partonshower_;
	incollectionp_=rhs.incollectionp_;
        statTop_=rhs.statTop_;
        finalstateleptons_=rhs.finalstateleptons_;
        finalstateleptonsdressed_=rhs.finalstateleptonsdressed_;
        finalstateleptonsfromw_=rhs.finalstateleptonsfromw_;
        meleptons_=rhs.meleptons_;
        dectops_=rhs.dectops_;
        decws_=rhs.decws_;
        metops_=rhs.metops_;
        mews_=rhs.mews_;
        finalstateneutrinos_=rhs.finalstateneutrinos_;
        meneutrinos_=rhs.meneutrinos_;
        mebs_=rhs.mebs_;
        dectops_mothers_=rhs.dectops_mothers_;
        mews_mothers_=rhs.mews_mothers_;
        decws_mothers_=rhs.decws_mothers_;
        meleptons_mothers_=rhs.meleptons_mothers_;
        finalstateleptonsfromw_mothers_=rhs.finalstateleptonsfromw_mothers_;
        meneutrinos_mothers_=rhs.meneutrinos_mothers_;
        mebs_mothers_=rhs.mebs_mothers_;
        metops_daughters_=rhs.metops_daughters_;
        dectops_daughters_=rhs.dectops_daughters_;
        mews_daughters_=rhs.mews_daughters_;
        decws_daughters_=rhs.decws_daughters_;
        meleptons_daughters_=rhs.meleptons_daughters_;
}

//Function to find last particle in decay chain
const reco::GenParticle* topDecaySelector::findLastParticle(const reco::GenParticle* p) {
        if(debug) std::cout<<"Looking for last particle in chain."<<std::endl;
        unsigned int particleID = std::abs(p->pdgId());
        bool containsItself = false;
        unsigned int d_idx = 0;
        for (unsigned idx = 0; idx < p->numberOfDaughters(); ++idx) {
                if (std::abs(p->daughter(idx)->pdgId()) == particleID) {
                        containsItself = true;
                        d_idx = idx;
                }
        }

        if (!containsItself) return p;
        else {
                if (partonshower_ == ps_pythia6 && (particleID == 24 || particleID == 6)){
                /* Pythia6 has a weird idea of W bosons and top quarks
                W (status == 3) -> q qbar' W. The new W is status 2 and has no daughters
                */
                        if(p->status() == 3)return p;
                }
        return findLastParticle((reco::GenParticle*)p->daughter(d_idx));
        }
}


//Function to set status codes for the top, this function is private, it is called when the partonShower is defined
void topDecaySelector::setTopStatus(partonShowers ps){
        if (ps == ps_pythia6) statTop_ = 3;
        if (ps==ps_pythia8) statTop_=22;
        if (ps==ps_herwig) statTop_=3;
}

}
