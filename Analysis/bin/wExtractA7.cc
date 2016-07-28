/*
 * wExtractA7.cc
 *
 *  Created on: 20 Jun 2016
 *      Author: jkiesele
 */


#include "../interface/wA7Extractor.h"
#include "TtZAnalysis/Tools/interface/applicationMainMacro.h"


invokeApplication(){
	using namespace ztop;


	const std::string configfile=parser->getOpt<std::string>("i","","configuration file");
	const size_t npseudo=parser->getOpt<int>("p",0,"n pseudo experiments");
	const bool enhancestat=parser->getOpt<bool>("-noenhancestat",true,"enhances MC statistics");
	const float statthresh=parser->getOpt<float>("s",0.006,"threshold for rebinning");

	const std::vector<std::string> infiles=parser->getRest<std::string>();


	if(infiles.size()!=1)
		parser->coutHelp();
	const std::string infile=infiles.at(0);


	wA7Extractor ex;
	//ex.debug=true;
	ex.setMCStatThreshold(statthresh);
	ex.readConfig(configfile);
	ex.setEnhanceStat(enhancestat);
	ex.loadPlots(infile);
	ex.setNPseude(npseudo);

	ex.fitAll();


	return 0;
}
