#include "../interface/container.h"


  //some more operators
  ztop::container1D operator * (double multiplier, const ztop::container1D & cont){ //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
    ztop::container1D out=cont;
    return out * multiplier;
  }
  ztop::container1D operator * (float multiplier, const ztop::container1D & cont){  //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
    ztop::container1D out=cont;
    return out * multiplier;
  }
  ztop::container1D operator * (int multiplier, const ztop::container1D & cont){    //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
    ztop::container1D out=cont;
    return out * multiplier;
  }



namespace ztop{

  //////////if you like cut here ;) 
  std::vector<container1D*> container1D::c_list;
  bool container1D::c_makelist=false;
  
  ///////function definitions
  container1D::container1D(){
    canfilldyn_=false;
    divideBinomial_=true;
    labelmultiplier_=1;
    showwarnings_=true;
    mergeufof_=true;
    wasunderflow_=false;
    wasoverflow_=false;
    if(c_makelist)c_list.push_back(this);
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
    if(c_makelist)c_list.push_back(this);
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
    if(c_makelist)c_list.push_back(this);
    mergeufof_=mergeufof;
    wasunderflow_=false;
    wasoverflow_=false;
  }
  container1D::~container1D(){
    for(unsigned int i=0;i<c_list.size();i++){
      if(c_list[i] == this) c_list.erase(c_list.begin()+i);
      break;
    }
  }

  void container1D::setBins(std::vector<float> bins){
    reset();
    float lastbin;
    for(size_t i=0;i<bins.size();i++){
      if(i<1){
	lastbin=bins.at(i);}
      else if(lastbin >= bins.at(i)){
	std::cout << "container1D::setBins: bins must be in increasing order!" << std::endl;
	std::exit(EXIT_FAILURE);
      }
    }

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
      fullerr2=sq(staterrup_[bin]); //stat
      if(limittosys==""){
	// make vector of all sys stripped
	std::vector<TString> sources;
	for(unsigned int i=0;i<syserrors_.size();i++){
	  TString source=stripVariation(syserrors_[i].first);
	  if(-1==isIn(source,sources)){
	    sources.push_back(source);
	    fullerr2 += sq(getDominantVariationUp(source,bin));
	  }
	}
      }
      else{
	fullerr2 += sq(getDominantVariationUp(limittosys,bin));
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
      fullerr2=sq(staterrdown_[bin]); //stat
      if(limittosys==""){
	// make vector of all sys stripped
	std::vector<TString> sources;
	for(unsigned int i=0;i<syserrors_.size();i++){
	  TString source=stripVariation(syserrors_[i].first);
	  if(-1==isIn(source,sources)){
	    sources.push_back(source);
	    fullerr2 += sq(getDominantVariationDown(source,bin));
	  }
	}
      }
      else{
	fullerr2 += sq(getDominantVariationUp(limittosys,bin));
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
    if(bins_.size()>0){
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
    else return 0;
  }
  double container1D::cIntegral(float from, float to){
    //select bins
    unsigned int minbin=getBinNo(from);
    unsigned int maxbin=getBinNo(to);

    double integr=0;
    for(unsigned int i=minbin;i<=maxbin;i++){
      integr+=content_.at(i);
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

  void container1D::writeTH1D(TString name, bool dividebybinwidth){
    TH1D * h = getTH1D(name,dividebybinwidth);
    h->Draw();
    h->Write();
    delete h;
  }

  TGraphAsymmErrors * container1D::getTGraph(TString name, bool dividebybinwidth, bool noXErrors){

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
  void container1D::writeTGraph(TString name, bool dividebybinwidth, bool noXErrors){
    TGraphAsymmErrors * g=getTGraph(name,dividebybinwidth);
    g->SetName(name);
    g->Draw("AP");
    g->Write();
    delete g;
  }

  void container1D::setDivideBinomial(bool divideBinomial){
    divideBinomial_=divideBinomial;
  }

  container1D container1D::operator + (const container1D & second){

      ztop::container1D out=second;
    if(bins_ != second.bins_){
      if(showwarnings_) std::cout << "operator +: not same binning for " << name_ << std::endl;
    }

    else{
      for(unsigned int i=0; i<content_.size(); i++){
	out.content_[i] += content_[i];
	out.entries_[i] += entries_[i];
	out.staterrup_[i] = sqrt(sq(second.staterrup_[i]) + sq(staterrup_[i]));
	out.staterrdown_[i] = sqrt(sq(second.staterrdown_[i]) + sq(staterrdown_[i]));
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
	    if(content <=1){
	      errup=sqrt(content*(1-content)/denominator.content_[i]);
	    }
	    else{
	      errup=0;
	      std::cout << "container1D::operator /: binomial division switched on, but content >1!" <<std::endl;
	    }
	    errdown=errup;
	  }
	  else{ 
	    errup=sqrt(sq(content_[i] / (denominator.content_[i] - denominator.staterrdown_[i]) - content) + sq((content_[i]+staterrup_[i]) / denominator.content_[i] -content) );
	    errdown=sqrt(sq(content_[i] / (denominator.content_[i] + denominator.staterrup_[i]) - content) + sq((content_[i]+staterrdown_[i]) / denominator.content_[i] -content) );
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
	out.staterrup_[i] = sqrt(sq(staterrup_[i]*multiplier.content_[i]) + sq(content_[i]*multiplier.staterrup_[i]));
	out.staterrdown_[i] = sqrt(sq(staterrdown_[i]*multiplier.content_[i]) + sq(content_[i]*multiplier.staterrdown_[i]));
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
	    if(deviation < 0) deviation = - sqrt(sq(deviation) + sq(weight * deviatingContainer.getBinErrorDown(i)));
	    else deviation = sqrt(sq(deviation) + sq(weight * deviatingContainer.getBinErrorUp(i)));
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
  void container1D::addRelSystematicsFrom(const ztop::container1D & container){
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
	  if(showwarnings_) std::cout << "container1D::addRelSystematicsFrom(): systematic uncertainty with name " << syst->first << " already included!" << std::endl;
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

  void container1D::transformStatToSyst(TString sysname){

    std::pair<TString, std::vector<double> > up(sysname+"_up", staterrup_);
    std::vector<double> statdownwithsign;
    for(unsigned int i=0;i<staterrdown_.size();i++) statdownwithsign.push_back(-staterrdown_.at(i));
    std::pair<TString, std::vector<double> > down(sysname+"_down", statdownwithsign);
    syserrors_.push_back(up);
    syserrors_.push_back(down);
    for(unsigned int i=0;i<staterrup_.size();i++){
      staterrup_[i]=0;
      staterrdown_[i]=0;
    }
  }

  void container1D::renameSyst(TString old, TString New){
    int found=0;
    for(unsigned int i=0;i<syserrors_.size();i++){
      if(old+"_up" == syserrors_[i].first){
	syserrors_[i].first = New +"_up";
	found++;
      }
      else if(old+"_down" == syserrors_[i].first){
	syserrors_[i].first = New +"_down";
	found++;
      }
      if(found == 2) break;
    }
    if(found !=2) std::cout << "container1D::renameSyst: Entry " << old << " not found!" << std::endl;
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

}
