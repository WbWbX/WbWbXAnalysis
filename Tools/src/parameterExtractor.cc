/*
 * parameterExtractor.cc
 *
 *  Created on: Jan 6, 2014
 *      Author: kiesej
 */



#include "../interface/parameterExtractor.h"
#include "TCanvas.h"
#include <iostream>
#include <stdexcept>
#include "TGraphAsymmErrors.h"
#include "TVirtualFitter.h"
#include "TFitResult.h"
#include <cmath>
#include <algorithm>
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"

namespace ztop{

bool parameterExtractor::debug=false;

bool parameterExtractor::checkSizes(){
	bool check=agraphs_.size() == bgraphs_.size();
	if(!check){
		std::cout << "parameterExtractor::checkSizes: Input vectors don't match in size" << std::endl;
	}
	return check;
}
/**
 * includes range check. returns 0 if something is wrong
 */
size_t parameterExtractor::getSize(){
	if(!checkSizes())
		return 0;
	else return agraphs_.size();
}
std::vector<graph> parameterExtractor::createLikelihoods(){
	if(debug)
		std::cout << "parameterExtractor::createLikelihoods" << std::endl;
	fittedgraphsa_.resize(agraphs_.size(),graph());
	fittedgraphsb_.resize(bgraphs_.size(),graph());
	std::vector<graph> out;
	size_t size=getSize();
	for(size_t i=0;i<size;i++){
		if(selfheal_
				&& std::find(nangraphs_.begin(),nangraphs_.end(),i) != nangraphs_.end())
			continue; //self healing

		out.push_back(createLikelihood(i));
	}
	return out;
}
float parameterExtractor::getLikelihoodMinimum(size_t idx, float * statlow, float*stathigh,bool global, graph* graphLH, TF1* togetFunc){
	if(debug)
		std::cout << "parameterExtractor::getLikelihoodMinimum" << std::endl;
	if(idx>=getSize() && ! global)
		throw std::out_of_range("parameterExtractor::getLikelihoodMinimum index out of range");
	graph temp(1);
	if(!global){
		temp=createLikelihood(idx);}
	else{
		std::vector<graph> binnedlh=createLikelihoods();
		if(binnedlh.size()>0)
			temp=binnedlh.at(0);
		for(size_t i=1;i<binnedlh.size();i++){
			temp=temp.addY(binnedlh.at(i));
		}
	}
	if(graphLH)
		*graphLH=temp;

	/*
	TCanvas ct;
	temp.getTGraph("",true)->Draw("AP");
	ct.Print("gllh.pdf");
	exit(EXIT_FAILURE);
	 */
	//fit a function here! polN with N being high. enough points so as high as possible



	//TGraphAsymmErrors * gtofit=temp.getTGraph("tmp",true);
	/* later
	    ROOT::Math::Functor f(this,&ttbarXsecExtractor::getChi2,fitter_.getParameters()->size());

	    fitter_.setMinFunction(&f);

	 */
	//debug using TF1

	//	std::cout << "hello " << temp.getNPoints()<<std::endl; //DEBUG
	graph shifted=temp;
	size_t minpointidx=0;
	temp.getYMin(minpointidx,false);

	if(minpointidx<1 || temp.hasNans()){ //problem with minimum, wrong likelihood or something. let the main program handle that!
		if(stathigh)
			*stathigh=-1;
		if(statlow)
			*statlow=-1;
		return 0;
	}

	//	std::cout << "minp idx "<<minpointidx; //DEBUG
	float shift=temp.getPointXContent(minpointidx);
	//	std::cout << "shift: " << shift<<std::endl; //DEBUG
	shifted.shiftAllXCoordinates(- shift);
	double sxmin=shifted.getPointXContent(0);
	double sxmax=shifted.getPointXContent(shifted.getNPoints()-1);
	//	std::cout << "min max: " << sxmin << " " <<sxmax <<std::endl; //DEBUG

	TGraphAsymmErrors * gtofit=shifted.getTGraph("tmp",true);
	TF1* ffunc = new TF1("fita","pol4",sxmin,sxmax);

	ffunc->SetParameter(0,shifted.getYMin());
	ffunc->SetLineColor(kRed);
	gtofit->Fit(ffunc,"VRQ");
	delete TVirtualFitter::GetFitter();

	size_t searchinterval= shifted.getNPoints()*0.05;
	//	std::cout << "searchint: " << searchinterval <<std::endl; //DEBUG
	if(searchinterval<5){
		searchinterval=5;
		if(searchinterval + minpointidx >= shifted.getNPoints()) searchinterval=shifted.getNPoints()-minpointidx-1;
		if(searchinterval + granularity_ - minpointidx <= granularity_)searchinterval=minpointidx-1;
	}


	size_t searchintleft=shifted.getPointXContent(minpointidx-searchinterval);
	size_t searchintright=shifted.getPointXContent(minpointidx+searchinterval);

	//this may all be done algebraically!

	float ymin =ffunc->GetMinimum(searchintleft,searchintright);
	// EvalPar(&x,0)
	float Xminimum=ffunc->GetX(ymin,searchintleft,searchintright);
	if(statlow){
		float xpleft=ffunc->GetX(ymin+1,sxmin,Xminimum);
		*statlow=fabs(Xminimum-xpleft);
	}
	if(stathigh){
		float xpright=ffunc->GetX(ymin+1,Xminimum,sxmax);
		*stathigh=fabs(xpright-Xminimum);
	}

	/*
	TCanvas ct;
	gtofit->Draw("AP");
	ffunc->Draw("same");
	ct.Print("debug_globalLH.pdf");
	exit(EXIT_FAILURE);
	 */
	if(togetFunc)
		togetFunc=ffunc;
	else
		delete ffunc;
	return Xminimum+shift;


	/*
		size_t difftog=0,leftpointn=0,rightpointn=0;;
		size_t coordssize=temp.getNPoints();
		while(difftog<g){
			if(!leftpointn && g-difftog>0){
				float yleft=temp.getPointYContent(g-difftog);
				if(yleft>yminimum+1){
					leftpointn=g-difftog;
	 *statlow=fabs(temp.getPointXContent(leftpointn) - xatymin);
				}
			}
			if(!rightpointn && g+difftog<coordssize){
				float yright=temp.getPointYContent(g+difftog);
				if(yright>yminimum+1){
					rightpointn=g+difftog;
	 *stathigh=fabs(temp.getPointXContent(rightpointn) - xatymin);

				}
			}
			if(rightpointn && leftpointn) break;
			++difftog;
		}

	 */

}

//////////protected member funcs


graph parameterExtractor::createLikelihood(size_t idx){
	if(debug)
		std::cout << "parameterExtractor::createLikelihood "<< idx << std::endl;

	if(fittedgraphsa_.size() != agraphs_.size()){
		fittedgraphsa_.resize(agraphs_.size());
	}
	if(fittedgraphsb_.size() != bgraphs_.size()){
		fittedgraphsb_.resize(bgraphs_.size());
	}

	if(LHMode_==lh_chi2){
		graph out= createChi2Likelihood(agraphs_.at(idx),bgraphs_.at(idx));
		if(std::find(nangraphs_.begin(),nangraphs_.end(),idx) == nangraphs_.end()
				&& out.hasNans())
			nangraphs_.push_back(idx);
		return out;
	}
	else if(LHMode_==lh_chi2Swapped){
		graph out=createChi2SwappedLikelihood(agraphs_.at(idx),bgraphs_.at(idx));
		if(std::find(nangraphs_.begin(),nangraphs_.end(),idx) == nangraphs_.end()
				&& out.hasNans())
			nangraphs_.push_back(idx);
		return out;
	}
	else if(LHMode_==lh_fit){
		bool hasnan=false;
		graph out=createIntersectionLikelihood(agraphs_.at(idx),bgraphs_.at(idx),
				fittedgraphsa_.at(idx),fittedgraphsb_.at(idx),hasnan);
		if(std::find(nangraphs_.begin(),nangraphs_.end(),idx) == nangraphs_.end()
				&& hasnan){
			nangraphs_.push_back(idx);
		}
		return out;
	}
	else if(LHMode_==lh_fitintersect){
		bool hasnan=false;
		graph out=createIntersectionLikelihood(agraphs_.at(idx),bgraphs_.at(idx)
				,fittedgraphsa_.at(idx),fittedgraphsb_.at(idx),hasnan);
		if(hasnan&& std::find(nangraphs_.begin(),nangraphs_.end(),idx) == nangraphs_.end()){
			nangraphs_.push_back(idx);
		}
		return out;
	}

	/*  other modes  */

	std::cout << "parameterExtractor::createLikelihood: likelihood definition not found" << std::endl;
	throw std::runtime_error("parameterExtractor::createLikelihood: likelihood definition not found");
}
/*
void parameterExtractor::drawFittedFunctions(size_t idx)const{
    if(LHMode_!=lh_fit) return;
    if(idx >= fittedFunctionsa_.size() || idx >= fittedFunctionsb_.size()){
        throw std::out_of_range("parameterExtractor::drawFittedFunctions: out of range");
    }
    for(size_t i=0;i<fittedFunctionsa_.at(idx).size();i++)
        fittedFunctionsa_.at(idx).at(i)->Draw("same");
    for(size_t i=0;i<fittedFunctionsb_.at(idx).size();i++)
        fittedFunctionsb_.at(idx).at(i)->Draw("same");

}
 */

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Helpers //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

void parameterExtractor::checkChi2Conditions(const graph& g,const graph& gcompare){
	if(g.getNPoints()!=gcompare.getNPoints()){
		std::cout << "parameterExtractor::checkChi2Conditions: needs same number of points" << std::endl;
		throw std::logic_error("parameterExtractor::checkChi2Conditions: needs same number of points" );
	}
	//only performing loose x check
	for(size_t i=0;i<g.getNPoints();i++){
		if(g.getPointXContent(i) != gcompare.getPointXContent(i)){
			std::cout << "parameterExtractor::checkChi2Conditions: needs same x coordinates" << std::endl;
			throw std::logic_error("parameterExtractor::checkChi2Conditions: needs same x coordinates" );
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// IMPLEMETATION OF LIKELIHOODS ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////


graph parameterExtractor::createChi2Likelihood(const graph& g,const graph& gcompare){
	checkChi2Conditions(g,gcompare);


	graph out=g;//get number of points
	out.clear();//clear
	//return out;//////////
	size_t nans=0;
	//compare systematics by systematics if present -> later
	//first ignore syst correlations
	//treat everything in g ang gcompare as uncorrelated

	for(size_t p=0;p<g.getNPoints();p++){
		float diff=g.getPointYContent(p)-gcompare.getPointYContent(p);
		float err2=0;

		float lerr=g.getPointYError(p,false);
		float rerr=gcompare.getPointYError(p,false);
		err2=lerr*lerr+rerr*rerr;

		out.setPointXContent(p,g.getPointXContent(p));
		float chi2point=diff*diff/err2;
		if(chi2point!=chi2point){
			chi2point=0;
			nans++;
		}
		out.setPointYContent(p,chi2point);
	}
	if(nans>0){
		std::cout << "parameterExtractor::createChi2Likelihood: nans created. ";
		if(nans == out.getNPoints())
			std::cout << " All chi2=0, keep in mind for ndof "<<std::endl;
		else
			std::cout << " Only some chi2=0, WARNING!!! Result will not be useful!" <<std::endl;
	}

	out.setYAxisName("#chi^{2}");
	return out;

}


graph parameterExtractor::createChi2SwappedLikelihood(const graph& g,const graph& gcompare){
	checkChi2Conditions(g,gcompare);

	graph out=g;//get number of points
	out.clear();//clear
	//return out;//////////
	size_t nans=0;
	//compare systematics by systematics if present -> later
	//first ignore syst correlations
	//treat everything as uncorrelated
	for(size_t p=0;p<g.getNPoints();p++){
		float diff=g.getPointYContent(p)-gcompare.getPointYContent(p);
		float err2=0;

		if(diff>0){ //use left down, right up
			float lerr=g.getPointYErrorDown(p,false);
			float rerr=gcompare.getPointYErrorUp(p,false);
			err2=lerr*lerr+rerr*rerr;
		}
		else{
			float lerr=g.getPointYErrorUp(p,false);
			float rerr=gcompare.getPointYErrorDown(p,false);
			err2=lerr*lerr+rerr*rerr;
		}


		out.setPointXContent(p,g.getPointXContent(p));
		float chi2point=diff*diff/err2;
		if(chi2point!=chi2point){
			chi2point=0;
			nans++;
		}
		out.setPointYContent(p,chi2point);
	}
	if(nans>0){
		std::cout << "graph::getChi2Points: nans created. ";
		if(nans == out.getNPoints())
			std::cout << " All chi2=0, keep in mind for ndof "<<std::endl;
		else
			std::cout << " Only some chi2=0, WARNING!!! Result will not be useful!" <<std::endl;
	}

	out.setYAxisName("#chi^{2}");
	return out;
}


double parameterExtractor::findintersect(TF1* a, TF1* b,float min, float max){//,float& devleft,float& devright) {

	double gran=(1/(double)granularity_);
	double out=-1;
	double tempmin=-1;
	for(double x=min;x<=max;x+=gran){
		double test=fabs(a->EvalPar(&x,0) - b->EvalPar(&x,0));
		//  std::cout << test << std::endl;
		if(tempmin <0 || test<tempmin){
			out=x;
			tempmin=test;
		}
	}
	return out;
}

graph parameterExtractor::createIntersectionLikelihood(const graph& aIn,const graph& bIn,graph& fitteda,graph& fittedb,bool & hasnan){
	if(debug)
		std::cout << "createIntersectionLikelihood" <<std::endl;
	if(fitfunctiona_=="" || fitfunctionb_=="" ){
		throw std::logic_error("parameterExtractor::createIntersectionLikelihood: needs fit function");
	}
	if(debug)
		std::cout << "createIntersectionLikelihood: for "<< aIn.getName()<<" and " << bIn.getName() <<std::endl;

	graph a=aIn;
	graph b=bIn;

	double meanya=(a.getYMax()+a.getYMin())/2;
	double meanyb=(b.getYMax()+b.getYMin())/2;
	double xmin=std::min(a.getXMin(),b.getXMin());
	double xmax=std::max(a.getXMax(),b.getXMax());
	float xshift = (xmax+xmin)/2;
	float yshift=(meanya+meanyb)/2;
	//preapre for better fitting


	a.shiftAllXCoordinates(-xshift);
	b.shiftAllXCoordinates(-xshift);

	a.shiftAllYCoordinates(-yshift);
	b.shiftAllYCoordinates(-yshift);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// SHIFTED COORDINATES ///////////////////////////////////////////
	////////////////////////////////////////////////////// START  //////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//recalculate and extend range a bit - xmin and ymin are now neagtive!
	//mean y should be around 0
	meanya=0;
	meanyb=0;
	xmin=std::min(a.getXMin()*(1.5),b.getXMin()*(1.5)); //some extrapolation
	xmax=std::max(a.getXMax()*(1.5),b.getXMax()*(1.5));

	TGraphAsymmErrors * agraph=a.getTGraph("a",false);
	TGraphAsymmErrors * bgraph=b.getTGraph("b",false);
	if(!agraph || !bgraph){
		throw std::logic_error("parameterExtractor::createIntersectionLikelihood: needs valid input graphs");

	}

	std::vector<TF1* >  funcsa,funcsb;

	tmpfa_ = new TF1("fita",fitfunctiona_,xmin,xmax);
	tmpfa_->SetLineColor(kBlack);
	funcsa.push_back(tmpfa_);

	tmpfb_ = new TF1("fitb",fitfunctionb_,xmin,xmax);
	tmpfb_->SetLineColor(kRed);
	funcsb.push_back(tmpfb_);

	size_t nxpoints=granularity_+1;

	std::vector<double> confintva(nxpoints);
	std::vector<double> confintvb(nxpoints);
	std::vector<double> xpoints(nxpoints);

	double interval=((xmax-xmin)/((double)(granularity_)));

	//xpoints.at(0)=xmin;

	std::vector<float> scancurvea(nxpoints);
	std::vector<float> scancurveb(nxpoints);

	for(size_t i=0;i<xpoints.size();i++){
		xpoints.at(i)=xmin+ ((double)i)*interval;
	}

	std::cout << "GetConfidenceIntervals" <<std::endl; //DEBUG

	bool afitsucc=agraph->Fit(tmpfa_,"VRQS")->IsValid();

	(TVirtualFitter::GetFitter())->GetConfidenceIntervals(nxpoints,1,&xpoints.at(0),&confintva.at(0),clfit_);
	//sotre fit parameter for 2nd pol
	if(fitfunctiona_=="pol2"){
		std::vector<double> paras(5);
		paras.at(0) = tmpfa_->GetParameter(0);
		paras.at(1) = tmpfa_->GetParameter(1);
		paras.at(2) = tmpfa_->GetParameter(2);
		paras.at(3) = xshift;
		paras.at(4) = yshift;
		fittedparasa_.push_back(paras);
	}

	delete TVirtualFitter::GetFitter();
	bool bfitsucc=bgraph->Fit(tmpfb_,"VRQS")->IsValid();
	if(fitfunctiona_=="pol2"){
		std::vector<double> paras(5);
		paras.at(0) = tmpfb_->GetParameter(0);
		paras.at(1) = tmpfb_->GetParameter(1);
		paras.at(2) = tmpfb_->GetParameter(2);
		paras.at(3) = xshift;
		paras.at(4) = yshift;
		fittedparasb_.push_back(paras);
	}



	(TVirtualFitter::GetFitter())->GetConfidenceIntervals(nxpoints,1,&xpoints.at(0),&confintvb.at(0),clfit_);
	delete TVirtualFitter::GetFitter();
	graph fita(nxpoints);
	graph fitb(nxpoints);
	graph multiplied(nxpoints);

	for(size_t it=0;it<xpoints.size();it++){
		float aval=tmpfa_->EvalPar(&xpoints.at(it),0);
		float bval=tmpfb_->EvalPar(&xpoints.at(it),0);
		fita.setPointContents(it,true,xpoints.at(it),aval);
		fitb.setPointContents(it,true,xpoints.at(it),bval);
	}

	//start of new implementation

	//dont get too nummerical, because:
	//int_-inf^inf ( normgaus(x,mu1,s1) * normgaus(x,mu2,s2) ) = normgaus(mu1,mu2,s1+s2)

	/*  float maxconfa=*std::max_element(confintva.begin(),confintva.end());
    float maxconfb=*std::max_element(confintvb.begin(),confintvb.end());
    float integralwidth2=maxconfa*maxconfa+maxconfb*maxconfb;
	 */

	//get average stat uncertainty
	float minstata=10000;
	float minstatb=10000;
	double shiftbyscalea=-1;
	double shiftbyscaleb=-1;
	float minpointa=0;
	float minpointb=0;

	//  if(fituncmodea_ == fitunc_statcorrgaus){
	for(size_t i=0;i<aIn.getNPoints();i++){
		float tmpmin=aIn.getPointYStat(i);///aIn.getPointYContent(i);
		if(minstata > tmpmin){
			if(aIn.getPointYContent(i)==0) continue;
			minstata=tmpmin;
			shiftbyscalea=minstata*minstata/(aIn.getPointYContent(i));
			minpointa=aIn.getPointYContent(i);

		}
	}
	// }
	// if(fituncmodeb_ == fitunc_statcorrgaus){
	for(size_t i=0;i<bIn.getNPoints();i++){
		float tmpmin=bIn.getPointYStat(i);///bIn.getPointYContent(i);
		if(minstatb > tmpmin){
			if(bIn.getPointYContent(i)==0) continue;
			minstatb=tmpmin;
			shiftbyscaleb=minstatb*minstatb/(bIn.getPointYContent(i));
			minpointb=bIn.getPointYContent(i);

		}
	}
	//}
	// minstatb*=minstatcontentb; //avoid bias towards low values
	// minstata*=minstatcontenta;

	std::cout << "\n\n scale a: " << shiftbyscalea << std::endl;
	std::cout << "\n\n scale b: " << shiftbyscaleb << std::endl;

	if( !afitsucc || !bfitsucc){//something is wrong! just return flat likelihood
		for(size_t i=0;i<xpoints.size();i++){
			multiplied.setPointContents(i,true,xpoints.at(i),1);
			multiplied.shiftAllXCoordinates(xshift);


		}
		std::cout << "parameterExtractor::createIntersectionLikelihood: something went wrong. returning flat likelihood!" <<std::endl;

		return multiplied;
	}
	if(shiftbyscalea<0){
		shiftbyscalea=fallbackscalea_;
		std::cout << "WARNING: was not able to extract scale for A, probably a 0 entry! fall back to fallBackScale "
				<< fallbackscalea_<<std::endl;
		minstata=1;
		minpointa=1;
	}
	if(shiftbyscaleb<0){
		shiftbyscaleb=fallbackscaleb_;
		std::cout << "WARNING: was not able to extract scale for B, probably a 0 entry! fall back to fallBackScale "
				<< fallbackscaleb_<<std::endl;
		minstatb=1;
		minpointb=1;
	}

	// evaluate for each scan point
	//points are rescaled to "normal" statistics
	if(LHMode_ == lh_fit){
		for(size_t i=0;i<xpoints.size();i++){
			float aval=fita.getPointYContent(i)+yshift;//tmpfa_->EvalPar(&xpoints.at(i),0) +yshift;
			float bval=fitb.getPointYContent(i)+yshift;//tmpfb_->EvalPar(&xpoints.at(i),0) +yshift;
			//in case of normalized, scaled... distr.

			//for drawing
			if(fituncmodea_ == fitunc_statcorrgaus){
				confintva.at(i)=minstata; //unscaled
			}
			if(fituncmodeb_ == fitunc_statcorrgaus){
				confintvb.at(i)=minstatb;//unscaled
			}

			bool usechi2=true;
			float chi2=1;

			if(fituncmodeb_ == fitunc_statuncorrpoisson){
				if(fituncmodea_ == fitunc_statuncorrpoisson){
					throw std::logic_error("parameterExtractor::createIntersectionLikelihood: \
                            can only define one of the inputs as prior (fitunc_statuncorrpoisson)");
				}
				if(fituncmodea_ != fitunc_statcorrgaus){
					throw std::logic_error("parameterExtractor::createIntersectionLikelihood: \
					                            right now needs one stat correlated for fitunc_statuncorrpoisson");
				}

				//scale everything to real number of events level
				//prevents stat function to run out of range (it can only do small shifts)
				/*			const float realbval=bval/shiftbyscalea;
				const float equivbstat2=bval /shiftbyscalea;
				const float intrinsicbstat2=bval/shiftbyscaleb;
				const float realaval=aval/shiftbyscalea;
				 */

				//do a scaling here to stay in parameter range. scale is of the order of normalization
				//the scaling does not affect the outcome and is a purely technical step

				//in case the fit goes below 0, a cutoff is introduced
				if(bval<0) bval=0;
				if(aval<0) aval=0;
				//bval=0;//DEBUG

				const float realbval=bval                             / shiftbyscalea;
				const float equivbstat=(bval * minstata/minpointa)    / shiftbyscalea;
				const float intrinsicbstat=(bval* minstatb/minpointb) / shiftbyscalea;
				const float realaval=aval                             / shiftbyscalea;

				chi2 = -2* shiftedLnPoissonMCStat(realbval,
						equivbstat,
						intrinsicbstat,
						realaval);
				//scale back for graph

				confintvb.at(i)=sqrt(equivbstat*equivbstat
						+ intrinsicbstat*intrinsicbstat) * shiftbyscalea;//+intrinsicbstat2);
				confintva.at(i)=0;
				usechi2=false;

			}
			if(fituncmodea_ == fitunc_statuncorrpoisson){
				if(fituncmodeb_ == fitunc_statuncorrpoisson){
					throw std::logic_error("parameterExtractor::createIntersectionLikelihood: \
                                       can only define one of the inputs as prior (fitunc_statuncorrpossion)");
				}
				if(fituncmodeb_ != fitunc_statcorrgaus){
					throw std::logic_error("parameterExtractor::createIntersectionLikelihood: \
									                            right now needs one stat correlated for fitunc_statuncorrpoisson");
				}
				//scale everything to real number of events level
				//prevents stat function to run out of range (it can only do small shifts)

				//in case the fit goes below 0, a cutoff is introduced
				if(bval<0) bval=0;
				if(aval<0) aval=0;

				throw std::logic_error("TBI!!!");

				const float realaval=aval/shiftbyscaleb;
				const float equivastat2=aval /shiftbyscaleb;
				const float intrinsicastat2=aval/shiftbyscalea;
				const float realbval=bval/shiftbyscaleb;

				chi2 = -2* shiftedLnPoissonMCStat(realaval,
						equivastat2,
						intrinsicastat2,
						realbval);
				//scale back for graph
				confintva.at(i)=sqrt(equivastat2*shiftbyscaleb*shiftbyscaleb + intrinsicastat2*shiftbyscalea*shiftbyscalea);//+intrinsicbstat2);
				confintvb.at(i)=0;
				usechi2=false;

			}


			//else
			if(usechi2){

				float widtha=confintva.at(i);
				float widthb=confintvb.at(i);
				chi2=chi_square(aval,widtha*widtha+widthb*widthb,bval);//);
				if(chi2!=chi2 && ! selfheal_){
					std::cout << "Nan produced! aval(reshifted): "<< aval
							<< " bval(reshifted): " <<bval <<std::endl;
					throw std::runtime_error("NAN! in fit");

				}
			}
			if(chi2!=chi2){
				hasnan=true;
				break;
			}

			fita.setPointYStat(i,confintva.at(i));
			fitb.setPointYStat(i,confintvb.at(i));


			multiplied.setPointContents(i,true,xpoints.at(i),chi2);

		}
		if(hasnan)
		{
			for(size_t p=0;p<xpoints.size();p++)
				multiplied.setPointContents(p,true,xpoints.at(p),1);
		}
	}


	//  float tempmin=-1000;

	std::vector<float> centralpoints;



	//reduce number in representation graphs if granularity is high

	float maxgran=20;

	if(granularity_ < maxgran || fastmode_){
		fitteda=(fita);
		fittedb=(fitb);
	}
	else{
		size_t granscaler=(float)granularity_/maxgran;
		size_t indexer=0,newsize=0;;

		for(size_t oldpoint=0;oldpoint<fita.getNPoints();oldpoint++){
			if(oldpoint == indexer){
				indexer+=granscaler;
				newsize++;
			}
		}
		indexer=0;
		fitteda=graph(newsize);
		fittedb=graph(newsize);
		if(debug)
			std::cout << "parameterExtractor::createIntersectionLikelihood: created graphs to represent confidence interval with less granularity (" <<  newsize<<")" <<std::endl;
		size_t newpointidx=0;
		for(size_t oldpoint=0;oldpoint<fita.getNPoints();oldpoint++){
			if(oldpoint == indexer){

				fitteda.setPointContents(newpointidx,true,fita.getPointXContent(oldpoint),fita.getPointYContent(oldpoint));
				fitteda.setPointYStat(newpointidx,fita.getPointYStat(oldpoint));
				fittedb.setPointContents(newpointidx,true,fitb.getPointXContent(oldpoint),fitb.getPointYContent(oldpoint));
				fittedb.setPointYStat(newpointidx,fitb.getPointYStat(oldpoint));
				indexer+=granscaler;
				newpointidx++;
			}
		}
	}

	fitteda.shiftAllXCoordinates(xshift);
	fittedb.shiftAllXCoordinates(xshift);

	fitteda.shiftAllYCoordinates(yshift);
	fittedb.shiftAllYCoordinates(yshift);

	multiplied.shiftAllXCoordinates(xshift);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// SHIFTED COORDINATES ///////////////////////////////////////////
	//////////////////////////////////////////////////////   END  //////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	delete tmpfa_;delete tmpfb_;
	return multiplied;




}

double parameterExtractor::lnNormedGaus(const float & centre, const float& widthsquared, const float& evalpoint,const float& extnorm)const{
	if(extnorm!=0){
		return (log(1/extnorm) + (-((long double)(evalpoint-centre)*(long double)(evalpoint-centre))/((long double)(2*widthsquared))));
	}
	else{//calculate on the fly in max precision
		return (log(1/sqrt(2*(3.14159265358979323846)*widthsquared)) + (-((long double)(evalpoint-centre))*((long double)(evalpoint-centre))/((long double)(2*widthsquared))));
	}


}
double parameterExtractor::chi_square(const float & centre, const float& widthsquared, const float& evalpoint) const{
	return (double) (((long double)(evalpoint-centre)*(long double)(evalpoint-centre))/((long double)(widthsquared)));
}




}//namespace
