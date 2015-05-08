/*
 * plotterMultiStack.cc
 *
 *  Created on: May 4, 2015
 *      Author: kiesej
 */




#include "../interface/plotterMultiStack.h"
#include "../interface/histoStack.h"
#include "../interface/fileReader.h"

namespace ztop{

plotterMultiStack::plotterMultiStack():plotterBase(), corrm_(0),globalleg_(0)/*,bottommarg_(.1),topmarg_(.1),leftmarg_(.1),rightmarg_(.1)*/{
	readStyleFromFileInCMSSW("/src/TtZAnalysis/Tools/styles/plotterMultiStack_standard.txt");
}

void plotterMultiStack::preparePad(){
	TVirtualPad * p=getPad();
	p->Clear();


	size_t cols=stacks_.size();

	p->Divide(cols,1);

	int w=0;//p->GetWw() * (int)plotter_.size();
	float totallength=0;
	for(size_t i=0;i<separators_.size();i++){
		w+=separators_.at(i)*p->GetWw();
		totallength+=separators_.at(i);
	}
	int h=p->GetWh();
	p->SetCanvasSize(w,h);


	p->cd(1)->SetPad(0,0,separators_.at(0)/totallength,1);
	float sepsum=separators_.at(0)/totallength;
	for(size_t i=1;i<separators_.size()-1;i++){
		p->cd(i+1)->SetPad(sepsum,0,sepsum+separators_.at(i)/totallength,1);
		sepsum+=separators_.at(i)/totallength;
	}
	p->cd(separators_.size())->SetPad(sepsum,0,1,1);




}
void plotterMultiStack::drawPlots(){

	if(stacks_.size()!=plotter_.size())
		throw std::out_of_range("plotterMultiStack::drawPlots: less or more plots to draw then specified in config");

	if(yaxisconnects_.size()>0){
		float  ymaxconnect=-100,yminconnect=1e20;
		for(size_t i=0;i<yaxisconnects_.size();i++){
			bool dibw=plotter_.at(yaxisconnects_.at(i))->getUpperStyle().divideByBinWidth;
			for(size_t j=0;j<yaxisconnects_.size();j++){
				if(dibw != plotter_.at(yaxisconnects_.at(i))->getUpperStyle().divideByBinWidth)
					throw std::out_of_range("plotterMultiStack::drawPlots: try to connect divbyBW and not divByBW axis");
			}
			float tmpmax=stacks_.at(yaxisconnects_.at(i))->getYMax(dibw);
			if(tmpmax>ymaxconnect)ymaxconnect=tmpmax;
			float tmpmin=stacks_.at(yaxisconnects_.at(i))->getYMin(dibw,true); //only data entries are important
			if(tmpmin && tmpmin<yminconnect)yminconnect=tmpmin; //ignore if zero
		}

		axisStyle* axst= plotter_.at(yaxisconnects_.at(0))->getUpperStyle().yAxisStyle(); //copy from first

		if(axst->applyAxisRange()){
			ymaxconnect=axst->max;
			yminconnect=axst->min;
		}
		else{
			if(axst->log)
				ymaxconnect*=2;
			else
				ymaxconnect*=1.1;
			axst->max=ymaxconnect;
			if(axst->log){
				if(yminconnect==0)
					yminconnect+=(10/ymaxconnect);
				axst->min=yminconnect;
			}
			else
				axst->min=0.01;//in linear it should be zero
		}

		if(debug) std::cout << "plotterMultiStack::drawPlots: connecting ";

		for(size_t i=1;i<yaxisconnects_.size();i++){
			if(debug) std::cout <<yaxisconnects_.at(i)<<" ";
			plotter_.at(yaxisconnects_.at(i))->getUpperStyle().yAxisStyle()->log=axst->log;
			plotter_.at(yaxisconnects_.at(i))->getUpperStyle().yAxisStyle()->max=axst->max;
			plotter_.at(yaxisconnects_.at(i))->getUpperStyle().yAxisStyle()->min=axst->min;
		}
		if(debug) std::cout << " with max: "<< ymaxconnect<<std::endl;
	}

	for(size_t i=0;i<plotter_.size();i++){
		TVirtualPad * p=getPad()->cd((int)(i+1));
		plotter_.at(i)->associateCorrelationMatrix(*corrm_);
		plotter_.at(i)->usePad(p);


		plotter_.at(i)->setStack(stacks_.at(i));

		//check margins
		float xmin,xmax;
		xmin=plotter_.at(i)->getXAxisLowerLimit();
		xmax=plotter_.at(i)->getXAxisHigherLimit();
		if(plotter_.at(i)->getRatioStyle().leftMargin < 0.05){
			xmin+=(xmax-xmin)/190;
		}
		if(plotter_.at(i)->getRatioStyle().rightMargin < 0.05){
			xmax-=(xmax-xmin)/190;
		}
		plotter_.at(i)->getRatioStyle().xAxisStyle()->max=xmax;
		plotter_.at(i)->getRatioStyle().xAxisStyle()->min=xmin;



		plotter_.at(i)->setDrawLegend(false);
		if(debug)
			std::cout << "plotterMultiStack::drawPlots: drawing stack " << i << std::endl;
		plotter_.at(i)->draw();
	}

}
void plotterMultiStack::drawLegends(){
	if(plotter_.size()<1)
		return;
	globalleg_=plotter_.at(0)->getLegend();
	legstyle_.applyLegendStyle(globalleg_);
	getPad()->cd(0);
	legstyle_.applyLegendStyle(globalleg_);
	globalleg_->Draw("same");
}
void plotterMultiStack::refreshPad(){
	//done by control plotters
}

void plotterMultiStack::printToPdf(const std::string& outname){
	TVirtualPad * oldpad=0;
	try{
		oldpad=getPad();
	}catch(...){}
	TCanvas c;


	usePad(&c);
	draw();
	TString outnameeps=outname;
	outnameeps+=".eps";
	c.Print(outnameeps);
	TString syscall="epstopdf --outfile="+outname+".pdf " + outnameeps;
	system(syscall);
	TString delcall="rm -f "+outnameeps;
	system(delcall);
	usePad(oldpad);
}

void plotterMultiStack::addStack(const histoStack *s){
	if(!s)
		throw std::logic_error("plotterMultiStack::addStack: pointer NULL");
	if(stacks_.size() >= plotter_.size())
		throw std::logic_error("plotterMultiStack::addStack: not set up for that many plots (check style file)");

	stacks_.push_back(s);

}



void plotterMultiStack::addStyleFromFile(const std::string& in){
	// FIXME readStylePriv(in,false);
}
void plotterMultiStack::readStyleFromFile(const std::string& in ){
	readStylePriv(in,true);
}

void plotterMultiStack::readStylePriv(const std::string& infile,bool requireall){
	fileReader fr;
	fr.setComment("$");
	fr.setDelimiter(",");
	fr.setStartMarker("[plotterMultiStack]");
	fr.setEndMarker("[end plotterMultiStack]");
	fr.readFile(infile);
	fr.setRequireValues(false);
	if(requireall)
		stylefile_=fr.getValue<std::string>("controlPlotStyleFile");
	else
		stylefile_=fr.getValue<std::string>("controlPlotStyleFile",stylefile_);
	if(!fileExists((getenv("CMSSW_BASE")+(std::string)"/"+stylefile_).data())){
		std::string err="plotterMultiStack::readStylePriv: style file ";
		err+=stylefile_;
		err+=" does not exist";
		throw std::logic_error(err);
	}

	size_t nplots=fr.getValue<size_t>("nplots");
	plotter_.clear();
	separators_.clear();
	for(size_t i=0;i<nplots;i++){
		std::string stylefile=fr.getValue<std::string>("addStyle"+toString(i),stylefile_);
		plotterControlPlot *pl=new plotterControlPlot();
		pl->readStyleFromFile(getenv("CMSSW_BASE")+(std::string)"/"+stylefile_);
		//std::cout << stylefile << std::endl;
		pl->addStyleFromFile(getenv("CMSSW_BASE")+(std::string)"/"+stylefile);
		pl->removeTextBoxes();
		plotter_.push_back(pl);

		float frac=fr.getValue<float>("frac"+toString(i),1);
		separators_.push_back(frac);
	}
	if(requireall)
		textboxes_.clear();
	textboxes_.readFromFile(infile,"boxes");
	legstyle_.readFromFile(infile,"",requireall);


	std::string connects=fr.getValue<std::string>("connectUpperYAxis","");
	textFormatter fmt;
	fmt.setDelimiter("|");
	std::vector<std::string> cntc=fmt.getFormatted(connects);
	if(cntc.size()>nplots)
		throw std::out_of_range("plotterMultiStack::readStylePriv: trying to connect more axis than plots defined");
	std::vector<size_t> cntv;
	for(size_t i=0;i<cntc.size();i++){
		size_t newentr=atoi(cntc.at(i).data());
		if(newentr>=nplots)
			throw std::out_of_range("plotterMultiStack::readStylePriv: trying to connect plots that are out of range");
		cntv.push_back(newentr);
	}
	yaxisconnects_=cntv;
}


void plotterMultiStack::cleanMem(){
	stacks_.clear(); //anyway live somewhere else
	for(size_t i=0;i<plotter_.size();i++)
		if(plotter_.at(i))delete plotter_.at(i);
	plotter_.clear(); //calls destructors

}

}
