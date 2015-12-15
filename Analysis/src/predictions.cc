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

	if(!fileExists(infile.data()))
		throw std::runtime_error((std::string)("top_prediction::readPrediction: file " +infile+" not found"));

	fileReader fr;
	fr.setStartMarker("[pred - "+idstring+"]");
	fr.setEndMarker("[end pred]");
	fr.setDelimiter(",");
	fr.setComment("#");
	fr.readFile(infile);

	if(fr.nLines()>25)
		throw std::runtime_error("top_prediction::readPrediction: read-in error. read to many lines (>25)");
	if(fr.nLines()<3)
		throw std::runtime_error((std::string)("top_prediction::readPrediction: read-in error for "+idstring));
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
	else
		throw std::runtime_error("top_prediction::readPrediction: no parameter identified");

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
		relerrors_.at(err_addgausup)=fr.getValue<double>("adderrup",0);
		relerrors_.at(err_addgausdown)=fr.getValue<double>("adderrdown",0);
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

	if((error == err_maxup || error==err_maxdown)
			|| error==err_gausup || error==err_gausdown || relerrors_.at(error) != 0){

		bool gausonly=error==err_gausup || error==err_gausdown;

		isrel=true;
		if(error<err_nominal)
			return relerrors_.at(error);
		//calculate sum
		double sum2=0;
		bool dummy;
		bool upvar=error==err_maxup || error==err_gausup;
		for(size_t i=0;i<err_nominal-1;i++){
			if(gausonly && (i==err_scaledown || i==err_scaleup))
				continue;
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
double top_prediction::scanTopMass(const double& xsec,  double intervaldown, double intervalup,error_enum error)const{

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

void top_prediction::exportLikelihood(histo2D *h,bool scalegaus, bool ignoreunc, error_names tobeignored)const{
	if(!h)
		throw std::logic_error("top_prediction::exportLikelihood: pointer NULL");

	if(!ignoreunc)
		tobeignored=errn_nom; //will have no effect


	h->removeAllSystematics();
	double LHmax=0;
	for(size_t i=1;i<h->getBinsX().size()-1;i++){
		float linemax=0;
		float centerx=h->getBinsX().at(i) +h->getBinsX().at(i+1);
		centerx/=2;

		double xsec = getXsec(centerx) ;
		/*
		double pdfup= getXsec(centerx,err_pdfup) - xsec;
		double pdfdown= getXsec(centerx,err_pdfdown)- xsec;
		double scaleup= getXsec(centerx,err_scaleup)- xsec;
		double scaledown= getXsec(centerx,err_scaledown)- xsec;
		double asup= getXsec(centerx,err_alphasup)- xsec;
		double asdown= getXsec(centerx,err_alphasdown)- xsec;
		double addgup= getXsec(centerx,err_addgausup)- xsec;
		double addgdown= getXsec(centerx,err_addgausdown)- xsec;

		bool corrdummy=true;
		double maxscaleup=getMaxVar(true,scaleup,scaledown,corrdummy);
		double maxscaledown=getMaxVar(false,scaleup,scaledown,corrdummy);

		double maxpdfup=getMaxVar(true,pdfup,pdfdown,corrdummy);
		double maxpdfdown=getMaxVar(false,pdfup,pdfdown,corrdummy);

		double maxasup=getMaxVar(true,asup,asdown,corrdummy);
		double maxasdown=getMaxVar(false,asup,asdown,corrdummy);

		double maxaddup=getMaxVar(true,addgup,addgdown,corrdummy);
		double maxadddown=getMaxVar(false,addgup,addgdown,corrdummy);
		//add alpha_s errors
		 * */

		double scaleup=   getXsec(centerx,err_scaleup)- xsec;
		double scaledown= getXsec(centerx,err_scaledown)- xsec;

		bool corrdummy=true;
		double maxscaleup=getMaxVar(true,scaleup,scaledown,corrdummy);
		double maxscaledown=getMaxVar(false,scaleup,scaledown,corrdummy);

		double totalgaussup2= 0;
		double totalgaussdown2= 0;
		if(!ignoreunc){
			totalgaussup2= getXsec(centerx,err_gausup) - xsec;//(maxasup*maxasup+maxpdfup*maxpdfup+maxaddup*maxaddup);
			totalgaussup2*=totalgaussup2;
			totalgaussdown2= getXsec(centerx,err_gausdown) - xsec;//(maxasdown*maxasdown+maxpdfdown*maxpdfdown+maxadddown*maxadddown);
			totalgaussdown2*=totalgaussdown2;
		}
		else{
			double pdfup= getXsec(centerx,err_pdfup) - xsec;
			double pdfdown= getXsec(centerx,err_pdfdown)- xsec;
			double asup= getXsec(centerx,err_alphasup)- xsec;
			double asdown= getXsec(centerx,err_alphasdown)- xsec;
			double addgup= getXsec(centerx,err_addgausup)- xsec;
			double addgdown= getXsec(centerx,err_addgausdown)- xsec;

			bool corrdummy=true;

			double maxpdfup=getMaxVar(true,pdfup,pdfdown,corrdummy);
			double maxpdfdown=getMaxVar(false,pdfup,pdfdown,corrdummy);

			double maxasup=getMaxVar(true,asup,asdown,corrdummy);
			double maxasdown=getMaxVar(false,asup,asdown,corrdummy);

			double maxaddup=getMaxVar(true,addgup,addgdown,corrdummy);
			double maxadddown=getMaxVar(false,addgup,addgdown,corrdummy);

			if(tobeignored!=errn_pdf){
				totalgaussup2+=maxpdfup*maxpdfup;
				totalgaussdown2+=maxpdfdown*maxpdfdown;
			}
			if(tobeignored!=errn_alphas){
				totalgaussup2+=maxasup*maxasup;
				totalgaussdown2+=maxasdown*maxasdown;
			}
			if(tobeignored!=errn_addgaus){
				totalgaussup2+=maxaddup*maxaddup;
				totalgaussdown2+=maxadddown*maxadddown;
			}
		}


		if(scalegaus && tobeignored!=errn_scale){
			totalgaussup2 += maxscaleup*maxscaleup;
			totalgaussdown2 += maxscaledown*maxscaledown;
		}

		for(size_t j=1;j<h->getBinsY().size()-1;j++){
			float centery;
			h->getBinCenter(i,j,centerx,centery);
			double likelihood  =0;
			if(!scalegaus){
				if(((tobeignored==errn_scale)))
					throw std::runtime_error("top_prediction::exportLikelihood: scale uncertainty must not be ignored when using a box prior for it.");

				likelihood =  erf( (maxscaleup   + xsec - centery)/std::sqrt(2*totalgaussup2) );
				likelihood-=  erf( (maxscaledown + xsec - centery)/std::sqrt(2*totalgaussdown2) ) ;
				//likelihood*=1/(2* (maxscaleup -  maxscaledown));
				//will be normalized to 1 anyway
			}
			else{
				double diff=xsec-centery;
				if(diff>0)
					likelihood = exp(-0.5 * diff*diff/(totalgaussdown2) );
				else
					likelihood = exp(-0.5 * diff*diff/(totalgaussup2) );
			}
			//likelihood=std::abs(likelihood);
			if(likelihood<0 && likelihood >-0.01) //protect against small numbers
				likelihood=0;
			else if(likelihood<0)
				throw std::runtime_error("top_prediction::exportLikelihood:likelihood became significantly negative, check for large asymmetric uncertainties.");
			if(LHmax<likelihood)
				LHmax=likelihood;
			h->getBin(i,j).setContent(likelihood);
		}
		if(!scalegaus)
			linemax=erf( (maxscaleup   )/std::sqrt(2*totalgaussup2) ) - erf( (maxscaledown )/std::sqrt(2*totalgaussdown2) ) ;
		else
			linemax=1;
		for(size_t j=1;j<h->getBinsY().size()-1;j++){
			float normedcontent=h->getBin(i,j).getContent() / linemax;
			h->getBin(i,j).setContent(normedcontent);
		} //avoid dagostini bias
	}
	//*h*=1/LHmax; // make maximum 1
}


graph top_prediction::getDependence(double intervaldown, double intervalup, size_t npoints,error_enum error)const{
	if(npoints<1)
		return graph();
	graph out(npoints);
	double increaseint=(intervalup-intervaldown)/((double) npoints-1);
	for(size_t i=0;i<npoints;i++){
		double topmass=intervaldown + (double)i*increaseint;
		double xsec=getXsec(topmass, error);
		out.setPointXContent(i,topmass);
		out.setPointYContent(i,xsec);
	}
	return out;
}

}

