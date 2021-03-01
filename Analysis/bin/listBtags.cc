/*
 * listBtags.cc
 *
 *  Created on: Jun 11, 2014
 *      Author: kiesej
 */



#include "WbWbXAnalysis/DataFormats/interface/NTBTagSF.h"
#include "TString.h"
#include "WbWbXAnalysis/Tools/interface/optParser.h"

#include "WbWbXAnalysis/Tools/interface/applicationMainMacro.h"

invokeApplication(){
    using namespace ztop;
    using namespace std;

    bool debug=true;

    TString namescontain = parser->getOpt<TString>("-s","","select only names containing <> TBI");
    std::vector<TString> instrings=parser->getRest<TString>();

    parser->doneParsing();
    if(instrings.size()!=1){
        std::cout << "need exactly one input file" <<std::endl;
        parser->coutHelp();
    }
    NTBTagSF btags;
    btags.readFromTFile(instrings.at(0));

    btags.listAllSampleNames();

    return 0;
}
