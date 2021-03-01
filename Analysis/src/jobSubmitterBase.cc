/*
 * jobSubmitterBase.cc
 *
 *  Created on: 2 Jun 2016
 *      Author: jkiesele
 */



#include "../interface/jobSubmitterBase.h"
#include "fstream"
#include "WbWbXAnalysis/Tools/interface/textFormatter.h"
#include <stdexcept>
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"

namespace ztop{


void jobSubmitterBase::setWorkingDir(const std::string & in){

	workingdir_=in;
	workingdir_=createWorkingBaseDir()+"/"+workingdir_;
	batchdir_=workingdir_+"/batch";
	jobscriptdir_=workingdir_+"/jobscripts";
	bindir_=workingdir_+"/bin";
	datadir_=workingdir_+"/data";
	scriptdir_=workingdir_+"/scripts";
	stdoutdir_=workingdir_+"/stdout";
	configdir_=workingdir_+"/configs";

}

void jobSubmitterBase::setSourceDir(const std::string & in){
	sourcedir_=in;

	source_datadir_=sourcedir_+"/data";
	source_scriptdir_=sourcedir_+"/scripts";

	source_configdir_=sourcedir_+"/configs";

}

void jobSubmitterBase::setExecutable(const std::string & in){

	executable_=textFormatter::getFilename(in);
	source_bindir_=textFormatter::getFileDir(in);
	if(source_bindir_==executable_)
		source_bindir_="";
}

void jobSubmitterBase::mkdirp(std::string path)const{
	int status= mkdir(path.data(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if(status){
		if(status == -1)
			throw std::runtime_error("jobSubmitterBase::mkdirp: directory exists: " + path+" "+toString(status));
		else
			throw std::runtime_error("jobSubmitterBase::mkdirp: failed creating " + path+" "+toString(status));
	}
}

void jobSubmitterBase::cpp(std::string from, std::string to)const{
	//make stupid system calls...
	int ret=system(((std::string)"cp -r " +from +" "+to).data());
	if(ret)
		throw std::runtime_error((std::string)"jobSubmitterBase::cp " +from +" "+to+" failed "+toString(ret));
}

void jobSubmitterBase::copySources()const{
	//needs to be a system call
	std::cout << "creating independent executable package" <<std::endl;
	std::cout << "(code can be compiled while jobs are running)" <<std::endl;

	std::string command="makePackage.sh "+source_bindir_;
	command+= executable_;
	command+= " ";
	command+= bindir_+"/";
	command+= executable_;
	system(command.data());

	std::cout << "copying snapshot of configuration and data" <<std::endl;

	cpp(source_datadir_+"/*",datadir_+"/");
	cpp(source_scriptdir_+"/*",scriptdir_+"/");
	cpp(source_configdir_+"/*",configdir_+"/");

}



void jobSubmitterBase::createDirStructure()const{
	mkdirp(workingdir_);
	mkdirp(batchdir_);
	mkdirp(jobscriptdir_);
	mkdirp(bindir_);
	mkdirp(datadir_);
	mkdirp(scriptdir_);
	mkdirp(stdoutdir_);
	mkdirp(configdir_);

}

//jobnames_ has been filled
void jobSubmitterBase::generateSubmitScripts()const{
	if(jobnames_.size()<1)
		throw std::logic_error("jobSubmitterBase::generateSubmitScripts: no jobnames defined");

	std::string outpath=jobscriptdir_+"/";

	for(size_t i=0;i<jobnames_.size();i++){
		const std::string & jobname=jobnames_.at(i);
		std::ofstream outfile(outpath+jobname);
		outfile << generateScriptHeader();
		outfile << std::endl;
		outfile << "cd " << workingdir_<< std::endl;
		outfile << "export ANALYSE_BATCH_BASE=" << workingdir_<< std::endl;
		outfile << "exec > \"stdout/" << jobname << "\" 2>&1"<< std::endl;
		outfile << "./bin/" << executable_ << " " << arguments_.at(i)<<" "<< genarguments_<< std::endl;
		outfile.close();
		system(("chmod +x "+outpath+jobname).data());
	}

}


void jobSubmitterBase::generateCheckScript()const{
	std::string outname=workingdir_+"/check.sh";
	std::ofstream outfile(outname);
	outfile << "#!/bin/zsh\n\ncd " << workingdir_<< std::endl;
	outfile << generateCheckScriptTemplate();
	//just parse rest
	outfile.close();
	system(("chmod +x "+outname).data());
}

void jobSubmitterBase::submitJobs()const{
	std::string cdbatch="cd "+batchdir_+"; ";
	for(size_t i=0;i<jobnames_.size();i++){
		std::string command=cdbatch+ getSubmissionExec() +" "+jobscriptdir_+"/"+jobnames_.at(i);
		if(!isDummyRun())
			system((command).data());
		else
			std::cout << "dummy run: " << command << std::endl;
	}
}

void jobSubmitterBase::start()const{
	createDirStructure();
	copySources();
	generateSubmitScripts();
	generateCheckScript();
	submitJobs();
}
}//ns
