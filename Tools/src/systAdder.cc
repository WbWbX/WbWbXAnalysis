/*
 * systAdder.cc
 *
 *  Created on: Sep 3, 2014
 *      Author: kiesej
 */


#include "../interface/systAdder.h"
#include "../interface/fileReader.h"

namespace ztop{

bool systAdder::debug=false;

/**
 * expects the following format:
 * <mergedname> <mergeoption> <contribution1> <contribution2> ....
 */
void systAdder::readMergeVariationsFileInCMSSW(const std::string&filename,const std::string&  marker){
	std::string cmsenv=getenv("CMSSW_BASE");
	readMergeVariationsFile(cmsenv+"/"+filename,marker);
}
/**
 * expects the following format:
 * <mergedname> <mergeoption> <contribution1> <contribution2> ....
 */
void systAdder::readMergeVariationsFile(const std::string&filename,const std::string&  marker){
	fileReader fr;
	fr.setComment("%");
	fr.setDelimiter(" ");
	//fileReader::debug=true;
	if(marker.length()<1){
		fr.setStartMarker("[merge_syst]");
		fr.setEndMarker("[end-merge_syst]");
	}
	else{
		fr.setStartMarker("[" +marker+ "]");
		fr.setEndMarker("[end - " +marker+ "]");
	}
	fr.readFile(filename);

	tomerge.clear();

	for(size_t line=0;line<fr.nLines();line++){

		if(debug)
			std::cout << "systAdder::readMergeVariationsFile: reading line\n" <<  fr.getReJoinedLine(line) <<std::endl;

		if(fr.getData(line).size()<3) //at least three entries -> igronre all other ones
			continue;

		TString mergedname=fr.getData<TString>(line,0);
		bool linearly= fr.getData<TString>(line,1) == "linear";
		bool squared= fr.getData<TString>(line,1) == "squared";

		if(!linearly && !squared){
			throw std::runtime_error("systAdder::readMergeVariationsFile: \
					 inrecognized option. specify way systematics should be added: linearly or squared");
		}
		std::vector<TString> tobemerged;
		for(size_t i=2;i<fr.getData(line).size();i++)
			tobemerged.push_back(fr.getData<TString>(line,i));

		systMergeDescription desc;
		desc.linearly=linearly;
		desc.mergename=mergedname;
		desc.tobemerged=tobemerged;

		tomerge.push_back(desc);

	}



}

TString              systAdder::getMergedName(size_t idx)const{
	if(idx>=tomerge.size())
		throw std::out_of_range("systAdder::getMergedName");
	return tomerge.at(idx).mergename;
}
std::vector<TString> systAdder::getToBeMergedName(size_t idx)const{
	if(idx>=tomerge.size())
		throw std::out_of_range("systAdder::getToBeMergedName");
	return tomerge.at(idx).tobemerged;
}

bool                 systAdder::getToBeMergedLinearly(size_t idx)const{
	if(idx>=tomerge.size())
		throw std::out_of_range("systAdder::getToBeMergedLinearly");
	return tomerge.at(idx).linearly;
}





}

