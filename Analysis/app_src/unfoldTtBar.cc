/*
 * unfold.C
 *
 *  Created on: Sep 15, 2013
 *      Author: kiesej
 */

#include "../interface/TtBarUnfolder.h"
#include "TtZAnalysis/Tools/interface/optParser.h"
#include "TString.h"

/**
 * input: files with fully merged systematics (as you like), not necessarily Canvases
 * output option: -o, otherwise <input>_unfolded
 */
//////////////for compling////////
int main(int argc, char* argv[]){
    using namespace ztop;
    optParser parse(argc,argv);
    parse.bepicky=true;
    TString out   = parse.getOpt<TString>("o","","string to add to outfile");
    bool moreoutput=parse.getOpt<bool>   ("v",false,"switches on verbose mode for unfolding");
    bool printpdfs=parse.getOpt<bool>   ("P",false,"print unfolding output as pdfs (switch)");
    bool bbb=parse.getOpt<bool>   ("BBB",false,"set all to bin-by-bin unfolding");
    std::vector<TString> allin = parse.getRest<TString>();

    parse.doneParsing();
    if(allin.size()<1){
        std::cout << "specify at leaste one input file" <<std::endl;
        parse.coutHelp();
    }
    TString outputstring;
    for(size_t i=0;i<allin.size();i++){
        TtBarUnfolder unf;
        unf.verbose=moreoutput;
        unf.printpdfs=printpdfs;
        unf.setUnits("pb");
        unf.setBinByBin(bbb);
        //get channel
        unf.addToBeCorrectedPlotID("total step");
        if(allin.at(i).BeginsWith("emu_")){
            unf.setBRCorr(0.03284);
        }
        else if(allin.at(i).BeginsWith("mumu_")){
            unf.setBRCorr(0.01668);
        }
        else if(allin.at(i).BeginsWith("ee_")){
            unf.setBRCorr(0.01616);
        }
        outputstring+=unf.unfold(out,allin.at(i));
    }

    std::cout << outputstring << std::endl;

    return 0;
}
//namespace
