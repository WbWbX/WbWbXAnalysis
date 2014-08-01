/*
 * container2D.h
 *
 *  Created on: Jun 24, 2013
 *      Author: kiesej
 */

#ifndef CONTAINER2D_H_
#define CONTAINER2D_H_

#include "container.h"


/*
 * to be implemented:
 * hasunderflow/overflow (can be function that checks entries in corr containers
 *
 */

namespace ztop{
class container1DUnfold;

class container2D:public taggedObject {
	friend class container1DUnfold;
public:


	enum plotTag{none,topdf};

	container2D();
	container2D(const std::vector<float> & ,const std::vector<float> &, TString name="",TString xaxisname="",TString yaxisname="", bool mergeufof=false); //! construct with binning
	~container2D();

	bool isDummy()const{return xbins_.size()<=2;}

	void setBinning(const std::vector<float> &xbins, std::vector<float> ybins);
	//setaxis etc still missing. should be ok as long as default constr. is protected



	void setXAxisName(const TString& name){xaxisname_=name;}
	const TString& getXAxisName()const {return xaxisname_;}

	void setYAxisName(const TString& name){yaxisname_=name;}
	const TString& getYAxisName()const {return yaxisname_;}


	size_t getBinNoX(const float&) const; // returns bin index number for (float variable)
	size_t getBinNoY(const float&) const; //! returns bin index number for (float variable)
	size_t getNBinsX() const {return xbins_.size()-2;}       //! returns numberof x bins
	size_t getNBinsY() const {return ybins_.size()-2;}   //! returns numberof x bins
	const size_t & getBinEntries(const size_t&,const size_t&) const;
	const std::vector<float> & getBinsX() const{return xbins_;}     //! returns  x bins
	const std::vector<float> & getBinsY() const{return ybins_;}       //! returns y bins

	const histoBin & getBin(const size_t &,const size_t&,const int &layer=-1) const;
	histoBin & getBin(const size_t&,const size_t&,const int& layer=-1);

	float getBinArea(const size_t&, const size_t&)const;

	container2D rebinXToBinning(const std::vector<float>& )const;
	container2D rebinXtoBinning(const container1D&)const;
	container2D rebinYToBinning(const std::vector<float>& )const;
	container2D rebinYtoBinning(const container1D&)const;

	void fill(const float & xval, const float & yval, const float & weight=1);    //! fills with weight

	size_t getSystSize() const {if(conts_.size()<1) return 0; else return conts_.at(0).getSystSize();}
	const float &getBinContent(const size_t & xbin,const size_t & ybin) const;
	float getBinErrorUp(const size_t & xbin, const size_t & ybin, bool onlystat=false,TString limittosys="") const;
	float getBinErrorDown(const size_t &,const size_t &,bool onlystat=false,TString limittosys="") const;
	float getBinError(const size_t &,const size_t &,bool onlystat=false,TString limittosys="") const;
	float getSystError(unsigned int number, const size_t & xbin, const size_t & ybin) const;
	float getSystErrorStat(unsigned int number, const size_t & xbin, const size_t & ybin) const;
	const TString & getSystErrorName(const size_t & number) const;
	/*
     float projectBinContentToY(const size_t & ybin,bool includeUFOF=false) const;
     float projectBinContentToX(const size_t & xbin,bool includeUFOF=false) const;
	 */
	container1D projectToX(bool includeUFOF=false) const;
	container1D projectToY(bool includeUFOF=false) const;
	container1D getYSlice(size_t ybinno) const;
	container1D getXSlice(size_t xbinno) const;

	void setXSlice(size_t xbinno,const container1D &, bool UFOF=true);
	void setYSlice(size_t xbinno,const container1D &, bool UFOF=true);

	void removeError(TString);
	void renameSyst(TString, TString);
	void removeAllSystematics();

	TH2D * getTH2D(TString name="", bool dividebybinarea=false, bool onlystat=false) const;
	TH2D * getTH2DSyst(TString name, unsigned int systNo, bool dividebybinarea=false, bool statErrors=false) const;

	void setDivideBinomial(bool);

	container2D & operator += (const container2D &);       //! adds stat errors in squares; treats same named systematics as correlated!!
	container2D operator + (const container2D &);       //! adds stat errors in squares; treats same named systematics as correlated!!
	container2D & operator -= (const container2D &);       //! adds errors in squares; treats same named systematics as correlated!!
	container2D operator - (const container2D &);       //! adds errors in squares; treats same named systematics as correlated!!
	container2D & operator /= (const container2D &);       //! binomial stat error or uncorr error (depends on setDivideBinomial()); treats same named systematics as correlated
	container2D operator / (const container2D &);       //! binomial stat error or uncorr error (depends on setDivideBinomial()); treats same named systematics as correlated
	container2D operator * (const container2D &);       //! adds stat errors in squares; treats same named systematics as correlated!!
	container2D & operator *= (float val){container2D copy=*this*val;*this=copy;return *this;} //fast hack!            //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
	container2D operator * (float);            //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
	container2D operator * (double val){return *this*(float)val;}             //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
	container2D operator * (int);               //! simple scalar multiplication. stat and syst errors are scaled accordingly!!



	void addErrorContainer(const TString & ,const container2D &,float);  //! adds deviation to (this) as systematic uncertianty with name and weight. name must be ".._up" or ".._down"
	void addErrorContainer(const TString &,const container2D & );        //! adds deviation to (this) as systematic uncertianty with name. name must be ".._up" or ".._down"
	void getRelSystematicsFrom(const container2D &);
	void addGlobalRelError(TString,float);


	/**
	 * merges partial variations. only the ones corresponding to the identifier are merged
	 * hint: do this step at the latest stage you can do it to still keep track of all correlations
	 * If you know its a global variation its ok to do it earlier
	 */
	void mergePartialVariations(const TString& identifier,bool strictpartialID=true);
	void mergeAllErrors(const TString & mergedname);

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


	static bool debug;
	static std::vector<container2D*> c_list;
	static bool c_makelist;
	static void c_deletelist(){
		size_t count=0,listsize=c_list.size();
		for(unsigned int i=0;i<listsize;i++){
			if(c_list[count]) delete c_list[count]; //delete also removes entry from list
			else count++; //that should never happen if list not touched from outside
		}
	}

	plotTag plottag;

protected:



	std::vector<ztop::container1D> conts_; //! for each y axis bin one container
	std::vector<float> xbins_,ybins_;
	bool divideBinomial_;
	bool mergeufof_;

	TString xaxisname_;
	TString yaxisname_;


};

inline size_t ztop::container2D::getBinNoX(const float & var) const{
	if(conts_.size()>0)
		return conts_.at(0).getBinNo(var);
	else
		return -1;
}
inline size_t ztop::container2D::getBinNoY(const float & var) const {

	if(ybins_.size() <2){
		return 0;
	}
	std::vector<float>::const_iterator it=std::lower_bound(ybins_.begin()+1, ybins_.end(), var);
	if(var==*it)
		return it-ybins_.begin();
	else
		return it-ybins_.begin()-1;

}
inline void ztop::container2D::fill(const float & xval, const float & yval, const float & weight){
	int ybin=getBinNoY(yval);
	conts_[ybin].fill(xval,weight);
}
/**
 * not protected
 */
 inline const histoBin & ztop::container2D::getBin(const size_t &xbinno,const size_t &ybinno,const int &layer) const{
	 return conts_[ybinno].getBin(xbinno,layer);
 }
 /**
  * not protected
  */
 inline histoBin & ztop::container2D::getBin(const size_t &xbinno,const size_t& ybinno,const int &layer){
	 return conts_[ybinno].getBin(xbinno,layer);
 }

}
#endif /* CONTAINER2D_H_ */
