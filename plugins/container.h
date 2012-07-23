#ifndef container_h
#define container_h

#include <vector>
#include "TH1D.h"
#include "TGraphAsymmErrors.h"
#include <iostream>
#include <math.h>

namespace top{
  template <class T>
  class List{
  public: 
    List(){list_.clear();};
    ~List(){};
    void push_back(T * cont){list_.push_back(cont);}
    typename std::vector<T *> getList(){return list_;}
    
  private:
    typename std::vector<T *> list_;
    

  };

  class container1D{
  public:
    container1D();
    container1D(float , TString name="",TString xaxisname="",TString yaxisname="");              //! construct with bin width (for dynamic filling - not yet implemented)
    container1D(std::vector<float> , TString name="",TString xaxisname="",TString yaxisname=""); //! construct with binning
    ~container1D();

    TString getName(){return name_;}
    void setNames(TString name, TString xaxis, TString yaxis){name_=name;xname_=xaxis;yname_=yaxis;}
    void copyNames(top::container1D);
    void setShowWarnings(bool show){showwarnings_=show;}
 
    void fill(double);            //! fills
    void fillDyn(double);         //! not implemented yet
    void fill(double, double);    //! fills with weight
    void fillDyn(double, double); //! not implemented yet

    void setBins(std::vector<float>);
    void setBinWidth(float);
    void setBinErrorUp(int, double);
    void setBinErrorDown(int, double);
    void setBinContent(int, double);


    int getBinNo(double); //! returns bin index number for (double variable)
    int getNBins();       //! returns nuberof bins 
    double getBinCenter(int);
    double getBinWidth(int); //! returns total bin width NOT 0.5* width
    float getXMax(){return bins_[getNBins()+1];}
    float getXMin(){return bins_[1];}


    double getBinContent(int);
    double getBinErrorUp(int);
    double getBinErrorDown(int);
    double getBinError(int);
    double getOverflow();
    double getUnderflow();

    void setAllErrorsZero(){for(unsigned int i=0;i<errup_.size();i++){errup_[i]=0;errdown_[i]=0;}}
    void reset();
    void clear();

    void setLabelSize(double size){labelmultiplier_=size;}       //! 1 for default
    TH1D * getTH1D(TString name=""); //! returns a TH1D pointer with symmetrized errors (TString name); small bug with content(bin)=0 and error(bin)=0
    void writeTH1D(TString name=""); //! writes TH1D->Write() with symmetrized errors (TString name)
    TGraphAsymmErrors * getTGraph(TString name="",bool noXErrors=false);  
    void writeTGraph(TString name=""); //! writes TGraph (TString name)

    
    void setDivideBinomial(bool);           //! default true

    container1D operator + (container1D);       //! adds errors in squares!!
    container1D operator - (container1D);       //! adds errors in squares!!
    container1D operator / (container1D);       //! binomial error or uncorr error (depends on setDivideBinomial())
    container1D operator * (container1D);       //! adds errors in squares!!
    container1D operator * (double);            //! simple scalar multiplication. errors are scaled accordingly!!
    container1D operator * (float);             //! simple scalar multiplication. errors are scaled accordingly!!
    container1D operator * (int);               //! simple scalar multiplication. errors are scaled accordingly!!
    container1D & operator = (const container1D &);       //! preserves the name and axis

    void addErrorContainer(container1D,double); //! adds deviation of input container from initial container to errors in quadr with a weight
    void addErrorContainer(container1D);        //! adds deviation of input container from initial container to errors in quadr
    void addGlobalRelErrorUp(double);
    void addGlobalRelErrorDown(double);
    void addGlobalRelError(double);

  protected:
    bool showwarnings_;
    float binwidth_;
    std::vector<float> bins_;
    bool canfilldyn_;
    std::vector<double> content_;
    std::vector<long int> entries_;
    std::vector<double> errup_;
    std::vector<double> errdown_;
    bool divideBinomial_;

    TString name_, xname_, yname_;

    double labelmultiplier_;

  };


  List<container1D> * c_list=0;


  ///////make definitions
  container1D::container1D(){
    canfilldyn_=false;
    divideBinomial_=true;
    labelmultiplier_=1;
    showwarnings_=false;
  }
  container1D::container1D(float binwidth, TString name,TString xaxisname,TString yaxisname){ //currently not used
    binwidth_=binwidth;
    canfilldyn_=true;
    divideBinomial_=true;
    name_=name;
    xname_=xaxisname;
    yname_=yaxisname;
    labelmultiplier_=1;
    showwarnings_=false;
    if(c_list) c_list->push_back(this);
  }
  container1D::container1D(std::vector<float> bins, TString name,TString xaxisname,TString yaxisname){
    setBins(bins);
    divideBinomial_=true;
    name_=name;
    xname_=xaxisname;
    yname_=yaxisname;
    labelmultiplier_=1;
    showwarnings_=false;
    if(c_list) c_list->push_back(this);
  }
  container1D::~container1D(){

  }
  void container1D::copyNames(top::container1D cont){
    name_=cont.name_;
    xname_=cont.xname_;
    yname_=cont.yname_;
  }

  void container1D::setBins(std::vector<float> bins){
    reset();
    bins_=bins;
    bins_.insert(bins_.begin(),0); //underflow
                                   //overflow is the last one
    canfilldyn_=false;
    for(unsigned int i=0; i<bins_.size(); i++){
      content_.push_back(0);
      entries_.push_back(0);
      errup_.push_back(0);
      errdown_.push_back(0);
    }
  }
  void container1D::setBinWidth(float binwidth){
    binwidth_=binwidth;
    canfilldyn_=true;
  }


  void container1D::fill(double what){
    fill(what,1);
  }
  void container1D::fillDyn(double what){
    fillDyn(what,1);
  }
  void container1D::fill(double what, double weight){
    int bin=getBinNo(what);
    content_[bin] += weight;
    entries_[bin]++;
    errup_[bin]= sqrt(pow(errup_[bin],2) + pow(weight,2));
    errdown_[bin]=sqrt(pow(errdown_[bin],2) + pow(weight,2));
  }
  void container1D::fillDyn(double what, double weight){
    if(canfilldyn_){
      what++;
      weight++; //just avoid warnings function not implemented yet. creates new bins based on binwidth_ if needed
    }
    else if(showwarnings_){
      std::cout << "dynamic filling not available with fixed bins!" << std::endl;
    }
  }
  void container1D::setBinErrorUp(int bin, double err){
    if((unsigned int)bin<bins_.size()) errup_[bin] = err;
    else if(showwarnings_)std::cout << "setBinErrorUp: bin not existent!" << std::endl;
  }
  void container1D::setBinErrorDown(int bin, double err){
    if((unsigned int)bin<bins_.size()) errdown_[bin] = err;
    else std::cout << "setBinErrorDown: bin not existent!" << std::endl;
  }
  void container1D::setBinContent(int bin, double content){
    if((unsigned int)bin<bins_.size()){
      entries_[bin] =1;
      content_[bin] = content;
    }
    else if(showwarnings_){
      std::cout << "setBinContent: bin not existent!" << std::endl;
    }
  }

  int container1D::getBinNo(double var){
    int binnumber=bins_.size()-1;
    if(bins_.size()==0){ //create underflow bin if empty vect.
      bins_.push_back(0);
      binnumber=0;
    }
    else{
      for(unsigned int i=1; i<bins_.size(); i++){
	if(var > bins_[i]){
	  continue;
	}
	else{
	  binnumber=i-1;
	  break;
	}
      }
    }
    return binnumber;
  }

  int container1D::getNBins(){
    return (int)bins_.size()-2;
  }
  double container1D::getBinCenter(int bin){
    double center=0;
    if(!((unsigned int)bin<bins_.size()-1) && showwarnings_){
      std::cout << "getBinCenter: bin not existent!" << std::endl;
    }
    else{
      center = (bins_[bin]+bins_[bin+1])/2;
    }
    return center;
  }
  double container1D::getBinWidth(int bin){
    double width=0;
    if(!((unsigned int)bin<bins_.size()-1) && showwarnings_){
      std::cout << "getBinCenter: bin not existent!" << std::endl;
    }
    else{
      width=fabs(bins_[bin+1]-bins_[bin]);
    }
    return width;
  }
  double container1D::getBinContent(int bin){
    if((unsigned int)bin<bins_.size()){
      return content_[bin];
    }
    else{
      if(showwarnings_)std::cout << "getBinContent: bin not existent!" << std::endl;
      return 0;
    }
  }
  double container1D::getBinErrorUp(int bin){
    if((unsigned int)bin<bins_.size()){
      return errup_[bin];
    }
    else{
      if(showwarnings_)std::cout << "getBinErrorUp: bin not existent!" << std::endl;
      return 0;
    }
  }
  double container1D::getBinErrorDown(int bin){
    if((unsigned int)bin<bins_.size()){
      return errdown_[bin];
    }
    else{
      if(showwarnings_) std::cout << "getBinErrorDown: bin not existent!" << std::endl;
      return 0;
    }
  }
  double container1D::getBinError(int bin){
    if((unsigned int)bin<bins_.size()){
      if(fabs(getBinErrorDown(bin)) > fabs(getBinErrorUp(bin))){
	return fabs(getBinErrorDown(bin));
      }
      else{
	return fabs(getBinErrorUp(bin));
      }
    }
    else{
      if(showwarnings_)std::cout << "getBinError: bin not existent!" << std::endl;
      return 0;
    }
  }
  double container1D::getOverflow(){
    double ret;
    if(content_.size()>0){
      ret= content_[content_.size()-1];
    }
    else{
      ret= 0;
    }
    return ret;
  }
  double container1D::getUnderflow(){
    double ret;
    if(content_.size()>0){
      ret= content_[0];
    }
    else{
      ret= 0;
    }
    return ret;
  }


  void container1D::reset(){
    binwidth_=0;
    bins_.clear();
    canfilldyn_=false;
    content_.clear();
    entries_.clear();
    errup_.clear();
    errdown_.clear();
  }
  void container1D::clear(){
    for(unsigned int i=0;i<content_.size();i++){
      content_[i]=0;
      entries_[i]=0;
      errup_[i]=0;
      errdown_[i]=0;
    }
  }




  TH1D * container1D::getTH1D(TString name){ //still missing overflow and underflow
    if(name=="") name=name_;
    double binarray[getNBins()+1];
    for(int i=0; i<=getNBins() ;i++){
      binarray[i]=bins_[i+1];
    }
    TH1D *  h = new TH1D(name,name,getNBins(),binarray);
    double entriessum=0;
    for(int i=1;i<=getNBins();i++){
      h->SetBinContent(i,getBinContent(i));
      h->SetBinError(i,getBinError(i));
      entriessum +=entries_[i];
    }
    h->SetEntries(entriessum);
    h->GetYaxis()->SetTitleSize(0.06*labelmultiplier_);
    h->GetYaxis()->SetLabelSize(0.05*labelmultiplier_);
    h->GetYaxis()->SetTitleOffset(h->GetYaxis()->GetTitleOffset() / labelmultiplier_);
    h->GetYaxis()->SetTitle(yname_);
    h->GetYaxis()->SetNdivisions(510);
    h->GetXaxis()->SetTitleSize(0.06*labelmultiplier_);
    h->GetXaxis()->SetLabelSize(0.05*labelmultiplier_);
    h->GetXaxis()->SetTitle(xname_);
    h->SetMarkerStyle(20);
    return h;
  }

  void container1D::writeTH1D(TString name){
    TH1D * h = getTH1D(name);
    h->Draw();
    h->Write();
    delete h;
  }

  TGraphAsymmErrors * container1D::getTGraph(TString name, bool noXErrors){

    if(name=="") name=name_;
    double x[getNBins()];
    double xeh[getNBins()];
    double xel[getNBins()];
    double y[getNBins()];
    double yel[getNBins()];
    double yeh[getNBins()];
    for(int i=1;i<=getNBins();i++){
      x[i-1]=getBinCenter(i);
      if(noXErrors){
	xeh[i-1]=0;
	xel[i-1]=0;
      }
      else{
	xeh[i-1]=getBinWidth(i)/2;
	xel[i-1]=getBinWidth(i)/2;
      }
      y[i-1]=getBinContent(i);
      yeh[i-1]=getBinErrorUp(i);
      yel[i-1]=getBinErrorDown(i);
    }
    TGraphAsymmErrors * g = new TGraphAsymmErrors(getNBins(),x,y,xel,xeh,yel,yeh);
    g->SetName(name);
    g->SetTitle(name);
    g->GetYaxis()->SetTitleSize(0.06*labelmultiplier_);
    g->GetYaxis()->SetLabelSize(0.05*labelmultiplier_);
    g->GetYaxis()->SetTitleOffset(g->GetYaxis()->GetTitleOffset() / labelmultiplier_);
    g->GetYaxis()->SetTitle(yname_);
    g->GetXaxis()->SetTitleSize(0.06*labelmultiplier_);
    g->GetXaxis()->SetLabelSize(0.05*labelmultiplier_);
    g->GetYaxis()->SetNdivisions(510);
    g->GetXaxis()->SetTitle(xname_);
    g->SetMarkerStyle(20);
    g->GetXaxis()->SetRangeUser(bins_[1], bins_[getNBins()+1]);

    return g;
  }
  void container1D::writeTGraph(TString name){
    TGraphAsymmErrors * g=getTGraph();
    g->SetName(name);
    g->Draw("AP");
    g->Write();
    delete g;
  }

  void container1D::setDivideBinomial(bool divideBinomial){
    divideBinomial_=divideBinomial;
  }

  container1D container1D::operator + (container1D second){
    if(bins_ != second.bins_){
      if(showwarnings_) std::cout << "operator +: not same binning!" << std::endl;
    }
    else{
      for(unsigned int i=0; i<content_.size(); i++){
	second.content_[i] += content_[i];
	second.entries_[i] += entries_[i];
	second.errup_[i] = sqrt(pow(second.errup_[i],2) + pow(errup_[i],2));
	second.errdown_[i] = sqrt(pow(second.errdown_[i],2) + pow(errdown_[i],2));
      }
    }
    return second;
  }

  container1D container1D::operator - (container1D second){
    if(bins_ != second.bins_){
      if(showwarnings_) std::cout << "operator -: not same binning!" << std::endl;
    }
    else{
      for(unsigned int i=0; i<content_.size(); i++){
	second.content_[i] -= content_[i];
	second.entries_[i] -= entries_[i];
	second.errup_[i] = sqrt(pow(second.errup_[i],2) + pow(errup_[i],2));
	second.errdown_[i] = sqrt(pow(second.errdown_[i],2) + pow(errdown_[i],2));
      }
    }
    return second;
  }

  container1D container1D::operator / (container1D second){
    container1D out= second;
    if(bins_ != second.bins_ || (divideBinomial_!=second.divideBinomial_)){
      if(showwarnings_) std::cout << "operator /: not same binning or different divide options!" << std::endl;
    }
    else{
      for(unsigned int i=0; i<content_.size(); i++){
	if(second.content_[i]!=0){
	  double content=content_[i] / second.content_[i];
	  double errup, errdown;
	  if(divideBinomial_){
	    errup=sqrt(content*(1-content)/second.entries_[i]);
	    errdown=errup;
	  }
	  else{
	    errup=sqrt(pow(content * (1/(1-second.errdown_[i]) - 1),2) + pow(content/content_[i] * errup_[i],2) );
	    errdown=sqrt(pow(content * (1/(1+second.errup_[i]) - 1),2) + pow(content/content_[i] * errdown_[i],2) );
	  }
	  out.content_[i]=content;
	  out.errup_[i]=errup;
	  out.errdown_[i]=errdown;
	  out.entries_[i]=second.entries_[i];
	}
	else{
	  out.content_[i]=0;
	  out.errup_[i]=0;
	  out.errdown_[i]=0;
	  if(i!=0 && i != content_.size()-1){ 
	    if(showwarnings_) std::cout << "warning: bin with denominator = 0!" << std::endl;
	  }
	}
      }
    }
    return out;
  }
  container1D container1D::operator * (container1D second){
    if(bins_ != second.bins_){
      if(showwarnings_) std::cout << "operator *: not same binning!" << std::endl;
    }
    else{
      for(unsigned int i=0; i<content_.size(); i++){
	second.content_[i] *= content_[i];
	second.errup_[i] = sqrt(pow(errup_[i]*second.content_[i],2) + pow(content_[i]*second.errup_[i],2));
	second.errdown_[i] = sqrt(pow(errdown_[i]*second.content_[i],2) + pow(content_[i]*second.errdown_[i],2));
	second.entries_[i]+=entries_[i];
      }
    }
    return second;
  }
  container1D container1D::operator * (double scalar){
    container1D out= * this;
    out.clear();
      for(unsigned int i=0; i<content_.size(); i++){
	out.content_[i] = content_[i] * scalar;
	out.errup_[i]   = errup_[i] * scalar;
	out.errdown_[i] = errdown_[i] * scalar;
	out.entries_[i] = entries_[i];
      }
    
    return out;
  }
  container1D container1D::operator * (float scalar){
    double scalard=(double)scalar;
    return *this * scalard;
  }
  container1D container1D::operator * (int scalar){
    double scalard=(double)scalar;
    return *this * scalard;
  }
  container1D & container1D::operator = (const container1D & sec){
    binwidth_=sec.binwidth_;
    bins_=sec.bins_;
    content_=sec.content_;
    entries_=sec.entries_;
    errup_=sec.errup_;
    errdown_=sec.errdown_;
    divideBinomial_=sec.divideBinomial_;

    return *this;
  }
  void container1D::addErrorContainer(container1D deviatingContainer, double weight){
    if(bins_!=deviatingContainer.bins_){
      if(showwarnings_) std::cout << "addErrorContainer(): not same binning!" << std::endl;
    }
    else{
      for(unsigned int i=0; i<content_.size(); i++){
	double deviation= weight * (deviatingContainer.content_[i] - content_[i]);
	if(deviation>0){
	  errup_[i]=sqrt(pow(errup_[i],2) + pow(deviation,2));
	}
	else{
	  errdown_[i]=sqrt(pow(errdown_[i],2) + pow(deviation,2));
	}
      }
    }
  }
  void container1D::addErrorContainer(container1D deviatingContainer){
    addErrorContainer(deviatingContainer,1);
  }

  void container1D::addGlobalRelErrorUp(double relerr){
    for(unsigned int i=0; i<content_.size(); i++){
      errup_[i]=sqrt(pow(errup_[i],2) + pow(content_[i]*relerr,2));
    }
  }
  void container1D::addGlobalRelErrorDown(double relerr){
    for(unsigned int i=0; i<content_.size(); i++){
      errdown_[i]=sqrt(pow(errdown_[i],2) + pow(content_[i]*relerr,2));
    }
  }
  void container1D::addGlobalRelError(double relerr){
    addGlobalRelErrorUp(relerr);
    addGlobalRelErrorDown(relerr);
  }

} //namespace

#endif
