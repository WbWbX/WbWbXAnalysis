/*
 * mtFromXSec2.cc
 *
 *  Created on: Feb 20, 2014
 *      Author: kiesej
 */
#include "TtZAnalysis/Tools/interface/optParser.h"
#include "TtZAnalysis/Tools/interface/graph.h"
#include "TtZAnalysis/Tools/interface/container.h"
#include "../interface/mtExtractor.h"

#include "TString.h"
#include <vector>
#include "TFile.h"

#include "TCanvas.h"

int main(int argc, char* argv[]){
    using namespace ztop;
    //get options
    //plotname, (fitmode), minbin, maxbin, excludebin
    optParser parse(argc,argv);
    parse.bepicky=true;
    TString plotname  =    parse.getOpt<TString>("p","m_lb pair comb unfold step 8");
    int minbin          =  parse.getOpt<float>    ("-minbin",-1);
    int maxbin          =  parse.getOpt<float>    ("-maxbin",-1);
    int excludebin      =  parse.getOpt<float>    ("-excludebin",-1);
    std::vector<TString> inputfiles = parse.getRest< TString >();


    /*
     *
    void setPlot(const TString& pl){plotnamedata_=pl;}
    void setMinBin(const int& pl){minbin_=pl;}
    void setMaxBin(const int& pl){excludebin_=pl;}
    void setExcludeBin(const int& pl){excludebin_=pl;}
    void setInputFilesData(const std::vector<TString>& pl){inputfiles_=pl;}
     *
     */

    //do this for each channel independently. Right now for testing reasons skip channel asso
// maybe then also implement all plots.. but later
    mtExtractor extractor;
    mtExtractor::debug=true;
    extractor.setPlot(plotname);
    extractor.setInputFilesData(inputfiles);
    extractor.getExtractor()->setLikelihoodMode(parameterExtractor::lh_chi2);

    extractor.setup();
    TCanvas*c=new TCanvas();
    extractor.setComparePlotterStyleFile((std::string)getenv("CMSSW_BASE")+
            "/src/TtZAnalysis/Tools/styles/comparePlots_mlbMCvsmt.txt");
    extractor.setBinsPlotterStyleFile((std::string)getenv("CMSSW_BASE")+
            "/src/TtZAnalysis/Tools/styles/multiplePlots_mlbbins.txt");


    TFile f("mtfromxsec.root","RECREATE");
    //for MC
    extractor.drawXsecDependence(c,false);
    c->Print("drawMCXsecDependence.pdf(");
    c->Write();
    extractor.cleanMem();
    c->Clear();
    //for data
    extractor.drawXsecDependence(c,true);
    c->Print("drawMCXsecDependence.pdf");
    c->Write();
    extractor.cleanMem();
    c->Clear();

    extractor.drawIndivBins(c);
    c->Print("drawMCXsecDependence.pdf)");
    c->Write();
    extractor.cleanMem();
    c->Clear();




    f.Close();



    //make combined extraction


    //split by syst



    delete c;

    return 0;
}
