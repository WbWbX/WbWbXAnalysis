/*
 * test.cc
 *
 *  Created on: 2 Jun 2016
 *      Author: jkiesele
 */


#include "WbWbXAnalysis/Tools/interface/textFormatter.h"
#include <iostream>
#include "WbWbXAnalysis/Tools/interface/applicationMainMacro.h"
#include "../interface/jobSubmitterNaf.h"
#include "../interface/jobSubmitterLxBatch.h"
#include "WbWbXAnalysis/Tools/interface/fileReader.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "TString.h"
#include <time.h>

invokeApplication(){
	using namespace ztop;

	parser->setAdditionalDesciption("<options> -i <configfile> <workdir name> <options to be parser to executable to be submitted>\nprogram to submit jobs to a cluster (e.g. NAF or lxbatch).\nUsage");

	const std::string CMSSWBASE=getenv("CMSSW_BASE");

	const std::string config=CMSSWBASE+"/src/WbWbXAnalysis/Analysis/configs/submitjobs/"+
			parser->getOpt<std::string>("i","","config file");

	const bool dummyrun=parser->getOpt<bool>("-dummy",false,"dummy run. Does not submit jobs");

	std::vector<std::string> args=parser->getRest<std::string>();
	parser->doneParsing();
	if(config.length()<1 || args.size()<1)
		parser->coutHelp();

	time_t now = time(0);
	tm* thistime=localtime(&now);
	char  datetag[11];
	strftime(datetag,11,"%y%d%m%H%M",thistime);


	const std::string workdir="20"+(std::string)datetag+"_"+args.at(0);
	std::string arg;
	for(size_t i=1;i<args.size();i++)
		arg+=args.at(i)+" ";

	jobSubmitterBase* js=0;

	/*
	 *
	 * Implement auto check
	 *
	 */
	bool naf=true;
	if(naf)
		js=new jobSubmitterNaf();
	else
		js=new jobSubmitterLxBatch();

	js->setGenArguments(arg);
	js->setWorkingDir(workdir);
	js->setSourceDir(CMSSWBASE+"/src/WbWbXAnalysis/Analysis");


	std::vector<std::string> systematics;
	//read config from file
	fileReader fr;
	fr.setDelimiter(",");
	fr.setComment("$");
	fr.clear();
	fr.setStartMarker("[general]");
	fr.setEndMarker("[end general]");
	fr.readFile(config);
	const std::string executable=fr.getValue<std::string>("executable");
	if(system(("which "+executable).data()))
		throw std::runtime_error("executable not found");
	js->setExecutable(executable);

	TString analyzertype=fr.getValue<TString>("type");
	std::cout << analyzertype << std::endl;

	systematics=fr.readList(config,"[systematics]","[end systematics]");

	fr.setStartMarker("[PDF]");
	fr.setEndMarker("[end - PDF]");
	fr.readFile(config);
	const int npdf=fr.getValue<int>("npdf",0);
	if(npdf>0)
		for(size_t i=0;i<(size_t)npdf;i++){
			if(analyzertype=="top"){
				if(!i)
					systematics.push_back("PDF_sysnominal");
				else{
					systematics.push_back("PDF_sysnominal_PDF"+toString(i)+"_down");
					systematics.push_back("PDF_sysnominal_PDF"+toString(i)+"_up");
				}
			}
			else if(analyzertype=="W"){
				systematics.push_back("PDF"+toString(i)+"_down");
				systematics.push_back("PDF"+toString(i)+"_up");
			}
		}

	std::cout << "running on systematics "<<std::endl;
	for(size_t i=0;i<systematics.size();i++) std::cout << systematics.at(i) << std::endl;


	if(analyzertype=="top"){
		using namespace std;
		bool isbtag=arg.find("-B")!=string::npos;

		std::cout << "top submission" <<std::endl;

		std::vector<std::string> channels,mts,energies;
		channels=fr.readList(config,"[channels]","[end channels]");
		mts=fr.readList(config,"[topmasses]","[end topmasses]");
		energies=fr.readList(config,"[energies]","[end energies]");

		ZTOP_COUTVAR(channels.size());
		ZTOP_COUTVAR(mts.size());
		ZTOP_COUTVAR(energies.size());

		//create the jobs (specific)
		//typename std::vector<std::string>::const_iterator sit;
		for(std::vector<std::string>::const_iterator  chan=channels.begin();chan<channels.end();chan++)
			for(std::vector<std::string>::const_iterator  en=energies.begin();en<energies.end();en++)
				for(std::vector<std::string>::const_iterator  mt=mts.begin();mt<mts.end();mt++)
					for(std::vector<std::string>::const_iterator  sys=systematics.begin();sys<systematics.end();sys++){
						if(mt->find("172.5")==string::npos){
							if(sys->find("SCALE")!=string::npos
									|| sys->find("SCALE")!=string::npos
									|| sys->find("MATCH")!=string::npos
									|| sys->find("TOPPT")!=string::npos
									|| sys->find("_sysnominal")!=string::npos
									|| sys->find("TT_GEN")!=string::npos
									|| sys->find("BJESNUDEC")!=string::npos
									|| sys->find("BJESRETUNE")!=string::npos){
								continue;
							}
						}
						if(sys->find("P11_sysnominal")!=string::npos && en->find("7TeV"))
							continue;
						if(isbtag && sys->find("BTAG")!=string::npos)
							continue;

						std::string jobname=*chan+"_"+*en+"_"+*mt+"_"+*sys;
						std::string cmdargs="-c "+*chan+ " -s "+ *sys + " -e " + *en + " -mt " +*mt;
						js->addJob(jobname,cmdargs);

					}
	}
	else if(analyzertype=="W"){
		std::string first="mu_8TeV_80_";
		for(std::vector<std::string>::const_iterator  sys=systematics.begin();sys<systematics.end();sys++){
			std::string jobname=first+*sys;
			std::string cmdargs=" -s "+ *sys;
			js->addJob(jobname,cmdargs);
		}
	}

	js->setIsDummy(dummyrun);
	js->start();

	delete js;
	return 0;
}
