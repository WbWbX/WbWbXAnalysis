#include "TtZAnalysis/Tools/interface/containerStackVector.h"
#include "TtZAnalysis/Tools/interface/fileReader.h"
#include <vector>
#include "TString.h"
#include "TFile.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#define __CINT__
#include "TtZAnalysis/DataFormats/interface/NTBTagSF.h"
#include <fstream>
#include <string>

///put in energy separation.. place to add emu channel based DY reweighting.

//[analyse]... or do bg stuff here

//merge btags here


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
TString getChannel(TString s){
	if(s.Contains("ee_")) return "ee";
	if(s.Contains("emu_")) return "emu";
	if(s.Contains("mumu_")) return "mumu";
	return "";
}

size_t associateChannel(TString s){
	if(s.Contains("ee_")) return 0;
	if(s.Contains("emu_")) return 1;
	if(s.Contains("mumu_")) return 2;
	else return 3; //exception
}

size_t associateEnergy(TString s){
	if(s.Contains("_8TeV_")) return 0;
	if(s.Contains("_7TeV_")) return 1;
	else return 2; //exception
}

ztop::container1DStackVector getFromFile(TString name)
{
	TFile * ftemp=new TFile(name+".root","read");
	TTree * ttemp = (TTree*)ftemp->Get("stored_objects");
	ztop::container1DStackVector vtemp;
	vtemp.loadFromTree(ttemp,name);
	delete ttemp;
	delete ftemp;
	return vtemp;
}

int main(int argc, char* argv[]){
	using namespace ztop;
	using namespace std;
	AutoLibraryLoader::enable();

	containerStackVector::fastadd=true;
	//containerStackVector::debug=true;
	//containerStack::debug=true;

	//container1D::debug=true;
	//histoContent::debug=true;

	// vector<TString> filenameids;
	if(argc <4){
		cout << "at least one nominal sample (first) and sys variations up and down need to be specified!" << endl;
		return -1;
	}
	//vector<TFile *> sysfiles;
	// vector<TTree *> systrees;
	vector<TString> names;
	vector<vector<ztop::container1DStackVector> >vnominal; //energy , channels
	vector<ztop::container1DStackVector>  vsysvecs;

	ztop::container1DStackVector def;
	vector<ztop::container1DStackVector> temp;
	for(size_t i=0;i<3;i++)
		temp.push_back(def);
	for(size_t i=0;i<2;i++)
		vnominal.push_back(temp);

	//read in files

	TString output;


	for(int i=1;i<argc;i++){
		if((TString)argv[i] == "-o"){ //output
			output=(TString)argv[i+1];
			i++;
			continue;
		}
		names.push_back(stripRoot((TString)argv[i]));
	}

	//fill nominal

	cout << "loading nominal plots... ";
	for(size_t i=0;i<names.size();i++){
		if(names.at(i).Contains("_nominal")){
			vnominal.at(associateEnergy(names.at(i))).at(associateChannel(names.at(i))) = getFromFile(names.at(i));
			cout << names.at(i) << " ";
		}
	}
	cout << endl;

	//get syst
	cout << "loading syst variations... ";
	for(size_t i=0;i<names.size();i++){
		if(!names.at(i).Contains("_nominal")){
			vsysvecs.push_back(getFromFile(names.at(i)));
			cout << names.at(i) << " ";
		}
	}
	cout << endl;
	///maybe do some BG variations here//add to vsysvecs

	//fill other syst

	cout << "adding variations to nominal..." <<endl;

	for(size_t i=0;i<vsysvecs.size();i++){
		TString name=vsysvecs.at(i).getName();
		cout << "adding variation " << name << std::endl;
		ztop::container1DStackVector  *nominal = & vnominal.at(associateEnergy(name)).at(associateChannel(name));
		nominal->addMCErrorStackVector(vsysvecs.at(i));
	}

	cout << "writing output..." << endl;

	for(size_t i=0;i<vnominal.size();i++){ //energies
		for(size_t j=0;j<vnominal.at(i).size();j++){ //channels
			ztop::container1DStackVector *outvec=&vnominal.at(i).at(j);
			if(outvec->getName() =="")//in case energies are not considered
				continue;
			outvec->setName(outvec->getName()+output);
			cout << "writing " << outvec->getName() << endl;
			outvec->writeAllToTFile(outvec->getName()+output+"_syst.root",true);
			cout << "syst: " << outvec->getStack(0).getContainer(0).getSystSize() << endl;
		}
	}

	/////////part to merge btagSF


	NTBTagSF btags;
	//check if file exists, if yes, assume btags have been recreated and merge

	if(names.size()>0){
		std::ifstream OutFileTest((names.at(0)+"_btags.root").Data());
		if(OutFileTest) { //btags were created
			cout << "merging btag SF" <<endl;

			for(size_t i=0;i<names.size();i++){
				TString filename=names.at(i)+"_btags.root";
				NTBTagSF temp;

				std::ifstream OutFileTest2(filename.Data());
				if(OutFileTest2){
					temp.readFromTFile(filename);
					btags=btags+temp;
				}
				else{
					cout << "btag file " << filename << " not found" << endl;
				}
			}
			btags.writeToTFile("all_btags.root");
		}

	}

	return 0;
}

ztop::NTBTagSF mergeBTagSF(std::vector<TString> filenames){

	return ztop::NTBTagSF();
}
