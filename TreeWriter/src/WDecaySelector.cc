/*
 * WDecaySelector.cc
 *
 *  Created on: Apr 7, 2015
 *      Author: kiesej
 */




#include "../interface/WDecaySelector.h"
#include <iostream>
#include <stdexcept>

namespace ztop{

bool WDecaySelector::debug=false;

//C++ initialize all members here that need an initial value. keep ordering
WDecaySelector::WDecaySelector(): partonshower_(ps_pythia6),incollectionp_(0)
{
    // I usually put this kind of debug ouput on top of each function call, it makes debugging
    // much easier. Also in case it is done long after the class was developed
    if(debug)
        std::cout << "WDecaySelector::WDecaySelector(): constructor" << std::endl;

}

WDecaySelector::~WDecaySelector(){
    //nothing to be done here
}


//C++: initializer should be defined
WDecaySelector::WDecaySelector(const WDecaySelector&rhs){
    copyFrom(rhs);
}

//C++: equal operator should be defined. return *this (to allow a=b=c;)
WDecaySelector& WDecaySelector::operator = (const WDecaySelector&rhs){
    if(this==&rhs) return *this;
    copyFrom(rhs);
    return *this;
}

//this is where the magic happens
void  WDecaySelector::process(){


    meleptons_.clear();
    mews_.clear();
    meneutrinos_.clear();

    finalstateleptons_.clear();
    finalstateleptonsfromw_.clear();
    finalstateneutrinos_.clear();

    if(!incollectionp_){// pointer 0 -> real check, no example! ;)
        throw std::logic_error("WDecaySelector::process: input collection not defined");
    }

    if(debug) std::cout<<"WDecaySelector::process: filling ws"<< std::endl;

    for(const auto& p: *incollectionp_){
        if(std::abs(p->pdgId()) == 24 && p->isHardProcess()){
            mews_.push_back(p);
        }
        if((std::abs(p->pdgId()) == 11 ||
                std::abs(p->pdgId()) == 13 ||
                std::abs(p->pdgId()) == 15 )
                && p->isHardProcess()){
            meleptons_.push_back(p);
        }
        if((std::abs(p->pdgId()) == 12 ||
                std::abs(p->pdgId()) == 14 ||
                std::abs(p->pdgId()) == 16 )
                && p->isHardProcess()){
            meneutrinos_.push_back(p);
        }
    }
}




////////////// private member functions //////////////////

void WDecaySelector::copyFrom(const WDecaySelector& rhs){
    partonshower_=rhs.partonshower_;
    incollectionp_=rhs.incollectionp_;
    finalstateleptons_=rhs.finalstateleptons_;
    finalstateleptonsfromw_=rhs.finalstateleptonsfromw_;
    meleptons_=rhs.meleptons_;
    mews_=rhs.mews_;
    finalstateneutrinos_=rhs.finalstateneutrinos_;
    meneutrinos_=rhs.meneutrinos_;

}

//Function to find last particle in decay chain
const reco::GenParticle* WDecaySelector::findLastParticle(const reco::GenParticle* p) {
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

//Function to handle Tau Decays
const reco::GenParticle* WDecaySelector::tauDecay(const reco::GenParticle* tau){
    const reco::GenParticle* daughter,*secondDaughter;
    for (unsigned i=0;i<tau->numberOfDaughters();i++){
        daughter = dynamic_cast<const reco::GenParticle*> (tau->daughter(i));
        if(std::abs(daughter->pdgId())==11 || std::abs(daughter->pdgId())==13){
            secondDaughter= findLastParticle(daughter);
            if(secondDaughter->status()==1) return daughter;
        }
    }
    return 0;
}


}
