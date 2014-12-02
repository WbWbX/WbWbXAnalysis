/*
 * taggedObject.cc
 *
 *  Created on: Mar 26, 2014
 *      Author: kiesej
 */


#include "../interface/taggedObject.h"
#include <algorithm>
namespace ztop{

void taggedObject::addTag(const taggedObject::tags& tag){
	if(tags_.size()<1 || !hasTag(tag)){
		tags_.push_back(tag);
		sorted_=false;
	}
}
void taggedObject::removeTag(const taggedObject::tags& tag){
	if(tags_.size()<1){
		return;
	}
	std::vector< taggedObject::tags>::iterator it=std::find(tags_.begin(),tags_.end(),tag);
	if(it!=tags_.end())
		tags_.erase(it);
}

void taggedObject::addTagsFrom(const taggedObject* obj){

	std::vector<taggedObject::tags> in=obj->tags_;
	if(!sorted_)
		std::sort(tags_.begin(),tags_.end());
	if(!obj->sorted_)
		std::sort(in.begin(),in.end());

	if(tags_==in)
		return;

	std::vector<taggedObject::tags> newtags(tags_.size()+obj->tags_.size());
	std::vector<taggedObject::tags>::iterator it=std::set_union (tags_.begin(), tags_.end(), in.begin(), in.end(), newtags.begin());
	newtags.resize(it-newtags.begin());
	tags_=newtags;
	sorted_=true;
}

bool taggedObject::hasTag(const taggedObject::tags& tag)const{
	if(tags_.size()<1) return false;
	return std::find(tags_.begin(),tags_.end(),tag) != tags_.end();
}
bool taggedObject::hasTag()const{
	return tags_.size()>0;
}
bool taggedObject::tagsAreEqual(const taggedObject* obj)const{
	std::vector<taggedObject::tags> in=obj->tags_;
	std::vector<taggedObject::tags> lhs=tags_;
	if(!sorted_)
		std::sort(lhs.begin(),lhs.end());
	if(!obj->sorted_)
		std::sort(in.begin(),in.end());
	return lhs==in;
}

/**
 * Returns the name formatted with:
 * any space replaced by "_"
 * any comma replaced by "_"
 * any "/" replaced by "_"
 */
TString taggedObject::getFormattedName()const{
	TString out=getName();
	out.ReplaceAll(" ","_");
	out.ReplaceAll(",","_");
	out.ReplaceAll("/","_");
	return out;
}

}

