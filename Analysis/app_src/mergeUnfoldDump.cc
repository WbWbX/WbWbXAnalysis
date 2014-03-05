/*
 * mergeUnfoldDump.cc
 *
 *  Created on: Nov 26, 2013
 *      Author: kiesej
 */




/*
 * should do the whole thing one day (for inclusive xsec measurement)
 * -> merges systematics as given to mergeSyst
 * -> unfolds everything
 * -> dumps all plots and resulting systematics on the web
 */

#include "../interface/TtBarUnfolder.h"
#include "../interface/systMerger.h"
#include "../interface/wwwDumper.h"
#include "TtZAnalysis/Tools/interface/optParser.h"

namespace application{
int mergeUnfoldDump(std::vector<std::string> infiles, TString outfile, TString title,TString descr,
         bool unfoldverbose,  TString inadd=""){

    using namespace ztop;

    systMerger merger;
    systMerger::debug=false;
    merger.setInFileAdd(inadd);
    merger.setOutFileAdd(outfile);
    merger.setInputStrings(infiles);
    std::vector<TString> outfilenames=merger.start();

    for(size_t i=0;i<outfilenames.size();i++){
        TString file=outfilenames.at(i);
        std::cout << file << std::endl;
        file.ReplaceAll(".root","");
        TtBarUnfolder unf;
        unf.verbose=unfoldverbose;
        TString unfoldoutputstring= unf.unfold("",file+".root");
        unfoldoutputstring.ReplaceAll("\n","<br/>");
        wwwDumper dumper;
        dumper.dumpToWWW(file+".root",title+"_"+file,descr+"<br/><br/>"+unfoldoutputstring);

    }

    return 0;
}

}//application

int main(int argc, char* argv[]){
    //get all options
    using namespace ztop;
    optParser parse(argc,argv);
    parse.bepicky=true;

    TString outfile      =  parse.getOpt<TString>("o","","specify output file name");
    TString title        =  parse.getOpt<TString>("t","","title for webpage");
    TString descr        =  parse.getOpt<TString>("d","","description");
    TString inadd        =  parse.getOpt<TString>("i","","string added to input file names"); //i
    bool unfoldverbose   =  parse.getOpt<bool>("v",false,"unfold in verbose mode (default: false)");  //v

    std::vector<std::string> infiles = parse.getRest<std::string>();



    ///check if everything is ok here!
    if(infiles.size() < 1 || descr.Length() <1 || title.Length()<1){
        parse.coutHelp();
        return -1;
    }



    return application::mergeUnfoldDump(infiles,outfile,title,descr,unfoldverbose,inadd);

}
