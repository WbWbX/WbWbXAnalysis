/*
 * wReweightingPlots.cc
 *
 *  Created on: 9 Jun 2016
 *      Author: jkiesele
 */

#include "../interface/wReweightingPlots.h"
#include "TtZAnalysis/Tools/interface/binFinder.h"
#include "TtZAnalysis/DataFormats/src/classes.h"
#include <algorithm>
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include <stdexcept>

namespace ztop{


std::vector<float> wReweightingPlots::makeBins(float lowtha, float lowthb, float lowthc)const{
	lowtha++;lowthb++;lowthc++;
	std::vector<float> out = histo1D::createBinning(30,0,100);
	return out;
}

void wReweightingPlots::bookPlots(){
	if(!enabled_)return;

	//hidden in createBinning

	std::vector<float> dy=histo1D::createBinning(10,-4,4);
	std::vector<float> ptW;
	ptW << 0 << 20 << 30 << 40 << 300;
	std::vector<float> dummy; //for additional subvar

	histo2D::c_makelist=true;
	histo1D::c_makelist=true;
	createBinning(dy,ptW,dummy);
	histo2D::c_makelist=false;
	histo1D::c_makelist=false;


}
void wReweightingPlots::fillPlots(){

	if(!enabled_ || !signal_)return;
	if(!event()->puweight) return;
	if(!event()->genW) return;
	if(!event()->genjets) return;
	if(!event()->genjets->size()>0) return;
	if(!event()->genjets->at(0))return;
	if(!event()->costheta_cs) return;
	if(!event()->phi_cs) return;

	//if(event()->genjets->at(0)->pt()<50) return; ///jet pt cut
	//if(event()->genW->q()<0)return; //only check W+ as test //DEBUG

	float detaWj=(event()->genW->eta()-event()->genjets->at(0)->eta());
	//detaWj*=(float)event()->genW->q();
	//if(event()->genW->eta() < 0)
	//	detaWj*=-1;
	//detaWj*=
	float phi=* event()->phi_cs ;
	float costheta=* event()->costheta_cs;
	float dummy=0;
	//float multia= event()->genW->q()<0? 1: -1;
	//float multib= detaWj? 1: -1;


	//costheta*=multia*multib;
	//phi*=multia*multib;


	histo2D *h=getHisto(detaWj,event()->genW->pt(),dummy);

	h->fill(costheta ,phi ,* event()->puweight);

	if(!event()->A7) return;
	getHisto1D(detaWj,event()->genW->pt(),dummy)->fill(*event()->A7,* event()->puweight);

}


void wReweightingPlots::createBinning( const std::vector<float>& a, const std::vector<float>& b,
		const std::vector<float>& c){
	binsa_=makeBinning(a);
	binsb_=makeBinning(b);
	binsc_=makeBinning(c);

	//underflow and overflow are merged. no need for "0" entries
	clearHists();

	hists_.resize(binsa_.size());
	hists1D_.resize(binsa_.size());
	for(size_t i=0;i<binsa_.size();i++){
		hists_.at(i).resize(binsb_.size());
		hists1D_.at(i).resize(binsb_.size());
		for(size_t j=0;j<binsb_.size();j++){
			hists_.at(i).at(j).resize(binsc_.size(),0);
			hists1D_.at(i).at(j).resize(binsc_.size(),0);
		}
	}


	for(size_t i=1;i<binsa_.size()-1;i++){
		for(size_t j=1;j<binsb_.size()-1;j++){
			for(size_t k=1;k<binsc_.size()-1;k++){

				std::vector<float> binsphi  =histo1D::createBinning(18,-M_PI,M_PI); //makeBins(binsa_.at(i) ,binsb_.at(j) ,binsc_.at(k) );
				std::vector<float> binsth=histo1D::createBinning(18, -1,1);

				TString name="rewhist_";
				name+=toTString( binsa_.at(i) )+":"+toTString( binsa_.at(i+1) )+"_";
				name+=toTString( binsb_.at(j) )+":"+toTString( binsb_.at(j+1) )+"_";
				name+=toTString( binsc_.at(k) )+":"+toTString( binsc_.at(k+1) );


				hists_.at(i).at(j).at(k) =  new histo2D(binsth,binsphi,name,"cos#theta","#phi", "Entries", false);
				hists1D_.at(i).at(j).at(k) =  new histo1D(histo1D::createBinning(11,-1.1,1.1),"1D_"+name,"sin#phisin#theta","Events",true);
			}
		}
	}

}
std::vector<float> wReweightingPlots::makeBinning(std::vector<float> bins)const{
	std::sort(bins.begin(),bins.end());
	std::vector<float>::iterator it=std::unique(bins.begin(),bins.end());
	bins.resize( std::distance(bins.begin(),it) );
	bins.insert(bins.begin(),0); //underflow
	if(bins.size()<2){
		bins.push_back(0); //variable not used, just create one bin
		bins.push_back(1); //variable not used, just create one bin
	}
	return bins;
}

histo2D*  wReweightingPlots::getHisto(const float & vara,const float & varb,const float & varc){
	size_t ai=get1DIndex(vara,binsa_);
	size_t bi=get1DIndex(varb,binsb_);
	size_t ci=get1DIndex(varc,binsc_);
	return hists_.at(ai).at(bi).at(ci);
}

histo1D*  wReweightingPlots::getHisto1D(const float & vara,const float & varb,const float & varc){
	size_t ai=get1DIndex(vara,binsa_);
	size_t bi=get1DIndex(varb,binsb_);
	size_t ci=get1DIndex(varc,binsc_);
	return hists1D_.at(ai).at(bi).at(ci);
}
size_t wReweightingPlots::get1DIndex(const float & var, const std::vector<float> &bins)const{

	binFinder<float> fb(bins);
	fb.setMergeUFOF(true);
	return fb.findBin(var);

}

void wReweightingPlots::clearHists(){
	for(size_t i=0;i<hists_.size();i++)
		for(size_t j=0;j<hists_.at(i).size();j++)
			for(size_t k=0;k<hists_.at(i).at(j).size();k++)
				if(hists_.at(i).at(j).at(k)) delete hists_.at(i).at(j).at(k);
	hists_.clear();
	for(size_t i=0;i<hists1D_.size();i++)
		for(size_t j=0;j<hists1D_.at(i).size();j++)
			for(size_t k=0;k<hists1D_.at(i).at(j).size();k++)
				if(hists1D_.at(i).at(j).at(k)) delete hists1D_.at(i).at(j).at(k);
	hists1D_.clear();
}

}

