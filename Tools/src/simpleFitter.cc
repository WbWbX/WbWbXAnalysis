/*
 * simpleFitter.cc
 *
 *  Created on: May 22, 2014
 *      Author: kiesej
 */

#include "../interface/simpleFitter.h"
#include <TROOT.h>
#include <TMinuit.h>
#include "Math/Minimizer.h"
#include <iostream>
#include "Math/Functor.h"
#include "Math/Factory.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"

#include <stdexcept>



//use TMinuit
//define all the functions - nee dto be c-stylish



namespace ztop{

simpleFitter * currentfitter=0;

namespace chi2defs{
void chisq(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag){
    if(!currentfitter)
        return;

    //calculate chisquare
    double chisq = 0;
    for (size_t i=0;i<currentfitter->getNomPoints()->size(); i++) {
        // chi square is the quadratic sum of the distance from the point to the function weighted by its error
        double delta  = (currentfitter->getNomPoints()->at(i).y
                -currentfitter->fitfunction(currentfitter->getNomPoints()->at(i).x,par));
        //   std::cout << currentfitter->getErrsUp()->at(i).y <<std::endl;
        double err=1;
        if(delta>0)
            err=currentfitter->getErrsUp()->at(i).y;
        else
            err=currentfitter->getErrsDown()->at(i).y;

        if(err!=0)
            delta/=err;
        else
            delta*=1e6;
        chisq += delta*delta;
    }

    f = chisq;
    return;
}
}

int simpleFitter::printlevel=0;

simpleFitter::simpleFitter():fitmode_(fm_pol0),maxcalls_(50000),requirefitfunction_(true){

    setFitMode(fitmode_);

    std::cout << "simpleFitter::simpleFitter: WARNING! This fitter is still in test mode! " <<std::endl;

}

simpleFitter::~simpleFitter(){}

void simpleFitter::setFitMode(fitmodes fitmode){
    fitmode_=fitmode;
    paras_.resize(fitmode_+1,0);
    stepsizes_.resize(fitmode_+1,0.001);
    paraerrsup_.resize(fitmode_+1,0);
    paraerrsdown_.resize(fitmode_+1,0);
    paracorrs_.resize(fitmode_+1,std::vector<double>(fitmode_+1));

}

void simpleFitter::setParameters(const std::vector<double>& inpars,const std::vector<double>& steps){
    paras_=inpars;stepsizes_=steps;
    paraerrsup_.resize(paras_.size(),0);
    paraerrsdown_.resize(paras_.size(),0);
    paracorrs_.resize(paras_.size(),std::vector<double>(paras_.size()));
}

float simpleFitter::getFitOutput(const float& xin)const{

    return (float)fitfunction(xin,&(paras_.at(0)));


}


void simpleFitter::fit(){
    fit(chi2defs::chisq);
}
void simpleFitter::fit(void (*chi2function)(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)){

    if(!checkSizes()){

        /*
         *  if(paras_.size() != stepsizes_.size())
        return false;
    if(paras_.size() != paraerrsup_.size())
        return false;
    if(paras_.size() != paraerrsdown_.size())
        return false;

    if(requirefitfunction_){
        if(paras_.size() != (size_t)fitmode_+1)
            return false;
        if(nompoints_.size()<(size_t)fitmode_+1)
            return false;
        if(nompoints_.size() != errsup_.size())
            return false;
        if(nompoints_.size() != errsdown_.size())
            return false;
        if(nompoints_.size() != errsup_.size())
            return false;
         */

        std::cout << "EROR IN simpleFitter::fit" <<std::endl;

        std::cout << "parameter:             " << paras_.size() << std::endl;
        std::cout << "parameter errors up:   " << paraerrsup_.size() << std::endl;
        std::cout << "parameter errors down: " << paraerrsdown_.size() << std::endl;
        std::cout << "parameter steps size:  " << stepsizes_.size() << std::endl;
        std::cout << "\nOnly if important when fitting to points ("<< requirefitfunction_ <<"): "  << std::endl;
        std::cout << "points:            " << nompoints_.size() << std::endl;
        std::cout << "point errors up:   " << errsup_.size() << std::endl;
        std::cout << "point errors down: " << errsdown_.size() << std::endl;



        throw std::runtime_error("simpleFitter::fit: number of parameters<->stepsizes or parameters<->fitfunction ");
        //  return;
    }

    //this kills parallelization
    currentfitter=this;


    TMinuit *ptMinuit = new TMinuit((Int_t)paras_.size());
    ptMinuit->SetPrintLevel(printlevel); //be quiet


    //  if(!chi2function)
    ptMinuit->SetFCN(chi2function);

    Int_t ierflg = 0;
    std::vector<Double_t> arglist;
    arglist.resize(paras_.size()+1);

    arglist[0] = 1;
    ptMinuit->mnexcm("SET ERR", &(arglist.at(0)) ,1,ierflg);


    for(size_t i=0;i<paras_.size();i++){
        TString paraname="";
        if(paranames_.size()>i)
            paraname=paranames_.at(i);
        else
            paraname=(TString)"par"+toTString(i);


        ptMinuit->mnparm(i, paraname.Data(),
                paras_.at(i), stepsizes_.at(i), 0,0,ierflg);

    }


    arglist.at(0)=1;
    ptMinuit->mnexcm("SET ERR", &(arglist.at(0)) ,1,ierflg);

    arglist.at(0)=1;
    ptMinuit->mnexcm("SET STR", &(arglist.at(0)), 1, ierflg);
/*
    arglist[0] = maxcalls_;
    arglist[1] = 0.01;
    ptMinuit->mnexcm("MIGRAD", &(arglist.at(0)) ,2,ierflg);
*/

    //  arglist[0] = 500;
    //    arglist[1] = 1.;
 /*

    arglist[0] = maxcalls_;
    arglist[1] = 1.;
    ptMinuit->mnexcm("MIGRAD", &(arglist.at(0)) ,2,ierflg);


    //ptMinuit->Migrad();
    //  [maxcalls]  [parno] [parno]

*/

    arglist[0]=maxcalls_;
    for(size_t i=0;i<minospars_.size();i++)
        arglist.at(i+1)=minospars_.at(i)+1;

    ptMinuit->mnexcm("MINOS", &(arglist.at(0)) ,minospars_.size()+1,ierflg);


    // std::cout << "\nPrint results from minuit\n";tesla
    //fill results from minuit
    for(size_t i=0;i<paras_.size();i++){
        double eparb=0,gcc=0,dummyerr=0;

        ptMinuit->GetParameter(i, paras_.at(i),dummyerr);
        ptMinuit->mnerrs(i, paraerrsup_.at(i), paraerrsdown_.at(i),eparb,gcc);// Double_t &eparab, Double_t &gcc

        TString paraname="";
        if(paranames_.size()>i)
            paraname=paranames_.at(i);
        else
            paraname=(TString)"par"+toTString(i);

        if(printlevel>=0)
        std::cout << paraname+" " << paras_.at(i) << "\n + " << paraerrsup_.at(i)
                                                        <<" -  " << paraerrsdown_.at(i)<< "\n symm: " << dummyerr << "\n";
    }
    std::cout << std::endl;


    Double_t amin,edm,errdef;
    Int_t nvpar,nparx,icstat;
    ptMinuit->mnstat(amin,edm,errdef,nvpar,nparx,icstat);

    delete ptMinuit;

}

size_t simpleFitter::findParameterIdx(const TString& paraname)const{
    size_t idx=std::find(paranames_.begin(), paranames_.end(),paraname) - paranames_.begin();
    if(idx<paranames_.size()) return idx;
    throw std::runtime_error("simpleFitter::findParameterIdx: name not found");
}

//FixParameter(Int_t parNo)
/*
 * all function definitions are placed here
 */
Double_t simpleFitter::fitfunction(float x,const double *par)const{
    //needs some safety
    if(fitmode_==fm_pol0){
        ++x;
        double value=par[0];
        return value;
    }
    if(fitmode_==fm_pol1){

        double value=par[0] + x* par[1];
        return value;
    }
    if(fitmode_==fm_pol2){

        double value=par[0] + x* par[1] + x*x*par[2];
        return value;
    }

    return 0;
}

void simpleFitter::clearPoints(){
    nompoints_.clear();
    errsup_.clear();
    errsdown_.clear();

}

bool simpleFitter::checkSizes()const{


    if(paras_.size() != stepsizes_.size())
        return false;
    if(paras_.size() != paraerrsup_.size())
        return false;
    if(paras_.size() != paraerrsdown_.size())
        return false;

    if(requirefitfunction_){
        if(paras_.size() != (size_t)fitmode_+1)
            return false;
        if(nompoints_.size()<(size_t)fitmode_+1)
            return false;
        if(nompoints_.size() != errsup_.size())
            return false;
        if(nompoints_.size() != errsdown_.size())
            return false;
        if(nompoints_.size() != errsup_.size())
            return false;

    }


    return true;
}


}


