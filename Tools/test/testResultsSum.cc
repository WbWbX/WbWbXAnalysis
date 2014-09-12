/*
 * testResultsSum.cc
 *
 *  Created on: Jun 27, 2014
 *      Author: kiesej
 */


#include "../interface/resultsSummary.h"
#include "TString.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"

int main(){
	bool dummy=false;
    using namespace ztop;
    resultsSummary rs;
    rs.addEntry("nominal",100,10,-10);
    rs.addEntry("bla_up",102,10,-10);
    rs.addEntry("bla_down",97,10,-10);
    rs.addEntry("blu_up",105,10,-10);
    rs.addEntry("blu_down",106,10,-10);
    rs.addEntry("bli_up",103,10,-10);
    rs.addEntry("bli_down",102,10,-10);

    texTabler tt(" l | c");

    resultsSummary::debug=true;

    for(size_t i=0;i<rs.nEntries();i++)
        tt << rs.getEntryName(i) << rs.getValueString(i,dummy);

    std::cout << tt.getTable() <<std::endl;

    std::vector<TString> sysnames;
    sysnames<<"bla"<<"blu";

    rs=rs.mergeVariations("nominal",sysnames,"merged",false);



    tt.clear();
    for(size_t i=0;i<rs.nEntries();i++)
        tt << rs.getEntryName(i) << rs.getValueString(i,dummy);

    std::cout << tt.getTable() <<std::endl;


    tt.clear();
    std::cout << "creating tot" <<std::endl;
    resultsSummary sys=rs.createTotalError();

    std::cout << "adding entries" <<std::endl;
    for(size_t i=0;i<sys.nEntries();i++)
        tt << sys.getEntryName(i) << sys.getValueString(i,dummy);

    std::cout << "getting table" <<std::endl;
    std::cout << tt.getTable() <<std::endl;

    return 0;
}

