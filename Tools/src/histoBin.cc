/*
 * histoBin.cc
 *
 *  Created on: Aug 26, 2013
 *      Author: kiesej
 */
#include "../interface/histoBin.h"
#include <iostream>
#include <stdexcept>
#include <stdint.h>
namespace ztop{

/**
 * clears stat, entries, and content.
 * keeps name
 */
void histoBin::clear(){
	content_=0;
	entries_=0;
	stat2_=0;
}

void  histoBin::add(const histoBin&rhs){
	content_+=rhs.content_;
	stat2_+=rhs.stat2_;
	entries_+=rhs.entries_;
}

void histoBin::multiply(const float&val){
	content_*=val;
	stat2_=stat2_*val*val;
	entries_*=val;
}
void histoBin::sqrt(){
	if(content_ < 0){
		throw std::logic_error("histoBin::sqrt: content < 0");
	}
	if(stat2_ < 0){
		throw std::logic_error("histoBin::sqrt: stat2_ < 0 !!! also check for other bugs!");
	}
	content_ = std::sqrt(content_);
	stat2_   = std::sqrt(stat2_);
	//entries unchanged
}

bool histoBin::operator != (const histoBin& rhs)const{
	if(entries_!=rhs.entries_) return true;
	if(content_!=rhs.content_) return true;
	if(stat2_!=rhs.stat2_) return true;
	//if(name_!=rhs.name_) return true;
	return false;
}
bool histoBin::operator == (const histoBin& rhs)const{
	return !(*this!=rhs);
}

/*
void histoBin::cutName(){
	if(name_.Length() > 65535){
		std::cout << "histoBin: warning: bin name length than 16bit, will be chopped"<<std::endl;
		name_=TString(name_.Data(),65535);
	}
}
 */
void histoBin::copyFrom(const histoBin& rhs){
	if(&rhs == this) return;
	content_=rhs.content_;
	entries_=rhs.entries_;
	stat2_=rhs.stat2_;
	//name_=rhs.name_;
}

/////binning//////

bool histoBins::showwarnings=false;
/*
void histoBins::setName(const TString & name){
	name_=name;
	if(name_.Length() > UINT16_MAX){
		std::cout << "histoBins: warning: bin name length than 16bit, will be chopped"<<std::endl;
		name_=TString(name_.Data(),65535);
	}
}
 */
histoBins::histoBins(size_t Size){// : name_(""),layer_(-1){
	setSize(Size);
}
void histoBins::setSize(size_t Size){
	bins_.clear();
	histoBin hb;
	bins_.resize(Size,hb);
}
/**
 * returns true if bins were added
 */
bool histoBins::resize(const size_t & newsize){
	size_t oldsize=bins_.size();
	bins_.resize(newsize);
	return bins_.size()>oldsize;
}

/**
 * histoBins::add(const histoBins& rhl,bool statCorr)
 * warning: no warnings here!
 * if success returns 0, otherwise negative value
 */
int histoBins::add(const histoBins& rhs,bool statCorr){
	if(size()!=rhs.size())
		return -1;
	for(size_t i=0;i<size();i++){
		getBin(i).addToContent(rhs.getBin(i).getContent());
		float staterr=0;
		if(statCorr){
			staterr=getBin(i).getStat()+rhs.getBin(i).getStat();
			getBin(i).setStat(staterr);
		}
		else{
			getBin(i).addToStat2(rhs.getBin(i).getStat2());
		}
		getBin(i).setEntries(getBin(i).getEntries()+rhs.getBin(i).getEntries());
	}
	return 0;
}
/**
 * histoBins::subtract(const histoBins& rhl,bool statCorr)
 * warning: no warnings here!
 * if success returns 0, otherwise negative value
 */
int histoBins::subtract(const histoBins& rhs,bool statCorr){
	if(size()!=rhs.size())
		return -1;
	for(size_t i=0;i<size();i++){
		getBin(i).setContent(getBin(i).getContent()-rhs.getBin(i).getContent());
		float staterr=0;
		if(statCorr){
			staterr=getBin(i).getStat2()-rhs.getBin(i).getStat2();
			getBin(i).setStat2(fabs(staterr));
		}
		else{
			getBin(i).addToStat2(rhs.getBin(i).getStat2());
		}
		getBin(i).setEntries(getBin(i).getEntries()+rhs.getBin(i).getEntries());
	}
	return 0;
}
/**
 * histoBins::divide(const histoBins& rhl,bool statCorr)
 * warning: no warnings here!
 * if success returns 0, otherwise negative value
 */
int histoBins::divide(const histoBins& rhs,bool statCorr){
	if(size()!=rhs.size())
		return -1;
	for(size_t i=0;i<size();i++){
		bool divbinstatcorr=statCorr;
		float div=0;
		if(rhs.getBin(i).getContent()==0){
			if(showwarnings) std::cout << "histoBins::divide: 0 content in bin " << i << std::endl;
			div=0;
		}
		else{
			div=getBin(i).getContent()/rhs.getBin(i).getContent();
		}
		float staterr2=0;

		if(divbinstatcorr){
			if(rhs.getBin(i).getContent())
				staterr2=fabs(div*(1-div)/rhs.getBin(i).getContent());
			else
				staterr2=0; //nan safe
		}
		else{
			if(rhs.getBin(i).getContent())
				staterr2=getBin(i).getStat2()/(rhs.getBin(i).getContent()*rhs.getBin(i).getContent()) + div*div*rhs.getBin(i).getStat2()/(rhs.getBin(i).getContent()*rhs.getBin(i).getContent());
			else staterr2=0; //nan safe
		}
		getBin(i).setStat2(staterr2);
		getBin(i).setContent(div);
		getBin(i).setEntries(getBin(i).getEntries()+rhs.getBin(i).getEntries());
	}
	return 0;
}

void histoBins::sqrt(){
	for(size_t i=0;i<size();i++){
		try{
			getBin(i).sqrt();
		}catch(...){
			throw std::logic_error("histoBins::sqrt: contents or stat with negative values");
		}
	}
}

/**
 * histoBins::multiply(const histoBins& rhl,bool statCorr)
 * warning: no warnings here!
 * if success returns 0, otherwise negative value
 */
int histoBins::multiply(const histoBins& rhs,bool statCorr){
	if(size()!=rhs.size())
		return -1;
	for(size_t i=0;i<size();i++){
		const float & ca=getBin(i).getContent();
		const float & cb=rhs.getBin(i).getContent();
		const float & ea2=getBin(i).getStat2();
		const float & eb2=rhs.getBin(i).getStat2();

		float mult=ca*cb;
		float staterr2=0;
		if(statCorr){
			float ea=std::sqrt(ea2);
			float eb=std::sqrt(eb2);
			staterr2=((ca+ea)*(cb+eb) - mult)*((ca+ea)*(cb+eb) - mult);
		}
		else{

			staterr2=ca*ca*eb2+cb*cb*ea2;
		}
		getBin(i).setStat2(fabs(staterr2));
		getBin(i).setContent(mult);
		getBin(i).setEntries(getBin(i).getEntries() * rhs.getBin(i).getEntries());
	}
	return 0;
}
void histoBins::multiply(const float& val){
	for(size_t i=0;i<size();i++)
		bins_[i].multiply(val);
}

void histoBins::removeStat(){
	for(size_t i=0;i<size();i++)
		bins_[i].setStat2(0);
}

bool histoBins::operator !=(const histoBins& rhs) const{
	if(bins_!=rhs.bins_) return true;
	return false;
}
bool histoBins::operator ==(const histoBins& rhs) const{
	return !(*this!=rhs);
}



bool histoBins::equalContent(const histoBins&rhs,float epsilon) const{
	if(bins_.size() != rhs.bins_.size())
		return false;
	if(!epsilon){
		if(bins_!=rhs.bins_) return false;
		return true;
	}
	else{
		for(size_t i=0;i<bins_.size();i++){
			float diff = fabs((bins_.at(i).getContent() - rhs.bins_.at(i).getContent())/bins_.at(i).getContent());
			diff /= epsilon* (bins_.at(i).getStat() / fabs(bins_.at(i).getContent()));
			if(diff > 1)
				return false;
		}
		return true;
	}
}
void histoBins::copyFrom(const histoBins&rhs){
	if(&rhs==this) return;
	bins_=rhs.bins_;
	//name_=rhs.name_;
	//layer_=rhs.layer_;

}

}//namespace

