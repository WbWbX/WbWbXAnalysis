/*
 * ttbarXsecFitter.h
 *
 *  Created on: Oct 9, 2014
 *      Author: kiesej
 */

#ifndef TTBARXSECFITTER_H_
#define TTBARXSECFITTER_H_
#include <vector>
#include "TtZAnalysis/Tools/interface/variateContainer1D.h"
#include "TtZAnalysis/Tools/interface/simpleFitter.h"
#include "TtZAnalysis/Tools/interface/formatter.h"
#include "TtZAnalysis/Tools/interface/container2D.h"
#include "TtZAnalysis/Tools/interface/texTabler.h"
#include "TString.h"
#include "TRandom3.h"

namespace ztop{
class containerStack;

class ttbarXsecFitter {
public:

	enum likelihoodmodes{lhm_chi2datastat,lhm_chi2datamcstat,lhm_poissondatastat};

	enum priors{prior_gauss,prior_box,prior_float,prior_narrowboxleft,prior_narrowboxright,
		prior_parameterfixed};

	/**
	 * Lumi uncertainties in %, lumi in pb
	 */
	ttbarXsecFitter(double lumi8TeV, double unclumi8TeV, double lumi7TeV=0, double  unclumi7TeV=0):
		lumi8_(lumi8TeV),lumi7_(lumi7TeV),unclumi8_(unclumi8TeV),unclumi7_(unclumi7TeV),
		exclude0bjetbin_(false),
		xsecidx8_(9999999),lumiidx8_(9999999),xsecidx7_(9999999),lumiidx7_(9999999),
		ndependencies_(0),last8TeVentry_(9999999),
		lhmode_(lhm_chi2datamcstat),
		fitsucc_(false),norm_nbjet_global_(true),
		xsecoff8_(251.7),xsecoff7_(170),
		useMConly_(false),removesyst_(false),nominos_(false),
		random_(0),parameterwriteback_(true),
		nosystbd_(false),silent_(false)
	{container_c_b_.resize(2);container_eps_b_.resize(2);eps_emu_.resize(2);} //one for each energy

	~ttbarXsecFitter(){
		if(random_) delete random_;
	}
	/**
	 * This is the cross section used to normalize the signal
	 * MC in the input distributions for 8 TeV! It has to match exactly!
	 */
	void setXsec8(float xsec){xsecoff8_=xsec;}
	/**
	 * This is the cross section used to normalize the signal
	 * MC in the input distributions for 7 TeV! It has to match exactly!
	 */
	void setXsec7(float xsec){xsecoff7_=xsec;}
	/**
	 * Give uncertainty in % and lumi in pb
	 * It has to match exactly the lumi used to normalize the
	 * input MC
	 */
	void setLumi8(double lumi,double unc){lumi8_=lumi;unclumi8_=unc;}

	/**
	 * Give uncertainty in % and lumi in pb
	 * It has to match exactly the lumi used to normalize the
	 * input MC
	 */
	void setLumi7(double lumi,double unc){lumi7_=lumi;unclumi7_=unc;}

	/**
	 * sets the likelihood mode
	 */
	void setLikelihoodMode(likelihoodmodes mode){lhmode_=mode;}

	/**
	 * Excludes the 0,3+ bjet bin from the fit
	 */
	void setExcludeZeroBjetBin(bool excl){exclude0bjetbin_=excl;}


	/**
	 * defines a prior for a given syst variation
	 */
	void setPrior(const TString& sysname, priors prior);
	//read in functions here

	/**
	 * For this uncertainty, the full error will be added at the end!
	 */
	void addFullExtrapolError(const TString& systname);

	void setUseMCOnly(bool set){useMConly_=set;}

	void setNoMinos(bool nomin){nominos_=nomin;}

	/**
	 * removes systematics already at read-in
	 */
	void setRemoveSyst(bool remove){removesyst_=remove;}

	/**
	 *
	 */
	void setNoSystBreakdown(bool no){nosystbd_=no;}

	/**
	 * no fit output
	 */
	void setSilent(bool silent){silent_=silent;}

	/**
	 * just append all in one
	 */
	containerStack produceStack(bool fittedvalues,size_t bjetcat,bool eighttev,double& chi2)const;

	/**
	 *
	 */
	void setReplaceInInfile(const TString& repl,const TString& with){replaceininfiles_=std::pair<TString,TString>(repl,with);}

	/**
	 * Reads in the input from file
	 * leave some hardcoded parts here for now
	 */
	void readInput(const std::string & configfilename);

	/**
	 * ask if 7 TeV input has been performed
	 */
	bool has7TeV(){return last8TeVentry_<100;}
	/**
	 * creates all variate containers
	 * adds lumi uncertainty and "xsec" as syst
	 * returns DEVIATION of xsec from initial prior!!!
	 */
	int fit(float& xsec8,float&errup8,float&errdown8,float& xsec7,float&errup7,float&errdown7);
	int fit();
	//container2D getCorrelationCoefficients()const{return fitter_.getCorrelationCoefficients();}



	/*
	 * All to be used after the fit!
	 *
	 */
	std::vector<double> getParameters()const{return *fitter_.getParameters();}


	container1D getCb (bool fittedvalues, bool eighttev=true)const; //{if(eighttev) return container_c_b_.at(0); else return container_c_b_.at(1);}
	container1D getEps(bool fittedvalues, bool eighttev=true)const;

	container2D getCorrelations()const;

	double getXsec7()const{return fitter_.getParameters()->at(xsecidx8_)+xsecoff8_;}
	double getXsec8()const{return fitter_.getParameters()->at(xsecidx7_)+xsecoff7_;}

	size_t getNParameters()const{return fitter_.getParameters()->size();}

	double getParaError(size_t idx)const;
	TString getParaName(size_t idx)const;
	void getParaErrorContributionToXsec(size_t idx, double sevenoreight,double& up,double&down,bool& anticorr);
	size_t getXsecIdx(double sevenoreight)const;

	texTabler makeSystBreakdown(double sevenoreight);

	void createPseudoDataFromMC(container1D::pseudodatamodes mode=container1D::pseudodata_poisson);

	void createContinuousDependencies();

	bool includes7TeV()const;

	//just a debug / verbose switch
	static bool debug;

private:


	double lumi8_,lumi7_,unclumi8_,unclumi7_;

	//for each b-jet category
	// lumi8_, lumi7_ is fixed!
	// after last8TeVentry_ entries, it repeats for 7 TeV
	// strictly speaking this is just a number
	// there will be exactly 3! numbers per energy

	// maybe disentangle the normalization again to have the possibility to add any distribution
	// .. should be a good idea
	std::vector<extendedVariable> normalization_nbjet_;
	std::vector<extendedVariable> eps_emu_; //for later 0: 8 TeVm 1: 7TeV

	// will contain the signal shape, one for each b-jet category
	// it will have (appended) n_jets distributions
	// it includes the leptonic acceptance (essentially the term eps_emu)
	// lui uncertainty is added here
	std::vector<variateContainer1D>  signalshape_nbjet_;

	// "standard ditributions"

	//for each b-jet, ADDITIONAL jet category: background
	std::vector<variateContainer1D>  background_nbjet_;

	//for each b-jet, ADDITIONAL jet category: data
	std::vector<variateContainer1D>  data_nbjet_;


	//this is mainly for testing and to keep track NOT use in actual calc!
	std::vector<variateContainer1D> container_c_b_;
	std::vector<variateContainer1D> container_eps_b_;

	///saved for pseudoexperiments
	std::vector<container1D> signalconts_nbjets_;
	std::vector<container1D> signalcontsorig_nbjets_;
	std::vector<container1D> dataconts_nbjets_;
	std::vector<container1D> datacontsorig_nbjets_;
	std::vector<container1D> backgroundconts_nbjets_;
	std::vector<container1D> backgroundcontsorig_nbjets_;


	bool exclude0bjetbin_;

	size_t
	xsecidx8_,
	lumiidx8_,
	xsecidx7_,
	lumiidx7_,
	ndependencies_,
	last8TeVentry_;


	std::vector<double> fittedparas_;
	std::vector<TString> parameternames_;
	likelihoodmodes lhmode_;
	std::vector<priors> priors_;
	std::vector<size_t> addfullerrors_;
	simpleFitter fitter_;

	bool fitsucc_;

	bool norm_nbjet_global_;

	double xsecoff8_,xsecoff7_;

	double toBeMinimized(const double * variations);

	//just a safety check. Should be redundant as soon as class works and is tested
	void checkSizes()const;

	variateContainer1D createLeptonJetAcceptance(const std::vector<container1D>& signals, size_t bjetcategory, bool eighttev);

	//nbjet, njet
	//energy only as number, e.g. "8"
	// will be ordered in nbjets categories
	// different jet multiplicities are appended!
	std::vector<containerStack>   readStacks(const std::string  configfilename,const TString energy)const;


	//can be more sophisticated
	void addUncertainties(containerStack * stack,size_t nbjets,bool eighttev=true)const;

	//might come handy in some cases
	formatter format_;

	bool useMConly_,removesyst_,nominos_;

	TRandom3 * random_;
	bool parameterwriteback_;
	ROOT::Math::Functor functor_; //(this,&ttbarXsecFitter::toBeMinimized,ndependencies_);

	std::pair<TString,TString> replaceininfiles_;

	bool nosystbd_,silent_;

};

}
#endif /* TTBARXSECFITTER_H_ */
