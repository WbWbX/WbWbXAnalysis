/*
 * jobSubmitterNaf.cc
 *
 *  Created on: 2 Jun 2016
 *      Author: jkiesele
 */



#include "../interface/jobSubmitterNaf.h"
#include <fstream>
#include <sstream>

namespace ztop{

std::string  jobSubmitterNaf::generateScriptHeader()const{
	std::string ret;
	ret+="#!/bin/sh\n";
	ret+="#$ -S /bin/sh\n";
	ret+="#$ -l site=hh\n";
	ret+="#$ -l distro=sld6\n";
	ret+="#$ -l h_rt=02:55:00\n";
	ret+="#$ -l h_vmem=2047M\n";
	ret+="#$ -j y\n";
	ret+="#$ -m a\n";
	ret+="#$ -cwd -V\n";
	ret+="#$ -pe local 6 -R y\n";
	ret+="#$ -P af-cms\n";
	ret+="export OMP_NUM_THREADS=1\n";
	ret+="\n";
	return ret;
}
std::string  jobSubmitterNaf::generateCheckScriptTemplate()const{
	std::ifstream infile(getSourceDir()+"/templates/check2.sh");
	std::string out;
	while(infile){
		std::string buf;
		std::getline(infile,buf);
		out+=buf;
		out+='\n';
	}
	infile.close();
	return out;
}

std::string  jobSubmitterNaf::getSubmissionExec()const{
	return "qsub";
}
std::string  jobSubmitterNaf::createWorkingBaseDir()const{
	system("mkdir -p /nfs/dust/cms/user/$USER/AnalysisWorkdir");
	system("cd $CMSSW_BASE/src/TtZAnalysis/Analysis; ln -s /nfs/dust/cms/user/$USER/AnalysisWorkdir workdir > /dev/null &2>1");
	std::string out=getenv("CMSSW_BASE");
	out+="/src/TtZAnalysis/Analysis/workdir";
	return out;
}

}
