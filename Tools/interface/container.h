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

////////bins include their border on the right side!!

namespace ztop{
  
  class container1D{
  public:
    container1D();
    container1D(float , TString name="",TString xaxisname="",TString yaxisname="", bool mergeufof=false);              //! construct with bin width (for dynamic filling - not yet implemented)
    container1D(std::vector<float> , TString name="",TString xaxisname="",TString yaxisname="", bool mergeufof=false); //! construct with binning
    ~container1D();

    TString getName(){return name_;}
    void setName(TString Name){name_=Name;}
    void setNames(TString name, TString xaxis, TString yaxis){name_=name;xname_=xaxis;yname_=yaxis;}
    void setShowWarnings(bool show){showwarnings_=show;}
 
    void fill(const double & val, const double & weight=1);    //! fills with weight
    void fillDyn(double what, double weight=1); //! not implemented yet

    void setBins(std::vector<float>);
    void setBinWidth(float);            //! sets bin width; enables dynamic filling (not implemented yet)
    void setBinErrorUp(int, double);    //! sets systematics in bin to zero and stat error to given value
    void setBinErrorDown(int, double);  //! sets systematics in bin to zero and stat error to given value
    void setBinError(int, double);      //! sets systematics in bin to zero and stat error to given value
    void setBinContent(int, double);    //! sets bin content, does not change error values!


    int getBinNo(const double&); //! returns bin index number for (double variable)
    int getNBins();       //! returns nuberof bins 
    const std::vector<float> & getBins(){return bins_;}
    double getBinCenter(int);
    double getBinWidth(int); //! returns total bin width NOT 0.5* width
    double getBinEntries(int);
    float getXMax(){return bins_[getNBins()+1];}
    float getXMin(){return bins_[1];}


    double getBinContent(int);
    double getBinErrorUp(int, bool onlystat=false,TString limittosys="");
    double getBinErrorDown(int,bool onlystat=false,TString limittosys="");
    double getBinError(int,bool onlystat=false,TString limittosys="");
    double getOverflow();                              //!returns -1 if overflow was merged with last bin
    double getUnderflow();                             //!returns -1 if underflow was merged with last bin

    double integral(bool includeUFOF=false);
    double cIntegral(float from=0, float to=0);       //! includes the full bin "from" or "to" is in

    
    void reset();    //! resets all uncertainties and binning, keeps names and axis
    void clear();    //! sets all bin contents to zero; clears all systematic uncertainties

    void setLabelSize(double size){labelmultiplier_=size;}       //! 1 for default
    TH1D * getTH1D(TString name="", bool dividebybinwidth=true, bool onlystat=false); //! returns a TH1D pointer with symmetrized errors (TString name); small bug with content(bin)=0 and error(bin)=0
    operator TH1D(){return *getTH1D();}
    operator TH1D*(){return getTH1D();}
    void writeTH1D(TString name="",bool dividebybinwidth=true, bool onlystat=false); //! writes TH1D->Write() with symmetrized errors (TString name)
    TGraphAsymmErrors * getTGraph(TString name="", bool dividebybinwidth=true,bool onlystat=false,bool noXErrors=false);
    operator TGraphAsymmErrors(){return *getTGraph();}
    operator TGraphAsymmErrors*(){return getTGraph();}
    void writeTGraph(TString name="",bool dividebybinwidth=true,bool onlystat=false, bool noXErrors=false); //! writes TGraph to TFile (must be opened)
    
    void setDivideBinomial(bool);                                   //! default true
    void setMergeUnderFlowOverFlow(bool merge){mergeufof_=merge;}   //! merges underflow/overflow in first or last bin, respectively

    container1D operator + (const container1D &);       //! adds stat errors in squares; treats same named systematics as correlated!!
    container1D operator - (const container1D &);       //! adds errors in squares; treats same named systematics as correlated!!
    container1D operator / (const container1D &);       //! binomial stat error or uncorr error (depends on setDivideBinomial()); treats same named systematics as correlated
    container1D operator * (const container1D &);       //! adds stat errors in squares; treats same named systematics as correlated!!
    container1D operator * (double);            //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
    container1D operator * (float);             //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
    container1D operator * (int);               //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
    

    void addErrorContainer(TString,container1D,double,bool ignoreMCStat=true);  //! adds deviation to (this) as systematic uncertianty with name and weight. name must be ".._up" or ".._down" 
    void addErrorContainer(TString,container1D ,bool ignoreMCStat=true);        //! adds deviation to (this) as systematic uncertianty with name. name must be ".._up" or ".._down" 
    void addRelSystematicsFrom(const container1D &);
    void addGlobalRelErrorUp(TString,double);    //! adds a global relative uncertainty with name; "..up" automatically added
    void addGlobalRelErrorDown(TString,double);  //! adds a global relative uncertainty with name; "..down" automatically added
    void addGlobalRelError(TString,double);      //! adds a global relative symmetric uncertainty with name; creates ".._up" and ".._down" variation names

    void removeError(TString); //! removes a systematic uncertainty with name ..
    void renameSyst(TString , TString); //! old, new

    void transformStatToSyst(TString);
    void setAllErrorsZero(){for(unsigned int i=0;i<staterrup_.size();i++){staterrup_[i]=0;staterrdown_[i]=0;} syserrors_.clear();} //! sets all errors zero

    static bool c_makelist;
    static std::vector<container1D*> c_list;
    static void c_clearlist(){c_list.clear();}
    static bool debug;

  protected:
    bool showwarnings_;
    float binwidth_;
    std::vector<float> bins_;
    bool canfilldyn_;
    std::vector<double> content_;
    std::vector<long int> entries_;
    std::vector<double> staterrup_;
    std::vector<double> staterrdown_;  //has POSITIVE entries

    std::vector<std::pair<TString, std::vector<double> > > syserrors_; //syst (name, each bin) stores absolute values!

    bool mergeufof_;
    bool wasunderflow_;
    bool wasoverflow_;

    bool divideBinomial_;

    TString name_, xname_, yname_;

    double labelmultiplier_;

    TString stripVariation(TString);
    double getDominantVariationUp(TString,int);
    double getDominantVariationDown(TString,int);

    double sq(double); //helper

  };
  
} //namespace


  //some more operators
ztop::container1D operator * (double multiplier, const ztop::container1D & cont);
ztop::container1D operator * (float multiplier, const ztop::container1D & cont);  //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
ztop::container1D operator * (int multiplier, const ztop::container1D & cont);    //! simple scalar multiplication. stat and syst errors are scaled accordingly!!

inline double ztop::container1D::sq(double a){
  return a*a;
}

inline int ztop::container1D::getBinNo(const double & var){

  if(bins_.size()<1){
    bins_.push_back(0);
    return 0;
  }
  if(bins_.size() <2){
    return 0;
  }
  std::vector<float>::iterator it=std::lower_bound(bins_.begin()+1, bins_.end(), var);
  if(var==*it)
    return it-bins_.begin();
  else 
    return it-bins_.begin()-1;
  
}


inline void ztop::container1D::fill(const double & what, const double & weight){
  int bin=getBinNo(what);
  //int bin=0;
  
  if(mergeufof_){
    if(bin==0 && bins_.size() > 1){
      bin=1;
      wasunderflow_=true;
    }
    else if(bin==(int)bins_.size()-1){
      bin--;
      wasoverflow_=true;
    }
  }
  content_[bin] += weight;
  entries_[bin]++;
  
  staterrup_[bin]=std::sqrt(sq(staterrup_[bin]) + sq(weight));
  staterrdown_[bin]=std::sqrt(sq(staterrdown_[bin]) + sq(weight)); 
  
}
inline void ztop::container1D::fillDyn(double what, double weight){
  if(canfilldyn_){
    what++;
    weight++; //just avoid warnings function not implemented yet. creates new bins based on binwidth_ if needed
  }
  else if(showwarnings_){
    std::cout << "dynamic filling not available with fixed bins!" << std::endl;
  }
}




#endif
