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
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"

#include "TString.h"
#include <vector>
#include "TFile.h"

#include "TtZAnalysis/Tools/interface/textFormatter.h"
#include "TCanvas.h"
#include "TPavesText.h"
#include "TLatex.h"
#include "TtZAnalysis/Tools/interface/plotterControlPlot.h"

#include "TtZAnalysis/Tools/interface/fileReader.h"
#include "TStyle.h"
#include "TtZAnalysis/Tools/interface/formatter.h"

#include "TtZAnalysis/Tools/interface/plotterMultiplePlots.h"
#include "TColor.h"
#include <iomanip>
std::vector<TString> filelist;

void WriteAndPrint(TCanvas*c, TFile &f,const TString& outputpdf,const TString& outdir){
    c->Print(outputpdf+".pdf");
    f.WriteTObject(c);
    c->Print(outdir+"/" + c->GetName()+".eps");
    filelist.push_back(outdir+"/" + c->GetName());
    //  TString syscall="epstopdf --outfile="+outdir+"/" + c->GetName()+".pdf " +outdir+"/" + c->GetName()+".eps";

    //  system(syscall.Data());
    //  TString delcall="rm -f "+outdir+"/" + c->GetName()+".eps";
    //  system(delcall.Data());

}


int main(int argc, char* argv[]){
    using namespace ztop;
    //get options
    //plotname, (fitmode), minbin, maxbin, excludebin
    std::string cmsswbase=getenv("CMSSW_BASE");
    // optParser::debug=true;
    optParser parse(argc,argv);
    parse.bepicky=true;
    const  TString plotname  =    parse.getOpt<TString>("p","m_lb pair comb unfold step 8","input plot name");
    TString output    =    parse.getOpt<TString>("o","","output file name extra");
    TString outdirectory   =    parse.getOpt<TString>("-dir","mtextract","output directory name");
    const TString mode    =    parse.getOpt<TString>("m","fit","mode (fit, fitintersect,chi2)");
    const TString fitmode    =    parse.getOpt<TString>("f","pol2","fit mode in root format");
    const float defmtop    =    parse.getOpt<float>("-defmt",172.5,"use other default mtop (used for calibration)");
    const TString usepdf    =    parse.getOpt<TString>("-pdf","MSTW200","chose a PDF to be used (if external input is fed): default: MSTW200");
    const bool rescaletonnlonnll = parse.getOpt<bool>   ("K",false,"rescale NLO norm to NNLO+NNLL using pdf dependent k-factor");
    const int minbin          =  parse.getOpt<int>    ("-minbin",-1,"minimum bin number to be considered in fit");
    const int maxbin          =  parse.getOpt<int>    ("-maxbin",-1,"maximum bin number to be considered in fit");
    const int excludebin      =  parse.getOpt<int>    ("-excludebin",-1,"exclude bin from fit");
    const bool usefolding     = ! parse.getOpt<bool>   ("-nofold",false,"dont! use folding of genertor input");
    const bool nosyst     =  parse.getOpt<bool>   ("-nosyst",false,"skip systematics");
    std::string extconfigfile      =  parse.getOpt<std::string>    ("-config",
            ((TString)cmsswbase+"/src/TtZAnalysis/Analysis/configs/mtExtractor_config.txt").Data(),
            "specify config file for external gen input");
    const bool normalize  = parse.getOpt<bool>   ("-normalize",false,"normalize all distributions");
    const bool makepdfs   =  parse.getOpt<bool>   ("-makepdfs",false,"converts output from eps to pdf");

    const bool onlyCP =  ! parse.getOpt<bool>   ("-extract",false,"really perform extraction");

    const TString extprefix =  parse.getOpt<TString>("-prefix","mcfm_","external file prefix (everything before topmass)");

    std::vector<TString> inputfiles = parse.getRest< TString >();

    parse.doneParsing();

    if(output.Length()>0)
        outdirectory = output +"_" + outdirectory;





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

    extractor.setConfigFile(extconfigfile);
    extractor.setExternalPrefix(extprefix);


    extractor.setDefMTop(defmtop);
    extractor.setUseNormalized(normalize);

    mtExtractor::debug=true;
    // histoContent::debug=true;

    // parameterExtractor::debug=true;
    // container1D::debug=true;

    if(mode=="fitintersect")
        extractor.getExtractor()->setLikelihoodMode(parameterExtractor::lh_fitintersect);
    else if(mode=="chi2")
        extractor.getExtractor()->setLikelihoodMode(parameterExtractor::lh_chi2);
    else if(mode=="fit")
        extractor.getExtractor()->setLikelihoodMode(parameterExtractor::lh_fit);



    extractor.setFitUncertaintyModeData(parameterExtractor::fitunc_statcorrgaus);
    extractor.setFitUncertaintyModeMC(parameterExtractor::fitunc_statuncorrpoisson);

    extractor.getExtractor()->setFitFunctions("pol2");
    extractor.getExtractor()->setConfidenceLevelFitInterval(0.68);
    extractor.getExtractor()->setIntersectionGranularity(500);

    extractor.setPlot(plotname);
    extractor.setExcludeBin(excludebin);
    extractor.setMinBin(minbin);
    extractor.setMaxBin(maxbin);
    extractor.setUseFolding(usefolding);
    extractor.setInputFiles(inputfiles);
    const bool isexternalgen=extractor.getIsExternalGen();
    extractor.setExternalGenInputPDF(usepdf);
    extractor.setRescaleNLOPred(rescaletonnlonnll);

    //  extractor.getExtractor()->setLikelihoodMode(parameterExtractor::lh_chi2);
    extractor.setFitMode(fitmode);

    if(outdirectory.Length()>0)
        outdirectory+="_"+plotname;
    else
        outdirectory=plotname;

    if(isexternalgen){
        std::cout << "is external...."<<std::endl;
        TString newpostfix=extprefix;
        if(newpostfix.EndsWith("_"))
            newpostfix = TString(newpostfix,newpostfix.Length()-1);
        if(newpostfix.Contains("_todk")){
            newpostfix.ReplaceAll("_todk",""); //for consistency with old naming scheme
        }
        outdirectory+="_"+usepdf+"_"+newpostfix;
        if(rescaletonnlonnll)
            outdirectory+="_kfact";
    }
    outdirectory.ReplaceAll("unfold ","");

    outdirectory.ReplaceAll(" ","_");
    if(normalize)
        outdirectory+="_normd";

    if(output.Length()>0)
        output+="_summary.pdf";
    else
        output+="summary.pdf";

    system(("mkdir -p "+outdirectory).Data());

    system(("mkdir -p "+outdirectory).Data());
    output=outdirectory+"/"+output;


    std::vector<float> allmasses,nominals,errups,errdowns;


    TFile f(output+".root","RECREATE");

    TCanvas*c=new TCanvas();
    c->SetBatch(true);
    c->Print(output+".pdf(");


    //print configuration on first page
    TString configString=extractor.printConfig();
    // configString.ReplaceAll("\n"," \\ ");

    textFormatter formattext;
    formattext.setDelimiter("\n");
    formattext.setComment("");
    formattext.setTrim(""); //only get lines

    std::vector<std::string> confstrings=formattext.getFormatted(configString.Data());


    TPavesText * configurationtext= new TPavesText(0.1,0.1,0.9,0.9);//;extractor.printConfig();
    for(size_t i=0;i<confstrings.size();i++){
        configurationtext->AddText(confstrings.at(i).data());
    }

    configurationtext->Draw();
    c->SetName("config");
    WriteAndPrint(c,f,output,outdirectory);
    c->Clear();
    delete configurationtext;

    if(!onlyCP){
        extractor.setup();
        //for MC
        extractor.drawXsecDependence(c,false);
        c->SetName("dependence_theo");
        WriteAndPrint(c,f,output,outdirectory);
        extractor.cleanMem();
        c->Clear();
        //for data
        extractor.drawXsecDependence(c,true);
        c->SetName("dependence_data");
        WriteAndPrint(c,f,output,outdirectory);
        extractor.cleanMem();
        c->Clear();

        extractor.drawIndivBins(c);
        c->SetName("indivibins_allsys");
        WriteAndPrint(c,f,output,outdirectory);
        extractor.cleanMem();
        c->Clear();

        float chi2nommin=0;

        //for all systematics
        for(int i=-1;i<extractor.getDataSystSize();i++){

            extractor.createBinLikelihoods(i,false);

            if(i>=0 && nosyst) break;

            TString systname="_nominal";
            if(i>-1)
                systname="_"+extractor.getSystName(i);


            extractor.drawBinsPlusFits(c,i);
            c->SetName("bins_plus_fits"+systname);
            WriteAndPrint(c,f,output,outdirectory);
            c->Clear();

            extractor.drawBinLikelihoods(c);
            c->SetName("bins_chi2"+systname);
            WriteAndPrint(c,f,output,outdirectory);
            c->Clear();
            //}
            extractor.cleanMem();
            extractor.createGlobalLikelihood();
            extractor.fitGlobalLikelihood();
            if(i<0)
                chi2nommin=extractor.drawGlobalLikelihood(c);
            else
                extractor.drawGlobalLikelihood(c);
            c->SetName("global_chi2"+systname);
            WriteAndPrint(c,f,output,outdirectory);
            extractor.cleanMem();
            c->Clear();

        }
        //close

        //  extractor.drawIndivBins(c);
        //  extractor.overlayBinFittedFunctions(c);


        float nom,errup,errdown,totup,totdown;

        c->Clear();
        extractor.drawResultGraph(c,&nom,&errdown,&errup,&totdown,&totup);


        TLatex * tb= new TLatex(0.15, 0.05, "#chi^{2}_{min,nom}/N_{dof}="+toTString(chi2nommin)+ "/" + toTString(extractor.getNDof())
                + "=" +toTString(chi2nommin / extractor.getNDof()));
        tb->SetTextSize(0.04);
        tb->SetNDC(true);
        tb->Draw("same");

        c->SetName("results");
        WriteAndPrint(c,f,output,outdirectory);


        std::vector<TString> sysvars = extractor.getSystNameList();
        for(size_t i=0;i<sysvars.size();i++){
            c->SetName("variation_"+sysvars.at(i));
            extractor.drawSystVariation(c,sysvars.at(i));
            WriteAndPrint(c,f,output,outdirectory);
        }


        //results table

        texTabler table=extractor.makeSystBreakdown();
        table.writeToFile(outdirectory+"/sys_breakdown.tex");

        c->SetName("spread_inlay");
        extractor.drawSpreadWithInlay(c);
        WriteAndPrint(c,f,output,outdirectory);


        extractor.reset(); //resets for next iteration, keeps config and input files, just deleted temp stuff


        std::cout<< /*std::setprecision(2) << */"\n\nextracted nominal mass: " << nom << " +" << errup << " -" <<errdown << " (stat)  "
                <<  " +" << totup << " -" << totdown<< " (total)\n\n"<<std::endl;




    }
    /////////////syst variations






    /////////////////////////////////additional control plots./////////////////////////////////
    ///print selected plots here or in extractor??
    fileReader specialcplots;
    specialcplots.setComment("$");
    specialcplots.setStartMarker("[additional plots]");
    specialcplots.setEndMarker("[end additional plots]");
    specialcplots.setDelimiter(",");
    specialcplots.readFile(extconfigfile);

    //get plot names
    std::vector<std::string> plotnames= specialcplots.getMarkerValues("plot");

    for(size_t i=0;i<plotnames.size();i++){

        std::cout << plotnames.at(i)<<std::endl;;

    }

    TString defmtfile=extractor.getDefMtopDataFile();
    std::cout << "loading additional plots from " << defmtfile<<std::endl;
    containerStackVector defmtvec;
    defmtvec.loadFromTFile(defmtfile);
    gStyle->SetOptStat(0);

    system(((TString)"mkdir -p controlPlots").Data());

    for(size_t i=0;i<plotnames.size();i++){
        fileReader extraconfig;
        extraconfig.setComment("$");
        extraconfig.setDelimiter(",");
        extraconfig.setStartMarker("[plot - " + plotnames.at(i) +"]");
        if(i<plotnames.size()-1)
            extraconfig.setEndMarker("[plot - "+ plotnames.at(i+1) +"]");
        extraconfig.readFile(extconfigfile);
        std::string tmpfile=extraconfig.dumpFormattedToTmp();
        containerStack stack=defmtvec.getStack(plotnames.at(i));


        TString name=stack.getName();
        name.ReplaceAll(" ","_");
        TCanvas cv(name);
        if(stack.is1D() || stack.is1DUnfold()){
            plotterControlPlot pl;
            textBoxes::debug=true;
            pl.setTextBoxNDC(true);
            pl.readStyleFromFileInCMSSW("/src/TtZAnalysis/Tools/styles/controlPlots_standard.txt");
            pl.addStyleFromFile(tmpfile);
            pl.readTextBoxesInCMSSW("/src/TtZAnalysis/Analysis/configs/general/CMS_boxes.txt","CMSSplit03Left");
            pl.setStack(&stack);
            pl.usePad(&cv);
            pl.draw();
            WriteAndPrint(&cv,f,output,"controlPlots/");
        }

        else if(stack.is2D()){//TBI

        }
        TString syscall="rm -f "+tmpfile;
        system(syscall.Data());


    }


    //make yield table
    containerStack stack=defmtvec.getStack("total step 8");

    //test
    /*
    std::vector<TString> testcontr;
    for(size_t i=0;i<stack.size();i++)
        if(stack.getLegend(i) != "data" && stack.getLegend(i)!= "t#bar{t}"&&stack.getLegend(i)!= "t#bar{t}(#tau)")testcontr.push_back(stack.getLegend(i));

    stack.mergeLegends(testcontr,"BG");
     */
    //test end

    texTabler yieldtable("l | rcr");
    yieldtable << "Process" << "\\multicolumn{3}{c}{Events} ";
    yieldtable.newLine();
    yieldtable << texLine(1);
    std::map<TString,histoBin> yields= stack.getAllContentsInBin(1,false);
    for(std::map<TString,histoBin>::iterator contrib=yields.begin();contrib!=yields.end();++contrib){
        if(contrib->first == "total MC" || contrib->first == "data"){
            continue;
        }

        formatter fmt;
        TString entry=contrib->first;
        entry.ReplaceAll("#","\\");
        entry.ReplaceAll(" ","\\ ");
        entry.ReplaceAll("rightarrow","rightarrow ");
        float value=fmt.round(contrib->second.getContent(),1);
        TString valuestr=fmt.toTString(value);
        float stat=fmt.round(contrib->second.getStat(),1);

        yieldtable << "$"+entry+"$"<<  valuestr << "$\\pm$" << fmt.toTString(stat);
    }

    for(std::map<TString,histoBin>::iterator contrib=yields.begin();contrib!=yields.end();++contrib){
        if(contrib->first == "total MC" || contrib->first == "data"){
            yieldtable << texLine(1);
            formatter fmt;
            TString entry=contrib->first;
            entry.ReplaceAll("#","\\");
            entry.ReplaceAll(" ","\\ ");
            entry.ReplaceAll("rightarrow","rightarrow ");

            float value=fmt.round(contrib->second.getContent(),1);
            TString valuestr=fmt.toTString(value);
            float stat=fmt.round(contrib->second.getStat(),1);
            if(contrib->first != "data")
                yieldtable << "$"+entry+"$"<<  valuestr << "$\\pm$" << fmt.toTString(stat);
            else
                yieldtable << "$"+entry+"$"<< valuestr << " " << " ";
        }
    }

    //std::cout << yieldtable.getTable() << std::endl;
    yieldtable.writeToFile("controlPlots/yields.tex");





    ///make response matrices
    stack=defmtvec.getStack(plotname);
    container1DUnfold signcuf=stack.produceUnfoldingContainer();
    container2D mresp=signcuf.getNormResponseMatrix();

    //fast print
    c->Clear();
    c->SetName("response_matrix");
    c->SetBottomMargin(0.15);
    c->SetLeftMargin(0.15);
    c->SetRightMargin(0.2);
    TString xaxisnamenew= mresp.getXAxisName();
    xaxisnamenew.ReplaceAll("reco","gen");
    mresp.setXAxisName(xaxisnamenew);
    TH2D * mrespth2d=mresp.getTH2D("",false,true);
    c->cd();
    ////
    const Int_t NRGBs = 5;
    const Int_t NCont = 255;

    Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
    Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
    Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    gStyle->SetNumberContours(NCont);
    ////
    mrespth2d->GetZaxis()->SetRangeUser(0,0.11);
    mrespth2d->GetZaxis()->SetTitle("");
    mrespth2d->GetZaxis()->SetTitleSize(0.06);
    mrespth2d->GetZaxis()->SetLabelSize(0.05);
    mrespth2d->GetYaxis()->SetTitleSize(0.06);
    mrespth2d->GetYaxis()->SetLabelSize(0.05);
    mrespth2d->GetXaxis()->SetTitleSize(0.06);
    mrespth2d->GetXaxis()->SetLabelSize(0.05);
    mrespth2d->Draw("colz");
    textBoxes tb2d;
    tb2d.readFromFileInCMSSW("/src/TtZAnalysis/Analysis/configs/general/CMS_boxes.txt","CMSnoSplitRight2DSim");
    tb2d.drawToPad(c);
    WriteAndPrint(c,f,output,outdirectory);
    delete mrespth2d;


    ////efficiency

    container1D efficiency =signcuf.getEfficiency();// mresp.projectToX(false);
    container1D ineff=mresp.projectToX(true)-efficiency;
    efficiency.removeAllSystematics();
    efficiency.transformToEfficiency();
    TString tmp=efficiency.getName();
    efficiency.setXAxisName(tmp.ReplaceAll("reco","gen"));
    ineff.setAllErrorsZero();
    plotterMultiplePlots effplotter;
    effplotter.readStyleFromFileInCMSSW("src/TtZAnalysis/Analysis/configs/topmass/multiplePlots_efficiency.txt");
    c->SetName("efficiency");
    effplotter.usePad(c);
    effplotter.addPlot(&efficiency,false);
    // effplotter.addPlot(&ineff,false);
    effplotter.draw();
    WriteAndPrint(c,f,output,outdirectory);
    effplotter.cleanMem();
    //////////////////////////////////////////////////////////////////
    //end print
    //////////////////////////////////////////////////////////////////
    c->Clear();
    c->Print(output+".pdf)");
    f.Close();
    delete c;

    textFormatter tf;
    if(makepdfs){
        //for debugging
        for(size_t i=0;i<filelist.size();i++){
            TString syscall="epstopdf --outfile="+filelist.at(i)+".pdf "+ filelist.at(i) +".eps";
            system(syscall.Data());
            syscall="rm "+filelist.at(i) +".eps";
            system(syscall.Data());
        }
    }
    return 0;
}
