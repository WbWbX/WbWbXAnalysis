/*
 * NTFullEvent.h
 *
 *  Created on: Oct 29; 2013
 *      Author: kiesej
 */

#ifndef NTFULLEVENT_H_
#define NTFULLEVENT_H_
#include "TtZAnalysis/DataFormats/interface/mathdefs.h"
#include "TtZAnalysis/DataFormats/interface/NTLorentzVector.h"
#include <vector>
namespace ztop{

class NTEvent;
class NTJet;
class NTMuon;
class NTElectron;
class NTLepton;
class NTMet;
class NTGenParticle;
class NTGenJet;

class NTFullEvent{
public:
    NTFullEvent(): event(0),
    gentops(0),
    genbs(0),
    genleptons1(0),
    genleptons3(0),
    genvisleptons1(0),
    genvisleptons3(0),
    genjets(0),
    genvisjets(0),
    genbjetsfromtop(0),
    genvisbjetsfromtop(0),
    idjets(0),medjets(0),hardjets(0),medbjets(0),hardbjets(0),dphilljjets(0),dphiplushardjets(0),
    selectedjets(0),selectedbjets(0),
    allmuons(0),kinmuons(0),idmuons(0),isomuons(0),
    allelectrons(0),kinelectrons(0),idelectrons(0),isoelectrons(0),
    allleptons(0),kinleptons(0),idleptons(0),isoleptons(0),
    leadinglep(0),secleadinglep(0),
    //leadingjetbtag(0),secleadingjetbtag(0),
    simplemet(0),adjustedmet(0),
    mll(0),leplepdr(0),cosleplepangle(0),dphillj(0),dphilljj(0),detallj(0),detalljj(0),ht(0),ptllj(0),topdiscr(0),topdiscr2(0),topdiscr3(0),

    lhi_dphillj(0),
    lhi_cosleplepangle(0),
    lhi_leadjetbtag(0),
    lhi_sumdphimetl(0),
    lhi_seljetmulti(0),
    lhi_selbjetmulti(0),
    lhi_leadleppt(0),
    lhi_secleadleppt(0),
    lhi_leadjetpt(0),
    lhi_drlbl(0),
    lhi_mll(0),
    lhi_ptllj(0),

    lh_toplh(0),

    midphi(0),

    S4(0),
    allobjects4(0),
    puweight(0){};
    ~NTFullEvent(){}

    void reset(){
        event=0;

        gentops=0;
        genbs=0;
        genleptons1=0;
        genleptons3=0;
        genvisleptons1=0;
        genvisleptons3=0;
        genjets=0;
        genvisjets=0;
        genbjetsfromtop=0;
        genvisbjetsfromtop=0;

        idjets=0;medjets=0;hardjets=0;medbjets=0;hardbjets=0;dphilljjets=0;dphiplushardjets=0;
        selectedjets=0;selectedbjets=0;
        allmuons=0;kinmuons=0;idmuons=0;isomuons=0;
        allelectrons=0;kinelectrons=0;idelectrons=0;isoelectrons=0;
        allleptons=0;kinleptons=0;idleptons=0;isoleptons=0;
        leadinglep=0;secleadinglep=0;
        //leadingjetbtag=0;secleadingjetbtag=0;
        simplemet=0;adjustedmet=0;
        mll=0;leplepdr=0;cosleplepangle=0;dphillj=0;dphilljj=0;detallj=0;detalljj=0;ht=0;ptllj=0;topdiscr=0;topdiscr2=0;topdiscr3=0;

        lhi_dphillj=0;
        lhi_cosleplepangle=0;
        lhi_leadjetbtag=0;
        lhi_sumdphimetl=0;
        lhi_seljetmulti=0;
        lhi_selbjetmulti=0;
        lhi_leadleppt=0;
        lhi_secleadleppt=0;
        lhi_leadjetpt=0;
        lhi_drlbl=0;
        lhi_mll=0;
        lhi_ptllj=0;

        lh_toplh=0;

        midphi=0;
        S4=0;
        allobjects4=0;
        puweight=0;
    }

    ztop::NTEvent * event;
    ///generator info
    std::vector<ztop::NTGenParticle *> * gentops;
    std::vector<ztop::NTGenParticle *> * genbs;
    std::vector<ztop::NTGenParticle *> * genleptons1;
    std::vector<ztop::NTGenParticle *> * genleptons3;
    std::vector<ztop::NTGenParticle *> * genvisleptons1;
    std::vector<ztop::NTGenParticle *> * genvisleptons3;
    std::vector<ztop::NTGenJet *> * genjets;
    std::vector<ztop::NTGenJet *> * genvisjets;
    std::vector<ztop::NTGenJet *> * genbjetsfromtop;
    std::vector<ztop::NTGenJet *> * genvisbjetsfromtop;




    ///reco info
    std::vector<ztop::NTJet*> * idjets;
    std::vector<ztop::NTJet*> * medjets;
    std::vector<ztop::NTJet*> * hardjets;
    std::vector<ztop::NTJet*> * medbjets;
    std::vector<ztop::NTJet*> * hardbjets;
    std::vector<ztop::NTJet*> * dphilljjets;
    std::vector<ztop::NTJet*> * dphiplushardjets;
    std::vector<ztop::NTJet*> * selectedjets;
    std::vector<ztop::NTJet*> * selectedbjets;


    std::vector<ztop::NTMuon*> * allmuons;
    std::vector<ztop::NTMuon*> * kinmuons;
    std::vector<ztop::NTMuon*> * idmuons;
    std::vector<ztop::NTMuon*> * isomuons;

    std::vector<ztop::NTElectron*> * allelectrons;
    std::vector<ztop::NTElectron*> * kinelectrons;
    std::vector<ztop::NTElectron*> * idelectrons;
    std::vector<ztop::NTElectron*> * isoelectrons;

    std::vector<ztop::NTLepton*> * allleptons;
    std::vector<ztop::NTLepton*> * kinleptons;
    std::vector<ztop::NTLepton*> * idleptons;
    std::vector<ztop::NTLepton*> * isoleptons;

    ztop::NTLepton* leadinglep;
    ztop::NTLepton* secleadinglep;


    ztop::NTMet * simplemet;
    ztop::NTMet * adjustedmet;

    float * mll;
    float * leplepdr;
    float * cosleplepangle;
    float * dphillj;
    float * dphilljj;
    float * detallj;
    float * detalljj;
    float * ht;
    float * ptllj;
    float * topdiscr;
    float * topdiscr2;
    float * topdiscr3;

    //inputs for likelihood lhi_<name>

    float * lhi_dphillj;
    float * lhi_cosleplepangle;
    float * lhi_leadjetbtag;
    float * lhi_sumdphimetl;
    float * lhi_seljetmulti;
    float * lhi_selbjetmulti;
    float * lhi_leadleppt;
    float * lhi_secleadleppt;
    float * lhi_leadjetpt;
    float * lhi_drlbl;
    float * lhi_mll;
    float * lhi_ptllj;

    //output of LH lh_<name>
    float * lh_toplh;



    //others
    bool *midphi;
    NTLorentzVector<float> * S4;
    NTLorentzVector<float>  * allobjects4;

    float * puweight;

};
}//namespace
#endif /* NTFULLEVENT_H_ */
