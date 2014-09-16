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

class containerStack;

/**
 * plotter for control plots. stack+ratio plot
 * here also canvas options, text boxes, legends are being defined
 */
class plotterControlPlot: public plotterBase{
public:

    plotterControlPlot(): plotterBase(), divideat_(0), stackp_(0),tempdataentry_(0),invertplots_(false),psmigthresh_(0){}
    ~plotterControlPlot(){plotterControlPlot::cleanMem();}

    void addStyleFromFile(const std::string& );
    void readStyleFromFile(const std::string& );
    /*
     * expects entries:
     * [plotterControlPlot] defines divideat, threshold for showing PS migrations
     * [textBoxes - boxes]
     * [containerStyle - DataUpper]
     * [containerStyle - MCUpper]
     * [containerStyle - DataRatio]
     * [containerStyle - MCRatio]
     * [plotStyle - Upper]
     * [plotStyle - Ratio]
     */


    void setStack(const containerStack *s){stackp_=s;}

    void clear();
    void clearPlots();
    void cleanMem();

protected:
    void preparePad();
    void drawPlots();
    // void drawTextBoxes();
    void drawLegends();
    void refreshPad();


private:
    plotStyle upperstyle_;
    plotStyle ratiostyle_;
    containerStyle datastyleupper_;
    containerStyle datastyleratio_;
    containerStyle mcstyleupper_;
    containerStyle mcstylepsmig_;
    containerStyle mcstyleratio_;



    float divideat_;

    const containerStack * stackp_;
//taken care of by addObject
  //  TLegend * tmplegp_;
    size_t tempdataentry_;
//must be taken care of
    std::vector<plot *> tempplots_;

    bool invertplots_;

    float psmigthresh_;

    void drawControlPlot();
    void drawRatioPlot();

    void readStylePriv(const std::string & file, bool requireall);

};


}


#endif /* PLOTTERCONTROLPLOT_H_ */
