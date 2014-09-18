#include "../interface/container.h"
#include <algorithm>
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include <parallel/algorithm>
#include "TTree.h"
#include "TFile.h"
#include "../interface/systAdder.h"
//#include <omp.h>

//some more operators
ztop::container1D operator * (float multiplier, const ztop::container1D & cont){ //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
	ztop::container1D out=cont;
	return out * multiplier;
}
/*ztop::container1D operator * (double multiplier, const ztop::container1D & cont){  //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
	ztop::container1D out=cont;
	return out * multiplier;
}*/
/*ztop::container1D operator * (int multiplier, const ztop::container1D & cont){    //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
	ztop::container1D out=cont;
	return out * multiplier;
}*/



namespace ztop{

//////////if you like cut here ;)
std::vector<container1D*> container1D::c_list;
bool container1D::c_makelist=false;

///////function definitions
container1D::container1D():

										taggedObject(taggedObject::type_container1D)

{
	canfilldyn_=false;
	//divideBinomial_=true;

	manualerror_=false;
	labelmultiplier_=1;
	showwarnings_=true;
	mergeufof_=true;
	wasunderflow_=false;
	wasoverflow_=false;
	plottag=none;
	binwidth_=0;
	gp_=0;
	hp_=0;
	if(c_makelist)c_list.push_back(this);
}
container1D::container1D(float binwidth, TString name,TString xaxisname,TString yaxisname, bool mergeufof):

						taggedObject(taggedObject::type_container1D)

{ //currently not used
	plottag=none;
	binwidth_=binwidth;
	canfilldyn_=true;
	//divideBinomial_=true;
	manualerror_=false;
	setName(name);
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
container1D::container1D(std::vector<float> bins, TString name,TString xaxisname,TString yaxisname, bool mergeufof):
						taggedObject(taggedObject::type_container1D)

{
	plottag=none;
	setBins(bins);
	//divideBinomial_=true;
	manualerror_=false;
	setName(name);
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
container1D::container1D(const container1D &c): taggedObject(c)

{
	copyFrom(c);
}
container1D& container1D::operator=(const container1D& c) {
	copyFrom(c);
	taggedObject::operator = (c);
	return *this;
}

void container1D::setBins(std::vector<float> bins){

	if(bins.size()<1){
		std::cout << "container1D::setBins: bins have to be at least of size 1! "<< name_ <<std::endl;
		throw std::logic_error("container1D::setBins: bins have to be at least of size 1!");
	}
	reset();
	//NEW: just sort bins. no error message anymore
	std::sort(bins.begin(),bins.end());
	//get rid of duplicates
	std::vector<float>::iterator it=std::unique(bins.begin(),bins.end());
	bins.resize( std::distance(bins.begin(),it) );

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
void container1D::setBinErrorUp(const size_t& bin, const float &err){
	if(!manualerror_){
		if(debug)
			std::cout << "container1D::setBinErrorUp: creating manual error" <<std::endl;
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
void container1D::setBinErrorDown(const size_t &bin, const float &err){
	if(!manualerror_){
		if(debug)
			std::cout << "container1D::setBinErrorDown: creating manual error" <<std::endl;
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
void container1D::setBinError(const size_t &bin, const float & err){
	if(!manualerror_){
		if(debug)
			std::cout << "container1D::setBinError: creating manual error" <<std::endl;
		createManualError();}
	if(bin>=bins_.size()){
		std::cout << "container1D::setBinError: bin not existent, doing nothing" << std::endl;
		return;
	}
	contents_.getBin(bin,0).setContent(contents_.getBin(bin).getContent()+err);
	contents_.getBin(bin,1).setContent(contents_.getBin(bin).getContent()-err);
}
void container1D::setBinStat(const size_t &bin, const float & err, const int &sysLayer){
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
void container1D::setBinContent(const size_t & bin, const float &content, const int &sysLayer){
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
float container1D::getBinCenter(const size_t &bin) const{
	float center=0;
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
float container1D::getBinWidth(const size_t &bin) const{
	float width=0;
	if(!(bin<bins_.size()-1) && showwarnings_){
		std::cout << "container1D::getBinWidth: ("<< name_ <<") bin not existent!" << std::endl;
	}
	else{
		width=fabs(bins_.at(bin+1)-bins_.at(bin));
	}
	return width;
}

const float & container1D::getBinContent(const size_t &bin,const int &sysLayer) const{
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
float container1D::getBinStat(const size_t& bin,const int &sysLayer) const{
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
float container1D::getBinErrorUp(const size_t & bin, bool onlystat,const TString &limittosys)const {
	float fullerr2=0;
	if(bin<bins_.size()){
		fullerr2=contents_.getBin(bin).getStat2(); //stat
		if(onlystat)
			return sqrt(fullerr2);
		if(limittosys==""){
			// make vector of all sys stripped
			std::vector<TString> sources;
			for(size_t i=0;i<contents_.layerSize();i++){ //there might be room for improvement here...
				if(debug){
					std::cout << "container1D::getBinErrorUp: checking variation with index: " <<i<< "("<< contents_.layerSize() << ")"<<std::endl;
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
float container1D::getBinErrorDown(const size_t & bin,bool onlystat,const TString & limittosys) const{
	float fullerr2=0;
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
float container1D::getBinError(const size_t & bin,bool onlystat,const TString & limittosys) const{
	float symmerror=0;
	if(getBinErrorUp(bin,onlystat,limittosys) > fabs(getBinErrorDown(bin,onlystat,limittosys))) symmerror=getBinErrorUp(bin,onlystat,limittosys);
	else symmerror=fabs(getBinErrorDown(bin,onlystat,limittosys));
	return symmerror;
}
/**
 * container1D::getSystError(unsigned int number, int bin)
 * returns deviation from nominal for systematic error with <number>
 */
float container1D::getSystError(const size_t& number, const size_t&  bin) const{

	if(number>=contents_.layerSize()){
		std::cout << "container1D::getSystError: " << number << " out of range(" << contents_.layerSize() << "-1)" << std::endl;
		return 0;
	}
	if(bin>=bins_.size()){
		std::cout << "container1D::getSystError: " << bin << " out of range(" << bins_.size() << ") (incl UF/OF)" << std::endl;
		return 0;
	}
	float err=contents_.getBin(bin,number).getContent()-contents_.getBin(bin).getContent();
	return err;
}
float container1D::getSystErrorStat(const size_t& number, const size_t&  bin) const{

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
	std::vector<float> errs;
	std::vector<float> stats;
	std::vector<TString> names;
	float cbin=bins.at(0);
	float nominal=getBinContent(bin);
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

container1D container1D::getSystContainer(const int& syslayer)const{
	container1D out;
	if(syslayer > (int) getSystSize()){
		std::cout << "container1D::getSystContainer: syst out of range" <<std::endl;
		throw std::out_of_range("container1D::getSystContainer: syst out of range");
	}
	if(syslayer<0){
		out=*this;
		out.removeAllSystematics();
	}
	else{
		out=*this;
		out.clear();
		out.contents_.getNominal()=contents_.getLayer((size_t)syslayer);
		out.setName(out.getName()+"_"+getSystErrorName((size_t)syslayer));
	}
	return out;
}
container1D container1D::getRelErrorsContainer()const{
	container1D out=*this;
	out.normalizeToContainer(*this);
	return out;
}

void container1D::removeStatFromAll(){
	for(size_t i=0;i<contents_.layerSize();i++){
		contents_.getLayer(i).removeStat();
	}
	contents_.getNominal().removeStat();
}

void container1D::mergePartialVariations(const TString & identifier,bool linearly, bool strictpartialID){
	//search for partial variations
	// <somename>_container1D_partialvariationIDString_<somepartialname>_up/down
	container1D cp=*this;
	cp.removeAllSystematics();
	//container1D mergedvarsdown=cp;

	cp.contents_.getNominal()=contents_.getNominal(); //copy nominal layer
	container1D mergedvars=cp;

	std::vector<size_t> tobemergedidxs;
	for(size_t i=0;i<getSystSize();i++){
		bool mergeit=false;
		if(strictpartialID)
			mergeit=getSystErrorName(i).BeginsWith(identifier+"_"+container1D_partialvariationIDString);
		else
			mergeit=getSystErrorName(i).BeginsWith(identifier+"_");

		if(mergeit){
			mergedvars.contents_.addLayer(getSystErrorName(i),contents_.getLayer(i));
		}
		else{
			cp.contents_.addLayer(getSystErrorName(i),contents_.getLayer(i));
		}
	}
	if(mergedvars.getSystSize()>1){
		mergedvars.mergeAllErrors(identifier,linearly);
		cp.addErrorContainer(identifier+"_up",mergedvars.getSystContainer(0));
		cp.addErrorContainer(identifier+"_down",mergedvars.getSystContainer(1));
	}
	*this=cp;
}

/**
 * names without "up" and "down"
 */
void container1D::mergeVariations(const std::vector<TString>& names, const TString & outname,bool linearly){
	container1D cp=*this;
	cp.removeAllSystematics();
	//container1D mergedvarsdown=cp;

	cp.contents_.getNominal()=contents_.getNominal(); //copy nominal layer
	container1D mergedvars=cp;

	for(size_t i=0;i<getSystSize();i++){
		if(std::find(names.begin(),names.end(), getSystErrorName(i)+"_up") != names.end()
				|| std::find(names.begin(),names.end(), getSystErrorName(i)+"_down") != names.end()){
			mergedvars.contents_.addLayer(getSystErrorName(i),contents_.getLayer(i));
		}
	}


	if(mergedvars.getSystSize() < names.size()*2){
		throw std::runtime_error("container1D::mergeVariations: at least one variation now found");
	}
	mergedvars.mergeAllErrors(outname,linearly);
	cp.addErrorContainer(outname+"_up",mergedvars.getSystContainer(0));
	cp.addErrorContainer(outname+"_down",mergedvars.getSystContainer(1));
	*this=cp;

}


void container1D::mergeVariationsFromFileInCMSSW(const std::string& filename){

	systAdder adder;
	adder.readMergeVariationsFileInCMSSW(filename);
	size_t ntobemerged=adder.mergeVariationsSize();
	for(size_t i=0;i<ntobemerged;i++){

		TString mergedname=adder.getMergedName(i);
		std::vector<TString> tobemerged=adder.getToBeMergedName(i);
		bool linearly=adder.getToBeMergedLinearly(i);

		mergeVariations(tobemerged,mergedname,linearly);
	}

}


void container1D::mergeAllErrors(const TString & mergedname,bool linearly){
	container1D cp=*this;
	cp.removeAllSystematics();
	cp.addGlobalRelErrorUp(mergedname,0);
	cp.addGlobalRelErrorDown(mergedname,0);
	for(size_t i=0;i<bins_.size();i++){
		//for(size_t sys=0;sys<getSystSize();sys++){
		float stat=getBinStat(i);
		if(!linearly){
			float cup=getBinErrorUp(i,false);
			float cdown=getBinErrorDown(i,false);
			cp.setBinContent(i,getBinContent(i)+sqrt(cup*cup-stat*stat) ,0);
			cp.setBinContent(i,getBinContent(i)-sqrt(cdown*cdown-stat*stat) ,1);
		}
		else{
			float cup=0,cdown=0;
			std::vector<TString> vars=contents_.getVariations();
			for(size_t var=0;var<vars.size();var++){
				cup  +=getBinContent(i,getSystErrorIndex(vars.at(var)+"_up"))   - getBinContent(i);
				cdown+=getBinContent(i,getSystErrorIndex(vars.at(var)+"_down")) - getBinContent(i);
			}
			cp.setBinContent(i,getBinContent(i) + cup ,0);
			cp.setBinContent(i,getBinContent(i) + cdown,1);
		}
		//}
	}
	*this=cp;
}

float container1D::getOverflow(const int& systLayer) const{
	float ret=0;
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
float container1D::getUnderflow(const int& systLayer) const{
	float ret=0;
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
float container1D::integral(bool includeUFOF,const int& systLayer) const{
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
	float integr=0;
	for(unsigned int i=minbin;i<maxbin;i++){
		integr+=contents_.getBin(i,systLayer).getContent();
	}
	return integr;
}
/**
 * get the integral . default: nominal (systLayer=-1)
 */
float container1D::cIntegral(float from, float to,const int& systLayer) const{
	//select bins
	unsigned int minbin=getBinNo(from);
	unsigned int maxbin=getBinNo(to);
	if(systLayer >= (int)contents_.layerSize())
		return 0;
	float integr=0;
	for(size_t i=minbin;i<=maxbin;i++){
		integr+=contents_.getBin(i,systLayer).getContent();
	}
	return integr;
}
/**
 * get the integral. default: nominal (systLayer=-1)
 * assumes uncorrelated Stat inbetween bins
 */
float container1D::integralStat(bool includeUFOF,const int& systLayer) const{
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
	float integr2=0;
	for(unsigned int i=minbin;i<maxbin;i++){
		integr2+=contents_.getBin(i,systLayer).getStat2();
	}
	return sqrt(integr2);
}
/**
 * get the integral. default: nominal (systLayer=-1)
 * assumes uncorrelated Stat inbetween bins
 */
float container1D::cIntegralStat(float from, float to,const int& systLayer) const{
	//select bins
	unsigned int minbin=getBinNo(from);
	unsigned int maxbin=getBinNo(to);
	if(systLayer >= (int)contents_.layerSize())
		return 0;
	float integr2=0;
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
	for(size_t i=minbin;i<=maxbin;i++){
		integr+=contents_.getBin(i,systLayer).getEntries();
	}
	return integr;
}
/**
 * only for visible bins
 */
float container1D::getYMax(bool dividebybinwidth,const int& systLayer) const{
	float max=-999999999999.;
	for(size_t i=1;i<bins_.size()-1;i++){ //only in visible bins
		float binc=getBinContent(i,systLayer);
		if(dividebybinwidth) binc*= 1/getBinWidth(i);
		if(max < binc) max = binc;
	}
	return max;
}

/**
 * only for visible bins
 */
float container1D::getYMin(bool dividebybinwidth,const int& systLayer) const{
	float min=999999999999.;
	for(size_t i=1;i<bins_.size()-1;i++){ //only in visible bins
		float binc=getBinContent(i,systLayer);
		if(dividebybinwidth) binc*= 1/getBinWidth(i);
		if(min > binc) min = binc;
	}
	return min;
}

/**
 * normalizes container and returns normalization factor of nominal
 */
float container1D::normalize(bool includeUFOF, bool normsyst, const float& normto){
	if(!normsyst){
		float norm=normto/integral(includeUFOF);
		*this *= norm;
		return norm;
	}
	else{
		//nominal
		float outnorm=normto / integral(includeUFOF);
		contents_.getNominal().multiply(outnorm);

		for(size_t layer=0;layer<getSystSize();layer++){
			float tmpnorm=normto / integral(includeUFOF,layer);
			contents_.getLayer(layer).multiply(tmpnorm);
		}
		return outnorm;
	}
}
/**
 * normalize to container (bin-by-bin)
 * this normalizes to the nominal content!
 */
void container1D::normalizeToContainer(const container1D & cont){
	container1D tcont=cont;
	tcont.setAllErrorsZero();
	//when dividing all systematics will be recreated from nominal (now with 0 stat error)
	//int syssize=(int)getSystSize();

	//new: do by hand
	/*
	for(size_t i=0;i<bins_.size();++i){
		float nomcont=cont.getBinContent(i);

		for(int sys=-1;sys<syssize;++sys){
			if(nomcont!=0){
				setBinContent(i, getBinContent(i,sys)/nomcont);
				setBinStat(i, getBinStat(i,sys)/nomcont);

			}
			else{
				setBinContent(i, 0);
				setBinStat(i, 0);
			}
		}
	}

	return; */
	//old impl
	bool temp=histoContent::divideStatCorrelated;
	histoContent::divideStatCorrelated=false;
	*this/=tcont;
	histoContent::divideStatCorrelated=temp;
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
void container1D::setAllZero(){
	for(size_t i=0;i<getSystSize();i++){
		contents_.getLayer(i).multiply(0);
	}
	contents_.getNominal().multiply(0);

}

/**
 * creates new TH1D and returns pointer to it
 *  getTH1D(TString name="", bool dividebybinwidth=true, bool onlystat=false)
 */
TH1D * container1D::getTH1D(TString name, bool dividebybinwidth, bool onlystat, bool nostat) const{
	if(name=="") name=name_;
	if(bins_.size() < 2)
		return 0;
	if(debug)
		std::cout << "container1D::getTH1D: bins ok, creating TH1D" <<std::endl;
	TH1D *  h = new TH1D(name,name,getNBins(),&(bins_.at(1)));
	float entriessum=0;
	if(debug)
		std::cout << "container1D::getTH1D: bins ok, filling TH1D bins" <<std::endl;
	for(size_t i=0;i<=getNBins()+1;i++){ // 0 underflow, genBins+1 overflow
		float cont=getBinContent(i);
		if(dividebybinwidth && i>0 && i<getNBins()+1) cont=cont/getBinWidth(i);
		h->SetBinContent(i,cont);
		float err=getBinError(i,onlystat);
		if(nostat) err-=getBinError(i,true);
		if(dividebybinwidth && i>0 && i<getNBins()+1) err=err/getBinWidth(i);
		h->SetBinError(i,err);
		entriessum +=contents_.getBin(i).getEntries();
	}
	h->SetEntries(entriessum);

	if(debug)
		std::cout << "container1D::getTH1D: performing some formatting" <<std::endl;
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
		float newcont=getBin(i,systNo).getContent();
		float newstat=getBin(i,systNo).getStat();
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

container1D & container1D::import(TH1 * h,bool isbinwidthdivided){
	int nbins=h->GetNbinsX();
	std::vector<float> cbins;
	for(int bin=1;bin<=nbins+1;bin++)
		cbins.push_back(h->GetBinLowEdge(bin));

	container1D out(cbins,h->GetName(),h->GetXaxis()->GetTitle(),h->GetYaxis()->GetTitle(),false);
	*this=out;
	for(int bin=0;bin<=nbins+1;bin++){
		float cont=h->GetBinContent(bin);
		if(isbinwidthdivided && bin>0 && bin<nbins)
			cont*=getBinWidth(bin);
		setBinContent(bin,cont);
		float err=h->GetBinError(bin);
		if(isbinwidthdivided && bin>0 && bin<nbins)
			err*=getBinWidth(bin);
		setBinStat(bin,err);
		setBinEntries(bin,cont/err);
	}
	return *this;
}

/**
 * imports TGraphAsymmErrors. Only works if the x-axis errors indicate the bin boundaries.
 * Y errors will be represented as new systematic layers, the stat error will be set to 0.
 * The new systematic layers can be named- if not, default is Graphimp_<up/down>
 */
container1D & container1D::import(TGraphAsymmErrors *g,bool isbinwidthdivided, const TString & newsystname){

	//create bins
	std::vector<float> bins;
	//sort points
	std::vector<double> xpoints;
	for(int i=0;i<g->GetN();i++)
		xpoints.push_back(g->GetX()[i]);

	std::vector<size_t> sortlist=retsort(xpoints.begin(),xpoints.end());
	for(size_t i=0;i<sortlist.size();i++){
		size_t realit=sortlist.at(i);
		//std::cout << "getting: "<< i << "->" << realit << std::endl;
		double x=0;
		double xnext=0;
		double y=0;
		g->GetPoint(realit,x,y);
		if(realit<(size_t)g->GetN()-1){
			g->GetPoint(realit+1,xnext,y);
			g->GetErrorXhigh(realit);
			g->GetErrorXlow(realit);
			g->GetErrorXlow(realit+1);
			if(fabs( x+ g->GetErrorXhigh(realit)-(xnext-g->GetErrorXlow(realit+1))) > 0.01*(xnext-x)){
				throw std::runtime_error("container1D::import: cannot find valid bin indications in input graph");
			}
			bins.push_back(x-g->GetErrorXlow(realit));
		}
		else{//last boundary
			bins.push_back(x-g->GetErrorXlow(realit));
			bins.push_back(x+g->GetErrorXhigh(realit));
		}
	}
	reset();
	setBins(bins);
	contents_.addLayer(newsystname+"_up");
	contents_.addLayer(newsystname+"_down");
	for(size_t i=0;i<sortlist.size();i++){
		size_t realit=sortlist.at(i);
		double x=0;
		double y=0;
		g->GetPoint(i,x,y);
		setBinContent(realit+1,y);
		setBinContent(realit+1,y+g->GetErrorYhigh(i),0);
		setBinContent(realit+1,y-g->GetErrorYlow(i),1);

		if(isbinwidthdivided){
			contents_.getBin(realit+1).multiply(getBinWidth(realit+1));
			contents_.getBin(realit+1,0).multiply(getBinWidth(realit+1));
			contents_.getBin(realit+1,1).multiply(getBinWidth(realit+1));
		}
	}


	return *this;
}

void container1D::writeTH1D(TString name, bool dividebybinwidth,bool onlystat, bool nostat) const{
	TH1D * h = getTH1D(name,dividebybinwidth,onlystat,nostat);
	h->Draw();
	h->Write();
	delete h;
}
/**
 * bins with zero content and zero stat error are being ignored! -> IMPLEMENT
 */
TGraphAsymmErrors * container1D::getTGraph(TString name, bool dividebybinwidth, bool onlystat, bool noXErrors, bool nostat) const{

	if(name=="") name=name_;


	float x[getNBins()];for(size_t i=0;i<getNBins();i++) x[i]=0;
	float xeh[getNBins()];for(size_t i=0;i<getNBins();i++) xeh[i]=0;
	float xel[getNBins()];for(size_t i=0;i<getNBins();i++) xeh[i]=0;
	float y[getNBins()];for(size_t i=0;i<getNBins();i++) y[i]=0;
	float yel[getNBins()];for(size_t i=0;i<getNBins();i++) yel[i]=0;
	float yeh[getNBins()];for(size_t i=0;i<getNBins();i++) yeh[i]=0;

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
		if(!nostat){
			yeh[i-1]=getBinErrorUp(i,onlystat)*scale;
			yel[i-1]=getBinErrorDown(i,onlystat)*scale;
		}
		else{
			yeh[i-1]=sqrt(sq(getBinErrorUp(i,onlystat)) - sq(getBinErrorUp(i,true)) )*scale;
			yel[i-1]=sqrt(sq(getBinErrorDown(i,onlystat)) - sq(getBinErrorDown(i,true)))*scale;
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
TGraphAsymmErrors * container1D::getTGraphSwappedAxis(TString name, bool dividebybinwidth, bool onlystat, bool noXErrors, bool nostat) const{

	if(name=="") name=name_;
	float x[getNBins()];
	float xeh[getNBins()];
	float xel[getNBins()];
	float y[getNBins()];
	float yel[getNBins()];
	float yeh[getNBins()];
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
		if(!nostat){
			yeh[i-1]=getBinErrorUp(i,onlystat)*scale;
			yel[i-1]=getBinErrorDown(i,onlystat)*scale;
		}
		else{
			yeh[i-1]=(getBinErrorUp(i,onlystat) - getBinErrorUp(i,true) )*scale;
			yel[i-1]=(getBinErrorDown(i,onlystat) - getBinErrorDown(i,true))*scale;
		}

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
void container1D::writeTGraph(TString name, bool dividebybinwidth, bool onlystat,bool noXErrors, bool nostat) const{
	TGraphAsymmErrors * g=getTGraph(name,dividebybinwidth,onlystat,noXErrors,nostat);
	g->SetName(name);
	g->Draw("AP");
	g->Write();
	delete g;
}
/**
 * underflow/Overflow will be lost
 */
graph container1D::convertToGraph(bool dividebybinwidth)const{
	graph out;
	out.name_=name_;
	//transfer all syst layers
	out.ycoords_=contents_;
	out.xcoords_=contents_;


	out.labelmultiplier_=labelmultiplier_;
	out.yname_=yname_;
	out.xname_=xname_;
	//get rid of UF / OF
	out.xcoords_.resizeBins(bins_.size()-2);
	out.ycoords_.resizeBins(bins_.size()-2);
	out.xcoords_.setAllZero();
	out.ycoords_.setAllZero();
	int syssize=getSystSize();
	for(size_t bin=1;bin<bins_.size()-1;bin++)
		for(int i=-1;i<syssize;i++){
			out.xcoords_.getBin(bin-1,i).setContent(getBinCenter(bin));
			if(i<0)
				out.xcoords_.getBin(bin-1,i).setStat(getBinWidth(bin)/2);
			out.ycoords_.getBin(bin-1,i)=getBin(bin,i);
		}
	return out;
}


/**
 * memory objects will be destroyed when container is destructed!!!!
 * just for quick plotting nothing particularly good....
 */
void container1D::drawFullPlot(TString name, bool dividebybinwidth,const TString &extraoptions){
	if(hp_) delete hp_;
	if(gp_) delete gp_;
	hp_=getTH1D(name,dividebybinwidth,true);
	gp_=getTGraph(name,dividebybinwidth,false,false);
	hp_->Draw("e1,"+extraoptions);
	gp_->Draw("P,same,e1"+extraoptions);
	hp_->Draw("e1,same"+extraoptions);
}

void container1D::setDivideBinomial(bool divideBinomial){
	//divideBinomial_=divideBinomial;
	std::cout << "container1D::setDivideBinomial not needed anymore!" << name_ << std::endl;
}

///////////////OPERATORS//////////////

container1D & container1D::operator += (const container1D & second){
	if(bins_ != second.bins_ && !isDummy()){
		if(showwarnings_) std::cout << "container1D::operator +=: not same binning for " << name_ << " return this" << std::endl;
		return *this;
	}
	if(!isDummy()){
		contents_ += second.contents_;
	}
	else{
		*this=second;
	}
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
container1D & container1D::operator *= (float scalar){
	contents_ *= scalar;
	return *this;
}

container1D container1D::operator * (float scalar){
	container1D out= * this;
	out*=scalar;
	return out;
}

container1D container1D::cutRight(const float & val)const{
	size_t binno=getBinNo(val);
	if(binno<1){
		throw std::out_of_range("container1D::cutRight: would cut full content. Probably not intended!?");
	}
	std::vector<float> newbins;
	newbins.insert(newbins.end(),bins_.begin()+1,bins_.begin()+binno+1);
	container1D out=*this;
	out.setBins(newbins);

	for(int i=-1;i<(int)getSystSize();i++){
		container1D tmp(newbins);
		for(size_t bin=0;bin<out.bins_.size();bin++){
			tmp.getBin(bin) = getBin(bin,i);
		}

		if(i>=0){
			out.addErrorContainer(getSystErrorName(i),tmp);
		}
		else{ //only first time
			out.contents_ = tmp.contents_;
		}
	}
	return out;
}

bool container1D::operator == (const container1D & rhs)const{
	return showwarnings_==rhs.showwarnings_
			&& binwidth_==rhs.binwidth_
			&& canfilldyn_==rhs.canfilldyn_
			&& manualerror_==rhs.manualerror_
			&& bins_==rhs.bins_
			&& contents_==rhs.contents_
			&& mergeufof_==rhs.mergeufof_
			&& wasunderflow_==rhs.wasunderflow_
			&& wasoverflow_==rhs.wasoverflow_
			&& name_==rhs.name_
			&& xname_==rhs.xname_
			&& yname_==rhs.yname_
			&& labelmultiplier_==rhs.labelmultiplier_
			//&& gp_==rhs.gp_
			// && hp_==rhs.hp_
			;

}

void container1D::divideByBinWidth(){
	for(int sys=-1;sys<(int)getSystSize();sys++){
		for(size_t i=1;i<bins_.size()-1;i++) //UF and OF don't have a binwidth
			contents_.getBin(i,sys).multiply(1/getBinWidth(i));
	}
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
container1D container1D::rebinToBinning( std::vector<float>  newbins) const{
	//check if same anyway
	std::sort(newbins.begin(),newbins.end());
	if(newbins.size() == bins_.size()-1){
		if( std::equal ( newbins.begin(),newbins.end(),++bins_.begin()))
			return *this;
	}
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
			if(oldbinno==newbin){//bins are added
				//if individual bins were rescaled for some reason,
				/*  const float& newbincontent=newcont.getBin(newbin,lay).getContent();
                const float& addbincontent=getBin(thisbin,lay).getContent(); */
				const float& newbinentries=newcont.getBin(newbin,lay).getEntries();
				const float& addbinentries=getBin(thisbin,lay).getEntries();
				const float& newbinstat2=newcont.getBin(newbin,lay).getStat2();
				const float& addbinstat2=getBin(thisbin,lay).getStat2();
				/*
                float errnewbinscale=newbinstat2/newbincontent;
                float erraddbinscale=addbinstat2/addbincontent;
				 */
				float newbinnewstat2=newbinstat2+addbinstat2;//sq(errnewbinscale)*newbinentries + sq(erraddbinscale)*addbinentries;

				newcont.getBin(newbin,lay).addToContent(getBin(thisbin,lay).getContent());
				newcont.getBin(newbin,lay).setEntries(newbinentries+addbinentries);
				//  newcont.getBin(newbin,lay).setStat2(newbinnewstat2);
				newcont.getBin(newbin,lay).setStat2(newbinnewstat2);
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
 * container1D::addErrorContainer(TString sysname,container1D deviatingContainer, float weight)
 * same named systematics do not exist by construction, makes all <>StatCorrelated options obsolete
 */
int container1D::addErrorContainer(const TString & sysname, container1D  deviatingContainer, float weight){
	if(hasTag(taggedObject::dontAddSyst_tag)){
		if(showwarnings_) std::cout << "container1D::addErrorContainer: not adding syst because container has tag \"dontAddSyst_tag\": " << getName()<<std::endl;
		return 0;
	}
	if(bins_!=deviatingContainer.bins_){
		if(deviatingContainer.isDummy()){
			deviatingContainer=*this;
		}
		else{
			std::cout << "container1D::addErrorContainer(): not same binning! doing nothing for "<<name_ << " " << sysname << std::endl;

			return -11;
		}
	}
	if(! (sysname.Contains("_up") || sysname.Contains("_down"))){
		std::cout << "container1D::addErrorContainer: systematic variation must be named \".._up\"  or \".._down\"! for consistent treatment. doing nothing for "
				<<name_ << " " << sysname  << std::endl;
		return -1;
	}
	if(contents_.getLayerIndex(sysname) < contents_.layerSize()){
		std::cout << "container1D::addErrorContainer: systematic variation already added. doing nothing for "
				<<name_ << " " << sysname  << std::endl;
		return -2;
	}
	//add layer with name and fill with content..
	manualerror_=false;
	if(debug)
		std::cout << "container1D::addErrorContainer: " << name_ << std::endl;
	contents_.setLayerFromNominal(sysname,deviatingContainer.contents_);
	return 0;
}
int container1D::addErrorContainer(const TString & sysname,const container1D  &deviatingContainer){
	return addErrorContainer(sysname,deviatingContainer,1);
}
void container1D::getRelSystematicsFrom(const ztop::container1D & rhs){
	if(bins_!=rhs.bins_){
		std::cout << "container1D::getRelSystematicsFrom(): not same binning!" << std::endl;
		return;
	}
	//what about already existing errors....? average them? better do it by hand in that case
	bool tempmakelist=c_makelist;
	c_makelist=false;
	container1D relerrs=rhs.getRelErrorsContainer(); //here we have all syst and nominal (and syst) stat

	//clear old syst
	removeAllSystematics();
	//what about nominal stat?
	// since this nominal and rhs nominal stat might be (un)correlated?
	// FOR LATER
	// first do fast fix: don't take into account problem and just treat rhs nominal and lhs nominal
	// as correlated (true for most syst)
	//relerrs.set
	relerrs.contents_.clearLayerStat(-1); //clear stat of nominal relerrors layer
	// histoContent::multiplyStatCorrelated doesn't have to be set -> no systematics!
	*this*=relerrs;
	std::cout << "container1D::getRelSystematicsFrom: carefully check the output of this function! not well tested" <<std::endl;
	c_makelist=tempmakelist;
	manualerror_=false;
	return;
}
void container1D::addRelSystematicsFrom(const ztop::container1D & rhs,bool ignorestat,bool strict){
	size_t nsysrhs=rhs.contents_.layerSize();
	container1D relerr=rhs.getRelErrorsContainer();
	for(size_t i=0;i<nsysrhs;i++){
		size_t oldlayersize=getSystSize();
		size_t newlayerit=contents_.addLayer(relerr.getSystErrorName(i));
		if(oldlayersize<getSystSize()){ //new one
			if(debug) std::cout << "container1D::addRelSystematicsFrom: adding new syst "<< relerr.getSystErrorName(i)<< " to container " <<name_ <<std::endl;
			//contents_.getLayer(newlayerit).removeStat();
			//stat are definitely not correlated
			bool isnominalequal=(!strict && rhs.contents_.getNominal().equalContent(rhs.contents_.getLayer(i),1e-2))
																			|| (strict && rhs.contents_.getNominal().equalContent(rhs.contents_.getLayer(i))) ;

			if(isnominalequal){ //this is just a copy leave it and add no variation
				//contents_.getLayer(newlayerit).removeStat();
				if(debug) std::cout << "container1D::addRelSystematicsFrom: " << relerr.getSystErrorName(i) << " is not a real variation "
						<< " only a duplicate of the nominal, skipping" <<std::endl;
				continue;
			}
			if(ignorestat)
				relerr.contents_.getLayer(i).removeStat();
			contents_.getLayer(newlayerit).multiply(relerr.contents_.getLayer(i),false);
		}
		else{
			if(debug) std::cout << "container1D::addRelSystematicsFrom: syst "<< relerr.getSystErrorName(i)<< " already existing in " <<name_ <<std::endl;
		}
	}
	manualerror_=false;
}

void container1D::addGlobalRelErrorUp(const TString & sysname,const float &relerr){
	if(sysname.EndsWith("_up") || sysname.EndsWith("_down")){
		std::cout << "container1D::addGlobalRelErrorUp: name of syst. must not be named <>_up or <>_down! this is done automatically! doing nothing" << std::endl;
		return;
	}

	addErrorContainer(sysname+"_up", ((*this) * (relerr+1)));
}
void container1D::addGlobalRelErrorDown(const TString & sysname,const float &relerr){
	if(sysname.EndsWith("_up") || sysname.EndsWith("_down")){
		std::cout << "container1D::addGlobalRelErrorDown: name of syst. mustn't be named <>_up or <>_down! this is done automatically! doing nothing" << std::endl;
		return;
	}
	addErrorContainer(sysname+"_down", ((*this) * (1-relerr)));
}
void container1D::addGlobalRelError(const TString & sysname,const float &relerr){
	if(debug) std::cout << "container1D::addGlobalRelError: " << sysname <<std::endl;
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
	if(contents_.getLayerIndex(sysname+"_up") < layers || contents_.getLayerIndex(sysname+"_down") < layers){
		std::cout << "container1D::transformStatToSyst: Syst name already used. doing nothing " <<std::endl;
		return;
	}
	histoContent nominal=contents_,down=contents_,up=contents_;
	nominal.removeAdditionalLayers(); //performance
	down.removeAdditionalLayers();
	up.removeAdditionalLayers();
	for(size_t bin=0;bin<nominal.size();bin++){
		float nom=nominal.getBin(bin).getContent();
		float stat=nominal.getBin(bin).getStat();
		up.getBin(bin).setContent(nom+stat);
		up.getBin(bin).setStat(0);
		down.getBin(bin).setContent(nom-stat);
		down.getBin(bin).setStat(0);
		contents_.setBinStat(bin,0);
	}
	const TString ups=sysname+"_up",downs=sysname+"_down";
	contents_.setLayerFromNominal(ups,up);
	contents_.setLayerFromNominal(downs,down);

}

void container1D::transformToEfficiency(){
	transformStatToSyst("binom_error");
	for(int sys=-1;sys<(int)getSystSize();sys++){
		for(size_t bin=0;bin<bins_.size();bin++){
			if(getBinContent(bin,sys) > 1) setBinContent(bin,1,sys);
			if(getBinContent(bin,sys) < 0) setBinContent(bin,0,sys);
		}
	}

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
void container1D::equalizeSystematicsIdxs(container1D &rhs){
	if(hasSameLayerOrdering(rhs))
		return;
	bool tmp=c_makelist;
	c_makelist=false;
	std::map<size_t,size_t> asso=mergeLayers(rhs);
	container1D reordered=*this;
	reordered.contents_.removeAdditionalLayers();
	for(size_t i=0;i<rhs.getSystSize();i++){ //could use asso..
		size_t oldidx=getSystErrorIndex(rhs.getSystErrorName(i));
		reordered.contents_.addLayer(getSystErrorName(oldidx),contents_.getLayer(oldidx));
		if(debug)
			std::cout << "container1D::equalizeSystematicsIdxs "<<name_ << ": " << i << " " << oldidx << " " << rhs.getSystErrorName(i) << getSystErrorName(oldidx) << std::endl;
	}
	*this=reordered;

	c_makelist=tmp;
}

/**
 * all systematics
 */
TString container1D::coutBinContent(size_t bin,const TString& unit) const{
	using namespace std;
	if(bin>=bins_.size()){
		cout << "container1D::coutBinContent: "<< bin << " bin out of range" <<endl;
		return "";
	}
	TString out;
	float content=getBinContent(bin);
	//rewrite to tex format
	//cout in standard, "out" in tex table
	/*
	 * \begin{tabular}{|c|c|}
\hline
¥ & ¥ \\
\hline
¥ & ¥ \\
\hline
\end{tabular}
	 */
	TString starttable="\\begin{tabular}{|c|c|}\n  \\hline \n" ;
	std::stringstream cont;
	cont.setf(ios::fixed,ios::floatfield);
	cont.precision(2);
	cont << content;
	starttable+= "Central & ";
	starttable+=cont.str();
	starttable+= unit +" \\\\ \\hline \\hline \n";
	starttable+=" Uncertainty& [\\%]  \\\\ \\hline\n";

	TString endtable="\\end{tabular}\n";
	TString lineend="\\\\ \\hline \n";

	cout << "container1D::coutBinContent: bin " << bin << endl;
	cout << content << " \t+-" << getBinStat(bin) << endl;
	//out+=toTString(content) +" \t+-"+ toTString(getBinStat(bin))+"\n";
	out+=getName()+"\n\n";
	out.ReplaceAll("_","\\_");
	out+=starttable;


	for(int i=0;i<(int) getSystSize();i++){
		cout << getSystErrorName(i) << "\t" << getSystError(i,bin)/content * 100 << "% +-";
		cout    << getSystErrorStat(i,bin)/content * 100 << "%" << endl;
	}
	cout << endl;
	std::vector<TString> vars=contents_.getVariations();
	for(size_t i=0;i<vars.size();i++){
		size_t idx=getSystErrorIndex(vars.at(i)+"_up");
		float errup=(getBinContent(bin,idx)-content)/content *100;
		idx=getSystErrorIndex(vars.at(i)+"_down");
		float errdn=(getBinContent(bin,idx)-content)/content *100;
		std::stringstream sup,sdn;
		sup.setf(ios::fixed,ios::floatfield);
		sup.precision(2);
		sdn.setf(ios::fixed,ios::floatfield);
		sdn.precision(2);


		TString pmmp="$\\pm^{";
		if(errup>0 && errdn<0){
			pmmp="$\\pm^{";
			errdn=fabs(errdn);
		}
		else if(errup>0 && errdn>0){
			pmmp="+^{";
		}
		else if(errup<0 && errdn>0){
			pmmp="$\\mp^{";
			errup=fabs(errup);
		}
		else if(errup<0 && errdn<0){
			pmmp="-^{";
			errup=fabs(errup); errdn=fabs(errdn);
		}
		sup << errup;
		sdn << errdn;
		TString varname=vars.at(i);
		varname.ReplaceAll("_","\\_");
		out+=varname+" & " + pmmp+sup.str()+ "}_{"+ sdn.str()+"}$" + lineend;

	}



	float totalerrdn=getBinErrorDown(bin,false,"")/content * 100;
	float totalerrup=getBinErrorUp(bin,false,"")/content * 100;

	std::stringstream stat;
	stat.setf(ios::fixed,ios::floatfield);
	stat.precision(2);
	stat << getBinError(bin,true)/content *100;
	out+=" stat. & $\\pm$" + stat.str();
	out+=lineend;

	cout << "total: -" << (totalerrdn) << " +" << totalerrup <<std::endl;

	std::stringstream sup,sdn;
	sup.setf(ios::fixed,ios::floatfield);
	sup.precision(2);
	sdn.setf(ios::fixed,ios::floatfield);
	sdn.precision(2);
	sup << totalerrup;
	sdn << totalerrdn;
	out+=(TString)"Total " + (TString)" & " + (TString)"$\\pm^{"+sup.str()+ (TString)"}_{"+ sdn.str()+(TString)"}$" + lineend + "\\hline\n";
	sup.clear();
	sdn.clear();


	out+=endtable;



	return out;
	/*
    ///old implementation

    cout << "container1D::coutBinContent: bin " << bin << endl;
    cout << content << " \t+-" << getBinStat(bin) << endl;
    out+=toTString(content) +" \t+-"+ toTString(getBinStat(bin))+"\n";

    for(int i=0;i<(int) getSystSize();i++){
        cout << getSystErrorName(i) << "\t" << getSystError(i,bin)/content * 100 << "% +-";
        out+=getSystErrorName(i) + "\t" + toTString(getSystError(i,bin)/content * 100) + "% +-";
        cout	<< getSystErrorStat(i,bin)/content * 100 << "%" << endl;
        out+=toTString(getSystErrorStat(i,bin)/content * 100) + "%\n";
    }
    float totalerrdn=getBinErrorDown(bin,false,"")/content * 100;
    float totalerrup=getBinErrorUp(bin,false,"")/content * 100;
    cout << endl;
    cout << "total: -" << (totalerrdn) << " +" << totalerrup <<std::endl;
    out+="\ntotal: -" + toTString((totalerrdn)) + " +" + toTString(fabs(totalerrup));
    return out;
	 */
}



/**
 * for bin
 * sys has to be formatted <name> without up or down or (if existent) value
 * fills in values divided by binwidth!!!!
 */
graph container1D::getDependenceOnSystematic(const size_t & bin, TString sys,float offset,TString replacename) const{ //copy on purpose

	bool divbw=true;
	if(bin>=bins_.size()){
		std::cout << "container1D::getDependenceOnSystematic: bin out of range." << std::endl;
		throw std::out_of_range("container1D::getDependenceOnSystematic: bin out of range.");
	}
	else if(bin==bins_.size()-1){
		std::cout << "container1D::getDependenceOnSystematic: warning no binwidth division for underflow/of bin" << std::endl;
		divbw=false;
	}

	bool makelist=graph::g_makelist;
	graph::g_makelist=false;
	bool hasvariations=true;

	//get variation indices and values
	std::vector<size_t> varidx;
	std::vector<const histoBin *> values;
	std::vector<float> variations;
	for(size_t i=0;i<getSystSize();i++){
		TString syserrorname=getSystErrorName(i);
		if(syserrorname.Contains(sys+"_")){

			if(hasvariations && (syserrorname == sys+"_up" || syserrorname == sys+"_down"))
				hasvariations=false;
			if(hasvariations){
				//split name
				std::stringstream ss(getSystErrorName(i).Data());
				std::string item,val;
				float value=99;
				while(std::getline(ss,item,'_')){
					if(item=="up"){
						value=atof(val.data());
						break;
					}
					else if(item=="down"){
						value=atof(val.data());
						value*=-1;
						break;
					}
					val=item;
				}
				value+=offset;
				variations.push_back(value);
			}
			else{
				if(syserrorname == sys+"_up")
					variations.push_back(1);
				else if (syserrorname == sys+"_down")
					variations.push_back(-1);
			}
			varidx.push_back(i);
			values.push_back(&getBin(bin,i));
		}
	}
	//got indices and variation values
	if(variations.size() <1){
		std::cout << "container1D::getDependenceOnSystematic: systematic " << sys << " not found. return empty" << std::endl;
		graph def;
		graph::g_makelist=makelist;
		return def;
	}
	if(replacename!="") sys=replacename;
	//if(offset) newnames=sys;
	TString add="#Delta";
	if(offset) add="";

	//get rid of possible units in name
	std::vector<TString> units;
	TString newname=sys;
	units << "[GeV]"<< "[MeV]"<< "[pb]"<< "[fb]"<< "[nb]" << "[pb/GeV]"<< "[pb/MeV]"<< "[pb]"<< "[fb]"<< "[nb]";
	for(size_t i=0;i<units.size();i++){
		if(newname.EndsWith(units.at(i)))
			newname.ReplaceAll(units.at(i),"");
	}

	TString newnames=add+newname+": "+getXAxisName()+"("+
			toTString(getBinCenter(bin)-0.5*getBinWidth(bin))+"-"+
			toTString(getBinCenter(bin)+0.5*getBinWidth(bin))+")";
	graph out(values.size()+1,newnames); //one point for each value + nominal

	out.setXAxisName(add+sys);
	variations.push_back(offset); //nominal
	values.push_back(&getBin(bin));


	float scale=1;
	if(divbw) scale = 1/getBinWidth(bin);
	for(int sysidx=-1;sysidx<(int)getSystSize();sysidx++){
		if(sysidx>-1 && std::find(varidx.begin(),varidx.end(),(size_t)sysidx) != varidx.end()) //don't use these sysidx
			continue;
		size_t sysindex=0;
		TString tsysname,stsysname;
		if(sysidx>-1){
			tsysname=getSystErrorName(sysidx);
			stsysname=stripVariation(tsysname);
			sysindex=out.xcoords_.addLayer(tsysname);
			sysindex=out.ycoords_.addLayer(tsysname);
			for(size_t p=0;p<out.getNPoints();p++){
				out.ycoords_.getBin(p,sysindex)=getBin(bin,sysidx); //takes care of sysnominal content and stat
				float nomcont=out.getPointYContent(p);
				out.ycoords_.getBin(p,sysindex).multiply( nomcont/getBinContent(bin));
				out.setPointXContent(p,variations.at(p),sysindex);
			}
		}
		else{
			for(size_t p=0;p<out.getNPoints();p++){
				out.ycoords_.getBin(p)=*values.at(p); //takes care of sysnominal content and stat
				out.ycoords_.getBin(p).multiply(scale);
				out.setPointXContent(p,variations.at(p));
			}
		}

	}


	graph::g_makelist=makelist;
	return out;
}
//protected

TString container1D::stripVariation(const TString &in) const{
	TString out;
	//out.Resize(in.Last('_'));
	if(in.EndsWith("_up")){
		out=TString(in,in.Last('_'));
	}
	else if(in.EndsWith("_down")){
		out=TString(in,in.Last('_'));
	}
	else{
		throw std::runtime_error("container1D::stripVariation: must be <>_down or <>_up");
	}

	return out;
}
/**
 * not protected!!
 */
float container1D::getDominantVariationUp( TString  sysname, const size_t& bin) const{ //copy on purpose
	float up=0,down=0;
	const float & cont=contents_.getBin(bin).getContent();
	size_t idx=contents_.getLayerIndex(sysname+"_up");
	if(idx >= contents_.layerSize())
		std::cout << "container1D::getDominantVariationUp: serious error: " << sysname << "_up not found" << std::endl;
	up=contents_.getBin(bin,idx).getContent()-cont;
	idx=contents_.getLayerIndex(sysname+"_down");
	if(idx >= contents_.layerSize())
		std::cout << "container1D::getDominantVariationUp: serious error: " << sysname << "_down not found" << std::endl;
	down=contents_.getBin(bin,idx).getContent()-cont;

	if(up < 0 && down < 0) return 0;
	else if(up >= down) return up;
	else if(down > up) return down;
	else return 0; //never reached only for docu purposes

}
float container1D::getDominantVariationDown( TString  sysname, const size_t& bin) const{//copy on purpose
	float up=0,down=0;
	const float & cont=contents_.getBin(bin).getContent();
	size_t idx=contents_.getLayerIndex(sysname+"_up");
	if(idx >= contents_.layerSize())
		std::cout << "container1D::getDominantVariationDown: serious error: " << sysname << "_up not found" << std::endl;
	up=contents_.getBin(bin,idx).getContent()-cont;
	idx=contents_.getLayerIndex(sysname+"_down");
	if(idx >= contents_.layerSize())
		std::cout << "container1D::getDominantVariationDown: serious error: " << sysname << "_down not found" << std::endl;
	down=contents_.getBin(bin,idx).getContent()-cont;

	if(up > 0 && down > 0) return 0;
	else if(up <= down) return up;
	else if(down < up) return down;
	else return 0; //never reached only for docu purposes

}
/**
 * deletes all syst and creates manual entry at indices 0 and 1
 * and deletes stat uncertainty in nominal entry
 */
void container1D::createManualError(){
	if(manualerror_)
		return;
	if(debug)
		std::cout << "container1D::createManualError..." << std::endl;
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

void container1D::copyFrom(const container1D& c){
	showwarnings_=c.showwarnings_;
	binwidth_=c.binwidth_;
	canfilldyn_=c.canfilldyn_;
	manualerror_=c.manualerror_;
	bins_=c.bins_;
	contents_=c.contents_;
	mergeufof_=c.mergeufof_;
	wasunderflow_=c.wasunderflow_;
	wasoverflow_=c.wasoverflow_;
	//divideBinomial_=c.divideBinomial_;
	name_=c.name_, xname_=c.xname_, yname_=c.yname_;
	labelmultiplier_=c.labelmultiplier_;

	gp_=0;
	hp_=0;



}

container1D container1D::append(const container1D& rhs)const{
	container1D rhsc=rhs;
	container1D out=*this;
	out.equalizeSystematicsIdxs(rhsc);
	container1D equlsdoutcp=out;

	std::vector<float> newbins(out.bins_.begin()+1,out.bins_.end()); //no UF
	float maxbinb=newbins.at(newbins.size()-1);
	float relOF=0;

	relOF=fabs(maxbinb-newbins.at(0))/100;
	newbins.push_back(maxbinb+relOF);


	for(size_t i=1;i<rhsc.bins_.size();i++){ //no UF
		newbins.push_back(maxbinb +2*relOF+ rhsc.bins_.at(i));
	}



	out.setBins(newbins);
	//add layers
	for(size_t i=0;i<equlsdoutcp.getSystSize();i++)
		out.contents_.addLayer(equlsdoutcp.getSystErrorName(i));



	if(debug)    std::cout << "container1D::append: was lhs overflow: " << wasoverflow_ << " was rhs underflow: "<<rhsc.wasunderflow_<<std::endl;

	for(int sys=-1;sys<(int)out.getSystSize();sys++){
		for(size_t i=0;i<bins_.size();i++){
			if(debug)    std::cout <<"container1D::append:  first"<<std::endl;
			out.getBin(i,sys) = equlsdoutcp.getBin(i,sys);
		}
		//rest from rhs
		for(size_t i=0;i<rhsc.bins_.size();i++){
			out.getBin(i+bins_.size(),sys) = rhsc.getBin(i,sys);
		}

	}
	return out;
}



//////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////IO////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void container1D::loadFromTree(TTree *t, const TString & plotname){
	if(!t || t->IsZombie()){
		throw std::runtime_error("container1D::loadFromTree: tree not ok");
	}
	ztop::container1D * cuftemp=0;
	if(!t->GetBranch("container1Ds")){
		throw std::runtime_error("container1D::loadFromTree: branch container1Ds not found");
	}
	bool found=false;
	size_t count=0;
	t->SetBranchAddress("container1Ds", &cuftemp);
	for(float n=0;n<t->GetEntries();n++){
		t->GetEntry(n);
		if(cuftemp->getName()==(plotname)){
			found=true;
			count++;
			*this=*cuftemp;
		}
	}
	delete cuftemp;
	if(!found){
		std::cout << "searching for: " << plotname << "... error!" << std::endl;
		throw std::runtime_error("container1D::loadFromTree: no container with name not found");
	}
	if(count>1){
		std::cout << "container1D::loadFromTree: found more than one object with name "
				<< getName() << ", took the last one." << std::endl;
	}
}
void container1D::loadFromTFile(TFile *f, const TString & plotname){
	if(!f || f->IsZombie()){
		throw std::runtime_error("container1D::loadFromTFile: file not ok");
	}
	AutoLibraryLoader::enable();
	TTree * ttemp = (TTree*)f->Get("container1Ds");
	loadFromTree(ttemp,plotname);
	delete ttemp;
}
void container1D::loadFromTFile(const TString& filename,
		const TString & plotname){
	AutoLibraryLoader::enable();
	TFile * ftemp=new TFile(filename,"read");
	loadFromTFile(ftemp,plotname);
	delete ftemp;
}

void container1D::writeToTree(TTree *t){
	if(!t || t->IsZombie()){
		throw std::runtime_error("container1D::writeToTree: tree not ok");
	}
	ztop::container1D * cufpointer=this;
	if(t->GetBranch("container1Ds")){
		t->SetBranchAddress("container1Ds", &cufpointer);
	}
	else{
		t->Branch("container1Ds",&cufpointer);
		t->SetBranchAddress("container1Ds", &cufpointer);
	}

	t->Fill();
	t->Write(t->GetName(),TObject::kOverwrite);
}
void container1D::writeToTFile(TFile *f){
	if(!f || f->IsZombie()){
		throw std::runtime_error("container1D::writeToTFile: file not ok");
	}
	f->cd();
	TTree * ttemp = (TTree*)f->Get("container1Ds");
	if(!ttemp || ttemp->IsZombie())//create
		ttemp = new TTree("container1Ds","container1Ds");
	writeToTree(ttemp);
	delete ttemp;
}
void container1D::writeToTFile(const TString& filename){
	TFile * ftemp=new TFile(filename,"update");
	if(!ftemp || ftemp->IsZombie()){
		delete ftemp;
		ftemp=new TFile(filename,"create");
	}

	writeToTFile(ftemp);
	delete ftemp;
}


}//namespace
