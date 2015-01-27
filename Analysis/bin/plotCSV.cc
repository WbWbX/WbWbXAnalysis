#include "TtZAnalysis/Tools/interface/containerStackVector.h"
#include "TtZAnalysis/Tools/interface/fileReader.h"
#include <vector>
#include "TString.h"
#include "TFile.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#define __CINT__

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
	TFile * ftemp=new TFile(filename,"read");
	TTree * ttemp = (TTree*)ftemp->Get("containerStackVectors");
	ztop::container1DStackVector vtemp;
	vtemp.loadFromTree(ttemp);
	delete ttemp;
	delete ftemp;
	return vtemp;
}
int main(int argc, char* argv[]){
	using namespace ztop;
	using namespace std;
	AutoLibraryLoader::enable();


	//std::vector<TString> filenames,containerNames;
	for(int i=1;i<argc;i++){
		//  std::cout << argv[i] << std::endl;;
		TString filename=(TString)argv[i];
		//filenames.push_back(filename);
		//containerNames.push_back(cname);
		containerStackVector c=getFromFile(filename);
		c.writeAllToTFile(filename.ReplaceAll(".root","_plots.root"),true);
	}

	return 0;
}
