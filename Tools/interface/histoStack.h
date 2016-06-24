#ifndef histoStack_h
#define histoStack_h

#include "histo1D.h"
#include "histo2D.h"
#include "histo1DUnfold.h"
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

class histoStack: public tObjectList,public taggedObject{
	friend class plotterControlPlot;
public:


	enum plotmode{normal,ratio,sigbg};

	histoStack();
	histoStack(TString);
	~histoStack();

	bool is2D()const{if(mode==dim2) return true; else return false;}
	bool is1D()const{if(mode==dim1) return true; else return false;}
	bool is1DUnfold()const{if(mode==unfolddim1) return true; else return false;}

	void push_back(const ztop::histo1D& cont,const TString& legend, int color, double norm, int legord= 99999); //! adds container with, name, colour, norm to stack
	void push_back(const ztop::histo2D&,const TString&, int, double, int legord=99999); //! adds container with, name, colour, norm to stack
	void push_back(const ztop::histo1DUnfold&,const TString&, int, double, int legord=99999); //! adds container with, name, colour, norm to stack
	void removeContribution(TString); //! removes contribution

	void setDataLegend(TString leg="data");

	void setLegendOrder(const TString &leg, const size_t& no);

	size_t getSystSize()const{
		if(containers_.size()>0) return containers_.at(0).getSystSize();
		if(containers2D_.size()>0) return containers2D_.at(0).getSystSize();
		if(containers1DUnfold_.size()>0) return containers1DUnfold_.at(0).getSystSize();
		return 0;
	}

	std::vector<TString> getSystNameList()const;

	// void mergeSameLegends();       //! shouldn't be necessary

	void mergeLegends(const std::vector<TString>& tobemerged,const TString & mergedname, int mergedColor, bool allowsignals=false,int newlego=99999);
	void mergeLegends(const TString& tobemergeda,const TString & tobemergedb,const TString & mergedname, int mergedColor, bool allowsignals=false);

	void addEmptyLegend(const TString & legendentry, int color, int legord);
	/**
	 * names without "up" and "down"
	 */
	void mergeVariations(const std::vector<TString>& names, const TString & outname,bool linearly=false);

	void mergeVariationsFromFileInCMSSW(const std::string& filename,const std::string& marker="");

	ztop::histo1D getContribution(TString)const;   //! does not ignore data; makes copy, doesn't change member containers!
	ztop::histo1D getContributionsBut(TString)const;  //!does not ignore data; makes copy, doesn't change member containers!
	ztop::histo1D getContributionsBut(std::vector<TString>)const;  //!does not ignore data; makes copy, doesn't change member containers!

	ztop::histo2D getContribution2D(TString)const;   //! does not ignore data; makes copy, doesn't change member containers!
	ztop::histo2D getContributions2DBut(TString)const;  //!does not ignore data; makes copy, doesn't change member containers!
	ztop::histo2D getContributions2DBut(std::vector<TString>)const;  //!does not ignore data; makes copy, doesn't change member containers!

	ztop::histo1DUnfold getContribution1DUnfold(TString)const;   //! does not ignore data; makes copy, doesn't change member containers!
	ztop::histo1DUnfold getContributions1DUnfoldBut(TString)const;  //!does not ignore data; makes copy, doesn't change member containers!
	ztop::histo1DUnfold getContributions1DUnfoldBut(std::vector<TString>)const;  //!does not ignore data; makes copy, doesn't change member containers!


	size_t size() const{return colors_.size();}
	TString & getLegend(unsigned int i){return legends_.at(i);}
	const TString & getLegend(unsigned int i)const{return legends_.at(i);}
	int & getColor (unsigned int i){return colors_.at(i);};
	const int & getColor (unsigned int i)const {return colors_.at(i);};
	// double & getNorm  (unsigned int i){return norms_.at(i);}

	histo1D & getContainer(unsigned int i){return containers_.at(i);}
	const histo1D & getContainer(unsigned int i)const{return containers_.at(i);}
	//histo1D & getContainer(TString);
	//histo1D  getContainer(TString) const;
	histo1D getFullMCContainer()const;           //! gets the sum of all MC containers (normalized with their stored norm) including error handling

	histo2D & getContainer2D(unsigned int i){return containers2D_.at(i);}
	const histo2D & getContainer2D(unsigned int i)const{return containers2D_.at(i);}
	//histo2D & getContainer2D(TString);
	//histo2D  getContainer2D(TString)const;
	histo2D getFullMCContainer2D()const;           //! gets the sum of all MC containers (normalized with their stored norm) including error handling

	histo1DUnfold & getContainer1DUnfold(unsigned int i){return containers1DUnfold_.at(i);}
	const histo1DUnfold & getContainer1DUnfold(unsigned int i)const{return containers1DUnfold_.at(i);}
	//histo1DUnfold & getContainer1DUnfold(TString);
	//histo1DUnfold  getContainer1DUnfold(TString)const;
	histo1DUnfold getFullMCContainer1DUnfold()const;           //! gets the sum of all MC containers (normalized with their stored norm) including error handling

	void multiplyNorm(TString , float);
	void multiplyNorm(size_t , float);
	void multiplyAllMCNorms(float);
	void addGlobalRelMCError(TString,double);   //! adds a global systematic variation to the systematics stored (e.g. lumi) is o+err

	void addRelErrorToContribution(double err, const TString& contributionname, TString nameprefix="BG");
	void addRelErrorToContributions(double err, const std::vector<size_t> contribidxs, TString nameprefix="BG");
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

	void addMCErrorStack(const TString&,const histoStack &) ; //only for comp reasons
	void addErrorStack(const TString& ,const histoStack& ); //! calls histo1D::addErrorContainer for each same named member container
	void getRelSystematicsFrom(const ztop::histoStack&);
	void addRelSystematicsFrom(const ztop::histoStack&,bool ignorestat=false,bool strict=false);
	void removeError(TString);
	void removeAllSystematics(const TString& exception="");
	void renameSyst(TString, TString); //! old, new
	void splitSystematic(const size_t& number, const float& fracadivb,
			const TString & splinamea,  const TString & splinameb);
	/**
	 * works for up and down at the same time
	 */
	void splitSystematic(const TString& sysname, const float& fracadivb,
			const TString & splinamea,  const TString & splinameb);

	void equalizeSystematicsIdxs(histoStack& rhs);

	std::vector<size_t> removeSpikes(bool inclUFOF=false,int limittoindex=-1,float strength=100000,float sign=0.3,float threshold=8);

	void clear(){*this=histoStack();}

	/*
	THStack * makeTHStack(TString stackname = "");
	TLegend * makeTLegend(bool inverse=true);
	void drawControlPlot(TString name="", bool drawxaxislabels=true, double resizelabels=1); // the extra axis is in both... sorry for that!
	TGraphAsymmErrors * makeMCErrors();
	 */
	//TCanvas * drawControlPlot(TString stylefile="")const;
	// void drawRatioPlot(TString name="",double resizelabels=1);
	/*	std::vector<histo1D>  getPEffPlots(size_t idx)const;
	void drawPEffPlot(TString name="",double resizelabels=1);
	void drawSBGPlot(TString name="",double resizelabels=1,bool invert=false);

	//TCanvas * makeTCanvas(bool drawratioplot=true);
	TCanvas * makeTCanvas(plotmode plotMode=ratio);
	 */
	std::map<TString, histoBin>  getAllContentsInBin(size_t bin,int syst=-1,bool print=false)const;

	histoStack rebinXToBinning(const std::vector<float> &)const;
	histoStack rebinYToBinning(const std::vector<float> &)const;


	histoStack operator + ( const histoStack& );
	histoStack operator - ( const histoStack& );
	histoStack operator / ( const histoStack& );
	histoStack operator * ( const histoStack& );
	/*
	histoStack& operator += ( const histoStack& rhs){*this=*this+rhs;return *this;}//FIXME still contradictory to mode logic if *this is empty
	histoStack& operator -= ( const histoStack& rhs){*this=*this-rhs;return *this;}
	histoStack& operator /= ( const histoStack& rhs){*this=*this/rhs;return *this;}
	histoStack& operator *= ( const histoStack& rhs){*this=*this*rhs;return *this;}*/
	histoStack operator * (double);
	histoStack operator * (float);
	histoStack operator * (int);
	bool operator == (const histoStack&)const;
	bool operator != (const histoStack&rhs)const{
		return !(*this==rhs);
	}

	histoStack append(const histoStack&)const;

	bool setsignal(const TString&);
	bool addSignal(const TString&);
	bool setsignals(const std::vector<TString> &);// (just return whether successful)
	std::vector<size_t> getSignalIdxs() const;
	std::vector<size_t> getSortedIdxs(bool inverse) const;

	float getYMax(bool dividebybinwidth=true)const;
	float getYMin(bool dividebybinwidth=true, bool onlydata=false)const;

	size_t getDataIdx()const;

	histo1D getSignalContainer()const;
	histo1D getBackgroundContainer()const;
	histo1D getDataContainer()const;


	histo2D getSignalContainer2D()const;
	histo2D getBackgroundContainer2D()const;



	/**
	 * Warning! Member functions will return different things than usual for
	 * the output histo1DUnfold
	 * Prepares to be ready for unfolding in the following scheme:
	 * All MCs marked as signal (if no input given, the internal info will be used) will be added
	 * histo1DUnfold::getRecoContainer will return DATA!
	 * histo1DUnfold::getBackgroundContainer will return ALL backgrounds added
	 * if you need all MC (so summed reco MC), use:
	 * histo1DUnfold::getVisibleSignal() + histo1DUnfold::getBackground()
	 *
	 */
	histo1DUnfold produceUnfoldingContainer(const std::vector<TString> &sign=std::vector<TString>()) const;// if sign empty use "setsignal"


	/*
       ad all signal -> cuf
       add all recocont_ w/o signaldef -> set BG of cuf
       (add all) with datadef -> setData in cuf
	 */
	histo1DUnfold produceXCheckUnfoldingContainer(const std::vector<TString> & sign=std::vector<TString>()) const; /* if sign empty use "setsignal"
       just adds all with signaldef (to be then unfolded and a comparison plot to be made gen vs unfolded gen) -> SWITCH OFF SYSTEMATICS HERE! (should be independent)

	 */
	//define operators (propagate from containers) for easy handling
	// also define += etc. and use container += if better, needs to be implemented (later)

	histo1DUnfold getSignalContainer1DUnfold(const std::vector<TString> &sign=std::vector<TString>()) const;

	/**
	 * Function that just retunrs a formatted string listing all contributions
	 * cout the output to print them
	 */
	TString listContributions()const;

	static std::vector<ztop::histoStack*> cs_list;
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

	int getContributionIdx(const TString&) const;

	histoStack fitAllNorms(const std::vector<TString>& contributions, bool globalfloat);

	void poissonSmearMC(TRandom3* r, bool alsosys);

#ifndef __CINT__
	template <class T>
	void writeToStream(T & stream)const;
	template <class T>
	void readFromStream(T & stream);
#endif

	void writeToFile(const std::string& filename)const;
	void readFromFile(const std::string& filename);

private:

	std::vector<ztop::histo1D> containers_;
	std::vector<ztop::histo2D> containers2D_;
	std::vector<ztop::histo1DUnfold> containers1DUnfold_;
	std::vector<TString> legends_;
	std::vector<int> colors_;
	TString dataleg_;

	enum dimension{notset,dim1,dim2,unfolddim1};
	dimension mode;


	std::vector<int> legorder_;

	int checkLegOrder() const;


	std::vector<TString> signals_;


	bool checkDrawDimension() const;
	bool checkSignals(const std::vector<TString> &) const;





};

#ifndef __CINT__

template <class T>
inline void histoStack::writeToStream(T & out)const{
	taggedObject::writeToStream(out);
	IO::serializedWrite(containers_,out);
	IO::serializedWrite(containers2D_,out);
	IO::serializedWrite(containers1DUnfold_,out);
	IO::serializedWrite(legends_,out);
	IO::serializedWrite(colors_,out);
	IO::serializedWrite(dataleg_,out);
	IO::serializedWrite(mode,out);
	IO::serializedWrite(legorder_,out);
	IO::serializedWrite(signals_,out);
}

template <class T>
inline void histoStack::readFromStream(T & in){
	taggedObject::readFromStream(in);
	cleanMem();
	IO::serializedRead(containers_,in);
	IO::serializedRead(containers2D_,in);
	IO::serializedRead(containers1DUnfold_,in);
	IO::serializedRead(legends_,in);
	IO::serializedRead(colors_,in);
	IO::serializedRead(dataleg_,in);
	IO::serializedRead(mode,in);
	IO::serializedRead(legorder_,in);
	IO::serializedRead(signals_,in);
}


namespace IO{
//implements standardized IO operations for this class


template<class T>
inline void serializedWrite(const histoStack&c, T&stream){
	c.writeToStream(stream);
}
template<class T>
inline void serializedRead(histoStack&c, T&stream){
	c.readFromStream(stream);
}
//re-instantiate vector ops
template<class T>
inline void serializedWrite(const std::vector<histoStack>&in, T&saveFile){
	size_t len=in.size();
	saveFile.write(reinterpret_cast<const char *>(&len), sizeof(len));
	for(size_t i=0;i<len;i++)
		serializedWrite(in.at(i),saveFile);
}
template<class T>
inline void serializedRead(std::vector<histoStack>&in, T&saveFile){
	size_t len=0;
	saveFile.read(reinterpret_cast< char *>(&len), sizeof(len));
	in.resize(len,histoStack());
	for(size_t i=0;i<len;i++)
		serializedRead(in.at(i),saveFile);
}


}//io


#endif

typedef histoStack container1DStack;

}
#endif
