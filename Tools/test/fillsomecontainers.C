/*
 * fillsomecontainers.C
 *
 *  Created on: Oct 2, 2013
 *      Author: kiesej
 */

#ifdef __CINT__
{
#endif


#include <vector>
#include "TRandom.h"
#include "../interface/canvasStyle.h"

#ifndef __CINT__
int main(){
#endif

    using namespace ztop;
    std::vector<float> bins;
    std::vector<float> otherbins;
    for(float i=0;i<21;i++)bins.push_back(i);

    for(float ji=0;ji<11;ji++)otherbins.push_back(ji*2);

    histo2D c2d(bins,bins);

    histo1D c1(bins),c2(bins),c3(bins),cerr;
    TRandom * r=new TRandom(123);

    for(int j=0;j<1000;j++){
        c1.fill(r->Gaus(8,4));
        c2.fill(r->Gaus(15,3));
        c3.fill(r->Gaus(14,1));
        c2d.fill(r->Gaus(14,1),r->Gaus(15,3));
    }


    histo1D fdata=c1+c2+c3;
    histoStack stack;
    // push_back(ztop::histo1D, TString, int, double); //! adds container with, name, colour, norm to st
    stack.push_back(fdata,"data",kBlack,1);
    stack.push_back(c1,"signal",kRed,1);
    stack.push_back(c2,"bg1",kBlue,1);
    stack.push_back(c3,"bg2",kGreen,1);
    stack.addSignal("signal");
    stack.addSignal("bg2");

    histoStack::batchmode = false;


    c1.addErrorContainer("syst_up",c2);
    c1.addErrorContainer("syst_down",c3);
    c1.addErrorContainer("syst2_up",c2*0.99);
    c1.addErrorContainer("syst2_down",c3*0.99);
    c1.addErrorContainer("syst3_up",c2*1.01);
    c1.addErrorContainer("syst3_down",c3*1.02);


    graph g;

    g.import(&c1);





#ifndef __CINT__
    return 0;
#endif

}


