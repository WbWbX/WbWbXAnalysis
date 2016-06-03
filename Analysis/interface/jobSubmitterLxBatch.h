/*
 * jobSubmitterLxBatch.h
 *
 *  Created on: 2 Jun 2016
 *      Author: jkiesele
 */

#ifndef TTZANALYSIS_ANALYSIS_INTERFACE_JOBSUBMITTERLXBATCH_H_
#define TTZANALYSIS_ANALYSIS_INTERFACE_JOBSUBMITTERLXBATCH_H_


#include "jobSubmitterBase.h"

namespace ztop{

class jobSubmitterLxBatch: public jobSubmitterBase{
public:
	jobSubmitterLxBatch():jobSubmitterBase(){}
	~jobSubmitterLxBatch(){}

private:
	std::string generateScriptHeader()const{return "exit";}
	std::string getSubmissionExec()const{return "echo";}
	std::string createWorkingBaseDir()const{return "nowd";}
	std::string  generateCheckScriptTemplate()const{return "nowd";}
};
}


#endif /* TTZANALYSIS_ANALYSIS_INTERFACE_JOBSUBMITTERLXBATCH_H_ */
