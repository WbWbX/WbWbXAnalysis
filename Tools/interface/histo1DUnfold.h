/*
 * histo1DUnfold.h
 *
 *  Created on: Aug 14, 2013
 *      Author: kiesej
 */

#ifndef HISTO1DUNFOLD_H_
#define HISTO1DUNFOLD_H_

#include "histo2D.h"
#include <iostream>

/**
 * Warning:
 * due to internal reasons, statistics on systematic variation are NOW accounted for
 *
 */

/*
 * helper class to store unfolding output etc
 *
 */
class TTree;
class TFile;



namespace ztop{
class histo1DUnfold: public histo2D {
public:
	histo1DUnfold();
	histo1DUnfold( std::vector<float> genbins, std::vector<float> recobins, TString name="",TString xaxisname="",TString yaxisname="", bool mergeufof=false);
	histo1DUnfold( std::vector<float> genbins, TString name="",TString xaxisname="",TString yaxisname="", bool mergeufof=false);

	~histo1DUnfold();



	void setBinning(const std::vector<float> & genbins,const std::vector<float> &recobins);
	//void subdivideRecoBins(int div);
	//can also be done afterwards, but BEFORE unfolding
	void setGenBinning(const std::vector<float> & genbins);

	histo1DUnfold rebinToBinning(const std::vector<float> &)const;

	void clear();
	void reset();
	void setAllZero();
	void removeError(TString name){histo2D::removeError(name); gencont_.removeError(name); recocont_.removeError(name);unfolded_.removeError(name);}
	void renameSyst(TString old , TString New){histo2D::renameSyst(old,New);gencont_.renameSyst(old,New);recocont_.renameSyst(old, New);}
	void removeAllSystematics(const TString& exception="");

	void splitSystematic(const size_t & number, const float& fracadivb,
			const TString & splinamea,  const TString & splinameb);

	/**
	 * merges partial variations. only the ones corresponding to the identifier are merged
	 * hint: do this step at the latest stage you can do it to still keep track of all correlations
	 * If you know its a global variation its ok to do it earlier
	 */
	void mergePartialVariations(const TString& identifier,bool strictpartialID=true);
	void mergeAllErrors(const TString & mergedname);

	void equalizeSystematicsIdxs(histo1DUnfold &rhs);


	void mergeVariations(const std::vector<TString>& names, const TString & outname,bool linearly=false);
	void mergeVariationsFromFileInCMSSW(const std::string& filename);

	void fillGen(const float & val, const float & weight=1); //if isMC false only fill one specific container - which one?? 0<->underflow? or first one...?
	void fillReco(const float & yval, const float & weight=1);

	void setMC(bool ismc){isMC_=ismc; if(!isMC_) flushed_=true;}
	bool getMC(){return isMC_;}

	void setBinByBin(bool bbb);
	bool isBinByBin() const{return binbybin_;}

	void setLumi(float lumi){lumi_=lumi;}
	const float& getLumi(){return lumi_;}

	const histo1D & getGenContainer() const {return gencont_;}
	const histo1D & getRecoContainer() const {return recocont_;}

	histo1D  getBinnedGenContainer() const{return gencont_.rebinToBinning(genbins_);}

	const std::vector<float> & getGenBins()const{return genbins_;}


	void setRecoContainer(const histo1D &cont);
	void setBackground(const histo1D &cont);
	void addToBackground(const histo1D &cont){setBackground(getBackground()+cont);}

	/**
	 * returns the background as histo1D
	 * if the histo1DUnfold contains the signal this
	 * will contain the migrations from outside visible PS
	 * in the selected reco events
	 * This is a reco distribution
	 * But the overflow will contain events generated outside visible PS and not reconstructed
	 */
	histo1D getBackground() const;

	/**
	 * returns the signal generated in visible phasespace
	 * and reconstructed in measureable phasespace
	 * If this is pure background (all generated in underflow)
	 * it will just return an empty (binned) container
	 * This is a reco distribution
	 * But the overflow will contain generated but not reconstructed events
	 */
	histo1D getVisibleSignal() const;


	/*
	 * only histo1DUnfold operator * (float) operates on the unfolded distribution.. let's see for the others
	 */

	histo1DUnfold& operator += (const histo1DUnfold &);       //! adds stat errors in squares; treats same named systematics as correlated!!
	histo1DUnfold operator + (const histo1DUnfold &)const;       //! adds stat errors in squares; treats same named systematics as correlated!!
	histo1DUnfold& operator -= (const histo1DUnfold &);       //! adds errors in squares; treats same named systematics as correlated!!
	histo1DUnfold operator - (const histo1DUnfold &)const;       //! adds errors in squares; treats same named systematics as correlated!!
	histo1DUnfold& operator /= (const histo1DUnfold &);       //! binomial stat error or uncorr error (depends on setDivideBinomial()); treats same named systematics as correlated
	histo1DUnfold operator / (const histo1DUnfold &)const;       //! binomial stat error or uncorr error (depends on setDivideBinomial()); treats same named systematics as correlated
	histo1DUnfold& operator *= (const histo1DUnfold &);       //! adds stat errors in squares; treats same named systematics as correlated!!
	histo1DUnfold operator * (const histo1DUnfold &)const;       //! adds stat errors in squares; treats same named systematics as correlated!!
	histo1DUnfold operator * (float)const;            //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
	histo1DUnfold& operator *= (float);           //! simple scalar multiplication. stat and syst errors are scaled accordingly!! room for performance improvement
	histo1DUnfold operator * (double)const;
	histo1DUnfold& operator *= (double);

	bool operator == (const histo1DUnfold &rhs)const;
	bool operator != (const histo1DUnfold &rhs)const{
		return !(*this==rhs);
	}

	void setDivideBinomial(bool);

	int addErrorContainer(const TString & ,const histo1DUnfold &,float weight);  //! adds deviation to (this) as systematic uncertianty with name and weight. name must be ".._up" or ".._down"
	int addErrorContainer(const TString & ,const histo1DUnfold &);  //! adds deviation to (this) as systematic uncertianty with name and weight. name must be ".._up" or ".._down"
	void getRelSystematicsFrom(const histo1DUnfold &);
	void addRelSystematicsFrom(const histo1DUnfold &,bool ignorestat=false,bool strict=false);
	void addGlobalRelError(TString,float);

	static std::vector<histo1DUnfold *> c_list;
	static bool debug;
	static bool c_makelist;
	static void c_deletelist(){
		size_t count=0,listsize=c_list.size();
		for(unsigned int i=0;i<listsize;i++){
			if(c_list[count]) delete c_list[count]; //delete also removes entry from list
			else count++; //that should never happen if list not touched from outside
		}
	}


	/**
	 * does nothing for data where flushed_ is always true
	 * deprecated! always returns true
	 */
	bool check();

	/**
	 *
	 * This function folds a (generated) input distribution
	 * In contrast to the unfolding this takes place within this class.
	 * The unfolding is done by an outside class, the histoUnfolder,
	 * to keep the histo1DUnfold class slim.
	 * Checks on binning etc are performed.
	 * The output will be binned in reco binning and will incorporate all systematic
	 * uncertainties (TBI2:+stat of resp matrix).
	 * Migrations from outside of visible PS will be added to the distribution as well as all
	 * additionally added background(s). These backgrounds from other sources have to be added before!
	 * HINT: the whole preparation is automatically done by histoStack::produceUnfoldingContainer
	 * Statistical uncertainties of response matrix and input container will be treated as uncorrelated!
	 */
	histo1D fold(const histo1D& input,bool includebackgrounds=true) const;

	const histo1D & getControlPlot() const{return recocont_;}

	const histo1D & getUnfolded() const {return unfolded_;}
	const histo1D & getRefolded() const {return refolded_;}

	void setUnfolded(const histo1D & UF)  { unfolded_=UF;}
	void setRefolded(const histo1D & RF)  { refolded_=RF;}

	histo1D getPurity() const;
	histo1D getStability(bool includeeff=true) const;
	histo1D getEfficiency()const;

	/**
	 * should be obsolete now since 2D matrix anyway constructed as N x N (check) ?!
	 */
	bool checkCongruentBinBoundariesXY() const;
	histoBin getDiagonalBin(const size_t & xbin, int layer=-1) const;
	histo1D getDiagonal()const;

	histo2D getResponseMatrix()const;
	histo2D getNormResponseMatrix()const;
	TH2D * prepareRespMatrix(bool nominal=true,unsigned int systNumber=0) const;


	// someformat getCovarianceMatrix(size_t syst) { // M_ij = d_i * d_j, d_i= nominal_i-sys_i, i=bin}

	/**
	 * Important!
	 * Call this function (or individual flush()) after filling is done!
	 */
	static void flushAllListed();
	static void setAllListedMC(bool ISMC);
	static void setAllListedLumi(float lumi);
	static void checkAllListed();

	TString coutUnfoldedBinContent(size_t bin,const TString& units="") const;

	/**
	 * Important!
	 * Call this function after filling is done!
	 */
	void flush();

	void setAllowMultiRecoFill(bool allow){allowmultirecofill_=allow;}

	///IO
	void loadFromTree(TTree *, const TString & plotname);
	void loadFromTFile(TFile *, const TString & plotname);
	void loadFromTFile(const TString& filename, const TString & plotname);

	void writeToTree(TTree *);
	void writeToTFile(TFile *);
	void writeToTFile(const TString& filename);

	static bool TFileContainsContainer1DUnfolds(TFile *f);
	static bool TFileContainsContainer1DUnfolds(const TString & filename);



#ifndef __CINT__
	template <class T>
	void writeToStream(T & stream)const;
	template <class T>
	void readFromStream(T & stream);
#endif

	void writeToFile(const std::string& filename)const;
	void readFromFile(const std::string& filename);


private:
	size_t getBinNoY(const float & var) const;

	void fill(const float & xval, const float & yval, const float & weight=1){
		size_t ybin=getBinNoY(yval);
		conts_[ybin].fill(xval,weight);
	}
	histo2D rebinXToBinning(const std::vector<float>& vec)const{return histo2D::rebinXToBinning(vec);}
	histo2D rebinYToBinning(const std::vector<float>& vec)const{return histo2D::rebinYToBinning(vec);}
	void setZAxisName(const TString& name){}
	TString getZAxisName()const {return "";}


	TString xaxis1Dname_,yaxis1Dname_;

	float tempgen_,tempreco_,tempgenweight_,tempweight_;
	bool recofill_,genfill_;

	bool isMC_;
	bool flushed_;
	bool binbybin_;

	histo1D gencont_,recocont_,unfolded_,refolded_;
	std::vector<float> genbins_;

	float lumi_;

	bool congruentbins_;

	bool allowmultirecofill_;
	/*
   //destructor safe
   unfolder * unfold_;
   std::vector<unfolder *> unfolds_;
	 */
	//some functions that should not be used although inherited
	void addErrorContainer(const TString & ,const histo2D &,float){}
	void addErrorContainer(const TString &,const histo2D &){}
	void getRelSystematicsFrom(const histo2D &){}
	void equalizeSystematicsIdxs(histo2D &rhs){}

	std::vector<float> subdivide(const std::vector<float> & bins, size_t div);

	bool checkCongruence(const std::vector<float>&, const std::vector<float>&)const;

};

inline size_t ztop::histo1DUnfold::getBinNoY(const float & var) const {

	if(ybins_.size() <2){
		return 0;
	}
	size_t binno=0;
	std::vector<float>::const_iterator it=std::lower_bound(ybins_.begin()+1, ybins_.end(), var);
	if(var==*it)
		binno= it-ybins_.begin();
	else
		binno= it-ybins_.begin()-1;

	//no changes wrt underflow, overflow. MUST be kept

	 return binno;
}

#ifndef __CINT__

template <class T>
inline void histo1DUnfold::writeToStream(T & out)const{
	const histo1DUnfold * writeout=this;
	if(!flushed_){
		writeout=new histo1DUnfold(*this);
	}
	writeout->histo2D::writeToStream(out);
	if(!flushed_){
		delete writeout;
	}
	/*
	TString xaxis1Dname_,yaxis1Dname_;

	float tempgen_,tempreco_,tempgenweight_,tempweight_;
	bool recofill_,genfill_;

	bool isMC_;
	bool flushed_;
	bool binbybin_;

	histo1D gencont_,recocont_,unfolded_,refolded_;
	std::vector<float> genbins_;

	float lumi_;

	bool congruentbins_;

	bool allowmultirecofill_;
	 */
	IO::serializedWrite(xaxis1Dname_,out);
	IO::serializedWrite(yaxis1Dname_,out);
	IO::serializedWrite(isMC_,out);
	IO::serializedWrite(binbybin_,out);
	IO::serializedWrite(gencont_,out);
	IO::serializedWrite(recocont_,out);
	IO::serializedWrite(unfolded_,out);
	IO::serializedWrite(refolded_,out);
	IO::serializedWrite(genbins_,out);
	IO::serializedWrite(lumi_,out);
	IO::serializedWrite(congruentbins_,out);
	IO::serializedWrite(allowmultirecofill_,out);

}

template <class T>
inline void histo1DUnfold::readFromStream(T & in){

	*this=histo1DUnfold();
	histo2D::readFromStream(in);
	flushed_=true;
	/*
		TString xaxis1Dname_,yaxis1Dname_;

		float tempgen_,tempreco_,tempgenweight_,tempweight_;
		bool recofill_,genfill_;

		bool isMC_;
		bool flushed_;
		bool binbybin_;

		histo1D gencont_,recocont_,unfolded_,refolded_;
		std::vector<float> genbins_;

		float lumi_;

		bool congruentbins_;

		bool allowmultirecofill_;
	 */
	IO::serializedRead(xaxis1Dname_,in);
	IO::serializedRead(yaxis1Dname_,in);
	IO::serializedRead(isMC_,in);
	IO::serializedRead(binbybin_,in);
	IO::serializedRead(gencont_,in);
	IO::serializedRead(recocont_,in);
	IO::serializedRead(unfolded_,in);
	IO::serializedRead(refolded_,in);
	IO::serializedReadVector(genbins_,in);
	IO::serializedRead(lumi_,in);
	IO::serializedRead(congruentbins_,in);
	IO::serializedRead(allowmultirecofill_,in);

}

namespace IO{
//implements standardized IO operations for this class


template<class T>
inline void serializedWrite(const histo1DUnfold&c, T&stream){
	c.writeToStream(stream);
}
template<class T>
inline void serializedRead(histo1DUnfold&c, T&stream){
	c.readFromStream(stream);
}
//re-instantiate vector ops
template<class T>
inline void serializedWrite(const std::vector<histo1DUnfold>&in, T&saveFile){
	size_t len=in.size();
	saveFile.write(reinterpret_cast<const char *>(&len), sizeof(len));
	for(size_t i=0;i<len;i++)
		serializedWrite(in.at(i),saveFile);
}
template<class T>
inline void serializedRead(std::vector<histo1DUnfold>&in, T&saveFile){
	size_t len=0;
	saveFile.read(reinterpret_cast< char *>(&len), sizeof(len));
	in.resize(len,histo1DUnfold());
	for(size_t i=0;i<len;i++)
		serializedRead(in.at(i),saveFile);
}


}//io


#endif //cint


} //namespace

#endif /* CONTAINER1DUNFOLD_H_ */
