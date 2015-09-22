/*
 * predictions.h
 *
 *  Created on: May 22, 2015
 *      Author: kiesej
 */

#ifndef TTZANALYSIS_ANALYSIS_INTERFACE_PREDICTIONS_H_
#define TTZANALYSIS_ANALYSIS_INTERFACE_PREDICTIONS_H_

#include "TtZAnalysis/Tools/interface/graphFitter.h"
#include "TtZAnalysis/Tools/interface/likelihood2D.h"
#include <vector>
#include <algorithm>
#include <string>

namespace ztop{

/**
 * simple class to read in mass-dependent ttbar/single top predcitions
 */

class top_prediction {
public:



	enum error_enum{err_scaleup=0, err_scaledown=1,
		err_pdfup=2, err_pdfdown=3,
		err_alphasup=4, err_alphasdown=5,
		err_addgausup=6, err_addgausdown=7,
		err_nominal=8,
		err_maxup=9,err_maxdown=10,
		err_gausup=11,err_gausdown=12
		};
	enum error_names{
		errn_scale,
		errn_pdf,
		errn_alphas,
		errn_addgaus,
		errn_nom
	};

	top_prediction(){
		errgraphs_.resize(err_nominal,graphFitter());
		relerrors_.resize(err_nominal,0);
	}
	~top_prediction(){}

	const std::string & idString()const{return idstring_;}

	//operator =
	//prediction(const prediction&);

	/**
	 * Format:
	 *
	 * * energy in GeV
	 *
	 */
	void readPrediction(const std::string& infile, const std::string& idstring );


	double getXsec(const double& topmass, error_enum error=err_nominal)const;
	double scanTopMass(const double& xsec,  double intervaldown=140, double intervalup=190, error_enum error=err_nominal)const;

	//can be exported to likelihood or to histo2D
	//binning/range given by input
	void exportLikelihood(histo2D *,bool scalegaus=false, bool ignoreunc=false, error_names tobeign=errn_nom)const;


private:

	double getError( const double& topmass, error_enum error, bool& isrel)const;

	graphFitter fittedcurve_;
	std::vector< graphFitter> errgraphs_;
	std::vector< double > relerrors_;
	std::string idstring_;

};


}


#endif /* TTZANALYSIS_ANALYSIS_INTERFACE_PREDICTIONS_H_ */
