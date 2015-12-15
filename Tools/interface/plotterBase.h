/*
 * plotterBase.h
 *
 *  Created on: Jan 6, 2014
 *      Author: kiesej
 */

#ifndef PLOTTERBASE_H_
#define PLOTTERBASE_H_

#include "tObjectList.h"
#include "histoStyle.h"
#include "plotStyle.h"
#include "graph.h"
#include "legendStyle.h"
#include "TStyle.h"
#include "TGaxis.h"
#include "corrMatrix.h"

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
    plotterBase():tObjectList(), title_(""),tbndc_(true),lastplotidx_(0),
tmplegp_(0),corrm_(0),yspacemulti_(1.3),pad_(0),drawlegend_(true),preparepad_(true){
    	TGaxis::SetMaxDigits(4);
    	gStyle->SetOptStat(0);
    }
    virtual ~plotterBase(){}

    plotterBase(const plotterBase&);

    void setTitle(TString t){title_=t;}

    void setDrawLegend(bool dl){drawlegend_=dl;}

    void setLastNoLegend(){nolegendidx_.push_back(lastplotidx_);}

    void usePad(TVirtualPad* pad){pad_=pad;
   // gStyle->SetHatchesLineWidth(2);
    //gStyle->SetHatchesSpacing(1);}
    }
    TVirtualPad* getPad()const; //throws if 0
    bool padAssociated()const{return pad_;}
    /**
     * Set if pad will be cleared and prepared by plotter (default true)
     */
    void setPreparePad(bool set){preparepad_=set;}

    TLegend* getLegend()const {return tmplegp_;}

     void draw();
     void cleanMem(){tObjectList::cleanMem();}

     size_t size(){return 0;} //to be overwritten by inheriting classes

     static bool debug;
     void addTextBox(float x,float y,const TString & text,float textsize=0.05,int align=11,int color=1);
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

     void readStyleFromFile(const std::string&file){
    	 readStylePriv(file,true);
     }
     void addStyleFromFile(const std::string&file){
    	 readStylePriv(file,false);
     }

     void readTStyleFromFile(const std::string& pathtofile);

     void printToPdf(const std::string& outname);
void printToPng(const std::string& outname);
     void saveToTFile(const TString& outname);

     /**
      * Try to avoid this function whereever possible! This is NOT the recommended way
      * to use these plotters and should be banished anyway!
      * Only in here for compatibility reasons with other frameworks
      */
     void saveAsCanvasC(const std::string& outname);

     virtual bool hasRatio()const=0;

     void associateCorrelationMatrix(const corrMatrix& m){corrm_=&m;}


     virtual float getYSpaceMulti(bool logarithmic,bool divbybw)const=0;


     virtual plotStyle& getUpperStyle() =0;
     virtual plotStyle& getRatioStyle() =0;
     virtual const plotStyle& getUpperStyle()const=0;
     virtual const plotStyle& getRatioStyle()const=0;


     virtual float getXAxisLowerLimit()const=0;
    virtual float getXAxisHigherLimit()const=0;

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

    /**
     * eats some performance due to roots ridiculous transform system
     * gets NDC of parent (full) pad
     */
    void convertUserCoordsToNDC(TVirtualPad *subpad, const float & ux, const float & uy, float& ndcx, float& ndcy)const;


    legendStyle legstyle_;
    bool tbndc_; //used for drawing text boxes if defined

    TStyle intstyle_;
    std::vector<size_t> nolegendidx_;
    size_t lastplotidx_;
    virtual void readStylePriv(const std::string & , bool )=0;

    TLegend * tmplegp_;
    const corrMatrix* corrm_;

    float yspacemulti_;

private:
    TVirtualPad * pad_;
    bool drawlegend_;
    bool preparepad_;
//    int errToRootFillStyle(const histoStyle *s) const;

};
}//ns



#endif /* PLOTTERBASE_H_ */
