/*
 * jobSubmitterBase.h
 *
 *  Created on: 2 Jun 2016
 *      Author: jkiesele
 */

#ifndef TTZANALYSIS_ANALYSIS_INTERFACE_JOBSUBMITTERBASE_H_
#define TTZANALYSIS_ANALYSIS_INTERFACE_JOBSUBMITTERBASE_H_

#include <string>
#include <sys/stat.h>
#include <vector>
/**
 * class to handle job submission of analysis jobs on NAF or lxplus batch (TBI)
 *
 * Inherit once to specify topanalysis or wanalyses
 *    Inherit again to specify lxbatch or NAF
 *
 *
 *  Structure:
 *    executable needs to be put in workdir/bin
 *
 */

namespace ztop{
class jobSubmitterBase{
public:
	jobSubmitterBase(){}
	virtual ~jobSubmitterBase(){}

	void start()const;

	void setGenArguments(std::string arg){genarguments_=arg;}



	void setWorkingDir(const std::string &);
	void setExecutable(const std::string &);
	void setSourceDir(const std::string &);

	void addJob(const std::string &j,const std::string &args=""){
		jobnames_.push_back(j);
		arguments_.push_back(args);
	}

protected:
	virtual std::string generateScriptHeader()const=0; //adapt for naf/lxplus

	//skips in inheriting classes
	virtual std::string getSubmissionExec()const=0; //returns qsub for naf

	virtual std::string createWorkingBaseDir()const=0;

	virtual std::string generateCheckScriptTemplate()const=0;

	const std::string& getSourceDir()const{return sourcedir_;}

private:
	//works on jobnames
	void generateSubmitScripts()const;
	//works on jobnames
	void generateCheckScript()const;

	void copySources()const;

	//
	void createDirStructure()const;

	void submitJobs()const;

	void mkdirp(std::string path)const;

	void cpp(std::string from, std::string to)const;

	//
	std::string cmsswbase_;
	std::string sourcedir_;
	std::string
	//source_batchdir_,
	//source_jobscriptdir_,
	source_bindir_,
	source_datadir_,
	source_scriptdir_,
	//source_stdoutdir_,
	source_configdir_;


	std::string workingdir_;
	std::string
	batchdir_,
	jobscriptdir_,
	bindir_,
	datadir_,
	scriptdir_,
	stdoutdir_,
	configdir_;

	std::string executable_; //the one that is supposed to be run (e.g. analyse)
	//std::string submitexec_; //the one that subits to batch (e.g. qsub)
	std::string genarguments_;
	std::vector<std::string > jobnames_,arguments_;
};

/*
 */

}



#endif /* TTZANALYSIS_ANALYSIS_INTERFACE_JOBSUBMITTERBASE_H_ */
