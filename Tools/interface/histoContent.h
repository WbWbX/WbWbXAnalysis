/*
 * histoContent.h
 *
 *  Created on: Aug 26, 2013
 *      Author: kiesej
 */

#ifndef HISTOCONTENT_H_
#define HISTOCONTENT_H_
#include "histoBin.h"
#include <iostream>
#include <map>
#include <stdexcept>
#include "indexMap.h"
#include "serialize.h"

namespace ztop{

/**
 * collection of bins with n layers with ids and names (for systematics and their stat)
 * performance optimised for layer -1 (content layer) for filling purpose.
 * other layers should be for systematics probably, so less performant
 * each layer contains full content (not only deviations to the nominal layer)
 * and stat errors ~sqrt(content) (depends on filling)
 */
class histoContent{
public:
	histoContent(){}
	histoContent(size_t nbins);
	~histoContent(){}

	histoContent(const histoContent&rhs){
		copyFrom(rhs);
	}

	histoContent& operator=(const histoContent&rhs){
		if(rhs==*this) return *this;
		copyFrom(rhs);
		return *this;
	}

	size_t size() const {return nominal_.size();}
	size_t layerSize() const {return additionalbins_.size();}
	bool resizeBins(const size_t & newsize);

	size_t addLayer(const TString &); //pushes back
	size_t addLayer(const TString &, const histoBins & ); //pushes back
	size_t setLayerFromNominal(const TString &, const histoContent &, float devweight=1); //pushes back
	const TString & getLayerName(const size_t &) const;
	const size_t & getLayerIndex(const TString&) const;

	histoBins copyLayer(const size_t &) const;
	histoBins copyLayer(const TString &) const;

	const histoBins & getLayer(const size_t &) const;
	const histoBins & getLayer(const TString &) const;

	histoBins & getLayer(const size_t & ) ;
	histoBins & getLayer(const TString &) ;

	const histoBins & getNominal() const;
	histoBins & getNominal() ;

	void removeLayer(const size_t &);
	void removeLayer(const TString &);

	void clearLayerContent(const int &);
	void clearLayerStat(const int &);

	/**
	 * returns mapping of layer indices of *this to input histoContent
	 * careful input will get additional layers (copies of its nominal)
	 */
	std::map<size_t,size_t> mergeLayers( histoContent &);

	void removeAdditionalLayers(){additionalbins_.clear();layermap_.clear();}

	histoBin & getBin(size_t ,const int &layer=-1);
	const histoBin & getBin(size_t ,const int &layer=-1) const;

	void setBinContent(const size_t & ,const float &,const int &layer=-1);
	const float & getBinContent(const size_t & binno,const int &layer=-1) const;

	void setBinStat(const size_t & binno,const float &stat,const int &layer=-1);
	float getBinStat(const size_t & binno,const int &layer=-1) const;

	void setBinEntries(const size_t & binno,const float &stat,const int &layer=-1);
	const uint32_t & getBinEntries(const size_t & binno,const int &layer=-1) const;

	void clear();
	void setAllZero();

	static void setOperatorDefaults();

	static bool addStatCorrelated;
	static bool subtractStatCorrelated;
	static bool divideStatCorrelated;
	static bool multiplyStatCorrelated;


	histoContent & operator += (const histoContent&);
	histoContent operator + (const histoContent&);

	histoContent & operator -= (const histoContent&);
	histoContent operator - (const histoContent&);

	histoContent & operator /= (const histoContent&);
	histoContent operator / (const histoContent&);

	histoContent & operator *= (const histoContent&);
	histoContent operator * (const histoContent&);

	histoContent & operator *= (const float &);
	histoContent operator * (const float &);

	bool operator != (const histoContent&) const;
	bool operator == (const histoContent&) const;

	void cout() const;

	bool hasSameLayerMap(const histoContent & hc) const{return layermap_==hc.layermap_;}

	/**
	 * in case layers are named <>_up or <>_down it will give a list of all <>
	 */
	std::vector<TString> getVariations()const;


	static bool debug;

	/*
	 * high level possibilities to parametrize systematics binwise etc.
	 * in development
	 * e.g. possibility to return histcontent for a given value
	 * 	(not nec. one of input variations -> calculated by parameterization)
	 *
	 * assumptions:
	 * 		-systematics are named as <name>_<variation delta>_<up/down>
	 * 		-dependence is linear (for now)
	 * needs:
	 * 	    -map to identify the groups (not necessarily private member)
	 * 	    -way to save parameterization (for each bin) once it is done
	 * 	    -switch to change parameterization (linear, 2nd order, ((3rd order)))
	 * 	    -class to do the fitting
	 * 	    	-fitting takes into account statistic errors on sys var (switch!! on/off - depends on stat corr)
	 *
	 *
	 * 	-----------> maybe better in histo1D....
	 */

	void sqrt();

	/*
	 *
	 */


#ifndef __CINT__
	template <class T>
	void writeToStream(T & stream)const;
	template <class T>
	void readFromStream(T & stream);
#endif

private:
	histoBins nominal_;
	indexMap<TString> layermap_;
	std::vector<histoBins> additionalbins_;

	/**
	 * first map index: layer index of *this
	 * second index: layer index of rhs
	 */
	std::map<size_t,size_t> addLayers(const histoContent & rhs);

	void copyFrom(const histoContent& rhs);
};




/**
 * not protected wrt to no range and layer range
 */
//bool histoContent::debug=false;
inline histoBin & histoContent::getBin(size_t no,const int &layer){
	if(layer<0)
		return nominal_.getBin(no);
	else
		return additionalbins_.at((size_t)layer).getBin(no);
}
inline const histoBin & histoContent::getBin(size_t no,const int &layer) const{
	if(layer<0){
		return nominal_.getBin(no);}
	else{
		if(debug)
			std::cout << "histoContent::getBin: getLayer " << layer << " in " << layerSize() << std::endl;
		return additionalbins_.at((size_t)layer).getBin(no);}
}

inline void histoContent::setBinContent(const size_t & binno,const float &content,const int &layer){
	getBin(binno,layer).setContent(content);
}
inline const float & histoContent::getBinContent(const size_t & binno,const int &layer) const{
	return getBin(binno,layer).getContent();
}
inline void histoContent::setBinStat(const size_t & binno,const float &stat,const int &layer){
	getBin(binno,layer).setStat(stat);
}
inline float  histoContent::getBinStat(const size_t & binno,const int &layer) const{
	return getBin(binno,layer).getStat();
}
inline void histoContent::setBinEntries(const size_t & binno,const float &stat,const int &layer){
	getBin(binno,layer).setEntries(stat);
}
inline const uint32_t & histoContent::getBinEntries(const size_t & binno,const int &layer) const{
	return getBin(binno,layer).getEntries();
}

#ifndef __CINT__
template <class T>
inline void histoContent::writeToStream(T & stream)const{

	//boost::iostreams::filtering_ostream & stream=*streamp;
	//tagged object base part (name, tags..)
	//taggedObject::writeToStream(stream);
	///

	IO::serializedWrite(nominal_,stream);
	size_t nlay=layermap_.size();
	IO::serializedWrite(nlay,stream);
	for(size_t j=0;j<nlay;j++){
		IO::serializedWrite(layermap_.getData(j),stream);
	}

	IO::serializedWrite(additionalbins_,stream);

}

template <class T>
inline void histoContent::readFromStream(T & stream){

	IO::serializedRead(nominal_,stream);
	size_t nlay;
	IO::serializedRead(nlay,stream);
	layermap_.clear();
	for(size_t j=0;j<nlay;j++){
		TString layname;
		IO::serializedRead(layname,stream);
		addLayer(layname);
	}
	IO::serializedRead(additionalbins_,stream);
}

namespace IO{
template<class T>
inline void serializedWrite(const histoContent&c, T&stream){
	c.writeToStream(stream);
}
template<class T>
inline void serializedRead(histoContent&c, T&stream){
	c.readFromStream(stream);
}
//re-instantiate vector ops
ZTOP_IO_SERIALIZE_SPECIALIZEVECTORS(histoContent)

}//io
#endif
}//ztop

#endif /* HISTOCONTENT_H_ */
