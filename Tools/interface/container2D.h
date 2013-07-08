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
	container2D(){}
    container2D( std::vector<float> ,std::vector<float> , TString name="",TString xaxisname="",TString yaxisname="", bool mergeufof=false); //! construct with binning
	~container2D(){}



    int getBinNoX(const double&); // returns bin index number for (double variable)
    int getBinNoY(const double&); //! returns bin index number for (double variable)
    int getNBinsX();       //! returns numberof x bins
    int getNBinsY();       //! returns numberof x bins



    double getBinContent(int,int);

    TH2D * getTH2D(TString name="", bool dividebybinwidth=true);

    container2D operator + (const container1D &);       //! adds stat errors in squares; treats same named systematics as correlated!!
    container2D operator - (const container1D &);       //! adds errors in squares; treats same named systematics as correlated!!
    container2D operator / (const container1D &);       //! binomial stat error or uncorr error (depends on setDivideBinomial()); treats same named systematics as correlated
    container2D operator * (const container1D &);       //! adds stat errors in squares; treats same named systematics as correlated!!
    container2D operator * (double);            //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
    container2D operator * (float);             //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
    container2D operator * (int);               //! simple scalar multiplication. stat and syst errors are scaled accordingly!!



    void addErrorContainer(TString,container2D,double,bool ignoreMCStat=true);  //! adds deviation to (this) as systematic uncertianty with name and weight. name must be ".._up" or ".._down"
    void addErrorContainer(TString,container2D ,bool ignoreMCStat=true);        //! adds deviation to (this) as systematic uncertianty with name. name must be ".._up" or ".._down"
    void addRelSystematicsFrom(const container2D &);


    void reset();    //! resets all uncertainties and binning, keeps names and axis
    void clear();    //! sets all bin contents to zero; clears all systematic uncertainties

private:
	std::vector<ztop::container1D> conts_; //! for each y axis bin one container
	std::vector<float> ybins_;
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
    ybins_.push_back(0); //FIXME, can be dangerous need to be a container add or something..
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

}
#endif /* CONTAINER2D_H_ */
