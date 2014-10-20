/*
 * plotterCompare.cc
 *
 *  Created on: Jan 14, 2014
 *      Author: kiesej
 */


#include "../interface/plotterCompare.h"
#include "TH1.h"
#include "TGraphAsymmErrors.h"
#include <stdexcept>
#include "../interface/fileReader.h"
#include <iostream>
#include "TLegend.h"
#include "TStyle.h"
#include "TLine.h"

namespace ztop{

plotterCompare::plotterCompare(): plotterBase(),
divideat_(0.5),memnom_(0),memratio_(0),size_(0){
	readStyleFromFileInCMSSW("/src/TtZAnalysis/Tools/styles/comparePlots_3MC.txt");
}

void plotterCompare::setNominalPlot(const container1D *c,bool divbw){
    if(debug) std::cout <<"plotterCompare::setNominalPlot" << std::endl;
    nominalplot_.createFrom(c,divbw);
    lastplotidx_=0;
}
void plotterCompare::setComparePlot(const container1D *c,size_t idx,bool divbw){
    if(debug) std::cout <<"plotterCompare::setComparePlot" << std::endl;
    if(idx >= size_){
        throw std::out_of_range("plotterCompare::setComparePlot: plotter configured for less entries");
    }
    if(idx >= compareplots_.size()){
        if(debug) std::cout << "plotterCompare::setComparePlot: increasing compare plots vector" << std::endl;
        compareplots_.resize(idx+1,plot());
    }
    if(debug) std::cout <<"plotterCompare::setComparePlot: setting plot" << std::endl;
    compareplots_.at(idx).createFrom(c,divbw);
    lastplotidx_=compareplots_.size();
}

void plotterCompare::setNominalPlot(const graph *c){
    if(debug) std::cout <<"plotterCompare::setNominalPlot" << std::endl;
    nominalplot_.createFrom(c);
    lastplotidx_=0;
}
void plotterCompare::setComparePlot(const graph *c,size_t idx){
    if(debug) std::cout <<"plotterCompare::setComparePlot" << std::endl;
    if(idx >= size_){
        throw std::out_of_range("plotterCompare::setComparePlot: plotter configured for less entries");
    }
    if(idx >= compareplots_.size()){
        if(debug) std::cout << "plotterCompare::setComparePlot: increasing compare plots vector" << std::endl;
        compareplots_.resize(idx+1,plot());
    }
    if(debug) std::cout <<"plotterCompare::setComparePlot: setting plot" << std::endl;
    compareplots_.at(idx).createFrom(c);
    lastplotidx_=compareplots_.size();
}

/**
 * makes a mem clean, keeps styles
 * keeps input plots (but not ratio plots in mem)
 */
void plotterCompare::cleanMem(){
    if(debug) std::cout <<"plotterCompare::cleanMem" << std::endl;
    plotterBase::cleanMem();

    if(memnom_) delete memnom_;
    memnom_=0;
    if(memratio_){
        memratio_->clear();
        delete memratio_;
        memratio_=0;
    }
}
/**
 * clears plots, clears styles
 */
void plotterCompare::clear(){
    if(debug) std::cout <<"plotterCompare::clear" << std::endl;
    compareplots_.clear();
    compstylesupper_.clear();
    compstylesratio_.clear();
    plotterBase::cleanMem();
    if(memnom_) delete memnom_;
    memnom_=0;
    if(memratio_){
        memratio_->clear();
        delete memratio_;
        memratio_=0;
    }
}
/**
 * clears plots, keeps styles
 */
void plotterCompare::clearPlots(){
    if(debug) std::cout <<"plotterCompare::clearPlots" << std::endl;
    compareplots_.clear();
    plotterBase::cleanMem();
    if(memnom_) delete memnom_;
    memnom_=0;
    if(memratio_){
        memratio_->clear();
        delete memratio_;
        memratio_=0;
    }
}


/**
 * expects entries:
 * [containerStyle - NominalUpper]
 * [containerStyle - CompareUpper]
 * [containerStyle - NominalRatio]
 * [containerStyle - CompareRatio]
 * [plotStyle - Upper]
 * [plotStyle - Ratio]
 * [plotterCompareStyle] ? (TBI)
 *
 * require first style to be fully defined, the rest only differences
 * nominal upper and nominal ratio
 *
 */
void plotterCompare::addStyleFromFile(const std::string& infile){
    readStylePriv(infile,false);
}
void plotterCompare::readStyleFromFile(const std::string& infile){
    readStylePriv(infile,true);
}
void plotterCompare::readStylePriv(const std::string& infile, bool requireall){


    fileReader fr;
    fr.setComment("$");
    fr.setDelimiter(",");
    fr.setStartMarker("[plotterCompare]");
    fr.setEndMarker("[end plotterCompare]");
    fr.readFile(infile);
    fr.setRequireValues(false);
    if(fr.nLines()<1 && requireall){
        std::cout << "plotterCompare::readStyleFromFile: no [plotterCompare] found in " << infile<<std::endl;
        throw std::runtime_error("plotterCompare::readStyleFromFile: no [plotterCompare] found");
    }

    if(requireall){
        divideat_  = fr.getValue<float>("divideat");
        size_      = fr.getValue<size_t>("size");
    }
    else{
        divideat_  = fr.getValue<float>("divideat",divideat_);
        size_      = fr.getValue<size_t>("size",size_);

    }
    if(compids_.size()>1 && compids_.size()<size())
        throw std::runtime_error("plotterCompare::readStyleFromFile: compare plot ids set but trying to plot more plots than ids set");

    if(debug) std::cout <<"plotterCompare::readStyleFromFile: reading plot styles" << std::endl;
    upperstyle_.readFromFile(infile, "Upper",requireall);
    ratiostyle_.readFromFile(infile, "Ratio",requireall);
    if(debug) std::cout <<"plotterCompare::readStyleFromFile: reading nominal styles" << std::endl;
    nomstyleupper_.readFromFile(infile, "NominalUpper"+nominalid_,requireall);
    nomstyleratio_=nomstyleupper_;
    nomstyleratio_.readFromFile(infile, "NominalRatio"+nominalid_,false);


    if(debug) std::cout <<"plotterCompare::readStyleFromFile: reading compare styles" << std::endl;

    compstylesupper_.clear();
    compstylesratio_.clear();
    containerStyle compareupperDefault,compareRatioDefault;
    compareupperDefault.readFromFile(infile,"CompareUpperDefault",requireall);
    compareRatioDefault.readFromFile(infile,"CompareRatioDefault",requireall);


    if(compids_.size()>0 && compids_.size()!=size_){
    	std::cout << "plotterCompare::readStyle: size of compare ids (" <<compids_.size()
    			<< ") must match number of plots to be compared to nominal.(" << size_ << ")"<<std::endl;
    	throw std::out_of_range("plotterCompare::readStyle: size of compare ids must match number of plots to be compared to nominal.");
    }

    for(size_t i=0;i<size_;i++){
        containerStyle temps;
        std::string add;
        if(compids_.size()>1){
            add=compids_.at(i);
        }
        else{
            std::ostringstream oss;
            oss << i;
            add=oss.str();
        }
        temps=compareupperDefault;
        temps.readFromFile(infile, "CompareUpper"+add,false);
        compstylesupper_.push_back(temps);

        temps=compareRatioDefault;
        temps.readFromFile(infile, "CompareRatio"+add,false);
        compstylesratio_.push_back(temps);
    }

    //text boxes

    textboxes_.readFromFile(infile,"boxes");
    legstyle_.readFromFile(infile,"",false);

}




///////protected

void plotterCompare::preparePad(){
    if(debug) std::cout <<"plotterCompare::preparePad" << std::endl;
    //check
    if(compareplots_.size()>compstylesupper_.size()){
        throw std::runtime_error("plotterCompare: too many plots for given number of styles");
    }


    cleanMem();
    TVirtualPad * c = getPad();
    gStyle->SetOptStat(0);
    c->Clear();
    c->Divide(1,2);
    c->cd(1)->SetPad(0,divideat_,1,1);
    c->cd(2)->SetPad(0,0,1,divideat_);
    upperstyle_.applyPadStyle(c->cd(1));
    ratiostyle_.applyPadStyle(c->cd(2));
}
void  plotterCompare::drawPlots(){
    if(debug) std::cout <<"plotterCompare::drawPlots" << std::endl;
    TVirtualPad * c=getPad();

    plotStyle upperstyle=upperstyle_;
    plotStyle ratiostyle=ratiostyle_;
    if(debug) std::cout <<"plotterCompare::drawPlots: absorb scaling" << std::endl;
    upperstyle.absorbYScaling(getSubPadYScale(1));
    ratiostyle.absorbYScaling(getSubPadYScale(2));

    c->cd(1);
    if(! upperstyle.yAxisStyle()->applyAxisRange())
       upperstyle.yAxisStyle()->max=getMaximumUpper();

    drawAllPlots(&upperstyle,&nomstyleupper_,&compstylesupper_,&nominalplot_,&compareplots_,true,false);
    //return;
    //make ratio plots

    makeRatioPlots();

    c->cd(2);
    graph nomgr=nominalplot_.getInputGraph().getRelYErrorsGraph();
    memnom_= new plot(&nomgr);
    if(debug) std::cout <<"plotterCompare::drawPlots: draw ratio plots" << std::endl;
    drawAllPlots(&ratiostyle,&nomstyleratio_,&compstylesratio_,memnom_,memratio_,false,true);


}

void  plotterCompare::drawLegends(){
    if(debug) std::cout <<"plotterCompare::drawLegends" << std::endl;
    getPad()->cd(1);
    TLegend *leg = addObject(new TLegend((Double_t)0.65,(Double_t)0.50,(Double_t)0.95,(Double_t)0.90));
    leg->Clear();
    leg->SetFillStyle(0);
    leg->SetBorderSize(0);
    legstyle_.applyLegendStyle(leg);

    if(std::find(nolegendidx_.begin(),nolegendidx_.end(),0) == nolegendidx_.end() && nomstyleupper_.legendDrawStyle!="none")
    	leg->AddEntry(nominalplot_.getStatGraph(),nominalplot_.getName(), nomstyleupper_.legendDrawStyle);
    for(size_t i=0;i<compareplots_.size() ; i++){
    	if(std::find(nolegendidx_.begin(),nolegendidx_.end(),i+1) != nolegendidx_.end()
    			|| compstylesupper_.at(i).legendDrawStyle =="none") continue;
        leg->AddEntry(compareplots_.at(i).getStatGraph(),compareplots_.at(i).getName(),compstylesupper_.at(i).legendDrawStyle);
    }
    leg->Draw("same");
    tmplegp_=leg;
}

void plotterCompare::drawAllPlots(const plotStyle* ps, const containerStyle * cs, const std::vector<containerStyle>* vcs,
        const plot * nompl, const std::vector<plot>* vcpl, bool nomlast,bool isratio){
    if(debug) std::cout <<"plotterCompare::drawAllPlots" << std::endl;
    //draw axis
    TG * g=0,*gs=0;
    TH1 * axish=0;
    //get axis range from subplots

    axish=addObject(nompl->getInputGraph().getAxisTH1(false,true));

    ps->applyAxisStyle(axish);
    axish->Draw("AXIS");

    if(isratio){
    	axish->GetYaxis()->CenterTitle(true);
    	axish->Draw("AXIS");
    	TLine *line=addObject(new TLine(axish->GetXaxis()->GetBinLowEdge(1), 1, axish->GetXaxis()->GetBinLowEdge(axish->GetNbinsX()),1));
    	line->Draw("same");
    }
    if(!nomlast){
        g=nompl->getStatGraph();
        gs=nompl->getSystGraph();
        cs->applyContainerStyle(g,false);
        cs->applyContainerStyle(gs,true);
        if(cs->sysRootDrawOpt != "none")
            gs->Draw(cs->sysRootDrawOpt+"same");
        g->Draw(cs->rootDrawOpt+"same");
    }
    for(size_t i=vcpl->size();i>0;i--){
        size_t it=i-1;
        if(debug) std::cout << it << " " << vcpl->at(it).getName() << std::endl;
        g=vcpl->at(it).getStatGraph();
        gs=vcpl->at(it).getSystGraph();
        vcs->at(it).applyContainerStyle(g,false);
        vcs->at(it).applyContainerStyle(gs,true);
        if(vcs->at(it).sysRootDrawOpt != "none")
            gs->Draw(vcs->at(it).sysRootDrawOpt+"same");
        g->Draw(vcs->at(it).rootDrawOpt+"same");
    }
    if(nomlast){
        g=nompl->getStatGraph();
        gs=nompl->getSystGraph();
        cs->applyContainerStyle(g,false);
        cs->applyContainerStyle(gs,true);
        if(cs->sysRootDrawOpt != "none")
            gs->Draw(cs->sysRootDrawOpt+"same");
        g->Draw(cs->rootDrawOpt+"same");
    }
}
/**
 * to get rid of y errors or something
 */
plot  plotterCompare::plotterCompareStyle(const plot&p){
    if(debug) std::cout <<"plotterCompare::plotterCompareStyle" << std::endl;
    return p;


}
/**
 * memory needs to be cleaned before (cleanMem() )
 */
void plotterCompare::makeRatioPlots(){
    if(debug) std::cout <<"plotterCompare::makeRatioPlots" << std::endl;
    //////

    graph nomgr=nominalplot_.getInputGraph();
    memratio_=new std::vector<plot>();
    for(size_t i=0;i<compareplots_.size();i++){
        if(debug) std::cout << "plotterCompare::drawPlots: normalize " <<  compareplots_.at(i).getName()  << std::endl;
        graph compg=compareplots_.at(i).getInputGraph();
        compg.normalizeToGraph(nomgr);
        if(debug) std::cout << "plotterCompare::drawPlots: push_back " <<  compareplots_.at(i).getName()  << std::endl;
        memratio_->push_back(plot(&compg));
    }
    nomgr=nomgr.getRelYErrorsGraph();
    memnom_ = new plot(&nomgr);
}


float plotterCompare::getMaximumUpper(){
    float max=-1e20,min=1e20;
    for(size_t i=0;i<compareplots_.size();i++){
        if(compareplots_.at(i).getInputGraph().getYMax() > max)
            max=compareplots_.at(i).getInputGraph().getYMax();
        if(compareplots_.at(i).getInputGraph().getYMin() < min)
            min=compareplots_.at(i).getInputGraph().getYMin();

    }
    if(nominalplot_.getInputGraph().getYMax() > max)
        max=nominalplot_.getInputGraph().getYMax();
    if(nominalplot_.getInputGraph().getYMin() < min)
        min=nominalplot_.getInputGraph().getYMin();
    max=max+fabs((max-min)*0.05);
    return max;
}
}//ns





































