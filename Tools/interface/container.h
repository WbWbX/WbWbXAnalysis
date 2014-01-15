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

////////bins include their border on the right side!!



namespace ztop{
class container2D;
class containerStyle;

class container1D{
	friend class container2D;
public:

	enum plotTag{none,topdf};

	container1D();
	container1D(float , TString name="",TString xaxisname="",TString yaxisname="", bool mergeufof=false);              //! construct with bin width (for dynamic filling - not yet implemented)
	container1D(std::vector<float> , TString name="",TString xaxisname="",TString yaxisname="", bool mergeufof=false); //! construct with binning
	~container1D();
	container1D(const container1D&);
	container1D& operator=(const container1D&);

	bool isDummy()const{return bins_.size()<2;}

	const TString & getName()const{return name_;}
	void setName(TString Name){name_=Name;}
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

	float getOverflow(const int& systLayer=-1) const;                              //!returns -1 if overflow was merged with last bin
	float getUnderflow(const int& systLayer=-1) const;                             //!returns -1 if underflow was merged with last bin

	float integral(bool includeUFOF=false,const int& systLayer=-1) const;
	float cIntegral(float from=0, float to=0,const int& systLayer=-1) const;       //! includes the full bin "from" or "to" is in

	float integralStat(bool includeUFOF=false,const int& systLayer=-1) const;
	float cIntegralStat(float from=0, float to=0,const int& systLayer=-1) const;       //! includes the full bin "from" or "to" is in

	size_t integralEntries(bool includeUFOF=false,const int& systLayer=-1) const;
	size_t cIntegralEntries(float from=0, float to=0,const int& systLayer=-1) const;       //! includes the full bin "from" or "to" is in

	float getYMax(bool dividebybinwidth ,const int& systLayer=-1) const;
	float getYMin(bool dividebybinwidth, const int& systLayer=-1) const;

	float normalize(bool includeUFOF=true, const float &normto=1);
	void normalizeToContainer(const container1D & ); /////////


	void reset();    //! resets all uncertainties and binning, keeps names and axis
	void clear();    //! sets all bin contents to zero; clears all systematic uncertainties

	void setLabelSize(float size){labelmultiplier_=size;}       //! 1 for default
	TH1D * getTH1D(TString name="", bool dividebybinwidth=true, bool onlystat=false, bool nostat=false) const; //! returns a TH1D pointer with symmetrized errors (TString name); small bug with content(bin)=0 and error(bin)=0
	TH1D * getTH1DSyst(TString name, size_t systNo, bool dividebybinwidth=true, bool statErrors=false) const;
	//operator TH1D() const {TH1D* h=getTH1D();TH1D hr=*h;delete h;return hr;}
	// bad practise operator TH1D*(){return getTH1D();}
	//container1D & operator = (const TH1D &);
	container1D & import(TH1 *,bool isbinwidthdivided=false);
	void writeTH1D(TString name="",bool dividebybinwidth=true, bool onlystat=false, bool nostat=false) const; //! writes TH1D->Write() with symmetrized errors (TString name)

	TGraphAsymmErrors * getTGraph(TString name="", bool dividebybinwidth=true,bool onlystat=false,bool noXErrors=false, bool nostat=false) const;
	TGraphAsymmErrors * getTGraphSwappedAxis(TString name="", bool dividebybinwidth=true,bool onlystat=false,bool noXErrors=false, bool nostat=false) const;
	//operator TGraphAsymmErrors() const {return *getTGraph();}
	//operator TGraphAsymmErrors*() const {return getTGraph();}
	void writeTGraph(TString name="",bool dividebybinwidth=true,bool onlystat=false, bool noXErrors=false, bool nostat=false) const; //! writes TGraph to TFile (must be opened)

	graph convertToGraph(bool dividebybinwidth=true)const;

	void drawFullPlot(TString name="", bool dividebybinwidth=true,const TString &extraoptions="");

	void setDivideBinomial(bool);                                   //! default true
	void setMergeUnderFlowOverFlow(bool merge){mergeufof_=merge;}   //! merges underflow/overflow in first or last bin, respectively

	container1D & operator += (const container1D &);       //! adds stat errors in squares; treats same named systematics as correlated!!
	container1D operator + (const container1D &);       //! adds stat errors in squares; treats same named systematics as correlated!!
	container1D & operator -= (const container1D &);       //! adds stat errors in squares; treats same named systematics as correlated!!
	container1D operator - (const container1D &);       //! adds errors in squares; treats same named systematics as correlated!!
	container1D & operator /= (const container1D &);       //! adds stat errors in squares; treats same named systematics as correlated!!
	container1D operator / (const container1D &);       //! binomial stat error or uncorr error (depends on setDivideBinomial()); treats same named systematics as correlated
	container1D & operator *= (const container1D &);       //! adds stat errors in squares; treats same named systematics as correlated!!
	container1D operator * (const container1D &);       //! adds stat errors in squares; treats same named systematics as correlated!!
	container1D & operator *= (float);            //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
	container1D operator * (float);            //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
	container1D & operator *= (double val){return *this *=(float)val;}           //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
	container1D operator * (double val){return *this*(float)val;}           //! simple scalar multiplication. stat and syst errors are scaled accordingly!!


	//void mergeBins(size_t ,size_t );
	std::vector<float> getCongruentBinBoundaries(const container1D &) const;
	container1D rebinToBinning(const std::vector<float> & newbins) const;
	container1D rebinToBinning(const container1D &) const;
	container1D rebin(size_t merge) const;

	void addErrorContainer(const TString &,const container1D&, float);  //! adds deviation to (this) as systematic uncertianty with name and weight. name must be ".._up" or ".._down"
	void addErrorContainer(const TString &,const container1D&);        //! adds deviation to (this) as systematic uncertianty with name. name must be ".._up" or ".._down"
	void getRelSystematicsFrom(const container1D &);
	void addGlobalRelErrorUp(const TString &,const float &);    //! adds a global relative uncertainty with name; "..up" automatically added
	void addGlobalRelErrorDown(const TString &,const float &);  //! adds a global relative uncertainty with name; "..down" automatically added
	void addGlobalRelError(const TString &,const float &);      //! adds a global relative symmetric uncertainty with name; creates ".._up" and ".._down" variation names

	void removeError(const TString &); //! removes a systematic uncertainty with name ..
	void removeError(const size_t &); //! removes a systematic uncertainty with idx ..
	void renameSyst(const TString &,const  TString&); //! old, new

	void transformStatToSyst(const TString&);
	void setAllErrorsZero(){contents_.removeAdditionalLayers();contents_.clearLayerStat(-1);} //! sets all errors zero
	void removeAllSystematics(){contents_.removeAdditionalLayers();}

	static bool c_makelist;
	static std::vector<container1D*> c_list;
	static void c_clearlist(){c_list.clear();}
	static bool debug;

	static void setOperatorDefaults();

	void coutFullContent() const;

	bool hasSameLayers(const container1D&) const;
	bool hasSameLayerOrdering(const container1D&) const;

	TString coutBinContent(size_t bin) const;

	plotTag plottag;

	/*
	 * high level stuff e.g. transforming particular systematic to a container y(x)=y(variation)
	 * should work for variations named VARIATION_<value>_<up/down>
	 * other uncertainties stay untouched and are just propagated to each variation
	 * if variation doesn't hold a value, +-1 is assumed
	 */

	container1D getDependenceOnSystematicC(const size_t& bin,const TString & sys)const;
	ztop::graph getDependenceOnSystematic(const size_t& bin, TString  sys,float offset=0,TString replacename="")const;

protected:
	bool showwarnings_;
	float binwidth_;
	bool canfilldyn_;

	bool manualerror_;

	std::vector<float> bins_;
	histoContent contents_;


	bool mergeufof_;
	bool wasunderflow_;
	bool wasoverflow_;


	TString name_, xname_, yname_;

	float labelmultiplier_;

	//not optimal and a problem when copying etc but better than nothing for fast interactive usage
	TGraphAsymmErrors * gp_;
	TH1D * hp_;


	TString stripVariation(const TString&) const;
	float getDominantVariationUp( TString ,const size_t &) const;
	float getDominantVariationDown( TString ,const size_t &) const;

	float sq(const float&) const; //helper

	void createManualError();

	void copyFrom(const container1D&);

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
