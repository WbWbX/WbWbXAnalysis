/*
 * plotterCompare.h
 *
 *  Created on: Jan 14, 2014
 *      Author: kiesej
 */

#ifndef PLOTTERCOMPARE_H_
#define PLOTTERCOMPARE_H_

#include "plotterBase.h"
#include "containerStyle.h"
#include "plotStyle.h"
#include "container.h"
#include "plot.h"
#include <vector>

namespace ztop{

/**
 * workflow:
 * 1. read in style
 * 2. setNominal and addComparePlots
 *
 *
 * A usePad()
 * B draw()
 * (safe somehow)
 * cleanMem / set new
 */
class plotterCompare : public plotterBase{
public:
    plotterCompare(): plotterBase(),
    divideat_(0.5),memnom_(0),memratio_(0),size_(0){}
    ~plotterCompare(){cleanMem();}

    void setNominalPlot(const container1D *c,bool divbw=true);
    void setComparePlot(const container1D *c,size_t idx,bool divbw=true);
    void setNominalPlot(const graph *c);
    void setComparePlot(const graph *c,size_t idx);


    //set ids before reading style file!
    /**
     * If ids are not set they are assumed to be NominalUpper, NominalRatio
     * and CompareUpper<N> N=0,1,2... (same for ratio)
     * else Ids are added e.g. NominalUpper<idnominal>, CompareUpper<idcompare>
     */
    std::string & nominalId(){return nominalid_;}
    const std::string & nominalId()const{return nominalid_;}
    std::vector<std::string> & compareIds(){return compids_;}
    const std::vector<std::string> & compareIds()const{return compids_;}

    //void setComparePlotStyles();
    //void setNominalPlotStyle();
    void readStyleFromFile(const std::string&);
    void addStyleFromFile(const std::string&);
    /*
     * expects entries:
     * [plotterCompareStyle] defines size N>0
     * [containerStyle - NominalUpper]
     * N x [containerStyle - CompareUpper<N-1>]
     * [containerStyle - NominalRatio]
     * N x [containerStyle - CompareRatio<N-1>]
     * [plotStyle - Upper]
     * [plotStyle - Ratio]
     */

    void clear();
    void clearPlots();
    void cleanMem();
    /**
     * returns numer of total plots including nominal
     */
    size_t size(){return compareplots_.size()+1;}

protected:
    void preparePad();
    void drawPlots();
    // void drawTextBoxes();
    void drawLegends();



private:
    plotStyle upperstyle_;
    plotStyle ratiostyle_;
    containerStyle nomstyleupper_;
    containerStyle nomstyleratio_;
    std::vector<containerStyle> compstylesupper_;
    std::vector<containerStyle> compstylesratio_;

    std::string nominalid_;
    std::vector<std::string> compids_;


    float divideat_;

    plot nominalplot_, * memnom_;
    std::vector<plot> compareplots_, *memratio_;
    size_t size_;


    void drawAllPlots(const plotStyle* ps, const containerStyle * cs, const std::vector<containerStyle>* vcs,
            const plot * nompl, const std::vector<plot>* vcpl, bool nomlast);

    plot plotterCompareStyle(const plot&);
    void makeRatioPlots(); //sets mem*

    float getMaximumUpper();

    void readStylePriv(const std::string & file, bool requireall);

};

}



#endif /* PLOTTERCOMPARE_H_ */
