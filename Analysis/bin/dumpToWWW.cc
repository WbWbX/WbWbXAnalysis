/*
 * dumpToWWW.cc
 *
 *  Created on: Nov 25, 2013
 *      Author: kiesej
 */

///afs/desy.de/user/k/kiesej/www/analysisDump/index.html
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include <iostream>
#include "WbWbXAnalysis/Tools/interface/histoStackVector.h"
#include "TFile.h"
#include <fstream>
#include <string>
#include <time.h>
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"


ztop::container1DStackVector getFromFile(TString name)
{
	name.ReplaceAll(".root","");
	AutoLibraryLoader::enable();
	TFile * ftemp=new TFile(name+".root","read");
	TTree * ttemp = (TTree*)ftemp->Get("stored_objects");
	ztop::container1DStackVector vtemp;
	vtemp.loadFromTree(ttemp,name);
	delete ttemp;
	delete ftemp;
	return vtemp;
}


/**
 * title will be the link name and <title>/.index.html will be the file linked to
 * files and dirs have to be created outside this function!
 *
 *
 */
int appendToIndex(TString title, TString description,TString plotdir, TString indexfile){
	using namespace ztop;
	using namespace std;

	string strReplace="<!--NEW ENTRY HERE - DON'T DELETE-->";

	string strNew = "<!--NEW ENTRY HERE - DON'T DELETE-->";
	strNew+="\n";
	strNew+=(string)"<b>" + (string)"<a href=\"" +plotdir+ "/index.html\">"+title+"</a></b>\n<br/>";
	strNew+=description+"\n<br/><br/><hr />\n";
	std::ifstream filein(indexfile); //File to read from
	std::ofstream fileout((indexfile+"_tmp").Data()); //Temporary file
	if(!filein || !fileout)
	{
		cout << "Error opening files!" << endl;
		return -1;
	}

	string strTemp;
	//bool found = false;
	while(filein.good())
	{
		if (!getline( filein, strTemp )) break;

		if(strTemp == strReplace){
			strTemp = strNew;
			//found = true;
		}
		strTemp += "\n";
		fileout << strTemp;
		//if(found) break;
	}
	fileout.close();
	filein.close();
	system(("mv "+indexfile+"_tmp "+indexfile).Data());
return 0;
}

void makePlotHTML(TString title, TString descr, TString where , ztop::histoStackVector& csv){
	std::ofstream fileout((where+"index.html").Data());

	using namespace std;

	string header=(string)"<html><head><title>"+ (string)title  +(string)"</title></head><body>";
	header+="<b>"+title + "</b>\n<br/>\n<hr /><br/>\n";
	header+=descr;
	header+="\n<hr /><br/>\n";
	fileout << header;

//some table stuff for the plots
	std::vector<TString> names=csv.getStackNames(true);
	std::vector<TString> fullnames=csv.getStackNames(false);
	string plotspart;

	for(size_t i=0;i<names.size();i++){
		plotspart+="<b>" + names.at(i) + "</b><br/>\n";
		for(size_t j=0;j<fullnames.size();j++){
			if(fullnames.at(j).BeginsWith(names.at(i)+" step")){
				TString newname=fullnames.at(j);
				newname.ReplaceAll(" ","_");
				plotspart+="<img src="+ newname +".png width=\"270\" height=\"175\" alt=" + newname +".png />";
			}
		}
		plotspart+="\n<hr /><br/>\n";
	}
	csv.printAll("",where,".png");

	fileout << plotspart;

	string endfile="</body>";
	fileout << endfile;

	fileout.close();

}

int main(int argc, char* argv[]){
	using namespace ztop;

	TString title="";
	TString description="";
	std::vector<TString> infiles;
	///get title and description
	for(int i=1;i<argc;i++){
		if((TString)argv[i] == "-t"){ //title / link name
			if(i+1<argc){
				title=(TString)argv[i+1];
				i++;}
			continue;

		}
		if((TString)argv[i] == "-d"){ //description
			if(i+1<argc){
				description=(TString)argv[i+1];
				i++;}
			continue;

		}
		infiles.push_back((TString)argv[i]);
	}

	if(title ==""){
		std::cerr << "at least title needs to be specified" << std::endl;
		return -1;
	}
	if(infiles.size()<1 || infiles.size()>1){
		std::cerr << "exactly one input file  needs to be specified" << std::endl;
		return -1;
	}
	TString infile=infiles.at(0);


	TString dumpdir="/afs/desy.de/user/k/kiesej/www/analysisDump/";
	TString index="index.html";
	TString fullIndex=dumpdir+index;

	std::ofstream testtoken((dumpdir+"testtmp").Data());
	if(!testtoken){
		std::cerr << "File " << dumpdir<< " not accessible, no token?" <<std::endl;
		return -1;
	}
	system(("rm -f "+dumpdir+"testtmp").Data());



	TString plotdir=title;
	plotdir.ReplaceAll(" ","_");
	time_t seconds;

	seconds = time (NULL);
	plotdir+=toTString<time_t>(seconds);
	appendToIndex(title,description,plotdir,fullIndex);
	plotdir+="/";
	plotdir=dumpdir+plotdir;
	system(("mkdir "+plotdir).Data());
	histoStackVector csv=getFromFile(infile);
	makePlotHTML(title,  description, plotdir,csv);


}


