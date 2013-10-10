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

	size_t size() const {return nominal_.size();}
	size_t layerSize() const {return additionalbins_.size();}
	bool resizeBins(const size_t & newsize);

	size_t addLayer(const TString &); //pushes back
	size_t addLayer(const TString &, const histoBins & ); //pushes back
	size_t addLayerFromNominal(const TString &, const histoContent &); //pushes back
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


	void removeAdditionalLayers(){additionalbins_.clear();layermap_.clear();}

	histoBin & getBin(size_t ,const int &layer=-1);
	const histoBin & getBin(size_t ,const int &layer=-1) const;

	void setBinContent(const size_t & ,const double &,const int &layer=-1);
	const double & getBinContent(const size_t & binno,const int &layer=-1) const;

	void setBinStat(const size_t & binno,const double &stat,const int &layer=-1);
	double getBinStat(const size_t & binno,const int &layer=-1) const;

	void setBinEntries(const size_t & binno,const double &stat,const int &layer=-1);
	const size_t & getBinEntries(const size_t & binno,const int &layer=-1) const;

	void clear();

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

	histoContent & operator *= (const double &);
	histoContent operator * (const double &);

	void cout() const;

	bool hasSameLayerMap(const histoContent & hc) const{return layermap_==hc.layermap_;}

private:
	histoBins nominal_;
	std::vector<histoBins> additionalbins_;
	indexMap<TString> layermap_;

	std::map<size_t,size_t> addLayers(const histoContent & rhs);

};
/**
 * not protected wrt to no range and layer range
 */
inline histoBin & histoContent::getBin(size_t no,const int &layer){
	if(layer<0)
		return nominal_.getBin(no);
	else
		return additionalbins_.at((size_t)layer).getBin(no);
}
inline const histoBin & histoContent::getBin(size_t no,const int &layer) const{
	if(layer<0)
		return nominal_.getBin(no);
	else
		return additionalbins_.at((size_t)layer).getBin(no);
}

inline void histoContent::setBinContent(const size_t & binno,const double &content,const int &layer){
	getBin(binno,layer).setContent(content);
}
inline const double & histoContent::getBinContent(const size_t & binno,const int &layer) const{
	return getBin(binno,layer).getContent();
}
inline void histoContent::setBinStat(const size_t & binno,const double &stat,const int &layer){
	getBin(binno,layer).setStat(stat);
}
inline double  histoContent::getBinStat(const size_t & binno,const int &layer) const{
	return getBin(binno,layer).getStat();
}
inline void histoContent::setBinEntries(const size_t & binno,const double &stat,const int &layer){
	getBin(binno,layer).setEntries(stat);
}
inline const size_t & histoContent::getBinEntries(const size_t & binno,const int &layer) const{
	return getBin(binno,layer).getEntries();
}

}//namesp


#endif /* HISTOCONTENT_H_ */
