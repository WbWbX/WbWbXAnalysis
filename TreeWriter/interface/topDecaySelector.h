/*
 * topDecaySelector.h
 *
 *  Created on: Apr 7, 2015
 *      Author: kiesej
 */

#ifndef TOPDECAYSELECTOR_H_
#define TOPDECAYSELECTOR_H_


#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "DecaySelector.h"

/*
 * I added some C++ programming comments, can be deleted afterwards
 * please mind const'ness. First, it seems a pain, but later it provides very
 * useful constraints on what you expect from a function and what not
 */
namespace ztop{

class topDecaySelector: public DecaySelector {
public:

    //C++: constructor. initialize private members!
    topDecaySelector();

    //C++: clean up memory (if there is a "new" somewhere, or links are set from other objects)
    // this will clean all collections produced before
    ~topDecaySelector();

    //C++: initializer should be defined
    topDecaySelector(const topDecaySelector&);

    //C++: equal operator should be defined. return *this (to allow a=b=c;)
    topDecaySelector& operator = (const topDecaySelector&);

    //C++: this is global variable ztop::topDecaySelector::debug
    //C++: nice for debug output
    static bool debug;


    void setPartonShower(partonShowers ps){
        DecaySelector::setPartonShower(ps);
        setTopStatus(partonshower_);}

    // I would suggest to put this in one function and produce the collections here
    // afterwards, they are just asked for with getBlabla()
    void process();

    // returns collection of final state leptons following the rivet definition "not from hadron"
    const std::vector<const reco::GenParticle *>& getFinalStateLeptons()const{return finalstateleptons_;}

    /**
     * returns collection of final state leptons following the rivet definition "not from hadron" and dressed with photons
     */
    const std::vector<const reco::GenParticle *>& getFinalStateLeptonsDressed()const{return finalstateleptonsdressed_;}

    /**
     * returns collection of final state leptons following the rivet definition "not from hadron"
     */
    const std::vector<const reco::GenParticle *>& getFinalStateLeptonsFromW()const{return finalstateleptonsfromw_;}

    //returns collection of leptons directly from the W
    const std::vector<const reco::GenParticle *>& getMELeptons()const{return meleptons_;}

    //returns collection of the Decaying Tops (same as METops for Pythia6)
    const std::vector<const reco::GenParticle *>& getDecTops()const{return dectops_;}

    //returns collection of the Decaying Ws (same as MEWs for Pythia6)
    const std::vector<const reco::GenParticle *>& getDecWs()const{return decws_;}

    //returns collection of the Matrix Element Tops
    const std::vector<const reco::GenParticle *>& getMETops()const{return metops_;}

    //returns collection of the Ws directly from the Tops
    const std::vector<const reco::GenParticle *>& getMEWs()const{return mews_;}

    //returns collection of the Neutrinos directly form the Ws
    const std::vector<const reco::GenParticle *>& getMENeutrinos()const{return meneutrinos_;}

    //returns collection of all Status 1 Neutrinos
    const std::vector<const reco::GenParticle *>& getFinalStateNeutrinos()const{return finalstateneutrinos_;}

    //returns collection of the bs directly from the Ws
    const std::vector<const reco::GenParticle *>& getMEBs()const{return mebs_;}

    //Mothers
    const std::vector<const reco::GenParticle *>& getDecTopsMothers()const{return dectops_mothers_;}

    const std::vector<const reco::GenParticle *>& getMEWsMothers()const{return mews_mothers_;}

    const std::vector<const reco::GenParticle *>& getDecWsMothers()const{return decws_mothers_;}

    const std::vector<const reco::GenParticle *>& getMELeptonsMothers()const{return meleptons_mothers_;}

    const std::vector<const reco::GenParticle *>& getFinalStateLeptonsFromWMothers()const{return finalstateleptonsfromw_mothers_;}

    const std::vector<const reco::GenParticle *>& getMENeutrinosMothers()const{return meneutrinos_mothers_;}

    const std::vector<const reco::GenParticle *>& getMEBsMothers()const{return mebs_mothers_;}

    //Daughters

    const std::vector<const reco::GenParticle *>& getMETopsDaughters()const{return metops_daughters_;}

    const std::vector<const reco::GenParticle *>& getDecTopsDaughters()const{return dectops_daughters_;}

    const std::vector<const reco::GenParticle *>& getMEWsDaughters()const{return mews_daughters_;}

    const std::vector<const reco::GenParticle *>& getDecWsDaughters()const{return decws_daughters_;}

    const std::vector<const reco::GenParticle *>& getMELeptonsDaughters()const{return meleptons_daughters_;}




private:

    // The status codes for the Matrix Element and the decaying top quarks, and a function to define them
    int statTop_;
    void setTopStatus (partonShowers ps);

    //Function to follow up decay chains
    const reco::GenParticle* findLastParticle(const reco::GenParticle* p);
    const reco::GenParticle* tauDecay(const reco::GenParticle* tau);

    //output collections: Fill with Pointers
    //better fill more collections and avoid switches.
    std::vector<const reco::GenParticle *> finalstateleptons_;
    std::vector<const reco::GenParticle *> finalstateleptonsdressed_;
    std::vector<const reco::GenParticle *> finalstateleptonsfromw_;
    //leps directly from w on ME level
    std::vector<const reco::GenParticle *> meleptons_;
    // decaying tops and ws
    std::vector<const reco::GenParticle *> dectops_;
    std::vector<const reco::GenParticle *> decws_;
    // Matrix Element tops and ws
    std::vector<const reco::GenParticle *> metops_;
    std::vector<const reco::GenParticle *> mews_;

    //Neutrinos
    std::vector<const reco::GenParticle *> finalstateneutrinos_;
    std::vector<const reco::GenParticle *> meneutrinos_;
    //Bs from ME
    std::vector<const reco::GenParticle *> mebs_;

    //Mothers
    std::vector<const reco::GenParticle *> dectops_mothers_;
    std::vector<const reco::GenParticle *> mews_mothers_;
    std::vector<const reco::GenParticle *> decws_mothers_;
    std::vector<const reco::GenParticle *> meleptons_mothers_;
    std::vector<const reco::GenParticle *> finalstateleptonsfromw_mothers_;
    std::vector<const reco::GenParticle *> meneutrinos_mothers_;
    std::vector<const reco::GenParticle *> mebs_mothers_;

    //Daughters
    std::vector<const reco::GenParticle *> metops_daughters_;
    std::vector<const reco::GenParticle *> dectops_daughters_;
    std::vector<const reco::GenParticle *> mews_daughters_;
    std::vector<const reco::GenParticle *> decws_daughters_;
    std::vector<const reco::GenParticle *> meleptons_daughters_;

    // function used by operator= and copy-constructor.
    // needs to be adapted to the members
    void copyFrom(const topDecaySelector&);
};


}//namespace





#endif /* TOPDECAYSELECTOR_H_ */ 
