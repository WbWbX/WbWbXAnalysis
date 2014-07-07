/*
 * parameterExtractor.cc
 *
 *  Created on: Jan 6, 2014
 *      Author: kiesej
 */



#include "../interface/parameterExtractor.h"
#include "TCanvas.h"
#include <iostream>
#include <stdexcept>
#include "TGraphAsymmErrors.h"
#include "TVirtualFitter.h"
#include "TFitResult.h"
#include <cmath>
#include <algorithm>
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"

namespace ztop{

bool parameterExtractor::debug=false;

bool parameterExtractor::checkSizes(){
    bool check=agraphs_.size() == bgraphs_.size();
    if(!check){
        std::cout << "parameterExtractor::checkSizes: Input vectors don't match in size" << std::endl;
    }
    return check;
}
/**
 * includes range check. returns 0 if something is wrong
 */
size_t parameterExtractor::getSize(){
    if(!checkSizes())
        return 0;
    else return agraphs_.size();
}
std::vector<graph> parameterExtractor::createLikelihoods(){
    if(debug)
        std::cout << "parameterExtractor::createLikelihoods" << std::endl;
    fittedgraphsa_.resize(agraphs_.size(),graph());
    fittedgraphsb_.resize(bgraphs_.size(),graph());
    std::vector<graph> out;
    size_t size=getSize();
    for(size_t i=0;i<size;i++){
        out.push_back(createLikelihood(i));
    }
    return out;
}


//////////protected member funcs


graph parameterExtractor::createLikelihood(size_t idx){
    if(debug)
        std::cout << "parameterExtractor::createLikelihood "<< idx << std::endl;

    if(LHMode_==lh_chi2){
        return createChi2Likelihood(agraphs_.at(idx),bgraphs_.at(idx));
    }
    else if(LHMode_==lh_chi2Swapped){
        return createChi2SwappedLikelihood(agraphs_.at(idx),bgraphs_.at(idx));
    }
    else if(LHMode_==lh_fit){
        graph out=createIntersectionLikelihood(agraphs_.at(idx),bgraphs_.at(idx),fittedgraphsa_.at(idx),fittedgraphsb_.at(idx));

        return out;
    }
    else if(LHMode_==lh_fitintersect){
        graph out=createIntersectionLikelihood(agraphs_.at(idx),bgraphs_.at(idx),fittedgraphsa_.at(idx),fittedgraphsb_.at(idx));

        return out;
    }

    /*  other modes  */

    std::cout << "parameterExtractor::createLikelihood: likelihood definition not found" << std::endl;
    throw std::runtime_error("parameterExtractor::createLikelihood: likelihood definition not found");
}
/*
void parameterExtractor::drawFittedFunctions(size_t idx)const{
    if(LHMode_!=lh_fit) return;
    if(idx >= fittedFunctionsa_.size() || idx >= fittedFunctionsb_.size()){
        throw std::out_of_range("parameterExtractor::drawFittedFunctions: out of range");
    }
    for(size_t i=0;i<fittedFunctionsa_.at(idx).size();i++)
        fittedFunctionsa_.at(idx).at(i)->Draw("same");
    for(size_t i=0;i<fittedFunctionsb_.at(idx).size();i++)
        fittedFunctionsb_.at(idx).at(i)->Draw("same");

}
 */

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Helpers //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

void parameterExtractor::checkChi2Conditions(const graph& g,const graph& gcompare){
    if(g.getNPoints()!=gcompare.getNPoints()){
        std::cout << "parameterExtractor::checkChi2Conditions: needs same number of points" << std::endl;
        throw std::logic_error("parameterExtractor::checkChi2Conditions: needs same number of points" );
    }
    //only performing loose x check
    for(size_t i=0;i<g.getNPoints();i++){
        if(g.getPointXContent(i) != gcompare.getPointXContent(i)){
            std::cout << "parameterExtractor::checkChi2Conditions: needs same x coordinates" << std::endl;
            throw std::logic_error("parameterExtractor::checkChi2Conditions: needs same x coordinates" );
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// IMPLEMETATION OF LIKELIHOODS ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////


graph parameterExtractor::createChi2Likelihood(const graph& g,const graph& gcompare){
    checkChi2Conditions(g,gcompare);


    graph out=g;//get number of points
    out.clear();//clear
    //return out;//////////
    size_t nans=0;
    //compare systematics by systematics if present -> later
    //first ignore syst correlations
    //treat everything in g ang gcompare as uncorrelated

    for(size_t p=0;p<g.getNPoints();p++){
        float diff=g.getPointYContent(p)-gcompare.getPointYContent(p);
        float err2=0;

        float lerr=g.getPointYError(p,false);
        float rerr=gcompare.getPointYError(p,false);
        err2=lerr*lerr+rerr*rerr;

        out.setPointXContent(p,g.getPointXContent(p));
        float chi2point=diff*diff/err2;
        if(chi2point!=chi2point){
            chi2point=0;
            nans++;
        }
        out.setPointYContent(p,chi2point);
    }
    if(nans>0){
        std::cout << "parameterExtractor::createChi2Likelihood: nans created. ";
        if(nans == out.getNPoints())
            std::cout << " All chi2=0, keep in mind for ndof "<<std::endl;
        else
            std::cout << " Only some chi2=0, WARNING!!! Result will not be useful!" <<std::endl;
    }

    out.setYAxisName("#chi^{2}");
    return out;

}


graph parameterExtractor::createChi2SwappedLikelihood(const graph& g,const graph& gcompare){
    checkChi2Conditions(g,gcompare);

    graph out=g;//get number of points
    out.clear();//clear
    //return out;//////////
    size_t nans=0;
    //compare systematics by systematics if present -> later
    //first ignore syst correlations
    //treat everything as uncorrelated
    for(size_t p=0;p<g.getNPoints();p++){
        float diff=g.getPointYContent(p)-gcompare.getPointYContent(p);
        float err2=0;

        if(diff>0){ //use left down, right up
            float lerr=g.getPointYErrorDown(p,false);
            float rerr=gcompare.getPointYErrorUp(p,false);
            err2=lerr*lerr+rerr*rerr;
        }
        else{
            float lerr=g.getPointYErrorUp(p,false);
            float rerr=gcompare.getPointYErrorDown(p,false);
            err2=lerr*lerr+rerr*rerr;
        }


        out.setPointXContent(p,g.getPointXContent(p));
        float chi2point=diff*diff/err2;
        if(chi2point!=chi2point){
            chi2point=0;
            nans++;
        }
        out.setPointYContent(p,chi2point);
    }
    if(nans>0){
        std::cout << "graph::getChi2Points: nans created. ";
        if(nans == out.getNPoints())
            std::cout << " All chi2=0, keep in mind for ndof "<<std::endl;
        else
            std::cout << " Only some chi2=0, WARNING!!! Result will not be useful!" <<std::endl;
    }

    out.setYAxisName("#chi^{2}");
    return out;
}


double parameterExtractor::findintersect(TF1* a, TF1* b,float min, float max){//,float& devleft,float& devright) {

    double gran=(1/(double)granularity_);
    double out=-1;
    double tempmin=-1;
    for(double x=min;x<=max;x+=gran){
        double test=fabs(a->EvalPar(&x,0) - b->EvalPar(&x,0));
        //  std::cout << test << std::endl;
        if(tempmin <0 || test<tempmin){
            out=x;
            tempmin=test;
        }
    }
    return out;
}

graph parameterExtractor::createIntersectionLikelihood(const graph& aIn,const graph& bIn,graph& fitteda,graph& fittedb){
    if(debug)
        std::cout << "createIntersectionLikelihood" <<std::endl;
    if(fitfunctiona_=="" || fitfunctionb_=="" ){
        throw std::logic_error("parameterExtractor::createIntersectionLikelihood: needs fit function");
    }
    if(debug)
        std::cout << "createIntersectionLikelihood: for "<< aIn.getName()<<" and " << bIn.getName() <<std::endl;

    graph a=aIn;
    graph b=bIn;

    double meanya=(a.getYMax()+a.getYMin())/2;
    double meanyb=(b.getYMax()+b.getYMin())/2;
    double xmin=std::min(a.getXMin(),b.getXMin());
    double xmax=std::max(a.getXMax(),b.getXMax());
    float xshift = (xmax+xmin)/2;
    float yshift=(meanya+meanyb)/2;
    //preapre for better fitting


    a.shiftAllXCoordinates(-xshift);
    b.shiftAllXCoordinates(-xshift);

    a.shiftAllYCoordinates(-yshift);
    b.shiftAllYCoordinates(-yshift);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////// SHIFTED COORDINATES ///////////////////////////////////////////
    ////////////////////////////////////////////////////// START  //////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //recalculate and extend range a bit - xmin and ymin are now neagtive!
    //mean y should be around 0
    meanya=0;
    meanyb=0;
    xmin=std::min(a.getXMin()*(1.15),b.getXMin()*(1.15));
    xmax=std::max(a.getXMax()*(1.15),b.getXMax()*(1.15));



    TGraphAsymmErrors * agraph=a.getTGraph("a",false);
    TGraphAsymmErrors * bgraph=b.getTGraph("b",false);
    if(!agraph || !bgraph){
        throw std::logic_error("parameterExtractor::createIntersectionLikelihood: needs valid input graphs");

    }



    std::vector<TF1* >  funcsa,funcsb;

    tmpfa_ = new TF1("fita",fitfunctiona_,xmin,xmax);

    tmpfa_->SetLineColor(kBlack);
    funcsa.push_back(tmpfa_);




    tmpfb_ = new TF1("fitb",fitfunctionb_,xmin,xmax);
    tmpfb_->SetLineColor(kRed);
    funcsb.push_back(tmpfb_);


    size_t nxpoints=granularity_+1;

    std::vector<double> confintva(nxpoints);
    std::vector<double> confintvb(nxpoints);
    std::vector<double> xpoints(nxpoints);

    double interval=((xmax-xmin)/((double)(granularity_)));


    //xpoints.at(0)=xmin;

    std::vector<float> scancurvea(nxpoints);
    std::vector<float> scancurveb(nxpoints);

    for(size_t i=0;i<xpoints.size();i++){
        xpoints.at(i)=xmin+ ((double)i)*interval;
    }

    std::cout << "GetConfidenceIntervals" <<std::endl; //DEBUG

    agraph->Fit(tmpfa_,"VR");

    (TVirtualFitter::GetFitter())->GetConfidenceIntervals(nxpoints,1,&xpoints.at(0),&confintva.at(0),clfit_);

    bgraph->Fit(tmpfb_,"VR");

    (TVirtualFitter::GetFitter())->GetConfidenceIntervals(nxpoints,1,&xpoints.at(0),&confintvb.at(0),clfit_);

    graph fita(nxpoints);
    graph fitb(nxpoints);
    graph multiplied(nxpoints);
    //start of new implementation

    //dont get too nummerical, because:
    //int_-inf^inf ( normgaus(x,mu1,s1) * normgaus(x,mu2,s2) ) = normgaus(mu1,mu2,s1+s2)

    /*  float maxconfa=*std::max_element(confintva.begin(),confintva.end());
    float maxconfb=*std::max_element(confintvb.begin(),confintvb.end());
    float integralwidth2=maxconfa*maxconfa+maxconfb*maxconfb;
     */

    //get average stat uncertainty
    float minstata=10000;
    float minstatb=10000;
    if(fituncmodea_ == fitunc_statcorr){
        for(size_t i=0;i<aIn.getNPoints();i++){
            float tmpmin=aIn.getPointYStat(i);///aIn.getPointYContent(i);
            if(minstata > tmpmin){
                minstata=tmpmin;
            }
        }
    }
    if(fituncmodeb_ == fitunc_statcorr){
        for(size_t i=0;i<bIn.getNPoints();i++){
            float tmpmin=bIn.getPointYStat(i);///bIn.getPointYContent(i);
            if(minstatb > tmpmin){
                minstatb=tmpmin;
            }
        }
    }
    // minstatb*=minstatcontentb; //avoid bias towards low values
    // minstata*=minstatcontenta;


    // evaluate for each scan point
    if(LHMode_ == lh_fit){
        for(size_t i=0;i<xpoints.size();i++){
            float aval=tmpfa_->EvalPar(&xpoints.at(i),0);
            float bval=tmpfb_->EvalPar(&xpoints.at(i),0);

            //for drawing
            if(fituncmodea_ == fitunc_statcorr){
                confintva.at(i)=minstata;
            }
            if(fituncmodeb_ == fitunc_statcorr){
                confintvb.at(i)=minstatb;
            }
            float widtha=confintva.at(i);
            float widthb=confintvb.at(i);

            float chi2=chi_square(aval,widtha*widtha+widthb*widthb,bval);//);

            // integral*=-2; //chi2 definition log(L) -> -2*log(L)

            multiplied.setPointContents(i,true,xpoints.at(i),chi2);

        }
    }


    //  float tempmin=-1000;

    std::vector<float> centralpoints;

    for(size_t it=0;it<xpoints.size();it++){
        float aval=tmpfa_->EvalPar(&xpoints.at(it),0);
        float bval=tmpfb_->EvalPar(&xpoints.at(it),0);
        fita.setPointContents(it,true,xpoints.at(it),aval);
        fita.setPointYStat(it,confintva.at(it));
        fitb.setPointContents(it,true,xpoints.at(it),bval);
        fitb.setPointYStat(it,confintvb.at(it));
    }

    //reduce number in representation graphs if granularity is high

    float maxgran=20;

    if(granularity_ < maxgran){
        fitteda=(fita);
        fittedb=(fitb);
    }
    else{
        size_t granscaler=(float)granularity_/maxgran;
        size_t indexer=0,newsize=0;;

        for(size_t oldpoint=0;oldpoint<fita.getNPoints();oldpoint++){
            if(oldpoint == indexer){
                indexer+=granscaler;
                newsize++;
            }
        }
        indexer=0;
        fitteda=graph(newsize);
        fittedb=graph(newsize);
        if(debug)
            std::cout << "parameterExtractor::createIntersectionLikelihood: created graphs to represent confidence interval with less granularity (" <<  newsize<<")" <<std::endl;
        size_t newpointidx=0;
        for(size_t oldpoint=0;oldpoint<fita.getNPoints();oldpoint++){
            if(oldpoint == indexer){

                fitteda.setPointContents(newpointidx,true,fita.getPointXContent(oldpoint),fita.getPointYContent(oldpoint));
                fitteda.setPointYStat(newpointidx,fita.getPointYStat(oldpoint));
                fittedb.setPointContents(newpointidx,true,fitb.getPointXContent(oldpoint),fitb.getPointYContent(oldpoint));
                fittedb.setPointYStat(newpointidx,fitb.getPointYStat(oldpoint));
                indexer+=granscaler;
                newpointidx++;
            }
        }
    }
    /*
    //get mean central value
    float meancentral=(xmax-xmin)/2+xmin;
    if(setleft&&setright&&setcentral){
        meancentral=0;
        for(size_t i=0;i<centralpoints.size();i++){
            meancentral+=centralpoints.at(i);
        }
        meancentral/=(float)centralpoints.size();
    }
    else{ //no intersection found, add extremely large error
        xleft = meancentral - fabs(xmax-xmin)*100;
        xright= meancentral + fabs(xmax-xmin)*100;
    }


    graph output(1); //one point output graph
    output.setPointXContent(0,meancentral);
    output.setPointYContent(0,1);
    output.setPointXStat(0,0);
    output.setPointYStat(0,0);
    output.addErrorGraph("variation_up",output);
    output.addErrorGraph("variation_down",output);
    output.setPointXContent(0,xleft,0);
    output.setPointXContent(0,xright,1);

    if(debug)
        std::cout << "parameterExtractor::createIntersectionLikelihood: intersection (shift "<<xshift <<"): "<< meancentral
        << " right: " << xright << " left: " << xleft << std::endl;;


    //shift back
    output.shiftAllXCoordinates(xshift);
     */
    fitteda.shiftAllXCoordinates(xshift);
    fittedb.shiftAllXCoordinates(xshift);

    fitteda.shiftAllYCoordinates(yshift);
    fittedb.shiftAllYCoordinates(yshift);

    multiplied.shiftAllXCoordinates(xshift);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////// SHIFTED COORDINATES ///////////////////////////////////////////
    //////////////////////////////////////////////////////   END  //////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    return multiplied;




}

double parameterExtractor::lnNormedGaus(const float & centre, const float& widthsquared, const float& evalpoint,const float& extnorm)const{
    if(extnorm!=0){
        return (log(1/extnorm) + (-((long double)(evalpoint-centre)*(long double)(evalpoint-centre))/((long double)(2*widthsquared))));
    }
    else{//calculate on the fly in max precision
        return (log(1/sqrt(2*(3.14159265358979323846)*widthsquared)) + (-((long double)(evalpoint-centre))*((long double)(evalpoint-centre))/((long double)(2*widthsquared))));
    }


}
double parameterExtractor::chi_square(const float & centre, const float& widthsquared, const float& evalpoint) const{
    return (double) (((long double)(evalpoint-centre)*(long double)(evalpoint-centre))/((long double)(widthsquared)));
}




}//namespace
