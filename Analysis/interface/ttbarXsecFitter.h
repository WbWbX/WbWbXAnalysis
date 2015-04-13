/*
 * ttbarXsecFitter.h
 *
 *  Created on: Oct 9, 2014
 *      Author: kiesej
 */

#ifndef TTBARXSECFITTER_H_
#define TTBARXSECFITTER_H_
#include <vector>
#include "TtZAnalysis/Tools/interface/variateHisto1D.h"
#include "TtZAnalysis/Tools/interface/simpleFitter.h"
#include "TtZAnalysis/Tools/interface/formatter.h"
#include "TtZAnalysis/Tools/interface/histo2D.h"
#include "TtZAnalysis/Tools/interface/texTabler.h"
#include "TString.h"
#include "TRandom3.h"
#include "TtZAnalysis/Configuration/interface/version.h"

namespace ztop{
class histoStack;

class ttbarXsecFitter {
public:

	enum likelihoodmodes{lhm_chi2datastat,lhm_chi2datamcstat,lhm_poissondatastat};

	enum priors{prior_gauss,prior_box,prior_float,prior_narrowboxleft,prior_narrowboxright,
		prior_parameterfixed};

	/**
	 * Lumi uncertainties in %, lumi in pb
	 */
	ttbarXsecFitter():
		exclude0bjetbin_(false),
		ndependencies_(0),
		lhmode_(lhm_chi2datamcstat),
		fitsucc_(false),norm_nbjet_global_(true),
		useMConly_(false),removesyst_(false),nominos_(false),
		parameterwriteback_(true),
		nosystbd_(false),silent_(false),nopriors_(false),topmassrepl_(-100)
	{
	} //one for each energy

	~ttbarXsecFitter(){
		if(random_) delete random_;
	}

	/**
	 * sets the likelihood mode
	 */
	void setLikelihoodMode(likelihoodmodes mode){lhmode_=mode;}

	/**
	 * Excludes the 0,3+ bjet bin from the fit
	 */
	void setExcludeZeroBjetBin(bool excl){exclude0bjetbin_=excl;}

	//read in functions here


	void setUseMCOnly(bool set){useMConly_=set;}

	void setNoMinos(bool nomin){nominos_=nomin;}



	/**
	 * removes systematics already at read-in
	 */
	void setRemoveSyst(bool remove){removesyst_=remove;}

	/**
	 *
	 */
	void setIgnorePriors(bool ignore){nopriors_=ignore;}

	/**
	 * Only total unceratinty (and extrapolation errors) will be calculated
	 */
	void setNoSystBreakdown(bool no){nosystbd_=no;}

	/**
	 * no fit output
	 */
	void setSilent(bool silent){silent_=silent;}

	/**
	 * just append all in one
	 */
	histoStack produceStack(bool fittedvalues,size_t bjetcat,size_t datasetidx,double& chi2)const;

	/**
	 *
	 */
	void setReplaceInInfile(const TString& repl,const TString& with){replaceininfiles_=std::pair<TString,TString>(repl,with);}

	void setReplaceTopMass(float mass){topmassrepl_=mass;}

	/**
	 * Reads in the input from file
	 * leave some hardcoded parts here for now
	 */
	void readInput(const std::string & configfilename);


	/**
	 * creates all variate containers
	 * adds lumi uncertainty and "xsec" as syst
	 * returns DEVIATION of xsec from initial prior!!!
	 */
	int fit(std::vector<float>& xsecs,std::vector<float>& errup,std::vector<float>& errdown);
	int fit();
	//histo2D getCorrelationCoefficients()const{return fitter_.getCorrelationCoefficients();}



	/*
	 * All to be used after the fit!
	 *
	 */
	std::vector<double> getParameters()const{return *fitter_.getParameters();}


	histo1D getCb (bool fittedvalues,size_t datasetidx)const; //{if(eighttev) return container_c_b_.at(0); else return container_c_b_.at(1);}
	histo1D getEps(bool fittedvalues,size_t datasetidx)const;

	histo2D getCorrelations()const;

	double getXsec(size_t datasetidx)const;
	double getXsecOffset(size_t datasetidx)const;
	double getVisXsec(size_t datasetidx)const;

	double getXsecRatio(size_t datasetidx1,size_t datasetidx2, double & errup, double& errdown)const;

	size_t getNParameters()const{return fitter_.getParameters()->size();}

	double getParaError(size_t idx)const;
	TString getParaName(size_t idx)const;
	/**
	 * if idx < 0 -> stat error
	 */
	void getParaErrorContributionToXsec(int idx, size_t  datasetidx,double& up,double&down,bool& anticorr);

	void createSystematicsBreakdowns();
	void createSystematicsBreakdownsMerged();

	texTabler makeSystBreakDownTable(size_t datasetidx,bool detailed=true);

	texTabler makeCorrTable() const;

	graph getResultsGraph(size_t idx,const float x_coord)const;

	void createPseudoDataFromMC(histo1D::pseudodatamodes mode=histo1D::pseudodata_poisson);

	void createContinuousDependencies();

	size_t nDatasets()const;
	const TString& datasetName(size_t datasetidx)const;

	//just a debug / verbose switch
	static bool debug;


	histoStack applyParametersToStack(const histoStack& stack, size_t bjetcat, size_t datasetidx, bool fitted)const;

private:

	void createSystematicsBreakdown(size_t datasetidx);

	/**
	 * For this uncertainty, the full error will be added at the end!
	 */
	void addFullExtrapolError(const TString& systname);
	/**
	 * defines a prior for a given syst variation
	 */
	void setPrior(const TString& sysname, priors prior);

	class dataset{
	public:

		struct systematic_unc{
			TString name;
			double pull;
			double constr;
			double errup;
			double errdown;
		};

		dataset(double lumi,double lumiunc, double xsecin, TString name):
			lumi_(lumi),xsecoff_(xsecin),unclumi_(lumiunc),
			lumiidx_(9999),xsecidx_(9999),name_(name),totalvisgencontsread_(0)
		{}

		extendedVariable& eps_emu(){return eps_emu_;}
		extendedVariable& acceptance_extr(){return acceptance_extr_;}
		extendedVariable& acceptance(){return acceptance_;}
		const extendedVariable& eps_emu()const {return eps_emu_;}
		const extendedVariable& acceptance_extr()const{return acceptance_extr_;}
		const extendedVariable& acceptance()const{return acceptance_;}

		extendedVariable& omega_b(size_t nbjet){return omega_nbjet_.at(nbjet);}
		const extendedVariable& omega_b(size_t nbjet)const {return omega_nbjet_.at(nbjet);}

		variateHisto1D& signalshape(size_t nbjet){return signalshape_nbjet_.at(nbjet);}
		const variateHisto1D& signalshape(size_t nbjet)const {return signalshape_nbjet_.at(nbjet);}
		variateHisto1D& background(size_t nbjet){return background_nbjet_.at(nbjet);}
		const variateHisto1D& background(size_t nbjet)const {return background_nbjet_.at(nbjet);}
		variateHisto1D& data(size_t nbjet){return data_nbjet_.at(nbjet);}
		const variateHisto1D& data(size_t nbjet)const {return data_nbjet_.at(nbjet);}



		variateHisto1D& container_c_b(){return container_c_b_;}
		const variateHisto1D& container_c_b()const {return container_c_b_;}
		variateHisto1D& container_eps_b(){return container_eps_b_;}
		const variateHisto1D& container_eps_b()const {return container_eps_b_;}

		const TString & getName()const{return name_;}
		const double & lumi()const{return lumi_;}
		const double & xsecOffset()const{return xsecoff_;}
		const size_t & xsecIdx()const;
		void createXsecIdx();

		void readStack(const histoStack& stack, size_t nbjet);
		void readStackVec(const std::vector<histoStack>& stacks, size_t nbjet);

		void readStacks(const std::string  configfilename,const std::pair<TString,TString>&,bool,std::vector<std::pair<TString, double> >&);
		//also takes care of proper asso to lumiidx,xsecidx
		void equalizeIndices(dataset & rhs);
		/**
		 * implies that rhs has the same and more (or equal) entries
		 */
		void adaptIndices(const dataset & rhs);

		void createPseudoDataFromMC(histo1D::pseudodatamodes mode=histo1D::pseudodata_poisson);
		void createContinuousDependencies(bool,const std::vector<size_t>& );

		std::vector<TString> getSystNames()const{
			if(signalconts_nbjets_.size()<1)
				throw std::logic_error("ttbarXsecFitter::datasets::getSystNames: nothing read in yet");
			return signalconts_nbjets_.at(0).getSystNameList();
		}

		void checkSizes()const;
		bool readyForFit()const{return signalshape_nbjet_.size()>0;}

		static const size_t nBjetCat(){return 3;} //to avoid duplication

		void resetTotVisGenCount(){totalvisgencontsread_=0;}

		std::vector<systematic_unc>& postFitSystematicsFull(){return post_fit_systematics_;}
		const std::vector<systematic_unc>& postFitSystematicsFull() const{return post_fit_systematics_;}
		std::vector<systematic_unc>& postFitSystematicsFullSimple(){return post_fit_systematics_simple_;}
		const std::vector<systematic_unc>& postFitSystematicsFullSimple() const{return post_fit_systematics_simple_;}

		void addUncertainties(histoStack * stack,size_t nbjets,bool removesyst,const std::vector<std::pair<TString, double> >& )const;

		double signalIntegral(size_t nbjets) const{return signalintegral_.at(nbjets);}

	private:

		variateHisto1D createLeptonJetAcceptance(const std::vector<histo1D>& signals,
				const std::vector<histo1D>& signalpsmig,
				const std::vector<histo1D>& signalvisPSgen,
				size_t bjetcategory, std::vector<size_t> addfullerrs);

		dataset():totalvisgencontsread_(0){}

		double lumi_,xsecoff_;
		double unclumi_;
		size_t lumiidx_,xsecidx_;
		TString name_;

		//global per dataset dependence on extrapolation unc are set to 0 here
		extendedVariable eps_emu_;
		//global per dataset dependence on extrapolation unc (and all others) are implemented here
		extendedVariable acceptance_,acceptance_extr_;

		//per bjet_cat
		std::vector<extendedVariable> omega_nbjet_;
		//per b-jet cat (includes jet categories)
		std::vector<variateHisto1D>  signalshape_nbjet_;
		std::vector<variateHisto1D>  background_nbjet_;
		std::vector<variateHisto1D>  data_nbjet_;

		//for later use. BEFORE fit!
		std::vector<double> signalintegral_;

		///for checks
		variateHisto1D container_c_b_;
		variateHisto1D container_eps_b_;

		/*
		 * includes events from PS migrations
		 */
		std::vector<histo1D> signalconts_nbjets_;
		std::vector<histo1D> signalcontsorig_nbjets_;

		std::vector<histo1D> signalvisgenconts_nbjets_;

		size_t totalvisgencontsread_; //this is NOT signalvisgenconts_nbjets_.size() since some could be merged!

		std::vector<histo1D> signalpsmigconts_nbjets_;
		std::vector<histo1D> signalpsmigcontsorig_nbjets_;

		std::vector<histo1D> dataconts_nbjets_;
		std::vector<histo1D> datacontsorig_nbjets_;
		std::vector<histo1D> backgroundconts_nbjets_;
		std::vector<histo1D> backgroundcontsorig_nbjets_;

		std::vector<systematic_unc> post_fit_systematics_,post_fit_systematics_simple_;


	};


	std::vector<dataset> datasets_;

	bool exclude0bjetbin_;

	size_t
	ndependencies_;


	std::vector<double> fittedparas_;
	std::vector<TString> parameternames_;
	likelihoodmodes lhmode_;
	std::vector<priors> priors_;
	std::vector<size_t> addfullerrors_;
	simpleFitter fitter_;

	bool fitsucc_;

	bool norm_nbjet_global_;

	double toBeMinimized(const double * variations);

	//just a safety check. Should be redundant as soon as class works and is tested
	void checkSizes()const;

	double getExtrapolationError(size_t datasetidx, size_t paraidx, bool up);

	//can be more sophisticated

	TString translatePartName(const formatter& fmt,const TString& name)const;

	//might come handy in some cases
	formatter format_;

	bool useMConly_,removesyst_,nominos_;

	static TRandom3 * random_;
	bool parameterwriteback_;
	ROOT::Math::Functor functor_; //(this,&ttbarXsecFitter::toBeMinimized,ndependencies_);

	std::pair<TString,TString> replaceininfiles_;

	//needs to be adapted (?) for more than 2 datasets
	std::vector<std::pair<TString, double> > priorcorrcoeff_;

	bool nosystbd_,silent_,nopriors_;
float topmassrepl_;
};

}
#endif /* TTBARXSECFITTER_H_ */
