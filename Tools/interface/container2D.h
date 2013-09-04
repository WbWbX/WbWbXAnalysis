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

class container2D {
public:
	 container2D();
    container2D( std::vector<float> ,std::vector<float> , TString name="",TString xaxisname="",TString yaxisname="", bool mergeufof=false); //! construct with binning
	~container2D();

	//setaxis etc still missing. should be ok as long as default constr. is protected

	void setName(TString name){name_=name;}
	const TString &getName() const {return name_;}

    size_t getBinNoX(const double&) const; // returns bin index number for (double variable)
    size_t getBinNoY(const double&) const; //! returns bin index number for (double variable)
    size_t getNBinsX() const {return xbins_.size()-2;}       //! returns numberof x bins
    size_t getNBinsY() const {return ybins_.size()-2;}   //! returns numberof x bins
    const size_t & getBinEntries(const size_t&,const size_t&) const;
    const std::vector<float> & getBinsX() const{return xbins_;}     //! returns  x bins
    const std::vector<float> & getBinsY() const{return ybins_;}       //! returns y bins

    void fill(const double & xval, const double & yval, const double & weight=1);    //! fills with weight

    size_t getSystSize() const {if(conts_.size()<1) return 0; else return conts_.at(0).getSystSize();}
    const double &getBinContent(const size_t & xbin,const size_t & ybin) const;
    double getBinErrorUp(const size_t & xbin, const size_t & ybin, bool onlystat=false,TString limittosys="") const;
    double getBinErrorDown(const size_t &,const size_t &,bool onlystat=false,TString limittosys="") const;
    double getBinError(const size_t &,const size_t &,bool onlystat=false,TString limittosys="") const;
    double getSystError(unsigned int number, const size_t & xbin, const size_t & ybin) const;
    double getSystErrorStat(unsigned int number, const size_t & xbin, const size_t & ybin) const;
    const TString & getSystErrorName(const size_t & number) const;
/*
     double projectBinContentToY(const size_t & ybin,bool includeUFOF=false) const;
     double projectBinContentToX(const size_t & xbin,bool includeUFOF=false) const;
*/
     container1D projectToX(bool includeUFOF=false) const;
     container1D projectToY(bool includeUFOF=false) const;

    void removeError(TString);
    void renameSyst(TString, TString);

    TH2D * getTH2D(TString name="", bool dividebybinwidth=false, bool onlystat=false) const;
    TH2D * getTH2DSyst(TString name, unsigned int systNo, bool dividebybinwidth=false, bool statErrors=false) const;

    void setDivideBinomial(bool);

    container2D operator + (const container2D &);       //! adds stat errors in squares; treats same named systematics as correlated!!
    container2D operator - (const container2D &);       //! adds errors in squares; treats same named systematics as correlated!!
    container2D operator / (const container2D &);       //! binomial stat error or uncorr error (depends on setDivideBinomial()); treats same named systematics as correlated
    container2D operator * (const container2D &);       //! adds stat errors in squares; treats same named systematics as correlated!!
    container2D operator * (double);            //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
    container2D operator * (float);             //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
    container2D operator * (int);               //! simple scalar multiplication. stat and syst errors are scaled accordingly!!



    void addErrorContainer(const TString & ,const container2D &,double);  //! adds deviation to (this) as systematic uncertianty with name and weight. name must be ".._up" or ".._down"
    void addErrorContainer(const TString &,const container2D & );        //! adds deviation to (this) as systematic uncertianty with name. name must be ".._up" or ".._down"
    void addRelSystematicsFrom(const container2D &);
    void addGlobalRelError(TString,double);

    void reset();    //! resets all uncertainties and binning, keeps names and axis
    void clear();    //! sets all bin contents to zero; clears all systematic uncertainties


    static bool debug;
    static std::vector<container2D*> c_list;
    static bool c_makelist;

protected:



	std::vector<ztop::container1D> conts_; //! for each y axis bin one container
	std::vector<float> xbins_,ybins_;
    bool divideBinomial_;
    bool mergeufof_;

    TString xaxisname_;
    TString yaxisname_;
    TString name_;

};

inline size_t ztop::container2D::getBinNoX(const double & var) const{
	if(conts_.size()>0)
		return conts_.at(0).getBinNo(var);
	else
		return -1;
}
inline size_t ztop::container2D::getBinNoY(const double & var) const {

  if(ybins_.size() <2){
    return 0;
  }
  std::vector<float>::const_iterator it=std::lower_bound(ybins_.begin()+1, ybins_.end(), var);
  if(var==*it)
    return it-ybins_.begin();
  else
    return it-ybins_.begin()-1;

}
inline void ztop::container2D::fill(const double & xval, const double & yval, const double & weight){
	int ybin=getBinNoY(yval);
	conts_[ybin].fill(xval,weight);
}

}
#endif /* CONTAINER2D_H_ */
