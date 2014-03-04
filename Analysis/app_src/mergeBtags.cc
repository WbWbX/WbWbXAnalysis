/*
 * mergeBtags.cc
 *
 *  Created on: Mar 4, 2014
 *      Author: kiesej
 */

#include <vector>
#include "TString.h"
#include "TtZAnalysis/Tools/interface/optParser.h"
#include "TtZAnalysis/DataFormats/interface/NTBTagSF.h"


int main(int argc, char* argv[]){
    using namespace ztop;
    using namespace std;

    optParser parser(argc,argv);
    parser.bepicky=true;
    TString outname  = parser.getOpt<TString>("o","out");
    std::vector<TString> instrings_=parser.getRest<TString>();

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
