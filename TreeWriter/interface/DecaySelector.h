/*
 * DecaySelector.h
 *
 *  Created on: 30 Jan 2017
 *      Author: jkiesele
 */

#ifndef TTZANALYSIS_TREEWRITER_INTERFACE_DECAYSELECTOR_H_
#define TTZANALYSIS_TREEWRITER_INTERFACE_DECAYSELECTOR_H_



namespace ztop{

class DecaySelector {
public:
    enum partonShowers{ps_pythia6,ps_pythia8,ps_herwig,ps_herwigpp};



    DecaySelector(): partonshower_(ps_pythia6),incollectionp_(0){}

    void setPartonShower(partonShowers ps){partonshower_=ps;}

    const partonShowers& getPartonShower()const{return partonshower_;}

    void setGenCollection( std::vector<const reco::GenParticle *>* in){
        incollectionp_=in;}

protected:

    //configuration members
    partonShowers partonshower_;

    //input collection
    std::vector<const reco::GenParticle *>* incollectionp_;

};
}

#endif /* TTZANALYSIS_TREEWRITER_INTERFACE_DECAYSELECTOR_H_ */
