/*
 * containerUnfoldTest.C
 *
 *  Created on: Aug 19, 2013
 *      Author: kiesej
 */


#include "TtZAnalysis/Tools/interface/container1DUnfold.h"
#include "TtZAnalysis/Tools/interface/containerUnfolder.h"
#include "TtZAnalysis/Tools/interface/containerStack.h"
#include "TRandom.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "../interface/plotterCompare.h"
#include "TFile.h"

void divideByBinWidth(TH1* hu){
    for(int i=0;i<hu->GetNbinsX();i++){
        hu->SetBinContent(i, hu->GetBinContent(i)/hu->GetBinWidth(i));
        hu->SetBinError(i, hu->GetBinError(i)/hu->GetBinWidth(i));
    }
}

std::vector<float> subdivide(const std::vector<float> & bins, size_t div){
    std::vector<float> out;
    for(size_t i=0;i<bins.size()-1;i++){
        float width=bins.at(i+1)-bins.at(i);
        if((int)i<(int)bins.size()-2){
            for(size_t j=0;j<div;j++)
                out.push_back(bins.at(i)+j*width/div);
        }
        else{
            for(size_t j=0;j<=div;j++)
                out.push_back(bins.at(i)+j*width/div);
        }
    }
    return out;
}
void drawFullTG(const ztop::container1D * c,int color, int markerstyle,TString opt){
    TH1D *h =c->getTH1D("",true,true);
    h->SetLineColor(color);
    h->SetMarkerColor(color);
    h->SetMarkerStyle(markerstyle);
    TGraphAsymmErrors *g=c->getTGraph("",true,false);
    g->SetLineColor(color);
    g->SetMarkerColor(color);
    g->SetMarkerStyle(markerstyle);
    h->Draw("e1"+opt);
    g->Draw("P,same"+opt);
}

TCanvas * makeFullCheckCanvas(const ztop::container1DUnfold& cont){
    TCanvas *c = new TCanvas();
    //gen

    drawFullTG(&cont.getGenContainer(),kRed,23,"");
    drawFullTG(&cont.getRecoContainer(),kBlack,20,"same");
    drawFullTG(&cont.getUnfolded(),kBlue,23,"same");
    drawFullTG(&cont.getRefolded(),kGreen,34,"same");

    return c;
}

float randcount=1;
void generateAndFill(ztop::container1DUnfold * c,double syst,float nentries,float systshape=1){
    double detRes=0.9;
    double detOff=0.9;
    double effCorr=0.05;
    TRandom * r=new TRandom(123123*nentries*randcount);
    randcount++;
    std::cout << "generating " << nentries << " entries" <<std::endl;
    for(float entry=0;entry<nentries;entry++){
        c->flush();
        double gen=r->BreitWigner(10,4);
        double reco=gen* (1 + r->Gaus(detOff,detRes)/gen); //some resolution and offset effect
        double selcrit=r->Gaus(1,3)-gen*effCorr;
        double weight=1;
        c->fillGen(gen,weight);
        if(selcrit<4) continue;
        weight*=r->Gaus(1,0.5);

        if(reco<14)
            reco*=(1+0.1*reco);
        reco=reco+syst*(1+reco*(systshape-1));
        // std::cout << "fill " << reco << std::endl;
        c->fillReco(reco,weight);
    }


    delete r;
}

void test(){

    using namespace std;
    using namespace ztop;

    bool isMC=true;

    vector<float> genbins,recobins;

    genbins << 0 << 3 << 9 << 11 << 13 << 15 << 18 << 28;
    recobins << 0 << 2 << 4 << 5 << 5.5 << 6 << 6.5 << 7 << 7.5 << 8 << 10 << 11 << 12 << 13 << 14 << 16 << 17 << 18 << 19 << 20 << 21 << 23 << 25 << 28;

    recobins=subdivide(genbins,5);


    recobins=subdivide(genbins,10);

    container1DUnfold::c_makelist=true;
    container1DUnfold::debug = true;

    container1DUnfold tc(genbins,recobins,"nominal","p_{T}","events");
    container1DUnfold tcsup(genbins,recobins,"syst_up","p_{T}","events");
    container1DUnfold tcsdown(genbins,recobins,"syst_down","p_{T}","events");


    tc.setMC(true);
    tcsup.setMC(true);
    tcsdown.setMC(true);

    container1DUnfold tcd=tc;
    tcd.setMC(false);
    tcd.clear();
    container1DUnfold tcdsup=tcsup;
    container1DUnfold tcdsdown=tcsdown;
    tcsup.clear();
    tcsdown.clear();

    float nentries=10e6;
    double norm=0.1;

    //double syst=2.4;

    cout << "generating MC" <<endl;
    generateAndFill(&tc,0,nentries);
    generateAndFill(&tcsup,0.4,nentries);
    generateAndFill(&tcsdown,-0.3,nentries);

    cout << "generating \"data\"" <<endl;
    generateAndFill(&tcd,0,nentries*norm);

    double bgcont=0.9;
    double bgshape=4;
    cout << "generating \"BG\"" <<endl;
    generateAndFill(&tcd,bgshape,nentries*norm*bgcont);
    container1DUnfold tcmcbg=tcd;
    tcmcbg.clear();
    generateAndFill(&tcmcbg,bgshape,nentries*bgcont);

    tcdsup=tcd;
    tcdsdown=tcd;

    //stat independent


    container1DUnfold forfoldtest=tc;
    forfoldtest.clear();
    generateAndFill(&forfoldtest,0,nentries*0.02);
    forfoldtest*=1/0.02; //norm again

    bool systematics=true;
    cout << "adding syst mc" <<endl;
    container1DUnfold tccopywosys=tc;

    container1DUnfold moresys1=tc;
    container1DUnfold moresys2=tc;
    moresys1.clear();
    moresys2.clear();
    generateAndFill(&moresys1,0.03,nentries,0.9);
    generateAndFill(&moresys2,-0.02,nentries,1.2);

    if(systematics){
        tc.addErrorContainer("Sys_up",tcsup);
        tc.addErrorContainer("Sys_down",tcsdown);
        tc.addErrorContainer("Sys2_up",moresys1);
        tc.addErrorContainer("Sys2_down",moresys2);
      //  tc.addGlobalRelError("Lumi",0.05);
      //  tcmcbg.addGlobalRelError("Lumi",0.05);
        tcmcbg.addGlobalRelError("Sys",0.0);

        cout << "adding syst data" <<endl;

        tcd.addErrorContainer("Sys_up",tcdsup);
        tcd.addErrorContainer("Sys_down",tcdsdown);
     //   tcd.addGlobalRelError("Lumi",0.); //trick!
    }



    TCanvas * c=makeFullCheckCanvas(tcd);
    c->Draw();
    c->Print("containerUnfoldTest_stack.pdf(");


    tc.getResponseMatrix().getTH2D()->Draw("colz");
    c->Print("containerUnfoldTest_stack.pdf");

    tc.getNormResponseMatrix().getTH2D()->Draw("colz");
    c->Print("containerUnfoldTest_stack.pdf");
    for(size_t sys=0;sys<tc.getSystSize();sys++){
        tc.getNormResponseMatrix().getTH2DSyst("",sys)->Draw("colz");
        c->Print("containerUnfoldTest_stack.pdf");

    }

    //new "fold" stuff

    container1D simsignal=forfoldtest.getGenContainer();
    simsignal.setName("just for reference: simulation");
    container1D simsignalfolded=tc.fold(simsignal);
    simsignalfolded.setName("simu folded");
    container1D closurefolded=tc.fold(tc.getGenContainer());
    closurefolded.setName("closure folded");
    container1D simsignalreco=tc.getRecoContainer();
    simsignalreco.setName("reco");
    std::cout << "plotting..." <<std::endl;

    plotterCompare plc;
    plotterCompare::debug=true;
    std::string cmsswbase=getenv("CMSSW_BASE");
    plc.readStyleFromFile(cmsswbase+"/src/TtZAnalysis/Tools/styles/comparePlots_3MC.txt");

    plc.setNominalPlot(&simsignalreco,true);
    plc.setComparePlot(&simsignalfolded,0,true);
   // plc.setComparePlot(&closurefolded,1,true);
    //  plc.setComparePlot(&simsignal,1,true);

    plc.usePad(c);



    plc.draw();
    TFile * f=new TFile("containerUnfoldTest_stack.root","RECREATE");
    c->SetName("foldcheck");
    c->Print("containerUnfoldTest_stack.pdf");
    c->Write();
    f->Close();

    containerStack cstack("stack");
    cstack.push_back(tcd,"data",1,1);
    cstack.setLegendOrder("data",2);
    cstack.push_back(tcmcbg,"BG",kBlue,norm);
    cstack.setLegendOrder("BG",1);
    cstack.push_back(tc,"signal",kRed,norm);
    cstack.setLegendOrder("signal",0);



    cout << "doing norm" <<endl;
    tc=tc*norm; //normalize all MC contributions!
    tcsup=tcsup*norm;
    tcsdown=tcsdown*norm;
    tcmcbg=tcmcbg*norm;


    bool manualdraw=false;
    containerUnfolder unfolder,unfolder2;
    containerUnfolder::debug=true;
    cout << "drawing" <<endl;
    if(manualdraw){
        TGraphAsymmErrors* gmccp=tc.getRecoContainer().getTGraph("mc_reco",true,false,false);

        tc.setRecoContainer(tcd.getRecoContainer());
        tc.setBackground(tcmcbg.getRecoContainer());

        unfolder.unfold(tc);
        c=makeFullCheckCanvas(tc);
        c->Draw();
        c->Print("containerUnfoldTest_stack.pdf");
    }

    containerStack::debug=true;
    containerStack::batchmode=false;
    std::vector<TString> signals;signals << "signal";
    container1DUnfold cstackUF =cstack.produceUnfoldingContainer(signals);
    unfolder2.unfold(cstackUF);
    c=makeFullCheckCanvas(cstackUF);
    c->Draw();
    c->Print("containerUnfoldTest_stack.pdf");
    c=cstack.makeTCanvas(containerStack::ratio);
    c->Draw();
    c->Print("containerUnfoldTest_stack.pdf)");
}




void containerUnfoldTest(){
    test();
}
int main(){
    containerUnfoldTest();
    return 0;
}
