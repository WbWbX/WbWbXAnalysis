/*
 * WDecaySelector.h
 *
 *  Created on: Apr 7, 2015
 *      Author: kiesej
 */

#ifndef WDecaySelector_H_
#define WDecaySelector_H_


#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DecaySelector.h"

/*
 * I added some C++ programming comments, can be deleted afterwards
 * please mind const'ness. First, it seems a pain, but later it provides very
 * useful constraints on what you expect from a function and what not
 */
namespace ztop{

class WDecaySelector : public DecaySelector{
public:

    //C++: constructor. initialize private members!
    WDecaySelector();

    //C++: clean up memory (if there is a "new" somewhere, or links are set from other objects)
    // this will clean all collections produced before
    ~WDecaySelector();

    //C++: initializer should be defined
    WDecaySelector(const WDecaySelector&);

    //C++: equal operator should be defined. return *this (to allow a=b=c;)
    WDecaySelector& operator = (const WDecaySelector&);

    //C++: this is global variable ztop::WDecaySelector::debug
    //C++: nice for debug output
    static bool debug;


    // I would suggest to put this in one function and produce the collections here
    // afterwards, they are just asked for with getBlabla()
    void process();


    const std::vector<const reco::GenParticle *>& getFinalStateLeptons()const{return finalstateleptons_;}

    const std::vector<const reco::GenParticle *>& getFinalStateLeptonsFromW()const{return finalstateleptonsfromw_;}

    //returns collection of leptons directly from the W
    const std::vector<const reco::GenParticle *>& getMELeptons()const{return meleptons_;}

    //returns collection of the Ws directly from the Tops
    const std::vector<const reco::GenParticle *>& getMEWs()const{return mews_;}

    //returns collection of the Neutrinos directly form the Ws
    const std::vector<const reco::GenParticle *>& getMENeutrinos()const{return meneutrinos_;}



    //returns collection of all Status 1 Neutrinos
    const std::vector<const reco::GenParticle *>& getFinalStateNeutrinos()const{return finalstateneutrinos_;}


private:



    //Function to follow up decay chains
    const reco::GenParticle* findLastParticle(const reco::GenParticle* p);
    const reco::GenParticle* tauDecay(const reco::GenParticle* tau);

    //output collections: Fill with Pointers
    //better fill more collections and avoid switches.
    std::vector<const reco::GenParticle *> finalstateleptons_;
    std::vector<const reco::GenParticle *> finalstateleptonsfromw_;
    //leps directly from w on ME level
    std::vector<const reco::GenParticle *> meleptons_, melepton_mothers_;
    std::vector<const reco::GenParticle *> mews_;

    //Neutrinos
    std::vector<const reco::GenParticle *> finalstateneutrinos_;
    std::vector<const reco::GenParticle *> meneutrinos_, meneutrino_mothers_;

    // function used by operator= and copy-constructor.
    // needs to be adapted to the members
    void copyFrom(const WDecaySelector&);
};


}//namespace





#endif /* WDecaySelector_H_ */
