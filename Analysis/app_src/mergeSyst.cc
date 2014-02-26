#include "../interface/systMerger.h"
#include "TtZAnalysis/Tools/interface/optParser.h"
#include "TtZAnalysis/Tools/interface/containerStackVector.h"

int main(int argc, char* argv[]){
    using namespace ztop;
    using namespace std;

    containerStackVector::fastadd=false; //be careful because of parallization

    optParser parser(argc,argv);
    TString outputadd=parser.getOpt<TString>("o","");
    TString inputadd =parser.getOpt<TString>("i","");

    std::vector<std::string> allfiles=parser.getRest<std::string>();

    systMerger merger;
    systMerger::debug=false;

    merger.setInFileAdd(inputadd);
    merger.setOutFileAdd(outputadd);
    merger.setInputStrings(allfiles);
    merger.start();

}
