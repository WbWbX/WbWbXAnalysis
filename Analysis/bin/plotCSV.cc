#include "TtZAnalysis/Tools/interface/histoStackVector.h"
#include "TtZAnalysis/Tools/interface/fileReader.h"
#include <vector>
#include "TString.h"
#include "TFile.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "TtZAnalysis/Tools/interface/applicationMainMacro.h"
#include "TtZAnalysis/Tools/interface/plotterControlPlot.h"

TString stripRoot(TString s){
	if(s.EndsWith(".root")){ //cut ".root"
		s.ReplaceAll(".root","");
	}
	return s;
}

TString stripStuff(TString s){      /// needs rewriting for "/" part
	if(s.EndsWith(".root")){ //cut ".root"
		s.ReplaceAll(".root","");
	}
	//get rid of dir
	if(s.Contains("/")){
		ztop::fileReader fr;
		fr.setTrim("/");
		std::string st=s.Data();
		fr.rtrim(st); //make sure no / at end
		size_t pos = st.find("/");
		if( std::string::npos != pos)
		{
			st = st.substr(pos);
		}
		//now its only the filename that remains
		s=TString(st);
	}
	return s;
}

ztop::container1DStackVector getFromFile(TString filename)
{
	ztop::container1DStackVector vtemp;
	if(filename.EndsWith(".root")){
		TFile * ftemp=new TFile(filename,"read");
		TTree * ttemp = (TTree*)ftemp->Get("containerStackVectors");
		vtemp.loadFromTree(ttemp);
		delete ttemp;
		delete ftemp;
	}
	else{
		vtemp.readFromFile(filename.Data());
	}
	return vtemp;
}
invokeApplication(){
	using namespace ztop;
	using namespace std;
	AutoLibraryLoader::enable();

	const bool debug=parser->getOpt<bool>("d",false,"debug switch");

	const std::vector<TString> files=parser->getRest<TString> ();

	parser->doneParsing();
	if(debug){
		std::cout << "debug mode" << std::endl;
		plotterControlPlot::debug=true;
	}
	//std::vector<TString> filenames,containerNames;
	for(size_t i=0;i<files.size();i++){
		TString filename=files.at(i);

		std::cout << filename << std::endl;
		histoStackVector c=getFromFile(filename);
		filename.ReplaceAll(".root","_plots.root");
		filename.ReplaceAll(".ztop","_plots.root");
		c.writeAllToTFile(filename,true);
	}

	return 0;
}
