/*
 * histoBin.h
 *
 *  Created on: Aug 26, 2013
 *      Author: kiesej
 */

#ifndef HISTOBIN_H_
#define HISTOBIN_H_
#include <vector>
#include "TString.h"
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <stdint.h>
#include "serialize.h"

namespace ztop{
/**
 *
 */
class histoBin{
public:
	histoBin():content_(0),entries_(0),stat2_(0){}//,name_(""){}
	histoBin(const TString& name):content_(0),entries_(0),stat2_(0){}//,name_(name){
	//cutName();
	//}
	~histoBin(){}
	histoBin(const histoBin& rhs){
		copyFrom(rhs);
	}
	histoBin & operator=(const histoBin& rhs){
		copyFrom(rhs);
		return *this;
	}
	histoBin & operator+=(const histoBin& rhs);
	histoBin  operator+(const histoBin& rhs)const;
	void add(const histoBin&);


	const float & getContent() const {return content_;}
	float getStat()            const {return std::sqrt(stat2_);}
	const uint32_t & getEntries() const {return entries_;}
	const float & getStat2()   const {return stat2_;}

	void setContent(const float &);
	void setEntries(const uint32_t &);
	void setStat(const float &);
	void setStat2(const float &);

	void clear();

	void addToContent(const float &);
	void addToStat(const float &);
	void addToStat2(const float &);
	void addEntry();


	//void setName(const TString & name){name_=name;cutName();}
	//const TString & getName() const {return name_;}

	void multiply(const float&);
	void sqrt();


	bool operator == (const histoBin&) const;
	bool operator != (const histoBin&) const;


#ifndef __CINT__
	template <class T>
	void writeToStream(T & stream)const{
		IO::serializedWrite(content_,stream);
		IO::serializedWrite(entries_,stream);
		IO::serializedWrite(stat2_,stream);
	}
	template <class T>
	void readFromStream(T & stream){
		IO::serializedRead(content_,stream);
		IO::serializedRead(entries_,stream);
		IO::serializedRead(stat2_,stream);
	}
#endif

private:

	void cutName();
	void copyFrom(const histoBin&);
	float content_;
	uint32_t entries_;
	float stat2_;
	//TString name_;
};


inline void histoBin::setContent(const float & val){
	content_=val;
}
inline void histoBin::setEntries(const uint32_t & val){
	entries_=val;
}
inline void histoBin::setStat(const float & val){
	stat2_=val*val;
}
inline void histoBin::setStat2(const float & val){
	stat2_=val;
}
/**
 * add linearly
 */
inline void histoBin::addToContent(const float & val){
	content_+=val;
}
/**
 * adds to stat
 * internally this is done in sqares, so whenever possible use addToStat()
 * and NOT something like setStat(sqrt(getStat**2 + something**2))
 */
inline void histoBin::addToStat(const float & val){
	stat2_+=val*val;
}
inline void histoBin::addToStat2(const float & val){
	stat2_+=val;
}
inline void histoBin::addEntry(){
	entries_++;
}


#ifndef __CINT__
namespace IO{
template<class T>
inline void serializedWrite(const histoBin&c, T&stream){
	c.writeToStream(stream);
}
template<class T>
inline void serializedRead(histoBin&c, T&stream){
	c.readFromStream(stream);
}
//re-instantiate vector ops

ZTOP_IO_SERIALIZE_SPECIALIZEVECTORS(histoBin)
}
#endif
//top down to vertical
/*
 * systNames here, operators here?, final class to be filled in container
 *
 * stat are always absolute (~sqrt(n))
 *
 * only simple protection on some functions that are not important in terms of performance
 *
 * all operations (add, subtract, multiply, divide) use parallelization
 */
class histoBins{
public:
	histoBins(){}//: name_(""),layer_(-1){}
	histoBins(size_t Size); //defines size
	~histoBins(){}
	void setSize(size_t);
	bool resize(const size_t & newsize);
	histoBins(const histoBins& rhs){
		copyFrom(rhs);
	}
	histoBins & operator=(const histoBins& rhs){
		copyFrom(rhs);
		return *this;
	}

	histoBin & getBin(size_t binno){return bins_[binno];}//not protected
	const histoBin & getBin(size_t binno) const {return bins_[binno];}
	size_t size() const {return bins_.size();}

	/*
	void setName(const TString & name);
	const TString & getName(){return name_;}
	void setLayer(const int& Layer){layer_=Layer;}
	int getLayer() const {return layer_;}
	 */
	int add(const histoBins&,bool statCorr);
	int subtract(const histoBins&,bool statCorr);
	int divide(const histoBins&,bool statCorr);
	int multiply(const histoBins&,bool statCorr);

	//get sqrt of everything
	void sqrt();

	void multiply(const float& );
	void removeStat();

	bool operator == (const histoBins&) const;
	bool operator != (const histoBins&) const;

	/**
	 * compares the content, ignores names
	 */
	bool equalContent(const histoBins&,float epsilon=0) const;

	/**
	 * compares two histoContent objects.
	 * returns true if the relative difference in all bins
	 * is below epsilon * stat.
	 */
	//TBI	bool isApproxEqual(const histoBins&,double epsilon=1e-6) const;

	static bool showwarnings;

#ifndef __CINT__
	template <class T>
	void writeToStream(T & stream)const{
		IO::serializedWrite(bins_,stream);
	}
	template <class T>
	void readFromStream(T & stream){
		IO::serializedRead(bins_,stream);
	}
#endif


private:
	std::vector<histoBin> bins_;
	//TString name_;
	//int layer_;

	void copyFrom(const histoBins&);
};
/**
 * not protected, fast
 */


#ifndef __CINT__
namespace IO{
template<class T>
inline void serializedWrite(const histoBins&c, T&stream){
	c.writeToStream(stream);
}
template<class T>
inline void serializedRead(histoBins&c, T&stream){
	c.readFromStream(stream);
}
//re-instantiate vector ops
ZTOP_IO_SERIALIZE_SPECIALIZEVECTORS(histoBins)
}
#endif


}//namespace



#endif /* HISTOBIN_H_ */
