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
        for(float i=(float)L;i<=((float) H)+((float)H-(float)L)/((float)N)*0.01;i+=((float)H-(float)L)/((float)N)) \
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
    controlPlotBasket():switchedon_(true),limittostep_(-1),tmpidx_(0),tmpstep_(0),tmpnewstep_(true),evt_(0),lcont_(0),size_(0),initphase_(false){}
    virtual ~controlPlotBasket();

    void linkEvent(const NTFullEvent & evt){evt_=&evt;}


    virtual void makeControlPlots(const size_t& step)=0;
    /**
     * this is an optional function. But it helps to keep a particular ordering of the plots!
     */
    void initSteps(size_t no);

    void enable(bool swon=true){switchedon_=swon;}

    void limitToStep(int step){limittostep_=step;}

protected:
    const NTFullEvent * event(){return evt_;}

    container1D *addPlot(const TString & name, const TString & xaxisname,const TString & yaxisname, const bool & mergeufof=true);

    void initStep(const size_t& step);
    std::vector<float> & setBins(){tempbins_.clear();return tempbins_;}
    std::vector<float> & getBins(){return tempbins_;}
    container1D *  &last(){return lcont_;}

    const bool & isNewStep()const{return tmpnewstep_;}

    bool switchedon_;

    int limittostep_;

private:


    static std::vector<TString> namelist;
    size_t tmpidx_;
    size_t tmpstep_;
    bool tmpnewstep_;
    const NTFullEvent * evt_;
    container1D * lcont_;
    size_t size_;
    std::vector<float> tempbins_;
    std::vector< std::vector<ztop::container1D *> > cplots_; //step,plot
    std::vector< std::vector<ztop::container1D> > scplots_; //stack! step,plot
    bool initphase_;

};
}//namespace


#endif /* CONTROLPLOTBASKET_H_ */
