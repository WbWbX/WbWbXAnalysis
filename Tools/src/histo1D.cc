#include "../interface/histo1D.h"
#include <algorithm>
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include <algorithm>
#include "TTree.h"
#include "TFile.h"
#include "../interface/systAdder.h"
#include "../interface/textFormatter.h"

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>

//#include <omp.h>

//some more operators
ztop::histo1D operator * (float multiplier, const ztop::histo1D & cont){ //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
	ztop::histo1D out=cont;
	return out * multiplier;
}
/*ztop::histo1D operator * (double multiplier, const ztop::histo1D & cont){  //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
	ztop::histo1D out=cont;
	return out * multiplier;
}*/
/*ztop::histo1D operator * (int multiplier, const ztop::histo1D & cont){    //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
	ztop::histo1D out=cont;
	return out * multiplier;
}*/



namespace ztop{


//////////if you like cut here ;)
std::vector<histo1D*> histo1D::c_list;
bool histo1D::c_makelist=false;
bool histo1D::showwarnings=false;
///////function definitions
histo1D::histo1D():taggedObject(taggedObject::type_container1D)
{

	//divideBinomial_=true;

	manualerror_=false;
	showwarnings=true;
	mergeufof_=true;
	wasunderflow_=false;
	wasoverflow_=false;
	gp_=0;
	hp_=0;
	if(c_makelist)c_list.push_back(this);
}

histo1D::histo1D(const std::vector<float>& bins,const TString& name,const TString& xaxisname,const TString& yaxisname, bool mergeufof):
																																																																																																															taggedObject(taggedObject::type_container1D)

{
	setBins(bins);
	//divideBinomial_=true;
	manualerror_=false;
	setName(name);
	xname_=xaxisname;
	yname_=yaxisname;
	showwarnings=true;
	if(c_makelist)c_list.push_back(this);
	mergeufof_=mergeufof;
	wasunderflow_=false;
	wasoverflow_=false;
	gp_=0;
	hp_=0;
}
histo1D::~histo1D(){
	for(unsigned int i=0;i<c_list.size();i++){
		if(c_list[i] == this) c_list.erase(c_list.begin()+i);
	}
	if(gp_) delete gp_;
	if(hp_) delete hp_;
}
histo1D::histo1D(const histo1D &c): taggedObject(c){
	copyFrom(c);
}
histo1D& histo1D::operator=(const histo1D& c) {
	taggedObject::operator = (c);
	copyFrom(c);
	return *this;
}

histo1D histo1D::createOne()const{
	histo1D out=*this;
	out.setAllZero(); //sets all stat to 0

	for(int sys=-1;sys<(int)getSystSize();sys++)
		for(size_t bin=0;bin<bins_.size();bin++)
			out.getBin(bin,sys).setContent(1);
	return out;
}

void histo1D::setBins(std::vector<float> bins){


	reset();
	if(bins.size()<1)
		bins.push_back(1);
	//NEW: just sort bins. no error message anymore
	std::sort(bins.begin(),bins.end());
	//get rid of duplicates
	std::vector<float>::iterator it=std::unique(bins.begin(),bins.end());
	bins.resize( std::distance(bins.begin(),it) );

	bins_=bins;
	bins_.insert(bins_.begin(),0); //underflow
	//overflow is the last one
	contents_ = histoContent(bins_.size());
	manualerror_=false;
}

void histo1D::setBinsFrom(const histo1D&c){
	std::vector<float> bins=c.bins_;
	bins.erase(bins.begin()); //get rid of underflow
	setBins(bins);
}

/**
 * deletes all systematic entries and creates a new systematics! layer
 * named manually_set_(up/down)
 */
void histo1D::setBinErrorUp(const size_t& bin, const float &err){
	if(!manualerror_ || contents_.layerSize() <2){
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
void histo1D::setBinErrorDown(const size_t &bin, const float &err){
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
void histo1D::setBinError(const size_t &bin, const float & err){
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
void histo1D::setBinStat(const size_t &bin, const float & err, const int &sysLayer){
	if(bin<bins_.size() && (sysLayer<0 ||(size_t)sysLayer < contents_.layerSize() )){
		contents_.getBin(bin,sysLayer).setStat(err);
	}
	else{
		std::cout << "container1D::setBinStat: bin not existent!" << std::endl;
	}
}
void histo1D::setBinEntries(const size_t &bin, const uint32_t & entries, const int &sysLayer){
	if(bin<bins_.size() && (sysLayer<0 ||(size_t)sysLayer < contents_.layerSize() )){
		contents_.getBin(bin,sysLayer).setEntries(entries);
	}
	else{
		std::cout << "container1D::setBinEntries: bin not existent!" << std::endl;
	}
}
void histo1D::setBinContent(const size_t & bin, const float &content, const int &sysLayer){
	if(bin<bins_.size()&& (sysLayer<0 ||(size_t)sysLayer < contents_.layerSize() )){
		contents_.getBin(bin,sysLayer).setContent(content);
	}
	else{
		std::cout << "container1D::setBinContent: bin not existent!" << std::endl;
	}
}



bool histo1D::mirror(){
	if(!getNBins()) return false;
	if(bins_.at(1) != -bins_.at(bins_.size()-1)) return false;
	histo1D out=*this;

	for(int sys=-1;sys<(int)getSystSize();sys++){
		for(size_t i=0;i<getBins().size();i++){
			size_t rightbin=getBins().size()-1-i;
			out.getBin(rightbin)=getBin(i);

		}
	}
	*this=out;
	return true;
}

size_t histo1D::getNBins() const{
	return bins_.size()-2;
}
const TString & histo1D::getBinName(size_t idx)const{
	if(idx>=bins_.size())
		throw std::out_of_range("container1D::getBinName: idx out of range");
	return binnames_.at(idx);
}

void histo1D::setBinName(size_t idx,const TString& name){
	if(binnames_.size()!=bins_.size()){
		binnames_.resize(bins_.size(),"");
	}
	if(idx>=bins_.size())
		throw std::out_of_range("container1D::setBinName: idx out of range");
	binnames_.at(idx)=name;
}


float histo1D::getBinCenter(const size_t &bin) const{
	float center=0;
	if((bin>=bins_.size()) && showwarnings){
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
float histo1D::getBinWidth(const size_t &bin) const{
	float width=0;
	if(!(bin<bins_.size()-1) && showwarnings){
		std::cout << "container1D::getBinWidth: ("<< name_ <<") bin not existent!" << std::endl;
	}
	else{
		width=fabs(bins_.at(bin+1)-bins_.at(bin));
	}
	if(width==0)
		throw std::runtime_error("histo1D::getBinWidth: returned 0 bin width");
	return width;
}

const float & histo1D::getBinContent(const size_t &bin,const int &sysLayer) const{
	if(bin<bins_.size()&& (sysLayer<0 ||(size_t)sysLayer < contents_.layerSize())){
		return contents_.getBin(bin,sysLayer).getContent();
	}
	else{
		if(showwarnings)std::cout << "container1D::getBinContent: ("<< name_ <<") bin not existent!" << std::endl;
		throw std::out_of_range("container1D::getBinContent: bin not existent!");
		//return contents_.getBin(0).getContent();
	}
}
const uint32_t & histo1D::getBinEntries(const size_t& bin,const int &sysLayer) const{
	if(bin>=bins_.size() || sysLayer>=(long)contents_.layerSize()){
		throw std::out_of_range("container1D::getBinEntries: index out of range");
	}
	return contents_.getBin(bin,sysLayer).getEntries();
}
float histo1D::getBinStat(const size_t& bin,const int &sysLayer) const{
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
float histo1D::getBinErrorUp(const size_t & bin, bool onlystat,const TString &limittosys)const {
	double fullerr2=0;
	if(bin<bins_.size()){
		fullerr2=contents_.getBin(bin).getStat2(); //stat
		if(onlystat)
			return std::sqrt(fullerr2);
		if(limittosys==""){
			// make vector of all sys stripped
			std::vector<TString> sources=getSystNameList();
			for(size_t i=0;i<sources.size();i++){ //there might be room for improvement here...
				double domvar=getDominantVariationUp(sources.at(i),bin);
				fullerr2 += domvar*(domvar);
				if(fullerr2!=fullerr2)
					throw std::runtime_error(("histo1D::getBinErrorUp: nan for " + sources.at(i)+": "+ getName() +": "+toTString(bin)).Data());
			}
		}
		else{
			double tmppp=getDominantVariationUp(limittosys,bin);
			fullerr2 += tmppp*tmppp;
		}

		fullerr2=std::sqrt(fabs(fullerr2));
		if(fullerr2!=fullerr2)
			throw std::runtime_error(("histo1D::getBinErrorUp: nan produced somewhere for "+ getName()+": "+toTString(bin)).Data());


		return fullerr2;
	}
	else{
		if(showwarnings)std::cout << "container1D::getBinErrorUp: bin not existent!" << std::endl;
		return 0;
	}
}
/**
 * returns the bin error variation down. If onlystat=false, each systematic uncertainty will be added in squares to
 * the stat. error. limittosys identifies a systematic source the output should be limited to.
 * no effect if onlystat=true
 * always returns a POSITIVE value
 */
float histo1D::getBinErrorDown(const size_t & bin,bool onlystat,const TString & limittosys) const{
	double fullerr2=0;
	if(bin<bins_.size()){
		fullerr2=contents_.getBin(bin).getStat2(); //stat
		if(fullerr2<0)
			throw std::runtime_error("stat<0");
		if(onlystat)
			return std::sqrt(fullerr2);
		if(limittosys==""){
			// make vector of all sys stripped
			std::vector<TString> sources=getSystNameList();
			for(size_t i=0;i<sources.size();i++){ //there might be room for improvement here...
				double domvar=getDominantVariationDown(sources.at(i),bin);
				fullerr2 += sq(domvar);
				if(fullerr2!=fullerr2)
					throw std::runtime_error(("histo1D::getBinErrorDown: nan for " + sources.at(i)+": "+ getName()+": "+toTString(bin)).Data());

			}
		}
		else{
			double tmppp=getDominantVariationDown(limittosys,bin);
			fullerr2 += tmppp*tmppp;
		}
		fullerr2=std::sqrt(fabs(fullerr2));
		if(fullerr2!=fullerr2)
			throw std::runtime_error(("histo1D::getBinErrorDown: nan produced somewhere for "+ getName()+": "+toTString(bin)).Data());

		return fullerr2;
	}
	else{
		if(showwarnings)std::cout << "container1D::getBinErrorDown: bin not existent!" << std::endl;
		return 0;
	}
}
/**
 * returns the symmetrized (max(down,up)) bin error variation. If onlystat=false, each systematic uncertainty will be added in squares to
 * the stat. error. limittosys identifies a systematic source the output should be limited to.
 * no effect if onlystat=true
 */
float histo1D::getBinError(const size_t & bin,bool onlystat,const TString & limittosys) const{
	float symmerror=0;
	float errup=getBinErrorUp(bin,onlystat,limittosys);
	float errdow=getBinErrorDown(bin,onlystat,limittosys);
	if(errup > fabs(errdow))
		symmerror=errup;
	else symmerror=fabs(errdow);

	return symmerror;
}
/**
 * histo1D::getSystError(unsigned int number, int bin)
 * returns deviation from nominal for systematic error with <number>
 */
float histo1D::getSystError(const size_t& number, const size_t&  bin) const{

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
float histo1D::getSystErrorStat(const size_t& number, const size_t&  bin) const{

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
const TString &histo1D::getSystErrorName(const size_t &number) const{
	if(number>=contents_.layerSize()){
		std::string errstr= ("container1D::getSystErrorName: " + toTString(number) +
				" out of range(" + toTString((int)getSystSize()-1) + ")" ).Data();
		throw std::out_of_range(errstr);
	}
	return contents_.getLayerName(number);
}
size_t  histo1D::getSystErrorIndex(const TString & name) const{
	size_t ret;
	ret=contents_.getLayerIndex(name);
	if(ret==getSystSize()){
		std::string errstr=("container1D::getSystErrorIndex: name " + name + "not found").Data();
		throw std::out_of_range(errstr);
	}
	return ret;

}

void histo1D::splitSystematic(const size_t & number, const float& fracadivb,
		const TString & splinamea,  const TString & splinameb){
	if(number>=contents_.layerSize()){
		std::string errstr= ("container1D::splitSystematic: " + toTString(number) +
				" out of range(" + toTString((int)getSystSize()-1) + ")" ).Data();
		throw std::out_of_range(errstr);
	}
	if(fracadivb <0 || (fracadivb)>1.){
		throw std::out_of_range("container1D::splitSystematic: fraction needs to be between 0 and 1");
	}
	float weighta = std::sqrt(fracadivb);
	float weightb = 0;
	if(!isApprox(1-fracadivb,(float)0.,0.0001))
		weightb =	std::sqrt(1.-fracadivb);
	if(weighta!=weighta || weightb!=weightb)
		throw std::runtime_error("histo1D::splitSystematic: nan. check weights (0 <= w <=1.");

	histo1D syscont = getSystContainer(number);
	contents_.removeLayer(number);
	if(weighta>0)
		addErrorContainer(splinamea,syscont,weighta);
	if(weightb>0)
		addErrorContainer(splinameb,syscont,weightb);

}

/**
 * histo1D breakDownSyst(const size_t & bin, const TString & constrainTo="")
 * constrain to "_up" or "_down"
 */
histo1D histo1D::breakDownSyst(const size_t & bin, bool updown) const{
	std::vector<float> bins;
	bins.push_back(-0.5);
	TString nameadd="_up";
	if(!updown) nameadd="_down";
	histo1D out(bins,name_+"_syst_"+toTString(bin)+nameadd,"","#Delta [%]");
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
		out.setBinName(i+1,names.at(i));
	}
	return out;
}
/**
 * make sure there are no bins with the same name!
 */
histo1D histo1D::sortByBinNames(const histo1D & reference) const{
	if(reference.bins_.size() != bins_.size()){
		std::cout << "container1D::sortByBinNames: binning has to be the same return empty container" << std::endl;
		return histo1D();
	}
	if(!hasBinNames() || !reference.hasBinNames()){
		throw std::logic_error("container1D::sortByBinNames: no bins names");
	}
	std::vector<size_t> asso;
	asso.resize(bins_.size(),0); //only for same indicies
	for(size_t i=1;i<bins_.size()-1;i++){
		for(size_t j=1;j<reference.bins_.size()-1;j++){
			if(reference.getBinName(j) == getBinName(i)){
				asso.at(i)=j;
			}
		}
	}
	histo1D out = *this;
	for(int sys=-1;sys<(int)getSystSize();sys++){
		for(size_t i=0;i<out.bins_.size();i++){
			out.getBin(asso.at(i),sys)=getBin(i,sys);
		}
	}
	return out;
}

histo1D histo1D::getSystContainer(const int& syslayer)const{
	histo1D out;
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
histo1D histo1D::getRelErrorsContainer()const{
	histo1D out=*this;
	out.normalizeToContainer(*this);
	return out;
}

void histo1D::removeStatFromAll(bool alsonom){
	for(size_t i=0;i<contents_.layerSize();i++){
		contents_.getLayer(i).removeStat();
	}
	if(alsonom)
		contents_.getNominal().removeStat();
}
void histo1D::createStatFromContent(){
	for(int i=-1;i<(int)contents_.layerSize();i++){
		for(size_t j=0;j<bins_.size();j++){
			if(contents_.getBinContent(j,i)>0)
				contents_.setBinStat(j,std::sqrt(contents_.getBinContent(j,i)),i);
			else
				contents_.setBinStat(j,0,i);
		}
	}
}

void histo1D::mergePartialVariations(const TString & identifier,bool linearly, bool strictpartialID){
	//search for partial variations
	// <somename>_histo1D_partialvariationIDString_<somepartialname>_up/down
	histo1D cp=*this;
	cp.removeAllSystematics();
	//histo1D mergedvarsdown=cp;

	cp.contents_.getNominal()=contents_.getNominal(); //copy nominal layer
	histo1D mergedvars=cp;

	std::vector<size_t> tobemergedidxs;
	for(size_t i=0;i<getSystSize();i++){
		bool mergeit=false;
		if(strictpartialID)
			mergeit=getSystErrorName(i).BeginsWith(identifier+"_"+histo1D_partialvariationIDString);
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
void histo1D::mergeVariations(const std::vector<TString>& names, const TString & outname,bool linearly){
	histo1D cp=*this;
	cp.removeAllSystematics();
	//histo1D mergedvarsdown=cp;

	cp.contents_.getNominal()=contents_.getNominal(); //copy nominal layer
	histo1D mergedvars=cp;
	std::vector<size_t> usedidcs;

	for(size_t i=0;i<getSystSize();i++){
		TString namewithoutupdown=getSystErrorName(i);
		if(namewithoutupdown.EndsWith("_up"))
			namewithoutupdown.Remove(namewithoutupdown.Length()-3);
		else if(namewithoutupdown.EndsWith("_down"))
			namewithoutupdown.Remove(namewithoutupdown.Length()-5);

		if(std::find(names.begin(),names.end(), namewithoutupdown) != names.end()){
			mergedvars.contents_.addLayer(getSystErrorName(i),contents_.getLayer(i));
			usedidcs.push_back(i);
		}
	}


	if(mergedvars.getSystSize() < names.size()*2){
		std::cout << "container1D::mergeVariations: tried to merge: " ;
		for(size_t i=0;i<names.size();i++)
			std::cout << names.at(i) << " ";
		std::cout << "\nmerged: ";
		for(size_t i=0;i<usedidcs.size();i++)
			std::cout << getSystErrorName(usedidcs.at(i)) << " ";
		std::cout << std::endl;
		std::cout << "avail: ";
		for(size_t i=0;i<getSystSize();i++)
			std::cout << getSystErrorName(i)<< " ";
		std::cout << std::endl;

		throw std::runtime_error("container1D::mergeVariations: at least one variation now found");
	}
	mergedvars.mergeAllErrors(outname,linearly);

	//add remaining syst
	for(size_t i=0;i<getSystSize();i++){
		if(std::find(usedidcs.begin(),usedidcs.end(),i) == usedidcs.end()){ //not used
			cp.contents_.addLayer(getSystErrorName(i),contents_.getLayer(i));
		}
	}
	//add merged ones
	cp.addErrorContainer(outname+"_up",mergedvars.getSystContainer(0));
	cp.addErrorContainer(outname+"_down",mergedvars.getSystContainer(1));
	*this=cp;

}


void histo1D::mergeVariationsFromFileInCMSSW(const std::string& filename){

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


void histo1D::mergeAllErrors(const TString & mergedname,bool linearly,const corrMatrix& corr){

	std::vector<size_t > assoup,assodown;
	bool usecorr=false;
	if(corr.size()>0){
		//do a check of syst and asso indicies (up/down -> corrmat idx)
		if(corr.size()*2 != contents_.layerSize())
			throw std::out_of_range("histo1D::mergeAllErrors: correlation matrix has wrong size");
		for(size_t i=0;i<corr.size();i++){
			try{
				size_t upidx=getSystErrorIndex(corr.getEntryName(i)+"_up");
				size_t downidx=getSystErrorIndex(corr.getEntryName(i)+"_down");
				assoup.push_back(upidx);
				assodown.push_back(downidx);
			}
			catch(...){
				std::string errstr="histo1D::mergeAllErrors: syst name from corr matrix not found in histogram: ";
				errstr+=corr.getEntryName(i).Data();
				throw std::out_of_range(errstr);
			}

		}
		if(assoup.size() != corr.size()){
			throw std::out_of_range("histo1D::mergeAllErrors: not all systematics specified in correlation matrix found in histo");
		}
		usecorr=true;
	}


	histo1D cp=*this;
	cp.removeAllSystematics();
	cp.addGlobalRelErrorUp(mergedname,0);
	cp.addGlobalRelErrorDown(mergedname,0);
	for(size_t i=0;i<bins_.size();i++){
		//for(size_t sys=0;sys<getSystSize();sys++){
		float stat=getBinStat(i);
		if(!linearly && !usecorr){
			float cup=getBinErrorUp(i,false);
			float cdown=getBinErrorDown(i,false);
			cp.setBinContent(i,getBinContent(i)+std::sqrt(cup*cup-stat*stat) ,0);
			cp.setBinContent(i,getBinContent(i)-std::sqrt(cdown*cdown-stat*stat) ,1);
		}
		else if(usecorr){
			float cup=0; //defined from direction of 0,0 entry , can also mean down
			//float cdown=0;
			for(size_t row=0;row<assoup.size();row++){
				for(size_t col=0;col<assoup.size();col++){
					double  corrcoeff=corr.getEntry(row,col);
					float errupvara =  (getBinContent(i,assoup.at(row)) - getBinContent(i));
					float errdownvara= (getBinContent(i,assodown.at(row)) - getBinContent(i));
					float errupvarb =  (getBinContent(i,assoup.at(col)) - getBinContent(i));
					float errdownvarb= (getBinContent(i,assodown.at(col)) - getBinContent(i));

					//	float corra=1,corrb=1;
					float maxa=std::max(fabs(errupvara), fabs(errdownvara));
					//	if(errupvara<errdownvara){
					//		corra=-1;
					//	}
					float maxb=std::max(fabs(errupvarb), fabs(errdownvarb));
					//	if(errupvarb<errdownvarb){
					//		corrb=-1;
					//	} corra*corrb
					cup+= corrcoeff*maxa*maxb ;

					//symmetrize

				}
			}
			//cdown=cup;
			//cup+=stat*stat;
			//cdown+=stat*stat;
			//bool anticorr;
			//float maxup=fabs(getMaxVar(true, cup, -cdown, anticorr));
			//float maxdown=fabs(getMaxVar(false, cup, -cdown, anticorr));
			cp.setBinContent(i,getBinContent(i)+std::sqrt(cup) ,0);
			cp.setBinContent(i,getBinContent(i)-std::sqrt(cup) ,1);
		}
		else{ //linearly
			float cup=0,cdown=0;
			std::vector<TString> vars=contents_.getVariations();
			for(size_t var=0;var<vars.size();var++){ //what if in same direction...?
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

float histo1D::getOverflow(const int& systLayer) const{
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
float histo1D::getUnderflow(const int& systLayer) const{
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
float histo1D::integral(bool includeUFOF,const int& systLayer) const{
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
float histo1D::cIntegral(float from, float to,const int& systLayer) const{
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
float histo1D::integralStat(bool includeUFOF,const int& systLayer) const{
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
	return std::sqrt(integr2);
}
/**
 * get the integral. default: nominal (systLayer=-1)
 * assumes uncorrelated Stat inbetween bins
 */
float histo1D::cIntegralStat(float from, float to,const int& systLayer) const{
	//select bins
	unsigned int minbin=getBinNo(from);
	unsigned int maxbin=getBinNo(to);
	if(systLayer >= (int)contents_.layerSize())
		return 0;
	float integr2=0;
	for(size_t i=minbin;i<=maxbin;i++){
		integr2+=contents_.getBin(i,systLayer).getStat2();
	}
	return std::sqrt(integr2);
}
/**
 * get the integral. default: nominal (systLayer=-1)
 * assumes uncorrelated Stat inbetween bins
 */
size_t histo1D::integralEntries(bool includeUFOF,const int& systLayer) const{
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
size_t histo1D::cIntegralEntries(float from, float to,const int& systLayer) const{
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

histo1D histo1D::getIntegralBin(const bool& includeUFOF)const{
	if(bins_.size()<2)
		throw std::out_of_range("container1D::getIntegralBin: no bins!");

	std::vector<float> newbins;
	newbins.push_back(0);
	newbins.push_back(1);
	histo1D newcont=*this;
	newcont.contents_.resizeBins(newbins.size()+1); //+1: UF
	newcont.bins_=newbins;
	newcont.bins_.insert(newcont.bins_.begin(),0);
	newcont.setAllZero();
	for(int lay=-1;lay<(int)contents_.layerSize();lay++){
		for(size_t thisbin=0;thisbin<bins_.size();thisbin++){
			size_t newbin=0;
			if(includeUFOF || thisbin>0)//underflow extra
				newbin=newcont.getBinNo(0.5);
			if(!includeUFOF && thisbin<bins_.size()-1)
				newbin=newcont.getBinNo(1.5);
			newcont.getBin(newbin,lay)+=getBin(thisbin,lay);
		}
	}
	//keep the debug here - still no idea where an error might come from
	float debugtesta=integral(includeUFOF);
	float debugtestb=newcont.integral(includeUFOF);

	if(fabs((debugtesta-debugtestb)/debugtestb)>0.0001){
		throw std::logic_error("histo1D::getIntegralBin: failed!");
	}

	return newcont;
	/*
	///////////////old implementation
	size_t maxbin=bins_.size()-1;
	std::vector<float> onebin;
	onebin.push_back(bins_.at(1));
	onebin.push_back(bins_.at(maxbin));

	histo1D out= rebinToBinning(onebin);
	//now has 3 bins: UF, bin, OF
	out.bins_.at(0)=0;
	out.bins_.at(1)=0; //real bin is from 0 to 1
	out.bins_.at(2)=1;
	if(includeUFOF){
		for(int sys=-1;sys<(int)out.getSystSize();sys++){
			out.contents_.getBin(1,sys).add(out.contents_.getBin(0,sys));
			out.contents_.getBin(1,sys).add(out.contents_.getBin(2,sys));
			out.contents_.getBin(0,sys).multiply(0);
			out.contents_.getBin(2,sys).multiply(0);
		}
	}
	float debugtesta=integral(includeUFOF);
	float debugtestb=out.integral(includeUFOF);

	if(fabs((debugtesta-debugtestb)/debugtestb)>0.0001){
		throw std::logic_error("histo1D::getIntegralBin: failed!");
	}

	return out; */
}

/**
 * only for visible bins
 */
float histo1D::getYMax(bool dividebybinwidth,const int& systLayer) const{
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
float histo1D::getYMin(bool dividebybinwidth,const int& systLayer) const{
	float min=999999999999.;
	for(size_t i=1;i<bins_.size()-1;i++){ //only in visible bins
		float binc=getBinContent(i,systLayer);
		if(dividebybinwidth) binc*= 1/getBinWidth(i);
		if(min > binc) min = binc;
	}
	return min;
}

float histo1D::getYMax(size_t & bin,bool dividebybinwidth ,const int& systLayer) const{
	float max=-999999999999.;
	for(size_t i=1;i<bins_.size()-1;i++){ //only in visible bins
		float binc=getBinContent(i,systLayer);
		if(dividebybinwidth) binc*= 1/getBinWidth(i);
		if(max < binc){
			max = binc;
			bin=i;
		}
	}
	return max;
}
float histo1D::getYMin(size_t & binidx,bool dividebybinwidth, const int& systLayer) const{
	float min=999999999999.;
	for(size_t i=1;i<bins_.size()-1;i++){ //only in visible bins
		float binc=getBinContent(i,systLayer);
		if(dividebybinwidth) binc*= 1/getBinWidth(i);
		if(min > binc){
			min = binc;
			binidx=i;
		}
	}
	return min;
}

/**
 * normalizes container and returns normalization factor of nominal
 */
float histo1D::normalize(bool includeUFOF, bool normsyst, const float& normto){
	if(!normsyst){
		float integralf=integral(includeUFOF);
		float norm=0;
		if(integralf != 0)
			norm=normto/integralf;
		*this *= norm;
		return norm;
	}
	else{
		//nominal
		float integralf=integral(includeUFOF);
		float norm=0;
		if(integralf != 0)
			norm=normto/integralf;
		contents_.getNominal().multiply(norm);

		for(size_t layer=0;layer<getSystSize();layer++){
			float sysintegral=integral(includeUFOF,layer);
			float tmpnorm=0;
			if(sysintegral != 0)
				tmpnorm=normto / sysintegral;
			contents_.getLayer(layer).multiply(tmpnorm);
		}
		return norm;
	}
}
/**
 * normalize to container (bin-by-bin)
 * this normalizes to the nominal content!
 */
void histo1D::normalizeToContainer(const histo1D & cont){
	histo1D tcont=cont;
	tcont.removeAllSystematics();
	bool temp=histoContent::divideStatCorrelated;
	histoContent::divideStatCorrelated=false;
	*this/=tcont;
	histoContent::divideStatCorrelated=temp;
}

void histo1D::reset(){
	bins_.clear();
	manualerror_=false;
	contents_=histoContent();
}
void histo1D::clear(){
	contents_.clear();
	manualerror_=false;
}
void histo1D::setAllZero(){
	for(size_t i=0;i<getSystSize();i++){
		contents_.getLayer(i).multiply(0);
	}
	contents_.getNominal().multiply(0);

}

void histo1D::removeUFOF(){
	if(bins_.size()<3)return;
	for(int i=-1;i<(int)getSystSize();i++){
		contents_.getBin(0,i).multiply(0);
		contents_.getBin(bins_.size()-1,i).multiply(0);
	}
}

/**
 * creates new TH1D and returns pointer to it
 *  getTH1D(TString name="", bool dividebybinwidth=true, bool onlystat=false)
 */
TH1D * histo1D::getTH1D(TString name, bool dividebybinwidth, bool onlystat, bool nostat,const int& systidx) const{
	if(name=="") name=name_;
	name=textFormatter::makeCompatibleFileName(name.Data());
	if(bins_.size() < 2)
		return 0;
	if(debug)
		std::cout << "container1D::getTH1D: bins ok, creating TH1D" <<std::endl;
	TH1D *  h = new TH1D(name,name,getNBins(),&(bins_.at(1)));
	float entriessum=0;
	if(debug)
		std::cout << "container1D::getTH1D: bins ok, filling TH1D bins" <<std::endl;
	for(size_t i=0;i<=getNBins()+1;i++){ // 0 underflow, genBins+1 overflow
		float cont=getBinContent(i,systidx);

		if(dividebybinwidth && i>0 && i<getNBins()+1) cont/=getBinWidth(i);
		if(cont!=cont)
			throw std::runtime_error(("histo1D::getTH1D: nan produced for cont somewhere "+ getName()).Data());
		h->SetBinContent(i,cont);
		float err=0;
		if(!(nostat&&onlystat)){ // only stat but nostat means without error
			err=getBinError(i,onlystat);
			if(nostat){
				err=err*err -getBin(i,systidx).getStat2();
				if(err<0)err=0;
			}
			if(dividebybinwidth && i>0 && i<getNBins()+1) err/=getBinWidth(i);
		}
		if(err!=err)
			throw std::runtime_error(("histo1D::getTH1D: nan produced for err somewhere "+ getName()).Data());

		h->SetBinError(i,err);
		entriessum +=contents_.getBin(i).getEntries();
	}
	h->SetEntries(entriessum);

	if(debug)
		std::cout << "container1D::getTH1D: performing some formatting" <<std::endl;
	h->GetYaxis()->SetTitleSize(0.06);
	h->GetYaxis()->SetLabelSize(0.05);
	h->GetYaxis()->SetTitleOffset(h->GetYaxis()->GetTitleOffset() );
	h->GetYaxis()->SetTitle(yname_);
	h->GetYaxis()->SetNdivisions(510);
	h->GetXaxis()->SetTitleSize(0.06);
	h->GetXaxis()->SetLabelSize(0.05);
	h->GetXaxis()->SetTitle(xname_);
	h->LabelsDeflate("X");
	h->SetMarkerStyle(20);
	if(wasunderflow_) h->SetTitle((TString)h->GetTitle() + "_uf");
	if(wasoverflow_)  h->SetTitle((TString)h->GetTitle() + "_of");
	return h;
}

TH1D * histo1D::getAxisTH1D()const{

	size_t nbins=getNBins();
	if(nbins<100) nbins*=100;
	if(nbins<1000) nbins*=10;
	std::vector<float> bins=createBinning(nbins,bins_.at(1),bins_.at(bins_.size()-1));

	TH1D *  h = new TH1D((TString)textFormatter::makeCompatibleFileName(name_.Data())+"_axis","",bins.size()-1,&(bins.at(0)));
	h->GetYaxis()->SetTitleSize(0.06);
	h->GetYaxis()->SetLabelSize(0.05);
	h->GetYaxis()->SetTitleOffset(h->GetYaxis()->GetTitleOffset() );
	h->GetYaxis()->SetTitle(yname_);
	h->GetYaxis()->SetNdivisions(510);
	h->GetXaxis()->SetTitleSize(0.06);
	h->GetXaxis()->SetLabelSize(0.05);
	h->GetXaxis()->SetTitle(xname_);
	h->LabelsDeflate("X");
	h->SetMarkerStyle(20);
	return h;
}

/**
 * histo1D::getTH1DSyst(TString name="", unsigned int systNo, bool dividebybinwidth=true, bool statErrors=false)
 * -name: name of output histogram
 * -systNo syst number
 *
 *
 * THIS IS TOTAL BULLSHIT!!!
 */
TH1D * histo1D::getTH1DSyst(TString name, size_t systNo, bool dividebybinwidth, bool statErrors) const{
	return getTH1D(name,dividebybinwidth,true,systNo); //gets everything with only stat errors
}

histo1D& histo1D::import(const graph&gin){
	graph g=gin;
	g.sortPointsByX();
	std::vector<float> xpoints;

	bool noxerrs=true;
	for(size_t realit=0;realit<g.getNPoints();realit++){
		if(g.getPointXError(realit,false)!=0.)
			noxerrs=false;
	}
	if(noxerrs){
		for(size_t i=0;i<g.getNPoints();i++){
			if(i<g.getNPoints()-1)
				xpoints.push_back(g.getPointXContent(i)- (g.getPointXContent(i+1)-g.getPointXContent(i))/2);
			else if(i>0){
				xpoints.push_back(g.getPointXContent(i)- (g.getPointXContent(i)-g.getPointXContent(i-1))/2);
				xpoints.push_back(g.getPointXContent(i)+ (g.getPointXContent(i)-g.getPointXContent(i-1))/2);
			}
			else{
				xpoints.push_back(g.getPointXContent(i)-1);
				xpoints.push_back(g.getPointXContent(i)+1);
			}
		}
	}
	else{
		throw std::logic_error("histo1D::import: graph import with x bin indications not supported, yet (TBI)");
	}
	setBins(xpoints);
	for(int sys=-1;sys<(int)g.getSystSize();sys++){
		if(sys>-1)
			contents_.addLayer(g.getSystErrorName(sys));
		for(size_t bin=1;bin<bins_.size()-1;bin++)
			getBin(bin,sys)=gin.ycoords_.getBin(bin-1,sys);
	}

	return *this;
}
histo1D & histo1D::import(TH1 * h,bool isbinwidthdivided){
	int nbins=h->GetNbinsX();
	std::vector<float> cbins;
	for(int bin=1;bin<=nbins+1;bin++)
		cbins.push_back(h->GetBinLowEdge(bin));

	histo1D out(cbins,h->GetName(),h->GetXaxis()->GetTitle(),h->GetYaxis()->GetTitle(),false);
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
histo1D & histo1D::import(TGraphAsymmErrors *g,bool isbinwidthdivided, const TString & newsystname){

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
				throw std::runtime_error("histo1D::import: cannot find valid bin indications in input graph");
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

void histo1D::writeTH1D(TString name, bool dividebybinwidth,bool onlystat, bool nostat) const{
	TH1D * h = getTH1D(name,dividebybinwidth,onlystat,nostat);
	h->Draw();
	h->Write();
	delete h;
}
/**
 * bins with zero content and zero stat error are being ignored! -> IMPLEMENT
 */
TGraphAsymmErrors * histo1D::getTGraph(TString name, bool dividebybinwidth, bool onlystat, bool noXErrors, bool nostat) const{

	if(name=="") name=name_;
	name=textFormatter::makeCompatibleFileName(name.Data());

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
			float up=sq(getBinErrorUp(i,onlystat)) - sq(getBinErrorUp(i,true));
			float down =sq(getBinErrorDown(i,onlystat)) - sq(getBinErrorDown(i,true));
			yeh[i-1]=std::sqrt(fabs(up) )*scale;
			yel[i-1]=std::sqrt(fabs(down))*scale;
		}

	}


	TGraphAsymmErrors * g = new TGraphAsymmErrors(getNBins(),x,y,xel,xeh,yel,yeh);
	g->SetName(name);
	g->SetTitle(name);
	g->GetYaxis()->SetTitleSize(0.06);
	g->GetYaxis()->SetLabelSize(0.05);
	g->GetYaxis()->SetTitleOffset(g->GetYaxis()->GetTitleOffset() );
	g->GetYaxis()->SetTitle(yname_);
	g->GetXaxis()->SetTitleSize(0.06);
	g->GetXaxis()->SetLabelSize(0.05);
	g->GetYaxis()->SetNdivisions(510);
	g->GetXaxis()->SetTitle(xname_);
	g->SetMarkerStyle(20);
	g->GetXaxis()->SetRangeUser(bins_[1], bins_[getNBins()+1]);

	return g;
}
TGraphAsymmErrors * histo1D::getTGraphSwappedAxis(TString name, bool dividebybinwidth, bool onlystat, bool noXErrors, bool nostat) const{

	if(name=="") name=name_;
	name=textFormatter::makeCompatibleFileName(name_.Data());
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
	g->GetXaxis()->SetTitleSize(0.06);
	g->GetXaxis()->SetLabelSize(0.05);
	g->GetXaxis()->SetTitleOffset(g->GetYaxis()->GetTitleOffset() );
	g->GetXaxis()->SetTitle(yname_);
	g->GetYaxis()->SetTitleSize(0.06);
	g->GetYaxis()->SetLabelSize(0.05);
	g->GetXaxis()->SetNdivisions(510);
	g->GetYaxis()->SetTitle(xname_);
	g->SetMarkerStyle(20);
	g->GetYaxis()->SetRangeUser(bins_[1], bins_[getNBins()+1]);

	return g;
}
void histo1D::writeTGraph(TString name, bool dividebybinwidth, bool onlystat,bool noXErrors, bool nostat) const{
	TGraphAsymmErrors * g=getTGraph(name,dividebybinwidth,onlystat,noXErrors,nostat);
	g->Draw("AP");
	g->Write();
	delete g;
}
/**
 * underflow/Overflow will be lost
 */
graph histo1D::convertToGraph(bool dividebybinwidth)const{
	graph out;
	out.name_=name_;//textFormatter::makeCompatibleFileName(name_.Data());
	//transfer all syst layers
	out.ycoords_=contents_;
	out.xcoords_=contents_;


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
void histo1D::drawFullPlot(TString name, bool dividebybinwidth,const TString &extraoptions){
	if(hp_) delete hp_;
	if(gp_) delete gp_;
	hp_=getTH1D(name,dividebybinwidth,true);
	gp_=getTGraph(name,dividebybinwidth,false,false);
	hp_->Draw("e1,"+extraoptions);
	gp_->Draw("P,same,e1"+extraoptions);
	hp_->Draw("e1,same"+extraoptions);
}

void histo1D::setDivideBinomial(bool divideBinomial){
	//divideBinomial_=divideBinomial;
	std::cout << "container1D::setDivideBinomial not needed anymore!" << name_ << std::endl;
}

///////////////OPERATORS//////////////

histo1D & histo1D::operator += (const histo1D & second){
	if(bins_ != second.bins_ && !isDummy()){
		std::string errstr= "container1D::operator +=: not same binning for " + (std::string)name_.Data() +
				"+="+ (std::string)second.name_.Data()  ;
		throw std::runtime_error(errstr);
	}
	if(!isDummy()){
		contents_ += second.contents_;
	}
	else{
		*this=second;
	}
	return *this;
}


histo1D histo1D::operator + (const histo1D & second)const{
	ztop::histo1D out=*this;
	out += second;
	return out;
}

histo1D & histo1D::operator -= (const histo1D & second){
	if(bins_ != second.bins_ && !isDummy()){
		std::string errstr= "container1D::operator -=: not same binning for " + (std::string)name_.Data() +
				"-="+ (std::string)second.name_.Data()  ;
		throw std::runtime_error(errstr);
	}
	if(!isDummy()){
		contents_ -= second.contents_;
	}
	else{
		*this=second;
	}
	return *this;
}
histo1D histo1D::operator - (const histo1D & second)const{
	histo1D out=*this;
	out -= second;
	return out;
}
histo1D & histo1D::operator /= (const histo1D & denominator){
	if(bins_ != denominator.bins_){
		std::string errstr= "container1D::operator /=: not same binning for " + (std::string)name_.Data() +
				"+="+ (std::string)denominator.name_.Data()  ;
		throw std::runtime_error(errstr);
	}
	contents_ /= denominator.contents_;
	return *this;
}

histo1D histo1D::operator / (const histo1D & denominator)const{
	histo1D out=*this;
	out /= denominator;
	return out;
}
histo1D & histo1D::operator *= (const histo1D & rhs){
	if(bins_ != rhs.bins_){
		std::string errstr= "container1D::operator *=: not same binning for " + (std::string)name_.Data() +
				"+="+ (std::string)rhs.name_.Data()  ;
		throw std::runtime_error(errstr);
	}
	contents_ *= rhs.contents_;
	return *this;
}
histo1D histo1D::operator * (const histo1D & multiplier)const{
	histo1D out = *this;
	out *= multiplier;
	return out;
}
histo1D & histo1D::operator *= (float scalar){
	if(scalar==(float)1) return *this;
	contents_ *= scalar;
	return *this;
}

histo1D histo1D::operator * (float scalar)const{
	if(scalar==(float)1) return *this;
	histo1D out= * this;
	out*=scalar;
	return out;
}


void histo1D::sqrt(){
	contents_.sqrt();
}
histo1D histo1D::chi2container(const histo1D&rhs,size_t * ndof)const{
	if(bins_!=rhs.bins_)
		throw std::out_of_range("container1D::chi2container: bins have to match!");
	histo1D out=*this;
	out.removeAllSystematics();
	out.removeStatFromAll();
	out.setName(getName()+"_"+rhs.getName()+"_chi2");
	if(ndof) *ndof=0;
	for(size_t i=0;i<out.bins_.size();i++){
		float chi2=getBinContent(i) - rhs.getBinContent(i);
		float erra=getBinError(i,false);
		float errb=rhs.getBinError(i,false);
		chi2*=chi2;

		if(erra*erra+errb*errb>0){
			if(getBinContent(i)!=0 && rhs.getBinContent(i)!=0){ //is the case if no events are in there (0 stat)
				chi2/=(erra*erra+errb*errb);
				out.setBinContent(i,chi2);
				if(ndof)
					(*ndof)++;
			}
			else{ //for such small entries, the chi2 approach breaks down! this why these bins are ignored!
				out.setBinContent(i,0);
			}
		}
		else{
			out.setBinContent(i,0);
			out.setBinStat(i,1);
		}
	}
	return out;
}
float histo1D::chi2(const histo1D&rhs,size_t * ndof)const{
	//return 0;
	return chi2container(rhs,ndof).integral(true);
}


histo1D histo1D::cutRight(const float & val)const{
	size_t binno=getBinNo(val);
	if(binno<1){
		throw std::out_of_range("container1D::cutRight: would cut full content. Probably not intended!?");
	}
	std::vector<float> newbins;
	newbins.insert(newbins.end(),bins_.begin()+1,bins_.begin()+binno+1);
	histo1D out=*this;
	out.setBins(newbins);

	for(int i=-1;i<(int)getSystSize();i++){
		histo1D tmp(newbins);
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

histo1D histo1D::cutLeft(const float & val)const{
	size_t binno=getBinNo(val);
	if(binno<1){
		throw std::out_of_range("container1D::cutRight: would cut full content. Probably not intended!?");
	}
	std::vector<float> newbins;
	newbins.insert(newbins.end(),bins_.begin()+binno+1,bins_.end());
	histo1D out=*this;
	out.setBins(newbins);

	for(int i=-1;i<(int)getSystSize();i++){
		histo1D tmp(newbins);
		for(size_t bin=0;bin<out.bins_.size();bin++){
			tmp.getBin(bin) = getBin(bin+binno,i);
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

bool histo1D::operator == (const histo1D & rhs)const{

	if(manualerror_!=rhs.manualerror_)
		return false;
	if( bins_!=rhs.bins_)
		return false;
	if( contents_!=rhs.contents_)
		return false;
	if(mergeufof_!=rhs.mergeufof_)
		return false;
	if(wasunderflow_!=rhs.wasunderflow_)
		return false;
	if(wasoverflow_!=rhs.wasoverflow_)
		return false;
	if(name_!=rhs.name_)
		return false;
	if( xname_!=rhs.xname_)
		return false;
	if(yname_!=rhs.yname_)
		return false;

	return true;

}

void histo1D::divideByBinWidth(){
	for(int sys=-1;sys<(int)getSystSize();sys++){
		for(size_t i=1;i<bins_.size()-1;i++) //UF and OF don't have a binwidth
			contents_.getBin(i,sys).multiply(1/getBinWidth(i));
	}
}

/** */
std::vector<float> histo1D::getCongruentBinBoundaries(const histo1D & cont) const{
	size_t maxbinssize=bins_.size();
	if(cont.bins_.size()>maxbinssize) maxbinssize=cont.bins_.size();
	std::vector<float> sames(maxbinssize);
	std::vector<float>::iterator it=std::set_intersection(++bins_.begin(),bins_.end(),++cont.bins_.begin(),cont.bins_.end(),sames.begin()); //ignore UF
	sames.resize(it-sames.begin());
	return sames;
}
/**
 *  */
histo1D histo1D::rebinToBinning( std::vector<float>  newbins) const{
	//check if same anyway
	std::sort(newbins.begin(),newbins.end());
	if(newbins.size() == bins_.size()-1){
		std::vector<float>  testbins(bins_.begin()+1,bins_.end());
		if( testbins == newbins)//these are the same bins
			return *this;
	}
	size_t maxbinssize=bins_.size();
	if(newbins.size()>maxbinssize) maxbinssize=newbins.size();
	std::vector<float> sames(maxbinssize);
	std::vector<float>::iterator it=std::set_intersection(++bins_.begin(),bins_.end(),newbins.begin(),newbins.end(),sames.begin());//ignore UF
	sames.resize(it-sames.begin());
	//check if possible at all:
	if(sames.size()!=newbins.size()){
		std::cout << "histo1D::rebinToBinning: binning not compatible" <<std::endl;
		return *this;
	}
	/*
	 * All checks done, operation can be performed
	 */
	histo1D newcont=*this;
	//set new binning
	newcont.contents_.resizeBins(sames.size()+1); //+1: UF
	newcont.bins_=sames;
	newcont.bins_.insert(newcont.bins_.begin(),0);
	newcont.setAllZero();//set all zero
	for(int lay=-1;lay<(int)contents_.layerSize();lay++){
		for(size_t thisbin=0;thisbin<bins_.size();thisbin++){
			size_t newbin=0;
			if(thisbin>0)//underflow extra
				newbin=newcont.getBinNo(bins_.at(thisbin));
			newcont.getBin(newbin,lay)+=getBin(thisbin,lay);
		}
	}
	return newcont;
}
/**
 * histo1D::rebinToBinning(const histo1D & cont)
 * reference(cont) needs a binning that is a subset of container to be rebinned
 * */
histo1D histo1D::rebinToBinning(const histo1D & cont) const{
	if(cont.bins_ == bins_) return *this;
	std::vector<float> newbins=getCongruentBinBoundaries(cont);
	return rebinToBinning(newbins);
}
histo1D histo1D::rebin(size_t merge) const{
	std::vector<float> newbins;
	for(size_t i=0;i<bins_.size();i+=merge){
		newbins.push_back(bins_.at(i));
	}
	if(newbins.at(newbins.size()-1) != bins_.at(bins_.size()-1))
		newbins.push_back(bins_.at(bins_.size()-1));
	return rebinToBinning(newbins);
}

histo1D histo1D::rebinToRelStat(float relstatth)const{


	class interval{
	public:
		interval(size_t l,size_t r):left(l),right(r){}
		size_t left,right;
		bool isIn(size_t i){return i>=left&&i<right;}
		static bool compare(const interval& a, const interval& b){
			return a.left < b.left;
		}
	};
	class Intervals{
	public:
		void push_back(const interval& iv){ivs.push_back(iv);}
		std::vector<interval> ivs;
		bool isIn(size_t i){
			for(size_t j=0;j<ivs.size();j++)
				if(ivs.at(j).isIn(i)) return true;
			return false;
		}
	};

	//get stat and content of all bins
	std::vector<float>v_contents(bins_.size()-2);
	std::vector<float> relstat(bins_.size()-2);
	std::vector<float> stat2(bins_.size()-2);

	for(size_t i=1;i<bins_.size()-1;i++){
		v_contents.at(i-1)=getBinContent(i);
		stat2.at(i-1)=getBin(i).getStat2();
		relstat.at(i-1)=fabs(getBin(i).getStat()/getBinContent(i));
	}
	Intervals merbins;
	std::vector<size_t> statsort=retsort(relstat.begin(),relstat.end());
	//indecies ordered by stat error
	for(size_t i=0;i<statsort.size();i++){
		const size_t & binidx=statsort.at(statsort.size()-i-1);
		if(merbins.isIn(binidx)) continue;
		float statm=stat2.at(binidx);
		float contm=fabs(v_contents.at(binidx));
		bool searchleft=true;
		bool searchright=true;
		if(binidx==0)searchleft=false;
		if(binidx>=v_contents.size()-1)searchright=false;
		if(std::sqrt(statm)/contm > relstatth){
			size_t searchbinright=binidx;
			size_t searchbinleft=binidx;
			while(searchleft || searchright){
				if(searchright){
					if(searchbinright>=v_contents.size()-1){
						if(!searchleft){//hit a limit
							merbins.push_back(interval(searchbinleft,searchbinright));
						}
						searchright=false;//for next
						continue;
					}
					searchbinright+=1;
					if(merbins.isIn(searchbinright)){ //hit other boundary
						searchright=false;//for next
						continue;
					}
					statm+=stat2.at(searchbinright);
					contm+=fabs(v_contents.at(searchbinright));
					float relerr=std::sqrt(statm)/contm;
					if(relerr<relstatth){
						merbins.push_back(interval(searchbinleft,searchbinright));
						break;
					}
				}
				if(searchleft){
					if(searchbinleft<1){
						if(!searchright){//hit a limit
							merbins.push_back(interval(searchbinleft,searchbinright));
						}
						searchleft=false;//for next
						continue;
					}
					searchbinleft-=1;
					if(merbins.isIn(searchbinleft)){ //hit other boundary
						searchleft=false;//for next
						continue;
					}
					statm+=stat2.at(searchbinleft);
					contm+=fabs(v_contents.at(searchbinleft));
					float relerr=std::sqrt(statm)/contm;
					if(relerr<relstatth){
						merbins.push_back(interval(searchbinleft,searchbinright));
						break;
					}
				}

			}
		}
	}
	std::sort(merbins.ivs.begin(),merbins.ivs.end(),interval::compare);

	std::vector<size_t> nmerbins;

	for(size_t i=0;i<bins_.size()-1;i++){
		if(merbins.isIn(i))continue;
		nmerbins.push_back(i);
	}

	for(size_t i=0;i<merbins.ivs.size();i++){
		nmerbins.push_back(merbins.ivs.at(i).left);
	}
	//last bin

	std::sort(nmerbins.begin(),nmerbins.end());

	std::vector<float> newbins;
	//newbins.push_back(bins_.at(0));//uf
	for(size_t i=0;i<nmerbins.size();i++){
		if(nmerbins.at(i)>=bins_.size()-1)
			throw std::out_of_range("histo1D::rebinToRelStat: fundamental error");
		newbins.push_back(bins_.at(nmerbins.at(i)+1));
	}
	if(debug){
		for(size_t i=0;i<bins_.size();i++)
			std::cout << bins_.at(i) << " " ;
		std::cout << std::endl;
		for(size_t i=0;i<newbins.size();i++)
			std::cout << newbins.at(i) << " " ;
		std::cout << std::endl;
	}
	return rebinToBinning(newbins);
}


/*
 * histo1D::addErrorContainer(TString sysname,histo1D deviatingContainer, float weight)
 * same named systematics do not exist by construction, makes all <>StatCorrelated options obsolete
 */
int histo1D::addErrorContainer(const TString & sysname, histo1D  deviatingContainer, float weight){
	if(hasTag(taggedObject::dontAddSyst_tag)){
		if(showwarnings) std::cout << "container1D::addErrorContainer: not adding syst because histo1D.has tag \"dontAddSyst_tag\": " << getName()<<std::endl;
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
	contents_.setLayerFromNominal(sysname,deviatingContainer.contents_,weight);
	return 0;
}
int histo1D::addErrorContainer(const TString & sysname,const histo1D  &deviatingContainer){
	return addErrorContainer(sysname,deviatingContainer,1);
}
void histo1D::getRelSystematicsFrom(const ztop::histo1D & rhs){
	if(bins_!=rhs.bins_){
		std::cout << "container1D::getRelSystematicsFrom(): not same binning!" << std::endl;
		return;
	}
	//what about already existing errors....? average them? better do it by hand in that case
	bool tempmakelist=c_makelist;
	c_makelist=false;
	histo1D relerrs=rhs.getRelErrorsContainer(); //here we have all syst and nominal (and syst) stat

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
void histo1D::addRelSystematicsFrom(const ztop::histo1D & rhs,bool ignorestat,bool strict){
	size_t nsysrhs=rhs.contents_.layerSize();
	histo1D relerr=rhs.getRelErrorsContainer();
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

void histo1D::addGlobalRelErrorUp(const TString & sysname,const float &relerr){
	if(sysname.EndsWith("_up") || sysname.EndsWith("_down")){
		std::cout << "container1D::addGlobalRelErrorUp: name of syst. must not be named <>_up or <>_down! this is done automatically! doing nothing" << std::endl;
		return;
	}

	addErrorContainer(sysname+"_up", ((*this) * (relerr+1)));
}
void histo1D::addGlobalRelErrorDown(const TString & sysname,const float &relerr){
	if(sysname.EndsWith("_up") || sysname.EndsWith("_down")){
		std::cout << "container1D::addGlobalRelErrorDown: name of syst. mustn't be named <>_up or <>_down! this is done automatically! doing nothing" << std::endl;
		return;
	}
	addErrorContainer(sysname+"_down", ((*this) * (1-relerr)));
}
void histo1D::addGlobalRelError(const TString & sysname,const float &relerr){
	if(debug) std::cout << "container1D::addGlobalRelError: " << sysname <<std::endl;
	addGlobalRelErrorUp(sysname,relerr);
	addGlobalRelErrorDown(sysname,relerr);
}
void histo1D::removeError(const TString &sysname){
	contents_.removeLayer(sysname);
}
void histo1D::removeError(const size_t &idx){
	contents_.removeLayer(idx);
}
void histo1D::transformSystToStat(){
	size_t idxup=getSystErrorIndex("stat_up");
	size_t idxdown=getSystErrorIndex("stat_down");
	histo1D cp=*this;
	cp.removeAllSystematics();
	for(size_t bin=0;bin<contents_.size();bin++){
		histoBin& nombin=cp.contents_.getBin(bin);
		float maxstat=fabs(contents_.getBin(bin,idxup).getContent()-nombin.getContent());
		float otherstat=fabs(contents_.getBin(bin,idxdown).getContent()-nombin.getContent());
		if(maxstat<otherstat) maxstat=otherstat;
		nombin.setStat(maxstat);
	}
	for(size_t i=0;i<getSystSize();i++){
		if(i!=idxup && i!=idxdown)
			cp.contents_.addLayer(getSystErrorName(i),contents_.getLayer(i));
	}
	*this=cp;
}
void histo1D::transformStatToSyst(const TString &sysname){
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

void histo1D::transformToEfficiency(){
	transformStatToSyst("binom_error");
	for(int sys=-1;sys<(int)getSystSize();sys++){
		for(size_t bin=0;bin<bins_.size();bin++){
			if(getBinContent(bin,sys) > 1) setBinContent(bin,1,sys);
			if(getBinContent(bin,sys) < 0) setBinContent(bin,0,sys);
		}
	}

}
void histo1D::setAllErrorsZero(bool nominalstat){
	if(nominalstat)
		contents_.clearLayerStat(-1);
	for(size_t i=0;i<contents_.layerSize();i++)
		contents_.getLayer(i) = contents_.getNominal();

}

void histo1D::setErrorZero(const size_t& idx){
	if(idx>=getSystSize()){
		throw std::out_of_range("histo1D::setErrorZero: idx out of range");
	}
	contents_.getLayer(idx) = contents_.getNominal();

}
void histo1D::removeAllSystematics(const TString& exception){
	if(exception.Length()<1){
		contents_.removeAdditionalLayers();manualerror_=false;
	}
	else{
		histoBins upcp=contents_.getLayer(exception+"_up");
		histoBins downcp=contents_.getLayer(exception+"_down");
		contents_.removeAdditionalLayers();manualerror_=false;
		contents_.addLayer(exception+"_up",upcp);
		contents_.addLayer(exception+"_down",downcp);
	}
}

size_t histo1D::setErrorZeroContaining(const TString &in){
	size_t count=0;
	for(size_t i=0;i<contents_.layerSize();i++){
		if(contents_.getLayerName(i).Contains(in)){
			contents_.getLayer(i) = contents_.getNominal();
			count++;
		}
	}
	return count;
}

histo1D histo1D::createPseudoExperiment(TRandom3* rand,const histo1D* c, pseudodatamodes mode, int syst)const{
	std::vector<float> stat2(bins_.size(),0);
	if(c){
		if(c->bins_ != bins_){
			throw std::out_of_range("histo1D::createPseudoExperiment: parsed container does not match binning");
		}
		for(size_t i=0;i<bins_.size();i++)
			stat2.at(i)=c->getBin(i).getStat2();
	}
	else{
		for(size_t i=0;i<bins_.size();i++)
			stat2.at(i)=contents_.getBin(i,syst).getStat2();
	}
	histo1D out=*this;
	if(integral()<=0 && mode==pseudodata_poisson){ //at least one negative entry or all zero
		out.setAllZero();
		return out;
	}

	out.removeAllSystematics();
	out.removeStatFromAll();
	out.setName(getName()+"_pe");
	for(size_t i=0;i<bins_.size();i++){
		const float & nscaled=contents_.getBin(i,syst).getContent();
		if((nscaled<0 && mode==pseudodata_poisson) || nscaled==0 || stat2.at(i)==0){
			out.setBinContent(i,0);
			out.setBinStat(i,0);
			continue;
		}
		double nunscaled = sq(nscaled) / stat2.at(i);
		float newpoint=0;
		if(mode==pseudodata_poisson){
			newpoint=rand->Poisson(nunscaled);}
		else if(mode==pseudodata_gaus){
			double stat=std::sqrt(nunscaled);
			newpoint=rand->Gaus(nunscaled, stat);
			if(newpoint<0) newpoint=0;
		}
		else{
			throw std::runtime_error("histo1D::createPseudoExperiment: mode not supported");
		}
		float scale= stat2.at(i) / contents_.getBin(i,syst).getContent() ;
		float newstatorg=std::sqrt(newpoint);
		newpoint *= scale;
		newstatorg*=scale;
		if(newpoint!=newpoint){
			newpoint=0;newstatorg=0;
		}
		out.setBinContent(i,newpoint);
		out.setBinStat(i,newstatorg);

	}
	if(debug){
		std::cout << "histo1D::createPseudoExperiment: creating copies of nominal for syst "<< out.contents_.layerSize() <<std::endl;
	}
	//add the other layers as plain copies to maintain consistence
	for(size_t i=0;i<getSystSize();i++)
		out.contents_.addLayer(getSystErrorName(i),out.contents_.getNominal());

	if(debug){
		std::cout << "histo1D::createPseudoExperiment: done "<< out.contents_.layerSize() <<std::endl;
	}
	return out;

}
histo1D histo1D::createPseudoExperiments(TRandom3* rand,const histo1D* c, pseudodatamodes mode, const std::vector<size_t> & excludefromvar)const{

	histo1D tmp=*this;
	tmp.setName(getName()+"_pe");
	tmp.removeAllSystematics();
	tmp=tmp.createPseudoExperiment(rand,c,mode);

	tmp.addRelSystematicsFrom(*this,true,true);

	return tmp;
}

void histo1D::renameSyst(const TString &old, const TString &New){
	histoBins oldsysup=contents_.copyLayer(old+"_up");
	histoBins oldsysdown=contents_.copyLayer(old+"_down");

	removeError(old+"_up");
	removeError(old+"_down");

	contents_.addLayer(New+"_up",oldsysup);
	contents_.addLayer(New+"_down",oldsysdown);
}


void histo1D::coutFullContent() const{
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
bool histo1D::hasSameLayers(const histo1D& cont) const{
	for(size_t i=0;i<cont.getSystSize();i++){
		if(contents_.getLayerIndex(cont.contents_.getLayerName(i)) >= contents_.layerSize())
			return false;
	}
	return true;
}
/**
 * checks whether cont has same layers AND ordering of them
 */
bool histo1D::hasSameLayerOrdering(const histo1D& cont) const{
	return contents_.hasSameLayerMap(cont.contents_);
}
void histo1D::equalizeSystematicsIdxs(histo1D &rhs){
	if(hasSameLayerOrdering(rhs))
		return;
	bool tmp=c_makelist;
	c_makelist=false;
	std::map<size_t,size_t> asso=mergeLayers(rhs);
	histo1D reordered=*this;
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
TString histo1D::coutBinContent(size_t bin,const TString& unit) const{
	using namespace std;
	if(bin>=bins_.size()){
		cout << "container1D::coutBinContent: "<< bin << " bin out of range" <<endl;
		return "";
	}
	TString out;
	float content=getBinContent(bin);

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
	cout << content << " \t+-" << getBinStat(bin)/content*100 <<"% (stat)" << endl;
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

	cout << "total: -" << (totalerrdn) << "% +" << totalerrup <<"%" <<std::endl;
	cout << "total: -" << (totalerrdn)*content/100 << " +" << totalerrup*content/100  <<std::endl;

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

}

texTabler histo1D::makeTableFromBinContent(size_t bin,bool symmetrize,bool relative, float prec) const{
	if(bin>=bins_.size())
		throw std::out_of_range("histo1D::makeTableFromBinContent: bin out of range");
	texTabler tab("l | c ");
	float content=getBinContent(bin);
	if(prec<0){//auto precision
		float fullerr=getBinError(bin);
		if(relative){
			fullerr/=content;
			fullerr*=100;
		}
		fullerr=fabs(fullerr);
		int exp = (fullerr == 0) ? 0 : (int)(1 + std::log10(std::fabs(fullerr) ) );
		prec= fullerr * pow(10 , (exp-2));
	}
	formatter fmt;
	tab << "content "+fmt.toTString(bin) << fmt.toFixedCommaTString(content,prec/10);
	tab << texLine(2);
	std::vector<TString> sysnames=getSystNameList();
	for(size_t i=0;i<sysnames.size();i++){

		float berrorup=getBinContent(bin, getSystErrorIndex(sysnames.at(i)+"_up" )) -content;
		float berrordown=getBinContent(bin, getSystErrorIndex(sysnames.at(i)+"_down" )) -content;
		bool anticorr;//= errordown>errorup;
		float errorup=getMaxVar(true,berrorup,berrordown,anticorr);
		float errordown=getMaxVar(false,berrorup,berrordown,anticorr);
		if(relative){
			errorup/=content;
			errordown/=content;
			errorup*=100;
			errordown*=100;
		}
		if(symmetrize){
			float error=std::max(fabs(errorup),fabs(errordown));
			if(relative){
				tab << sysnames.at(i) << fmt.toFixedCommaTString(error,prec)+"\\%";
			}
			else
				tab << sysnames.at(i) << fmt.toFixedCommaTString(error,prec);
		}
		else{
			//getMaxVar(true,errorup,errordown,anticorr);

			TString errstr;
			if(anticorr)
				errstr="$\\mp^{";
			else
				errstr="$\\pm^{";
			errstr+=fmt.toFixedCommaTString(fabs(errorup),prec)+"}_{"+fmt.toFixedCommaTString(fabs(errordown),prec)+"}$";
			if(relative)
				errstr+="\\%";

			tab << sysnames.at(i) << errstr;
		}
	}
	tab << texLine(2);
	float stat=getBinStat(bin);
	if(relative){
		stat/=content;
		stat*=100;
		tab << "Stat"  << fmt.toFixedCommaTString(stat,prec)+"\\%";
	}
	else{
		tab << "Stat"  << fmt.toFixedCommaTString(stat,prec);
	}
	tab << "Total";
	if(symmetrize){
		float errtot=getBinError(bin);
		if(relative){
			errtot/=content;
			errtot*=100;
			tab  << fmt.toFixedCommaTString(errtot,prec)+"\\%";
		}
		else{
			tab  << fmt.toFixedCommaTString(errtot,prec);
		}
	}
	else{
		float errtotup=getBinErrorUp(bin);
		float errtotdown=getBinErrorDown(bin);
		if(relative){
			errtotup/=content;
			errtotup*=100;
			errtotdown/=content;
			errtotdown*=100;
			tab  << "$\\pm^{"+fmt.toFixedCommaTString(errtotup,prec)+"}_{"+ fmt.toFixedCommaTString(errtotdown,prec) +"}$\\%";
		}
		else{
			tab  << "$\\pm^{"+fmt.toFixedCommaTString(errtotup,prec)+"}_{"+ fmt.toFixedCommaTString(errtotdown,prec) +"}$";
		}
	}

	return tab;
}

std::vector<histo1D> histo1D::produceVariations(const TString & sysname, const TString& legendnom, const TString& legendup, const TString& legenddown)const{
	std::vector<histo1D>  out(3);
	out.at(0) = getSystContainer(-1);
	if(legendnom.Length()<1)
		out.at(0).setName("nominal");
	else
		out.at(0).setName(legendnom);
	size_t idxup=getSystErrorIndex(sysname+"_up");
	size_t idxdown=getSystErrorIndex(sysname+"_down");
	out.at(1) = getSystContainer(idxup);
	if(legendup.Length()<1)
		out.at(1).setName(sysname+" up");
	else
		out.at(1).setName(legendup);
	out.at(2) = getSystContainer(idxdown);
	if(legenddown.Length()<1)
		out.at(2).setName(sysname+" down");
	else
		out.at(2).setName(legenddown);
	return out;
}

/**
 * for bin
 * sys has to be formatted <name> without up or down or (if existent) value
 * fills in values divided by binwidth!!!!
 */
graph histo1D::getDependenceOnSystematic(const size_t & bin, TString sys,float offset,TString replacename) const{ //copy on purpose

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

/**
 * For weighted entries, this function returns different errors on the mean than
 * root TH1 histograms do. The error calculation of for TH1 histograms relies on assumptions
 * about Gaussian shapes and only takes into account effective entries.
 * This function propagates the uncertainties on a per-bin level.
 *
 * The central values, however coincide. If they don't, it is a bug.
 */
float histo1D::getMean(float& err, int sys)const{
	if(isDummy())return 0;
	float sumweightedbins=0;
	float sumweightedunc=0;
	float weightnorm=0;
	for(size_t bin=1;bin<bins_.size()-1;bin++){
		float weight=getBinContent(bin,sys);
		weightnorm+=weight;
	}
	if(weightnorm==0)return 0; //protect against empty or not well defined
	//calculate it once for performance reasons
	for(size_t binj=1;binj<bins_.size()-1;binj++){
		sumweightedbins+= getBinCenter(binj)*getBinContent(binj,sys);///(weightnorm*weightnorm);
	}

	for(size_t bin=1;bin<bins_.size()-1;bin++){
		//weight is stat unc
		float weight=getBinContent(bin,sys);
		float xval=getBinCenter(bin);
		float stat2=getBin(bin,sys).getStat2();

		float weightnormminus=weightnorm-weight;

		float dmdcontent=xval*weightnormminus;/// (weightnorm*weightnorm);//derivate of this part
		dmdcontent-=sumweightedbins; //subtract all
		dmdcontent+=xval*weight;///(weightnorm*weightnorm);//add only this bin

		dmdcontent*=dmdcontent;
		sumweightedunc+=  dmdcontent * stat2;
	}
	float mean=sumweightedbins/weightnorm;
	err=std::sqrt(sumweightedunc)/(weightnorm*weightnorm);//factored out the weightnorm, just divide once
	return mean;
}




//protected

TString histo1D::stripVariation(const TString &in) const{
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
float histo1D::getDominantVariationUp( const TString&  sysname, const size_t& bin, bool& anticorr) const{ //copy on purpose
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
	return getMaxVar(true,up,down,anticorr);

}
float histo1D::getDominantVariationUp(const TString&  sysname, const size_t& bin)const{
	bool dummy;
	return getDominantVariationUp(sysname,bin,dummy);
}
float histo1D::getDominantVariationDown(const TString&  sysname, const size_t& bin, bool& anticorr) const{//copy on purpose
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

	return getMaxVar(false,up,down,anticorr);
}
float histo1D::getDominantVariationDown( const TString&  sysname, const size_t& bin)const{
	bool dummy;
	return getDominantVariationDown(sysname,bin,dummy);
}
/**
 * deletes all syst and creates manual entry at indices 0 and 1
 * and deletes stat uncertainty in nominal entry
 */
void histo1D::createManualError(){
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


void histo1D::setOperatorDefaults(){
	histoContent::addStatCorrelated=false;
	histoContent::subtractStatCorrelated=false;
	histoContent::divideStatCorrelated=true;
	histoContent::multiplyStatCorrelated=true;
}

histo1D histo1D::createRandom(size_t nbins,size_t distr,size_t n,size_t seed){
	std::vector<float> bins(nbins,0);
	for(size_t i=0;i<nbins;i++)
		bins.at(i)=i;
	histo1D out(bins);
	TRandom3 *r = new TRandom3(seed);
	double mean=nbins/2;
	for(size_t i=0;i<n;i++){
		float rand=0;
		if(distr==0)
			rand= r->Gaus(mean,mean/5);
		else if(distr==1)
			rand=r->BreitWigner(mean,mean/10);
		else if(distr==2)
			rand= r->Poisson(mean);
		out.fill(rand);
	}
	delete r;
	return out;
}

std::vector<float> histo1D::createBinning(size_t nbins, float first, float last){
	std::vector<float> out;
	float div=(last-first)/(float)(nbins);
	float current=first;
	for(size_t i=0;i<nbins;i++){
		out.push_back(current);
		current+=div;
	}
	out.push_back(last);
	return out;
}

void histo1D::equalizeSystematics(std::vector<histo1D>& lhs,std::vector<histo1D>& rhs){
	if(rhs.size() <1 || rhs.size() < 1){
		throw std::logic_error("static container1D::equalizeSystematics: rhs or lhs is empty");
	}
	for(size_t i=0;i<lhs.size();i++){
		lhs.at(i).equalizeSystematicsIdxs(rhs.at(0));
	}
	// after this->containers_.at(0) now has a superset of syst,
	// it will drive the new ordering of rhs
	for(size_t i=0;i<rhs.size();i++){
		rhs.at(i).equalizeSystematicsIdxs(lhs.at(0));
	}
}
void histo1D::adaptSystematicsIdxs(const histo1D& rhs){
	if(hasSameLayerOrdering(rhs))
		return;
	histo1D cp=*this;
	contents_.removeAdditionalLayers();
	for(size_t i=0;i<rhs.contents_.layerSize();i++)
		contents_.addLayer(rhs.getSystErrorName(i));
	//ordering ok
	std::vector<size_t> used;
	for(size_t i=0;i<rhs.contents_.layerSize();i++){
		const TString& namerhs=rhs.getSystErrorName(i);
		size_t oldidx=cp.contents_.getLayerIndex(namerhs);
		if(oldidx<cp.contents_.layerSize()){
			contents_.getLayer(i)=cp.contents_.getLayer(oldidx);
			used.push_back(oldidx);
		}
	}
	if(used.size() == cp.contents_.layerSize())
		return; //all ok

	std::string errstr="histo1D::adaptSystematicsIdxs: rhs need to incorporate all syst that *this has (and opt more)\n not found: ";

	for(size_t i=0;i<cp.contents_.layerSize();i++)
		if(std::find(used.begin(),used.end(),cp.contents_.getLayerName(i)) == used.end())
			errstr+=(cp.contents_.getLayerName(i)+" ").Data();

	throw std::logic_error(errstr);
}

void histo1D::copyFrom(const histo1D& c){

	manualerror_=c.manualerror_;
	bins_=c.bins_;
	contents_=c.contents_;
	binnames_=c.binnames_;
	mergeufof_=c.mergeufof_;
	wasunderflow_=c.wasunderflow_;
	wasoverflow_=c.wasoverflow_;
	//divideBinomial_=c.divideBinomial_;
	name_=c.name_, xname_=c.xname_, yname_=c.yname_;

	gp_=0;
	hp_=0;



}

histo1D histo1D::append(const histo1D& rhs)const{
	if(bins_.size()<2) //there is nothing in this
		return rhs;
	histo1D rhsc=rhs;
	histo1D out=*this;
	out.equalizeSystematicsIdxs(rhsc);
	histo1D equlsdoutcp=out;

	std::vector<float> newbins(out.bins_.begin()+1,out.bins_.end()); //no UF
	float maxbinb=newbins.at(newbins.size()-1);
	float relOF=0;

	relOF=fabs(maxbinb-newbins.at(0))/100;
	newbins.push_back(maxbinb+relOF);

	float minrhs=*std::min_element(rhsc.bins_.begin(),rhsc.bins_.end());

	for(size_t i=1;i<rhsc.bins_.size();i++){ //no UF

		newbins.push_back(maxbinb +2*relOF+ rhsc.bins_.at(i)-minrhs);
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

void histo1D::loadFromTree(TTree *t, const TString & plotname){
	if(!t || t->IsZombie()){
		throw std::runtime_error("container1D::loadFromTree: tree not ok");
	}
	ztop::histo1D * cuftemp=0;
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
void histo1D::loadFromTFile(TFile *f, const TString & plotname){
	if(!f || f->IsZombie()){
		throw std::runtime_error("container1D::loadFromTFile: file not ok");
	}
	AutoLibraryLoader::enable();
	TTree * ttemp = (TTree*)f->Get("container1Ds");
	loadFromTree(ttemp,plotname);
	delete ttemp;
}
void histo1D::loadFromTFile(const TString& filename,
		const TString & plotname){
	AutoLibraryLoader::enable();
	TFile * ftemp=new TFile(filename,"read");
	loadFromTFile(ftemp,plotname);
	delete ftemp;
}

void histo1D::writeToTree(TTree *t){
	if(!t || t->IsZombie()){
		throw std::runtime_error("container1D::writeToTree: tree not ok");
	}
	ztop::histo1D * cufpointer=this;
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
void histo1D::writeToTFile(TFile *f){
	if(!f || f->IsZombie()){
		throw std::runtime_error("container1D::writeToTFile: file not ok");
	}
	f->cd();
	AutoLibraryLoader::enable();
	TTree * ttemp = (TTree*)f->Get("container1Ds");
	if(!ttemp || ttemp->IsZombie())//create
		ttemp = new TTree("container1Ds","container1Ds");
	writeToTree(ttemp);
	delete ttemp;
}
void histo1D::writeToTFile(const TString& filename){
	TFile * ftemp=new TFile(filename,"update");
	if(!ftemp || ftemp->IsZombie()){
		delete ftemp;
		ftemp=new TFile(filename,"create");
	}

	writeToTFile(ftemp);
	delete ftemp;
}



void histo1D::writeToFile(const std::string& filename)const{
	std::ofstream saveFile;
	saveFile.open(filename.data(), std::ios_base::binary | std::ios_base::trunc | std::fstream::out );
	{
		boost::iostreams::filtering_ostream out;
		boost::iostreams::zlib_params parms;
		//parms.level=boost::iostreams::zlib::best_speed;
		out.push(boost::iostreams::zlib_compressor(parms));
		out.push(saveFile);
		{
			writeToStream(out);
		}
	}
	saveFile.close();
}
void histo1D::readFromFile(const std::string& filename){
	std::ifstream saveFile;
	saveFile.open(filename.data(), std::ios_base::binary | std::fstream::in );
	{
		boost::iostreams::filtering_istream in;
		boost::iostreams::zlib_params parms;
		//parms.level=boost::iostreams::zlib::best_speed;
		in.push(boost::iostreams::zlib_decompressor(parms));
		in.push(saveFile);
		{
			readFromStream(in);
		}
	}
	saveFile.close();


}




}//namespace
