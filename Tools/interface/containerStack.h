#ifndef containerStack_h
#define containerStack_h

#include "container.h"
#include "container2D.h"
#include "container1DUnfold.h"
#include <iostream>
#include "TString.h"
#include <vector>
#include "THStack.h"
#include "TLegend.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLine.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TGaxis.h"
#include <map>
#include "tObjectList.h"
#include <map>


namespace ztop{
class plotterControlPlot;

class containerStack: public tObjectList,public taggedObject{
	friend class plotterControlPlot;
public:


	enum plotmode{normal,ratio,sigbg};

	containerStack();
	containerStack(TString);
	~containerStack();

	bool is2D()const{if(mode==dim2) return true; else return false;}
	bool is1D()const{if(mode==dim1) return true; else return false;}
	bool is1DUnfold()const{if(mode==unfolddim1) return true; else return false;}

	void push_back(const ztop::container1D&,const TString&, int, double, int legord=-1); //! adds container with, name, colour, norm to stack
	void push_back(const ztop::container2D&,const TString&, int, double, int legord=-1); //! adds container with, name, colour, norm to stack
	void push_back(const ztop::container1DUnfold&,const TString&, int, double, int legord=-1); //! adds container with, name, colour, norm to stack
	void removeContribution(TString); //! removes contribution

	void setDataLegend(TString leg="data"){dataleg_=leg;}

	void setLegendOrder(const TString &leg, const size_t& no);

	size_t getSystSize()const{
		if(containers_.size()>0) return containers_.at(0).getSystSize();
		if(containers2D_.size()>0) return containers2D_.at(0).getSystSize();
		if(containers1DUnfold_.size()>0) return containers1DUnfold_.at(0).getSystSize();
		return 0;
	}

	// void mergeSameLegends();       //! shouldn't be necessary

	void mergeLegends(const std::vector<TString>& tobemerged,const TString & mergedname, int mergedColor, bool allowsignals=false);
	void mergeLegends(const TString& tobemergeda,const TString & tobemergedb,const TString & mergedname, int mergedColor, bool allowsignals=false);

	void addEmptyLegend(const TString & legendentry, int color, int legord);
	/**
	 * names without "up" and "down"
	 */
	void mergeVariations(const std::vector<TString>& names, const TString & outname,bool linearly=false);

	void mergeVariationsFromFileInCMSSW(const std::string& filename);

	ztop::container1D getContribution(TString)const;   //! does not ignore data; makes copy, doesn't change member containers!
	ztop::container1D getContributionsBut(TString)const;  //!does not ignore data; makes copy, doesn't change member containers!
	ztop::container1D getContributionsBut(std::vector<TString>)const;  //!does not ignore data; makes copy, doesn't change member containers!

	ztop::container2D getContribution2D(TString)const;   //! does not ignore data; makes copy, doesn't change member containers!
	ztop::container2D getContributions2DBut(TString)const;  //!does not ignore data; makes copy, doesn't change member containers!
	ztop::container2D getContributions2DBut(std::vector<TString>)const;  //!does not ignore data; makes copy, doesn't change member containers!

	ztop::container1DUnfold getContribution1DUnfold(TString)const;   //! does not ignore data; makes copy, doesn't change member containers!
	ztop::container1DUnfold getContributions1DUnfoldBut(TString)const;  //!does not ignore data; makes copy, doesn't change member containers!
	ztop::container1DUnfold getContributions1DUnfoldBut(std::vector<TString>)const;  //!does not ignore data; makes copy, doesn't change member containers!


	size_t size() const{return colors_.size();}
	TString & getLegend(unsigned int i){return legends_.at(i);}
	const TString & getLegend(unsigned int i)const{return legends_.at(i);}
	int & getColor (unsigned int i){return colors_.at(i);};
	const int & getColor (unsigned int i)const {return colors_.at(i);};
	// double & getNorm  (unsigned int i){return norms_.at(i);}

	container1D & getContainer(unsigned int i){return containers_.at(i);}
	const container1D & getContainer(unsigned int i)const{return containers_.at(i);}
	//container1D & getContainer(TString);
	//container1D  getContainer(TString) const;
	container1D getFullMCContainer()const;           //! gets the sum of all MC containers (normalized with their stored norm) including error handling

	container2D & getContainer2D(unsigned int i){return containers2D_.at(i);}
	const container2D & getContainer2D(unsigned int i)const{return containers2D_.at(i);}
	//container2D & getContainer2D(TString);
	//container2D  getContainer2D(TString)const;
	container2D getFullMCContainer2D()const;           //! gets the sum of all MC containers (normalized with their stored norm) including error handling

	container1DUnfold & getContainer1DUnfold(unsigned int i){return containers1DUnfold_.at(i);}
	const container1DUnfold & getContainer1DUnfold(unsigned int i)const{return containers1DUnfold_.at(i);}
	//container1DUnfold & getContainer1DUnfold(TString);
	//container1DUnfold  getContainer1DUnfold(TString)const;
	container1DUnfold getFullMCContainer1DUnfold()const;           //! gets the sum of all MC containers (normalized with their stored norm) including error handling

	void multiplyNorm(TString , float);
	void multiplyNorm(size_t , float);
	void multiplyAllMCNorms(float);
	void addGlobalRelMCError(TString,double);   //! adds a global systematic variation to the systematics stored (e.g. lumi) is o+err

	void addRelErrorToContribution(double err, const TString& contributionname, TString nameprefix="BG");
	/**
	 * adds error to all backgrounds (not signal nor data) that are not excluded by legendname
	 * error is 0+error
	 * if aspartials is true, the errors will be merged to one error afterwards
	 */
	void addRelErrorToBackgrounds(double err ,bool aspartials , TString nameprefix,const TString& excludecontr);
	/**
	 * adds error to all backgrounds (not signal nor data) that are not excluded by legendnames
	 * error is 0+error
	 * if aspartials is true, the errors will be merged to one error afterwards
	 */
	void addRelErrorToBackgrounds(double err ,bool aspartials , TString nameprefix,const std::vector<TString> excludecontr);
	/**
	 * adds error to all backgrounds (not signal nor data) that are not excluded by excludeidxs
	 * error is 0+error
	 * if aspartials is true, the errors will be merged to one error afterwards
	 */
	void addRelErrorToBackgrounds(double err ,bool aspartials=false, TString nameprefix="BG",const std::vector<size_t> excludeidxs=std::vector<size_t>());
	void mergePartialVariations(const TString& identifier, bool strictpartialID=true);

	void addMCErrorStack(const TString&,const containerStack &) ; //only for comp reasons
	void addErrorStack(const TString& ,const containerStack& ); //! calls container1D::addErrorContainer for each same named member container
	void getRelSystematicsFrom(const ztop::containerStack&);
	void addRelSystematicsFrom(const ztop::containerStack&,bool ignorestat=false,bool strict=false);
	void removeError(TString);
	void removeAllSystematics();
	void renameSyst(TString, TString); //! old, new
	void splitSystematic(const size_t& number, const float& fracadivb,
			const TString & splinamea,  const TString & splinameb);
	/**
	 * works for up and down at the same time
	 */
	void splitSystematic(const TString& sysname, const float& fracadivb,
			const TString & splinamea,  const TString & splinameb);

	void equalizeSystematicsIdxs(containerStack& rhs);

	std::vector<size_t> removeSpikes(bool inclUFOF=false,int limittoindex=-1,float strength=100000,float sign=0.3,float threshold=8);

	void clear(){containers_.clear();legends_.clear();colors_.clear();}

	/*
	THStack * makeTHStack(TString stackname = "");
	TLegend * makeTLegend(bool inverse=true);
	void drawControlPlot(TString name="", bool drawxaxislabels=true, double resizelabels=1); // the extra axis is in both... sorry for that!
	TGraphAsymmErrors * makeMCErrors();
	 */
	//TCanvas * drawControlPlot(TString stylefile="")const;
	// void drawRatioPlot(TString name="",double resizelabels=1);
	/*	std::vector<container1D>  getPEffPlots(size_t idx)const;
	void drawPEffPlot(TString name="",double resizelabels=1);
	void drawSBGPlot(TString name="",double resizelabels=1,bool invert=false);

	//TCanvas * makeTCanvas(bool drawratioplot=true);
	TCanvas * makeTCanvas(plotmode plotMode=ratio);
	 */
	std::map<TString, histoBin>  getAllContentsInBin(size_t bin,int syst=-1,bool print=false)const;

	containerStack rebinXToBinning(const std::vector<float> &)const;
	containerStack rebinYToBinning(const std::vector<float> &)const;


	containerStack operator + ( containerStack );
	containerStack operator - ( containerStack );
	containerStack operator / ( containerStack );
	containerStack operator * ( containerStack );
	containerStack operator * (double);
	containerStack operator * (float);
	containerStack operator * (int);

	containerStack append(const containerStack&)const;

	bool setsignal(const TString&);
	bool addSignal(const TString&);
	bool setsignals(const std::vector<TString> &);// (just return whether successful)
	std::vector<size_t> getSignalIdxs() const;
	std::vector<size_t> getSortedIdxs(bool inverse) const;

	float getYMax(bool dividebybinwidth=true)const;

	size_t getDataIdx()const;

	container1D getSignalContainer()const;
	container1D getBackgroundContainer()const;
	container1D getDataContainer()const;


	container2D getSignalContainer2D()const;
	container2D getBackgroundContainer2D()const;



	/**
	 * Warning! Member functions will return different things than usual for
	 * the output container1DUnfold
	 * Prepares to be ready for unfolding in the following scheme:
	 * All MCs marked as signal (if no input given, the internal info will be used) will be added
	 * container1DUnfold::getRecoContainer will return DATA!
	 * container1DUnfold::getBackgroundContainer will return ALL backgrounds added
	 * if you need all MC (so summed reco MC), use:
	 * container1DUnfold::getVisibleSignal() + container1DUnfold::getBackground()
	 *
	 */
	container1DUnfold produceUnfoldingContainer(const std::vector<TString> &sign=std::vector<TString>()) const;// if sign empty use "setsignal"


	/*
       ad all signal -> cuf
       add all recocont_ w/o signaldef -> set BG of cuf
       (add all) with datadef -> setData in cuf
	 */
	container1DUnfold produceXCheckUnfoldingContainer(const std::vector<TString> & sign=std::vector<TString>()) const; /* if sign empty use "setsignal"
       just adds all with signaldef (to be then unfolded and a comparison plot to be made gen vs unfolded gen) -> SWITCH OFF SYSTEMATICS HERE! (should be independent)

	 */
	//define operators (propagate from containers) for easy handling
	// also define += etc. and use container += if better, needs to be implemented (later)

	container1DUnfold getSignalContainer1DUnfold(const std::vector<TString> &sign=std::vector<TString>()) const;

	/**
	 * Function that just retunrs a formatted string listing all contributions
	 * cout the output to print them
	 */
	TString listContributions()const;

	static std::vector<ztop::containerStack*> cs_list;
	static bool cs_makelist;
	static void cs_clearlist(){cs_list.clear();}
	static bool debug;

	static bool batchmode;

	/**
	 * If true all legends are required to be found in both stacks when adding errors
	 */
	static bool addErrorsStrict;


	///stat test functions

	/**
	 * calls TH1::Chi2Test on full MC histo and data hist
	 */
	double chi2Test(Option_t* option = "WW", Double_t* res = 0) const;

	/**
	 * double chi2
	 */
	double chi2(size_t * ndof=0)const;

	/**
	 * calls TH1::KolmogorovTest on full MC histo and data hist
	 */
	double kolmogorovTest(Option_t* option = "") const;


private:

	std::vector<ztop::container1D> containers_;
	std::vector<ztop::container2D> containers2D_;
	std::vector<ztop::container1DUnfold> containers1DUnfold_;
	std::vector<TString> legends_;
	std::vector<int> colors_;
	TString dataleg_;

	enum dimension{notset,dim1,dim2,unfolddim1};
	dimension mode;


	std::vector<int> legorder_;

	int checkLegOrder() const;


	std::vector<TString> signals_;


	int getContributionIdx(TString) const;
	bool checkDrawDimension() const;
	bool checkSignals(const std::vector<TString> &) const;





};



typedef containerStack container1DStack;

}
#endif
