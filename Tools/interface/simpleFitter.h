/*
 * simpleFitter.h
 *
 *  Created on: May 22, 2014
 *      Author: kiesej
 */

#ifndef SIMPLEFITTER_H_
#define SIMPLEFITTER_H_

#include <TROOT.h>
#include <vector>


namespace ztop{

/**
 * small 2d point helper
 *
 * */
class point2D{
public:
    point2D(): x(0),y(0){}
    point2D(const float xin, const float yin): x(xin),y(yin){}
    float x,y;
};

/**
 * only for functionality
 * no interface to ztop data formats!
 *
 * just keep it simple at first
 *
 * minimizes chi2
 *
 *
 * DOES NOT ALLOW FOR PARALLALIZATION!!!! PROBLEM WITH TMINUIT
 *
 */
class simpleFitter{
public:

    enum fitmodes{fm_pol0,fm_pol1,fm_pol2,fm_pol3};
    enum printlevels{pl_silent,pl_normal,pl_verb}; //TBI

    simpleFitter();
    ~simpleFitter();
    /**
     * Available fit modes:
     * <some docu>
     */
    void setFitMode(fitmodes fitmode);

    void addPoint(const float & px, const float & py){nompoints_.push_back(point2D(px,py));}
    void setPoints(const std::vector<point2D> inp){nompoints_=inp;}

    void addYError(const float & err){errsup_.push_back(point2D(0,err));errsdown_.push_back(point2D(0,err));}
    void setErrorsUp(const std::vector<point2D> inp){errsup_=inp;}
    void setErrorsDown(const std::vector<point2D> inp){errsdown_=inp;}

    /**
     * these are then considered as start values
     * if not defined, will assume 0 for all parameters
     * and step sizes of 0.01 for all parameters
     */
    void setParameters(const std::vector<double>& inpars,const std::vector<double>& steps);
    void setParameterNames(const std::vector<TString>& nms){paranames_=nms;}

    void setRequireFitFunction(bool req){requirefitfunction_=req;}

    void setMaxCalls(size_t calls){maxcalls_=calls;}

    void addMinosParameter(size_t parnumber){minospars_.push_back(parnumber);}

    float getFitOutput(const float& xin)const;

    static int printlevel;

    void fit(void (*chi2function)(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag) );
    void fit();

    const std::vector<point2D> * getNomPoints()const{return &nompoints_;}
    const std::vector<point2D> * getErrsUp()const{return &errsup_;}
    const std::vector<point2D> * getErrsDown()const{return &errsdown_;}
    const std::vector<double> *getParameters()const{return &paras_;}
    const std::vector<double> *getParameterErrUp()const{return &paraerrsup_;}
    const std::vector<double> *getParameterErrDown()const{return &paraerrsdown_;}

    size_t findParameterIdx(const TString& paraname)const;

    Double_t fitfunction(float x,const Double_t *par)const;

    /**
     * clears all input points
     */
    void clearPoints();

private: //set some to protected if inheritance is needed
    fitmodes fitmode_;


    std::vector<point2D> nompoints_;
    std::vector<point2D> errsup_;
    std::vector<point2D> errsdown_;

    std::vector<double> paras_;
    std::vector<double> stepsizes_;
    std::vector<double> paraerrsup_,paraerrsdown_;
    std::vector<TString> paranames_;

    std::vector< std::vector<double> > paracorrs_;

    std::vector<int> minospars_;

    size_t maxcalls_;

    //definitely privateL

    //1D section
    //this is moved to global due to tminuit reasons
    // void chisq(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);


    bool checkSizes()const;

    bool requirefitfunction_;
    //void * chisqf_;

};

}


#endif /* SIMPLEFITTER_H_ */
