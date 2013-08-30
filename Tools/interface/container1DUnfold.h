/*
 * container1DUnfold.h
 *
 *  Created on: Aug 14, 2013
 *      Author: kiesej
 */

#ifndef CONTAINER1DUNFOLD_H_
#define CONTAINER1DUNFOLD_H_

#include "container2D.h"
#include <iostream>
#include "TSpline.h"
#include "unfolder.h"

/**
 * Warning:
 * due to internal reasons, statistics on systematic variation are NOT accounted for.
 * To change this, the whole container1D substructure would need to be changed
 * ->do in future
 */

/*
 * helper class to store unfolding output etc
 *
 */



#define TEMPGENDEFAULT -99999

namespace ztop{
class container1DUnfold: public container2D {
public:
	container1DUnfold();
	container1DUnfold( std::vector<float> genbins, std::vector<float> recobins, TString name="",TString xaxisname="",TString yaxisname="", bool mergeufof=false);

	~container1DUnfold();


	void clear(){container2D::clear(); gencont_.clear(); datacont_.clear();}
	void reset(){container2D::reset(); gencont_.reset();datacont_.reset();}
	void removeError(TString name){container2D::removeError(name); gencont_.removeError(name); datacont_.removeError(name);}
	void renameSyst(TString old , TString New){container2D::renameSyst(old,New);gencont_.renameSyst(old,New);datacont_.renameSyst(old, New);}


	void fillGen(const double & val, const double & weight=1); //if isMC false only fill one specific container - which one?? 0<->underflow? or first one...?
	void fillReco(const double & yval, const double & weight=1);

	void setMC(bool ismc){isMC_=ismc; if(!isMC_) flushed_=true;}
	bool getMC(){return isMC_;}


	container1D getGenContainer(){return gencont_;}
	container1D getDataContainer(){return datacont_;}


	void setDataContainer(container1D cont){datacont_=cont;}

	/*
	 * only container1DUnfold operator * (double) operates on the unfolded distribution.. let's see for the others
	 */

    container1DUnfold operator + (const container1DUnfold &);       //! adds stat errors in squares; treats same named systematics as correlated!!
    container1DUnfold operator - (const container1DUnfold &);       //! adds errors in squares; treats same named systematics as correlated!!
    container1DUnfold operator / (const container1DUnfold &);       //! binomial stat error or uncorr error (depends on setDivideBinomial()); treats same named systematics as correlated
    container1DUnfold operator * (const container1DUnfold &);       //! adds stat errors in squares; treats same named systematics as correlated!!
    container1DUnfold operator * (double);            //! simple scalar multiplication. stat and syst errors are scaled accordingly!!

    void setDivideBinomial(bool);

    void addErrorContainer(const TString & ,const container1DUnfold &,double weight,bool ignoreMCStat=true);  //! adds deviation to (this) as systematic uncertianty with name and weight. name must be ".._up" or ".._down"
    void addErrorContainer(const TString & ,const container1DUnfold &,bool ignoreMCStat=true);  //! adds deviation to (this) as systematic uncertianty with name and weight. name must be ".._up" or ".._down"
    void addRelSystematicsFrom(const container1DUnfold &);
    void addGlobalRelError(TString,double);

    static std::vector<container1DUnfold *> c_list;
    static bool debug;
    static bool c_makelist;
    static bool printinfo;

    /**
     * does nothing for data where flushed_ is always true
     */
    void flush(){if(!flushed_) flushMC();flushed_=true;}

    int unfold(TUnfold::ERegMode regmode=TUnfold::kRegModeCurvature, bool LCurve=true);

    container1D getUnfolded(){return unfolded_;}
    container1D getRefolded(){return refolded_;}

    container1D getPurity();
    container1D getStability(bool includeeff);
    bool checkCongruentBinBoundaries();

    TH2D * prepareRespMatrix(bool nominal=true,unsigned int systNumber=0);

    unfolder * getUnfolder(int num=-1){if(num<0) return unfold_;else return unfolds_.at(num);}

    static void flushAllListed();

private:

    void fill(const double & xval, const double & yval, const double & weight=1){
    	container2D::fill(xval,yval,weight);
    }
    void flushMC();


   TString xaxis1Dname_,yaxis1Dname_;

   double tempgen_,tempreco_,tempgenweight_,tempweight_;
   bool recofill_;

   bool isMC_;
   bool flushed_;

   container1D gencont_,datacont_,unfolded_,refolded_;

   //destructor safe
   unfolder * unfold_;
   std::vector<unfolder *> unfolds_;

   //some functions that should not be used although inherited
   void addErrorContainer(const TString & ,const container2D &,double,bool){}
   void addErrorContainer(const TString &,const container2D & ,bool){}
   void addRelSystematicsFrom(const container2D &){}



};
inline void container1DUnfold::flushMC(){ //only for MC
	if(conts_.size()<1){
		std::cout << "container1DUnfold::flush: attempt to flush container1DUnfold with no bins - exit" << std::endl;
		std::exit(EXIT_FAILURE);
	}
	gencont_.fill(tempgen_,tempgenweight_);
	if(recofill_){
		fill(tempgen_,tempreco_,tempweight_);
	}
	else{ //put in UF bin
		fill(tempgen_,ybins_.at(0)-100,tempgenweight_);
	}
	recofill_=false;
}


inline void container1DUnfold::fillGen(const double & val, const double & weight){
	flushed_=false;
	if(isMC_){
		if(TEMPGENDEFAULT != tempgen_)
			flushMC();
		tempgen_=val;
		tempgenweight_=weight;
	}
}

inline void container1DUnfold::fillReco(const double & val, const double & weight){ //fills and resets tempgen_
	if(isMC_){
		tempreco_=val;
		tempweight_=weight;
		recofill_=true;
		datacont_.fill(val,weight);
	}
	else{
		datacont_.fill(val,weight);
	}
}





} //namespace

#endif /* CONTAINER1DUNFOLD_H_ */
