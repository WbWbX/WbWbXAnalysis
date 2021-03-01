#ifndef MainAnalyzer_h
#define MainAnalyzer_h

#include "TString.h"
#include "TopAnalysis/ZTopUtils/interface/PUReweighter.h"
#include "WbWbXAnalysis/Tools/interface/histoStackVector.h"
#include "WbWbXAnalysis/DataFormats/interface/NTJERAdjuster.h"
#include "WbWbXAnalysis/DataFormats/interface/NTJECUncertainties.h"
#include "WbWbXAnalysis/DataFormats/interface/NTBTagSF.h"
#include "pdfReweighter.h"
#include "scalefactors.h"
#include "reweightfunctions.h"
#include <cstdlib>
#include "AnalysisUtils.h"
#include "../interface/AnalysisUtils.h"
#include "../interface/NTFullEvent.h"
#include "../interface/ttbarControlPlots.h"
#include "../interface/ttXsecPlots.h"
#include "../interface/ZControlPlots.h"
#include "../interface/leptonSelector2.h"

#include "WbWbXAnalysis/DataFormats/interface/NTLepton.h"
#include "WbWbXAnalysis/DataFormats/interface/NTMuon.h"
#include "WbWbXAnalysis/DataFormats/interface/NTElectron.h"
#include "WbWbXAnalysis/DataFormats/interface/NTJet.h"
#include "WbWbXAnalysis/DataFormats/interface/NTMet.h"
#include "WbWbXAnalysis/DataFormats/interface/NTEvent.h"
#include "WbWbXAnalysis/DataFormats/interface/NTIsolation.h"
#include "WbWbXAnalysis/DataFormats/interface/NTTrack.h"
#include "WbWbXAnalysis/DataFormats/interface/NTSuClu.h"
#include "WbWbXAnalysis/DataFormats/interface/NTTrigger.h"
#include "WbWbXAnalysis/DataFormats/interface/NTTriggerObject.h"


#include "WbWbXAnalysis/DataFormats/interface/NTVertex.h"

#include "WbWbXAnalysis/DataFormats/interface/NTGenLepton.h"
#include "WbWbXAnalysis/DataFormats/interface/NTGenParticle.h"
#include "WbWbXAnalysis/DataFormats/interface/NTGenJet.h"
#include "WbWbXAnalysis/DataFormats/interface/NTLorentzVector.h"


#include "WbWbXAnalysis/DataFormats/interface/helpers.h"

#include "../interface/analysisPlotsMlbMt.h"
#include "../interface/analysisPlotsTtbarXsecFit.h"

#include "../interface/discriminatorFactory.h"

#include "../interface/tBranchHandler.h"

#include "WbWbXAnalysis/DataFormats/interface/NTSystWeight.h"

#include <sys/types.h>


#include "WbWbXAnalysis/Tools/interface/fileReader.h"

#include "WbWbXAnalysis/DataFormats/interface/elecRhoIsoAdder.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "TTree.h"
#include "TFile.h"
#include <fstream>
#include "WbWbXAnalysis/Analysis/interface/AnalysisUtils.h"
#include <algorithm>
#include <sys/stat.h>
#include "../interface/tBranchHandler.h"

#include "basicAnalyzer.h"

namespace ztop{class NTEvent;}

///// now available: removeContribution; use it to run on the systematics etc (only signal) or pdf stuff (remove nominal,for(i) add pdf[i], xsec, remove pdf[i])

//// run in batch mode otherwise it is dramatically slowed down by all the drawing stuff; the latter might also produce seg violations in the canvas libs.
//// name Z contribution "Z" or something similar for generator stuff and so on

//maybe the container filling uses a lot of cpu time, check that! checked. inlined and pow->sq

/*
 *
 * DONT'T add containers as members
 *
 */

namespace ztop{

class topAnalyzer: public basicAnalyzer{

public:
	topAnalyzer();


	virtual ~topAnalyzer();

	//goes to higher level
	void setChannel(const TString& chan);
	//void setSyst(TString syst){syst_=syst;}
	void setEnergy(TString e){if(e.Contains("7TeV")) is7TeV_=true; energy_=e;}
	void setTopMass(TString topmass){topmass_=topmass;}
	void setBTagMCEffFile(TString file){btagefffile_=file;}
	TString getChannel(){return channel_;}
	//TString getSyst(){return syst_;}
	TString Energy(){return energy_;}

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

	void addWeightBranch(const TString & name){additionalweights_.push_back(name);}


	//override with specifics in higher level
	TString getOutFileName(){
		if(outfileadd_=="") return channel_+"_"+energy_+"_"+topmass_+"_"+syst_;
		else return channel_+"_"+energy_+"_"+topmass_+"_"+syst_+"_"+outfileadd_;
	}
	/////////////////////////// interface to lower levels


	fileForker::fileforker_status writeOutput();
public:
	/////////////////////////// remain in base

	virtual void analyze(size_t i)=0;

	void setMode(TString mode){mode_=mode;} //will only set the mode, reading will be done at higher level

	void setOutFileAdd(TString o){outfileadd_=o;}

	float createNormalizationInfo(TFile *f,bool isMC,size_t anaid);



	void setFileList(TString fl){filelist_=fl;}


	void setShowStatus(bool show){showstatus_=show;}
	void setOnlySummary(bool show){onlySummary_=show;}
	void setTickOnceMode(bool test){tickoncemode_=test;}



	int start();
	//  void start(TString);

	void clear(){allplotsstackvector_.clear();}


	topAnalyzer & operator= (const topAnalyzer &);

	void setFakeDataStartNEntries(float startdiv){fakedata_startentries_=startdiv;}


	///analysis helper functions

	virtual bool checkTrigger(std::vector<bool> * ,ztop::NTEvent * , bool , size_t)=0;

	/**
	 * for child processes
	 * if you report an error, please add its return value to the description
	 * to the analyse executable
	 */
	void reportError(int , size_t );

	/**
	 * set the path to the inital configuration file (if any) to use it for further input to
	 * event loop or something else
	 *
	 * this should not be done normally and is only for special cases
	 */

	void setPathToConfigFile(const TString & path){pathtoconffile_=path;}

protected:


	void copyAll(const topAnalyzer &);
	void readFileList(); //some extras compared to basic impl


	bool showstatus_,onlySummary_,tickoncemode_,singlefile_;

	TString name_,channel_,energy_;
	bool b_ee_,b_emu_,b_mumu_,is7TeV_;

	/**
	 * default: NTEvent, can be changed e.g. to use weights of different pdfs
	 * incorporated in the same ntuple
	 */
	TString eventbranch_;

	TString mode_;

	TString filelist_;

	ztop::PUReweighter  puweighter_;
	ztop::NTJERAdjuster  jeradjuster_;
	ztop::NTJECUncertainties  jecuncertainties_;
	ztop::NTBTagSF  btagsf_;

	ztop::pdfReweighter pdfweighter_;


	size_t filecount_;
	TString outfileadd_;

	TString btagefffile_;

	//for scalefactors provided in THXX format:
	ztop::scalefactors elecsf_,muonsf_,triggersf_,elecenergysf_,muonenergysf_,trackingsf_;
	ztop::reweightfunctions topptReweighter_;

	//for parallel stuff

	int usepdfw_;



	TString topmass_;

	//for discriminators
	std::string discrInput_;
	bool usediscr_;

	std::vector<TString> additionalweights_;

	/////path to initial configuration file (in case adjustments are being made afterwards)
	TString pathtoconffile_;

	float fakedata_startentries_;



};
}

#endif
