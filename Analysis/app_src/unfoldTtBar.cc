/*
 * unfold.C
 *
 *  Created on: Sep 15, 2013
 *      Author: kiesej
 */

#include "../interface/TtBarUnfolder.h"
#include "TtZAnalysis/Tools/interface/optParser.h"


/**
 * input: files with fully merged systematics (as you like), not necessarily Canvases
 * output option: -o, otherwise <input>_unfolded
 */
//////////////for compling////////
int main(int argc, char* argv[]){
    using namespace ztop;
    optParser parse(argc,argv);
    parse.bepicky=true;
    TString out   = parse.getOpt<TString>("o","");
    bool moreoutput=parse.getOpt<bool>   ("v",false);
    std::vector<TString> allin = parse.getRest<TString>();


    for(size_t i=0;i<allin.size();i++){
        TtBarUnfolder unf;
        unf.verbose=moreoutput;
        unf.unfold(out,allin.at(i));
    }


    return 0;
}
//namespace
