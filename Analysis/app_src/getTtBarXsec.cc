/*
 * getTtBarXsec.cc
 *
 *  Created on: May 23, 2014
 *      Author: kiesej
 */

#include "TtZAnalysis/Tools/interface/containerStackVector.h"
#include "TString.h"
#include <iostream>
#include "../interface/ttbarXsecExtractor.h"
#include <vector>
#include "TtZAnalysis/Tools/interface/graph.h"
#include "TtZAnalysis/Tools/interface/extendedVariable.h"
#include "TtZAnalysis/Tools/interface/plotterMultiplePlots.h"
#include "TCanvas.h"
#include <string>
#include "TError.h"

#include "TtZAnalysis/Tools/interface/optParser.h"

int main(int argc, char* argv[]){

    using namespace ztop;

    optParser parse(argc,argv);
    parse.setAdditionalDesciption("program to determine the ttbar cross section using a likelihood approach with the b-jet multi\n parse input file (standard plots should be present)");
    bool ispseudodata=parse.getOpt<bool>("PD",false,"switch on if input is pseudo data (ngenevents*=0.9)");
    TString minstr = parse.getOpt<TString>("m","Minuit2","Minimizer to be used");
    TString minalgostr = parse.getOpt<TString>("a","Migrad","Minimizer algo to be used");

    std::vector<TString> inputfiles=parse.getRest<TString>();
    if(inputfiles.size()>1){
        parse.coutHelp();
        return -1;
    }
    parse.doneParsing();

    TString infilelocation="/nfs/dust/cms/user/kiesej/testtemp/emu_8TeV_172.5_nominal_syst.root";
    if(inputfiles.size()>0)
        infilelocation=inputfiles.at(0);

    TString btagmultiplot="selected b jet multi step 7"; //we use step 4 here, No jet required
    // TString jetmultiplot="hard jet multi step 7";

    /*
     * prepare the input
     */

    containerStackVector csv_in;
    csv_in.loadFromTFile(infilelocation);



    ttbarXsecExtractor extractor;
    extractor.setIsPseudoData(ispseudodata);
    extractor.setLumi(19741);
 //   extractor.setNGenerated(45096.8 + 116346);
    extractor.readInput(csv_in,btagmultiplot);
    extractor.setMinimizerStr(minstr);
    extractor.setMinimizerAlgoStr(minalgostr);

    extractor.extract();

    std::cout <<  extractor.getXsec() << " + "
            << extractor.getXsecErrUp()
            << " - " <<  extractor.getXsecErrDown() << " ("
            << " + " << extractor.getXsecErrUp()/extractor.getXsec()*100
            << " - " << extractor.getXsecErrDown()/extractor.getXsec()*100
            << " %)\n" <<std::endl;


    ////////////do some plotting

    std::vector< TString > sysnames=extractor.getSystNames();
    std::vector<std::vector<graph> > graphs=extractor.getSystGraphs();
    std::vector< extendedVariable* > vars = extractor.getAllExtendedVars();
    std::vector< double>  extrparas=*extractor.getFitter()->getParameters();
    extrparas.resize(sysnames.size());


    plotterMultiplePlots plotter;
    plotter.readStyleFromFile((std::string)getenv("CMSSW_BASE")+(std::string)"/src/TtZAnalysis/Tools/styles/multiplePlots_sysdependencies.txt");

    //switch off nasty root info output
    gErrorIgnoreLevel = kWarning;
    TCanvas * c= new TCanvas();
    plotter.usePad(c);
    plotter.setDrawLegend(false);
    c->Print("ttbarXsecOut.pdf[");
    size_t fitcounter=0;
    for(size_t sys=0;sys<sysnames.size();sys++){
        //  std::cout << extrparas.at(sys) << std::endl;
        for(size_t var=0;var<vars.size();var++){
            plotter.clear();

            graph sysg=graphs.at(sys).at(var);
            sysg.setXAxisName(sysnames.at(sys)+"[#sigma_{"+sysnames.at(sys)+"}]");
            sysg.setYAxisName(sysg.getName());

            graph fittedpoint;
            fittedpoint.addPoint(extrparas.at(sys),vars.at(var)->getValue(extrparas));

            graph function(200);

            for(size_t xpoint=0;xpoint<200;xpoint++){
                float xval=((float)xpoint-100)/20;
                function.setPointContents(xpoint,true,xval,vars.at(var)->getValue(sys,xval));

            }

            plotter.addPlot(&sysg);
            plotter.addPlot(&function);
        //    plotter.addPlot(&fittedpoint);

            plotter.draw();
            c->Print("ttbarXsecOut.pdf");
            fitcounter++;
        }
    }
    c->Print("ttbarXsecOut.pdf]");
    std::cout << "performed " <<fitcounter << " fits on variations" <<std::endl;

    // std::cout << "xsec / BR: " <<  extractor.getXsec() / 0.03284 <<std::endl;

}
