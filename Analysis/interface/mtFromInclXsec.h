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
#include "TtZAnalysis/Analysis/interface/predictions.h"

namespace ztop{

class mtFromInclXsec{
public:

	mtFromInclXsec():mtexpunc_(1),mt_(0),mtup_(0),mtdown_(0),intxsec_(0){}

	void readInFiles(const std::vector<std::string>&);
	void readPrediction(const std::string& file,const std::string& id){
		predicted_.readPrediction(file,id);
	}

	/**
	 * can be used to remove the uncorrelated part
	 */
	void scaleFitUnc(const float scale, const std::string& uncname="fit");

	void setEnvelopeUnc(const TString uncname){envunc_=uncname;}

	graphFitter getMassDependence(const TString& syst="")const;

	enum sys_enum{sys_nominal,sys_totup,sys_totdown};

	graph getMassDepGraphTheo(sys_enum sys, size_t npoints=20)const;

	//void setEnergy(const float& en){energy_=en;}

	void extract();

	const float& getMtopCentral()const{return mt_;}
	const float& getMtopUp()const{return mtup_;}
	const float& getMtopDown()const{return mtdown_;}

	const float getXsec()const{return intxsec_;}

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

	graph getOneSigmaPointsJoint()const{return onesigmajoint_;}
	/**
	 * This will include the envelope uncertainty used for the Joint likelihood,
	 * and variations according to the other uncertainties (for comibnation with
	 * other results)
	 */


	graph getResult()const;

	const top_prediction* getPrediction()const{return &predicted_;}

	static bool debug;

private:

	//float energy_;
	TString envunc_;
	histo2D explh_,theolh_,jointlh_;
	graph exppoints_;
	graph result_;
	top_prediction predicted_;

	float mtexpunc_;

	double LH_Exp(const float& mtop,const float& xsec,const ztop::graphFitter & gf, const float& errup,const float& errdown)const;

	double sq(const double& a)const{
		return a*a;
	}

	graph createOneSigmaPoints(const histo2D&)const;
	graph onesigmajoint_;
	float mt_,mtup_,mtdown_,intxsec_;

};

}



#endif /* MTFROMINCLXSEC_H_ */
