/*
 * scalefactors.cc
 *
 *  Created on: Aug 1, 2013
 *      Author: kiesej
 */
#include "../interface/scalefactors.h"
#include <cstdlib>
#include <stdexcept>
#include "TGraphAsymmErrors.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"

namespace ztop {


/**
 * decide for 1d, 2d before
 */



/**
 * sets input with a lot of error checks
 * return codes < 0 --> error
 * -1: wrong filename
 * -2: wrong histoname
 * -3: wrong histoclass
 *
 * no error returns 0;
 */
int scalefactors::setInput(TString filename, TString histname, TString histnameerrup, TString histnameerrdown) {
	TFile * f = new TFile(filename, "READ");
	isglobal_ = false;
	if (f->IsZombie()) {
		std::cout << "scalefactors::setInput: file " << filename
				<< " not found.!" << std::endl;
		throw std::runtime_error("scalefactors::setInput: file not found");
	}

	TObject * ph = tryToGet<TObject>(f,histname);
	TString classname = ph->ClassName();
	if (classname.Contains("TH2")) {
		TH2* th2 = (TH2D*) ph;
                for(int binx=0;binx<=th2->GetNbinsX();binx++){
                        for(int biny=0;biny<=th2->GetNbinsY();biny++){
                        th2d_.SetBinContent(binx,biny, th2->GetBinContent(binx,biny) );  
                        }
                }
		h = &th2d_;
		isth2d_ = true;
		isglobal_ = false;
		//fill errors
		th2derrup_=th2d_;
		th2derrdown_=th2d_;
		if(histnameerrup.Length()<1 || histnameerrup.Length()<1){
			for(int binx=0;binx<=th2d_.GetNbinsX();binx++){
				for(int biny=0;biny<=th2d_.GetNbinsY();biny++){
					//std::cout << "setting errors: " << th2d_.GetBinContent(binx,biny) + th2d_.GetBinError(binx,biny) <<std::endl;
					th2derrup_.SetBinContent(binx,biny, th2d_.GetBinContent(binx,biny) + th2d_.GetBinError(binx,biny) );
					th2derrdown_.SetBinContent(binx,biny, th2d_.GetBinContent(binx,biny) - th2d_.GetBinError(binx,biny) );
				}
			}
		}
		else{
			th2derrup_=*tryToGet<TH2D>(f, histnameerrup);
			th2derrdown_=*tryToGet<TH2D>(f, histnameerrdown);
		}

		return 0;
	} else if (classname.Contains("TH1D")) {
		th1d_ = *(TH1D*) ph;
		h = &th1d_;
		isth2d_ = false;
		isglobal_ = false;

		th1derrup_=th1d_;
		th1derrdown_=th1d_;
		if(histnameerrup.Length()<1 || histnameerrup.Length()<1){
			for(int binx=0;binx<=th1d_.GetNbinsX();binx++){
				th1derrup_.SetBinContent(binx, th1d_.GetBinContent(binx) + th1d_.GetBinError(binx) );
				th1derrdown_.SetBinContent(binx, th1d_.GetBinContent(binx) - th1d_.GetBinError(binx) );
			}

		}
		else{
			th1derrup_=*tryToGet<TH1D>(f, histnameerrup);
			th1derrdown_=*tryToGet<TH1D>(f, histnameerrdown);
		}

		return 0;
	} else if(classname.Contains("TGraphAsymmErrors")){
		TGraphAsymmErrors *g = (TGraphAsymmErrors*)ph;
		//create bins
		std::vector<double> bins;
		//sort points
		std::vector<double> xpoints;//(g->GetX(),(size_t)g->GetN());
		for(int i=0;i<g->GetN();i++)
			xpoints.push_back(g->GetX()[i]);

		std::vector<size_t> sortlist=retsort(xpoints.begin(),xpoints.end());
		for(size_t i=0;i<sortlist.size();i++){
			size_t realit=sortlist.at(i);
			//std::cout << "getting: "<< i << "->" << realit << std::endl;
			double x=0;
			double xnext=0;
			double y=0;
			g->GetPoint(realit,x,y);
			if(realit<(size_t)g->GetN()-1){
				g->GetPoint(realit+1,xnext,y);
				g->GetErrorXhigh(realit);
				g->GetErrorXlow(realit);
				g->GetErrorXlow(realit+1);
				if(fabs( x+ g->GetErrorXhigh(realit)-(xnext-g->GetErrorXlow(realit+1))) > 0.01*(xnext-x)){
					throw std::runtime_error("scalefactors::setInput: cannot find valid bin indications in input graph");
				}
				bins.push_back(x-g->GetErrorXlow(realit));
			}
			else{//last boundary
				bins.push_back(x-g->GetErrorXlow(realit));
				bins.push_back(x+g->GetErrorXhigh(realit));
			}
			//g->GetErrorYhigh(i);
			//g->GetErrorYlow(i);


		}
		//for (size_t i=0;i<bins.size();i++)
		//	std::cout << bins.at(i) << std::endl;

		th1d_=TH1D("","",bins.size()-1,&(bins.at(0)));
		th1derrdown_=th1d_;
		th1derrup_=th1d_;
		for(size_t i=0;i<sortlist.size();i++){
			size_t realit=sortlist.at(i);
			double x=0;
			double y=0;
			g->GetPoint(i,x,y);
			th1d_.SetBinContent(realit+1,y);
			th1derrdown_.SetBinContent(realit+1,y-g->GetErrorYlow(i));
			th1derrup_.SetBinContent(realit+1,y+g->GetErrorYhigh(i));
		}
		h = &th1d_;
		TH1D* htemp=(TH1D*)h->Clone();
		htemp->Draw();
		isth2d_ = false;
		isglobal_ = false;
		return 0;

	}else {
		std::cout
		<< "scalefactors::setInput: class of input histo must be TH1D, TGraphAsymmErrors or TH2*"
		<< std::endl;
		throw std::runtime_error("scalefactors::setInput: histo wrong format");
	}

}
/**
 * errors in percent!
 */
void scalefactors::setGlobal(double sf, double errup, double errdown) {
	glsf_ = sf;
	glsfup_ = glsf_ + errup / 100;
	glsfd_ = glsf_ - errdown / 100;
	isglobal_ = true;
}
//priv
void scalefactors::setHistPointer(){
	if(syst_==sys_nominal){
		if(isth2d_) h=&th2d_;
		else h=&th1d_;
	}
	else if(syst_==sys_up){
		if(isth2d_) h=&th2derrup_;
		else h=&th1derrup_;
	}
	else if(syst_==sys_down){
		if(isth2d_) h=&th2derrdown_;
		else h=&th1derrdown_;
	}
}

/**
 * sets systematic variation.
 * returns -1 in case of no success and sets to nominal
 * inputstring should contain only one option of:
 * -nom
 * -down
 * -up
 * if more than one option is provided, the first found is set according to the
 * ordering given above
 */
int scalefactors::setSystematics(TString updownnom) {
	if (updownnom.Contains("nom")) {
		syst_ = sys_nominal;
		setHistPointer();
		std::cout << "scalefactors::setSystematics: set to " << updownnom
				<< std::endl;
		return 0;
	} else if (updownnom.Contains("down")) {
		syst_ = sys_down;
		setHistPointer();
		std::cout << "scalefactors::setSystematics: set to " << updownnom
				<< std::endl;
		return 0;
	} else if (updownnom.Contains("up")) {
		syst_ = sys_up;
		setHistPointer();
		std::cout << "scalefactors::setSystematics: set to " << updownnom
				<< std::endl;
		return 0;
	} else {
		std::cout
		<< "scalefactors::setSystematics: input must contain up, down or nom"
		<< std::endl;
		h=0;
		return -1;
	}
}
scalefactors::scalefactors(const ztop::scalefactors & rhs) {
	if(&rhs != this)
		copyFrom(rhs);
}

scalefactors& scalefactors::operator = (const scalefactors& rhs){
	if(&rhs!=this)
		copyFrom(rhs);
	return *this;
}

void scalefactors::copyFrom(const scalefactors& rhs){
	th1d_ = rhs.th1d_;
	th2d_ = rhs.th2d_;
	th1derrup_ = rhs.th1derrup_;
	th2derrup_ = rhs.th2derrup_;
	th1derrdown_ = rhs.th1derrdown_;
	th2derrdown_ = rhs.th2derrdown_;
	isth2d_ = rhs.isth2d_;

	isMC_ = rhs.isMC_;
	isglobal_ = rhs.isglobal_;
	glsf_=rhs.glsf_;
	glsfup_=rhs.glsfup_;
	glsfd_=rhs.glsfd_;
	rangecheck_ = rhs.rangecheck_;
	switchedoff_=rhs.switchedoff_;

	syst_ = rhs.syst_;
	setHistPointer();
}

}

