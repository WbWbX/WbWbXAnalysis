/*
 * resultsSummary.cc
 *
 *  Created on: Jun 27, 2014
 *      Author: kiesej
 */


#include "../interface/resultsSummary.h"
#include <algorithm>
#include "../interface/formatter.h"
#include "../interface/container.h"
#include "../interface/systAdder.h"

namespace ztop{


bool resultsSummary::debug=false;


graph resultsSummary::makeGraph()const{


	return graph();
}
const TString& resultsSummary::getEntryName(size_t entr)const{

	return entries_.at(entr).name;

}

TString resultsSummary::getValueString(size_t idx,  bool& isNeglegible , const float & prec)const{

	formatter fmt;
	TString out;
	if(entries_.at(idx).issys){
		float up=fmt.round(entries_.at(idx).totup,prec);
		float down=fmt.round(entries_.at(idx).totdown,prec);

		out="$";
		isNeglegible=false;
		if(fabs(up) < prec && fabs(down) < prec)
			isNeglegible=true;

		TString upvar;
		if(up>0)
			upvar="+"+fmt.toTString(fabs(up));
		else
			upvar="-"+fmt.toTString(fabs(up));
		TString downvar;
		if(down>0)
			downvar="+"+fmt.toTString(fabs(down));
		else
			downvar="-"+fmt.toTString(fabs(down));


		out+="^{"+upvar+"}_{"+downvar+"}$";

	}
	else{

		out=fmt.toTString( fmt.round(entries_.at(idx).entry,prec));

	}
	return out;

}

TString resultsSummary::getStatString(size_t idx,  bool& isNeglegible , const float & prec)const{

	formatter fmt;
	TString out;
	float up=fmt.round(entries_.at(idx).statup,prec);
	float down=fmt.round(entries_.at(idx).statdown,prec);

	if(up<down)up=down; //symm

	out="$\\pm$ ";
	isNeglegible=false;
	if(fabs(up) < prec)
		isNeglegible=true;

	TString var=fmt.toTString(fabs(up));
	out+=var;

	return out;

}


void resultsSummary::addEntry(const TString & name, const float & entry,
		const float& statup, const float& statdown,
		const float& totup, const float& totdown){

	summaryEntry newentry;
	newentry.name=name;
	newentry.entry=entry;
	newentry.statup=statup;
	newentry.statdown=statdown;
	newentry.totup=totup;
	newentry.totdown=totdown;
	entries_.push_back(newentry);

}

void resultsSummary::removeEntry(const TString& name){

	for(size_t i=0;i<entries_.size();i++){
		if(entries_.at(i).name == name){
			entries_.erase(entries_.begin()+i);
			return;
		}
	}
}

void resultsSummary::mergeVariations(const TString &nomentry,const std::vector<TString>& names, const TString & outname,bool linearly){
	std::vector<TString> sysnames;
	std::vector< std::pair<size_t,size_t> > asso=resultsSummary::getVariations(sysnames);
	// sysnames.at( asso.at(bla).second )

	if(sysnames.size() <2)
		return;

	float upmerged=0;
	float downmerged=0;

	if(names.size()<1)
		return ;

	float nominal=getEntry(nomentry).entry;

	resultsSummary cp;
	cp.addEntry(getEntry(nomentry).name,getEntry(nomentry).entry,getEntry(nomentry).statup,getEntry(nomentry).statdown,
			getEntry(nomentry).totup,getEntry(nomentry).totdown);

	bool firstmerged=false;
	size_t newfidx=0,newsidx=0;
	for(size_t i=0;i<asso.size();i++){

		size_t firstentry=asso.at(i).first;
		size_t secondentry=asso.at(i).second;
		if(debug){
			std::cout <<entries_.at(firstentry).name<<std::endl;
			std::cout <<		entries_.at(secondentry).name<<std::endl;
		}

		float upvar=0;
		float downvar=0;
		if(std::find(names.begin(),names.end(),TString(entries_.at(firstentry).name,entries_.at(firstentry).name.Last('_'))) == names.end()){
			cp.entries_.push_back(entries_.at(firstentry));
			cp.entries_.push_back(entries_.at(secondentry));
			if(debug)
				std::cout << "resultsSummary::mergeVariations: not using " << entries_.at(firstentry).name <<  " and "
				<<entries_.at(secondentry).name <<std::endl;
			continue;
		}

		if(entries_.at(firstentry).name.EndsWith("_up")){
			upvar=entries_.at(firstentry).entry - nominal;
			downvar=entries_.at(secondentry).entry - nominal;
		}
		else{
			downvar=entries_.at(firstentry).entry - nominal;
			upvar=entries_.at(secondentry).entry - nominal;
		}
		if(!linearly){
			float max= upvar; if(max<downvar) max=downvar;
			float min= downvar; if(min>upvar) min=upvar;
			if(max>0)
				upmerged+=max*max;
			if(min<0)
				downmerged+=min*min;
		}
		else{
			upmerged+=upvar;
			downmerged+=downvar;
		}
		if(!firstmerged){
			cp.entries_.push_back(entries_.at(firstentry));
			newfidx=cp.entries_.size()-1;
			cp.entries_.push_back(entries_.at(secondentry));
			newsidx=cp.entries_.size()-1;
			firstmerged=true;
		}
	}
	if(!linearly){
		upmerged=sqrt(upmerged);
		downmerged=-sqrt(downmerged);
	}
	cp.entries_.at(newfidx).entry = upmerged       + nominal;
	cp.entries_.at(newfidx).name = outname+"_up"   ;
	cp.entries_.at(newsidx).entry = downmerged     + nominal;
	cp.entries_.at(newsidx).name = outname+"_down" ;
	*this=cp;


}
void resultsSummary::mergeVariationsFromFile(const TString &nomentry,const std::string& filename){
	systAdder adder;
	adder.readMergeVariationsFile(filename);
	size_t ntobemerged=adder.mergeVariationsSize();
	if(debug)
		std::cout << "resultsSummary::mergeVariationsFromFile: to be merged: " << ntobemerged << std::endl;
	for(size_t i=0;i<ntobemerged;i++){

		TString mergedname=adder.getMergedName(i);
		std::vector<TString> tobemerged=adder.getToBeMergedName(i);
		bool linearly=adder.getToBeMergedLinearly(i);

		mergeVariations(nomentry,tobemerged,mergedname,linearly);
	}

}


void resultsSummary::mergeVariationsFromFileInCMSSW(const TString &nomentry,const std::string& filename){
	systAdder adder;
	adder.readMergeVariationsFileInCMSSW(filename);
	size_t ntobemerged=adder.mergeVariationsSize();
	for(size_t i=0;i<ntobemerged;i++){

		TString mergedname=adder.getMergedName(i);
		std::vector<TString> tobemerged=adder.getToBeMergedName(i);
		bool linearly=adder.getToBeMergedLinearly(i);

		mergeVariations(nomentry,tobemerged,mergedname,linearly);
	}

}

resultsSummary resultsSummary::createTotalError(const TString nomentry)const{

	resultsSummary out;

	summaryEntry nominal=getEntry(nomentry);
	std::vector<TString> vars;
	getVariations(vars);

	float nomp=nominal.entry;
	float errup2=0,errdown2=0;
	for(size_t i=0;i<vars.size();i++){


		const summaryEntry& upentry=getEntry(vars.at(i)+"_up");
		const summaryEntry& downentry=getEntry(vars.at(i)+"_down");

		float diffa= upentry.entry - nomp;
		float diffb= downentry.entry - nomp;
		float stata=std::max(fabs(upentry.statup ),fabs(upentry.statdown ) );
		float statb=std::max(fabs(downentry.statup ),fabs(downentry.statdown ) );



		if(sysrelout_){
			diffa/=nomp;diffa*=100;
			diffb/=nomp;diffb*=100;
		}

		out.addEntry(vars.at(i),0,stata,statb,diffa,diffb);


		float up = diffa;
		if(diffb>up) up= diffb;
		if(up< 0) up=0;

		float down=diffb;
		if(diffa<up) down=diffa;
		if(down>0) down=0;

		errup2+=up*up;
		errdown2+=down*down;
	}
	if(sysrelout_){
		errup2+=nominal.statup/nomp * nominal.statup/nomp * 10000;
		errdown2+=nominal.statdown/nomp * nominal.statdown/nomp * 10000;
		out.addEntry("stat",0,0,0,nominal.statup/nomp*100,nominal.statdown/nomp*100);
	}
	else{
		errup2+=nominal.statup * nominal.statup;
		errdown2+=nominal.statdown * nominal.statdown;
		out.addEntry("stat",0,0,0,nominal.statup,nominal.statdown);
	}



	out.addEntry("total",0,0,0,sqrt(errup2),-sqrt(errdown2));

	for(size_t i=0;i<out.nEntries();i++)
		out.entries_.at(i).issys=true;
	return out;

}

resultsSummary::summaryEntry& resultsSummary::getEntry(const TString & name){
	for(size_t i=0;i<entries_.size();i++){
		if(entries_.at(i).name == name){
			return entries_.at(i);
		}
	}
	throw std::out_of_range("resultsSummary::getEntry not found");
}


const resultsSummary::summaryEntry& resultsSummary::getEntry(const TString & name)const{
	for(size_t i=0;i<entries_.size();i++){
		if(entries_.at(i).name == name){
			return entries_.at(i);
		}
	}
	std::cout << "resultsSummary::getEntry " <<name << " not found" <<std::endl;
	throw std::out_of_range("resultsSummary::getEntry not found");
}

std::vector< std::pair<size_t,size_t> > resultsSummary::getVariations( std::vector<TString> &out)const{
	std::vector< std::pair<size_t,size_t> > pairs;
	out.clear();
	for(size_t i=0;i<entries_.size();i++){
		const TString& entry=entries_.at(i).name;
		TString newstr="";
		bool isvar=false;
		bool isup=false;
		if(entry.EndsWith("_up")){
			newstr=TString(entry,entry.Last('_'));
			isvar=true;
			isup=true;
		}
		if(entry.EndsWith("_down")){
			newstr=TString(entry,entry.Last('_'));
			isvar=true;
		}
		std::vector<TString>::const_iterator it=std::find(out.begin(),out.end(),newstr);
		if(isvar && it != out.end()){ //not new
			//find other index
			size_t otherit=0;
			TString add="_up";if(isup)add="_down";
			for(size_t j=0;j<entries_.size();j++){
				if(entries_.at(j).name == newstr+add){
					otherit=j;
					break;
				}
			}

			std::pair<size_t,size_t> p(i,otherit);
			pairs.push_back(p);
		}
		else if(newstr.Length()>0){
			out.push_back(newstr);
		}
	}
	return pairs;
}
/*
std::vector< std::pair<size_t,size_t> > resultsSummary::getVariations()const{
    std::vector<TString> dummy;
    return getVariations(dummy);
}*/

}

