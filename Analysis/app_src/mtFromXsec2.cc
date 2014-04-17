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

///DEBUG
#include "TtZAnalysis/Tools/interface/fileReader.h"

int main(int argc, char* argv[]){
    using namespace ztop;
    //get options
    //plotname, (fitmode), minbin, maxbin, excludebin
    std::string cmsswbase=getenv("CMSSW_BASE");
    // optParser::debug=true;
    optParser parse(argc,argv);
    parse.bepicky=true;
    TString plotname  =    parse.getOpt<TString>("p","m_lb pair comb unfold step 8","input plot name");
    TString output    =    parse.getOpt<TString>("o","mtFromXSec_output","output file name");
    TString mode    =    parse.getOpt<TString>("m","fit","mode (fit, fitintersect,chi2)");
    TString fitmode    =    parse.getOpt<TString>("f","pol4","fit mode in root format");
    TString usepdf    =    parse.getOpt<TString>("-pdf","MSTW200","chose a PDF to be used (if external input is fed): default: MSTW200");
    int minbin          =  parse.getOpt<int>    ("-minbin",-1,"minimum bin number to be considered in fit");
    int maxbin          =  parse.getOpt<int>    ("-maxbin",-1,"maximum bin number to be considered in fit");
    int excludebin      =  parse.getOpt<int>    ("-excludebin",-1,"exclude bin from fit");
    bool usefolding     =  parse.getOpt<bool>   ("-fold",false,"use folding of genertor input");
    bool nosyst     =  parse.getOpt<bool>   ("-nosyst",false,"skip systematics");
    std::string extconfigfile      =  parse.getOpt<std::string>    ("-c",
            ((TString)cmsswbase+"/src/TtZAnalysis/Analysis/configs/mtExtractor_config.txt").Data(),
            "specify config file for external gen input");
    std::vector<TString> inputfiles = parse.getRest< TString >();

    parse.doneParsing();

    //requirements
    if(inputfiles.size() <1 || (mode!="fit" && mode!="chi2"&&mode!="fitintersect")){

        parse.coutHelp();
        std::cout << "specify at least one input file" <<std::endl;
        return -1;
    }

    std::cout << "starting mt extraction..." <<std::endl;


    //fileReader::debug=true;
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

    parameterExtractor::debug=true;
    container1D::debug=true;

    if(mode=="fitintersect")
        extractor.getExtractor()->setLikelihoodMode(parameterExtractor::lh_fitintersect);
    else if(mode=="chi2")
        extractor.getExtractor()->setLikelihoodMode(parameterExtractor::lh_chi2);
    else if(mode=="fit")
            extractor.getExtractor()->setLikelihoodMode(parameterExtractor::lh_fit);
    extractor.getExtractor()->setFitFunctions("pol2");
    extractor.getExtractor()->setConfidenceLevelFitInterval(0.68);
    extractor.getExtractor()->setIntersectionGranularity(2000);
    extractor.setExternalGenInputFilesFormat(extconfigfile);
    extractor.setPlot(plotname);
    extractor.setExcludeBin(excludebin);
    extractor.setMinBin(minbin);
    extractor.setMaxBin(maxbin);
    extractor.setUseFolding(usefolding);
    extractor.setInputFiles(inputfiles);
    bool isexternalgen=extractor.getIsExternalGen();
    extractor.setExternalGenInputPDF(usepdf);
    //  extractor.getExtractor()->setLikelihoodMode(parameterExtractor::lh_chi2);
    extractor.setFitMode(fitmode);

    output+="_"+plotname;
    if(isexternalgen){
        output+="_"+usepdf+"_ext";
    }
    output+="_"+mode;
    output.ReplaceAll(" ","_");

    extractor.setup();
    TCanvas*c=new TCanvas();
    extractor.setComparePlotterStyleFile((std::string)getenv("CMSSW_BASE")+
            "/src/TtZAnalysis/Tools/styles/comparePlots_mlbMCvsmt.txt");
    extractor.setBinsPlotterStyleFile((std::string)getenv("CMSSW_BASE")+
            "/src/TtZAnalysis/Tools/styles/multiplePlots_mlbbins.txt");

    extractor.setBinsChi2PlotterStyleFile((std::string)getenv("CMSSW_BASE")+
            "/src/TtZAnalysis/Tools/styles/multiplePlots_mlbbins.txt");

    extractor.setBinsPlusFitPlotterStyleFile((std::string)getenv("CMSSW_BASE")+
            "/src/TtZAnalysis/Tools/styles/multiplePlots_mlbbinsPlusFit.txt");

    extractor.setAllSystStyleFile((std::string)getenv("CMSSW_BASE")+
            "/src/TtZAnalysis/Tools/styles/multiplePlots_mlbbinsTotalSyst.txt");

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
    c->Print(output+".pdf");
    c->Write();
    extractor.cleanMem();
    c->Clear();

    //for all systematics
    for(int i=-1;i<extractor.getDataSystSize();i++){

            extractor.createBinLikelihoods(i,false);

        if(i>=0 && nosyst) break;



        extractor.drawBinLikelihoods(c);
        c->Print(output+".pdf");
        c->Write();

        c->Clear();
        //if(extractor.getExtractor()->getLikelihoodMode()==parameterExtractor::lh_fit){
            extractor.drawBinsPlusFits(c,i);
            c->Print(output+".pdf");
            c->Write();
            c->Clear();
        //}
        extractor.cleanMem();
        extractor.createGlobalLikelihood();
        extractor.fitGlobalLikelihood();
        extractor.drawGlobalLikelihood(c);
        c->Print(output+".pdf");
        c->Write();
        extractor.cleanMem();
        c->Clear();

    }
    //close

    //  extractor.drawIndivBins(c);
    //  extractor.overlayBinFittedFunctions(c);
    c->Print(output+".pdf");
    c->Write();

    c->Clear();
    extractor.drawResultGraph(c);
    c->Write();
    c->Print(output+".pdf)");

    f.Close();



    //make combined extraction


    //split by syst



    delete c;

    return 0;
}
