/*
 * wNLOReweighter.cc
 *
 *  Created on: 23 Jun 2016
 *      Author: jkiesele
 */



#include "../interface/wNLOReweighter.h"
#include <cmath>
#include "TtZAnalysis/Tools/interface/fileReader.h"
#include <algorithm>
#include "TtZAnalysis/DataFormats/interface/NTGenJet.h"
#include "TtZAnalysis/DataFormats/interface/NTGenParticle.h"

namespace ztop{

const size_t wNLOReweighter::wdxsec_npars=9;
//move to reweihgting class later
double wNLOReweighter::wdxsec(double * angles, double* pars){
	//this will be probably optimized away, but is great for debugging if -d is compiled
	double theta=std::acos(angles[0]);
	double phi=angles[1];

	double F0=pars[0];
	double F1=pars[1];
	double F2=pars[2];
	double F3=pars[3];
	double F4=pars[4];
	double F5=pars[5];
	double F6=pars[6];
	double F7=pars[7];
	double F=pars[8];

	double costheta=angles[0];
	//double cos2theta=std::cos(2*theta);
	double sintheta=std::sin(theta);
	double sin2theta=std::sin(2*theta);
	double cosphi=std::cos(phi);
	double cos2phi=std::cos(2*phi);
	double sinphi=std::sin(phi);
	double sin2phi=std::sin(2*phi);

	double res= //
			F*(1+ costheta*costheta)
			+ F0*0.5*(1-3*costheta*costheta)
			+ F1*sin2theta*cosphi
			+ F2*0.5*sintheta*sintheta*cos2phi
			+ F3*sintheta*cosphi
			+ F4*costheta
			+ F5*sintheta*sintheta*sin2phi
			+ F6*sin2theta*sinphi
			+ F7*sintheta*sinphi;

	return res;

}




void wNLOReweighter::readParameterFile(const std::string& infile){
	fileReader fr;
	fr.setDelimiter(" ");
	fr.setComment("$");
	fr.readFile(infile);

	const size_t firstparaentry=6;

	//first read in then remove duplicates in binning vectors
	for(size_t i=0;i<fr.nLines();i++){
		if(fr.nEntries(i)<4)continue;

		detabins_.push_back(fr.getData<float>(i,0));
		detabins_.push_back(fr.getData<float>(i,1));

		Wptbins_.push_back(fr.getData<float>(i,2));
		Wptbins_.push_back(fr.getData<float>(i,3));

	}

	std::sort(detabins_.begin(),detabins_.end());
	std::sort(Wptbins_.begin(),Wptbins_.end());
	std::vector<float>::iterator it=std::unique(detabins_.begin(),detabins_.end());
	detabins_.resize( std::distance(detabins_.begin(),it) );
	it=std::unique(Wptbins_.begin(),Wptbins_.end());
	Wptbins_.resize( std::distance(Wptbins_.begin(),it) );

	detabins_.insert(detabins_.begin(),0);
	Wptbins_.insert(Wptbins_.begin(),0); //create dummy underflow (not used)

	origparas_.resize(detabins_.size());
	for(size_t i=0;i<detabins_.size();i++){
		origparas_.at(i).resize(Wptbins_.size());
		//for(size_t j=0;j<Wptbins_.size();j++){
		//	origparas_.at(i).at(j).resize(Wptbins_.size());
		//}
	}

	//for std binning, add an "UF" bin
	size_t line=0;
	for(size_t detabin=1;detabin<detabins_.size()-1;detabin++){
		//std::cout << detabins_.at(detabin) << std::endl;
		for(size_t ptbin=1;ptbin<Wptbins_.size()-1;ptbin++){
			//std::cout << Wptbins_.at(ptbin) << std::endl;
			std::vector<double> paras;
			for(size_t entr=firstparaentry;entr<fr.nEntries(line);entr++){
				paras.push_back(fr.getData<double>(line,entr));
			}
			origparas_.at(detabin).at(ptbin)=paras;
			line++;
			if(line==fr.nLines()) break;
		}
		if(line==fr.nLines()) break;
	}

	bfdeta_=binFinder<float>(detabins_);
	bfdeta_.setMergeUFOF(true);
	bfWpt_ = binFinder<float> (Wptbins_);
	bfWpt_.setMergeUFOF(true);

}

void wNLOReweighter::setReweightParameter(size_t paraindex, float reweightfactorminusone){
	if(origparas_.size() < 2 || origparas_.at(1).size() < 2 || origparas_.at(1).at(1).size() <1 )
		throw std::runtime_error("wNLOReweighter::setReweightParameter: first read-in default parameters");
	if(rewparas_.size()<1)
		rewparas_=origparas_;
	for(size_t detabin=1;detabin<detabins_.size()-1;detabin++){
		//std::cout << detabins_.at(detabin) << std::endl;
		for(size_t ptbin=1;ptbin<Wptbins_.size()-1;ptbin++){
			//std::cout << Wptbins_.at(ptbin) << std::endl;
			if(paraindex >= origparas_.at(detabin).at(ptbin).size())
					throw std::out_of_range("wNLOReweighter::setReweightParameter: para index out of range");

			rewparas_.at(detabin).at(ptbin).at(paraindex) = origparas_.at(detabin).at(ptbin).at(paraindex) * (1+reweightfactorminusone);
		}
	}


}


void wNLOReweighter::prepareWeight(const float& costheta, const float& phi, const NTGenParticle* W, const NTGenJet * jet){
	if(!W || !jet)
		return;
	if(switchedOff())return;
	if(rewparas_.size()<1)
		throw std::logic_error("wNLOReweighter::prepareWeight: no reweighting set");

	float deta=W->eta()-jet->eta();
	size_t detabin=bfdeta_.findBin(deta);
	size_t ptbin=bfWpt_.findBin(deta);
	double angles[2]={costheta,phi};
	double nom=wdxsec(angles,&(origparas_.at(detabin).at(ptbin).at(0)));
	double var=wdxsec(angles,&(rewparas_.at(detabin).at(ptbin).at(0)));

	setNewWeight(var/nom);

}

}//ns
