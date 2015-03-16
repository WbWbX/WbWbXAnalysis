/*
 * makeCombination.cc
 *
 *  Created on: Aug 19, 2014
 *      Author: kiesej
 */


#include "TtZAnalysis/Tools/interface/optParser.h"
#include "TtZAnalysis/Tools/interface/resultCombiner.h"
#include "TtZAnalysis/Tools/interface/histo1D.h"
#include "TtZAnalysis/Tools/interface/fileReader.h"
#include "TString.h"

double sq(const double & in){
	return in*in;
}


#include "TtZAnalysis/Tools/interface/applicationMainMacro.h"

invokeApplication(){

	using namespace ztop;

	parser->bepicky=true;
	TString inputfile= parser->getOpt<TString>  ("i","","specify input file");

	parser->doneParsing();
	if(inputfile.Length()<1){
		parser->coutHelp();
	}

	fileReader fr;

	//fileReader::debug=true;
	fr.setDelimiter(",");
	fr.setComment("#");
	fr.setStartMarker("[central values]");
	fr.setEndMarker("[end-central values]");
	fr.readFile(inputfile.Data());

	std::vector<float> centralpoints;
	for(size_t i=0;i<fr.getData(0).size();i++){
		centralpoints.push_back(fr.getData<float>(0,i));
	}
	std::cout << std::endl;

	fr.clear();
	fr.setDelimiter(",");
	fr.setComment("#");
	fr.setStartMarker("[configuration]");
	fr.setEndMarker("[end-configuration]");
	fr.readFile(inputfile.Data());

	const bool inpercent=fr.getValue<bool>("errorsInPercent");

	fr.clear();
	fr.setDelimiter(",");
	fr.setComment("#");
	fr.setStartMarker("[systematics]");
	fr.setEndMarker("[end-systematics]");
	fr.readFile(inputfile.Data());

	std::vector<float> bin;bin.push_back(0);bin.push_back(1);
	resultCombiner combine;

	//variateHisto1D::debug=true;

	for(size_t meas=0;meas<centralpoints.size();meas++){
		histo1D onbinned(bin);
		onbinned.setBinContent(1,centralpoints.at(meas));
		std::cout << "adding systematics for measurement "<< meas <<std::endl;
		for(size_t i=0;i<fr.nLines();i++){
			if( fr.getData(i).size() <= meas+1) continue;
			TString sysname=fr.getData<TString>(i,0);
			float errinpercent=fr.getData<float>(i,meas+1);
			if(!inpercent){
				errinpercent=errinpercent * 100 / centralpoints.at(meas);
			}
			std::cout << sysname<< " " << errinpercent << "%"<<std::endl;

			if(sysname!="Stat"){
				if(!(sysname.EndsWith("_up") || sysname.EndsWith("_down") )){

					TString upsys=sysname+"_up";
					TString downsys=sysname+"_down";
					onbinned.addErrorContainer(upsys,onbinned*(1+0.01*errinpercent));
					onbinned.addErrorContainer(downsys,onbinned*(1-0.01*errinpercent));

				}
				else{
					onbinned.addErrorContainer(sysname,onbinned*(1+0.01*errinpercent));
				}

			}
			else{

				onbinned.setBinStat(1,centralpoints.at(meas)*0.01*errinpercent);

			}
		}
		combine.addInput(onbinned);
	}
	std::vector<TString> sysforms,sysnames;
	for(size_t i=0;i<fr.nLines();i++){
		if( fr.getData(i).size() <= centralpoints.size()+1) continue;
		TString tmpname=fr.getData<TString>(i,0);
		tmpname.ReplaceAll("_up","");
		tmpname.ReplaceAll("_down","");
		sysnames.push_back(tmpname);
		sysforms.push_back(fr.getData<TString>(i,centralpoints.size()+1));
	}

	for(size_t i=0;i<sysforms.size();i++){
		if(sysforms.at(i)=="gaus")
			combine.setSystForm(sysnames.at(i),resultCombiner::rc_sysf_gaus);
		else if(sysforms.at(i)=="box")
			combine.setSystForm(sysnames.at(i),resultCombiner::rc_sysf_box);
	}

	bool succ=combine.minimize();

	if(!succ){
		std::cout << "\n\nFit has not converged! exit" <<std::endl;
		return -1;
	}

	histo1D output=combine.getOutput();


	combine.coutSystBreakDownInBin(1);

	std::cout << "\n\ncombined value:   " << output.getBinContent(1) <<std::endl;
	std::cout << "uncertainty up:   +" << output.getBinErrorUp(1,false)/output.getBinContent(1) *100 << "%"<<std::endl;
	std::cout << "uncertainty down: -" << output.getBinErrorDown(1,false)/output.getBinContent(1) *100 << "%" <<std::endl;


	return 1;

}

