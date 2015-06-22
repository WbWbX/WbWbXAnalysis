/*
 * plot.h
 *
 *  Created on: Jan 6, 2014
 *      Author: kiesej
 */

#ifndef PLOT_H_
#define PLOT_H_

#include "tObjectList.h"
#include "TString.h"
#include "graph.h"
#include "histo1D.h"

class TH1D;
class TGraphAsymmErrors;

namespace ztop{

/**
 * just a simple class incorporating two Tgraphs (with stat and with stat+syst)
 * can give back the input (saved as graph object)
 */
class plot: tObjectList{
public:
    plot();
    plot(const histo1D *,bool divbbw=true);
    plot(const graph*);
    ~plot();
    plot(const plot& rhs);
    plot & operator = (const plot& rhs);

    void createFrom(const histo1D *,bool divbbw=true);
    void createFrom(const graph *);

    void removeXErrors();

    void clear();//{tObjectList::cleanMem();axish_=0;statg_=0;systg_=0;}
    bool empty() const {return tObjectList::size()<1;}

    const TString & getName()const{return inputg_.getName();};


    TGraphAsymmErrors * getSystGraph()const{return systg_;}
    TGraphAsymmErrors * getStatGraph()const{return statg_;}

    void setDivideByBinWidth(bool div){divbybw_=div;}
    bool isDivByBinWidth()const{return isBWdiv_;}

    void Draw(const TString& opt="")const;


    const graph& getInputGraph()const{return inputg_;}
    void getXRange(float& low,float& hi)const{
    	low=xmin_;hi=xmax_;
    }

    static bool debug;

private:

    graph inputg_;
    TGraphAsymmErrors* systg_, *statg_;
    bool isBWdiv_,divbybw_;
    float xmin_,xmax_;
    void cleanMem(){}
    void size()const{}
    //internally 0: TH1D, 1: with syst, 2: only stat


};
}//ns



#endif /* PLOT_H_ */
