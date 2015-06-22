/*
 * plotter2D.h
 *
 *  Created on: Oct 27, 2014
 *      Author: kiesej
 */

#ifndef PLOTTER2D_H_
#define PLOTTER2D_H_

#include "plotterBase.h"
#include "histo2D.h"

namespace ztop{

class plotter2D : public plotterBase{

public:
	plotter2D();
	~plotter2D();

	void setPlot(const histo2D*, bool dividebybinarea=false);
	//void setZAxis(const TString &in){zaxistitle_=in;}

/**
 * simplistic format:
 * zaxis.min
 * zaxis.max
 * rootDrawOpt
 */
    //void readStyleFromFile(const std::string&);


    bool hasRatio()const{return false;}

protected:
    void preparePad();
    void drawPlots();
    // void drawTextBoxes();
    void drawLegends();
    void refreshPad(){};
    void addStyleFromFile(const std::string&){}

private:

    void readStylePriv(const std::string & , bool );
	//hide .... throw
    plotStyle& getUpperStyle(){throw std::logic_error("NA");}
    const plotStyle& getUpperStyle()const{throw std::logic_error("NA");}
    plotStyle& getRatioStyle(){throw std::logic_error("NA");}
    const plotStyle& getRatioStyle()const{throw std::logic_error("NA");}
    float getXAxisLowerLimit()const{return 0;}
    float getXAxisHigherLimit()const{return 0;}

    histo2D plot_;
    bool dividebybinarea_;


    float zaxismin_,zaxismax_;
    TString rootDrawOpt_;//,zaxistitle_;

};



}



#endif /* PLOTTER2D_H_ */
