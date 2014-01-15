/*
 * testPlotter.cc
 *
 *  Created on: Jan 13, 2014
 *      Author: kiesej
 */
#include "../interface/testPlotter.h"


namespace ztop{

void testPlotter::setPlot(const container1D * c){
    plot_.createFrom(c);
}

void testPlotter::preparePad(){
    TVirtualPad * c = getPad();
    pstyle_.applyPadStyle(c);
    c->cd();
}
void  testPlotter::drawPlots(){
    getPad()->cd();
    TG * g = plot_.getSystGraph();
    TH1 * axish=addObject(plot_.getInputGraph().getAxisTH1(false,true));
    pstyle_.applyAxisStyle(axish);
    cstyle_.applyContainerStyle(g,false);
    axish->Draw("AXIS");
    g->Draw("same,P"+cstyle_.rootDrawOpt);
}
void  testPlotter::drawTextBoxes(){
//void
}
void  testPlotter::drawLegends(){
  // TLegend * leg=addObject(new TLegend()); //TBI
}


}
