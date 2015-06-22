/*
 * predictions.cc
 *
 *  Created on: May 26, 2015
 *      Author: kiesej
 */


#include "../interface/predictions.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "TtZAnalysis/Tools/interface/fileReader.h"
#include <ctgmath>

namespace ztop{
/*
class top_prediction {
public:

	top_prediction(){}
	~top_prediction(){}
	//operator =
	//prediction(const prediction&);


	void readPrediction(const std::string& infile, const std::string& idstring );


	double getXsec(const double& topmass, const double& energy, float* scaleerr=0, float * pdferr=0)const;


private:
	graphFitter fittedcurve_;
std::vector<std::pair<TString, graphFitter> > errgraphs_;
//fixed: scaleup, scaledown, pdfup, pdfdown

};
 */

void top_prediction::readPrediction(const std::string& infile, const std::string& idstring  ){
	fileReader fr;
	fr.setStartMarker("[pred - "+idstring+"]");
	fr.setEndMarker("[end pred]");
	fr.setDelimiter(",");
	fr.setComment("#");
	fr.readFile(infile);

	//check if rel errors
	const bool relerrors=fr.getValue<bool>("relerr");
	//get parameters
	std::vector<double> pars;
	for(size_t i=0;;i++){
		try{
			pars.push_back(fr.getValue<double>("par"+toString(i)));
		}
		catch(...){
			break;
		}
	}
	//fm_pol0,fm_pol1,fm_pol2,fm_pol3,fm_pol4
	if(pars.size()>7)
		throw std::out_of_range("no mode for more than 7 parameters defined");
	else if(pars.size()>6)
		fittedcurve_.setFitMode(graphFitter::fm_pol6);
	else if(pars.size()>5)
		fittedcurve_.setFitMode(graphFitter::fm_pol5);
	else if(pars.size()>4)
		fittedcurve_.setFitMode(graphFitter::fm_pol4);
	else if(pars.size()>3)
		fittedcurve_.setFitMode(graphFitter::fm_pol3);
	else if(pars.size()>2)
		fittedcurve_.setFitMode(graphFitter::fm_pol2);
	else if(pars.size()>1)
		fittedcurve_.setFitMode(graphFitter::fm_pol1);
	else if(pars.size()>0)
		fittedcurve_.setFitMode(graphFitter::fm_pol0);

	std::vector<double> dummysteps(pars.size(),0.1);
	fittedcurve_.setParameters(pars,dummysteps);//copy
	fittedcurve_.setDoXYShift(true);
	fittedcurve_.setXShift(fr.getValue<double>("xshift"));
	fittedcurve_.setYShift(fr.getValue<double>("yshift"));

	//wuick check for exception
	fittedcurve_.getFitOutput(150);
	idstring_=idstring;

	if(relerrors){
		relerrors_.at(err_scaleup)=fr.getValue<double>("scaleerrup");
		relerrors_.at(err_scaledown)=fr.getValue<double>("scaleerrdown");
		relerrors_.at(err_pdfup)=fr.getValue<double>("pdferrup");
		relerrors_.at(err_pdfdown)=fr.getValue<double>("pdferrdown");
		fr.setRequireValues(false);
		relerrors_.at(err_alphasup)=fr.getValue<double>("alphaserrup",0);
		relerrors_.at(err_alphasdown)=fr.getValue<double>("alphaserrdown",0);
		for(size_t i=0;i<relerrors_.size();i++){
			if(relerrors_.at(i)==0)
				relerrors_.at(i)=1e-6; //protect against zeros
		}
	}
	else{
		throw std::runtime_error("top_prediction::readPrediction: non-relative errors TBI!");
	}
}

double top_prediction::getError( const double& topmass,error_enum error, bool& isrel)const{
	if(error==err_nominal)
		throw std::out_of_range("");

	if((error == err_maxup || error==err_maxdown) || relerrors_.at(error) != 0){
		isrel=true;
		if(error<err_nominal)
			return relerrors_.at(error);
		//calculate sum
		double sum2=0;
		bool dummy;
		bool upvar=error==err_maxup;
		for(size_t i=0;i<err_nominal-1;i++){
			double max=getMaxVar(upvar,relerrors_.at(i),relerrors_.at(i+1),dummy);;
			sum2+=max*max;
			i++;
		}
		if(upvar)
			return sqrt(sum2);
		else
			return -sqrt(sum2);
	}
	isrel=false;
	if(errgraphs_.at(error).getParameters()->size() <1)
		throw std::logic_error("top_prediction::getError: no error defined.");

	return errgraphs_.at(error).getFitOutput(topmass);

}


double top_prediction::getXsec(const double& topmass, error_enum error)const{
	if(fittedcurve_.getParameters()->size() < 1){
		throw std::logic_error("top_prediction::getXsec: first load prediction");
	}

	double out = fittedcurve_.getFitOutput(topmass);
	if(error==err_nominal)
		return out;


	bool isrel=true;
	double err=getError(topmass, error,isrel);

	if(isrel)
		return out*(1+err);
	else
		return err;

}
double top_prediction::scanTopMass(const double& xsec, error_enum error)const{
	double intervalup=190;
	double intervaldown=140;

	while(intervalup-intervaldown > 0.01){
		double xsecup=getXsec(intervalup,error);
		double xsecdown=getXsec(intervaldown,error);

		if(xsecup < xsec)
			intervalup-= (intervalup-intervaldown)*0.2;
		else
			intervalup+= (intervalup-intervaldown)*0.3;
		if(xsecdown> xsec)
			intervaldown+=(intervalup-intervaldown)*0.2;
		else
			intervaldown-=(intervalup-intervaldown)*0.3;
	}
	double out=0;
	out=(intervalup+intervaldown)/2;
	return out;
}

void top_prediction::exportLikelihood(histo2D *h,bool scalegaus)const{
	if(!h)
		throw std::logic_error("top_prediction::exportLikelihood: pointer NULL");

	h->removeAllSystematics();
	double LHmax=0;
	for(size_t i=1;i<h->getBinsX().size()-1;i++){
		for(size_t j=1;j<h->getBinsY().size()-1;j++){
			float centerx,centery;
			h->getBinCenter(i,j,centerx,centery);
			double xsec = getXsec(centerx) ;


			double pdfup= getXsec(centerx,err_pdfup) - xsec;
			double pdfdown= getXsec(centerx,err_pdfdown)- xsec;
			double scaleup= getXsec(centerx,err_scaleup)- xsec;
			double scaledown= getXsec(centerx,err_scaledown)- xsec;
			double asup= getXsec(centerx,err_alphasup)- xsec;
			double asdown= getXsec(centerx,err_alphasdown)- xsec;
			bool corrdummy=true;
			double maxscaleup=getMaxVar(true,scaleup,scaledown,corrdummy);
			double maxscaledown=getMaxVar(false,scaleup,scaledown,corrdummy);

			double maxpdfup=getMaxVar(true,pdfup,pdfdown,corrdummy);
			double maxpdfdown=getMaxVar(false,pdfup,pdfdown,corrdummy);

			double maxasup=getMaxVar(true,asup,asdown,corrdummy);
			double maxasdown=getMaxVar(false,asup,asdown,corrdummy);

			//add alpha_s errors

			double totalgaussup=sqrt(maxasup*maxasup+maxpdfup*maxpdfup);
			double totalgaussdown=sqrt(maxasdown*maxasdown+maxpdfdown*maxpdfdown);
			double likelihood  =0;
			if(!scalegaus){
				likelihood= 1/(2* (maxscaleup -  maxscaledown));
				likelihood =  erf( (maxscaleup   + xsec - centery)/totalgaussup );
				likelihood-=  erf( (maxscaledown + xsec - centery)/totalgaussdown ) ;
			}
			else{
				totalgaussup = sqrt(totalgaussup*totalgaussup + maxscaleup*maxscaleup);
				totalgaussdown = sqrt(totalgaussdown*totalgaussdown + maxscaledown*maxscaledown);
				double diff=xsec-centery;
				if(diff>0)
					likelihood = exp(-0.5 * diff*diff/(totalgaussup*totalgaussup) );
				else
					likelihood = exp(-0.5 * diff*diff/(totalgaussdown*totalgaussdown) );
			}
			//likelihood=std::abs(likelihood);

			if(LHmax<likelihood)
				LHmax=likelihood;
			h->getBin(i,j).setContent(likelihood);
		}
	}
	*h*=1/LHmax; // make maximum 1
}

}

