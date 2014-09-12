#ifndef MainAnalyzer_h
#define MainAnalyzer_h

#include "TString.h"
#include "TopAnalysis/ZTopUtils/interface/PUReweighter.h"
#include "TtZAnalysis/Tools/interface/containerStackVector.h"
#include "TtZAnalysis/DataFormats/interface/NTJERAdjuster.h"
#include "TtZAnalysis/DataFormats/interface/NTJECUncertainties.h"
#include "TtZAnalysis/DataFormats/interface/NTBTagSF.h"
#include "pdfReweighter.h"
#include "Pipes.h"
#include "scalefactors.h"
#include "reweightfunctions.h"
#include <cstdlib>
#include "AnalysisUtils.h"

namespace ztop{class NTEvent;}

///// now available: removeContribution; use it to run on the systematics etc (only signal) or pdf stuff (remove nominal,for(i) add pdf[i], xsec, remove pdf[i])

//// run in batch mode otherwise it is dramatically slowed down by all the drawing stuff; the latter might also produce seg violations in the canvas libs.
//// name Z contribution "Z" or something similar for generator stuff and so on

//maybe the container filling uses a lot of cpu time, check that! checked. inlined and pow->sq




class MainAnalyzer{

public:
    MainAnalyzer();
    MainAnalyzer(const MainAnalyzer &);

    ~MainAnalyzer();

    void setChannel(TString chan){
        if(chan.Contains("mumu")) b_mumu_=true;
        else if(chan.Contains("emu")) b_emu_=true;
        else if(chan.Contains("ee")) b_ee_=true;
        else{
            std::cout << "channel wrongly set! exit" << std::endl;
            std::exit(EXIT_FAILURE);
        }
        channel_=chan;}
    void setSyst(TString syst){syst_=syst;}
    void setEnergy(TString e){if(e.Contains("7TeV")) is7TeV_=true; energy_=e;}
    void setOutFileAdd(TString o){outfileadd_=o;}
    void setTopMass(TString topmass){topmass_=topmass;}

    TString getOutFileName(){
        if(outfileadd_=="") return channel_+"_"+energy_+"_"+topmass_+"_"+syst_;
        else return channel_+"_"+energy_+"_"+topmass_+"_"+syst_+"_"+outfileadd_;
    }

    void setOutDir(TString dir){
        if(dir.EndsWith("/"))
            outdir_=dir;
        else
            outdir_=dir+"/";
    }

    TString getOutPath(){return outdir_+getOutFileName();}
    TString getOutDir(){return outdir_;}

    void setBTagSFFile(TString file){btagsffile_=file;}

    void setLumi(double Lumi){lumi_=Lumi;}

    // void replaceInAllOptions(TString replace, TString with){name_.ReplaceAll(replace,with);analysisplots_.setName(name_);}
    TString getChannel(){return channel_;}
    TString getSyst(){return syst_;}
    TString Energy(){return energy_;}

    float createNormalizationInfo(TFile *f,bool isMC,size_t anaid);

    void setMaxChilds(size_t childs){maxchilds_=childs;}
    void analyze(TString, TString, int, size_t,size_t i=0);
    void analyze(size_t i);

    void setFileList(TString fl){filelist_=fl;}
    void setFilePostfixReplace(TString file,TString pf,bool clear=false){
        if(clear){ fwithfix_.clear();ftorepl_.clear();}
        ftorepl_.push_back(file); fwithfix_.push_back(pf);
    }

    void setFilePostfixReplace(std::vector<TString> files,std::vector<TString> pf){
        if(files.size() != pf.size()){std::cout << "setFilePostfixReplace: vectors have to be same size, exit!" << std::endl; std::exit(EXIT_FAILURE);}
        ftorepl_=files;fwithfix_=pf;
    }


    void setDataSetDirectory(TString dir){datasetdirectory_=dir;}
    void setShowStatus(bool show){showstatus_=show;}
    void setOnlySummary(bool show){onlySummary_=show;}
    void setTestMode(bool test){testmode_=test;}

    void setMode(TString mode){mode_=mode;}

    //COMPAT
    //void setShowStatusBar(bool show){showstatus_=show;}

    ztop::containerStackVector * getPlots(){return & allplotsstackvector_;}

    int start();
    //  void start(TString);

    void clear(){allplotsstackvector_.clear();}

    void setDiscriminatorInputFile(const std::string filename){discrInput_=filename;}
    void setUseDiscriminators(bool use){usediscr_=use;}

    ztop::PUReweighter * getPUReweighter(){return & puweighter_;}
    ztop::NTJERAdjuster * getJERAdjuster(){return & jeradjuster_;}
    ztop::NTJECUncertainties * getJECUncertainties(){return & jecuncertainties_;}
    ztop::NTBTagSF * getBTagSF(){return &btagsf_;}
    ztop::scalefactors * getMuonSF(){return &muonsf_;}
    ztop::scalefactors * getElecSF(){return &elecsf_;}
    ztop::scalefactors * getTriggerSF(){return &triggersf_;}
    ztop::scalefactors * getTrackingSF(){return &trackingsf_;}

    ztop::scalefactors * getElecEnergySF(){return &elecenergysf_;}
    ztop::scalefactors * getMuonEnergySF(){return &muonenergysf_;}

    ztop::reweightfunctions * getTopPtReweighter(){return &topptReweighter_;}

    ztop::pdfReweighter * getPdfReweighter(){return &pdfweighter_;}

    MainAnalyzer & operator= (const MainAnalyzer &);

    void setFakeDataStartNEntries(float startdiv){fakedata_startentries_=startdiv;}

    ///analysis helper functions

    bool checkTrigger(std::vector<bool> * ,ztop::NTEvent * , bool , size_t);

    /**
     * for child processes
     * if you report an error, please add its return value to the description
     * to the analyse executable
     */
    void reportError(int , size_t );
    /**
     * for child processes
     * reports the Status (% of events already processed) to the main program
     */
    void reportStatus(Long64_t entry,Long64_t nEntries, size_t anaid);


    /**
     * set the path to the inital configuration file (if any) to use it for further input to
     * event loop or something else
     *
     * this should not be done normally and is only for special cases
     */

   void setPathToConfigFile(const TString & path){pathtoconffile_=path;}

private:



    void copyAll(const MainAnalyzer &);
    void readFileList(); //run automatically when start() is called

    TString replaceExtension(TString filename );

    bool showstatus_,onlySummary_,testmode_,singlefile_;

    TString name_,dataname_,channel_,syst_,energy_;
    bool b_ee_,b_emu_,b_mumu_,is7TeV_;
    TString datasetdirectory_;
    double lumi_;
    /**
     * default: NTEvent, can be changed e.g. to use weights of different pdfs
     * incorporated in the same ntuple
     */
    TString eventbranch_;

    TString mode_;

    TString filelist_;
    std::vector<TString> fwithfix_,ftorepl_;
    int freplaced_;

    ztop::PUReweighter  puweighter_;
    ztop::NTJERAdjuster  jeradjuster_;
    ztop::NTJECUncertainties  jecuncertainties_;
    ztop::NTBTagSF  btagsf_;

    ztop::pdfReweighter pdfweighter_;

    ztop::containerStackVector  allplotsstackvector_;

    size_t filecount_;
    TString outfileadd_;
    TString outdir_;

    TString btagsffile_;

    //for scalefactors provided in THXX format:
    ztop::scalefactors elecsf_,muonsf_,triggersf_,elecenergysf_,muonenergysf_,trackingsf_;
    ztop::reweightfunctions topptReweighter_;

    //for parallel stuff

    std::vector<TString> infiles_,legentries_;
    std::vector<int> colz_;
    std::vector<double> norms_;
    std::vector<size_t> legord_;
    std::vector<bool> issignal_;

    ///communication pipes

    IPCPipes<int> p_idx;
    IPCPipes<int> p_finished;

    IPCPipes<int> p_allowwrite;
    IPCPipes<int> p_askwrite;

    IPCPipes<int> p_status; //not  implemented, yet

    bool writeAllowed_;
    int usepdfw_;

    // bool askForWrite();
    int  checkForWriteRequest();
    // void blockWrite();
    // void freeWrite();

    size_t maxchilds_;
    pid_t PID_;
    std::vector<pid_t> daughPIDs_;

    TString topmass_;

    //for discriminators
    std::string discrInput_;
    bool usediscr_;



    /////path to initial configuration file (in case adjustments are being made afterwards)
    TString pathtoconffile_;

    float fakedata_startentries_;

    template <class T>
    std::vector<T*> produceCollection(std::vector<T> * t){
        std::vector<T*> out;
        for(size_t i=0;i<t->size();i++)
            out.push_back(&(t->at(i)));
        std::sort(out.begin(),out.end(), ztop::comparePt<T*> );
        return out;
    }


    void setCacheProperties(TTree * t,const TString& inputtree)const;


};

#endif
