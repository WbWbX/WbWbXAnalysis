
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "TtZAnalysis/DataFormats/interface/elecRhoIsoAdder.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "TTree.h"
#include "TFile.h"
#include <fstream>
#include "TtZAnalysis/Analysis/interface/AnalysisUtils.h"
#include <algorithm>
#include <sys/stat.h>
#include <TtZAnalysis/Analysis/interface/topAnalyzer.h>
#include "../interface/tBranchHandler.h"

namespace ztop{

//go to higher level at some point!
void topAnalyzer::setChannel(const TString& chan){
	if(chan.Contains("mumu")) b_mumu_=true;
	else if(chan.Contains("emu")) b_emu_=true;
	else if(chan.Contains("ee")) b_ee_=true;
	else{
		std::cout << "channel wrongly set! exit" << std::endl;
		std::exit(EXIT_FAILURE);
	}
	channel_=chan;
}


///////////////////////////
///////////////////////////
///////////////////////////
///////////////////////////
/////////////////////////// remain in base
///////////////////////////
///////////////////////////
///////////////////////////


void topAnalyzer::reportError(int errorno, size_t anaid){
	anaid++; // doesnt matter
	errorno++; //doenst matter anymore
	fileForker::reportStatus(basicAnalyzer::ff_status_child_generror);
}


topAnalyzer::topAnalyzer():basicAnalyzer(){

	AutoLibraryLoader::enable();
	freplaced_=0;
	b_ee_=false;
	b_mumu_=false;
	b_emu_=false;
	is7TeV_=false;
	showstatus_=false;
	onlySummary_=false;
	filecount_=0;
	testmode_=false;
	tickoncemode_=false;
	//showstatusbar_=false;
	lumi_=0;
	mode_="";
	singlefile_=false;
	topmass_="172.5";
	usepdfw_=-1;
	usediscr_=false;
	eventbranch_="NTEvent";

	///set defaults for SF
	pdfweighter_.switchOff(true); //switch off as default
	fakedata_startentries_=0.9;


}
/**
 * takes care of not already deleted containers etc
 */
topAnalyzer::~topAnalyzer(){
	//delete any remaining containers on heap
	ztop::histo1D::c_deletelist();
	ztop::histo1DUnfold::c_deletelist();
	ztop::histo2D::c_deletelist();

}





/**
 * handles all the parallalization and global init things
 */
int topAnalyzer::start(){

	using namespace std;
	using namespace ztop;


	///////////////////////////////////////////////////////
	////////////////// GLOBAL FILE READ ETC..//////////////
	///////////////////////////////////////////////////////

	clear();

	//add changes due to top mass value


	readFileList();


	//TString name=channel_+"_"+energy_+"_"+syst_;

	if(channel_=="" || energy_=="" || syst_ == ""){
		std::cout << "MainAnalyzer::start Analyzer not properly named - check!" << std::endl;
		return -1;
	}


	setOutputFileName((getOutPath()+".ztop").Data());

	allplotsstackvector_.setName(getOutFileName());
	allplotsstackvector_.setSyst(getSyst());

	//further SF configuration

	if(! getPdfReweighter()->switchedOff()){
		usepdfw_ = getPdfReweighter()->getConfIndex();
	}

	int interval=-1;
	if(showstatus_)
		interval=5;

	if(runParallels(interval)==fileForker::ff_status_parent_success)
		return 1;
	else return -99;
}


void topAnalyzer::readFileList(){
	basicAnalyzer::readFileList(filelist_.Data());
}

float topAnalyzer::createNormalizationInfo(TFile *f, bool isMC,size_t anaid){
	float norm=1;
	using namespace ztop;
	using namespace std;

	vector<float> onebin;
	onebin << 0.5 << 1.5;
	bool tmp=histo1D::c_makelist;
	histo1D::c_makelist=true;
	histo1D * generated=  new histo1D(onebin, "generated events", "gen", "N_{gen}");
	histo1D * generated2=  new histo1D(onebin, "generated filtered events", "gen", "N_{gen}");
	histo1D::c_makelist=tmp; //switch off automatic listing

	float genentries=0;
	if(isMC){

		TTree * tnorm = (TTree*) f->Get("preCutPUInfo/preCutPUInfo");
		genentries=tnorm->GetEntries();

		norm = lumi_   / genentries;
		for(size_t i=1;i<=generated->getNBins();i++){
			generated->setBinContent(i, (float)genentries);
			generated->setBinStat(i,sqrt(genentries));
		}
		double fgen=0;
		TTree * tfgen = (TTree*) f->Get("postCutPUInfo/PUTreePostCut");
		if(tfgen){
			fgen=tfgen->GetEntries();
			for(size_t i=1;i<=generated2->getNBins();i++){
				generated2->setBinContent(i, (float)fgen);
				generated2->setBinStat(i, sqrt(fgen));
			}
		}
		else{
			for(size_t i=1;i<=generated->getNBins();i++){
				generated2->setBinContent(i, (float)genentries);
				generated2->setBinStat(i, sqrt(genentries));
			}
		}
	}
	else{//not mc
		for(size_t i=1;i<=generated->getNBins();i++){
			generated->setBinContent(i, 0);
			generated->setBinStat(i, 0);
		}
		for(size_t i=1;i<=generated2->getNBins();i++){
			generated2->setBinContent(i, 0);
			generated2->setBinStat(i, 0);
		}
		norm=1;
	}
	//check for top mass scaling
	//if(infiles_.at(anaid).Contains("ttbar") && ! infiles_.at(anaid).Contains("_ttbar")){ //selects all ttbar samples but not ttbarg/w/z
	if(legentries_.at(anaid) != datalegend_){ //don't automatically rescale pseudodata
		float sampletopmass = atof(topmass_.Data());
		//subtract TeV in string and convert to float 
		TString tmp(energy_);
		tmp.ReplaceAll("TeV","");
		if(!tmp.IsFloat()) throw std::runtime_error("MainAnalyzer::Can't convert energy string to float!");  
		float energy = atof(tmp.Data());

		if(infiles_.at(anaid).Contains("ttbar") &&  !infiles_.at(anaid).Contains("_ttbar")){
			float xsec=getTtbarXsec(sampletopmass,energy);
			std::cout << "GetNorm: File " << infiles_.at(anaid) << "\tis a ttbar sample , top mass is "
					<< sampletopmass << " xsec: " << xsec <<std::endl;
			norms_.at(anaid)=xsec;
		}
		//tW
		if(infiles_.at(anaid).Contains("_tW.root") || infiles_.at(anaid).Contains("_tbarW.root")
				|| infiles_.at(anaid).Contains("_tbarWtoLL")|| infiles_.at(anaid).Contains("_tWtoLL")){
			float xsec=getTWXsec(sampletopmass,energy);
			std::cout << "GetNorm: File " << infiles_.at(anaid) << "\tis a tW sample , top mass is "
					<< sampletopmass << " xsec: " << xsec <<std::endl;
			norms_.at(anaid)=xsec;
		}
		//}
	}

	norm*=norms_.at(anaid);
	return norm;
}





fileForker::fileforker_status topAnalyzer::writeOutput(){
	writeHistos();
	if(btagsf_.makesEff()){
		getBTagSF()->writeToFile((std::string)btagefffile_.Data()); //recreates the file
	}///makes eff

	return ff_status_child_success;
}


}
