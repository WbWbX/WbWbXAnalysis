/*
 * topDecaySelector.h
 *
 *  Created on: Apr 7, 2015
 *      Author: kiesej
 */

#ifndef TOPDECAYSELECTOR_H_
#define TOPDECAYSELECTOR_H_


#include "DataFormats/HepMCCandidate/interface/GenParticle.h"


/*
 * I added some C++ programming comments, can be deleted afterwards
 * please mind const'ness. First, it seems a pain, but later it provides very
 * useful constraints on what you expect from a function and what not
 *
 * I would think of something like:
 * topDecaySelector tds;
 * tds.setPartonShower(topDecaySelector::ps_pythia6);
 * tds.setGenCollection(allgeneratedparticles);
 * tds.process();
 *
 *  // get output.
 *
 */

namespace ztop{

class topDecaySelector {
public:
	enum partonShowers{ps_pythia6,ps_pythia8,ps_herwig};

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

	// This is the input collection (in TreeWriterBase this would be "allgen")
	//C++: this function is just inlined, should be done for simple setter and getter
	void setGenCollection( std::vector<const reco::GenParticle *>* in){
		incollectionp_=in;}

	void setPartonShower(partonShowers ps){partonshower_=ps;
                setTopStatus(partonshower_);}

	// I would suggest to put this in one function and produce the collections here
	// afterwards, they are just asked for with getBlabla()
	// this way, there is a clear separation of const access (get an output)
	// and non-const "produce" steps.
	void process();

	//C++: this function is just inlined, should be done for simple setter and getter
	/**
	 * returns collection of final state leptons following the rivet definition "not from hadron"
	 */
	const std::vector<const reco::GenParticle *>& getFinalStateLeptons()const{return finalstateleptons_;}

	/**
	 * returns collection of final state leptons following the rivet definition "not from hadron" and dressed with photons
	 */
	const std::vector<const reco::GenParticle *>& getFinalStateLeptonsDressed()const{return finalstateleptonsdressed_;}

	/**
	 * returns collection of final state leptons following the rivet definition "not from hadron"
	 */
	const std::vector<const reco::GenParticle *>& getFinalStateLeptonsFromW()const{return finalstateleptonsfromw_;}

	const std::vector<const reco::GenParticle *>& getMELeptons()const{return meleptons_;}
        
        const std::vector<const reco::GenParticle *>& getDecTops()const{return dectops_;}

        const std::vector<const reco::GenParticle *>& getDecWs()const{return decws_;}

        const std::vector<const reco::GenParticle *>& getMETops()const{return metops_;}
  
        const std::vector<const reco::GenParticle *>& getMEWs()const{return mews_;}

        const std::vector<const reco::GenParticle *>& getMENeutrinos()const{return meneutrinos_;}
 
        const std::vector<const reco::GenParticle *>& getFinalStateNeutrinos()const{return finalstateneutrinos_;}

        const std::vector<const reco::GenParticle *>& getMEBs()const{return mebs_;}        

private:

	//configuration members
	partonShowers partonshower_;

	//input collection
	std::vector<const reco::GenParticle *>* incollectionp_;
        
        // The status codes for the Matrix Element and the decaying top quarks, and a function to define them
        int statTop_;
        void setTopStatus (partonShowers ps);
  
        //Function to follow up decay chains
        const reco::GenParticle* findLastParticle(const reco::GenParticle* p);

	//output collections: ...
	//fill them with pointers
	//better fill more collections and avoid switches.
	//we could put many switches and then many instances of the topDecaySelector class
	//if you prefer that...
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
	/* ...
	std::vector<const reco::GenParticle *> mews_;
	std::vector<const reco::GenParticle *> finalstateleptons_;
	 */
        
        std::vector<int > metops_mothers_;
        std::vector<int > metops_daughters_;
        
	// function used by operator= and copy-constructor.
	// needs to be adapted to the members
	void copyFrom(const topDecaySelector&);
};


}//namespace





#endif /* TOPDECAYSELECTOR_H_ */ 
