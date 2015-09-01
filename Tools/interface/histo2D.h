/*
 * histo2D.h
 *
 *  Created on: Jun 24, 2013
 *      Author: kiesej
 */

#ifndef histo2D_h_
#define histo2D_h_

#include "histo1D.h"


/*
 * to be implemented:
 * hasunderflow/overflow (can be function that checks entries in corr containers
 *
 */

namespace ztop{
class histo1DUnfold;
class likelihood2D;

class histo2D:public taggedObject {
	friend class histo1DUnfold;
	friend class likelihood2D;
public:



	histo2D();
	histo2D(const std::vector<float> & ,const std::vector<float> &, TString name="",TString xaxisname="",TString yaxisname="",TString zaxisname="", bool mergeufof=false); //! construct with binning
	~histo2D();
	histo2D(const histo2D&);
	histo2D& operator=(const histo2D&);

	bool isDummy()const{return xbins_.size()<=2;}

	void setBinning(const std::vector<float> &xbins, std::vector<float> ybins);
	//setaxis etc still missing. should be ok as long as default constr. is protected



	void setXAxisName(const TString& name){xaxisname_=name;}
	const TString& getXAxisName()const {return xaxisname_;}

	void setYAxisName(const TString& name){yaxisname_=name;}
	const TString& getYAxisName()const {return yaxisname_;}

	void setZAxisName(const TString& name){zaxisname_=name;}
	const TString& getZAxisName()const {return zaxisname_;}



	size_t getBinNoX(const float&) const; // returns bin index number for (float variable)
	size_t getBinNoY(const float&) const; //! returns bin index number for (float variable)
	size_t getNBinsX() const {return xbins_.size()-2;}       //! returns numberof x bins
	size_t getNBinsY() const {return ybins_.size()-2;}   //! returns numberof x bins
	const uint32_t & getBinEntries(const size_t&,const size_t&) const;
	const std::vector<float> & getBinsX() const{return xbins_;}     //! returns  x bins
	const std::vector<float> & getBinsY() const{return ybins_;}       //! returns y bins

	const histoBin & getBin(const size_t &,const size_t&,const int &layer=-1) const;
	histoBin & getBin(const size_t&,const size_t&,const int& layer=-1);

	float getBinArea(const size_t&, const size_t&)const;
	void getBinCenter(const size_t binx,const size_t biny, float& centerx,float& centery)const;

	float getMax(int syslayer=-1)const;
	float getMin(int syslayer=-1)const;
	float getMax(size_t& xbin, size_t& ybin,int syslayer=-1)const;
	float getMin(size_t& xbin, size_t& ybin,int syslayer=-1)const;

	histo2D rebinXToBinning(const std::vector<float>& )const;
	histo2D rebinXtoBinning(const histo1D&)const;
	histo2D rebinYToBinning(const std::vector<float>& )const;
	histo2D rebinYtoBinning(const histo1D&)const;

	void fill(const float & xval, const float & yval, const float & weight=1);    //! fills with weight

	size_t getSystSize() const {if(conts_.size()<1) return 0; else return conts_.at(0).getSystSize();}
	const float &getBinContent(const size_t & xbin,const size_t & ybin) const;
	float getBinErrorUp(const size_t & xbin, const size_t & ybin, bool onlystat=false,TString limittosys="") const;
	float getBinErrorDown(const size_t &,const size_t &,bool onlystat=false,TString limittosys="") const;
	float getBinError(const size_t &,const size_t &,bool onlystat=false,TString limittosys="") const;
	float getSystError(unsigned int number, const size_t & xbin, const size_t & ybin) const;
	float getSystErrorStat(unsigned int number, const size_t & xbin, const size_t & ybin) const;
	const TString & getSystErrorName(const size_t & number) const;
	size_t getSystErrorIndex(const TString& name)const;

	void mergeVariations(const std::vector<TString>& names, const TString & outname,bool linearly=false);
	void mergeVariationsFromFileInCMSSW(const std::string& filename);
	void splitSystematic(const size_t & number, const float& fracadivb,
			const TString & splinamea,  const TString & splinameb);

	/*
     float projectBinContentToY(const size_t & ybin,bool includeUFOF=false) const;
     float projectBinContentToX(const size_t & xbin,bool includeUFOF=false) const;
	 */
	histo1D projectToX(bool includeUFOF=false) const;
	histo1D projectToY(bool includeUFOF=false) const;
	histo1D getYSlice(size_t ybinno) const;
	histo1D getXSlice(size_t xbinno) const;

	void setXSlice(size_t xbinno,const histo1D &, bool UFOF=true);
	void setYSlice(size_t xbinno,const histo1D &, bool UFOF=true);

	void removeError(TString);
	void renameSyst(TString, TString);
	void removeAllSystematics(const TString& exception="");

	void setAllErrorsZero(bool nominalstat);

	void createStatFromContent();

	TH2D * getTH2D(TString name="", bool dividebybinarea=false, bool onlystat=false) const;
	TH2D * getTH2DSyst(TString name, unsigned int systNo, bool dividebybinarea=false, bool statErrors=false) const;

	/**
	 * imports TGraphAsymmErrors. Only works if the x-axis errors indicate the bin boundaries,
	 * the graph binning corresponds to X axis binning, and all bins are the same size
	 * Y errors will be represented as new systematic layers, the stat error will be set to 0.
	 * The new systematic layers can be named- if not, default is Graphimp_<up/down>
	 */
	histo2D & import(std::vector<TGraphAsymmErrors *>,bool isbinwidthdivided=false, const TString & newsystname="Graphimp");


	void setDivideBinomial(bool);

	histo2D & operator += (const histo2D &);       //! adds stat errors in squares; treats same named systematics as correlated!!
	histo2D operator + (const histo2D &)const;       //! adds stat errors in squares; treats same named systematics as correlated!!
	histo2D & operator -= (const histo2D &);       //! adds errors in squares; treats same named systematics as correlated!!
	histo2D operator - (const histo2D &)const;       //! adds errors in squares; treats same named systematics as correlated!!
	histo2D & operator /= (const histo2D &);       //! binomial stat error or uncorr error (depends on setDivideBinomial()); treats same named systematics as correlated
	histo2D operator / (const histo2D &)const;       //! binomial stat error or uncorr error (depends on setDivideBinomial()); treats same named systematics as correlated
	histo2D & operator *= (const histo2D &);       //! adds stat errors in squares; treats same named systematics as correlated!!
	histo2D operator * (const histo2D &)const;       //! adds stat errors in squares; treats same named systematics as correlated!!
	histo2D & operator *= (float); //fast hack!            //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
	histo2D operator * (float)const;            //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
	histo2D operator * (double)const;             //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
	histo2D operator * (int)const;               //! simple scalar multiplication. stat and syst errors are scaled accordingly!!

	bool operator == (const histo2D &rhs)const{
		return isEqual(rhs);
	}
	bool operator != (const histo2D &rhs)const{
		return !(*this==rhs);
	}


	histo2D chi2container(const histo2D&,size_t * ndof=0)const;
	float chi2(const histo2D&,size_t * ndof=0)const;

	/**
	 * cuts everything on the right of the input value (bin boundary chosen accorind to getBinNo())
	 * and returns new container.
	 */
	histo2D cutRightX(const float & )const;/**
	 * cuts everything on the right of the input value (bin boundary chosen accorind to getBinNo())
	 * and returns new container.
	 */
	histo2D cutLeftX(const float & )const;/**
	 * cuts everything on the right of the input value (bin boundary chosen accorind to getBinNo())
	 * and returns new container.
	 */
	histo2D cutUpY(const float & )const;/**
	 * cuts everything on the right of the input value (bin boundary chosen accorind to getBinNo())
	 * and returns new container.
	 */
	histo2D cutDownY(const float & )const;

	/**copies content as far as bins are overlapping. Does not create over/underflow
	* only nominal!
	 */
	void copyContentFrom(const histo2D&);

	int addErrorContainer(const TString & ,const histo2D &,float);  //! adds deviation to (this) as systematic uncertianty with name and weight. name must be ".._up" or ".._down"
	int addErrorContainer(const TString &,const histo2D & );        //! adds deviation to (this) as systematic uncertianty with name. name must be ".._up" or ".._down"
	void getRelSystematicsFrom(const histo2D &);
	void addRelSystematicsFrom(const histo2D &,bool ignorestat=false, bool strict=false);
	void addGlobalRelError(TString,float);

	/**
	 * smoothens large unphysical variations that can occour in case of
	 * some combinations of stat uncertainties when adding relative
	 * systematics
	 * - the strength parameter defines the strength the fluctuation is damped
	 * - the sign parameter gives the maximum significance (1/sigma) of the stat fluctuation
	 *   to be smoothed out
	 * - the threshold parameter defines what to consider a fluctuation. the bin content
	 *   must be <threshold> times higher than the average of the neighboring bins
	 * returns list of syst indicies that were changed
	 */
	std::vector<size_t> removeSpikes(bool inclUFOF=false,int limittoindex=-1,float strength=100000,float sign=0.3,float threshold=8);


	histo2D createPseudoExperiment(TRandom3* rand,const histo2D* c=0, histo1D::pseudodatamodes mode=histo1D::pseudodata_poisson, int syst=-1)const;

	/**
	 * merges partial variations. only the ones corresponding to the identifier are merged
	 * hint: do this step at the latest stage you can do it to still keep track of all correlations
	 * If you know its a global variation its ok to do it earlier
	 */
	void mergePartialVariations(const TString& identifier,bool strictpartialID=true);
	void mergeAllErrors(const TString & mergedname);

	void equalizeSystematicsIdxs(histo2D &rhs);

	std::vector<TString> getSystNameList()const;

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
	void setAllZero();

	static bool debug;
	static std::vector<histo2D*> c_list;
	static bool c_makelist;
	static void c_deletelist(){
		size_t count=0,listsize=c_list.size();
		for(unsigned int i=0;i<listsize;i++){
			if(c_list[count]) delete c_list[count]; //delete also removes entry from list
			else count++; //that should never happen if list not touched from outside
		}
	}


#ifndef __CINT__
	template <class T>
	void writeToStream(T & stream)const;
	template <class T>
	void readFromStream(T & stream);
#endif


	void writeToFile(const std::string& filename)const;
	void readFromFile(const std::string& filename);

protected:

	bool isEqual( const histo2D &rhs)const;

	std::vector<ztop::histo1D> conts_; //! for each y axis bin one container
	std::vector<float> xbins_,ybins_;
	bool divideBinomial_;
	bool mergeufof_;

	TString xaxisname_;
	TString yaxisname_;
	TString zaxisname_;
	//TString zaxisname_;

	void copyFrom(const histo2D&);//!< helper
};

inline size_t ztop::histo2D::getBinNoX(const float & var) const{
	if(conts_.size()>0)
		return conts_.at(0).getBinNo(var);
	else
		return 1e9;
}
inline size_t ztop::histo2D::getBinNoY(const float & var) const {

	if(ybins_.size() <2){
		return 0;
	}
	size_t binno=0;
	std::vector<float>::const_iterator it=std::lower_bound(ybins_.begin()+1, ybins_.end(), var);
	if(var==*it)
		binno= it-ybins_.begin();
	else
		binno= it-ybins_.begin()-1;
	if(mergeufof_){
		if(binno==0)
			binno++;
		else if(binno == ybins_.size()-1)
			binno--;
	}
	return binno;
}
inline void ztop::histo2D::fill(const float & xval, const float & yval, const float & weight){
	size_t ybin=getBinNoY(yval);
	conts_[ybin].fill(xval,weight);
}
/**
 * not protected
 */
inline const histoBin & ztop::histo2D::getBin(const size_t &xbinno,const size_t &ybinno,const int &layer) const{
	if(xbinno>=xbins_.size() || ybinno>= conts_.size()){
		throw std::out_of_range ("histo2D::getBin: xbin or ybin is out of range");// << std::endl;
	}
	return conts_.at(ybinno).getBin(xbinno,layer);
}

inline histoBin & ztop::histo2D::getBin(const size_t &xbinno,const size_t& ybinno,const int &layer){
	if(xbinno>=xbins_.size() || ybinno>= conts_.size()){
		throw std::out_of_range ("histo2D::getBin: xbin or ybin is out of range");// << std::endl;
	}
	return conts_.at(ybinno).getBin(xbinno,layer);
}

#ifndef __CINT__

template <class T>
inline void ztop::histo2D::writeToStream(T & out)const{
	taggedObject::writeToStream(out);
	IO::serializedWrite(conts_,out);
	IO::serializedWrite(xbins_,out);
	IO::serializedWrite(ybins_,out);
	IO::serializedWrite(divideBinomial_,out);
	IO::serializedWrite(mergeufof_,out);
	IO::serializedWrite(xaxisname_,out);
	IO::serializedWrite(yaxisname_,out);
}

template <class T>
inline void ztop::histo2D::readFromStream(T & in){
	taggedObject::readFromStream(in);
	IO::serializedRead(conts_,in);
	IO::serializedRead(xbins_,in);
	IO::serializedRead(ybins_,in);
	IO::serializedRead(divideBinomial_,in);
	IO::serializedRead(mergeufof_,in);
	IO::serializedRead(xaxisname_,in);
	IO::serializedRead(yaxisname_,in);
}

namespace IO{
//implements standardized IO operations for this class


template<class T>
inline void serializedWrite(const histo2D&c, T&stream){
	c.writeToStream(stream);
}
template<class T>
inline void serializedRead(histo2D&c, T&stream){
	c.readFromStream(stream);
}
//re-instantiate vector ops
template<class T>
inline void serializedWrite(const std::vector<histo2D>&in, T&saveFile){
	size_t len=in.size();
	saveFile.write(reinterpret_cast<const char *>(&len), sizeof(len));
	for(size_t i=0;i<len;i++)
		serializedWrite(in.at(i),saveFile);
}
template<class T>
inline void serializedRead(std::vector<histo2D>&in, T&saveFile){
	size_t len=0;
	saveFile.read(reinterpret_cast< char *>(&len), sizeof(len));
	in.resize(len,histo2D());
	for(size_t i=0;i<len;i++)
		serializedRead(in.at(i),saveFile);
}


}//io

#endif
}//ztop
#endif /* CONTAINER2D_H_ */
