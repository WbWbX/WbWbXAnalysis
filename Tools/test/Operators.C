/*
 * Operators.C
 *
 *  Created on: Aug 15, 2013
 *      Author: kiesej
 */
#include <iostream>

class testbase{
public:
	testbase() : mem_(0) {std::cout << "copiedbase" << std::endl;}
	testbase(const testbase& rhs){this->mem_=rhs.mem_;}
	~testbase(){}

	testbase operator- (const testbase & rhs){testbase out=*this;out.mem_-=rhs.mem_;return out;}

	int & getmem(){return mem_;}

protected:
	int mem_;

};

class test : public testbase{
public:
test(): testbase() {std::cout << "copied" << std::endl;}
test(const test& rhs): testbase() {this->mem_=rhs.mem_;}


};

void Operators(){

	test bla,blu;
	std::cout << "operator" <<std::endl;
	testbase * p1=&bla;
	testbase * p2=&blu;

testbase gg=(*p1-*p2);



}


