/*
 * histoContent.cc
 *
 *  Created on: Aug 27, 2013
 *      Author: kiesej
 */
#include "../interface/histoContent.h"
#include <omp.h>

namespace ztop{

/**
 * assume statistics of bins to be correlated/uncorrelated when adding
 * histoContent. This also applies for the statistics of the systematic entries
 */
bool histoContent::addStatCorrelated=false;
/**
 * assume statistics of bins to be correlated/uncorrelated when subtracting
 * histoContent. This also applies for the statistics of the systematic entries
 */
bool histoContent::subtractStatCorrelated=false;
/**
 * assume statistics of bins to be correlated/uncorrelated when dividing
 * histoContent. This also applies for the statistics of the systematic entries
 */
bool histoContent::divideStatCorrelated=true;
/**
 * assume statistics of bins to be correlated/uncorrelated when multiplying
 * histoContent. This also applies for the statistics of the systematic entries
 */
bool histoContent::multiplyStatCorrelated=true;

bool histoContent::debug=false;

void histoContent::setOperatorDefaults(){
	addStatCorrelated=false;
	subtractStatCorrelated=false;
	divideStatCorrelated=true;
	multiplyStatCorrelated=true;
}

void histoContent::clear(){
	nominal_.multiply(0);
	layermap_.clear();
	additionalbins_.clear();
}

histoContent::histoContent(size_t nbins){
	nominal_=histoBins(nbins);
}

void histoContent::clearLayerContent(const int & idx){
	if(idx>=(int)layerSize()){
		std::cout << "histoContent::clearLayerContent: index out of range" << std::endl;
	}
	for(size_t i=0;i<nominal_.size();i++)
		getBin(i,idx).setContent(0);
}
void histoContent::clearLayerStat(const int & idx){
	if(idx>=(int)layerSize()){
		std::cout << "histoContent::clearLayerStat: index out of range" << std::endl;
	}
	for(size_t i=0;i<nominal_.size();i++)
		getBin(i,idx).setStat(0);
}
/**
 * returns true if bins were added, returns false if bins were removed
 */
bool histoContent::resizeBins(const size_t & newsize){
	bool added=nominal_.resize(newsize);
	for(size_t i=0;i<layerSize();i++)
		additionalbins_.at(i).resize(newsize);
	return added;
}

/**
 * adds a layer of content with <name> to the existing ones.
 * returns the index of the new layer. layer will be a
 * copy of the content layer (-1) including statistics (is this a mistake?)
 * Typically useful for systematics.
 * if layer already exists it returns the index of the layer
 *
 * check whether a new layer was created: old.layersize() != new.layersize()
 */
size_t histoContent::addLayer(const TString & name){
	if(nominal_.size() < 1){
		std::cout << "histoContent::addLayer: first construct! default constructor is not supposed to be used" <<std::endl;
		return layerSize();
	}
	size_t idx=layermap_.getIndex(name);

	if(idx>=layermap_.size()){ //layer does not exist yet
		histoBins binning(nominal_);
		binning.setName(name);
		binning.setLayer(layermap_.size());
		additionalbins_.push_back(binning);
		return layermap_.push_back(name);
	}
	return idx;
} //this function allows for easy syst adding, loop over both content.layers and create empty layers if not exist. then just add, use mapping of one of them, doesn't matter

size_t histoContent::addLayer(const TString & name, const histoBins & histbins){
	if(nominal_.size() < 1){
		std::cout << "histoContent::addLayer(..histoBins): first construct! default constructor is not supposed to be used" <<std::endl;
		return layerSize();
	}
	if(histbins.size() != nominal_.size()){
		std::cout << "histoContent::addLayer(..histoBins): arguments size does not match. doing nothing" <<std::endl;
		return layerSize();
	}
	size_t idx=layermap_.getIndex(name);

	if(idx>=layermap_.size()){ //layer does not exist yet
		if(debug)
			std::cout << "histoContent::addLayer(..histoBins): adding layer " << name << std::endl;
		histoBins binning(histbins);
		binning.setName(name);
		binning.setLayer(layermap_.size());
		additionalbins_.push_back(binning);
		return layermap_.push_back(name);
	}
	else{
		std::cout << "histoContent::addLayer(..histoBins): layer that should have been filled externally already exists doing nothing " <<std::endl;
		return layerSize();
	}
	return idx;
} //this function allows for easy syst adding, loop over both content.layers and create empty layers if not exist. then just add, use mapping of one of them, doesn't matter



/**
 * adds layer from nominal of external with name.
 * if layer already exists, content gets added according to correlation settings
 */
size_t histoContent::addLayerFromNominal(const TString & name, const histoContent & external){
	if(nominal_.size() < 1){
		std::cout << "histoContent::addLayerFromNominal: first construct! default constructor is not supposed to be used" <<std::endl;
		return 999999999999;
	}
	size_t idx=layermap_.getIndex(name);

	if(idx>=layermap_.size()){ //layer does not exist yet
		if(debug)
			std::cout << "histoContent::addLayerFromNominal: adding layer " << name <<" " << idx << " ";
		histoBins binning(size());
		binning.setName(name);
		binning.setLayer(layermap_.size());
		additionalbins_.push_back(binning);
		idx= layermap_.push_back(name);
		if(debug)
			std::cout << " at idx: " << idx <<std::endl;
	}
	int err=additionalbins_[idx].add(external.nominal_,addStatCorrelated);
	if(err<0)
		std::cout << "histoContent::addLayerFromNominal: binning does not match" <<std::endl;
	return idx;
}


const TString & histoContent::getLayerName(const size_t &lno) const{
	if(lno>=layermap_.size()){
		std::cout << "histoContent::getLayerName: out of range" <<std::endl;
	}
	return layermap_.getData(lno);
}
const size_t & histoContent::getLayerIndex(const TString & name) const{
	return layermap_.getIndex(name);
}
histoBins histoContent::copyLayer(const size_t & idx) const{
	if(idx>=layerSize()){
		std::cout << "histoContent::copyLayer: index out of range, returning nominal" << std::endl;
		return nominal_;
	}
	return additionalbins_.at(idx);
}
histoBins histoContent::copyLayer(const TString & name) const{
	size_t idx=getLayerIndex(name);
	return copyLayer(idx);
}
const histoBins &  histoContent::getLayer(const size_t & idx) const{
	if(idx>=layerSize()){
		std::cout << "histoContent::copyLayer: index out of range" << std::endl;
		return nominal_;
	}
	return additionalbins_.at(idx);
}
const histoBins &  histoContent::getLayer(const TString &name) const{
	size_t idx=getLayerIndex(name);
	return getLayer(idx);
}

histoBins &  histoContent::getLayer(const size_t & idx){
	if(idx>=layerSize()){
		std::cout << "histoContent::copyLayer: index out of range" << std::endl;
		return nominal_;
	}
	return additionalbins_.at(idx);
}
histoBins &  histoContent::getLayer(const TString & name){
	size_t idx=getLayerIndex(name);
	return getLayer(idx);
}

const histoBins &  histoContent::getNominal() const{
	return nominal_;
}
histoBins &  histoContent::getNominal(){
	return nominal_;
}
/**
 * removes a layer. needs lots of performance (see indexMap::erase())
 * probably better to copy one element(s) and clear all layers.
 */
void histoContent::removeLayer(const size_t & idx){
	if(idx >= layerSize()){
		std::cout << "histoContent::removeLayer: out of range" <<std::endl;
		return;
	}
	additionalbins_.erase(additionalbins_.begin()+idx);
	layermap_.eraseByIdx(idx);
}
/**
 * removes a layer. needs lots of performance (see indexMap::erase())
 * probably better to copy one element(s) and clear all layers.
 */
void histoContent::removeLayer(const TString & name){
	size_t idx=getLayerIndex(name);
	removeLayer(idx);
}
/**
 * adds two histoContents.
 * depending on histoContent::addStatCorrelated statistical uncertainties are
 * treated as correlated or uncorrelated.
 * content of lower layers (systematics) is assumed to be correlated if the layers have the same name
 * protection one layer below (fills -1 without warning)
 */
histoContent & histoContent::operator += (const histoContent& rhs){
	if(layermap_.size() > 0 || rhs.layermap_.size() > 0){
		histoContent nrhs=rhs;
		//add layers that are in lhs but not in rhs to rhs
		nrhs.addLayers(*this);
		//add layers that are in rhs but not in lhs to lhs
		//and get association map
		std::map<size_t,size_t> lhstorhs=addLayers(nrhs);
		for(size_t l=0;l<layerSize();l++){
			size_t rhsidx=lhstorhs.find(l)->second;
			additionalbins_[l].add(nrhs.additionalbins_[rhsidx],addStatCorrelated);
		}
	}
	//nominal last! otherwise addLayer gets screwed up and adds wrt to NEW nominal
	if(nominal_.add(rhs.nominal_,addStatCorrelated)<0)
		std::cout << "histoContent::operator += sizes don't match. doing nothing" <<std::endl;
	return *this;
}
histoContent histoContent::operator + (const histoContent& rhs){
	histoContent out=*this;
	out+=rhs;
	return out;
}
histoContent & histoContent::operator -= (const histoContent& rhs){
	histoContent nrhs=rhs;
	//add layers that are in lhs but not in rhs to rhs
	nrhs.addLayers(*this);
	//add layers that are in rhs but not in lhs to lhs
	//and get association map
	std::map<size_t,size_t> lhstorhs=addLayers(nrhs);
	for(size_t l=0;l<layerSize();l++){
		size_t rhsidx=lhstorhs.find(l)->second;
		additionalbins_[l].subtract(nrhs.additionalbins_[rhsidx],subtractStatCorrelated);
	}
	if(nominal_.subtract(rhs.nominal_,subtractStatCorrelated)<0)
		std::cout << "histoContent::operator -= sizes don't match. output won't make sense" <<std::endl;
	return *this;
}
histoContent histoContent::operator - (const histoContent& rhs){
	histoContent out=*this;
	out-=rhs;
	return out;
}
histoContent& histoContent::operator /= (const histoContent& rhs){

	histoContent nrhs=rhs;
	//add layers that are in lhs but not in rhs to rhs
	nrhs.addLayers(*this);
	//add layers that are in rhs but not in lhs to lhs
	//and get association map
	std::map<size_t,size_t> lhstorhs=addLayers(nrhs);
	for(size_t l=0;l<layerSize();l++){
		size_t rhsidx=lhstorhs.find(l)->second;
		additionalbins_[l].divide(nrhs.additionalbins_[rhsidx],divideStatCorrelated);
	}
	//nominal
	if(nominal_.divide(rhs.nominal_,divideStatCorrelated)<0)
		std::cout << "histoContent::operator /= sizes don't match. output won't make sense" <<std::endl;
	return *this;
}
histoContent histoContent::operator / (const histoContent& rhs){
	histoContent out=*this;
	out /= rhs;
	return out;
}
histoContent & histoContent::operator *= (const histoContent& rhs){

	histoContent nrhs=rhs;
	//add layers that are in lhs but not in rhs to rhs
	nrhs.addLayers(*this);
	//add layers that are in rhs but not in lhs to lhs
	//and get association map
	std::map<size_t,size_t> lhstorhs=addLayers(nrhs);
	for(size_t l=0;l<layerSize();l++){
		size_t rhsidx=lhstorhs.find(l)->second;
		additionalbins_[l].multiply(nrhs.additionalbins_[rhsidx],multiplyStatCorrelated);
	}
	//nominal
	if(nominal_.multiply(rhs.nominal_,multiplyStatCorrelated)<0)
		std::cout << "histoContent::operator * sizes don't match. doing nothing" <<std::endl;
	return *this;
}
histoContent histoContent::operator * (const histoContent& rhs){
	histoContent out=*this;
	out*=rhs;
	return out;
}
histoContent & histoContent::operator *= (const double & val){
	nominal_.multiply(val);
	for(size_t i=0;i<layerSize();i++)
		additionalbins_[i].multiply(val);
	return *this;
}
histoContent histoContent::operator * (const double & val){
	histoContent out=*this;
	out*=val;
	return out;
}

void histoContent::cout() const{
	//nominal
	std::cout << "\nnominal " << std::endl;
	for(size_t i=0;i<size();i++){
		std::cout << "bin " <<i << ": " << getBin(i).getContent()  << " pm " << getBin(i).getStat() << std::endl;
	}
	for(size_t layer=0;layer<layermap_.size();layer++){
		std::cout << "\n"<< layermap_.getData(layer) << std::endl;
		for(size_t i=0;i<size();i++){
			std::cout << "bin " <<i << ": " << getBin(i,layer).getContent() << " pm " << getBin(i,layer).getStat()  << std::endl;
		}
	}
}

/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////private member funtions/////////
/////////////////////////////////////////////
/////////////////////////////////////////////

/**
 * if a layer does not exist, it creates new layers by copying the nominal (-1) layer
 * of the this histoContent object;
 * The stat errors from the newly created layers are removed
 * to avoid double counting when checkgin content-syst of resulting histoContent
 *
 * returns mapping of new layers to input histoContent.layers
 */
std::map<size_t,size_t> histoContent::addLayers(const histoContent & rhs){
	std::map<size_t,size_t> outmapping;
	for(size_t i=0;i<rhs.layermap_.size();i++){
		size_t oldsize=layerSize();
		size_t lhsidx=addLayer(rhs.layermap_.getData(i));
		// if the layer was created from nominal, the stats are cleared
		// otherwise it would be double counting when checking content-syst at some point
		if(oldsize!=layerSize())
			clearLayerStat(lhsidx);
		outmapping[lhsidx]=i;
	}
	return outmapping;
}

}//namespace
