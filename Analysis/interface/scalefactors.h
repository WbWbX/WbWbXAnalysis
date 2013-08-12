/*
 * scalefactors.h
 *
 *  Created on: Aug 1, 2013
 *      Author: kiesej
 */

#ifndef SCALEFACTORS_H_
#define SCALEFACTORS_H_

#include "TH1D.h"
#include "TH2D.h"
#include "TString.h"
#include <iostream>

namespace ztop{

class scalefactors{
	public:
	scalefactors(){syst_=100;h=0;}
	scalefactors(const scalefactors&);
	~scalefactors(){}

	int setInput(TString filename, TString histname);
	int setSystematics(TString updownnom);
	void setIsMC(bool is){isMC_=is;}

	double getScalefactor(double xval,double yval=0);

	private:
	TH1D th1d_;
	TH2D th2d_;
	TH1 * h;
	bool isth2d_,isMC_;
	int syst_;

};

/**
 * gets scale factor for either 2D or 1D input
 * returns -999999 if no histo input has been set
 */

inline double scalefactors::getScalefactor(double xval,double yval){
	if(!isMC_)
		return 1;
	if(!h){
		std::cout << "scalefactors::getScalefactor: input has not been set! returning -999999" << std::endl;
		return -999999;
	}

	Int_t bin=h->FindBin(xval,yval);
	double content=h->GetBinContent(bin);
	if(content<0.001){
		std::cout << "scalefactors::getScalefactor: scale factor very small ~0. warning\nmaybe ut of range? input was: " << xval<< ","<< yval<< std::endl;
	}
	if(syst_>99)
		return content;
	else if(syst_<0) //down
		return content-h->GetBinError(bin);
	else if(syst_>0) //up
		return content+h->GetBinError(bin);
	else
		return 0; //never reached
}




}//namespace
#endif /* SCALEFACTORS_H_ */
