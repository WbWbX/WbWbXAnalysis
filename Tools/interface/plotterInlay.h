/*
 * plotterInlay.h
 *
 *  Created on: Jun 17, 2014
 *      Author: kiesej
 */

#ifndef PLOTTERINLAY_H_
#define PLOTTERINLAY_H_

#include "plotterMultiplePlots.h"
namespace ztop{

class plotterInlay: public plotterMultiplePlots{
    /*
     * Internal info:
     * Will use multiplotter capabilities for main plot and then ADD the functionality for
     * the inlayed plot
     *
     * will draw in the following way:
     * prepare main pad, do stanard multiPlots stuff
     * add extra pad, switch to it and do multiplots stuff there
     */
public:
    plotterInlay():plotterMultiplePlots(),inlayx0_(0),inlayy0_(0),inlayx1_(0),inlayy1_(0),inlaypad_(0){}
    ~plotterInlay(){}//if(inlaypad_) delete inlaypad_;} //tkane care of by deleting main pad
    plotterInlay & operator = (const plotterInlay& );
    plotterInlay(const plotterInlay&);


    void addInlayPlot(const graph *c);
    void addInlayPlot(const container1D *c,bool divbw=true);

    void removeInlayPlot(size_t idx);


    /**
     * style format:
     * expects entries for main plots:
     * drawLegend =
     * inlayX0 =
     * inlayY0 =
     * inlayX1 =
     * inlayY1 =
     * [containerStyle - MainDefault]
     * [containerStyle - Main<N=0,1,...>] <- only differences wrt to default
     * [containerStyle - InlayDefault]
     * [containerStyle - Inlay<N=0,1,...>] <- only differences wrt to default
     * [plotStyle - MainPlots]
     * [plotStyle - InlayPlots]
     * n plots main n plots inlay
     */
    void addStyleFromFile(const std::string&);
    void readStyleFromFile(const std::string&);

    void clear();

    /**
     * returns main plots multiplicity
     */
    size_t size()const{return plotterMultiplePlots::size();}
    /**
     * returns inlay plots multiplicity
     */
    size_t inlaySize()const{return plotsinlay_.size();}

private:


    void setInlayPad(float x0, float y0, float x1, float y1);
    TVirtualPad * getInlayPad()const; //throws
    TVirtualPad * getInlayPad();
    void drawPlots();
    void refreshPad(){};

    plotStyle pstyleinlay_;
    std::vector<containerStyle> cstylesinlay_;
    std::vector<plot> plotsinlay_;

    float inlayx0_,inlayy0_,inlayx1_,inlayy1_;
    TVirtualPad * inlaypad_;


    void copyFrom(const plotterInlay&);

    float getInlayMaximum()const;
    float getInlayMinimum()const;

    void readStylePriv(const std::string & file, bool requireall);
};


}//ns
#endif /* PLOTTERINLAY_H_ */
