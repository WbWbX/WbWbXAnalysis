/*
 * graph.h
 *
 *  Created on: Nov 14, 2013
 *      Author: kiesej
 */

#ifndef GRAPH_H_
#define GRAPH_H_

#include "histoContent.h"
#include "containerStyle.h"
#include "taggedObject.h"
#include "TString.h"
#include <vector>

class TGraphAsymmErrors;
class TH1;
class TTree;
class TFile;

namespace ztop{
class container1D;

/**
 * Class for handling x-y points with n errors (each including optional statistics)
 * Be careful! X points contain Y variations and Y points contain X variations!
 *
 */
class graph : public taggedObject{
    friend class container1D;
public:
    graph(const TString &name="");
    graph(const size_t & npoints,const TString &name="");
    ~graph();

    bool isEmpty()const{return getNPoints()<1;}



    void setXAxisName(const TString& name){xname_=name;}
    const TString& getXAxisName()const {return xname_;}

    void setYAxisName(const TString& name){yname_=name;}
    const TString& getYAxisName()const {return yname_;}

    void setNPoints(const size_t & npoints); //for high perf deletes all content
    size_t getNPoints()const{return xcoords_.size();}

    size_t addPoint(const float& x, const float & y,const TString & pointname="");//returns point index
    size_t  addPoint(const float& x, const float & y, const float & ystaterr,const TString & pointname="");//returns point index

    void setPointContents(const size_t & point, bool nomstat, const float & xcont, const float & ycont, const int & syslayer=-1);
    void setPointXContent(const size_t & point, const float & xcont, const int & syslayer=-1);
    void setPointYContent(const size_t & point, const float & xcont, const int & syslayer=-1);
    void setPointXStat(const size_t & point, const float & xcont, const int & syslayer=-1);
    void setPointYStat(const size_t & point, const float & xcont, const int & syslayer=-1);
    void setPointName(const size_t & point, const TString & pointname);

    const float &  getPointXContent(const size_t & point, const int & syslayer=-1) const;
    const float &  getPointYContent(const size_t & point, const int & syslayer=-1) const;
    float   getPointXStat(const size_t & point, const int & syslayer=-1) const;
    float   getPointYStat(const size_t & point, const int & syslayer=-1) const;
    const TString & getPointName(const size_t & point)const;



    float getPointError(const size_t & point, bool yvar, bool updown, bool onlystat,const TString &limittosys="")const;
    //wrappers
    float getPointXErrorUp(const size_t & point, bool onlystat,const TString &limittosys="")const;
    float getPointXErrorDown(const size_t & point, bool onlystat,const TString &limittosys="")const;
    float getPointYErrorUp(const size_t & point, bool onlystat,const TString &limittosys="")const;
    float getPointYErrorDown(const size_t & point, bool onlystat,const TString &limittosys="")const;

    float getPointXError(const size_t & point, bool onlystat,const TString &limittosys="")const;
    float getPointYError(const size_t & point, bool onlystat,const TString &limittosys="")const;


    float  getXMax(size_t & point,bool includeerror=false) const;
    float  getXMin(size_t & point,bool includeerror=false) const;
    float  getYMax(size_t & point,bool includeerror=false) const;
    float  getYMin(size_t & point,bool includeerror=false) const;
    float  getXMax(bool includeerror=false) const;
    float  getXMin(bool includeerror=false) const;
    float  getYMax(bool includeerror=false) const;
    float  getYMin(bool includeerror=false) const;

    histoContent & getYCoords(){return ycoords_;}
    histoContent & getXCoords(){return xcoords_;}
    const histoContent & getYCoords()const{return ycoords_;}
    const histoContent & getXCoords()const{return xcoords_;}

    /**
     * return index of new layer
     * needs same indices for points
     * adds deviation of NOMINALS
     */
    size_t addErrorGraph(const TString &name,const graph &); //needs to have same point indices... if not check by closest point?
    graph getSystGraph(const size_t sysidx)const;
    graph getNominalGraph()const;
    graph getRelYErrorsGraph()const;

    const TString & getSystErrorName(const size_t & number) const;
    const size_t & getSystErrorIndex(const TString & ) const;
    size_t getSystSize() const{return ycoords_.layerSize();}

    void removeYErrors(); //comes handy
    void removeXErrors(); //comes handy

    void sortPointsByX();

    /*
     * operators... define carefully based on histocontent. Not too much overlap with container1D
     */

    graph addY(const graph &) const;//if any x errors differ, let it fail
    void clear();

    void normalizeToGraph(const graph &);


    /*
     * import/export + root interface
     */

    void import(const container1D*,bool dividebybinwidth=true); //uses bin centers, adds x errors binwidth_up/down

    TGraphAsymmErrors * getTGraph(TString name="",bool onlystat=false) const;

    TH1 * getAxisTH1(bool tighty=false,bool tightx=true)const;

    textBoxes getTextBoxesFromPoints()const;

    /*
     *
     * high level functions for fitting
     */
    enum chi2definitions{symmetrize,swap,parameter};
    chi2definitions chi2definition;

    graph getChi2Points(const graph&)const;

    void shiftAllXCoordinates(const float& value);
    void shiftAllYCoordinates(const float& value);


    static std::vector<ztop::graph *> g_list;
    static bool g_makelist;
    static bool debug;


    ///IO
    void loadFromTree(TTree *, const TString & plotname);
    void loadFromTFile(TFile *, const TString & plotname);
    void loadFromTFile(const TString& filename, const TString & plotname);

    void writeToTree(TTree *);
    void writeToTFile(TFile *);
    void writeToTFile(const TString& filename);

private:



    /*
     * point names are always stored in nominal of x coordinate!
     */
    histoContent xcoords_,ycoords_;
    float labelmultiplier_;
    TString yname_,xname_;

    TString stripVariation(const TString&) const;
    float getDominantVariation( TString ,const size_t &,bool,bool ) const;
    void addToList();

    /*
     * for all these functions be careful: due to use of histobins, x errors are represented by the y points and vice versa
     * makes interface to container1D easier but is not intuitive
     */
    histoBin & getPointX(const size_t & pointidx,const int & layer=-1); //histoc getbin
    const histoBin & getPointX(const size_t & pointidx,const int & layer=-1) const; //histoc getbin

    histoBin & getPointY(const size_t & pointidx,const int & layer=-1); //histoc getbin
    const histoBin & getPointY(const size_t & pointidx,const int & layer=-1) const; //histoc getbin


};
}//namespace
#endif /* GRAPH_H_ */
