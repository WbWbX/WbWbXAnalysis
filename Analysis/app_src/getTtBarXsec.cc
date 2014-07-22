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
#include "TtZAnalysis/Tools/interface/textFormatter.h"
#include "TCanvas.h"
#include <string>
#include "TError.h"
#include "TPavesText.h"

#include "TtZAnalysis/Tools/interface/optParser.h"

int main(int argc, char* argv[]){

    using namespace ztop;

    optParser parse(argc,argv);
    parse.setAdditionalDesciption("program to determine the ttbar cross section using a likelihood approach with the b-jet multi\n parse one input file for each channel  (standard plots should be present)");
    bool ispseudodata=parse.getOpt<bool>("PD",false,"switch on if input is pseudo data (ngenevents*=0.9)");
    TString minstr = parse.getOpt<TString>("m","Minuit2","Minimizer to be used");
    TString minalgostr = parse.getOpt<TString>("a","Migrad","Minimizer algo to be used");
    TString plotname=parse.getOpt<TString>("p","top likelihood output <N> b-jets step 7","Plots to be used (default: top likelihood output <N> b-jets step 7)");
    bool usepoisson =  !parse.getOpt<bool>("-gaus",false,"Not use poisson statistics and gaus instead");
    bool zerojet =  parse.getOpt<bool>("-zerojet",false,"use zero b-jet bin info");
    TString outname=parse.getOpt<TString>("o","","output add");

    if(outname.Length()<1)
        outname="ttbarXsecOut";
    else
        outname="ttbarXsecOut_"+outname;
    outname+=".pdf";

    std::vector<TString> inputfiles=parse.getRest<TString>();
    if(inputfiles.size()<1){
        parse.coutHelp();
        return -1;
    }
    parse.doneParsing();

 //   TString infilelocation;//="/nfs/dust/cms/user/kiesej/testtemp/emu_8TeV_172.5_nominal_syst.root";
  //  if(inputfiles.size()>0)
//        infilelocation=inputfiles.at(0);

    //   TString btagmultiplot="top likelihood output <N> b-jets step 7"; //we use step 4 here, No jet required
    // TString jetmultiplot="hard jet multi step 7";

    /*
     * prepare the input
     */


    std::vector<containerStackVector*> allincsvs;
    for(size_t i=0;i<inputfiles.size();i++){
        containerStackVector *csv_in=new containerStackVector();
        csv_in->loadFromTFile(inputfiles.at(i));
        allincsvs.push_back(csv_in);
    }

    ttbarXsecExtractor extractor;
    extractor.setIsPseudoData(ispseudodata);
    extractor.setLumi(19741);
    extractor.setUsePoisson(usepoisson);
    extractor.setUseZeroJet(zerojet);

    extractor.readInput(allincsvs,plotname);

    for(size_t i=0;i<allincsvs.size();i++)
        delete allincsvs.at(i);

    extractor.setMinimizerStr(minstr);
    extractor.setMinimizerAlgoStr(minalgostr);
    extractor.coutOutStream();
    extractor.clearOutStream();

    extractor.extract();

    containerStackVector& csvout=extractor.getPlots();

    csvout.writeAllToTFile(outname+".root",true,false);

    TString out=toTString(extractor.getXsec())  + " + "
            + toTString(extractor.getXsecErrUp())
            + " - "  + toTString( extractor.getXsecErrDown())  + " ("
            + " +"  + toTString(extractor.getXsecErrUp()/extractor.getXsec()*100)
            + " "  + toTString(extractor.getXsecErrDown()/extractor.getXsec()*100)
            + " %)";

    //////////// do some plotting

    std::vector< TString > sysnames=extractor.getSystNames();
    std::vector<graph>  graphs=extractor.getAllGraphs();
    std::vector< extendedVariable* > vars = extractor.getAllExtendedVars();
    std::vector< double>  extrparas=*extractor.getFitter()->getParameters();
    // extrparas.resize(sysnames.size());


    plotterMultiplePlots plotter;
    plotter.readStyleFromFileInCMSSW("/src/TtZAnalysis/Analysis/configs/xsecs/multiplePlots_sysdependencies.txt");

    //switch off nasty root info output
    gErrorIgnoreLevel = kWarning;

    TPavesText * configurationtext= new TPavesText(0.1,0.1,0.9,0.9);//;extractor.printConfig();

    configurationtext->AddText(out.Data());
    configurationtext->AddText(plotname);
    configurationtext->AddText(minstr);
    configurationtext->AddText(minalgostr);

    TCanvas * c= new TCanvas();
    configurationtext->Draw();
    c->SetName("result");
    c->Print(outname+"(");

    c->Clear();
    c->SetName("by bins");
    configurationtext->Clear();
    textFormatter tf;
    tf.setDelimiter("\n");
    std::vector<std::string> lines = tf.getFormatted(extractor.dumpOutStream().Data());

    size_t count=1;
    for(size_t line=0;line<lines.size();line++){
        configurationtext->AddText(lines.at(line).data());
        if(line>count*40){
            configurationtext->Draw();
            c->Print(outname);
            configurationtext->Clear();
            c->Clear();
            count++;
        }
    }
    configurationtext->Draw();
    c->Print(outname);
    // c->Print(outname+")");
    c->Clear();


    plotter.usePad(c);
    plotter.setDrawLegend(false);
    c->Print(outname);
    size_t fitcounter=0;



    for(size_t gidx=0;gidx<graphs.size();gidx++){

        size_t var=gidx;
        if(!extractor.fitWasUsed(gidx)) continue;

        plotter.clear();

        graph sysg=graphs.at(gidx);

        //  std::cout << sysg.getName() <<std::endl;


        //   graph fittedpoint;
        //    fittedpoint.addPoint(extrparas.at(gidx),vars.at(var)->getValue(extrparas));

        graph function(200);
        size_t varidx=10000;
        for(size_t sys=0;sys<sysnames.size();sys++){
            if(sysg.getName().Contains(sysnames.at(sys))){
                varidx=sys; break;
            }
        }
        for(size_t xpoint=0;xpoint<200;xpoint++){
            float xval=((float)xpoint-100)/50;

            function.setPointContents(xpoint,true,xval,vars.at(var)->getValue(varidx,xval));

        }

        plotter.addPlot(&sysg);
        plotter.addPlot(&function);
        //    plotter.addPlot(&fittedpoint);

        plotter.draw();
        c->Print(outname);
        fitcounter++;

    }



    c->Print(outname+")");
    std::cout << "performed " <<fitcounter << " fits on variations" <<std::endl;
    extractor.coutOutStream();


    std::cout  << out   <<std::endl;

}
