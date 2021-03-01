/*
 * plotterMultiColumn.cc
 *
 *  Created on: May 4, 2015
 *      Author: kiesej
 */




#include <WbWbXAnalysis/Tools/interface/plotterMultiColumn.h>
#include "../interface/histoStack.h"
#include "../interface/fileReader.h"

namespace ztop{

plotterMultiColumn::plotterMultiColumn():plotterBase(), corrm_(0),globalleg_(0),resolutionmultiplier_(1.5)/*,bottommarg_(.1),topmarg_(.1),leftmarg_(.1),rightmarg_(.1)*/{

}

void plotterMultiColumn::preparePad(){
	TVirtualPad * p=getPad();
	p->Clear();


	size_t cols=plotter_.size();

	p->Divide(cols,1);
	float totallength=0;
	for(size_t i=0;i<separators_.size();i++){
		totallength+=separators_.at(i);
	}
	p->cd(1)->SetPad(0,0,separators_.at(0)/totallength,1);
	float sepsum=separators_.at(0)/totallength;
	for(size_t i=1;i<separators_.size()-1;i++){
		p->cd(i+1)->SetPad(sepsum,0,sepsum+separators_.at(i)/totallength,1);
		sepsum+=separators_.at(i)/totallength;
	}
	p->cd(separators_.size())->SetPad(sepsum,0,1,1);


}
void plotterMultiColumn::adjustCanvas(TCanvas *p){
	if(!p)return;
	int w=0;//p->GetWw() * (int)plotter_.size();
	float totallength=0;
	for(size_t i=0;i<separators_.size();i++){
		w+=separators_.at(i)*p->GetWw();
		totallength+=separators_.at(i);
	}
	int h=p->GetWh();
	p->SetCanvasSize(w,h); //add some resolution for png conversion //DEBUG
}

void plotterMultiColumn::drawPlots(){

	if(plotter_.size()!=plotter_.size())
		throw std::out_of_range("plotterMultiColumn::drawPlots: less or more plots to draw then specified in config");

	for(size_t i=0;i<plotter_.size();i++){
		addPlotData(i);
	}
	if(yaxisconnects_.size()>0){
		float  ymaxconnect=-100,yminconnect=1e20;
		for(size_t i=0;i<yaxisconnects_.size();i++){
			bool dibw=plotter_.at(yaxisconnects_.at(i))->getUpperStyle().divideByBinWidth;
			for(size_t j=0;j<yaxisconnects_.size();j++){
				if(dibw != plotter_.at(yaxisconnects_.at(i))->getUpperStyle().divideByBinWidth)
					throw std::out_of_range("plotterMultiColumn::drawPlots: try to connect divbyBW and not divByBW axis");
			}
			float tmpmax=getYMax(yaxisconnects_.at(i),dibw);
			if(tmpmax>ymaxconnect)ymaxconnect=tmpmax;
			float tmpmin=getYMin(yaxisconnects_.at(i),dibw); //only data entries are important
			if(tmpmin<yminconnect)yminconnect=tmpmin;
		}

		axisStyle* axst= plotter_.at(yaxisconnects_.at(0))->getUpperStyle().yAxisStyle(); //copy from first

		if(axst->applyAxisRange()){
			ymaxconnect=axst->max;
			yminconnect=axst->min;
		}
		else{
			if(axst->log){
				float logmax=log(ymaxconnect)/(2.30258509299404590e+00);
				float logmin=log(yminconnect)/(2.30258509299404590e+00);

				float newmax=logmax+(logmax-logmin)*(yspacemulti_-1);
				ymaxconnect=pow(10,newmax);
				//extend a bit downwars
				logmin=logmin-(logmax-logmin)*0.08;
				yminconnect=pow(10,logmin);
				if(yminconnect<=0)
					yminconnect=(ymaxconnect)*0.0001;//avoid 0 on axis
			}
			else{
				ymaxconnect*=yspacemulti_;
				if(yminconnect==0)
					yminconnect+=(ymaxconnect-yminconnect)*0.0001;//avoid 0 on axis
			}
			axst->min=yminconnect;
			axst->max=ymaxconnect;
		}

		if(debug) std::cout << "plotterMultiColumn::drawPlots: connecting ";

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

		// template the class? then make some getter to set the plots for all
		// maybe cast based on the type? (add type check in plotterBase)
		// ... or inherit again.. prob best

		// .. .. specialized part


		//check margins -> that should be a (virtual) function of plotterbase
		float xmin,xmax;
		xmin=plotter_.at(i)->getXAxisLowerLimit();
		xmax=plotter_.at(i)->getXAxisHigherLimit();


		// .. .. common to all implementations

		if(plotter_.at(i)->hasRatio()){
			if(plotter_.at(i)->getRatioStyle().leftMargin < 0.05){
				xmin+=(xmax-xmin)/190;
			}
			if(plotter_.at(i)->getRatioStyle().rightMargin < 0.05){
				xmax-=(xmax-xmin)/190;
			}
			plotter_.at(i)->getRatioStyle().xAxisStyle()->max=xmax;
			plotter_.at(i)->getRatioStyle().xAxisStyle()->min=xmin;
		}
		else{
			if(plotter_.at(i)->getUpperStyle().leftMargin < 0.05){
				xmin+=(xmax-xmin)/190;
			}
			if(plotter_.at(i)->getUpperStyle().rightMargin < 0.05){
				xmax-=(xmax-xmin)/190;
			}
			plotter_.at(i)->getUpperStyle().xAxisStyle()->max=xmax;
			plotter_.at(i)->getUpperStyle().xAxisStyle()->min=xmin;
		}


		plotter_.at(i)->setDrawLegend(false);
		if(debug)
			std::cout << "plotterMultiColumn::drawPlots: drawing stack " << i << std::endl;
		plotter_.at(i)->draw();
	}

}
void plotterMultiColumn::drawLegends(){
	if(plotter_.size()<1)
		return;
	globalleg_=plotter_.at(0)->getLegend();
	if(!globalleg_){
		throw std::logic_error("plotterMultiColumn::drawLegends: no legend template found");
	}
	legstyle_.applyLegendStyle(globalleg_);
	getPad()->cd(0);
	legstyle_.applyLegendStyle(globalleg_);
	globalleg_->Draw("same");
}
void plotterMultiColumn::refreshPad(){
	//done by control plotters
}

/*
 * goes via bitmap to overcome roots PS processor restrictions
 */
void plotterMultiColumn::printToPdf(const std::string& outname){
	std::string outnamepng=(std::string)std::tmpnam(nullptr)+outname;
	printToPng(outnamepng);
	outnamepng+=".png";
	TString syscall="convert "+outnamepng+" -quality 100 " + outname+".pdf" ;
	system(syscall);
	TString delcall="rm -f "+outnamepng;
	system(delcall);
}


void plotterMultiColumn::printToPng(const std::string& outname){
	TVirtualPad * oldpad=0;
	try{
		oldpad=getPad();
	}catch(...){}
	TCanvas c;


	usePad(&c);
	adjustCanvas(&c);
	draw();
	TString outnamepng=outname;
	outnamepng+=".png";
	c.Print(outnamepng);
	usePad(oldpad);
}

void plotterMultiColumn::readStylePrivMarker(const std::string& infile,bool requireall,const std::string& marker){
	fileReader fr;
	fr.setComment("$");
	fr.setDelimiter(",");
	fr.setStartMarker("["+marker+ "]");
	fr.setEndMarker("[end "+marker+ "]");
	fr.readFile(infile);
	fr.setRequireValues(false);
	if(requireall)
		stylefile_=fr.getValue<std::string>("controlPlotStyleFile");
	else
		stylefile_=fr.getValue<std::string>("controlPlotStyleFile",stylefile_);
	if(!fileExists((getenv("CMSSW_BASE")+(std::string)"/"+stylefile_).data())){
		std::string err="plotterMultiColumn::readStylePriv: style file ";
		err+=stylefile_;
		err+=" does not exist";
		throw std::logic_error(err);
	}


	size_t nplots=fr.getValue<size_t>("nplots");
	plotter_.clear();
	separators_.clear();


	for(size_t i=0;i<nplots;i++){
		std::string stylefile=fr.getValue<std::string>("addStyle"+toString(i),stylefile_);

		///check for plotter type from input text file
		plotterBase *pl=0;
		pl=invokePlotter();
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
		throw std::out_of_range("plotterMultiColumn::readStylePriv: trying to connect more axis than plots defined");
	std::vector<size_t> cntv;
	for(size_t i=0;i<cntc.size();i++){
		size_t newentr=atoi(cntc.at(i).data());
		if(newentr>=nplots)
			throw std::out_of_range("plotterMultiColumn::readStylePriv: trying to connect plots that are out of range");
		cntv.push_back(newentr);
	}
	yaxisconnects_=cntv;
}
void plotterMultiColumn::addStyleForAllPlots(const std::string &f){
	for(size_t i=0;i<plotter_.size();i++)
		plotter_.at(i)->addStyleFromFile(f);
}

void plotterMultiColumn::cleanMem(){

	for(size_t i=0;i<plotter_.size();i++)
		if(plotter_.at(i))delete plotter_.at(i);
	plotter_.clear(); //calls destructors

}

}
