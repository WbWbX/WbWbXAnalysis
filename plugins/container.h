#ifndef container_h
#define container_h

#include <vector>
#include "TH1D.h"
#include "TGraphAsymmErrors.h"
#include <iostream>
#include <math.h>
#include "List.h"
#include <utility>
#include "TPad.h"
#include "miscUtils.h"

namespace top{
  
  class container1D{
  public:
    container1D();
    container1D(float , TString name="",TString xaxisname="",TString yaxisname="", bool mergeufof=false);              //! construct with bin width (for dynamic filling - not yet implemented)
    container1D(std::vector<float> , TString name="",TString xaxisname="",TString yaxisname="", bool mergeufof=false); //! construct with binning
    ~container1D();

    TString getName(){return name_;}
    void setNames(TString name, TString xaxis, TString yaxis){name_=name;xname_=xaxis;yname_=yaxis;}
    void setShowWarnings(bool show){showwarnings_=show;}
 
    void fill(double);            //! fills
    void fillDyn(double);         //! not implemented yet
    void fill(double, double);    //! fills with weight
    void fillDyn(double, double); //! not implemented yet

    void setBins(std::vector<float>);
    void setBinWidth(float);            //! sets bin width; enables dynamic filling (not implemented yet)
    void setBinErrorUp(int, double);    //! sets systematics in bin to zero and stat error to given value
    void setBinErrorDown(int, double);  //! sets systematics in bin to zero and stat error to given value
    void setBinError(int, double);      //! sets systematics in bin to zero and stat error to given value
    void setBinContent(int, double);    //! sets bin content, does not change error values!


    int getBinNo(double); //! returns bin index number for (double variable)
    int getNBins();       //! returns nuberof bins 
    double getBinCenter(int);
    double getBinWidth(int); //! returns total bin width NOT 0.5* width
    float getXMax(){return bins_[getNBins()+1];}
    float getXMin(){return bins_[1];}


    double getBinContent(int);
    double getBinErrorUp(int,TString limittosys="");
    double getBinErrorDown(int,TString limittosys="");
    double getBinError(int,TString limittosys="");
    double getOverflow();                              //!returns -1 if overflow was merged with last bin
    double getUnderflow();                             //!returns -1 if underflow was merged with last bin

    double integral(bool includeUFOF=false);

    void setAllErrorsZero(){for(unsigned int i=0;i<staterrup_.size();i++){staterrup_[i]=0;staterrdown_[i]=0;} syserrors_.clear();} //! sets all errors zero
    void reset();    //! resets all uncertainties and binning, keeps names and axis
    void clear();    //! sets all bin contents to zero; clears all systematic uncertainties

    void setLabelSize(double size){labelmultiplier_=size;}       //! 1 for default
    TH1D * getTH1D(TString name="", bool dividebybinwidth=true); //! returns a TH1D pointer with symmetrized errors (TString name); small bug with content(bin)=0 and error(bin)=0
    operator TH1D(){return *getTH1D();}
    operator TH1D*(){return getTH1D();}
    void writeTH1D(TString name=""); //! writes TH1D->Write() with symmetrized errors (TString name)
    TGraphAsymmErrors * getTGraph(TString name="",bool noXErrors=false, bool dividebybinwidth=true);  
    operator TGraphAsymmErrors(){return *getTGraph();}
    operator TGraphAsymmErrors*(){return getTGraph();}
    void writeTGraph(TString name=""); //! writes TGraph to TFile (must be opened)

    
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
    void addSystematicsFrom(const container1D &);
    void addGlobalRelErrorUp(TString,double);    //! adds a global relative uncertainty with name; "..up" automatically added
    void addGlobalRelErrorDown(TString,double);  //! adds a global relative uncertainty with name; "..down" automatically added
    void addGlobalRelError(TString,double);      //! adds a global relative symmetric uncertainty with name; creates ".._up" and ".._down" variation names

    void removeError(TString); //! removes a systematic uncertainty with name ..

  protected:
    bool showwarnings_;
    float binwidth_;
    std::vector<float> bins_;
    bool canfilldyn_;
    std::vector<double> content_;
    std::vector<long int> entries_;
    std::vector<double> staterrup_;
    std::vector<double> staterrdown_;

    std::vector<std::pair<TString, std::vector<double> > > syserrors_;

    bool mergeufof_;
    bool wasunderflow_;
    bool wasoverflow_;

    bool divideBinomial_;

    TString name_, xname_, yname_;

    double labelmultiplier_;

    TString stripVariation(TString);
    double getDominantVariationUp(TString,int);
    double getDominantVariationDown(TString,int);

  };


  //some more operators
  top::container1D operator * (double multiplier, const top::container1D & cont){ //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
    top::container1D out=cont;
    return out * multiplier;
  }
  top::container1D operator * (float multiplier, const top::container1D & cont){  //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
    top::container1D out=cont;
    return out * multiplier;
  }
  top::container1D operator * (int multiplier, const top::container1D & cont){    //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
    top::container1D out=cont;
    return out * multiplier;
  }


  //////////if you like cut here ;) 

  List<container1D> * c_list=0;


  ///////function definitions
  container1D::container1D(){
    canfilldyn_=false;
    divideBinomial_=true;
    labelmultiplier_=1;
    showwarnings_=true;
    mergeufof_=true;
    wasunderflow_=false;
    wasoverflow_=false;
    if(c_list) c_list->push_back(this);
  }
  container1D::container1D(float binwidth, TString name,TString xaxisname,TString yaxisname, bool mergeufof){ //currently not used
    binwidth_=binwidth;
    canfilldyn_=true;
    divideBinomial_=true;
    name_=name;
    xname_=xaxisname;
    yname_=yaxisname;
    labelmultiplier_=1;
    showwarnings_=true;
    if(c_list) c_list->push_back(this);
    mergeufof_=mergeufof;
    wasunderflow_=false;
    wasoverflow_=false;
  }
  container1D::container1D(std::vector<float> bins, TString name,TString xaxisname,TString yaxisname, bool mergeufof){
    setBins(bins);
    divideBinomial_=true;
    name_=name;
    xname_=xaxisname;
    yname_=yaxisname;
    labelmultiplier_=1;
    showwarnings_=true;
    if(c_list) c_list->push_back(this);
    mergeufof_=mergeufof;
    wasunderflow_=false;
    wasoverflow_=false;
  }
  container1D::~container1D(){

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
      staterrup_.push_back(0);
      staterrdown_.push_back(0);
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
    staterrup_[bin]= sqrt(pow(staterrup_[bin],2) + pow(weight,2));
    staterrdown_[bin]=sqrt(pow(staterrdown_[bin],2) + pow(weight,2));
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
    if((unsigned int)bin<bins_.size()){
      staterrup_[bin] = err;
      for(unsigned int i=0;i<syserrors_.size();i++){
	syserrors_[i].second[bin] = 0;
      }
    }
    else{
      std::cout << "container1D::setBinErrorUp: bin not existent!" << std::endl;
    }
  }
  void container1D::setBinErrorDown(int bin, double err){
    if((unsigned int)bin<bins_.size()){
      staterrdown_[bin] = err;
      for(unsigned int i=0;i<syserrors_.size();i++){
	syserrors_[i].second[bin] = 0;
      }
    }
    else{
      std::cout << "container1D::setBinErrorDown: bin not existent!" << std::endl;
    }
  }
  void container1D::setBinError(int bin, double err){
    setBinErrorUp(bin, err);
    setBinErrorDown(bin,err);
  }
  void container1D::setBinContent(int bin, double content){
    if((unsigned int)bin<bins_.size()){
      entries_[bin] =1;
      content_[bin] = content;
    }
    else{
      std::cout << "container1D::setBinContent: bin not existent!" << std::endl;
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
    if(!((unsigned int)bin<bins_.size()) && showwarnings_){
      std::cout << "container1D::getBinCenter: ("<< name_ <<") bin not existent!" << std::endl;
    }
    else if((unsigned int)bin==bins_.size()){ // overflow
      center = 0;
    }
    else if((unsigned int)bin==0){
      center = 0;
    }
    else {
      center = (bins_[bin]+bins_[bin+1])/2;
    }
    return center;
  }
  double container1D::getBinWidth(int bin){
    double width=0;
    if(!((unsigned int)bin<bins_.size()-1) && showwarnings_){
      std::cout << "container1D::getBinWidth: ("<< name_ <<") bin not existent!" << std::endl;
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
      if(showwarnings_)std::cout << "container1D::getBinContent: ("<< name_ <<") bin not existent!" << std::endl;
      return 0;
    }
  }
  double container1D::getBinErrorUp(int bin, TString limittosys){
    double fullerr2=0;
    if((unsigned int)bin<bins_.size()){
      fullerr2=pow(staterrup_[bin],2); //stat
      if(limittosys=""){
	// make vector of all sys stripped
	std::vector<TString> sources;
	for(unsigned int i=0;i<syserrors_.size();i++){
	  TString source=stripVariation(syserrors_[i].first);
	  if(-1==isIn(source,sources)){
	    sources.push_back(source);
	    fullerr2 += pow(getDominantVariationUp(source,bin), 2);
	  }
	}
      }
      else{
	fullerr2 += pow(getDominantVariationUp(limittosys,bin), 2);
      }
      return sqrt(fullerr2);
    }
    else{
      if(showwarnings_)std::cout << "container1D::getBinErrorUp: bin not existent!" << std::endl;
      return 0;
    }
  }
  double container1D::getBinErrorDown(int bin,TString limittosys){
    double fullerr2=0;
    if((unsigned int)bin<bins_.size()){
      fullerr2=pow(staterrup_[bin],2); //stat
      if(limittosys=""){
	// make vector of all sys stripped
	std::vector<TString> sources;
	for(unsigned int i=0;i<syserrors_.size();i++){
	  TString source=stripVariation(syserrors_[i].first);
	  if(-1==isIn(source,sources)){
	    sources.push_back(source);
	    fullerr2 += pow(getDominantVariationDown(source,bin), 2);
	  }
	}
      }
      else{
	fullerr2 += pow(getDominantVariationUp(limittosys,bin), 2);
      }
      return sqrt(fullerr2);
    }
    else{
      if(showwarnings_)std::cout << "container1D::getBinErrorUp: bin not existent!" << std::endl;
      return 0;
    }
  }
  double container1D::getBinError(int bin,TString limittosys){
    double symmerror=0;
    if(getBinErrorUp(bin,limittosys) > fabs(getBinErrorDown(bin,limittosys))) symmerror=getBinErrorUp(bin,limittosys);
    else symmerror=fabs(getBinErrorDown(bin,limittosys));
    return symmerror;
  }
  double container1D::getOverflow(){
    double ret;
    if(content_.size()>0){
      ret= content_[content_.size()-1];
    }
    else{
      ret= 0;
    }
    if(wasoverflow_) ret=-1.;
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
    if(wasunderflow_) ret=-1.;
    return ret;
  }

  double container1D::integral(bool includeUFOF){
    unsigned int minbin,maxbin;
    if(includeUFOF){
      minbin=0;
      maxbin=content_.size();
    }
    else{
      minbin=1;
      maxbin=content_.size() -1;
    }
    double integr=0;
    for(unsigned int i=minbin;i<maxbin;i++){
      integr+=content_[i];
    }
    return integr;
  }

  void container1D::reset(){
    binwidth_=0;
    bins_.clear();
    canfilldyn_=false;
    content_.clear();
    entries_.clear();
    staterrup_.clear();
    staterrdown_.clear();
    syserrors_.clear();
  }
  void container1D::clear(){
    for(unsigned int i=0;i<content_.size();i++){
      content_[i]=0;
      entries_[i]=0;
      staterrup_[i]=0;
      staterrdown_[i]=0;
    }
    syserrors_.clear();
  }

  TH1D * container1D::getTH1D(TString name, bool dividebybinwidth){ 
    if(name=="") name=name_;
    double binarray[getNBins()+1];
    for(int i=0; i<=getNBins() ;i++){
      binarray[i]=bins_[i+1];
    }
    TH1D *  h = new TH1D(name,name,getNBins(),binarray);
    double entriessum=0;
    for(int i=0;i<=getNBins()+1;i++){ // 0 underflow, genBins+1 overflow
      double cont=getBinContent(i);
      if(dividebybinwidth && i>0 && i<getNBins()+1) cont=cont/getBinWidth(i);
      h->SetBinContent(i,cont);
      double err=getBinError(i);
      if(dividebybinwidth && i>0 && i<getNBins()+1) err=err/getBinWidth(i);
      h->SetBinError(i,err);
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
    h->LabelsDeflate("X");
    h->SetMarkerStyle(20);
    if(wasunderflow_) h->SetTitle((TString)h->GetTitle() + "_uf");
    if(wasoverflow_)  h->SetTitle((TString)h->GetTitle() + "_of");
    return h;
  }

  void container1D::writeTH1D(TString name){
    TH1D * h = getTH1D(name);
    h->Draw();
    h->Write();
    delete h;
  }

  TGraphAsymmErrors * container1D::getTGraph(TString name, bool noXErrors, bool dividebybinwidth){

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
      if(dividebybinwidth){
	y[i-1]=getBinContent(i)/getBinWidth(i); 
	yeh[i-1]=getBinErrorUp(i)/getBinWidth(i); 
	yel[i-1]=getBinErrorDown(i)/getBinWidth(i);
      }
      else{
	y[i-1]=getBinContent(i); 
	yeh[i-1]=getBinErrorUp(i); 
	yel[i-1]=getBinErrorDown(i);
      }
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

  container1D container1D::operator + (const container1D & second){

      top::container1D out=second;
    if(bins_ != second.bins_){
      if(showwarnings_) std::cout << "operator +: not same binning!" << std::endl;
    }

    else{
      for(unsigned int i=0; i<content_.size(); i++){
	out.content_[i] += content_[i];
	out.entries_[i] += entries_[i];
	out.staterrup_[i] = sqrt(pow(second.staterrup_[i],2) + pow(staterrup_[i],2));
	out.staterrdown_[i] = sqrt(pow(second.staterrdown_[i],2) + pow(staterrdown_[i],2));
      }
      //systematics
      for(unsigned int firstsys=0;firstsys<syserrors_.size();firstsys++){
	bool foundsame=false;
	for(unsigned int secsys=0;secsys<second.syserrors_.size();secsys++){
	  if(syserrors_[firstsys].first == second.syserrors_[secsys].first){
	    for(unsigned int bin=0;bin<content_.size();bin++){
	      out.syserrors_[secsys].second[bin] += syserrors_[firstsys].second[bin];
	    }
	    foundsame=true;
	    break;
	  }
	}
	if(!foundsame){
	  out.syserrors_.push_back(syserrors_[firstsys]);
	}
      }
    }	
    
    return out;
  }

  container1D container1D::operator - (const container1D & second){
    container1D out=second;
    out = *this + (out * (-1));
    return out;
  }

  container1D container1D::operator / (const container1D & denominator){  
    container1D out= denominator;
    if(bins_ != denominator.bins_ || (divideBinomial_!=denominator.divideBinomial_)){
      if(showwarnings_) std::cout << "container1D::operator /: not same binning or different divide options!" << std::endl;
    }
    else{
      //deal with the systematics
      std::vector<unsigned int> useddenomsyst;

      for(unsigned int firstsys=0;firstsys<syserrors_.size();firstsys++){
	bool foundsame=false;
	for(unsigned int secsys=0;secsys<denominator.syserrors_.size();secsys++){
	  if(syserrors_[firstsys].first == denominator.syserrors_[secsys].first){ // systematic in both
	    for(unsigned int bin=0;bin<content_.size();bin++){
	      if(denominator.content_[bin]!=0){
		out.syserrors_[secsys].second[bin] = (syserrors_[firstsys].second[bin] + content_[bin]) / (denominator.syserrors_[firstsys].second[bin] + denominator.content_[bin]) - (content_[bin] / denominator.content_[bin]);
	      }
	      else{
		out.syserrors_[secsys].second[bin] = 0;
	      }
	    }
	    useddenomsyst.push_back(secsys);
	    foundsame=true;
	    break;
	  }
	}
	if(!foundsame){ //systematic is only in nominator
	  out.syserrors_.push_back(syserrors_[firstsys]);
	  for(unsigned int bin=0;bin<content_.size();bin++){
	    unsigned int lastentry=out.syserrors_.size() -1;
	      if(denominator.content_[bin]!=0){
		out.syserrors_[lastentry].second[bin] =  (syserrors_[firstsys].second[bin] + content_[bin]) / denominator.content_[bin] - (content_[bin] / denominator.content_[bin]);
	      }
	      else{
		out.syserrors_[lastentry].second[bin] = 0;
	      }
	  }
	}	
      }
      if(useddenomsyst.size() !=denominator.syserrors_.size()){
	for(unsigned int remsec=0;remsec<denominator.syserrors_.size();remsec++){
	  if(isIn(remsec,useddenomsyst)<0){ 
	    for(unsigned int bin=0;bin<content_.size();bin++){
	      if(denominator.content_[bin]!=0){ // systematics only in denominator
		out.syserrors_[remsec].second[bin] =  content_[bin] / (denominator.content_[bin] + denominator.syserrors_[remsec].second[bin]) - (content_[bin] / denominator.content_[bin]);
	      }
	      else{
		out.syserrors_[remsec].second[bin] = 0;
	      }
	    }
	  }
	}
      }
      // content and satistics:
      for(unsigned int i=0; i<content_.size(); i++){
	if(denominator.content_[i]!=0){
	  double content=content_[i] / denominator.content_[i];
	  double errup, errdown;
	  if(divideBinomial_){
	    errup=sqrt(content*(1-content)/denominator.content_[i]);
	    errdown=errup;
	  }
	  else{ 
	    errup=sqrt(pow(content_[i] / (denominator.content_[i] - denominator.staterrdown_[i]) - content,2) + pow((content_[i]+staterrup_[i]) / denominator.content_[i] -content,2) );
	    errdown=sqrt(pow(content_[i] / (denominator.content_[i] + denominator.staterrup_[i]) - content,2) + pow((content_[i]+staterrdown_[i]) / denominator.content_[i] -content,2) );
	  }
	  out.content_[i]=content;
	  out.staterrup_[i]=errup;
	  out.staterrdown_[i]=errdown;
	  out.entries_[i]=denominator.entries_[i];	  
	}
	else{
	  out.content_[i]=0;
	  out.staterrup_[i]=0;
	  out.staterrdown_[i]=0;
	  if(i!=0 && i != content_.size()-1){ 
	    if(showwarnings_)  std::cout << "container1D::operator /: warning: bin with denominator = 0 which is not underflow or overflow!" << std::endl;
	  }
	}
      }
    }
    return out;
  }
  container1D container1D::operator * (const container1D & multiplier){ 
    container1D out = multiplier;
    if(bins_ != multiplier.bins_){
      if(showwarnings_) std::cout << "container1D::operator *: not same binning!" << std::endl;
    }
    else{
      //systematics
      std::vector<unsigned int> usedmultisyst;
      for(unsigned int firstsys=0;firstsys<syserrors_.size();firstsys++){
	bool foundsame=false;
	for(unsigned int secsys=0;secsys<multiplier.syserrors_.size();secsys++){
	  if(syserrors_[firstsys].first == multiplier.syserrors_[secsys].first){ // systematic in both
	    for(unsigned int bin=0;bin<content_.size();bin++){
	      out.syserrors_[secsys].second[bin] = (multiplier.syserrors_[secsys].second[bin] + multiplier.content_[bin]) * (syserrors_[firstsys].second[bin] + content_[bin]) - (multiplier.content_[bin] * content_[bin]);
	    
	    }
	    usedmultisyst.push_back(secsys);
	    foundsame=true;
	    break;
	  }
	}
	if(!foundsame){ //systematic is only in first
	  out.syserrors_.push_back(syserrors_[firstsys]);
	  for(unsigned int bin=0;bin<content_.size();bin++){
	    unsigned int lastentry=out.syserrors_.size() -1;
	    out.syserrors_[lastentry].second[bin] =  (multiplier.content_[bin]) * (syserrors_[firstsys].second[bin] + content_[bin]) - (multiplier.content_[bin] * content_[bin]);
	  }
	}	
      }
      if(usedmultisyst.size() !=multiplier.syserrors_.size()){
	for(unsigned int remsec=0;remsec<multiplier.syserrors_.size();remsec++){
	  if(isIn(remsec,usedmultisyst)<0){  // only in multplier
	    for(unsigned int bin=0;bin<content_.size();bin++){
	      out.syserrors_[remsec].second[bin] =  (multiplier.syserrors_[remsec].second[bin] + multiplier.content_[bin]) * content_[bin]  - (multiplier.content_[bin] * content_[bin]);
	    }
	  }
	}
      }
      //statistics and content
      for(unsigned int i=0; i<content_.size(); i++){
	out.content_[i] *= content_[i];
	out.staterrup_[i] = sqrt(pow(staterrup_[i]*multiplier.content_[i],2) + pow(content_[i]*multiplier.staterrup_[i],2));
	out.staterrdown_[i] = sqrt(pow(staterrdown_[i]*multiplier.content_[i],2) + pow(content_[i]*multiplier.staterrdown_[i],2));
	out.entries_[i]+=entries_[i];
      }
    }
    return out;
  }
  container1D container1D::operator * (double scalar){
    container1D out= * this;
      for(unsigned int i=0; i<content_.size(); i++){
	out.content_[i] = content_[i] * scalar;
	out.staterrup_[i]   = staterrup_[i] * scalar;
	out.staterrdown_[i] = staterrdown_[i] * scalar;
	out.entries_[i] = entries_[i];
	for(unsigned int j=0;j<syserrors_.size();j++){
	  out.syserrors_[j].second[i] = syserrors_[j].second[i] * scalar;
	}
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
  
  void container1D::addErrorContainer(TString sysname,container1D deviatingContainer, double weight, bool ignoreMCStat){
    if(bins_!=deviatingContainer.bins_){
      std::cout << "container1D::addErrorContainer(): not same binning!" << std::endl;
    }
    else{
      if(! (sysname.Contains("_up") || sysname.Contains("_down"))){
	std::cout << "container1D::addErrorContainer: systematic variation must be named \".._up\"  or \".._down\"! for consistent treatment." << std::endl;
      }
      bool unambigous=true;
      for(unsigned int i=0;i<syserrors_.size();i++){
	if(syserrors_[i].first == sysname){
	  std::cout << "container1D::addErrorContainer: same named systematics (" << syserrors_[i].first << ") not allowed! ignoring input" << std::endl;
	  unambigous=false;
	  break;
	}
      }
      std::vector<double> devvec;
      if(unambigous){
	for(unsigned int i=0; i<content_.size(); i++){
	  double deviation;
	  deviation= weight * (deviatingContainer.content_[i] - content_[i]);
	  if(!ignoreMCStat){
	    if(((deviation > 0 && deviatingContainer.getBinErrorDown(i) > deviation)          //overlap
		|| (deviation < 0 && deviatingContainer.getBinErrorUp(i) > fabs(deviation)))  //overlap 
	       && i>0 && i<content_.size()-1){ 
	      std::cout << "container1D::addErrorContainer: Uncertainty of error container \""
			<< name_ <<"\" exceeds deviation(*weight) in bin " 
			<< xname_ << ": "<< bins_[i] << " - " << bins_[i+1] << std::endl;
	    }
	    if(deviation < 0) deviation = - sqrt(pow(deviation,2) + pow(deviatingContainer.getBinErrorDown(i),2));
	    else sqrt(pow(deviation,2) + pow(weight * deviatingContainer.getBinErrorUp(i),2));
	  }
	  devvec.push_back(deviation);
	}
      std::pair<TString, std::vector<double> > newsys(sysname, devvec);
      syserrors_.push_back(newsys);
      }
    }
  }
  void container1D::addErrorContainer(TString sysname,container1D deviatingContainer, bool ignoreMCStat){
    addErrorContainer(sysname,deviatingContainer,1,ignoreMCStat);
  }
  void container1D::addSystematicsFrom(const top::container1D & container){
    if(bins_!=container.bins_){
      std::cout << "container1D::addErrorContainer(): not same binning!" << std::endl;
    }
    else{
      for(std::vector<std::pair<TString, std::vector<double> > >::const_iterator syst=container.syserrors_.begin(); syst<container.syserrors_.end();++syst){
	bool unamb=true;
	for(unsigned int i=0;i<syserrors_.size();i++){
	  if(syserrors_[i].first == syst->first){
	    unamb=false;
	    break;
	  }
	}
	if(unamb){
	  syserrors_.push_back(*syst);
	}
	else{
	  if(showwarnings_) std::cout << "container1D::addSystematicsFrom(): systematic uncertainty with name " << syst->first << " already included!" << std::endl;
	}
      }
    }
  }
  void container1D::addGlobalRelErrorUp(TString sysname,double relerr){
    addErrorContainer(sysname+"_up", ((*this) * (relerr+1)));
  }
  void container1D::addGlobalRelErrorDown(TString sysname,double relerr){
    addErrorContainer(sysname+"_down", ((*this) * (1-relerr)));   
  }
  void container1D::addGlobalRelError(TString sysname,double relerr){
    addGlobalRelErrorUp(sysname,relerr);
    addGlobalRelErrorDown(sysname,relerr);
  }
  void container1D::removeError(TString sysname){
    for(unsigned int i=0;i<syserrors_.size();i++){
      if(sysname == syserrors_[i].first) syserrors_.erase(syserrors_.begin()+i);
    }
  }

  //protected

  TString container1D::stripVariation(TString in){
    in.Resize(in.Last('_'));
    return in;
  }

  double container1D::getDominantVariationUp(TString sysname, int bin){
    double max=0;
    for(unsigned int i=0;i<syserrors_.size();i++){
      if(syserrors_[i].first.Contains(sysname) && max < syserrors_[i].second[bin]) max=syserrors_[i].second[bin];
    }
    return max;
  }
  double container1D::getDominantVariationDown(TString sysname, int bin){
    double min=0;
    for(unsigned int i=0;i<syserrors_.size();i++){
      if(syserrors_[i].first.Contains(sysname) && min > syserrors_[i].second[bin]) min=syserrors_[i].second[bin];
    }
    return min;
  }




} //namespace

#endif
