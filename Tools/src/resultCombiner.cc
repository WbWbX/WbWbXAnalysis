/*
 * resultCombiner.c
 *
 *  Created on: Aug 19, 2014
 *      Author: kiesej
 */



#include "../interface/resultCombiner.h"

namespace ztop{



void resultCombiner::addInput(const container1D& cont){
	if(distributions_.size()>0){
		if(distributions_.at(0).bins() != cont.getBins()){
			throw std::logic_error("resultCombiner::addInput: inputs must have same binning");
		}
		if(!temp_.hasSameLayers(cont)){
			throw std::logic_error("resultCombiner::addInput: inputs must have same systematics and ordering!");
		}
	}

	temp_=cont;
	variateContainer1D temp;
	temp.import(cont);
	distributions_.push_back(temp);
	sysforms_.resize(distributions_.at(0).getNDependencies(), rc_sysf_gaus);
}

void resultCombiner::setSystForm(const TString& sys,rc_sys_forms form){
	if(distributions_.size()<1){
		throw std::logic_error("resultCombiner::setSystForm: first fill input distributions!");
	}


	std::vector<TString> sysnames=distributions_.at(0).getSystNames();
	for(size_t i=0;i<sysnames.size();i++){
		if(sysnames.at(i) == sys){
			sysforms_.at(i) = form;
			return;
		}
	}
	throw std::logic_error("resultCombiner::setSystForm: systematics name not found!");

}


void resultCombiner::reset(){

}

bool resultCombiner::minimize(){
	if(distributions_.size()<1)
		return false;
	//configure fitter
	std::vector<double> startparas(distributions_.at(0).getNDependencies() ,0);

	//make sure to treat UF OF properly

	double integral=0;
	//add mean values as parameters to be extracted:
	for(size_t i=0;i<distributions_.at(0).bins().size();i++){ //no UF OF
		double mean=0;
		for(size_t j=0;j<distributions_.size();j++){
			mean+=distributions_.at(j).getBin(i)->getNominal();

		}
		mean/=(double)distributions_.size();
		//	std::cout << "mean in bin " << i << ": " << mean << std::endl;
		integral+= distributions_.at(0).getBin(i)->getNominal();
		startparas.push_back(mean);
	}

	forcednorm_=integral;

	std::vector<double> stepwidths;
	stepwidths.resize(startparas.size(),1e-10);
	std::vector<TString> paranames=distributions_.at(0).getSystNames();

	fitter_.setParameterNames(paranames);
	fitter_.setRequireFitFunction(false);
	fitter_.setParameters(startparas,stepwidths);

	fitter_.setMinimizer(simpleFitter::mm_minuit2);
	fitter_.setMaxCalls(4e6);
	//fitter_.setTolerance(0.5);//0.1);
	//first do a simple scan


	ROOT::Math::Functor f(this,&resultCombiner::getChi2,fitter_.getParameters()->size());

	fitter_.setMinFunction(&f);

	simpleFitter::printlevel=0;

	fitter_.setTolerance(0.0001);
	fitter_.fit();



	//feed back
	std::vector<double> fittedbins=*fitter_.getParameters();
	size_t nsys=distributions_.at(0).getNDependencies();
	fittedbins.erase(fittedbins.begin(),fittedbins.begin() + nsys);

	std::vector<double> fittederrsup=*fitter_.getParameterErrUp();
	fittederrsup.erase(fittederrsup.begin(),fittederrsup.begin() + nsys);
	std::vector<double> fittederrsdown=*fitter_.getParameterErrDown();
	fittederrsdown.erase(fittederrsdown.begin(),fittederrsdown.begin() + nsys);


	std::vector<float> bins = distributions_.at(0).bins();
	bins.erase(bins.begin());

	combined_=container1D(bins);
	combined_.setName("combined");
	combined_.addGlobalRelErrorUp("combined_err",0);
	combined_.addGlobalRelErrorDown("combined_err",0);

	for(size_t i=0;i<combined_.getBins().size();i++){
		combined_.setBinContent(i,fittedbins.at(i));

		combined_.setBinContent(i,fittedbins.at(i)+fittederrsup.at(i),0);
		combined_.setBinContent(i,fittedbins.at(i)+fittederrsdown.at(i),1);
	}
	return fitter_.wasSuccess();

}

void resultCombiner::coutSystBreakDownInBin(size_t idx)const{
	if(distributions_.size()<1) return;
	if(combined_.isDummy()) return;


	float avgerror=(fabs(combined_.getBinErrorUp(idx,false))+fabs(combined_.getBinErrorDown(idx,false)))/2;
	float content=combined_.getBinContent(idx);

	for(size_t i=0;i<getFitter()->getParameters()->size() - combined_.getBins().size();i++){
		float breakdown=0;
		breakdown=sqrt(sq(avgerror)-sq(avgerror * sqrt(1-sq(getFitter()->getCorrelationCoefficient(distributions_.at(0).getNDependencies()+idx,i)))));
		breakdown/=content;
		breakdown*=100;
		std::cout  << getFitter()->getParameters()->at(i) << "\t"<< getFitter()->getParameterErr(i) << "\t"
				<<getFitter()->getParameterNames()->at(i)<< "\t"<< breakdown<<"%" <<std::endl;

	}


}


double resultCombiner::getNuisanceLogGaus(const double & in)const{
	return in*in;
}
double resultCombiner::getNuisanceLogBox(const double & in)const{
	double disttoone=1-fabs(in);
	if(disttoone>0) return 0;
	return (1e3*disttoone*disttoone);
}




double resultCombiner::getChi2(const double * variations){
	double chi2=0;
	const double * combbincontents=variations;
	combbincontents += distributions_.at(0).getNDependencies();

	//for norm use  forcednorm_ later and for last bin = forcednorm_-allbinssum
	for(size_t meas=0;meas<distributions_.size();meas++){
		double binsum=0;
		for(size_t i=0;i<distributions_.at(0).bins().size();i++){
			double binerror=distributions_.at(meas).getBinErr(i);
			if(binerror==0){
				chi2+=sq(combbincontents [i]*100); //let them go to zero
				continue;
			}
			if(additionalconstraint_==rc_addc_normalized){
				if(i<distributions_.at(0).bins().size()-1){
					chi2+= sq( combbincontents [i] - distributions_.at(meas).getBin(i)->getValue(variations))
											/ sq(binerror); //this is gaussian
					binsum+=distributions_.at(meas).getBin(i)->getValue(variations);
				}
				else{
					chi2+= sq( combbincontents [i] - binsum)
											/ sq(binerror); //this is gaussian
				}
			}
			else{
				chi2+= sq( combbincontents [i] - distributions_.at(meas).getBin(i)->getValue(variations)) / sq(binerror) ; //this is gaussian
			}
		}
	}
	//	std::cout <<"chi2 " << chi2 << std::endl;
	//	return chi2;
	//nuisance parameters
	for(size_t i=0;i<distributions_.at(0).getNDependencies();i++){
		if(sysforms_.at(i) == rc_sysf_gaus)
			chi2+=getNuisanceLogGaus(variations[i]);
		else if(sysforms_.at(i) == rc_sysf_box)
			chi2+=getNuisanceLogBox(variations[i]);
	}
	return chi2;
}


}//ns
