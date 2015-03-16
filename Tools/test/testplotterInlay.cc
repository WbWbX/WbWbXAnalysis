/*
 * testplotterInlay.cc
 *
 *  Created on: Jun 17, 2014
 *      Author: kiesej
 */


#include "../interface/plotterInlay.h"
#include "../interface/histo1D.h"
#include "fillContainerRandom.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "TCanvas.h"

int main(){

    using namespace ztop;
    using namespace std;
    std::vector<float> bins;
    bins << 0 << 1 << 2 << 5 << 10 << 14;
    histo1D c1 = histo1D(bins,"c1","xaxis","yaxis");
    histo1D c2 = histo1D(bins,"c2","xaxis","yaxis");

    bins.clear();
    bins << 40 << 50 << 200 << 500 << 1000 << 1400;
    histo1D c3 = histo1D(bins,"c3Inl","xaxis","yaxis");

    fillRandom(&c1,func_gaus,4,5,5e3);
    fillRandom(&c2,func_gaus,3,8,5e3);
    fillRandom(&c3,func_breitwigner,400,5,2e3);

    plotterInlay pl;
    plotterBase::debug=true;


    pl.addPlot(&c1);
    pl.addPlot(&c2);
    pl.addInlayPlot(&c3);
    pl.readStyleFromFile("styles/inlayPlots.txt");
    TCanvas *c=new TCanvas();
    pl.usePad(c);
    pl.draw();

    c->Print("inlaytest.pdf");
    delete c;
    return 0;
}
