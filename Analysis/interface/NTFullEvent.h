/*
 * NTFullEvent.h
 *
 *  Created on: Oct 29, 2013
 *      Author: kiesej
 */

#ifndef NTFULLEVENT_H_
#define NTFULLEVENT_H_

namespace ztop{

class NTEvent;
class NTJet;
class NTMuon;
class NTElectron;
class NTLepton;
class NTMet;

class NTFullEvent{
public:
	NTFullEvent(): event(0),
	idjets(0),medjets(0),hardjets(0),medbjets(0),hardbjets(0),
	allmuons(0),kinmuons(0),idmuons(0),isomuons(0),
	allelectrons(0),kinelectrons(0),idelectrons(0),isoelectrons(0),
	allleptons(0),kinleptons(0),idleptons(0),isoleptons(0),
	leadinglep(0),secleadinglep(0),
	simplemet(0),adjustedmet(0),
	mll(0),leplepdr(0),dphillj(0),ht(0),ptllj(0),topdiscr(0),
	puweight(0){};
	~NTFullEvent(){}

	void reset(){NTFullEvent();}

	ztop::NTEvent * event;

	std::vector<ztop::NTJet*> * idjets;
	std::vector<ztop::NTJet*> * medjets;
	std::vector<ztop::NTJet*> * hardjets;
	std::vector<ztop::NTJet*> * medbjets;
	std::vector<ztop::NTJet*> * hardbjets;

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

	double * mll;
	double * leplepdr;
	double * dphillj;
	double * ht;
	double * ptllj;
	double * topdiscr;

	double * puweight;

};
}//namespace
#endif /* NTFULLEVENT_H_ */
