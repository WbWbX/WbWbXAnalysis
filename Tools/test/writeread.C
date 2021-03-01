
#include "WbWbXAnalysis/Tools/interface/fileReader.h"
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include "TString.h"

void writeread(){
	using namespace ztop;

	fileReader fr;

	fileReader::debug=true;
	TString file,legend;
	double norm;
	bool signal=false;
	int color,lego;
	//fr.setDelimiter(" ");
	fr.setComment("$");
	fr.setStartMarker("[inputfiles]");
	fr.setEndMarker("[inputfiles-end]");
	fr.readFile("testread.txt");

	std::cout << "\n\nOUTPUT" << std::endl;

	for(size_t i=0;i<fr.nLines();i++){
		if(fr.getData(i).size() < 2)
			continue;
		signal=false;
		for(size_t j=0;j<fr.getData(i).size();j++){
			if(j==0)
				file=fr.getData<TString>(i,j);
			else if(j==1)
				legend=fr.getData<TString>(i,j);
			else if(j==2)
				color=fr.getData<int>(i,j);
			else if(j==3)
				norm=fr.getData<double>(i,j);
			else if(j==4)
				lego=fr.getData<int>(i,j);
			else if(j==5)
				signal=fr.getData<bool>(i,j);
		}
		std::cout << file << " "<< legend << " "<< color << " " <<norm << " " << lego<<" " << signal<<std::endl;
		std::cout << std::endl;
	}
	fileReader fr2=fr;
	fr2.clear();
	fr2.setStartMarker("[valuedefs]");
	fr2.setEndMarker("[valuedefs-end]");
	fr2.readFile("testread.txt");

	std::cout << "\n\nVALUES" <<std::endl;

	std::cout << fr2.getValue<TString>("value1") << std::endl;
	std::cout << fr2.getValue<std::string>("value2") << std::endl;
	std::cout << fr2.getValue<bool>("value3") << std::endl;
	std::cout << fr2.getValue<double>("value4")/3 << std::endl;
	std::cout << fr2.getValue<int>("value5") << std::endl;


}
