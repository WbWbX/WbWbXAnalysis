/*
 * plotter2D.h
 *
 *  Created on: Oct 27, 2014
 *      Author: kiesej
 */

#ifndef PLOTTER2D_H_
#define PLOTTER2D_H_

#include "plotterBase.h"
#include "container2D.h"

namespace ztop{

class plotter2D : public plotterBase{

public:
	plotter2D();
	~plotter2D();

	void setPlot(const container2D*, bool dividebybinarea=false);
	void setZAxis(const TString &in){zaxistitle_=in;}

/**
 * simplistic format:
 * zaxis.min
 * zaxis.max
 * rootDrawOpt
 */
    void readStyleFromFile(const std::string&);



protected:
    void preparePad();
    void drawPlots();
    // void drawTextBoxes();
    void drawLegends();
    void refreshPad(){};
    void addStyleFromFile(const std::string&){}

private:
    container2D plot_;
    bool dividebybinarea_;

    void readStylePriv(const std::string&s, bool){}

    float zaxismin_,zaxismax_;
    TString rootDrawOpt_,zaxistitle_;

};



}



#endif /* PLOTTER2D_H_ */
