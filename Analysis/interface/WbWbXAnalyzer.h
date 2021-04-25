/*
 * WbWbXAnalyzer.h
 *
 *  Created on: 25 Apr 2021
 *      Author: jkiesele
 */

#ifndef WBWBXANALYSIS_ANALYSIS_INTERFACE_WBWBXANALYZER_H_
#define WBWBXANALYSIS_ANALYSIS_INTERFACE_WBWBXANALYZER_H_


#include "basicAnalyzer.h"

namespace ztop{

class WbWbXAnalyzer: public basicAnalyzer{

public:
    WbWbXAnalyzer();

    void analyze(size_t i);//the actual meat

    //depends what else is needed
    fileForker::fileforker_status writeOutput(){
        writeHistos();
        return ff_status_child_success;
    }

    int start();


private:

};

}//namespace
#endif /* WBWBXANALYSIS_ANALYSIS_INTERFACE_WBWBXANALYZER_H_ */
