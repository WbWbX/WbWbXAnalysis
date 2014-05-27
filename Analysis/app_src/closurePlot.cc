/*
 * closurePlot.cc
 *
 *  Created on: May 14, 2014
 *      Author: kiesej
 */

#include "TtZAnalysis/Tools/interface/graph.h"
#include "TtZAnalysis/Tools/interface/optParser.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "TCanvas.h"
#include "TGraphAsymmErrors.h"
#include "TtZAnalysis/Tools/interface/plotterMultiplePlots.h"
#include <vector>
#include "TLine.h"

int main(int argc, char* argv[]){
    using namespace ztop;
    optParser parser(argc,argv);
    parser.setAdditionalDesciption("Expects N + N_stat values for 7 mass points.\nFormat: mass1 stat1*100 mass2 stat2*100 ...\n");

    TString output = parser.getOpt<TString>("o","closure","output file name (without .pdf");

    std::vector<float> parsed = parser.getRest<float>();

    parser.doneParsing();
    if(parsed.size() != 14)
        parser.coutHelp();

    std::vector<float> masses,stats;
    for(size_t i=0;i<parsed.size();i++){
        masses.push_back(parsed.at(i));
        i++;
        stats.push_back(parsed.at(i) / 100);
    }

    graph g;
    g.setXAxisName("m_{t}^{in} [GeV]");
    g.setYAxisName("m_{t}^{out} - m_{t}^{in} [GeV]");

    std::vector<float> defmasses;
    defmasses << 166.5 << 169.5 << 171.5 << 172.5 << 173.5 << 175.5 << 178.5 ;

    for(size_t i=0;i<masses.size();i++){
        g.addPoint(defmasses.at(i), masses.at(i)-defmasses.at(i), stats.at(i));
    }

    plotterMultiplePlots pl;
    TString stylefile=getenv("CMSSW_BASE");
    stylefile+="/src/TtZAnalysis/Tools/styles/mtClosurePlots.txt";
    pl.readStyleFromFile(stylefile.Data());
    pl.setDrawLegend(false);
    pl.addPlot(&g);
    TCanvas * c = new TCanvas();
    pl.usePad(c);
    pl.draw();
    TLine * l = new TLine(165,0,180,0);
    l->Draw("same");
    c->Print(output+".pdf");
    delete l;
    delete c;
}
