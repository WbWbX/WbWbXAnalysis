/*
 * mtExtractor.cc
 *
 *  Created on: Feb 20, 2014
 *      Author: kiesej
 */

#include "../interface/mtExtractor.h"
#include "TtZAnalysis/Tools/interface/container1DUnfold.h"
#include <stdexcept>
#include <string>
#include <cstdlib>
#include <omp.h>

#include "TtZAnalysis/Tools/interface/plotterCompare.h"
#include "TtZAnalysis/Tools/interface/plotterMultiplePlots.h"
#include "TCanvas.h"
#include "TStyle.h"
#include <cmath>
#include "TLine.h"

namespace ztop{

bool mtExtractor::debug=false;

void mtExtractor::setup(){
    std::cout << "inputfiles: " << inputfiles_.size() << std::endl;
    getMtValues();
    readFiles();
    renormalize();

    mergeSyst();

    makeGraphs();
    makeBinGraphs();
    setup_=true;
}
void  mtExtractor::cleanMem(){
    for(size_t i=0;i<pltrptrs_.size();i++){ if(pltrptrs_.at(i)) delete pltrptrs_.at(i);}
    pltrptrs_.clear();
    if(tmpglgraph_)  delete tmpglgraph_;tmpglgraph_=0;
    tObjectList::cleanMem();
}



void mtExtractor::drawXsecDependence(TCanvas *c, bool fordata){
    //read style file heplotterComparere
    using namespace ztop;
    TString add="MC";
    if(fordata) add="data";
    c->SetName("full distribution "+add);
    c->SetTitle("full distribution "+add);

    plotterCompare * pltrptr=0;
    pltrptr = new plotterCompare();
    pltrptrs_.push_back(pltrptr);
    std::vector<std::string> cids_;

    std::vector<graph> * graphs=&mcgraphs_;
    if(fordata) graphs=&datagraphs_;

    for(size_t i=0;i<mtvals_.size();i++){
        if(fabs(defmtop_ - mtvals_.at(i))<0.1) continue;
        std::string mt=toString(mtvals_.at(i));
        mt="_mt"+mt;
        pltrptr->compareIds().push_back(mt);
        if(debug) std::cout << "mtExtractor::drawMCXsecDependence: added mt: " <<mt<<std::endl;

    }
    //read style
    pltrptr->readStyleFromFile(compplotsstylefile_);
    //next loop set plots!
    size_t newidx=0;
    for(size_t i=0;i<mtvals_.size();i++){
        if(fabs(defmtop_ - mtvals_.at(i))<0.1){
            pltrptr->setNominalPlot(&graphs->at(i));
        }
        else{
            pltrptr->setComparePlot(&graphs->at(i),newidx);
            newidx++;
        }
    }
    TVirtualPad* pad=c;
    pltrptr->usePad(pad);
    pltrptr->draw();


}

void mtExtractor::drawIndivBins(TCanvas *c){
    //read style file here
    plotterMultiplePlots plotterdef;
    plotterdef.readStyleFromFile(binsplotsstylefile_);


    c->SetName("cross section per bin");
    c->SetTitle("cross section per bin");


    //make pads
    int vdivs=((int)(sqrt(databingraphs_.size())-0.0001))+1;//+1;
    int hdivs=(int)((float)databingraphs_.size()/(float)vdivs)+1;
    c->Divide(vdivs,hdivs);
    gStyle->SetOptTitle(1);
    for(size_t i=0;i<databingraphs_.size();i++){
        plotterMultiplePlots * pl=new plotterMultiplePlots(plotterdef);
        pltrptrs_.push_back(pl);
        TVirtualPad *p=c->cd(i+1);
        pl->usePad(p);
        graph td=databingraphs_.at(i);
        td.setName("data");
        graph tmc=mcbingraphs_.at(i);
        tmc.setName("MC");
        pl->setTitle(databingraphs_.at(i).getName());
        pl->addPlot(&td);
        pl->addPlot(&tmc);
        pl->draw();
    }



}


void mtExtractor::reset(){
    cleanMem();
    datacont_.clear();
    mccont_.clear();
    datagraphs_.clear();
    mcgraphs_.clear();
    mtvals_.clear();
    databingraphs_.clear();
    mcbingraphs_.clear();
    tmpbinchi2_.clear();
    syspidx_=1;
    allsyst_.clear();
    allsyst_.setNPoints(1);
    allsyst_.addErrorGraph("systatd",graph(1,""));
    allsyst_.addErrorGraph("systatup",graph(1,""));
}


///private functions
double mtExtractor::getNewNorm(double deltam,bool eighttev)const{ //following mitov NNLO paper
    double a1=0;
    double a2=0;

    double mref=defmtop_;
    if(eighttev){
        a1=-1.1125;
        a2=0.070778;
    }
    else{
        a1=-1.24243;
        a2=0.890776;
    }
    double reldm=mref/(mref+deltam);

    double out= (reldm*reldm*reldm*reldm) * (1+ a1*(deltam)/mref + a2*(deltam/mref)*(deltam/mref));
    if(debug) std::cout << "sigmtopmulti deltam: " << deltam << "\tmulti="<< out <<std::endl;
    return out;
}

void mtExtractor::getMtValues(){
    for(size_t i=0;i<inputfiles_.size();i++){
        std::string file=inputfiles_.at(i).Data();
        //cut out the mass value *TeV_<mass>_
        size_t spos=file.find("TeV_")+4;
        size_t endpos=file.find("_",spos);
        file=file.substr(spos,endpos-spos);
        if(debug) std::cout << "mtExtractor::getMtValues: found "  << file << std::endl;
        mtvals_.push_back(atof(file.data()));
    }
}

void mtExtractor::readFiles(){
    if(debug) std::cout << "mtExtractor::readFiles" <<std::endl;
    bool mciscuf=plottypemc_=="cuf";

    for(size_t i=0;i<inputfiles_.size();i++){
        container1DUnfold tempcuf;
        tempcuf.loadFromTFile(inputfiles_.at(i),plotnamedata_);
        if(tempcuf.isDummy())
            throw std::runtime_error("mtExtractor::readFiles: at least one file without container1DUnfold");

        container1D unfolded=tempcuf.getUnfolded();
        container1D gen;
        if(mciscuf){
            gen=tempcuf.getBinnedGenContainer();
            gen*= (1/tempcuf.getLumi());
        }
        else{ //TBI
            throw std::runtime_error("mtExtractor::readFiles: other mc input not supported yet");
            //needs to use inputfilesmc and convert to ontainer from some format, renorm?!?
        }
        datacont_.push_back(unfolded);
        mccont_.push_back(gen);
    }
    if(datacont_.size() <1 || mccont_.size() < 1){
        throw std::runtime_error("mtExtractor::readFiles: no input distributions found");
    }

}
void mtExtractor::renormalize(){
    if(debug) std::cout << "mtExtractor::renormalize" <<std::endl;
    if(plottypemc_!=(TString)"cuf")
        throw std::runtime_error("mtExtractor::renormalize: other mc input not supported yet");

    for(size_t i=0;i<mccont_.size();i++){
        float renorm= getNewNorm(mtvals_.at(i)-defmtop_,iseighttev_);
        mccont_.at(i) *=renorm;
        if(debug) std::cout << "mtExtractor::renormalize: mt: " <<  mtvals_.at(i) << " sf: " << renorm << std::endl;
    }


}
void mtExtractor::mergeSyst(){

    //add from all
    if(debug) std::cout <<"mtExtractor::mergeSyst: MC" << std::endl;
    for(size_t i=0;i<mccont_.size();i++){
        for(size_t j=0;j<mccont_.size();j++){
            if(i==j) continue;
            mccont_.at(i).addRelSystematicsFrom(mccont_.at(j));
        }
    }
    if(debug) std::cout <<"mtExtractor::mergeSyst: data" << std::endl;
    for(size_t i=0;i<datacont_.size();i++){
        for(size_t j=0;j<datacont_.size();j++){
            if(i==j) continue;
            datacont_.at(i).addRelSystematicsFrom(datacont_.at(j));
        }
    }

}
//just transforms to graph
void mtExtractor::makeGraphs(){
    for(size_t i=0;i<mccont_.size();i++){
        graph tempgraph;
        tempgraph.import(&mccont_.at(i),true);
        tempgraph.setName("m_{t}="+toTString(mtvals_.at(i)));
        tempgraph.sortPointsByX();
        mcgraphs_.push_back(tempgraph);
    }
    mccont_.clear();
    if(debug) std::cout << "mtExtractor::makeGraphs: created "<< mcgraphs_.size() << " MC graphs" << std::endl;
    for(size_t i=0;i<datacont_.size();i++){
        graph tempgraph;
        tempgraph.import(&datacont_.at(i),true);
        tempgraph.setName("m_{t}="+toTString(mtvals_.at(i)));
        tempgraph.sortPointsByX();
        datagraphs_.push_back(tempgraph);
    }
    datacont_.clear();
    if(debug) std::cout << "mtExtractor::makeGraphs: created "<< datagraphs_.size() << " data graphs" << std::endl;

}

graph mtExtractor::makeDepInBin(const std::vector<graph> & graphs, size_t bin)const{
    graph massdep=graph(mtvals_.size());
    massdep.setXAxisName("m_{t} [GeV]");
    massdep.setYAxisName(graphs.at(0).getYAxisName());
    TString newname;
    for(size_t mtit=0;mtit<mtvals_.size();mtit++){
        const graph & g= graphs.at(mtit);
        if(mtit==0){
            //gives bin width
            float min=g.getPointXContent(bin) - g.getPointXError(bin,true);
            float max=g.getPointXContent(bin) + g.getPointXError(bin,true);
            newname = g.getXAxisName()+": "+toTString(min) +"-" + toTString(max);
            if(debug) std::cout << "mtExtractor::makeDepInBin: new graph name: "
                    << newname << std::endl;
        }
        const float & mtval=mtvals_.at(mtit);
        for(int sys=-1;sys<(int)g.getSystSize();sys++){
            int innersys=-1;
            if(sys>-1){
                massdep.getXCoords().addLayer(g.getSystErrorName(sys));
                innersys=massdep.getYCoords().addLayer(g.getSystErrorName(sys));
            }
            float newval=g.getPointYContent(bin,sys);
            float newstat=g.getPointYStat(bin,sys);
            massdep.setPointYContent(mtit,newval,innersys);
            massdep.setPointXContent(mtit,mtval,innersys);
            massdep.setPointYStat(mtit,newstat,innersys);
            massdep.setPointXStat(mtit,0,innersys);
        }
    }


    massdep.sortPointsByX();
    return massdep;
}


void mtExtractor::makeBinGraphs(){
    //keep in mind: The points of each graph are properly sorted by X
    //So no fancy extra bin finding anymore!
    // But be careful aith the ordering of systematics in graphs

    //create graph with right amount of points, assume same for MC and data


    for(size_t bin=0;bin<datagraphs_.at(0).getNPoints();bin++){
        databingraphs_.push_back(makeDepInBin(datagraphs_,bin));
    }
    if(debug) std::cout << "mtExtractor::makeBinGraphs: created "<< databingraphs_.size()<< " data graphs" << std::endl;
    for(size_t bin=0;bin<mcgraphs_.at(0).getNPoints();bin++){
        mcbingraphs_.push_back(makeDepInBin(mcgraphs_,bin));
    }
    if(debug) std::cout << "mtExtractor::makeBinGraphs: created "<< mcbingraphs_.size() << "  mc graphs" << std::endl;

}
void mtExtractor::createBinLikelihoods(int syslayer,bool includesyst){
    //get graphs for syst.
    if(includesyst){
        paraExtr_.setInputA(databingraphs_);
        paraExtr_.setInputB(mcbingraphs_);
        tmpSysName_="all syst.";
    }
    else{ //create inputs
        if(syslayer>=(int)databingraphs_.at(0).getSystSize())
            throw std::out_of_range("mtExtractor::createBinLikelihoods: out of range");
        //check for same variation in MC
        tmpSysName_=databingraphs_.at(0).getSystErrorName(syslayer);
        std::vector<graph> dgs,mcgs;
        for(size_t i=0;i<databingraphs_.size();i++){
            if(syslayer>-1){
                size_t mcsysidx=mcbingraphs_.at(i).getSystErrorIndex(databingraphs_.at(i).getSystErrorName(syslayer));
                if(mcsysidx<mcbingraphs_.at(i).getSystSize()){
                    mcgs.push_back(mcbingraphs_.at(i).getSystGraph(mcsysidx));
                }
                else{
                    mcgs.push_back(mcbingraphs_.at(i));
                }
                dgs.push_back(databingraphs_.at(i).getSystGraph((size_t)syslayer));
            }
            else{ //nominal
                mcgs.push_back(mcbingraphs_.at(i));
                dgs.push_back(databingraphs_.at(i).getNominalGraph());
            }
        }
        paraExtr_.setInputA(dgs);
        paraExtr_.setInputB(mcgs);
    }
    //input prepared

    tmpbinchi2_=paraExtr_.createLikelihoods();
    if(debug) std::cout << "mtExtractor::createBinLikelihoods: created." <<std::endl;



}
void mtExtractor::drawBinLikelihoods(TCanvas *c){
    plotterMultiplePlots plotterdef;
    plotterdef.readStyleFromFile(binschi2plotsstylefile_);
    c->SetName("#chi^{2} per bin "+tmpSysName_);
    c->SetTitle("#chi^{2} per bin "+tmpSysName_);
    //make pads
    int vdivs=((int)(sqrt(tmpbinchi2_.size())-0.0001))+1;//+1;
    int hdivs=(int)((float)tmpbinchi2_.size()/(float)vdivs)+1;
    c->Divide(vdivs,hdivs);
    gStyle->SetOptTitle(1);
    for(size_t i=0;i<tmpbinchi2_.size();i++){
        plotterMultiplePlots * pl=new plotterMultiplePlots(plotterdef);
        pltrptrs_.push_back(pl);
        TVirtualPad *p=c->cd(i+1);
        pl->usePad(p);
        graph td=tmpbinchi2_.at(i);
        pl->setTitle(databingraphs_.at(i).getName());
        pl->addPlot(&td);
        pl->draw();
    }

}

void mtExtractor::createGlobalLikelihood(){
    if(tmpbinchi2_.size()<1)
        throw std::logic_error("mtExtractor::createGlobalLikelihood: first create (non empty) likelihoods for bins");
    //just add upp everything
    tmpglchi2_=tmpbinchi2_.at(0);
    gStyle->SetOptTitle(1);
    for(size_t i=1;i<tmpbinchi2_.size();i++)
        tmpglchi2_=tmpglchi2_.addY(tmpbinchi2_.at(i));

    if(tmpglgraph_)  {delete tmpglgraph_;tmpglgraph_=0;}
    tmpglgraph_=(tmpglchi2_.getTGraph());

}
void mtExtractor::drawGlobalLikelihood(TCanvas *c,bool zoom){


    if(!tfitf_) throw std::logic_error("mtExtractor::drawGlobalLikelihood: first fitGlobalLikelihood");

    c->SetName("global #chi^{2} "+tmpSysName_);
    c->SetTitle("global #chi^{2} "+tmpSysName_);

    plotterMultiplePlots * pl=new plotterMultiplePlots; //FIXME use a fancy fitting stuff here TBI
    pltrptrs_.push_back(pl);
    pl->readStyleFromFile(binschi2plotsstylefile_);
    pl->usePad(c);
    pl->setTitle("global "+tmpSysName_);
    pl->addPlot(&tmpglchi2_);
    pl->draw();

    float tmpglchi2_minimum=0;
    tfitf_->Draw("same");
    tmpglchi2_minimum=tfitf_->GetMinimum(tmpglchi2_.getXMin(),tmpglchi2_.getXMax());
    std::cout << "minimum: " << tmpglchi2_minimum <<std::endl;
    float plotmin=tmpglchi2_minimum-1;

    float epsilon=(tmpglchi2_.getXMax()-tmpglchi2_.getXMin()/5);
    float Xminimum=tfitf_->GetX(tmpglchi2_minimum,tmpglchi2_.getXMin(),tmpglchi2_.getXMax());
    float xpleft=tfitf_->GetX(tmpglchi2_minimum+1,tmpglchi2_.getXMin(),Xminimum);
    float xpright=tfitf_->GetX(tmpglchi2_minimum+1,Xminimum,tmpglchi2_.getXMax());
    TLine * xlinel=new TLine(xpleft,plotmin,xpleft,tmpglchi2_minimum+1);
    TLine * xliner=new TLine(xpright,plotmin,xpright,tmpglchi2_minimum+1);
    TLine * xlinec=new TLine(Xminimum,plotmin,Xminimum,tmpglchi2_minimum);
    TLine * yline=new TLine(tmpglchi2_.getXMin(),tmpglchi2_minimum+1,xpleft,tmpglchi2_minimum+1);
    TLine * yline2=new TLine(tmpglchi2_.getXMin(),tmpglchi2_minimum,Xminimum,tmpglchi2_minimum);
    yline->Draw("same");
    yline2->Draw("same");
    xlinel->Draw("same");
    xliner->Draw("same");
    xlinec->Draw("same");

 //  size_t pidx= allsyst_.addPoint(Xminimum,syspidx_,tmpSysName_);
 //  allsyst_.setPointXContent(pidx,xpleft,0);
  // allsyst_.setPointXContent(pidx,xpright,1);

}

void mtExtractor::fitGlobalLikelihood(){
    if(!tmpglgraph_)
        throw std::logic_error("mtExtractor::fitGlobalLikelihood: need to create global likelihood before fitting it");


    if(tfitf_) {delete tfitf_; tfitf_=0;}

    tfitf_ = new TF1("f1",fitmode_,tmpglchi2_.getXMin(),tmpglchi2_.getXMax());
    tmpglgraph_->Fit("f1","VR");




}

}
