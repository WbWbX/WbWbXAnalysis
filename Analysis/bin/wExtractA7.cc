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
	const std::vector<std::string> infiles=parser->getRest<std::string>();
	if(infiles.size()!=1)
		parser->coutHelp();
	const std::string infile=infiles.at(0);

	wA7Extractor ex;
	ex.readConfig(configfile);
	ex.loadPlots(infile);




	return 0;
}
