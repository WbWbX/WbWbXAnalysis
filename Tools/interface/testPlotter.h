/*
 * testPlotter.h
 *
 *  Created on: Jan 13, 2014
 *      Author: kiesej
 */

#ifndef TESTPLOTTER_H_
#define TESTPLOTTER_H_

#include "plotterBase.h"
#include "containerStyle.h"
#include "plotStyle.h"
#include "container.h"
#include "plot.h"

namespace ztop{

class testPlotter : public plotterBase{
public:
    testPlotter(){}
    ~testPlotter(){}

    void setPlot(const container1D *c);

protected:
    void preparePad();
    void drawPlots(); //object handling by plot class
    void drawTextBoxes(); //adds new objects to list
    void drawLegends();//adds new objects to list

//private:
public:
    containerStyle cstyle_;
    plotStyle pstyle_;
    plot plot_;

};


}


#endif /* TESTPLOTTER_H_ */
