/*
 * wNLOReweighter.h
 *
 *  Created on: 22 Jun 2016
 *      Author: jkiesele
 */

#ifndef TTZANALYSIS_ANALYSIS_INTERFACE_WNLOREWEIGHTER_H_
#define TTZANALYSIS_ANALYSIS_INTERFACE_WNLOREWEIGHTER_H_

#include "TtZAnalysis/Tools/interface/binFinder.h"
#include "scalefactors.h"
#include "simpleReweighter.h"
#include <string>
#include <vector>

namespace ztop{

class NTGenJet;
class NTGenParticle;

class wNLOReweighter: public simpleReweighter{
public:
	wNLOReweighter():simpleReweighter(),up_(false),simple_(false){}
	~wNLOReweighter(){}

	void setUp(bool up){up_=up;}
	const bool getUp()const{return up_;}

	void readParameterFile(const std::string& infile);

	void setReweightParameter(size_t paraindex, float reweightfactorminusone);

	void setSimple(bool set){simple_=set;}

	static const size_t wdxsec_npars;
	static double wdxsec(double * angles, double* pars);

	void prepareWeight(const float& costheta, const float& phi, const NTGenParticle* W, const NTGenJet * jet);




private:

	void setNewWeight(const float &w){
		simpleReweighter::setNewWeight(w);
	}

	bool up_;
	bool simple_;


	std::vector<float> detabins_;
	std::vector<float> Wptbins_;
	binFinder<float> bfdeta_,bfWpt_;

	//in deta, Wpt bins, then para number
	std::vector<  std::vector< std::vector<double> > > origparas_;
	std::vector<  std::vector< std::vector<double> > > rewparas_;
	std::vector<  std::vector< scalefactors *> > simplerew_;
};

}



#endif /* TTZANALYSIS_ANALYSIS_INTERFACE_WNLOREWEIGHTER_H_ */
