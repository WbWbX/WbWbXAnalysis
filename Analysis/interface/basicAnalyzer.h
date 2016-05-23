/*
 * basicAnalyzer.h
 *
 *  Created on: 20 May 2016
 *      Author: kiesej
 */

#ifndef TTZANALYSIS_ANALYSIS_INTERFACE_BASICANALYZER_H_
#define TTZANALYSIS_ANALYSIS_INTERFACE_BASICANALYZER_H_

#include "TtZAnalysis/Tools/interface/fileForker.h"
#include "TtZAnalysis/Tools/interface/histoStackVector.h"
#include "TString.h"
#include <vector>


/**
 * quick and dirty generic analysis interface
 */
namespace ztop{
class basicAnalyzer : public fileForker{
public:
	basicAnalyzer();


	void readFileList(const std::string& );
	virtual void analyze(size_t )=0;



private:
	void process();

protected:

	fileForker::fileforker_status writeHistos();

	ztop::histoStackVector  allplotsstackvector_;

	bool createOutFile()const{return true;}

	std::vector<TString> infiles_,legentries_;
	std::vector<int> colz_;
	std::vector<double> norms_;
	std::vector<size_t> legords_;
	std::vector<bool> issignal_;
	std::vector<TString> extraopts_;

	///child variables
	TString inputfile_;
	TString legendname_;
	int col_;
	double norm_;
	size_t legorder_;
	bool signal_;
};



}
#endif /* TTZANALYSIS_ANALYSIS_INTERFACE_BASICANALYZER_H_ */
