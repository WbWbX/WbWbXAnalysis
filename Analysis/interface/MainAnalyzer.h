#ifndef Analyzer_h
#define Analyzer_h

#include "TtZAnalysis/DataFormats/src/classes.h" //gets all the dataformats
//#include "TtZAnalysis/DataFormats/interface/elecRhoIsoAdder.h"
#include "TtZAnalysis/plugins/reweightPU.h"
#include "TtZAnalysis/plugins/leptonSelector2.h"
#include "TtZAnalysis/Tools/interface/miscUtils.h"
#include "TtZAnalysis/Tools/interface/containerStackVector.h"
#include "TtZAnalysis/Tools/interface/containerUF.h"
#include "TtZAnalysis/DataFormats/interface/NTJERAdjuster.h"
#include "TtZAnalysis/DataFormats/interface/NTJECUncertainties.h"
#include "TtZAnalysis/DataFormats/interface/NTBTagSF.h"
//#include "TtZAnalysis/Tools/interface/bTagSF.h"
#include "TTree.h"
#include "TFile.h"
#include <fstream>

#include <stdlib.h>

bool testmode=true;


std::vector<TString> dycontributions;

namespace ztop{
  typedef std::vector<ztop::NTElectron>::iterator NTElectronIt;
  typedef std::vector<ztop::NTMuon>::iterator NTMuonIt;
  typedef std::vector<ztop::NTJet>::iterator NTJetIt;
  typedef std::vector<ztop::NTTrack>::iterator NTTrackIt;
  typedef std::vector<ztop::NTSuClu>::iterator NTSuCluIt;
}

///// now available: removeContribution; use it to run on the systematics etc (only signal) or pdf stuff (remove nominal,for(i) add pdf[i], xsec, remove pdf[i])

//// run in batch mode otherwise it is dramatically slowed down by all the drawing stuff; the latter might also produce seg violations in the canvas libs.
//// name Z contribution "Z" or something similar for generator stuff and so on

//maybe the container filling uses a lot of cpu time, check that! checked. inlined and pow->sq




class MainAnalyzer{

public:
  MainAnalyzer(){
    dataname_="data";
  }
  MainAnalyzer(const MainAnalyzer &);
  MainAnalyzer(TString Name, TString additionalinfo){
    name_=Name;additionalinfo_=additionalinfo;
    dataname_="data";
  }
  ~MainAnalyzer(){}

  void setName(TString Name, TString additionalinfo){
    name_=Name;additionalinfo_=additionalinfo;
    analysisplots_.setName(Name);
  }
  void setOptions(TString Name, TString additionalinfo, TString systName){
    name_=Name;
    additionalinfo_=additionalinfo;
    analysisplots_.setName(Name);
    analysisplots_.setSyst(systName);
  }
  void setLumi(double Lumi){lumi_=Lumi;}

  void replaceInAllOptions(TString replace, TString with){name_.ReplaceAll(replace,with);analysisplots_.setName(name_);}
  TString getName(){return name_;}


 
  void analyze(TString, TString, int, double);

  void setFileList(TString filelist){filelist_=filelist;}
  void setDataSetDirectory(TString dir){datasetdirectory_=dir;}
  void setShowStatusBar(bool show){showstatusbar_=show;}

  ztop::container1DStackVector * getPlots(){return & analysisplots_;}

  void start(TString ident="def");
  //  void start(TString);

  void clear(){analysisplots_.clear();}

  void setAdditionalInfoString(TString add){additionalinfo_=add;} //!for adding things like JEC up or other systematics options

  ztop::PUReweighter * getPUReweighter(){return & puweighter_;}

  ztop::NTJERAdjuster * getJERAdjuster(){return & jeradjuster_;}
  ztop::NTJECUncertainties * getJECUncertainties(){return & jecuncertainties_;}

  ztop::NTBTagSF * getBTagSF(){return &btagsf_;}

  MainAnalyzer & operator= (const MainAnalyzer &);


private:

  void copyAll(const MainAnalyzer &);

  bool isInInfo(TString s){return additionalinfo_.Contains(s);}
  //bool triggersContain(TString , ztop::NTEvent *);

  bool showstatusbar_;

  TString name_,dataname_;
  TString datasetdirectory_;
  double lumi_;

  TString filelist_;
  
  ztop::PUReweighter  puweighter_;
  ztop::NTJERAdjuster  jeradjuster_;
  ztop::NTJECUncertainties  jecuncertainties_;
  ztop::NTBTagSF  btagsf_;

  ztop::container1DStackVector  analysisplots_;

  TString additionalinfo_;

};

#endif
