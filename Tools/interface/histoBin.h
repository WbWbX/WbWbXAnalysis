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

namespace ztop{
/**
 *
 */
class histoBin{
public:
	histoBin():content_(0),entries_(0),stat2_(0),name_(""){}
	histoBin(TString name):content_(0),entries_(0),stat2_(0),name_(name){}
	~histoBin(){}

	const double & getContent() const {return content_;}
	double getStat()            const {return sqrt(stat2_);}
	const size_t & getEntries() const {return entries_;}
	const double & getStat2()   const {return stat2_;}

	void setContent(const double &);
	void setEntries(const int &);
	void setStat(const double &);
	void setStat2(const double &);

	void addToContent(const double &);
	void addToStat(const double &);
	void addEntry();

	void setName(const TString & name){name_=name;}
	const TString & getName() const {return name_;}

	void multiply(const double&);

private:
	double content_;
	size_t entries_;
	double stat2_;
	TString name_;
};

inline void histoBin::setContent(const double & val){
	content_=val;
}
inline void histoBin::setEntries(const int & val){
	entries_=val;
}
inline void histoBin::setStat(const double & val){
	stat2_=val*val;
}
inline void histoBin::setStat2(const double & val){
	stat2_=val;
}
/**
 * add linearly
 */
inline void histoBin::addToContent(const double & val){
	content_+=val;
}
/**
 * adds to stat
 * internally this is done in sqares, so whenever possible use addToStat()
 * and NOT something like setStat(sqrt(getStat**2 + something**2))
 */
inline void histoBin::addToStat(const double & val){
	stat2_+=val*val;
}
inline void histoBin::addEntry(){
	entries_++;
}


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
	histoBins(): name_(""),layer_(-1){}
	histoBins(size_t Size); //defines size
	~histoBins(){}
	void setSize(size_t);
	bool resize(const size_t & newsize);

	histoBin & getBin(size_t binno){return bins_[binno];}//not protected
	const histoBin & getBin(size_t binno) const {return bins_[binno];}
	size_t size() const {return bins_.size();}


	void setName(const TString & name){name_=name;}
	const TString & getName(){return name_;}
	void setLayer(int Layer){layer_=Layer;}
	int getLayer() const {return layer_;}

	int add(const histoBins&,bool statCorr);
	int subtract(const histoBins&,bool statCorr);
	int divide(const histoBins&,bool statCorr);
	int multiply(const histoBins&,bool statCorr);

	void multiply(const double& );

	static bool showwarnings;

private:
	std::vector<histoBin> bins_;
	TString name_;
	int layer_;
};
/**
 * not protected, fast
 */




}//namespace



#endif /* HISTOBIN_H_ */
