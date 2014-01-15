/*
 * testplotterControlPlot.cc
 *
 *  Created on: Jan 15, 2014
 *      Author: kiesej
 */





#ifdef __CINT__
{
#endif


#include <vector>
#include "TRandom.h"
#include "../interface/containerStack.h"
#include "../interface/plotterControlPlot.h"
#include "TStyle.h"

#ifndef __CINT__
int main(){
#endif

    using namespace ztop;
    std::vector<float> bins;
    std::vector<float> otherbins;
    for(float i=0;i<29;i++)bins.push_back(i);

    for(float ji=0;ji<11;ji++)otherbins.push_back(ji*2);

    container2D c2d(bins,bins);

    container1D c1(bins),c2(bins),c3(bins),cerr;
    TRandom * r=new TRandom(123);

    for(int j=0;j<10000;j++){
        c1.fill(r->Gaus(8,4));
        c2.fill(r->Gaus(15,3));
        c3.fill(r->Gaus(14,1));
        c2d.fill(r->Gaus(14,1),r->Gaus(15,3));
    }

    containerStack::debug=true;
    container1D fdata=c1+c2+c3;
    fdata *= 1.05;

    fdata.addGlobalRelError("relerror",0.1);
   // fdata-=c3*0.15;
    c1.addGlobalRelError("somemcsyst",0.1);
    fdata.setNames("data","xaxis [u^{N}_{i}Ts]","yaxis [u^{N}_{i}Ts]");

    containerStack stack;
    // push_back(ztop::container1D, TString, int, double); //! adds container with, name, colour, norm to st
    stack.push_back(c1,"signal",kRed,1,1);
    stack.push_back(c2,"bg1",kBlue,1,2);
    stack.push_back(fdata,"data",kBlack,1,0);
    stack.push_back(c3,"bg2",kGreen,1,9);

    stack.addSignal("signal");

    containerStack::batchmode = false;

    std::string stylePath="/afs/naf.desy.de/user/k/kieseler/recentProj/src/TtZAnalysis/Tools/styles/";

    plotterControlPlot::debug=true;
    plotterControlPlot pl;
    pl.readStyleFromFile(stylePath+"controlPlots_standard.txt");

    gStyle->SetOptStat(0);
    TCanvas *c = new TCanvas();
    pl.setTitle("test controlPlot");
    pl.usePad(c);
    pl.setStack(&stack);

    pl.draw();
    c->Print("testPlotterControlPlot.pdf");

#ifndef __CINT__
    return 0;
#endif

}


