/*
 * plotterMultiplePlots.h
 *
 *  Created on: Feb 25, 2014
 *      Author: kiesej
 */

#ifndef PLOTTERMULTIPLEPLOTS_H_
#define PLOTTERMULTIPLEPLOTS_H_

#include "plotterBase.h"
#include "plot.h"

namespace ztop{


/**
 * Should also be used for one single plot up to N plots in the same canvas
 * does not compare the plots and does not draw a ratio
 * In case you want this, use plotterCompare
 */
class plotterMultiplePlots : public plotterBase{
public:
    plotterMultiplePlots():plotterBase(),drawlegend_(true),tightyaxis_(false),tightxaxis_(true){}

    void addPlot(const graph *c);
    void addPlot(const container1D *c,bool divbw=true);

    void removePlot(size_t idx);
    /**
     * returns number of plots added
     */
    size_t size()const{return plots_.size();}

    /**
     * style format:
     * expects entries:
     * [containerStyle - Default]
     * [containerStyle - <N=0,1,...>] <- only differences wrt to default
     * [plotStyle - MultiPlots]
     * TBI:!! instead of N it is possible to use identifiers (e.g. MC, data)
     */
    void readStyleFromFile(const std::string&);
    void addStyleFromFile(const std::string&);

    /**
     * cleans mem and deletes plots.
     * Doesn't change styles
     */
    void clear();
protected:

    void preparePad();
    void drawPlots();
    void drawLegends();
    plotStyle pstyle_;
    std::vector<containerStyle> cstyles_;
    std::vector<plot> plots_;
    bool drawlegend_,tightyaxis_,tightxaxis_;

private:


    float getMaximum()const;
    float getMinimum()const;


    void readStylePriv(const std::string & file, bool requireall);
};

}

#endif /* PLOTTERMULTIPLEPLOTS_H_ */
