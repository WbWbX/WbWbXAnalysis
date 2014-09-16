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
#include "TStyle.h"
//#include "TGaxis.h"

class TH1D;
class TGraphAsymmErrors;
class TCanvas;
class TVirtualPad;
class TLegend;

namespace ztop{
/**
 * applies styles to TGraphs,TH1Ds, (TCanvas etc)
 */
class plotterBase :protected tObjectList{
public:
    plotterBase():tObjectList(), title_(""),tbndc_(true),lastplotidx_(0),tmplegp_(0),pad_(0),drawlegend_(true){
    	//TGaxis::SetMaxDigits();
    }
    virtual ~plotterBase(){}

    plotterBase(const plotterBase&);

    void setTitle(TString t){title_=t;}

    void setDrawLegend(bool dl){drawlegend_=dl;}

    void setLastNoLegend(){nolegendidx_.push_back(lastplotidx_);}

    void usePad(TVirtualPad* pad){pad_=pad;}
    TVirtualPad* getPad()const; //throws if 0
    bool padAssociated()const{return pad_;}

    TLegend* getLegend()const {return tmplegp_;}

     void draw();
     void cleanMem(){tObjectList::cleanMem();}

     size_t size(){return 0;} //to be overwritten by inheriting classes

     static bool debug;
     void addTextBox(float x,float y,const TString & text,float textsize=0.05);
     void setTextBoxes(textBoxes tb){textboxes_=tb;}
     void setTextBoxNDC(bool p){tbndc_=p;}
     void removeTextBoxes(){textboxes_.clear();}
     /**
      * adds additional text boxes from file
      * (e.g. Lumi, "CMS Preliminary" ...
      */
     void readTextBoxes(const std::string& pathtofile,const std::string& marker);
     void readTextBoxesInCMSSW(const std::string& pathtofile,const std::string& marker);

     void readStyleFromFileInCMSSW(const std::string&);

     virtual void readStyleFromFile(const std::string&) =0;
     virtual void addStyleFromFile(const std::string&) =0;

     void readTStyleFromFile(const std::string& pathtofile);

     void printToPdf(const std::string& outname);

protected:

    typedef TGraphAsymmErrors TG;
    TString title_;
    textBoxes textboxes_;


    //should be used in that part order
    virtual void preparePad() =0;//adjusts canvas that lives outside
    virtual void drawPlots()=0; //object handling by plot class
    void drawTextBoxes(); //adds new objects to list
    virtual void drawLegends()=0;//adds new objects to list
    virtual void refreshPad()=0;

    float getSubPadYScale(int padidx);
    float getSubPadXScale(int padidx);


    legendStyle legstyle_;
    bool tbndc_; //used for drawing text boxes if defined

    TStyle intstyle_;
    std::vector<size_t> nolegendidx_;
    size_t lastplotidx_;
    virtual void readStylePriv(const std::string & , bool )=0;

    TLegend * tmplegp_;

private:
    TVirtualPad * pad_;
    bool drawlegend_;
//    int errToRootFillStyle(const containerStyle *s) const;

};
}//ns



#endif /* PLOTTERBASE_H_ */
