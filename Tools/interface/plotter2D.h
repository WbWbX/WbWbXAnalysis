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


    void readStyleFromFile(const std::string&){}
    void addStyleFromFile(const std::string&){}


protected:
    void preparePad();
    void drawPlots();
    // void drawTextBoxes();
    void drawLegends();
    void refreshPad(){};

private:
    container2D plot_;
    bool dividebybinarea_;

    void readStylePriv(const std::string&s, bool){}

};



}



#endif /* PLOTTER2D_H_ */
