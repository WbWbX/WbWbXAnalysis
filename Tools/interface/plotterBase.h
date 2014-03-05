/*
 * plotterBase.h
 *
 *  Created on: Jan 6, 2014
 *      Author: kiesej
 */

#ifndef PLOTTERBASE_H_
#define PLOTTERBASE_H_

#include "tObjectList.h"
#include "containerStyle.h"
#include "plotStyle.h"
#include "graph.h"
#include "legendStyle.h"

class TH1D;
class TGraphAsymmErrors;
class TCanvas;
class TVirtualPad;

namespace ztop{
/**
 * applies styles to TGraphs,TH1Ds, (TCanvas etc)
 */
class plotterBase :protected tObjectList{
public:
    plotterBase():tObjectList(), title_(""),tbndc_(true),pad_(0){}
    virtual ~plotterBase(){}

    plotterBase(const plotterBase&);

    void setTitle(TString t){title_=t;}

    void usePad(TVirtualPad* pad){pad_=pad;}
    TVirtualPad* getPad()const; //throws if 0

     void draw();
     void cleanMem(){tObjectList::cleanMem();}

     size_t size(){return 0;} //to be overwritten by inheriting classes

     static bool debug;
     void setTextBoxes(textBoxes tb){textboxes_=tb;}
     void setTextBoxNDC(bool p){tbndc_=p;}

protected:

    typedef TGraphAsymmErrors TG;
    TString title_;
    textBoxes textboxes_;

    //to apply styles to the objects created/from input
/*use as subfunctions eg to apply all the styles do:
 *  - get container style 1 from plot style, apply to histo
 *  - get ...
 *  - apply rest to pad
 */

    //containerStyle readCntainerStyleFromFile(const std::string& filename,const std::string& stylename);
    //plotStyle      readPlotStyleFromFile(const std::string& filename,const std::string& stylename);
/*
    void applyStyle(TH1D *,const containerStyle*)const;
    void applyAxisStyle(TH1D *,const plotStyle*)const;
    void applyStyle(TGraphAsymmErrors *,const containerStyle*)const;
    void applyAxisStyle(TGraphAsymmErrors *,const plotStyle*)const;
    void applyStyle(TVirtualPad *,const plotStyle*)const;

    // get rid of canvas style, do derived classes instead
    //void applyStyle(TCanvas *, canvasStyle *);


    TString errToRootDrawOpt(const containerStyle *s) const;
*/
    //should be used in that part order
    virtual void preparePad() =0;//adjusts canvas that lives outside
    virtual void drawPlots()=0; //object handling by plot class
    void drawTextBoxes(); //adds new objects to list
    virtual void drawLegends()=0;//adds new objects to list

    float getSubPadYScale(int padidx);
    float getSubPadXScale(int padidx);


    legendStyle legstyle_;
    bool tbndc_; //used for drawing text boxes if defined

private:
    TVirtualPad * pad_;

//    int errToRootFillStyle(const containerStyle *s) const;

};
}//ns



#endif /* PLOTTERBASE_H_ */
