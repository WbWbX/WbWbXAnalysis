/*
 * texTabler.h
 *
 *  Created on: Jun 27, 2014
 *      Author: kiesej
 */

#ifndef TEXTABLER_H_
#define TEXTABLER_H_

#include <sstream>
#include "TString.h"
#include <stdexcept>
#include "formatter.h"

namespace ztop{

class texLine{
public:
	texLine():multi_(1){}
	texLine(size_t multi):multi_(multi){}

	size_t getMulti(){return multi_;}



	TString getTex()const;

private:
	size_t multi_;

};

/**
 * reads style, blabla
 * no extra style class, just make it plain
 * can be set in c++ manner
 * no graph interface - make class that inherits for these functions
 */
class texTabler{
public:


	texTabler();
	/**
	 * this works as standard tex
	 * so you just pass a string a la
	 * "| c | l | r |"
	 *
	 */
	texTabler(const TString& rowformat);
	~texTabler(){}

	void format(const TString& str);

	void setTopLine(const texLine &tl){toplines_=tl;}
	void setBottomLine(const texLine &tl){bottomlines_=tl;}


	/** adds an entry
	 * jumps to next row if columns are full
	 *
	 */
	template <class T>
	texTabler& operator << (T);

	texTabler& operator << (const texLine&);

	/**
	 * returns if the next addition of an entry will trigger a new row
	 */
	bool newRow()const{return addnewrow_;}

	/**
	 * to be used e.g. after multicolumn; Dont use otherwise!
	 */
	void newLine();

	TString getTable()const;

	void writeToFile(const TString& )const;
	void writeToPdfFile(const TString& )const;

	/**
	 * resets entries, not format
	 */
	void clear();
private:



	size_t columns_;
	texLine toplines_;
	texLine bottomlines_;

	std::vector<texLine> rowboundaries_;

	TString texformat_;


	std::vector<TString> entries_;

	size_t linecount_;

	bool addnewrow_;

};

///templates

template <class T>
texTabler& texTabler::operator << (T in){
	if(columns_<1)
		throw std::logic_error("texTabler::operator <<: you cannot fill a table without defining columns first");
	std::ostringstream s;
	s << in;
	TString out(s.str());
	formatter fmt;
	out=fmt.makeTexCompatible(out);
	entries_.push_back(out);
	if(addnewrow_){
		linecount_++;
		rowboundaries_.push_back(texLine(0));
	}

	if( (entries_.size() +1) % columns_ ==0)
		addnewrow_=true;
	else
		addnewrow_=false;


	return *this;
}

}



#endif /* TEXTABLER_H_ */
