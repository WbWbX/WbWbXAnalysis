/*
 * controlPlotBasket.h
 *
 *  Created on: Oct 29, 2013
 *      Author: kiesej
 */

#ifndef CONTROLPLOTBASKET_H_
#define CONTROLPLOTBASKET_H_

#include "TtZAnalysis/Tools/interface/container.h"
#include <vector>

//helper
#define FILLFOREACH(A,B) if(event()->A)for(size_t i=0;i<event()->A->size();i++)last()->fill(event()->A->at(i)->B,*event()->puweight)
#define FILL(A,B) if(event()->A)last()->fill(event()->A->B,*event()->puweight)
#define FILLSINGLE(A) if(event()->A)last()->fill(*(event()->A),*event()->puweight)
#define SETBINS if(isNewStep()) setBins()
#define SETBINSRANGE(N,L,H) if(isNewStep()){ \
	setBins(); \
	for(float i=(float)L;i<=((float) H);i+=((float)H-(float)L)/((float)N)) \
	getBins().push_back(i);}

namespace ztop{
class NTFullEvent;
/**
 * Warning undefined behaviour when getting copied. needs to be fixed
 * or prohibited, because it would result in same named containers
 *
 */
class controlPlotBasket{
public:
	controlPlotBasket():tmpidx_(0),tmpstep_(0),tmpnewstep_(true),evt_(0),lcont_(0),size_(0){}
	virtual ~controlPlotBasket();

	void setEvent(const NTFullEvent & evt){evt_=&evt;}

	const NTFullEvent * event(){return evt_;}

	container1D *addPlot(const TString & name, const TString & xaxisname,const TString & yaxisname, const bool & mergeufof=true);

	void initStep(const size_t& step);

	const bool & isNewStep()const{return tmpnewstep_;}
	std::vector<float> & setBins(){tempbins_.clear();return tempbins_;}
	std::vector<float> & getBins(){return tempbins_;}

	virtual void makeControlPlots(const size_t& step)=0;

	container1D *  &last(){return lcont_;}

	static std::vector<TString> namelist;

private:


	size_t tmpidx_;
	size_t tmpstep_;
	bool tmpnewstep_;
	const NTFullEvent * evt_;
	container1D * lcont_;
	size_t size_;
	std::vector<float> tempbins_;
	std::vector< std::vector<ztop::container1D *> > cplots_; //step,plot
	std::vector< std::vector<ztop::container1D> > scplots_; //stack! step,plot

};
}//namespace


#endif /* CONTROLPLOTBASKET_H_ */
