/*
 * selectionMode.cc
 *
 *  Created on: Mar 4, 2015
 *      Author: kiesej
 */



#include "../interface/selectionMode.h"

#include <string>
#include <algorithm>

namespace ztop{


std::vector<selectionModeBase*> selectionModeBase::allselmodes_;
std::vector<std::string> selectionModeBase::allids_=std::vector<std::string>();

selectionModeBase::selectionModeBase(const std::string& modeid):modeid_(modeid){
	if(alreadyUsed()){
		std::string errstr="selectionMode: already used this id: "+modeid_;
		throw std::logic_error(errstr);
	}
	allids_.push_back(modeid_);
	allselmodes_.push_back(this);
}

selectionModeBase::~selectionModeBase(){
	std::vector<selectionModeBase*>::iterator it=std::find(allselmodes_.begin(),allselmodes_.end(),this);
	if(it != allselmodes_.end()){
		allselmodes_.erase(it);
	}
}

void selectionModeBase::readConfigStringForAll(const std::string &str){
	for(size_t i=0;i<allselmodes_.size();i++)
		if(allselmodes_.at(i))
			allselmodes_.at(i)->readConfigString(str);
}

bool selectionModeBase::alreadyUsed()const{
	if(std::find(allids_.begin(),allids_.end(),modeid_) != allids_.end())
		return true;
	return false;
}





std::string selectionModeBase::getAssoVal(const std::string &str)const{

	size_t pos = str.find(modeid_+"(");
	if(pos==std::string::npos){
		std::string errstr="selectionMode: mode not found: " + modeid_+ " in " + str;
		throw std::logic_error(errstr);
	}

	pos+=modeid_.length();
	if(str.find(modeid_+"(",pos+1)!= std::string::npos){
		std::string errstr="selectionMode: mode found twice: " + modeid_+ " in " + str;
		throw std::logic_error(errstr);
	}
	/*
	if(str.at(pos) != '('){
		std::string errstr="selectionMode: format (modeID(val)) error in " + str;
		throw std::logic_error(errstr);
	}
	 */
	pos++;
	size_t nextpos=str.find(")",pos);
	if( std::string::npos != nextpos){
		return str.substr( pos, nextpos-pos);
	}
	std::string errstr="selectionMode: format (modeID(val)) error in " + str;
	throw std::logic_error(errstr);
}

}
