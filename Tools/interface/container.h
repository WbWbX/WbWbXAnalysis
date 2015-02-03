#ifndef container_h
#define container_h

#include <vector>
#include "TH1D.h"
#include "TGraphAsymmErrors.h"
#include <iostream>
#include <math.h>
#include <utility>
#include "TPad.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include <algorithm>
#include <complex>
#include "histoContent.h"
#include "graph.h"
#include "taggedObject.h"
#include "extendedVariable.h"
#include "TRandom3.h"

////////bins include their border on the right side!!

#define MAKECOMPWITHOLD

class TFile;
class TTree;

#define container1D_partialvariationIDString "pvar_"

namespace ztop{

class container2D;
class containerStyle;
/**
 * Histogram class with built in N-Layers of systematics, operators that account for correlations
 * etc etc.
 * Try not to use the getTH1D... member functions and use classes inheriting from ztop::plotterBase instead
 */
class container1D:public taggedObject

{
	friend class container2D;
public:

	enum plotTag{none,topdf};


	container1D();
	container1D(float , TString name="",TString xaxisname="",TString yaxisname="", bool mergeufof=false);              //! construct with bin width (for dynamic filling - not yet implemented)
	container1D(const std::vector<float>& ,const TString& name="",const TString& xaxisname="",const TString& yaxisname="", bool mergeufof=false); //! construct with binning
	~container1D();
	container1D(const container1D&);
	container1D& operator=(const container1D&);

	/**
	 * creates a container with same syst, bin-boundaries
	 * all entries are 1, with 0 stat (useful algebraic for operations)
	 */
	container1D createOne()const;

	bool isDummy()const{return bins_.size()<=2;}

	void setNames(TString name, TString xaxis, TString yaxis){if(name!="")name_=name;if(xaxis!="")xname_=xaxis;if(yaxis!="")yname_=yaxis;}
	void setShowWarnings(bool show){showwarnings_=show;}

	void setXAxisName(const TString& name){xname_=name;}
	const TString& getXAxisName()const {return xname_;}

	void setYAxisName(const TString& name){yname_=name;}
	const TString& getYAxisName()const {return yname_;}


	void fill(const float & val, const float & weight=1);    //! fills with weight
	void fillDyn(float what, float weight=1); //! not implemented yet

	void setBins(std::vector<float>);
	void setBinWidth(float);            //! sets bin width; enables dynamic filling (not implemented yet)
	void setBinsFrom(const container1D&);

	void setBinErrorUp(const size_t&, const float&);    //! clears ALL systematics and adds a new one
	void setBinErrorDown(const size_t&, const float&);  //! clears ALL systematics and adds a new one
	void setBinError(const size_t&, const float&);      //! clears ALL systematics and adds a new one

	void setBinContent(const size_t&, const float&,const int &sysLayer=-1);    //! sets bin content, does not change error values!
	void setBinStat(const size_t &bin, const float & err, const int &sysLayer=-1); //! sets bin stat, does not change error values!
	void setBinEntries(const size_t &bin, const size_t & entries, const int &sysLayer=-1); //! sets bin stat, does not change error values!


	size_t getBinNo(const float&) const; //! returns bin index number for (float variable)
	size_t getNBins() const;       //! returns nuberof bins
	const std::vector<float> & getBins() const {return bins_;}

	const histoBin & getBin(const size_t & no, const int& layer=-1)const{return contents_.getBin(no,layer);}
	histoBin & getBin(const size_t & no, const int& layer=-1){return contents_.getBin(no,layer);}

	float getBinCenter(const size_t &) const;
	float getBinWidth(const size_t &) const; //! returns total bin width NOT 0.5* width

	const float &getXMax() const{return bins_[getNBins()+1];}
	const float &getXMin() const{return bins_[1];}

	size_t getSystSize() const{return contents_.layerSize();}

	const float & getBinContent(const size_t&,const int &sysLayer=-1) const;
	const size_t & getBinEntries(const size_t &,const int &sysLayer=-1) const;
	float  getBinStat(const size_t &,const int &sysLayer=-1) const;

	float  getBinErrorUp(const size_t&, bool onlystat=false,const TString& limittosys="") const; //returns DEVIATION!
	float  getBinErrorDown(const size_t&,bool onlystat=false,const TString&  limittosys="") const; //returns DEVIATION!
	float getBinError(const size_t&,bool onlystat=false,const TString&  limittosys="") const; //returns DEVIATION!

	float  getSystError(const size_t & number, const size_t & bin) const; //returns DEVIATION!
	float getSystErrorStat(const size_t & number, const size_t & bin) const; //returns DEVIATION!

	const TString & getSystErrorName(const size_t & number) const;
	const size_t & getSystErrorIndex(const TString & ) const;

	container1D breakDownSyst(const size_t & bin, bool updown) const;
	container1D sortByBinNames(const container1D & ) const;
	container1D getSystContainer(const int &syslayer)const;
	container1D getRelErrorsContainer()const;

	void removeStatFromAll();

	void createStatFromContent();

	/**
	 * merges partial variations. only the ones corresponding to the identifier are merged
	 * hint: do this step at the latest stage you can do it to still keep track of all correlations
	 * If you know its a global variation its ok to do it earlier
	 */
	void mergePartialVariations(const TString& identifier,bool linearly=false,bool strictpartialID=true);
	void mergeAllErrors(const TString & mergedname,bool linearly=false);
	/**
	 * names without "up" and "down"
	 */
	void mergeVariations(const std::vector<TString>& names, const TString & outname,bool linearly=false);

	void mergeVariationsFromFileInCMSSW(const std::string& filename);

	float getOverflow(const int& systLayer=-1) const;                              //!returns -1 if overflow was merged with last bin
	float getUnderflow(const int& systLayer=-1) const;                             //!returns -1 if underflow was merged with last bin

	float integral(bool includeUFOF=false,const int& systLayer=-1) const;
	float cIntegral(float from=0, float to=0,const int& systLayer=-1) const;       //! includes the full bin "from" or "to" is in

	float integralStat(bool includeUFOF=false,const int& systLayer=-1) const;
	float cIntegralStat(float from=0, float to=0,const int& systLayer=-1) const;       //! includes the full bin "from" or "to" is in

	size_t integralEntries(bool includeUFOF=false,const int& systLayer=-1) const;
	size_t cIntegralEntries(float from=0, float to=0,const int& systLayer=-1) const;       //! includes the full bin "from" or "to" is in


	/**
	 * creates container1D with one bin holding the integral
	 * including syst
	 * UF and OF will be kept
	 */
	container1D getIntegralBin()const;

	float getYMax(bool dividebybinwidth ,const int& systLayer=-1) const;
	float getYMin(bool dividebybinwidth, const int& systLayer=-1) const;

	float normalize(bool includeUFOF=true, bool normsyst=false, const float &normto=1);
	void normalizeToContainer(const container1D & ); /////////

	/**
	 * sets all bin contents to zero; clears all systematic uncertainties
	 * resets also binning, keeps name and axis names
	 */
	void reset();

	/**
	 * sets all bin contents to zero; clears all systematic uncertainties
	 * keeps binning
	 */
	void clear();

	/**
	 * sets all eintries to zero and all stat to zero
	 * all syst layers as well as the binning is kept
	 */
	void setAllZero();

	void setLabelSize(float size){labelmultiplier_=size;}       //! 1 for default
	TH1D * getTH1D(TString name="", bool dividebybinwidth=true, bool onlystat=false, bool nostat=false) const; //! returns a TH1D pointer with symmetrized errors (TString name); small bug with content(bin)=0 and error(bin)=0
	TH1D * getTH1DSyst(TString name, size_t systNo, bool dividebybinwidth=true, bool statErrors=false) const;


	/**
	 * imports TH1. Assumes errros to be stat errors! That means, bin entries are
	 * defined according to stat^2
	 */
	container1D & import(TH1 *,bool isbinwidthdivided=false);
	/**
	 * imports TGraphAsymmErrors. Only works if the x-axis errors indicate the bin boundaries.
	 * Y errors will be represented as new systematic layers, the stat error will be set to 0.
	 * The new systematic layers can be named- if not, default is Graphimp_<up/down>
	 */
	container1D & import(TGraphAsymmErrors *,bool isbinwidthdivided=false, const TString & newsystname="Graphimp");
	void writeTH1D(TString name="",bool dividebybinwidth=true, bool onlystat=false, bool nostat=false) const; //! writes TH1D->Write() with symmetrized errors (TString name)

	TGraphAsymmErrors * getTGraph(TString name="", bool dividebybinwidth=true,bool onlystat=false,bool noXErrors=false, bool nostat=false) const;
	TGraphAsymmErrors * getTGraphSwappedAxis(TString name="", bool dividebybinwidth=true,bool onlystat=false,bool noXErrors=false, bool nostat=false) const;
	//operator TGraphAsymmErrors() const {return *getTGraph();}
	//operator TGraphAsymmErrors*() const {return getTGraph();}
	void writeTGraph(TString name="",bool dividebybinwidth=true,bool onlystat=false, bool noXErrors=false, bool nostat=false) const; //! writes TGraph to TFile (must be opened)

	graph convertToGraph(bool dividebybinwidth=true)const;

	void drawFullPlot(TString name="", bool dividebybinwidth=true,const TString &extraoptions="");

	void setDivideBinomial(bool);                                   //! default true
	void setMergeUnderFlowOverFlow(bool merge){mergeufof_=merge;}   //! merges underflow/overflow in first or last bin, respectively has to be executed before filling to have effect

	container1D & operator += (const container1D &);       //!< adds stat errors in squares; treats same named systematics as correlated!!
	container1D operator + (const container1D &)const;       //!< adds stat errors in squares; treats same named systematics as correlated!!
	container1D & operator -= (const container1D &);       //!< adds stat errors in squares; treats same named systematics as correlated!!
	container1D operator - (const container1D &)const;       //!< adds errors in squares; treats same named systematics as correlated!!
	container1D & operator /= (const container1D &);       //!< adds stat errors in squares; treats same named systematics as correlated!!
	container1D operator / (const container1D &)const;       //!< binomial stat error or uncorr error (depends on setDivideBinomial()); treats same named systematics as correlated
	container1D & operator *= (const container1D &);       //!< adds stat errors in squares; treats same named systematics as correlated!!
	container1D operator * (const container1D &)const;       //!< adds stat errors in squares; treats same named systematics as correlated!!
	container1D & operator *= (float);            //!< simple scalar multiplication. stat and syst errors are scaled accordingly!!
	container1D operator * (float)const;            //!< simple scalar multiplication. stat and syst errors are scaled accordingly!!
	container1D & operator *= (double val){return *this *=(float)val;}           //!< simple scalar multiplication. stat and syst errors are scaled accordingly!!
	container1D operator * (double val)const{return *this*(float)val;}           //!< simple scalar multiplication. stat and syst errors are scaled accordingly!!

	void sqrt();

	container1D chi2container(const container1D&,size_t * ndof=0)const;
	float chi2(const container1D&,size_t * ndof=0)const;

	/**
	 * cuts everything on the right of the input value (bin boundary chosen accorind to getBinNo())
	 * and returns new container.
	 */
	container1D cutRight(const float & )const;

	/**
	 * compares bins, bin contents and all uncertainties
	 */
	bool operator == (const container1D &)const;

	void divideByBinWidth();


	/**
	 * retuns a vector of congruent bin boundries between *this container and input
	 */
	std::vector<float> getCongruentBinBoundaries(const container1D &) const;
	/**
	 * do not include OF UF bins here
	 */
	container1D rebinToBinning( std::vector<float>  newbins) const;
	/**
	 * attempts to rebin container to the binning of rhs
	 * returns the rebinned container
	 */
	container1D rebinToBinning(const container1D &rhs) const;
	/**
	 * rebins by merging <merge> bins
	 * e.g. merge=2 merges 8 bins to 4
	 *
	 */
	container1D rebin(size_t merge) const;


	int addErrorContainer(const TString &, container1D, float);  //!< adds deviation to (this) as systematic uncertianty with name and weight. name must be ".._up" or ".._down"
	int addErrorContainer(const TString &,const container1D&);        //!< adds deviation to (this) as systematic uncertianty with name. name must be ".._up" or ".._down"

	/**
	 * deletes the systematics and adds relative contributions from input
	 */
	void getRelSystematicsFrom(const container1D &);
	/**
	 * adds relative systematics from input to the set of systematics if they don't exist
	 * if they already exist, they are left untouched
	 * In case <strict> is false and the systematic variation is de facto 0, only a new layer is added
	 * as a plain copy of the nominal layer
	 */
	void addRelSystematicsFrom(const container1D &,bool ignorestat=false,bool strict=false);
	void addGlobalRelErrorUp(const TString &,const float &);    //!< adds a global relative uncertainty with name; "..up" automatically added
	void addGlobalRelErrorDown(const TString &,const float &);  //!< adds a global relative uncertainty with name; "..down" automatically added
	void addGlobalRelError(const TString &,const float &);      //!< adds a global relative symmetric uncertainty with name; creates ".._up" and ".._down" variation names

	void removeError(const TString &); //!< removes a systematic uncertainty with name ..
	void removeError(const size_t &); //!< removes a systematic uncertainty with idx ..
	void renameSyst(const TString &,const  TString&); //!< rename from para1 to para2

	/**
	 * transforms the stat uncertainty to a systematic uncertainty
	 * para1: name of new uncertainty
	 */
	void transformStatToSyst(const TString&);
	/**
	 * transform stat uncertainty
	 * uses stat errors on nominal, transforms them into syst and cuts at 1 and 0
	 * the new systematic uncertainty will be named binom_error
	 */
	void transformToEfficiency();

	/**
	 * sets all errors (sys and stat) to 0
	 * but doesn't remove layers
	 * if nominalstat=false, it keeps stat of nominal
	 * syst will be exact copies of nominal
	 */
	void setAllErrorsZero(bool nominalstat=true); //! sets all errors zero
	/**
	 * removes all syst uncertainties, but leaves stat uncertainties of nominal untouched
	 */
	void removeAllSystematics(){contents_.removeAdditionalLayers();manualerror_=false;}

	enum pseudodatamodes{pseudodata_poisson,pseudodata_gaus};
	/**
	 * creates a new container. Based on the statistics of the initial (or given) container,
	 * a new Random distribution is created. Please parse the random generator for this, to
	 * assure proper seeds etc.
	 * The syst. entry on applies to THIS container, not the parsed one
	 *
	 * Mode can be set to poisson (default) or gauss (mostly for checks)
	 */
	container1D createPseudoExperiment(TRandom3* rand,const container1D* c=0, pseudodatamodes mode=pseudodata_poisson,int syst=-1)const;


	/**
	 * bool whether a newly created container will be added to a static list of all containers
	 */
	static bool c_makelist;
	/**
	 * the static list of containers
	 */
	static std::vector<container1D*> c_list;
	/**
	 * deletes all containers in static list
	 * be careful here
	 */
	static void c_deletelist(){
		size_t count=0,listsize=c_list.size();
		for(unsigned int i=0;i<listsize;i++){
			if(c_list[count]) delete c_list[count]; //delete also removes entry from list
			else count++; //that should never happen if list not touched from outside
		}
	}
	/**
	 * debug bool, switches on additional printout
	 */
	static bool debug;
	/**
	 * sets defaults for operators (+,-,...) as far as handling of statistical uncertainties
	 * are concerned
	 */
	static void setOperatorDefaults();

	/**
	 * This is just for testing!!!
	 */
	static container1D createRandom(size_t nbins,size_t distr=0,size_t n=1000,size_t seed=12345);

	/**
	 * to make fast euql distant bins
	 */
	static std::vector<float> createBinning(size_t nbins, float first, float last);

	static void equalizeSystematics(std::vector<container1D>&,std::vector<container1D>&);

	/**
	 * prints the full content in each bin
	 */
	void coutFullContent() const;

	/**
	 * checks whether both containers have the same layers (systamtic uncertainties)
	 */
	bool hasSameLayers(const container1D&) const;
	/**
	 * checks whether two containers have the smae layers AND the same ordering
	 */
	bool hasSameLayerOrdering(const container1D&) const;
	/**
	 * this only merges layers, it does not change any content of input or *this
	 * if a layer is added, it will be a copy of nominal (with stat error=0)
	 * return: map of (new) *this syst indices to rhs indices
	 */
	std::map<size_t,size_t> mergeLayers(container1D& rhs){return contents_.mergeLayers(rhs.contents_);}

	/**
	 * This function equalizes the association of systematic indices between rhs and *this
	 * if an uncertainty does not exist, it is copied from nominal
	 * The new ordering will be derived  from rhs mostly, fully if rhs has a superset of
	 * systematics
	 */
	void equalizeSystematicsIdxs(container1D &rhs);

	/**
	 * returns a list of all systematic names
	 */
	std::vector<TString> getSystNameList()const{return contents_.getVariations();}

	/**
	 * prints the bin content for bin <bin> and optionally adds a unit <unit>
	 */
	TString coutBinContent(size_t bin,const TString & unit="") const;

	/**
	 * additional tag for plotting. Not used right now
	 */
	plotTag plottag;

	/*
	 * high level stuff e.g. transforming particular systematic to a container y(x)=y(variation)
	 * should work for variations named VARIATION_<value>_<up/down>
	 * other uncertainties stay untouched and are just propagated to each variation
	 * if variation doesn't hold a value, +-1 is assumed
	 */

	/**
	 * returns a graph that describes the dependence of a bin on a systematic variation <sys>
	 * the name of the systematic will correspond to the output graphs x-axis
	 * it can be replaced by <replacename>
	 */
	ztop::graph getDependenceOnSystematic(const size_t& bin, TString  sys,float offset=0,TString replacename="")const;


	/**
	 * appends a container to the last bin
	 * will screw up real binning, but might come handy
	 * overflow and underflow bins will be represented by intermediate bins
	 */
	container1D append(const container1D&)const;
	/**
	 * loads a container from a TTree
	 */
	void loadFromTree(TTree *, const TString & plotname);
	/**
	 * loads a container from a TFile *
	 */
	void loadFromTFile(TFile *, const TString & plotname);
	/**
	 * directly reads from a TFile
	 */
	void loadFromTFile(const TString& filename, const TString & plotname);

	/**
	 * writes object to a TTree
	 */
	void writeToTree(TTree *);
	/**
	 * writes object to a TFile
	 */
	void writeToTFile(TFile *);
	/**
	 * writes object to a Root file
	 */
	void writeToTFile(const TString& filename);





protected:
	//whenever you add a member, make sure it turns up in at least the copy and == operator


	bool showwarnings_; //!< switch to add more printout
	float binwidth_; //!< not used yet
	bool canfilldyn_;//!< comes with binwidth_. not used right now

	bool manualerror_; //!< boolean to check/set if errors were created using setBinError<>() functions

	std::vector<float> bins_; //!< vector that describes binning. includes UF OF
	histoContent contents_; //!< content of the bins incl syst


	bool mergeufof_; //!< boolean: decides if fills go to UF/OF or to first/last visible bin
	bool wasunderflow_;//!< set to true ones one fill went to underflow
	bool wasoverflow_;//!< set to true ones one fill went to overflow


	TString  xname_, yname_;

	float labelmultiplier_;//!< multiplier to labels when plotting, becomes less and les important with plotter classes inheriting from plotterBase


	TGraphAsymmErrors * gp_;//!< this is only for interactive usage to keep track on newly created plots
	TH1D * hp_;//!< this is only for interactive usage to keep track on newly created plots

	TString stripVariation(const TString&) const;//!< strips the <up> or <down>
	float getDominantVariationUp( TString ,const size_t &) const;//!< gets dominant variation larger than nominal content for syst = para1, bin=para2
	float getDominantVariationDown( TString ,const size_t &) const;//!< gets dominant variation smaller than nominal content for syst = para1, bin=para2

	float sq(const float&) const; //!< helper

	void createManualError();//!< helper

	void copyFrom(const container1D&);//!< helper

};

} //namespace


//some more operators
ztop::container1D operator * (float multiplier, const ztop::container1D & cont);
ztop::container1D operator * (float multiplier, const ztop::container1D & cont);  //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
ztop::container1D operator * (int multiplier, const ztop::container1D & cont);    //! simple scalar multiplication. stat and syst errors are scaled accordingly!!

inline float ztop::container1D::sq(const float& a) const{
	return a*a;
}

inline size_t ztop::container1D::getBinNo(const float & var) const{

	if(bins_.size()<1){
		return 0;
	}
	if(bins_.size() <2){
		return 0;
	}
	std::vector<float>::const_iterator it=std::lower_bound(bins_.begin()+1, bins_.end(), var);
	if(var==*it)
		return it-bins_.begin();
	else
		return it-bins_.begin()-1;

}


inline void ztop::container1D::fill(const float & what, const float & weight){
	size_t bin=getBinNo(what);
	//int bin=0;

	if(mergeufof_){
		if(bin==0 && bins_.size() > 1){
			bin=1;
			wasunderflow_=true;
		}
		else if(bin==bins_.size()-1){
			bin--;
			wasoverflow_=true;
		}
	}
	histoBin * b=&contents_.getBin(bin);
	b->addToContent(weight);
	b->addEntry();
	b->addToStat(weight);
}
inline void ztop::container1D::fillDyn(float what, float weight){
	if(canfilldyn_){
		what++;
		weight++; //just avoid warnings function not implemented yet. creates new bins based on binwidth_ if needed
	}
	else if(showwarnings_){
		std::cout << "dynamic filling not available with fixed bins!" << std::endl;
	}
}




#endif
