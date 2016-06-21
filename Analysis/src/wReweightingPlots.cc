/*
 * wReweightingPlots.cc
 *
 *  Created on: 9 Jun 2016
 *      Author: jkiesele
 */

#include "../interface/wReweightingPlots.h"

#include "TtZAnalysis/DataFormats/src/classes.h"
#include <algorithm>
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include <stdexcept>

namespace ztop{


std::vector<float> wReweightingPlots::makeBins(float lowtha, float lowthb, float lowthc)const{
	lowtha++;lowthb++;lowthc++;
	std::vector<float> out = histo1D::createBinning(40,0,100);
	return out;
}

void wReweightingPlots::bookPlots(){
	if(!enabled_)return;

	//hidden in createBinning

	std::vector<float> dy=histo1D::createBinning(4,-5,5);
	std::vector<float> ptW=histo1D::createBinning(3, 0,60);
	std::vector<float> dummy; //for additional subvar

	histo2D::c_makelist=true;
	createBinning(dy,ptW,dummy);
	histo2D::c_makelist=false;


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



	float detaWj=event()->genW->eta()-event()->genjets->at(0)->eta();
	float dummy=0;

	histo2D *h=getHisto(detaWj,event()->genW->pt(),dummy);

	h->fill(std::acos(* event()->costheta_cs),* event()->phi_cs ,* event()->puweight);

}


void wReweightingPlots::createBinning( const std::vector<float>& a, const std::vector<float>& b,
		const std::vector<float>& c){
	binsa_=makeBinning(a);
	binsb_=makeBinning(b);
	binsc_=makeBinning(c);

	//underflow and overflow are merged. no need for "0" entries
	clearHists();

	hists_.resize(binsa_.size());
	for(size_t i=0;i<binsa_.size();i++){
		hists_.at(i).resize(binsb_.size());
		for(size_t j=0;j<binsb_.size();j++){
			hists_.at(i).at(j).resize(binsc_.size(),0);
		}
	}


	for(size_t i=1;i<binsa_.size()-1;i++){
		for(size_t j=1;j<binsb_.size()-1;j++){
			for(size_t k=1;k<binsc_.size()-1;k++){

				std::vector<float> binsphi  =histo1D::createBinning(100,-M_PI,M_PI); //makeBins(binsa_.at(i) ,binsb_.at(j) ,binsc_.at(k) );
				std::vector<float> binsth=histo1D::createBinning(100, 0,M_PI);

				TString name="rewhist_";
				name+=toTString( binsa_.at(i) )+"-"+toTString( binsa_.at(i+1) )+"_";
				name+=toTString( binsb_.at(j) )+"-"+toTString( binsb_.at(j+1) )+"_";
				name+=toTString( binsc_.at(k) )+"-"+toTString( binsc_.at(k+1) );


				hists_.at(i).at(j).at(k) =  new histo2D(binsth,binsphi,name,"#theta","#phi", "Entries", true);
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
	size_t bi=get1DIndex(vara,binsb_);
	size_t ci=get1DIndex(vara,binsc_);
	return hists_.at(ai).at(bi).at(ci);
}

size_t wReweightingPlots::get1DIndex(const float & var, const std::vector<float> &bins_)const{
	if(bins_.size()<1){
		return 0;
	}
	if(bins_.size() <2){
		return 0;
	}
	size_t binno=0;
	std::vector<float>::const_iterator it=std::lower_bound(bins_.begin()+1, bins_.end(), var);
	if(var==*it)
		binno= it-bins_.begin();
	else
		binno= it-bins_.begin()-1;
	//no underflow or overflow, merge all in last/first
	if(binno==0){
		binno++;
	}
	else if(binno == bins_.size()-1){
		binno--;
	}
	return binno;
}
size_t wReweightingPlots::getIndex(const size_t& a, const size_t& b, const size_t& c)const{
	//the 0 indicies don't matter here!!

	return 1;//think of something

}
void wReweightingPlots::clearHists(){
	for(size_t i=0;i<hists_.size();i++)
		for(size_t j=0;j<hists_.at(i).size();j++)
			for(size_t k=0;k<hists_.at(i).at(j).size();k++)
				if(hists_.at(i).at(j).at(k)) delete hists_.at(i).at(j).at(k);
	hists_.clear();
}

}

