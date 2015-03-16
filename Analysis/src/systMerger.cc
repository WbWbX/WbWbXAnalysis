/*
 * systMerger.cc
 *
 *  Created on: Feb 14, 2014
 *      Author: kiesej
 */



#include "../interface/systMerger.h"
#include "TtZAnalysis/Tools/interface/histoStackVector.h"
#include "../interface/AnalysisUtils.h"
#include "TtZAnalysis/DataFormats/interface/NTBTagSF.h"
#include <fstream>
#include "TFile.h"
#include <iostream>
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include <omp.h>
#include "../interface/discriminatorFactory.h"

namespace ztop{


bool systMerger::debug=false;

void systMerger::setInputStrings(const std::vector<std::string>& instrings){
	//needs to deliver a directory and filenames without root (by convention the same as csv names
	using namespace ztop;
	std::string addstr="";
	if(infileadd_.Length()>0)
		addstr=("_"+infileadd_).Data();
	//get dir from first one and then check if changes. if it does, throw.
	//it could mean same csv names
	size_t ignorecount=0;
	for(size_t i=0;i<instrings.size();i++){
		const std::string& instring=instrings.at(i);
		if((infileadd_.Length()>0 && !hasEnding(instring,addstr)) || instring.find(ignorestring_)!=std::string::npos){
			ignorecount++;
			if(debug) std::cout << "systMerger::setInputStrings: ignored " << instring <<std::endl;
			continue; //wrong add
		}
		std::string st=instring;
		std::string dir="";
		if(instring.find("/") != std::string::npos){
			dir=st.substr( 0, st.find_last_of( '/' ) +1 );
			st=st.substr( st.find_last_of( '/' ) +1 );
			//now its only the filename that remains
		}
		if(dir.size()>0){
			if(ignorecount == i){ //first entry not ignored
				indir_=dir;
				if(debug) std::cout << "systMerger::setInputStrings: used dir " << indir_ <<std::endl;
			}
			else{
				if(indir_!=dir)
					throw std::runtime_error(
							"systMerger::setInputStrings: files with same ..<marer>.ztop need to be in the same directory");
			}
		} //dir set
		TString topushback=instring;
		if(topushback.EndsWith(".ztop")){
			topushback.ReplaceAll(".ztop","");
			removedext_=".ztop";
		}
		if(infileadd_.Length()>0)
			topushback.ReplaceAll(infileadd_,"");
		if(debug) std::cout << "systMerger::setInputStrings: pushed back " << topushback <<std::endl;
		instrings_.push_back(topushback);
	}
}


std::vector<TString>  systMerger::start(){
	searchNominals();
	searchSystematics();
	std::vector<TString>  outfilenames=mergeAndSafe();
	mergeBTags();
	return outfilenames;
}



///sort inputs
void systMerger::searchNominals(){
	if(debug) std::cout << "systMerger::searchNominals" <<std::endl;
	TString check=nominalid_;
	nominals_.clear();
	for(size_t i=0;i<instrings_.size();i++){

		if(instrings_.at(i).Contains(check)
				&& (infileadd_.Length() < 1 || instrings_.at(i).EndsWith("_"+infileadd_))
				&& !(instrings_.at(i).EndsWith("_up") || instrings_.at(i).EndsWith("_down")) ){
			if(debug) std::cout << "systMerger::searchNominals: identified " << instrings_.at(i) <<std::endl;
			nominals_.push_back(i);
		}
	}
}

void systMerger::searchSystematics(){
	if(debug) std::cout << "systMerger::searchSystematics" <<std::endl;
	//create syst vector
	if(nominals_.size() < 1 && instrings_.size() >0){
		throw std::runtime_error("systMerger::searchSystematics: no input"); //should not happen!
	}
	syst_.clear();
	//resize
	syst_.resize(nominals_.size(),std::vector<size_t>());
	if(debug) std::cout << "systMerger::searchSystematics: resized" <<std::endl;
	//for each nominal the corresp syst
	for(size_t nom=0;nom<nominals_.size();nom++){
		TString startstring=instrings_.at(nominals_.at(nom));
		if(debug) std::cout << "systMerger::searchSystematics: for nominal "<< startstring<<std::endl;

		if(nominalid_ == standardnominal_)
			startstring.ReplaceAll(nominalid_,"");

		for(size_t sys=0;sys<instrings_.size();sys++){
			if(sys==nominals_.at(nom)) continue; //is nominal
			if(instrings_.at(sys).BeginsWith(startstring) && (infileadd_.Length() < 1 ||  instrings_.at(sys).EndsWith("_"+infileadd_))){
				if(debug) std::cout << "systMerger::searchSystematics: identified " << instrings_.at(sys)
                                        								<< " for nominal " << instrings_.at(nominals_.at(nom)) << std::endl;

				syst_.at(nom).push_back(sys);
			}
		}
	}
}

histoStackVector * systMerger::getFromFileToMem( TString dir, const TString &name)const
{

	histoStackVector * vtemp=new histoStackVector();
	TString readf=name;
	if(removedext_.length())
		readf+=removedext_;
	vtemp->readFromFile(readf.Data());
	/*
	TFile * ftemp=0;
	TTree * ttemp =0;
#pragma omp critical (systMerger_getFromFileToMem)
	{
		AutoLibraryLoader::enable();
		if(dir!="") dir+="/";

		ftemp=new TFile(dir+name+".root","read");
		ttemp = (TTree*)ftemp->Get("containerStackVectors");

		vtemp=new histoStackVector();
		vtemp->loadFromTree(ttemp,name);
		delete ttemp;
		delete ftemp;
	}
	 */
	return vtemp;
}


std::vector<TString>  systMerger::mergeAndSafe(){
	if(debug) std::cout << "systMerger::mergeAndSafe" <<std::endl;
	std::vector<TString>  outputfilenames;
	//get a nominal
	TString dir=indir_;
	TString outadd="";
	if(outfileadd_.Length()>0)
		outadd="_"+outfileadd_;

	//not parallel for all nominals, file IO is the bottleneck!


	std::vector<discriminatorFactory> alldisc;
	//try in parallel
	//#pragma omp parallel for
	for(size_t nom=0;nom<nominals_.size();nom++){
		histoStackVector * nominal=getFromFileToMem(dir,instrings_.at(nominals_.at(nom)));
		TString reldir=dir;
		if(reldir!="")reldir+="/";
		std::vector<discriminatorFactory> disc;
		if(fileExists(reldir+instrings_.at(nominals_.at(nom))+"_discr.root")){
			disc=discriminatorFactory::readAllFromTFile(reldir+instrings_.at(nominals_.at(nom))+"_discr.root");//get nominal configuration
		}
		for(size_t sys=0;sys<syst_.at(nom).size();sys++){
			histoStackVector * sysvec=getFromFileToMem(dir,instrings_.at(syst_.at(nom).at(sys)));
			if(debug) std::cout << "systMerger::mergeAndSafe: adding "<< sysvec->getName() << " to " << nominal->getName()<< std::endl;
			if(nominalid_ == standardnominal_){
				nominal->addMCErrorStackVector(*sysvec);
			}
			else{
				TString sysname=sysvec->getSyst();
				if(debug)std::cout << "replacing name " << sysname << " by ";
				sysname.ReplaceAll(nominalid_+"_","");
				sysname.ReplaceAll(nominalid_,"");
				if(debug)std::cout << sysname << " removed "<< nominalid_<< std::endl;
				nominal->addMCErrorStackVector(sysname,*sysvec);
			}
			delete sysvec;
		}
		nominal->setName(nominal->getName()+outadd+"_syst");
		std::cout << "systMerger: writing "<< nominal->getName() << std::endl;
		outputfilenames << nominal->getName()+".ztop";
		std::cout <<std::endl;
		nominal->writeToFile((nominal->getName()+".ztop").Data()); //is critical anyway
		if(drawcanv_)
			nominal->writeAllToTFile(nominal->getName()+"plots_.root",true,false);
		std::cout << "systMerger: written "<< nominal->getName() << std::endl;

		delete nominal;
	} //end parallel

	//#pragma omp parallel for
	for(size_t nom=0;nom<nominals_.size();nom++){
		std::vector<discriminatorFactory> disc;
		histoStackVector * nominal=getFromFileToMem(dir,instrings_.at(nominals_.at(nom)));
		for(size_t i=0;i<disc.size();i++){
			disc.at(i).extractLikelihoods(*nominal);
		}
#pragma omp critical
		{
			alldisc << disc;
			delete nominal;
		}
	}

	system(((TString)"rm -f "+"all"+outadd+ "_discr.root").Data());
	if(alldisc.size()>0)
		discriminatorFactory::writeAllToTFile("all" +outadd+  "_discr.root",alldisc);

	return outputfilenames;
}

void systMerger::mergeBTags()const{
	if(debug) std::cout << "systMerger::mergeBTags" <<std::endl;
	std::cout << "systMerger::mergeBTags: not used anymore, single files should be provided to the analysis chain." <<std::endl;
	return;

}

std::vector<size_t> systMerger::getMergedFiles()const{
	std::vector<size_t> out=nominals_;
	for(size_t i=0;i<syst_.size();i++)
		out.insert(out.end(),syst_.at(i).begin(),syst_.at(i).end());

	return out;
}



}


