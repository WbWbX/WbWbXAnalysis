/*
 * selectionModes.h
 *
 *  Created on: Mar 4, 2015
 *      Author: kiesej
 */

#ifndef SELECTIONMODES_H_
#define SELECTIONMODES_H_

#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <algorithm>


namespace ztop{

//helper
class selectionModeBase{
public:
	selectionModeBase(const std::string&);
	virtual  ~selectionModeBase();
	virtual void readConfigString(const std::string &str){}

	static void readConfigStringForAll(const std::string &str);
protected:
	selectionModeBase(){}
	std::string getAssoVal(const std::string &str)const;
	std::string modeid_;

private:
	bool alreadyUsed()const;
	static std::vector<std::string> allids_;
	static std::vector<selectionModeBase*> allselmodes_;
};


// contrary to first ideas, this gos to the inheriting analysers that also implement
// the event loop
template <class T>
class selectionMode : public selectionModeBase{
public:
	selectionMode(const std::string& modeid):selectionModeBase(modeid){

	}

	const T& val()const{return val_;}

	/**
	 * includes double-definition check
	 * string should be formatted:
	 *
	 * <modeid>(<value>)
	 *
	 */
	void readConfigString(const std::string &str){
		//str.find()
		std::string substr=getAssoVal(str);
		std::stringstream ss(substr);
		ss >> val_;
	}


private:
	selectionMode():selectionModeBase(){}

	//does the hard work



	T val_;

};

template<>
void selectionMode<bool>::readConfigString(const std::string &str){
	//str.find()
	std::string substr=getAssoVal(str);
	std::stringstream ss(substr);
	ss >>std::boolalpha>> val_;
}




}//ns


#endif /* SELECTIONMODES_H_ */
