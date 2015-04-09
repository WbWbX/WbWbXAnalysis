/*
 * topDecaySelector.cc
 *
 *  Created on: Apr 7, 2015
 *      Author: kiesej
 */




#include "../interface/topDecaySelector.h"
#include <iostream>
#include <stdexcept>

namespace ztop{

bool topDecaySelector::debug=false;

//C++ initialize all members here that need an initial value. keep ordering
topDecaySelector::topDecaySelector(): partonshower_(ps_pythia6),incollectionp_(0)
{
	// I usually put this kind of debug ouput on top of each function call, it makes debugging
	// much easier. Also in case it is done long after the class was developed
	if(debug)
		std::cout << "topDecaySelector::topDecaySelector(): constructor" << std::endl;

}

topDecaySelector::~topDecaySelector(){
	//nothing to be done here
}


//C++: initializer should be defined
topDecaySelector::topDecaySelector(const topDecaySelector&rhs){
	copyFrom(rhs);
}

//C++: equal operator should be defined. return *this (to allow a=b=c;)
topDecaySelector& topDecaySelector::operator = (const topDecaySelector&rhs){
	if(this==&rhs) return *this;
	copyFrom(rhs);
	return *this;
}


//this is where the magic happens
void  topDecaySelector::process(){

	// whenever something goes wrong, please throw exceptions with a proper
	// error output. examples below
	std::string errstr="topDecaySelector::process: has to be implemented. ";
	errstr+=" additional info (index, etc)";
	throw std::logic_error(errstr);

	if(!incollectionp_){// pointer 0 -> real check, no example! ;)
		throw std::logic_error("topDecaySelector::process: input collection not defined");
	}


}








////////////// private member functions //////////////////

void topDecaySelector::copyFrom(const topDecaySelector& rhs){
	partonshower_=rhs.partonshower_;
	incollectionp_=rhs.incollectionp_;
	finalstateleptons_=rhs.finalstateleptons_;
}


}
