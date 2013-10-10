#include "../interface/container.h"
#include <algorithm>
#include <parallel/algorithm>
#include <omp.h>

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
	//divideBinomial_=true;
	manualerror_=false;
	labelmultiplier_=1;
	showwarnings_=true;
	mergeufof_=true;
	wasunderflow_=false;
	wasoverflow_=false;
	gp_=0;
	hp_=0;
	if(c_makelist)c_list.push_back(this);
}
container1D::container1D(float binwidth, TString name,TString xaxisname,TString yaxisname, bool mergeufof){ //currently not used
	binwidth_=binwidth;
	canfilldyn_=true;
	//divideBinomial_=true;
	manualerror_=false;
	name_=name;
	xname_=xaxisname;
	yname_=yaxisname;
	labelmultiplier_=1;
	showwarnings_=true;
	if(c_makelist)c_list.push_back(this);
	mergeufof_=mergeufof;
	wasunderflow_=false;
	wasoverflow_=false;
	gp_=0;
	hp_=0;
}
container1D::container1D(std::vector<float> bins, TString name,TString xaxisname,TString yaxisname, bool mergeufof){
	setBins(bins);
	//divideBinomial_=true;
	manualerror_=false;
	name_=name;
	xname_=xaxisname;
	yname_=yaxisname;
	labelmultiplier_=1;
	showwarnings_=true;
	if(c_makelist)c_list.push_back(this);
	mergeufof_=mergeufof;
	wasunderflow_=false;
	wasoverflow_=false;
	gp_=0;
	hp_=0;
}
container1D::~container1D(){
	for(unsigned int i=0;i<c_list.size();i++){
		if(c_list[i] == this) c_list.erase(c_list.begin()+i);
		break;
	}
	if(gp_) delete gp_;
	if(hp_) delete hp_;
}

void container1D::setBins(std::vector<float> bins){
	reset();
	//NEW: just sort bins. no error message anymore
	std::sort(bins.begin(),bins.end());
	bins_=bins;
	bins_.insert(bins_.begin(),0); //underflow
	//overflow is the last one
	canfilldyn_=false;
	contents_ = histoContent(bins_.size());
	manualerror_=false;
}
void container1D::setBinWidth(float binwidth){
	binwidth_=binwidth;
	canfilldyn_=true;
}


/**
 * deletes all systematic entries and creates a new systematics! layer
 * named manually_set_(up/down)
 */
void container1D::setBinErrorUp(const size_t& bin, const double &err){
	if(!manualerror_){
		createManualError();}
	if(bin>=bins_.size()){
		std::cout << "container1D::setBinErrorUp: bin not existent, doing nothing" << std::endl;
		return;
	}
	contents_.getBin(bin,0).setContent(contents_.getBin(bin).getContent()+err);
}
/**
 * deletes all systematic entries and creates a new systematics! layer
 * sets the deviation, not the value of of variation
 * named manually_set_(up/down)
 * for a downward vairiation input a POSITIVE value!
 */
void container1D::setBinErrorDown(const size_t &bin, const double &err){
	if(!manualerror_){
		createManualError();}
	if(bin>=bins_.size()){
		std::cout << "container1D::setBinErrorDown: bin not existent, doing nothing" << std::endl;
		return;
	}
	contents_.getBin(bin,1).setContent(contents_.getBin(bin).getContent()-err);
}
/**
 * deletes all systematic entries and creates a new systematics! layer
 * named manually_set_(up/down)
 */
void container1D::setBinError(const size_t &bin, const double & err){
	if(!manualerror_){
		createManualError();}
	if(bin>=bins_.size()){
		std::cout << "container1D::setBinError: bin not existent, doing nothing" << std::endl;
		return;
	}
	contents_.getBin(bin,0).setContent(contents_.getBin(bin).getContent()+err);
	contents_.getBin(bin,1).setContent(contents_.getBin(bin).getContent()-err);
}
void container1D::setBinStat(const size_t &bin, const double & err, const int &sysLayer){
	if(bin<bins_.size() && (sysLayer<0 ||(size_t)sysLayer < contents_.layerSize() )){
		contents_.getBin(bin,sysLayer).setStat(err);
	}
	else{
		std::cout << "container1D::setBinStat: bin not existent!" << std::endl;
	}
}
void container1D::setBinEntries(const size_t &bin, const size_t & entries, const int &sysLayer){
	if(bin<bins_.size() && (sysLayer<0 ||(size_t)sysLayer < contents_.layerSize() )){
		contents_.getBin(bin,sysLayer).setEntries(entries);
	}
	else{
		std::cout << "container1D::setBinEntries: bin not existent!" << std::endl;
	}
}
void container1D::setBinContent(const size_t & bin, const double &content, const int &sysLayer){
	if(bin<bins_.size()&& (sysLayer<0 ||(size_t)sysLayer < contents_.layerSize() )){
		contents_.getBin(bin,sysLayer).setContent(content);
	}
	else{
		std::cout << "container1D::setBinContent: bin not existent!" << std::endl;
	}
}


size_t container1D::getNBins() const{
	return bins_.size()-2;
}
double container1D::getBinCenter(const size_t &bin) const{
	double center=0;
	if((bin>=bins_.size()) && showwarnings_){
		std::cout << "container1D::getBinCenter: ("<< name_ <<") bin not existent!" << std::endl;
		return 0;
	}
	else if(bin==bins_.size()-1){ // overflow
		center = 0;
	}
	else if(bin==0){
		center = 0;
	}
	else {
		center = (bins_[bin]+bins_[bin+1])/2;
	}
	return center;
}
double container1D::getBinWidth(const size_t &bin) const{
	double width=0;
	if(!(bin<bins_.size()-1) && showwarnings_){
		std::cout << "container1D::getBinWidth: ("<< name_ <<") bin not existent!" << std::endl;
	}
	else{
		width=fabs(bins_[bin+1]-bins_[bin]);
	}
	return width;
}

const double & container1D::getBinContent(const size_t &bin,const int &sysLayer) const{
	if(bin<bins_.size()&& (sysLayer<0 ||(size_t)sysLayer < contents_.layerSize())){
		return contents_.getBin(bin,sysLayer).getContent();
	}
	else{
		if(showwarnings_)std::cout << "container1D::getBinContent: ("<< name_ <<") bin not existent!" << std::endl;
		return contents_.getBin(0).getContent();
	}
}
const size_t & container1D::getBinEntries(const size_t& bin,const int &sysLayer) const{
	if(bin>=bins_.size() || sysLayer>=(long)contents_.layerSize()){
		std::cout << "container1D::getBinEntries: bin out of range, returning bin number" << std::endl;
		return bin;
	}
	return contents_.getBin(bin,sysLayer).getEntries();
}
double container1D::getBinStat(const size_t& bin,const int &sysLayer) const{
	if(bin>=bins_.size() || sysLayer>=(long)contents_.layerSize()){
		std::cout << "container1D::getBinStat: bin out of range, returning stat2 of underflow" << std::endl;
		return contents_.getBin(0).getStat2();
	}
	return contents_.getBin(bin,sysLayer).getStat();
}

/**
 * returns the bin error variation up. If onlystat=false, each systematic uncertainty will be added in squares to
 * the stat. error. limittosys identifies a systematic source the output should be limited to.
 * no effect if onlystat=true
 * statistics of systematics are not taken into account here
 * always returns a POSITIVE value
 */
double container1D::getBinErrorUp(const size_t & bin, bool onlystat,const TString &limittosys)const {
	double fullerr2=0;
	if(bin<bins_.size()){
		fullerr2=contents_.getBin(bin).getStat2(); //stat
		if(onlystat)
			return sqrt(fullerr2);
		if(limittosys==""){
			// make vector of all sys stripped
			std::vector<TString> sources;
			for(size_t i=0;i<contents_.layerSize();i++){ //there might be room for improvement here...
				if(debug){
					std::cout << "container1D::getBinErrorUp: checking variation with index: " <<i<<std::endl;
					std::cout << "container1D::getBinErrorUp: stripping variation with name: " <<contents_.getLayerName(i)<<std::endl;
				}
				TString source=stripVariation(contents_.getLayerName(i));
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
/**
 * returns the bin error variation down. If onlystat=false, each systematic uncertainty will be added in squares to
 * the stat. error. limittosys identifies a systematic source the output should be limited to.
 * no effect if onlystat=true
 * always returns a POSITIVE value
 */
double container1D::getBinErrorDown(const size_t & bin,bool onlystat,const TString & limittosys) const{
	double fullerr2=0;
	if(bin<bins_.size()){
		fullerr2=contents_.getBin(bin).getStat2(); //stat
		if(onlystat)
			return sqrt(fullerr2);
		if(limittosys==""){
			// make vector of all sys stripped
			std::vector<TString> sources;
			for(size_t i=0;i<contents_.layerSize();i++){ //there might be room for improvement here...
				if(debug){
					std::cout << "container1D::getBinErrorDown: checking variation with index: " <<i<<std::endl;
					std::cout << "container1D::getBinErrorDown: stripping variation with name: " <<contents_.getLayerName(i)<<std::endl;
				}
				TString source=stripVariation(contents_.getLayerName(i));
				if(-1==isIn(source,sources)){
					sources.push_back(source);
					fullerr2 += sq(getDominantVariationDown(source,bin));
				}
			}
		}
		else{
			fullerr2 += sq(getDominantVariationDown(limittosys,bin));
		}
		return sqrt(fullerr2);
	}
	else{
		if(showwarnings_)std::cout << "container1D::getBinErrorDown: bin not existent!" << std::endl;
		return 0;
	}
}
/**
 * returns the symmetrized (max(down,up)) bin error variation. If onlystat=false, each systematic uncertainty will be added in squares to
 * the stat. error. limittosys identifies a systematic source the output should be limited to.
 * no effect if onlystat=true
 */
double container1D::getBinError(const size_t & bin,bool onlystat,const TString & limittosys) const{
	double symmerror=0;
	if(getBinErrorUp(bin,onlystat,limittosys) > fabs(getBinErrorDown(bin,onlystat,limittosys))) symmerror=getBinErrorUp(bin,onlystat,limittosys);
	else symmerror=fabs(getBinErrorDown(bin,onlystat,limittosys));
	return symmerror;
}
/**
 * container1D::getSystError(unsigned int number, int bin)
 * returns deviation from nominal for systematic error with <number>
 */
double container1D::getSystError(const size_t& number, const size_t&  bin) const{

	if(number>=contents_.layerSize()){
		std::cout << "container1D::getSystError: " << number << " out of range(" << contents_.layerSize() << "-1)" << std::endl;
		return 0;
	}
	if(bin>=bins_.size()){
		std::cout << "container1D::getSystError: " << bin << " out of range(" << bins_.size() << ") (incl UF/OF)" << std::endl;
		return 0;
	}
	double err=contents_.getBin(bin,number).getContent()-contents_.getBin(bin).getContent();
	return err;
}
double container1D::getSystErrorStat(const size_t& number, const size_t&  bin) const{

	if(number>=contents_.layerSize()){
		std::cout << "container1D::getSystError: " << number << " out of range(" << contents_.layerSize() << "-1)" << std::endl;
		return 0;
	}
	if(bin>=bins_.size()){
		std::cout << "container1D::getSystError: " << bin << " out of range(" << bins_.size() << ") (incl UF/OF)" << std::endl;
		return 0;
	}
	return contents_.getBin(bin,number).getStat();
}
const TString &container1D::getSystErrorName(const size_t &number) const{
	if(number>=contents_.layerSize()){
		std::cout << "container1D::getSystErrorName: " << number << " out of range(" << (int)getSystSize()-1 << ")" << std::endl;
		return name_;
	}
	return contents_.getLayerName(number);
}
const size_t & container1D::getSystErrorIndex(const TString & name) const{
	return contents_.getLayerIndex(name);
}
/**
 * container1D breakDownSyst(const size_t & bin, const TString & constrainTo="")
 * constrain to "_up" or "_down"
 */
container1D container1D::breakDownSyst(const size_t & bin, bool updown) const{
	std::vector<float> bins;
	bins.push_back(-0.5);
	TString nameadd="_up";
	if(!updown) nameadd="_down";
	container1D out(bins,name_+"_syst_"+toTString(bin)+nameadd,"","#Delta [%]");
	if(bin>= bins_.size()){
		std::cout << "container1D::breakDownSyst: bin out of range, return empty" << std::endl;
		return out;
	}
	std::vector<double> errs;
	std::vector<double> stats;
	std::vector<TString> names;
	float cbin=bins.at(0);
	double nominal=getBinContent(bin);
	for(size_t i=0;i<getSystSize();i++){
		const TString & name=getSystErrorName(i);
		if(updown && name.EndsWith("_down")){
			continue;
		}
		else if(!updown && name.EndsWith("_up")){
			continue;
		}
		cbin++;
		bins.push_back(cbin);
		TString sname=name;
		sname.ReplaceAll("_up","");
		sname.ReplaceAll("_down","");
		errs.push_back(getSystError(i,bin)/nominal * 100);
		stats.push_back(getBinStat(bin,i)/getSystError(i,bin)/nominal *100);
		names.push_back(sname);
	}

	out.setBins(bins);
	for(size_t i=0;i<errs.size();i++){ //no OF or UF
		out.setBinContent(i+1,errs.at(i));
		out.setBinStat(i+1,stats.at(i));
		out.getBin(i+1).setName(names.at(i));
	}
	return out;
}
/**
 * make sure there are no bins with the same name!
 */
container1D container1D::sortByBinNames(const container1D & reference) const{
	if(reference.bins_.size() != bins_.size()){
		std::cout << "container1D::sortByBinNames: binning has to be the same return empty container" << std::endl;
		return container1D();
	}
	std::vector<size_t> asso;
	asso.resize(bins_.size(),0); //only for same indicies
	for(size_t i=1;i<bins_.size()-1;i++){
		for(size_t j=1;j<reference.bins_.size()-1;j++){
			if(reference.getBin(j).getName() == getBin(i).getName()){
				asso.at(i)=j;
			}
		}
	}
	container1D out = *this;
	for(int sys=-1;sys<(int)getSystSize();sys++){
		for(size_t i=0;i<out.bins_.size();i++){
			out.getBin(asso.at(i),sys)=getBin(i,sys);
		}
	}
	return out;
}

double container1D::getOverflow(const int& systLayer) const{
	double ret=0;
	if(systLayer > (int)contents_.layerSize())
		return 0;
	if(contents_.size()>0){
		ret= contents_.getBin(contents_.size()-1,systLayer).getContent();
	}
	else{
		ret= 0;
	}
	if(wasoverflow_) ret=-1.;
	return ret;
}
double container1D::getUnderflow(const int& systLayer) const{
	double ret=0;
	if(systLayer > (int)contents_.layerSize())
		return 0;
	if(contents_.size()>0){
		ret= contents_.getBin(0,systLayer).getContent();
	}
	else{
		ret= 0;
	}
	if(wasunderflow_) ret=-1.;
	return ret;
}
/**
 * get the integral. default: nominal (systLayer=-1)
 */
double container1D::integral(bool includeUFOF,const int& systLayer) const{
	if(bins_.size()<1)
		return 0;
	size_t minbin,maxbin;
	if(systLayer >= (int)contents_.layerSize())
		return 0;
	if(includeUFOF){
		minbin=0;
		maxbin=bins_.size();
	}
	else{
		minbin=1;
		maxbin=bins_.size() -1;
	}
	double integr=0;
#pragma omp parallel for reduction(+:integr)
	for(unsigned int i=minbin;i<maxbin;i++){
		integr+=contents_.getBin(i,systLayer).getContent();
	}
	return integr;
}
/**
 * get the integral . default: nominal (systLayer=-1)
 */
double container1D::cIntegral(float from, float to,const int& systLayer) const{
	//select bins
	unsigned int minbin=getBinNo(from);
	unsigned int maxbin=getBinNo(to);
	if(systLayer >= (int)contents_.layerSize())
		return 0;
	double integr=0;
#pragma omp parallel for reduction(+:integr)
	for(size_t i=minbin;i<=maxbin;i++){
		integr+=contents_.getBin(i,systLayer).getContent();
	}
	return integr;
}
/**
 * get the integral. default: nominal (systLayer=-1)
 * assumes uncorrelated Stat inbetween bins
 */
double container1D::integralStat(bool includeUFOF,const int& systLayer) const{
	if(bins_.size()<1)
		return 0;
	size_t minbin,maxbin;
	if(systLayer >= (int)contents_.layerSize())
		return 0;
	if(includeUFOF){
		minbin=0;
		maxbin=bins_.size();
	}
	else{
		minbin=1;
		maxbin=bins_.size() -1;
	}
	double integr2=0;
#pragma omp parallel for reduction(+:integr2)
	for(unsigned int i=minbin;i<maxbin;i++){
		integr2+=contents_.getBin(i,systLayer).getStat2();
	}
	return sqrt(integr2);
}
/**
 * get the integral. default: nominal (systLayer=-1)
 * assumes uncorrelated Stat inbetween bins
 */
double container1D::cIntegralStat(float from, float to,const int& systLayer) const{
	//select bins
	unsigned int minbin=getBinNo(from);
	unsigned int maxbin=getBinNo(to);
	if(systLayer >= (int)contents_.layerSize())
		return 0;
	double integr2=0;
#pragma omp parallel for reduction(+:integr2)
	for(size_t i=minbin;i<=maxbin;i++){
		integr2+=contents_.getBin(i,systLayer).getStat2();
	}
	return sqrt(integr2);
}
/**
 * get the integral. default: nominal (systLayer=-1)
 * assumes uncorrelated Stat inbetween bins
 */
size_t container1D::integralEntries(bool includeUFOF,const int& systLayer) const{
	if(bins_.size()<1)
		return 0;
	size_t minbin,maxbin;
	if(systLayer >= (int)contents_.layerSize())
		return 0;
	if(includeUFOF){
		minbin=0;
		maxbin=bins_.size();
	}
	else{
		minbin=1;
		maxbin=bins_.size() -1;
	}
	size_t integr=0;
#pragma omp parallel for reduction(+:integr)
	for(unsigned int i=minbin;i<maxbin;i++){
		integr+=contents_.getBin(i,systLayer).getEntries();
	}
	return integr;
}
/**
 * get the integral. default: nominal (systLayer=-1)
 * assumes uncorrelated Stat inbetween bins
 */
size_t container1D::cIntegralEntries(float from, float to,const int& systLayer) const{
	//select bins
	unsigned int minbin=getBinNo(from);
	unsigned int maxbin=getBinNo(to);
	if(systLayer >= (int)contents_.layerSize())
		return 0;
	size_t integr=0;
#pragma omp parallel for reduction(+:integr)
	for(size_t i=minbin;i<=maxbin;i++){
		integr+=contents_.getBin(i,systLayer).getEntries();
	}
	return integr;
}
/**
 * only for visible bins
 */
double container1D::getYMax(bool dividebybinwidth,const int& systLayer) const{
	double max=-999999999999.;
	for(size_t i=1;i<bins_.size()-1;i++){ //only in visible bins
		double binc=getBinContent(i,systLayer);
		if(dividebybinwidth) binc*= 1/getBinWidth(i);
		if(max < binc) max = binc;
	}
	return max;
}

/**
 * only for visible bins
 */
double container1D::getYMin(bool dividebybinwidth,const int& systLayer) const{
	double min=999999999999.;
	for(size_t i=1;i<bins_.size()-1;i++){ //only in visible bins
		double binc=getBinContent(i,systLayer);
		if(dividebybinwidth) binc*= 1/getBinWidth(i);
		if(min > binc) min = binc;
	}
	return min;
}

/**
 * normalizes container and returns normalization factor (to reverse normalization)
 */
double container1D::normalize(bool includeUFOF,const double& normto){
	double norm=normto/integral(includeUFOF);
	*this *= norm;
	return norm;
}


void container1D::reset(){
	binwidth_=0;
	bins_.clear();
	canfilldyn_=false;

	manualerror_=false;
	contents_=histoContent();
}
void container1D::clear(){
	contents_.clear();
	manualerror_=false;
}
/**
 * creates new TH1D and returns pointer to it
 *  getTH1D(TString name="", bool dividebybinwidth=true, bool onlystat=false)
 */
TH1D * container1D::getTH1D(TString name, bool dividebybinwidth, bool onlystat) const{
	if(name=="") name=name_;
	if(bins_.size() < 2)
		return 0;
	TH1D *  h = new TH1D(name,name,getNBins(),&(bins_.at(1)));
	double entriessum=0;
	for(size_t i=0;i<=getNBins()+1;i++){ // 0 underflow, genBins+1 overflow
		double cont=getBinContent(i);
		if(dividebybinwidth && i>0 && i<getNBins()+1) cont=cont/getBinWidth(i);
		h->SetBinContent(i,cont);
		double err=getBinError(i,onlystat);
		if(dividebybinwidth && i>0 && i<getNBins()+1) err=err/getBinWidth(i);
		h->SetBinError(i,err);
		entriessum +=contents_.getBin(i).getEntries();
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
/**
 * container1D::getTH1DSyst(TString name="", unsigned int systNo, bool dividebybinwidth=true, bool statErrors=false)
 * -name: name of output histogram
 * -systNo syst number
 *
 *
 * THIS IS TOTAL BULLSHIT!!!
 */
TH1D * container1D::getTH1DSyst(TString name, size_t systNo, bool dividebybinwidth, bool statErrors) const{
	TH1D * h=getTH1D(name,dividebybinwidth,true); //gets everything with only stat errors

	//now shift by systematic
	for(size_t i=0;i<=getNBins()+1;i++){ // 0 underflow, genBins+1 overflow
		double newcont=getBin(i,systNo).getContent();
		double newstat=getBin(i,systNo).getStat();
		if(dividebybinwidth){
			newcont=newcont/getBinWidth(i);
			newstat=newstat/getBinWidth(i);
		}
		h->SetBinContent((int)i,newcont);
		h->SetBinError((int)i,newstat);
		if(!statErrors)
			h->SetBinError(i,0);
	}
	return h;
}
/*
container1D & container1D::operator = (const TH1D & h){
	int nbins=h.GetNbinsX();
	std::vector<float> cbins;
	for(int bin=1;bin<=nbins+1;bin++)
		cbins.push_back(h.GetBinLowEdge(bin));

	container1D out(cbins,h.GetName(),h.GetXaxis()->GetTitle(),h.GetYaxis()->GetTitle(),false);
	*this=out;
	for(int bin=0;bin<=nbins+1;bin++){
		setBinContent(bin,h.GetBinContent(bin));
		setBinStat(bin,h.GetBinError(bin));
	}
	return *this;
}
*/
container1D & container1D::import(TH1 * h,bool isbinwidthdivided){
	int nbins=h->GetNbinsX();
	std::vector<float> cbins;
	for(int bin=1;bin<=nbins+1;bin++)
		cbins.push_back(h->GetBinLowEdge(bin));

	container1D out(cbins,h->GetName(),h->GetXaxis()->GetTitle(),h->GetYaxis()->GetTitle(),false);
	*this=out;
	for(int bin=0;bin<=nbins+1;bin++){
		double cont=h->GetBinContent(bin);
		if(isbinwidthdivided && bin>0 && bin<nbins)
			cont*=getBinWidth(bin);
		setBinContent(bin,cont);
		double err=h->GetBinError(bin);
		if(isbinwidthdivided && bin>0 && bin<nbins)
			err*=getBinWidth(bin);
		setBinStat(bin,err);
	}
	return *this;
}

void container1D::writeTH1D(TString name, bool dividebybinwidth,bool onlystat) const{
	TH1D * h = getTH1D(name,dividebybinwidth,onlystat);
	h->Draw();
	h->Write();
	delete h;
}

TGraphAsymmErrors * container1D::getTGraph(TString name, bool dividebybinwidth, bool onlystat, bool noXErrors) const{

	if(name=="") name=name_;
	double x[getNBins()];
	double xeh[getNBins()];
	double xel[getNBins()];
	double y[getNBins()];
	double yel[getNBins()];
	double yeh[getNBins()];
	for(size_t i=1;i<=getNBins();i++){
		x[i-1]=getBinCenter(i);
		float scale=1;
		if(dividebybinwidth)
			scale=1/getBinWidth(i);
		if(noXErrors){
			xeh[i-1]=0;
			xel[i-1]=0;
		}
		else{
			xeh[i-1]=getBinWidth(i)/2;
			xel[i-1]=getBinWidth(i)/2;
		}
		y[i-1]=getBinContent(i)*scale;
		yeh[i-1]=getBinErrorUp(i,onlystat)*scale;
		yel[i-1]=getBinErrorDown(i,onlystat)*scale;

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
TGraphAsymmErrors * container1D::getTGraphSwappedAxis(TString name, bool dividebybinwidth, bool onlystat, bool noXErrors) const{

	if(name=="") name=name_;
	double x[getNBins()];
	double xeh[getNBins()];
	double xel[getNBins()];
	double y[getNBins()];
	double yel[getNBins()];
	double yeh[getNBins()];
	for(size_t i=1;i<=getNBins();i++){
		x[i-1]=getBinCenter(i);
		float scale=1;
		if(dividebybinwidth)
			scale=1/getBinWidth(i);
		if(noXErrors){
			xeh[i-1]=0;
			xel[i-1]=0;
		}
		else{
			xeh[i-1]=getBinWidth(i)/2;
			xel[i-1]=getBinWidth(i)/2;
		}
		y[i-1]=getBinContent(i)*scale;
		yeh[i-1]=getBinErrorUp(i,onlystat)*scale;
		yel[i-1]=getBinErrorDown(i,onlystat)*scale;

	}
	TGraphAsymmErrors * g = new TGraphAsymmErrors(getNBins(),y,x,yel,yeh,xel,xeh);
	g->SetName(name);
	g->SetTitle(name);
	g->GetXaxis()->SetTitleSize(0.06*labelmultiplier_);
	g->GetXaxis()->SetLabelSize(0.05*labelmultiplier_);
	g->GetXaxis()->SetTitleOffset(g->GetYaxis()->GetTitleOffset() / labelmultiplier_);
	g->GetXaxis()->SetTitle(yname_);
	g->GetYaxis()->SetTitleSize(0.06*labelmultiplier_);
	g->GetYaxis()->SetLabelSize(0.05*labelmultiplier_);
	g->GetXaxis()->SetNdivisions(510);
	g->GetYaxis()->SetTitle(xname_);
	g->SetMarkerStyle(20);
	g->GetYaxis()->SetRangeUser(bins_[1], bins_[getNBins()+1]);

	return g;
}
void container1D::writeTGraph(TString name, bool dividebybinwidth, bool onlystat,bool noXErrors) const{
	TGraphAsymmErrors * g=getTGraph(name,dividebybinwidth,onlystat,noXErrors);
	g->SetName(name);
	g->Draw("AP");
	g->Write();
	delete g;
}
/**
 * memory objects will be destroyed when container is destructed!!!!
 */
void container1D::drawFullPlot(TString name, bool dividebybinwidth,const TString &extraoptions){
	hp_=getTH1D(name,dividebybinwidth,true);
	gp_=getTGraph(name,dividebybinwidth,false,false);
	hp_->Draw("e1,"+extraoptions);
	gp_->Draw("P,same,e1"+extraoptions);
	hp_->Draw("e1,same"+extraoptions);
}

void container1D::setDivideBinomial(bool divideBinomial){
	divideBinomial_=divideBinomial;
	std::cout << "container1D::setDivideBinomial not needed anymore!" << name_ << std::endl;
}

///////////////OPERATORS//////////////

container1D & container1D::operator += (const container1D & second){
	if(bins_ != second.bins_){
		if(showwarnings_) std::cout << "container1D::operator +=: not same binning for " << name_ << " return this" << std::endl;
		return *this;
	}
	contents_ += second.contents_;
	return *this;
}


container1D container1D::operator + (const container1D & second){
	ztop::container1D out=*this;
	out += second;
	return out;
}

container1D & container1D::operator -= (const container1D & second){
	if(bins_ != second.bins_){
		if(showwarnings_) std::cout << "container1D::operator +=: not same binning for " << name_ << " return this" << std::endl;
		return *this;
	}
	contents_ -= second.contents_;
	return *this;
}
container1D container1D::operator - (const container1D & second){
	container1D out=*this;
	out -= second;
	return out;
}
container1D & container1D::operator /= (const container1D & denominator){
	if(bins_ != denominator.bins_){
		if(showwarnings_) std::cout << "container1D::operator /=: not same binning for " << name_ << " return this" << std::endl;
		return *this;
	}
	contents_ /= denominator.contents_;
	return *this;
}

container1D container1D::operator / (const container1D & denominator){
	container1D out=*this;
	out /= denominator;
	return out;
}
container1D & container1D::operator *= (const container1D & rhs){
	if(bins_ != rhs.bins_){
		if(showwarnings_) std::cout << "container1D::operator *=: not same binning for " << name_ << " return this" << std::endl;
		return *this;
	}
	contents_ *= rhs.contents_;
	return *this;
}
container1D container1D::operator * (const container1D & multiplier){
	container1D out = *this;
	out *= multiplier;
	return out;
}
container1D & container1D::operator *= (double scalar){
	contents_ *= scalar;
	return *this;
}

container1D container1D::operator * (double scalar){
	container1D out= * this;
	out*=scalar;
	return out;
}


/** */
std::vector<float> container1D::getCongruentBinBoundaries(const container1D & cont) const{
	size_t maxbinssize=bins_.size();
	if(cont.bins_.size()>maxbinssize) maxbinssize=cont.bins_.size();
	std::vector<float> sames(maxbinssize);
	std::vector<float>::iterator it=std::set_intersection(++bins_.begin(),bins_.end(),++cont.bins_.begin(),cont.bins_.end(),sames.begin()); //ignore UF
	sames.resize(it-sames.begin());
	return sames;
}
/**
 *  */
container1D container1D::rebinToBinning(const std::vector<float> & newbins) const{
	size_t maxbinssize=bins_.size();
	if(newbins.size()>maxbinssize) maxbinssize=newbins.size();
	std::vector<float> sames(maxbinssize);
	std::vector<float>::iterator it=std::set_intersection(++bins_.begin(),bins_.end(),newbins.begin(),newbins.end(),sames.begin());//ignore UF
	sames.resize(it-sames.begin());
	//check if possible at all:
	if(sames.size()!=newbins.size()){
		std::cout << "container1D::rebinToBinning: binning not compatible" <<std::endl;
		return *this;
	}
	container1D newcont=*this;
	//set new binning
	newcont.contents_.resizeBins(sames.size()+1); //+1: UF
	newcont.bins_=sames;
	newcont.bins_.insert(newcont.bins_.begin(),0); //create UF in newbins
	//set all zero
	for(int lay=-1;lay<(int)contents_.layerSize();lay++){
		size_t oldbinno=1;
		for(size_t thisbin=0;thisbin<bins_.size();thisbin++){
			size_t newbin=0;
			if(thisbin>0)
				newbin=newcont.getBinNo(bins_.at(thisbin));
			if(oldbinno==newbin){//add
				newcont.getBin(newbin,lay).addToContent(getBin(thisbin,lay).getContent());
				newcont.getBin(newbin,lay).setEntries(newcont.getBin(newbin,lay).getEntries()+getBin(thisbin,lay).getEntries());
				newcont.getBin(newbin,lay).setStat2(newcont.getBin(newbin,lay).getStat2()+getBin(thisbin,lay).getStat2());
			}
			else{//set
				newcont.getBin(newbin,lay).setContent(getBin(thisbin,lay).getContent());
				newcont.getBin(newbin,lay).setEntries(getBin(thisbin,lay).getEntries());
				newcont.getBin(newbin,lay).setStat2(getBin(thisbin,lay).getStat2());
				oldbinno=newbin;
			}

		}
	}
	return newcont;

}
/**
 * container1D::rebinToBinning(const container1D & cont)
 * reference(cont) needs a binning that is a subset of container to be rebinned
 * */
container1D container1D::rebinToBinning(const container1D & cont) const{
	std::vector<float> newbins=getCongruentBinBoundaries(cont);
	return rebinToBinning(newbins);
}
container1D container1D::rebin(size_t merge) const{
	std::vector<float> newbins;
	for(size_t i=0;i<bins_.size();i+=merge){
		newbins.push_back(bins_.at(i));
	}
	if(newbins.at(newbins.size()-1) != bins_.at(bins_.size()-1))
		newbins.push_back(bins_.at(bins_.size()-1));
	return rebinToBinning(newbins);
}


/*
 * container1D::addErrorContainer(TString sysname,container1D deviatingContainer, double weight)
 * same named systematics do not exist by construction, makes all <>StatCorrelated options obsolete
 */
void container1D::addErrorContainer(const TString & sysname,const container1D & deviatingContainer, double weight){
	if(bins_!=deviatingContainer.bins_){
		std::cout << "container1D::addErrorContainer(): not same binning! doing nothing for "<<name_ << " " << sysname << std::endl;
		return;
	}
	if(! (sysname.Contains("_up") || sysname.Contains("_down"))){
		std::cout << "container1D::addErrorContainer: systematic variation must be named \".._up\"  or \".._down\"! for consistent treatment. doing nothing for "
				<<name_ << " " << sysname  << std::endl;
		return;
	}
	if(contents_.getLayerIndex(sysname) < contents_.layerSize()){
		std::cout << "container1D::addErrorContainer: systematic variation already added. doing nothing for "
				<<name_ << " " << sysname  << std::endl;
		return;
	}
	//add layer with name and fill with content..

	contents_.addLayerFromNominal(sysname,deviatingContainer.contents_);

}
void container1D::addErrorContainer(const TString & sysname,const container1D  &deviatingContainer){
	addErrorContainer(sysname,deviatingContainer,1);
}
void container1D::addRelSystematicsFrom(const ztop::container1D & rhs){
	if(bins_!=rhs.bins_){
		std::cout << "container1D::addRelSystematicsFrom(): not same binning!" << std::endl;
		return;
	}
	std::cout << "container1D::addRelSystematicsFrom(): check this function it produces bullshit -> exit!!" << std::endl;
	std::exit(EXIT_FAILURE);
	/////CHECK/////
	bool tempSubtractStatCorrelated=histoContent::subtractStatCorrelated; //should not do anything here
	histoContent::subtractStatCorrelated=true;
	histoContent nominal=rhs.contents_;
	nominal.removeAdditionalLayers();
	histoContent reldiff = nominal - rhs.contents_;
	reldiff /= nominal;
	reldiff.clearLayerContent(-1); //clears nominal. only syst now should anyway be 0
	reldiff.clearLayerStat(-1);
	histoContent lhsnominal=contents_;
	lhsnominal.removeAdditionalLayers();
	//for each syst in reldiff, a lhs nominal is copied and multiplied by reldiff;
	lhsnominal *= reldiff;

	//now, lhsnominal contains systematics from rhs scaled to *this.
	contents_=lhsnominal;
	histoContent::subtractStatCorrelated=tempSubtractStatCorrelated;

}
void container1D::addGlobalRelErrorUp(const TString & sysname,const double &relerr){
	if(sysname.EndsWith("_up") || sysname.EndsWith("_down")){
		std::cout << "container1D::addGlobalRelErrorUp: name of syst. mustn't be named <>_up or <>_down! this is done automatically! doing nothing" << std::endl;
		return;
	}
	addErrorContainer(sysname+"_up", ((*this) * (relerr+1)));
}
void container1D::addGlobalRelErrorDown(const TString & sysname,const double &relerr){
	if(sysname.EndsWith("_up") || sysname.EndsWith("_down")){
		std::cout << "container1D::addGlobalRelErrorDown: name of syst. mustn't be named <>_up or <>_down! this is done automatically! doing nothing" << std::endl;
		return;
	}
	addErrorContainer(sysname+"_down", ((*this) * (1-relerr)));
}
void container1D::addGlobalRelError(const TString & sysname,const double &relerr){
	addGlobalRelErrorUp(sysname,relerr);
	addGlobalRelErrorDown(sysname,relerr);
}
void container1D::removeError(const TString &sysname){
	contents_.removeLayer(sysname);
}
void container1D::removeError(const size_t &idx){
	contents_.removeLayer(idx);
}

void container1D::transformStatToSyst(const TString &sysname){
	size_t layers=contents_.layerSize();
	if(contents_.getLayerIndex(sysname+"_up") < layers || contents_.getLayerIndex(sysname+"_down")){
		std::cout << "container1D::transformStatToSyst: Syst name already used. doing nothing " <<std::endl;
		return;
	}
	histoContent nominal=contents_,down=contents_,up=contents_;
	nominal.removeAdditionalLayers(); //performance
	down.removeAdditionalLayers();
	up.removeAdditionalLayers();
	for(size_t bin=0;bin<nominal.size();bin++){
		double nom=nominal.getBin(bin).getContent();
		double stat=nominal.getBin(bin).getStat();
		up.getBin(bin).setContent(nom+stat);
		up.getBin(bin).setStat(0);
		down.getBin(bin).setContent(nom-stat);
		down.getBin(bin).setStat(0);
	}
	const TString ups=sysname+"_up",downs=sysname+"_down";
	contents_.addLayerFromNominal(ups,up);
	contents_.addLayerFromNominal(downs,down);
}

void container1D::renameSyst(const TString &old, const TString &New){
	histoBins oldsysup=contents_.copyLayer(old+"_up");
	histoBins oldsysdown=contents_.copyLayer(old+"_down");

	removeError(old+"_up");
	removeError(old+"_down");

	contents_.addLayer(New+"_up",oldsysup);
	contents_.addLayer(New+"_down",oldsysdown);
}


void container1D::coutFullContent() const{
	std::cout << "\nContent of container " << name_ << ": " <<std::endl;
	std::cout << "binning: ";
	for(size_t i=0;i<getNBins()+1;i++)
		std::cout << bins_.at(i) << " " ;
	std::cout << std::endl;
	contents_.cout();
}

/**
 * same layers are defined by comparing their names, NOT their content NOR their ordering!
 */
bool container1D::hasSameLayers(const container1D& cont) const{
	for(size_t i=0;i<cont.getSystSize();i++){
		if(contents_.getLayerIndex(cont.contents_.getLayerName(i)) >= contents_.layerSize())
			return false;
	}
	return true;
}
/**
 * checks whether cont has same layers AND ordering of them
 */
bool container1D::hasSameLayerOrdering(const container1D& cont) const{
	return contents_.hasSameLayerMap(cont.contents_);
}

//protected

TString container1D::stripVariation(const TString &in) const{
	TString out=in;
	out.Resize(in.Last('_'));
	return out;
}
/**
 * not protected!!
 */
double container1D::getDominantVariationUp(const TString & sysname, const size_t& bin) const{
	double up=0,down=0;
	const double & cont=contents_.getBin(bin).getContent();
	size_t idx=contents_.getLayerIndex(sysname+"_up");
	up=contents_.getBin(bin,idx).getContent()-cont;
	idx=contents_.getLayerIndex(sysname+"_down");
	down=contents_.getBin(bin,idx).getContent()-cont;
	if(up>down)down=up;
	return down;
}
double container1D::getDominantVariationDown(const TString & sysname, const size_t& bin) const{
	double up=0,down=0;
	const double & cont=contents_.getBin(bin).getContent();
	size_t idx=contents_.getLayerIndex(sysname+"_up");
	up=contents_.getBin(bin,idx).getContent()-cont;
	idx=contents_.getLayerIndex(sysname+"_down");
	down=contents_.getBin(bin,idx).getContent()-cont;
	if(up<down)down=up;
	return down;
}
/**
 * deletes all syst and creates manual entry at indices 0 and 1
 * and deletes stat uncertainty in nominal entry
 */
void container1D::createManualError(){
	if(manualerror_)
		return;
	contents_.removeAdditionalLayers();
	contents_.addLayer("manual_error_up");
	contents_.addLayer("manual_error_down");
	contents_.clearLayerStat(0);
	contents_.clearLayerStat(1);
	contents_.clearLayerStat(-1);
	manualerror_=true;
}


void container1D::setOperatorDefaults(){
	histoContent::addStatCorrelated=false;
	histoContent::subtractStatCorrelated=false;
	histoContent::divideStatCorrelated=true;
	histoContent::multiplyStatCorrelated=true;
}

}//namespace
