/*
 * mtFromInclXsec.h
 *
 *  Created on: Mar 24, 2015
 *      Author: kiesej
 */

#ifndef MTFROMINCLXSEC_H_
#define MTFROMINCLXSEC_H_
#include "TString.h"
#include <vector>
#include <string>
#include "TtZAnalysis/Tools/interface/graph.h"
#include "TtZAnalysis/Tools/interface/graphFitter.h"
#include "TtZAnalysis/Tools/interface/histo2D.h"

namespace ztop{

class mtFromInclXsec{
public:

	mtFromInclXsec():energy_(8){}

	void readInFiles(const std::vector<std::string>&);

	void setEnvelopeUnc(const TString uncname){envunc_=uncname;}

	graphFitter getMassDependence(const TString& syst="")const;

	void setEnergy(const float& en){energy_=en;}

	void extract();

	/**
	 * Only envelope error
	 */
	const histo2D&  getExpLikelihood()const;
	const histo2D&  getTheoLikelihood()const;
	/**
	 *  Only envelope error
	 */
	const histo2D&  getJointLikelihood()const;

	const graph& getExpPoints()const;

	/**
	 * This will include the envelope uncertainty used for the Joint likelihood,
	 * and variations according to the other uncertainties (for comibnation with
	 * other results)
	 */
	graph getResult()const;

private:

	float energy_;
	TString envunc_;
	histo2D explh_,theolh_,jointlh_;
	graph exppoints_;
	graph result_;


	double LH_Exp(const float& mtop,const float& xsec,const ztop::graphFitter & gf, const float& errup,const float& errdown)const;

	double sq(const double& a)const{
		return a*a;
	}

};

}



#endif /* MTFROMINCLXSEC_H_ */
