/*
 * analysisPlots.h
 *
 *  Created on: Feb 12, 2014
 *      Author: kiesej
 */

#ifndef ANALYSISPLOTS_H_
#define ANALYSISPLOTS_H_
#include <stdexcept>
#include <vector>
#include "TString.h"
#include "TtZAnalysis/Tools/interface/histo1DUnfold.h"
#include "NTFullEvent.h"

namespace ztop{

/**
 * class to contain container1DUnfolds
 * NO automatic step identification as in controlPlotsBasket
 * much simpler
 */
class analysisPlots{
public:
	analysisPlots():eventp_(0),use_(false),step_(0){
		throw std::logic_error("analysisPlots must be initialized with a defined step");
	}
	analysisPlots(size_t step):eventp_(0),use_(false),step_(step){}
	virtual ~analysisPlots(){
		for(size_t i=0;i<conts_.size();i++){
			if(conts_.at(i)){
				delete conts_.at(i);
				conts_.at(i)=0;//...
			}
		}
		for(size_t i=0;i<conts1D_.size();i++){
			if(conts1D_.at(i)){
				delete conts1D_.at(i);
				conts1D_.at(i)=0;//...
			}
		}
	}
	/**
	 * define your plots here
	 */
	virtual void bookPlots()=0;
	virtual void fillPlotsGen()=0;
	virtual void fillPlotsReco()=0;

	void setEvent(const NTFullEvent&evt){eventp_=&evt;}

	void enable(bool doit=true){use_=doit;}
	void disable(){use_=false;}

protected:

	const NTFullEvent * event()const{
		if(!eventp_){
			throw std::runtime_error("analysisPlots: event(): event not set!");
		}
		return eventp_;
	}

	const float& puweight(){
		if(event() && event()->puweight) return *(event()->puweight);
		throw std::runtime_error("analysisPlots: no puweight associated");
	}
	const bool & use()const{return use_;}
	histo1DUnfold* addPlot(const std::vector<float>& genbins, const std::vector<float>& recobins, const TString& name, const TString &xname, const TString &yname);
	histo1D* addPlot1D(const std::vector<float>& bins, const TString& name, const TString &xname, const TString &yname);

	std::vector<float> makebins(size_t nbins,float min,float max)const;

private:


	const NTFullEvent * eventp_;
	bool use_;
	std::vector<histo1DUnfold* > conts_;
	std::vector<histo1D* > conts1D_;
	size_t step_;
};

}


#endif /* ANALYSISPLOTS_H_ */
