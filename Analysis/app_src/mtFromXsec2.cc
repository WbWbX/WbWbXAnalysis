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
    optParser::debug=true;
    optParser parse(argc,argv);
    parse.bepicky=true;
    TString plotname  =    parse.getOpt<TString>("p","m_lb pair comb unfold step 8");
    TString output    =    parse.getOpt<TString>("o","mtFromXSec_output");
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
    extractor.setFitMode("pol4");

    extractor.setup();
    TCanvas*c=new TCanvas();
    extractor.setComparePlotterStyleFile((std::string)getenv("CMSSW_BASE")+
            "/src/TtZAnalysis/Tools/styles/comparePlots_mlbMCvsmt.txt");
    extractor.setBinsPlotterStyleFile((std::string)getenv("CMSSW_BASE")+
            "/src/TtZAnalysis/Tools/styles/multiplePlots_mlbbins.txt");

    extractor.setBinsChi2PlotterStyleFile((std::string)getenv("CMSSW_BASE")+
            "/src/TtZAnalysis/Tools/styles/multiplePlots_mlbbins.txt");

    TFile f(output+".root","RECREATE");
    //for MC
    extractor.drawXsecDependence(c,false);
    c->Print(output+".pdf(");
    c->Write();
    extractor.cleanMem();
    c->Clear();
    //for data
    extractor.drawXsecDependence(c,true);
    c->Print(output+".pdf");
    c->Write();
    extractor.cleanMem();
    c->Clear();

    extractor.drawIndivBins(c);
    c->Print(output+".pdf");
    c->Write();
    extractor.cleanMem();
    c->Clear();

    //for all systematics
    for(int i=-2;i<extractor.getDataSystSize();i++){
        if(i<-1)
            extractor.createBinLikelihoods(i,true);
        else
            extractor.createBinLikelihoods(i,false);

        extractor.drawBinLikelihoods(c);
        c->Print(output+".pdf");
        c->Write();
        extractor.cleanMem();
        c->Clear();

        extractor.createGlobalLikelihood();
        extractor.fitGlobalLikelihood();
        extractor.drawGlobalLikelihood(c);
        c->Print(output+".pdf");
        c->Write();
        extractor.cleanMem();
        c->Clear();

    }
    //close
    c->Print(output+".pdf)");

    f.Close();



    //make combined extraction


    //split by syst



    delete c;

    return 0;
}
