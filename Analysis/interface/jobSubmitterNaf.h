/*
 * jobSubmitterNaf.h
 *
 *  Created on: 2 Jun 2016
 *      Author: jkiesele
 */

#ifndef TTZANALYSIS_ANALYSIS_INTERFACE_JOBSUBMITTERNAF_H_
#define TTZANALYSIS_ANALYSIS_INTERFACE_JOBSUBMITTERNAF_H_

#include "jobSubmitterBase.h"

namespace ztop{

class jobSubmitterNaf: public jobSubmitterBase{
public:
	jobSubmitterNaf():jobSubmitterBase(){}
	~jobSubmitterNaf(){}

private:
	std::string generateScriptHeader()const;
	std::string getSubmissionExec()const;
	std::string createWorkingBaseDir()const;
	std::string  generateCheckScriptTemplate()const;

};
}



#endif /* TTZANALYSIS_ANALYSIS_INTERFACE_JOBSUBMITTERNAF_H_ */
