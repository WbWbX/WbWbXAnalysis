/*
 * ZControlPlots.cc
 *
 *  Created on: Nov 1, 2013
 *      Author: kiesej
 */

#include "../interface/ZControlPlots.h"
#include "../interface/NTFullEvent.h"

#include "TtZAnalysis/DataFormats/src/classes.h"

namespace ztop{
/*

enum etapten{
		pt2030_eta05,
		pt2030_eta0515,
		pt2030_eta1521,
		pt2030_eta2124,

		pt3040_eta05,
		pt3040_eta0515,
		pt3040_eta1521,
		pt3040_eta2124,

		pt4060_eta05,
		pt4060_eta0515,
		pt4060_eta1521,
		pt4060_eta2124,

		pt60100_eta05,
		pt60100_eta0515,
		pt60100_eta1521,
		pt60100_eta2124,

		pt100inf_eta05,
		pt100inf_eta0515,
		pt100inf_eta1521,
		pt100inf_eta2124,

	} ptetacategory;
 */
bool ZControlPlots::categoryCutsFullfilled(const NTLepton* l1, const NTLepton* l2, const float& ptcutlow, const float & ptcuth,const float& etacutlow, const float & etacuth)const{
	//const NTLepton * tmp;



	float abseta1=fabs(l1->eta());
	float abseta2=fabs(l2->eta());
	if(abseta1 < etacutlow) return false;
	if(abseta2 < etacutlow) return false;
	if(l1->pt() < ptcutlow) return false;
	if(l2->pt() < ptcutlow) return false;
	if(abseta1 > etacuth) return false;
	if(abseta2 > etacuth) return false;
	if(l1->pt() > ptcuth) return false;
	if(l2->pt() > ptcuth) return false;
	return true;
}


void ZControlPlots::setCategory(const NTLepton* l1, const NTLepton* l2){
	if(!l1 || !l2){
		ptetacategory_=invalid;
		return;
	}
	ptetacategory_=invalid;

	if(categoryCutsFullfilled(l1,l2, 20, 30, 0, 0.5))
		ptetacategory_=pt2030_eta05;
	if(categoryCutsFullfilled(l1,l2, 20, 30, 0.5, 1.5))
		ptetacategory_=pt2030_eta0515;
	if(categoryCutsFullfilled(l1,l2, 20, 30, 1.5, 2.1))
		ptetacategory_=pt2030_eta1521;
	if(categoryCutsFullfilled(l1,l2, 20, 30, 2.1, 2.4))
		ptetacategory_=pt2030_eta2124;

	if(categoryCutsFullfilled(l1,l2, 30, 40, 0, 0.5))
		ptetacategory_=pt3040_eta05;
	if(categoryCutsFullfilled(l1,l2, 30, 40, 0.5, 1.5))
		ptetacategory_=pt3040_eta0515;
	if(categoryCutsFullfilled(l1,l2, 30, 40, 1.5, 2.1))
		ptetacategory_=pt3040_eta1521;
	if(categoryCutsFullfilled(l1,l2, 30, 40, 2.1, 2.4))
		ptetacategory_=pt3040_eta2124;


	if(categoryCutsFullfilled(l1,l2, 40, 60, 0, 0.5))
		ptetacategory_=pt4060_eta05;
	if(categoryCutsFullfilled(l1,l2, 40, 60, 0.5, 1.5))
		ptetacategory_=pt4060_eta0515;
	if(categoryCutsFullfilled(l1,l2, 40, 60, 1.5, 2.1))
		ptetacategory_=pt4060_eta1521;
	if(categoryCutsFullfilled(l1,l2, 40, 60, 2.1, 2.4))
		ptetacategory_=pt4060_eta2124;


	if(categoryCutsFullfilled(l1,l2, 60, 100, 0, 0.5))
		ptetacategory_=pt60100_eta05;
	if(categoryCutsFullfilled(l1,l2, 60, 100, 0.5, 1.5))
		ptetacategory_=pt60100_eta0515;
	if(categoryCutsFullfilled(l1,l2, 60, 100, 1.5, 2.1))
		ptetacategory_=pt60100_eta1521;
	if(categoryCutsFullfilled(l1,l2, 60, 100, 2.1, 2.4))
		ptetacategory_=pt60100_eta2124;


	if(categoryCutsFullfilled(l1,l2, 100, 1000000, 0, 0.5))
		ptetacategory_=pt100inf_eta05;
	if(categoryCutsFullfilled(l1,l2, 100, 1000000, 0.5, 1.5))
		ptetacategory_=pt100inf_eta0515;
	if(categoryCutsFullfilled(l1,l2, 100, 1000000, 1.5, 2.1))
		ptetacategory_=pt100inf_eta1521;
	if(categoryCutsFullfilled(l1,l2, 100, 1000000, 2.1, 2.4))
		ptetacategory_=pt100inf_eta2124;


}




/**
 * setbins, addplot, FILL
 */
void ZControlPlots::makeControlPlots(const size_t & step){
	initStep(step);
	using namespace std;


	SETBINSRANGE(120,60,120);
	addPlot("mll Z Range","m_{ll}[GeV]","Events/GeV",true);
	FILLSINGLE(mll);



	//make pt,eta plots
	SETBINSRANGE(55,85,100);
	addPlot("mll Z Peak overview","m_{ll}[GeV]","Events/GeV",false);
	FILLSINGLE(mll);
	if(event())
		setCategory(event()->leadinglep,event()->secleadinglep);


	SETBINSRANGE(30,89,95);
	addPlot("mll Z Peak barrel","m_{ll}[GeV]","Events/GeV",false);
	if(event() && event()->leadinglep && event()->secleadinglep){
		if(categoryCutsFullfilled(event()->leadinglep,event()->secleadinglep,0,100000000,0,1.5))
			FILLSINGLE(mll);
	}
	addPlot("mll Z Peak barrel pt40inf","m_{ll}[GeV]","Events/GeV",false);
	if(event() && event()->leadinglep && event()->secleadinglep){
		if(categoryCutsFullfilled(event()->leadinglep,event()->secleadinglep,40,100000,0,1.5))
			FILLSINGLE(mll);
	}
	SETBINSRANGE(30,88,94);
	addPlot("mll Z Peak barrel pt2040","m_{ll}[GeV]","Events/GeV",false);
	if(event() && event()->leadinglep && event()->secleadinglep){
		if(categoryCutsFullfilled(event()->leadinglep,event()->secleadinglep,0,40,0,1.5))
			FILLSINGLE(mll);
	}

	SETBINSRANGE(15,88,95);
	addPlot("mll Z Peak endcaps","m_{ll}[GeV]","Events/GeV",false);
	if(event() && event()->leadinglep && event()->secleadinglep){
		if(categoryCutsFullfilled(event()->leadinglep,event()->secleadinglep,0,100000000,1.5,2.4))
			FILLSINGLE(mll);
	}
	SETBINSRANGE(15,88,94);
	addPlot("mll Z Peak endcaps pt40inf","m_{ll}[GeV]","Events/GeV",false);
	if(event() && event()->leadinglep && event()->secleadinglep){
		if(categoryCutsFullfilled(event()->leadinglep,event()->secleadinglep,40,100000,1.5,2.4))
			FILLSINGLE(mll);
	}
	SETBINSRANGE(10,83,97);
	addPlot("mll Z Peak endcaps pt2040","m_{ll}[GeV]","Events/GeV",false);
	if(event() && event()->leadinglep && event()->secleadinglep){
		if(categoryCutsFullfilled(event()->leadinglep,event()->secleadinglep,0,40,1.5,2.4))
			FILLSINGLE(mll);
	}


	SETBINSRANGE(60,75,106);
	addPlot("mll Z Peak barrel overview","m_{ll}[GeV]","Events/GeV",false);
	if(event() && event()->leadinglep && event()->secleadinglep){
		if(categoryCutsFullfilled(event()->leadinglep,event()->secleadinglep,0,100000000,0,1.5))
			FILLSINGLE(mll);
	}
	SETBINSRANGE(30,75,106);
	addPlot("mll Z Peak endcaps overview","m_{ll}[GeV]","Events/GeV",false);
	if(event() && event()->leadinglep && event()->secleadinglep){
		if(categoryCutsFullfilled(event()->leadinglep,event()->secleadinglep,0,100000000,1.5,2.4))
			FILLSINGLE(mll);
	}

	SETBINSRANGE(30,20,200);
	addPlot("electron pt endcaps","p_{T} [GeV]","Events/GeV",false);
	if(event() && event()->leadinglep && event()->secleadinglep){
		if(categoryCutsFullfilled(event()->leadinglep,event()->secleadinglep,0,100000000,1.5,2.4)){
			FILL(leadinglep,pt());
			FILL(secleadinglep,pt());
		}
	}

}

}//namespace

