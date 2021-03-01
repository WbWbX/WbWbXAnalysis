/*
 * testTestPlotter.cc
 *
 *  Created on: Jan 13, 2014
 *      Author: kiesej
 */

#include "../interface/testPlotter.h"

#include <vector>
#include "TRandom.h"
#include "../interface/plotterCompare.h"
#include "../interface/histo1D.h"
#include "../interface/histo2D.h"
#include "../interface/graph.h"
#include "TCanvas.h"
#include <iostream>
#include "../interface/histoStyle.h"
#include "../interface/plotStyle.h"
#include "../interface/fileReader.h"

int main(){
    using namespace ztop;
    std::vector<float> bins;
    std::vector<float> otherbins;
    for(float i=0;i<21;i++)bins.push_back(i);

    for(float ji=0;ji<11;ji++)otherbins.push_back(ji*2);

    histo2D c2d(bins,bins);

    histo1D c1(bins),c2(bins),c3(bins),fdata(bins),cerr;
    TRandom * r=new TRandom(123);

    for(int j=0;j<1000;j++){
        c1.fill(r->Gaus(8,4));
        c2.fill(r->Gaus(15,3));
        c3.fill(r->Gaus(14,1));
        c2d.fill(r->Gaus(14,1),r->Gaus(15,3));
        if(j>950)
            fdata.fill(r->Gaus(15,3));
    }
    TCanvas *c=new TCanvas();
    testPlotter::debug=true;

    std::string stylePath="/afs/naf.desy.de/user/k/kieseler/recentProj/src/WbWbXAnalysis/Tools/styles/";

    //fileReader::debug=true;
    plotStyle ps;
    ps.readFromFile(stylePath+"plotStyles.txt", "Test");
    histoStyle cs;
    cs.readFromFile(stylePath+"containerStyles.txt", "Test");


    c1.setNames("title","xaxis","yaxis");
    testPlotter tp;
    tp.pstyle_=ps;
    tp.cstyle_=cs;
    tp.usePad(c);
    tp.setPlot(&c1);

    tp.draw();
    c->Print("testTestPlotter.pdf");
    delete c;
    tp.cleanMem();

    return 0;
}
