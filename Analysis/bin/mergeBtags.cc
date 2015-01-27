/*
 * mergeBtags.cc
 *
 *  Created on: Mar 4, 2014
 *      Author: kiesej
 */

#include <vector>
#include "TString.h"
#include "TtZAnalysis/DataFormats/interface/NTBTagSF.h"

#include "TtZAnalysis/Tools/interface/applicationMainMacro.h"

invokeApplication(){
    using namespace ztop;
    using namespace std;

    bool debug=true;

    parser->bepicky=true;
    TString outname  = parser->getOpt<TString>("o","out","output file name <name>_btags.root (default out)");
    std::vector<TString> instrings_=parser->getRest<TString>();

    parser->doneParsing();
    if(instrings_.size()<2){
        std::cout << "need minimum 2 input files to merge" <<std::endl;
        parser->coutHelp();
    }
    NTBTagSF btags;


    //check if file exists, if yes, assume btags have been recreated and merge

    outname+="_btags.root";

    if(instrings_.size()>0){
        std::ifstream OutFileTest(instrings_.at(0).Data());

        if(OutFileTest) { //btags were created
            cout << "systMerger: merging btag SF" <<endl;
            for(size_t i=0;i<instrings_.size();i++){
                TString filename=instrings_.at(i);

                NTBTagSF temp;

                std::ifstream OutFileTest2(filename.Data());
                if(OutFileTest2){
                    if(debug) cout << "adding from file " << filename <<endl;
                    temp.readFromTFile(filename);
                    btags=btags+temp;
                }
                else{
                    cout << "btag file " << filename << " not found" << endl;
                }
            }


            btags.writeToTFile(outname);
        }
        else{
            std::cout << "Did not find a valid b-tag file. Not merging b-tag info" << std::endl;
            return -1;
        }

    }
    else{
        std::cout << "Did not find a valid b-tag file. Not merging b-tag info" << std::endl;
        return -1;
    }
    return 0;

}
