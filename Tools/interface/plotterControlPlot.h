/*
 * plotterControlPlot.h
 *
 *  Created on: Jan 7, 2014
 *      Author: kiesej
 */

#ifndef PLOTTERCONTROLPLOT_H_
#define PLOTTERCONTROLPLOT_H_

#include "plotterBase.h"

class TLegend;
namespace ztop{
class plot;

class histoStack;

/**
 * plotter for control plots. stack+ratio plot
 * here also canvas options, text boxes, legends are being defined
 */
class plotterControlPlot: public plotterBase{
public:

    plotterControlPlot();
    ~plotterControlPlot(){plotterControlPlot::cleanMem();}

  //  void addStyleFromFile(const std::string& );

    void addStyleFromFile(const std::string&,const std::string&,const std::string&    );
  //  void readStyleFromFile(const std::string& );
    /*
     * expects entries:
     * [plotterControlPlot] defines divideat, threshold for showing PS migrations
     * [textBoxes - boxes]
     * [histoStyle - DataUpper]
     * [histoStyle - MCUpper]
     * [histoStyle - DataRatio]
     * [histoStyle - MCRatio]
     * [plotStyle - Upper]
     * [plotStyle - Ratio]
     */


    void setStack(const histoStack *s){stackp_=s;}

    void clear();
    void clearPlots();
    void cleanMem();

    plotStyle& getUpperStyle(){return upperstyle_;}
    const plotStyle& getUpperStyle()const{return upperstyle_;}
    plotStyle& getRatioStyle(){return ratiostyle_;}
    const plotStyle& getRatioStyle()const{return ratiostyle_;}

    float getXAxisLowerLimit()const;
    float getXAxisHigherLimit()const;

    bool hasRatio()const{return true;}

protected:
    void preparePad();
    void drawPlots();
    // void drawTextBoxes();
    void drawLegends();
    void refreshPad();


private:
    plotterControlPlot& operator=(const plotterControlPlot&){return *this;}
    plotterControlPlot(const plotterControlPlot&){}

    plotStyle upperstyle_;
    plotStyle ratiostyle_;
    histoStyle datastyleupper_;
    histoStyle datastyleratio_;
    histoStyle mcstyleupper_;
    histoStyle mcstylepsmig_;
    histoStyle mcstyleratio_;



    float divideat_;

    const histoStack * stackp_;
//taken care of by addObject
  //  TLegend * tmplegp_;
  //  size_t tempdataentry_;
//must be taken care of
    std::vector<plot *> tempplots_;

    bool invertplots_;

    float psmigthresh_;


    void drawControlPlot();
    void drawRatioPlot();

    void readStylePriv(const std::string & file, bool requireall);

    class legendmerge{
    public:
    	TString mergedname;
    	int mergedcolor;
    	std::vector<TString> tobemerged;
    };

    std::vector<legendmerge> mergelegends;

};


}


#endif /* PLOTTERCONTROLPLOT_H_ */
