#include "WbWbXAnalysis/Analysis/interface/AnalysisUtils.h"
#include "WbWbXAnalysis/Tools/interface/fileReader.h"

#include "WbWbXAnalysis/Tools/interface/applicationMainMacro.h"
#include "WbWbXAnalysis/Analysis/interface/WbWbXAnalyzer.h"

invokeApplication(){

    //this would usually run for each systematic variation on batch, which is why it does not
    //contain any plotting, but only creation of histogrammed data
    //output will be stored in CWD


    using namespace ztop;
    TString Syst   = parser->getOpt<TString>   ("s","nominal","systematic variation <var>_<up/down>, default: nominal");


    TString cmssw_base=getenv("CMSSW_BASE");
    TString scram_arch=getenv("SCRAM_ARCH");
    TString batchbase = getenv("ANALYSE_BATCH_BASE");

    TString configfile = cmssw_base+"/src/WbWbXAnalysis/Analysis/configs/WbWbX_configs/analysis_config.txt";



    fileReader fr;
    fr.setComment("$");
    fr.setDelimiter(",");
    fr.setStartMarker("[parameters-begin]");
    fr.setEndMarker("[parameters-end]");
    fr.readFile(configfile.Data());
    //some more parameters that one might want to parse (anything really)

    WbWbXAnalyzer ana;

    ana.setDataSetDirectory(fr.getValue<TString>("inputFilesDir"));
    ana.setLumi(100000);
    ana.setSyst(Syst);

    //now running it:
    ana.readFileList(configfile.Data());
    ana.setOutputFileName("WbWbX_output");//systematics get added automatically


    return ana.start();

}
