/*
 * plot.cc
 *
 *  Created on: Jan 6, 2014
 *      Author: kiesej
 */

#include "../interface/plot.h"
#include "TGraphAsymmErrors.h"
#include "TH1D.h"
#include "../interface/container.h"
#include <stdexcept>
namespace ztop{


bool plot::debug=false;

plot::plot():tObjectList(),inputg_(graph()),systg_(0),statg_(0),isBWdiv_(false),divbybw_(true){
    if(debug) std::cout << "plot()" <<std::endl;

}

plot::plot(const container1D *c,bool divbbw){
    if(debug) std::cout << "plot(const container1D *)" <<std::endl;
    if(!c || c->isDummy()){
        throw std::logic_error("plot(const graph *) point NULL or graph empty" );
    }
    TH1::AddDirectory(false);
    divbybw_=divbbw;
    statg_ = addObject(c->getTGraph("",divbybw_,true,false,false));
    systg_ = addObject(c->getTGraph("",divbybw_,false,false,false));
    isBWdiv_=divbybw_;
    inputg_.import(c,divbbw);
}
plot::plot(const graph* g){
    if(debug) std::cout << "plot(const graph *)" <<std::endl;
    if(!g || g->isEmpty()){
        throw std::logic_error("plot(const graph *) point NULL or graph empty" );
    }
    TH1::AddDirectory(false);
    statg_ = addObject(g->getTGraph("",true));
    systg_ = addObject(g->getTGraph("",false));
    isBWdiv_=false;
    divbybw_=false;
    inputg_=*g;
}
plot::plot(const plot& rhs):tObjectList(rhs){
    if(debug) std::cout << "plot::plot(const plot& rhs)" <<std::endl;
    if(&rhs==this)
        return;
    clear();
    if(!rhs.statg_) return;
    createFrom(&rhs.inputg_);
    isBWdiv_=rhs.isBWdiv_;
}
plot::~plot(){
    if(debug) std::cout << "~plot" <<std::endl;
    clear();
}

plot & plot::operator = (const plot& rhs){
    if(&rhs==this)
        return *this;
    clear();
    if(!rhs.statg_) return *this;
    createFrom(&rhs.inputg_);
    isBWdiv_=rhs.isBWdiv_;
    return *this;
}
void plot::createFrom(const container1D * c,bool divbbw){
    if(debug) std::cout << "plot::createFrom(const container1D *)" <<std::endl;
    if(!c){
        if(debug) std::cout << "plot::createFrom(const graph *): creating empty from empty (null pointer)" <<std::endl;
        clear();
        return;
    }
    clear();
    divbybw_=divbbw;
    TH1::AddDirectory(false);
    statg_ = addObject(c->getTGraph("",divbybw_,true,false,false));
    systg_ = addObject(c->getTGraph("",divbybw_,false,false,false));
    isBWdiv_=divbybw_;
    inputg_.import(c,divbbw);
}
void plot::createFrom(const graph * g){
    if(debug) std::cout << "plot::createFrom(const graph *)" <<std::endl;
    if(!g){
        if(debug) std::cout << "plot::createFrom(const graph *): creating empty from empty (null pointer)" <<std::endl;
        clear();
        return;
    }
    TH1::AddDirectory(false);
    statg_ = addObject(g->getTGraph("",true));
    systg_ = addObject(g->getTGraph("",false));
    isBWdiv_=false;
    inputg_=*g;
}
void plot::clear(){
    if(debug) std::cout << "plot::clear" <<std::endl;
    tObjectList::cleanMem();statg_=0;systg_=0;
    inputg_=graph();
}

void plot::removeXErrors(){
    TH1::AddDirectory(false);
    cleanMem();
    inputg_.removeXErrors();
    statg_ = addObject(inputg_.getTGraph("",true));
    systg_ = addObject(inputg_.getTGraph("",false));
}

void plot::Draw(const TString& opt)const{
    if(debug) std::cout << "plot::Draw" <<std::endl;
    if(empty()){
        std::cout << "plot::Draw: is empty, doing nothing!" << std::endl;
        return;
    }
    getSystGraph()->Draw(("AP"+opt));
    getStatGraph()->Draw(("P,same"+opt));
}

}//ns
