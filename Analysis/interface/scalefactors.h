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
#include <stdexcept>

namespace ztop{

class scalefactors{
public:
	scalefactors(): h(0), isth2d_(false), isMC_(false), syst_(100), rangecheck_(true),isglobal_(false),glsf_(0),glsfup_(0),glsfd_(0){}
	scalefactors(const scalefactors&);
	~scalefactors(){}

	int setInput(TString filename, TString histname);
	void setGlobal(double sf,double errup=0,double errdown=0);
	int setSystematics(TString updownnom);

	void setIsMC(bool is){isMC_=is;}
	void setRangeCheck(bool check){rangecheck_=check;}

	bool isSystematicsUp(){return (syst_ <100 && syst_>0);}
	bool isSystematicsDown(){return ( syst_<0);}

	double getScalefactor(double xval,double yval=0);

private:
	TH1D th1d_;
	TH2D th2d_;
	TH1 * h;
	bool isth2d_,isMC_;
	int syst_;
	bool rangecheck_;
	bool isglobal_;
	double glsf_,glsfup_,glsfd_;

};

/**
 * gets scale factor for either 2D or 1D input
 * returns -999999 if no histo input has been set
 */

inline double scalefactors::getScalefactor(double xval,double yval){
	if(!isMC_)
		return 1;
	if(!isglobal_ && !h){
		std::cout << "scalefactors::getScalefactor: input has not been set! returning -999999" << std::endl;
		return -999999;
	}

	if(!isglobal_){
		double content=0;
		Int_t bin=h->FindBin(xval,yval);
		content=h->GetBinContent(bin);
		if(content<0.001 && rangecheck_){
			std::cout << "scalefactors::getScalefactor: scale factor very small ~0. warning\nmaybe ut of range? input was: " << xval<< ","<< yval<< std::endl;
			throw std::runtime_error("scalefactors::getScalefactor: scale factor very small ~0. maybe ut of range");
		}
		if(content<0.001){
			return 1;
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
	else{ //global sf
		if(syst_>99)
			return glsf_;
		else if(syst_<0) //down
			return glsfd_;
		else if(syst_>0) //up
			return glsfup_;
		else return 0; //never reached
	}
}




}//namespace
#endif /* SCALEFACTORS_H_ */
