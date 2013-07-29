/*
 * container2D.h
 *
 *  Created on: Jun 24, 2013
 *      Author: kiesej
 */

#ifndef CONTAINER2D_H_
#define CONTAINER2D_H_

#include "container.h"

namespace ztop{

class container2D {
public:
	container2D(){divideBinomial_=true;}
    container2D( std::vector<float> ,std::vector<float> , TString name="",TString xaxisname="",TString yaxisname="", bool mergeufof=false); //! construct with binning
	~container2D(){}

	void setName(TString name){name_=name;}
	TString getName(){return name_;}

    int getBinNoX(const double&); // returns bin index number for (double variable)
    int getBinNoY(const double&); //! returns bin index number for (double variable)
    int getNBinsX(){return xbins_.size()-2;}       //! returns numberof x bins
    int getNBinsY(){return ybins_.size()-2;}   //! returns numberof x bins
    double getBinEntries(int,int);
    const std::vector<float> & getBinsX(){return xbins_;}     //! returns  x bins
    const std::vector<float> & getBinsY(){return ybins_;}       //! returns y bins

    void fill(const double & xval, const double & yval, const double & weight=1);    //! fills with weight

    double getBinContent(int xbin,int ybin);
    double getBinErrorUp(int xbin, int ybin, bool onlystat=false,TString limittosys="");
    double getBinErrorDown(int,int,bool onlystat=false,TString limittosys="");
    double getBinError(int,int,bool onlystat=false,TString limittosys="");

    TH2D * getTH2D(TString name="", bool dividebybinwidth=false);

    void setDivideBinomial(bool);

    container2D operator + (const container2D &);       //! adds stat errors in squares; treats same named systematics as correlated!!
    container2D operator - (const container2D &);       //! adds errors in squares; treats same named systematics as correlated!!
    container2D operator / (const container2D &);       //! binomial stat error or uncorr error (depends on setDivideBinomial()); treats same named systematics as correlated
    container2D operator * (const container2D &);       //! adds stat errors in squares; treats same named systematics as correlated!!
    container2D operator * (double);            //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
    container2D operator * (float);             //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
    container2D operator * (int);               //! simple scalar multiplication. stat and syst errors are scaled accordingly!!



    void addErrorContainer(const TString & ,const container2D &,double,bool ignoreMCStat=true);  //! adds deviation to (this) as systematic uncertianty with name and weight. name must be ".._up" or ".._down"
    void addErrorContainer(const TString &,const container2D & ,bool ignoreMCStat=true);        //! adds deviation to (this) as systematic uncertianty with name. name must be ".._up" or ".._down"
    void addRelSystematicsFrom(const container2D &);


    void reset();    //! resets all uncertainties and binning, keeps names and axis
    void clear();    //! sets all bin contents to zero; clears all systematic uncertainties


    static bool debug;

private:
	std::vector<ztop::container1D> conts_; //! for each y axis bin one container
	std::vector<float> xbins_,ybins_;
    bool divideBinomial_;

    TString yaxisname_;
    TString xaxisname_;
    TString name_;

};

inline int ztop::container2D::getBinNoX(const double & var){
	if(conts_.size()>0)
		return conts_.at(0).getBinNo(var);
	else
		return -1;
}
inline int ztop::container2D::getBinNoY(const double & var){

  if(ybins_.size()<1){
    ybins_.push_back(0);
    container1D UFOF;
    conts_.push_back(UFOF);
    return 0;
  }
  if(ybins_.size() <2){
    return 0;
  }
  std::vector<float>::iterator it=std::lower_bound(ybins_.begin()+1, ybins_.end(), var);
  if(var==*it)
    return it-ybins_.begin();
  else
    return it-ybins_.begin()-1;

}
inline void ztop::container2D::fill(const double & xval, const double & yval, const double & weight){
	int ybin=getBinNoY(yval);
	conts_.at(ybin).fill(xval,weight);
}

}
#endif /* CONTAINER2D_H_ */
