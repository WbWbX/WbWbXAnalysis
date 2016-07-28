/*
 * semiconst.h
 *
 *  Created on: 20 Jul 2016
 *      Author: jkiesele
 */

#ifndef TTZANALYSIS_TOOLS_INTERFACE_SEMICONST_H_
#define TTZANALYSIS_TOOLS_INTERFACE_SEMICONST_H_
#include <stdexcept>

namespace ztop{
template<class T>
class semiconst {
public:
	explicit semiconst(const T& t):t_(t),locked_(true){}
	operator T()const {return t_;}


	void lock(){locked_=true;}
	void unlock(){locked_=false;}

	semiconst& operator = (const T& rhs){
		if(locked_)
			throw std::logic_error("semiconst: operator = variable locked");
		t_=rhs;
		lock();
		return *this;
	}

private:
	T t_;
	bool locked_;
};


}


#endif /* TTZANALYSIS_TOOLS_INTERFACE_SEMICONST_H_ */
