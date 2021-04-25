
#include "../interface/WbWbXAnalyzer.h"
#include "../interface/tTreeHandler.h"
#include "../interface/tBranchHandler.h"

namespace ztop{

WbWbXAnalyzer::WbWbXAnalyzer():basicAnalyzer(){

}


//this will parallelise over input samples
void WbWbXAnalyzer::analyze(size_t id){

    const TString fullInputPath = datasetdirectory_+getInputFileName() ;

    tTreeHandler tree( datasetdirectory_+inputfile_ ,"Friends");
    tTreeHandler *t =&tree;
    tBranchHandler<float>     b_reco_mtw(t,"Reco_mtw");

    histo1D::c_makelist=true;//automatic bookkeeping of histograms

    histo1D myplot({0,100,150,190,200},"Reco_mtw","m_{tW} [GeV]","# Events");

    float JESscale = 1;
    if(getSyst() == "JES_up")
        JESscale = 1.5;
    else if(getSyst() == "JES_down")
        JESscale = 0.5;
    /* some more modifiers here */
    else if(getSyst() == "nominal")
    {}//nothing
    else
        throw std::runtime_error("WbWbXAnalyzer::analyze: systematics string not recognized");


    //set the norm (might mean reading some stuff from the input file or similar)
    float xsec = norms_.at(id); //this is the cross section from the input file
    //norm_ = xsec * lumi_; //other stuff might be necessary
    //here I am not doing it so that the plot looks ok, since it's just MC


    //apply certain things only to MC
    if(!isMC_){
        JESscale = 1.;
        //norm_=1;
    }

    for(Long64_t entry=0;entry<t->entries();entry++){
        reportStatus(entry,t->entries()); //keep this here

        t->setEntry(entry);

        float myweight=1;

        myplot.fill(JESscale * (*b_reco_mtw.content()),myweight);


    }


    //plots are automatically saved, keep all of this
    histo1DUnfold::flushAllListed();//just in case we end up using them
    processEndFunction();

}

int WbWbXAnalyzer::start(){

    allplotsstackvector_.setName(getOutFileName());
    allplotsstackvector_.setSyst(getSyst());

    setOutputFileName((getOutPath()+".ztop").Data());

    int interval=5;

    if(runParallels(interval)==fileForker::ff_status_parent_success)
        return 1;
    else return -99;
}


}
