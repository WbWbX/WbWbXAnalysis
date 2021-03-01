/*
 * TtBarUnfolder.cc
 *
 *  Created on: Feb 21, 2014
 *      Author: kiesej
 */

#include "../interface/TtBarUnfolder.h"

#include "WbWbXAnalysis/Tools/interface/histoUnfolder.h"
#include "WbWbXAnalysis/Tools/interface/histo1DUnfold.h"
#include "WbWbXAnalysis/Tools/interface/histoStackVector.h"
#include "WbWbXAnalysis/Tools/interface/histoStack.h"
#include "WbWbXAnalysis/Tools/interface/plotterControlPlot.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "TString.h"
#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TCanvas.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"


namespace ztop{

void TtBarUnfolder::setNameAndTitle(TCanvas *c, TString name)const{
    c->SetTitle(name);
    c->SetName(name);
}

TString TtBarUnfolder::unfold(TString out,TString in)const{
    using namespace ztop;

    bool moreoutput=verbose;
    TString outputstring;

    TH1::AddDirectory(false);
    AutoLibraryLoader::enable();
    bool debug=true;

    if(!in.EndsWith(".root"))
        in.Append(".root");
    TString outfile=in;
    outfile.ReplaceAll(".root","");
    outfile+="_"+out;
    TString outdir=outfile+"_unfolded/";
    outfile+="_unfolded.root";

    if(moreoutput){
        ztop::histoUnfolder::debug=true;
        ztop::histo2D::debug=false;
        ztop::histo1DUnfold::debug=true;
        ztop::histoStack::debug=false;
    }
    ztop::histoUnfolder::printinfo = moreoutput;


    /*
     * configure output and options END
     *
     */


    if(printpdfs)system(("mkdir -p "+outdir).Data());




    TString csvname=in;
    csvname.ReplaceAll("_plots.root","");
    csvname.ReplaceAll(".root","");

    ztop::histoStackVector csv;
    csv.loadFromTFile(in);


    //get right stacks and set signal
    size_t csvsize=csv.getVector().size();
    std::vector<ztop::histoStack> tobeunfolded;
    for(size_t i=0; i<csvsize;i++){
        ztop::histoStack  stack=csv.getStack(i);
        if(stack.is1DUnfold()){
            tobeunfolded.push_back(stack);
        }
    }
    //clear mem
    csv.clear();


    ///unfolding and output part!!

    ztop::histoUnfolder unfolder;
    //unfolder.setRegMode(TUnfold::kRegModeDerivative);
    unfolder.setScanMode(ztop::histoUnfolder::minCorr);

    size_t ufsize=tobeunfolded.size();
    //parallel for?!?
    //  TFile* f = new TFile(outdir+outfile,"RECREATE");
    TFile* f = new TFile(outfile,"RECREATE");
    csv.writeAllToTFile(f); //write back to have full info
    f->cd();
    TDirectory * d=0;
    for(size_t i=0;i<ufsize;i++){
        unfolder.clear();
        ztop::histoStack * stack=&tobeunfolded.at(i);

        TString name=stack->getName();
        TString nameus=name;
        nameus.ReplaceAll(" ","_");
        f->cd();
        d = f->mkdir(nameus,nameus);
        d->cd();
        TString outdirin=outdir+nameus+"/";
        if(printpdfs) system(("mkdir -p "+outdirin).Data());

        ztop::histo1DUnfold  data=stack->produceUnfoldingContainer();
        if(bbb_)
        	data.setBinByBin(true);

        bool correctbr=false;
        if(brcorr_>0){
            for(size_t i=0;i<plotids_.size();i++){
                std::cout << data.getName() <<std::endl;
                if(data.getName().Contains(plotids_.at(i))){
                    correctbr=true;
                    std::cout << "Will rescale plot " << data.getName()  << " with factor " << brcorr_ <<std::endl;
                    break;
                }
            }
        }


        std::cout << "unfolding " << data.getName() << " syst: " << std::endl;
        unfolder.unfold(data);

        if(correctbr)
            data*=1/brcorr_;


        ztop::histo1D unfolded=data.getUnfolded();
        ztop::histo1D refolded=data.getRefolded();



        unfolded.setName(data.getName()+"_unfolded");
        unfolded.writeToTFile(f);
        refolded.setName(data.getName()+"_refolded");
        refolded.writeToTFile(f);
        data.writeToTFile(f);


        d->cd();
        TCanvas * c = new TCanvas(name+"_unfolded",name+"_unfolded");
        c->Draw();
        c->cd(1)->SetBottomMargin(0.15);
        c->cd(1)->SetLeftMargin(0.15);
        unfolded.drawFullPlot();
        if(unfolded.getNBins() < 6){
            std::cout << "\n\n" << name << std::endl;
            outputstring+=name+"\n\n";
            for(size_t bin=1;bin<=unfolded.getNBins();bin++){
                //unfolded.coutBinContent(bin);
                outputstring+=data.coutUnfoldedBinContent(bin,units_); //dangerous
                outputstring+="\n\n";
                std::cout << "\n\n"  << std::endl;
            }

        }
        d->cd();
        //c->Write();
        d->WriteTObject(c,c->GetName());
        TString pdfname=outdirin+name+"_unfolded.pdf";
        pdfname.ReplaceAll(" ","_");
        if(printpdfs) c->Print(pdfname);
        c->Clear();
        delete c;


        histo1D unfoldednormd=unfolded;
        unfoldednormd.normalize(true,true);
        unfoldednormd.setYAxisName("1/#sigma "+unfoldednormd.getYAxisName());
        c=new TCanvas();
        c->cd(1)->SetBottomMargin(0.15);
        c->cd(1)->SetLeftMargin(0.15);
        setNameAndTitle(c,name+"_unfolded_norm");
        unfoldednormd.drawFullPlot("",true,"");
        d->cd();
        //c->Write();
        d->WriteTObject(c,c->GetName());




        c->Clear();
        TDirectory * histdir=d->mkdir("hists","hists");
        TGraphAsymmErrors * singleg=unfolded.getTGraph();
        histdir->WriteTObject(singleg,singleg->GetName());
        delete singleg;
        d->cd();


        ///use new compare plotters here //FIXME
        setNameAndTitle(c,name+"_refolded_recoBG");
        refolded.drawFullPlot();
        ztop::histo1D reco=data.getRecoContainer();
        //ztop::histo1D recmbg=reco-data.getBackground();
        reco.drawFullPlot("",true,"same");
        data.getBackground().drawFullPlot("",true,"same");
        d->cd();
        //c->Write();
        d->WriteTObject(c,c->GetName());
        pdfname=outdirin+name+"_refolded_recoBG.pdf";
        pdfname.ReplaceAll(" ","_");
        if(printpdfs) c->Print(pdfname);

        if(moreoutput)
            std::cout << "making control plot" << std::endl;
        plotterControlPlot pl;
        pl.usePad(c);
        pl.setStack(stack);
        pl.draw();
        d->WriteTObject(c,((TString)c->GetName()+"_cplot").Data());
        delete c;

        if(moreoutput)
            std::cout << "making data-bg plot" << std::endl;
        histo1D dminusBG=data.getRecoContainer();
        dminusBG-=data.getBackground();
        c=new TCanvas();
        c->cd(1)->SetBottomMargin(0.15);
        c->cd(1)->SetLeftMargin(0.15);
        setNameAndTitle(c,name+"_data-BG");
        dminusBG.drawFullPlot("",true,"");
        d->cd();
        //c->Write();
        d->WriteTObject(c,c->GetName());
        delete c;

        c=new TCanvas();
        c->cd(1)->SetBottomMargin(0.15);
        c->cd(1)->SetLeftMargin(0.15);
        setNameAndTitle(c,name+"_data-BG_norm");
        dminusBG.normalize(true,true);
        dminusBG.setYAxisName("1/N_{tot} "+dminusBG.getYAxisName());
        dminusBG.drawFullPlot("",true,"");
        d->cd();
        //c->Write();
        d->WriteTObject(c,c->GetName());
        delete c;

        if(moreoutput)
            std::cout << "making stab/pur plot" << std::endl;
        c=new TCanvas();
        c->cd(1)->SetBottomMargin(0.15);
        c->cd(1)->SetLeftMargin(0.15);
        setNameAndTitle(c,name+"_pur_stab");
        ztop::histo1DUnfold cuf=stack->getSignalContainer1DUnfold();
        cuf.checkCongruentBinBoundariesXY();
        TH1D * pur=cuf.getPurity().getTH1D("purity",false,false,false);
        TH1D * stab=cuf.getStability().getTH1D("stability",false,false,false);
        TLegend * leg=0;
        if(pur){
            stab->SetLineColor(kRed);
            stab->SetMarkerColor(kRed);
            pur->GetYaxis()->SetRangeUser(0,1);
            pur->Draw();
            stab->Draw("same");
            leg=new TLegend((Double_t)0.65,(Double_t)0.50,(Double_t)0.95,(Double_t)0.90);
            leg->AddEntry(pur,"purity","ep");
            leg->AddEntry(stab,"stability","ep");
            leg->Draw("same");

        }
        d->WriteTObject(c,((TString)c->GetName()+"_purStab").Data());
        if(leg) delete leg;

        ///
        if(moreoutput)
            std::cout << "making regularization plots" << std::endl;
        if(!data.isBinByBin()){

            ///lcurve plots
            c=new TCanvas();
            setNameAndTitle(c,name+"_lcurve");
            unfolder.getNominalUnfolder()->drawLCurve();
            d->WriteTObject(c,c->GetName());
            c->Clear();

            setNameAndTitle(c,name+"_tau");
            unfolder.getNominalUnfolder()->drawTau();
            d->WriteTObject(c,c->GetName());
            /////check distributions
            c->Clear();

            //all syst curves:

            TDirectory * indir=d->mkdir("sys","sys");
            indir->cd();
            for(size_t sys=0;sys<data.getSystSize();sys++){
                setNameAndTitle(c,name+"_"+data.getSystErrorName(sys)+"_tau");
                unfolder.getSystUnfolder().at(sys)->drawTau();
                indir->WriteTObject(c,c->GetName());
                /////check distributions
                c->Clear();
                setNameAndTitle(c,name+"_"+data.getSystErrorName(sys)+"_lcurve");
                unfolder.getSystUnfolder().at(sys)->drawLCurve();
                indir->WriteTObject(c,c->GetName());
                /////check distributions
                c->Clear();

            }
        }

        ////////make cross check
        unfolder.clear();
        std::cout << "Making XCheck for " << name << std::endl;
        d->cd();
        setNameAndTitle(c,name+"_checkUnfoldGen.pdf");

        ztop::histo1DUnfold  check=stack->produceXCheckUnfoldingContainer();
        if(bbb_)
        	check.setBinByBin(true);

        check.setName(check.getName()+"_genToGen");
        unfolder.unfold(check);
        ztop::histo1D checkunfolded=check.getUnfolded();
        float max=checkunfolded.getYMax(true);
        ztop::histo1D generated=check.getBinnedGenContainer();
        generated *= (1/check.getLumi());
        check.writeToTFile(f);
        if(max < generated.getYMax(true)) max=generated.getYMax(true);
        TH1D * ufgen=checkunfolded.getTH1D("genUF",true,true);
        TH1D*   gen =generated.getTH1D("gen",true,true);
        gen->SetMarkerColor(kRed);
        ufgen->GetYaxis()->SetRangeUser(0,max*1.1);
        ufgen->Draw();
        gen->Draw("same");
        if(debug){
            pdfname=outdirin+name+"_checkUnfoldGen.pdf";
            pdfname.ReplaceAll(" ","_");
            if(printpdfs) c->Print(pdfname);
        }
        d->cd();
        //c->Write();
        d->WriteTObject(c,c->GetName());


        delete c;
        histo1D refoldedgen=check.getBinnedGenContainer();;
        data.fold(refoldedgen);
        c=new TCanvas();
        c->cd(1)->SetBottomMargin(0.15);
        c->cd(1)->SetLeftMargin(0.15);
        setNameAndTitle(c,name+"_genrefolded");
        refoldedgen.drawFullPlot("",true,"");
        d->cd();
        //c->Write();
        d->WriteTObject(c,c->GetName());
        delete c;


        refoldedgen.normalize(true,true);
        refoldedgen.setYAxisName("1/N_{tot} "+unfoldednormd.getYAxisName());
        c=new TCanvas();
        c->cd(1)->SetBottomMargin(0.15);
        c->cd(1)->SetLeftMargin(0.15);
        setNameAndTitle(c,name+"_genrefolded_norm");
        refoldedgen.drawFullPlot("",true,"");
        d->cd();
        //c->Write();
        d->WriteTObject(c,c->GetName());


        d->Close();
        delete d;
        delete ufgen;
        delete gen;
        delete pur;
        delete stab;

    }
    f->Close();
    delete f;
    return outputstring;


}


}


