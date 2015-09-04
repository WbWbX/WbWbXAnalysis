/*
 * ttbarXsecFitter.cc
 *
 *  Created on: Oct 9, 2014
 *      Author: kiesej
 */


#include <TtZAnalysis/Tools/interface/plotterMultiStack.h>
#include <TtZAnalysis/Tools/interface/plotterMultiCompare.h>
#include "../interface/ttbarXsecFitter.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "TtZAnalysis/Tools/interface/histoStack.h"
#include "TtZAnalysis/Tools/interface/histoStackVector.h"
#include "TtZAnalysis/Tools/interface/fileReader.h"
#include "Math/Functor.h"
#include "TtZAnalysis/Tools/interface/plotterControlPlot.h"
#include "TtZAnalysis/Tools/interface/texTabler.h"
#include "limits.h"

namespace ztop{

bool ttbarXsecFitter::debug=false;
TRandom3 * ttbarXsecFitter::random_=0;

bool ttbarXsecFitter::includeUFOF=true;

void ttbarXsecFitter::readInput(const std::string & configfilename){
	if(debug)
		std::cout << "ttbarXsecFitter::readInput" <<std::endl;
	fitsucc_=false;

	fileReader fr;
	fr.setComment("$");
	fr.setDelimiter(",");

	fr.setStartMarker("[ general ]");
	fr.setEndMarker("[ end - general ]");
	fr.readFile(configfilename);
	fr.setRequireValues(false);
	wjetsrescalefactor_=fr.getValue<float>("rescaleWjets",1.);
	fr.setRequireValues(true);
	mergesystfile_="/src/TtZAnalysis/Analysis/configs/fitTtBarXsec/"+fr.getValue<std::string>("mergeSystFile");
	if(!fileExists((getenv("CMSSW_BASE")+mergesystfile_).data()))
		throw std::runtime_error("mergeSystFile does not exists");
	std::cout << "reading syst merging from " << mergesystfile_ << std::endl;

	if(wjetsrescalefactor_!=1.)
		std::cout << "Warning! Will rescale Wjets contribution with " << wjetsrescalefactor_ << std::endl;
	fr.clear();
	fr.setStartMarker("[ datasets ]");
	fr.setEndMarker("[ end - datasets ]");
	fr.readFile(configfilename);
	std::vector<std::string> datasetmarkers=fr.getMarkerValues("dataset");
	for(size_t i=0;i<datasetmarkers.size();i++){
		std::cout << datasetmarkers.at(i) <<std::endl;
		fileReader frd;
		frd.setComment("$");
		frd.setDelimiter(",");
		//8 TeV
		frd.setStartMarker("[ " +datasetmarkers.at(i) +  " ]");
		frd.setEndMarker("[ end " +datasetmarkers.at(i) + " ]");
		frd.readFile(configfilename);

		double lumi = frd.getValue<double>("lumi");
		double lumiunc = frd.getValue<double>("lumiunc");
		double energy = frd.getValue<double>("energy");
		double topmass = frd.getValue<double>("topmass");
		if(topmassrepl_>0){
			topmass=topmassrepl_;
			TString mtrepl="_"+toTString(172.5)+"_";TString mtrplwith="_"+toTString(topmass)+"_";
			setReplaceInInfile(mtrepl,mtrplwith);
			std::cout << "Replacing top mass with " <<topmass <<std::endl;
		}
		TString name = datasetmarkers.at(i);
		name.ReplaceAll(" ","");
		name.ReplaceAll("dataset-","");
		double xsec=getTtbarXsec(topmass,energy);

		datasets_.push_back(dataset(lumi,lumiunc,xsec,name,this));
		//DEBUG
		std::cout << "ttbarXsecFitter::readInput: created dataset: "
				<< lumi << ", "<< lumiunc <<", " << xsec << ", "<<name <<std::endl;
	}
	if(datasets_.size()<1)
		throw std::logic_error("ttbarXsecFitter::readInput: no dataset description found");

	if(datasets_.size()>1){
		//read correlations
		//more than two not implemented..but could be!
		if(datasets_.size()>2)
			throw std::out_of_range("ttbarXsecFitter: at the moment only max two datasets implemented\nIn case more datasets are required, the part that handles correlations between systematics has to be adjusted. The rest is already compatible");
		for(size_t i=0;i<datasets_.size();i++){
			for(size_t j=0;j<datasets_.size();j++){
				if(i==j)continue;
				fr.setStartMarker(("[ correlations "+ datasets_.at(i).getName() +" - " + datasets_.at(j).getName() + " ]").Data());
				fr.setEndMarker("[ end - correlations ]");
				fr.setDelimiter("=");
				//	fileReader::debug=true;
				fr.readFile(configfilename);

				for(size_t i=0;i<fr.nLines();i++){
					if(fr.nEntries(i)<2)
						continue;
					std::pair<TString, double> ptmp(fr.getData<TString>(i,0),fr.getData<double>(i,1));
					priorcorrcoeff_.push_back(ptmp);
				}
				//	fileReader::debug=false;
			}
		}
	}
	for(size_t i=0;i<datasets_.size();i++){
		datasets_.at(i).readStacks(configfilename,replaceininfiles_,removesyst_,priorcorrcoeff_);
	}
	for(size_t i=0;i<datasets_.size();i++){
		for(size_t j=0;j<datasets_.size();j++){
			if(i==j)
				continue;
			datasets_.at(i).equalizeIndices(datasets_.at(j));
		}
	}
	//now idices are fixed
	for(size_t i=0;i<datasets_.size();i++){
		datasets_.at(i).createXsecIdx();

	}
	createContinuousDependencies();
	//fileReader::debug=true;
	///////////// READ PRIORS ////////////


	fr.setStartMarker("[ priors ]");
	fr.setEndMarker("[ end - priors ]");
	fr.setDelimiter("=");
	fr.readFile(configfilename);
	for(size_t i=0;i<fr.nLines();i++){
		if(fr.nEntries(i)<2)
			continue;
		TString sysname= fr.getData<TString>(i,0);
		TString priorstr=fr.getData<TString>(i,1);
		if(!nopriors_ || sysname=="TOPMASS"){
			if(priorstr=="box")
				setPrior(sysname, prior_box);
			else if(priorstr=="boxright")
				setPrior(sysname, prior_narrowboxright);
			else if(priorstr=="boxleft")
				setPrior(sysname, prior_narrowboxleft);
			else if(priorstr=="float")
				setPrior(sysname, prior_float);
			else if(priorstr=="gauss")
				setPrior(sysname, prior_gauss);
			else if(priorstr=="fixed")
				setPrior(sysname, prior_parameterfixed);
			else{
				std::string errstr=("Did not recognize prior " +priorstr + " for "+ sysname).Data();
				throw std::runtime_error(errstr);
			}
		}
	}

	if(!removesyst_){
		fr.setStartMarker("[ full extrapolation ]");
		fr.setEndMarker("[ end - full extrapolation ]");
		fr.setDelimiter(",");
		fr.readFile(configfilename);
		for(size_t i=0;i<fr.nLines();i++){
			if(fr.nEntries(i)<1)
				continue;
			if(fr.nEntries(i)<2)
				addFullExtrapolError(fr.getData<TString>(i,0));
			else if(fr.nEntries(i)==3)
				addFullExtrapolError(fr.getData<TString>(i,0),fr.getData<float>(i,1),fr.getData<float>(i,2));
			else{
				std::string errstr="ttbarXsecFitter::readInput: extrapolation uncertainty line not recognized ";
				errstr+=fr.getReJoinedLine(i);
				throw std::runtime_error(errstr);
			}
		}

	}

	std::cout << "Created " << ndependencies_ << " nuisance parameters"<<std::endl;
	if(debug){
		std::cout << "ttbarXsecFitter::readInput: done. " <<std::endl;
	}
}
void ttbarXsecFitter::createPseudoDataFromMC(histo1D::pseudodatamodes mode){
	pseudodatarun_=true;
	std::vector<size_t> excludefromsyspseudo;
	for(size_t i=0;i<datasets_.size();i++)
		excludefromsyspseudo.push_back(datasets_.at(i).xsecIdx());
	for(size_t i=0;i<datasets_.size();i++)
		datasets_.at(i).createPseudoDataFromMC(mode,excludefromsyspseudo);
}

void ttbarXsecFitter::dataset::createPseudoDataFromMC(histo1D::pseudodatamodes mode, const std::vector<size_t> & excludefromvar){
	if(firstpseudoexp_){ //first call
		if(!random_){
			random_ = new TRandom3();
			if(debug) std::cout << "invoked new random" <<std::endl;
		}
		datacontsorig_nbjets_=dataconts_nbjets_; //safe originals
		backgroundcontsorig_nbjets_=backgroundconts_nbjets_; //safe originals
		signalcontsorig_nbjets_=signalconts_nbjets_; //safe originals
		signalpsmigcontsorig_nbjets_=signalpsmigconts_nbjets_; //safe originals
		signalvisgencontsorig_nbjets_=signalvisgenconts_nbjets_; //safe originals

		//there are no syst anymore (only dummies)
		/*for(size_t i=0;i<priors_.size();i++){
			if(i!=xsecidx7_ && i != xsecidx8_)
				setPrior(parameternames_.at(i),prior_parameterfixed);
		}
		 */
		//minimize output
		std::cout << "Preparing pseudo-experiments mode for dataset " << name_  <<std::endl;
		if(!debug)simpleFitter::printlevel=-1;
		firstpseudoexp_=false;
		//debug=false;
	}


	for(size_t i=0;i<dataconts_nbjets_.size();i++){
		histo1D tmpmc=backgroundcontsorig_nbjets_.at(i) + signalcontsorig_nbjets_.at(i);
		dataconts_nbjets_.at(i)=tmpmc.createPseudoExperiment(random_,&datacontsorig_nbjets_.at(i),mode); //no variations
		backgroundconts_nbjets_.at(i)=backgroundcontsorig_nbjets_.at(i).createPseudoExperiments(random_,0,mode,excludefromvar);
		signalconts_nbjets_.at(i)=signalcontsorig_nbjets_.at(i).createPseudoExperiments(random_,0,mode,excludefromvar);
		signalpsmigconts_nbjets_.at(i)=signalpsmigcontsorig_nbjets_.at(i).createPseudoExperiments(random_,0,mode,excludefromvar);
		signalvisgenconts_nbjets_.at(i)=signalvisgencontsorig_nbjets_.at(i).createPseudoExperiments(random_,0,mode,excludefromvar);
	}
	signalshape_nbjet_.clear();
	data_nbjet_.clear();
	background_nbjet_.clear();
	if(debug)
		std::cout << "ttbarXsecFitter::dataset::createPseudoDataFromMC: new pseudoexp created" <<std::endl;

}

void ttbarXsecFitter::createContinuousDependencies(){
	if(debug)
		std::cout << "ttbarXsecFitter::createContinuousDependencies" << std::endl;
	for(size_t i=0;i<datasets_.size();i++)
		datasets_.at(i).createContinuousDependencies();

	ndependencies_=datasets_.at(0).signalshape(0).getNDependencies();
	if(debug)
		std::cout << "ttbarXsecFitter::createContinuousDependencies: created dep: "
		<< ndependencies_<<std::endl;
	fittedparas_.resize(ndependencies_,0);
	priors_.resize(ndependencies_,prior_gauss);
	parameternames_=datasets_.at(0).signalshape(0).getSystNames();

	if(debug)
		for(size_t i=0;i<parameternames_.size();i++){
			std::cout << "\t"<< parameternames_.at(i) <<std::endl;
		}
}
void ttbarXsecFitter::dataset::createContinuousDependencies(){
	if(debug)
		std::cout << "ttbarXsecFitter::dataset::createContinuousDependencies" <<std::endl;

	size_t bjetcount=0;
	//bool eighttev=true;
	signalshape_nbjet_.clear();
	data_nbjet_.clear();
	background_nbjet_.clear();
	bool useMConly=parent_->useMConly_;
	for(size_t it=0;it<signalconts_nbjets_.size();it++){

		signalshape_nbjet_.push_back(createLeptonJetAcceptance(signalconts_nbjets_,signalpsmigconts_nbjets_,signalvisgenconts_nbjets_, bjetcount));
		histo1D temp=dataconts_nbjets_.at(it);
		if(useMConly){
			temp=signalconts_nbjets_.at(it) + backgroundconts_nbjets_.at(it);
			temp.setAllErrorsZero(false);
		}

		variateHisto1D tmpvarc;
		tmpvarc.import(temp);
		data_nbjet_.push_back(tmpvarc);

		tmpvarc.import(backgroundconts_nbjets_.at(it));
		background_nbjet_.push_back(tmpvarc);

		bjetcount++;
	}
}

size_t ttbarXsecFitter::nDatasets()const{
	return datasets_.size();
}
const TString& ttbarXsecFitter::datasetName(size_t datasetidx)const{
	if(datasetidx>=datasets_.size())
		throw std::out_of_range("ttbarXsecFitter::datasetName: index out of range");
	return datasets_.at(datasetidx).getName();
}
void ttbarXsecFitter::setPrior(const TString& sysname, priors prior){
	if(debug)
		std::cout << "ttbarXsecFitter::setPrior" <<std::endl;

	if(datasets_.size()<1){
		throw std::logic_error("ttbarXsecFitter::setPrior: first read in plots");
	}
	std::vector<TString> systs=datasets_.at(0).getSystNames();
	size_t idx=std::find(systs.begin(),systs.end(),sysname) - systs.begin();
	if(idx == systs.size()){
		std::string errstr=((TString)"ttbarXsecFitter::setPrior: requested variation \""+ sysname+"\" not found").Data();
		throw std::out_of_range(errstr);
	}
	priors_.at(idx) = prior;
}

void ttbarXsecFitter::addUncertainties(histoStack * stack,size_t datasetidx,size_t nbjets)const{
	if(datasets_.size()<=datasetidx){
		throw std::out_of_range("ttbarXsecFitter::addUncertainties: datasetidx too large");
	}
	datasets_.at(datasetidx).addUncertainties(stack,nbjets,false,std::vector<std::pair<TString, double> >());
}

void ttbarXsecFitter::addFullExtrapolError(const TString& sysname, const float & restmin, const float& restmax){
	if(debug)
		std::cout << "ttbarXsecFitter::addFullError" <<std::endl;

	if(datasets_.size()<1){
		throw std::logic_error("ttbarXsecFitter::addFullExtrapolError: first read in plots");
	}
	std::vector<TString> systs=datasets_.at(0).getSystNames();
	if(!sysname.EndsWith("*")){
		size_t idx=std::find(systs.begin(),systs.end(),sysname) - systs.begin();
		if(idx == systs.size()){
			std::string errstr=(std::string)"ttbarXsecFitter::addFullError: requested variation not found: "+sysname.Data();
			throw std::runtime_error (errstr);
		}
		addfullerrors_.push_back(extrapolationError(sysname, std::vector<size_t>(1, idx),
				std::vector<float>(1, restmin),std::vector<float>(1, restmax)));

	}
	else{
		size_t oldsize=addfullerrors_.size();
		TString choppedname=sysname;
		choppedname.ReplaceAll("*","");
		std::vector<float> maxes,mins;
		std::vector<size_t> idxs;
		for(size_t i=0;i<systs.size();i++){
			if(systs.at(i).BeginsWith(choppedname)){
				idxs.push_back(i);
				maxes.push_back(restmax);
				mins.push_back(restmin);
			}
		}
		if(idxs.size()>0)
			addfullerrors_.push_back(extrapolationError(choppedname, idxs, mins,maxes));
		if(oldsize==addfullerrors_.size())
			throw std::runtime_error ("ttbarXsecFitter::addFullError: requested variations not found");
	}

}

void  ttbarXsecFitter::printControlStack(bool fittedvalues,size_t bjetcat,size_t datasetidx,
		const std::string& prependToOutput)const{
	if(debug)
		std::cout << "ttbarXsecFitter::printControlStack" << std::endl;
	if(datasetidx>=datasets_.size())
		throw std::out_of_range("ttbarXsecFitter::printControlStack: dataset index.");
	if(bjetcat >= datasets_.at(0).nBjetCat()){
		throw std::out_of_range("ttbarXsecFitter::printControlStack: bjetcat out of range or no input read, yet.");
	}

	if(fittedvalues && !fitsucc_){
		std::cout << "ttbarXsecFitter::printControlStack: fit not successful, skipping post-fit plots" <<std::endl;
		return;
	}
	plotterMultiStack plm;
	plm.readStyleFromFileInCMSSW("/src/TtZAnalysis/Analysis/configs/fitTtBarXsec/plotterMultiStack_standard.txt");
	/* if(!fittedvalues)
		plm.addStyleForAllPlots(getenv("CMSSW_BASE")
				+(std::string)"/src/TtZAnalysis/Analysis/configs/fitTtBarXsec/controlPlots_mergeleg.txt",
				"[merge for pre-fit plots]",
				"[end - merge for pre-fit plots]");
	 */
	plm.readTextBoxesInCMSSW("/src/TtZAnalysis/Analysis/configs/fitTtBarXsec/plotterMultiStack_standard.txt",
			toString(bjetcat)+"btag"+toString( datasets_.at(datasetidx).getName()));

	std::vector<histoStack> stacks=datasets_.at(datasetidx).getOriginalInputStacks(bjetcat);

	corrMatrix corrmatr,corrm1;

	if(fittedvalues || pseudodatarun_){
		for(size_t i=0;i<stacks.size();i++){
			if(fittedvalues)
				stacks.at(i)=applyParametersToStack(stacks.at(i),bjetcat,datasetidx,fittedvalues,corrmatr);
			else
				stacks.at(i)=applyParametersToStack(stacks.at(i),bjetcat,datasetidx,false,corrmatr);
		}
	}
	else{
		//add background uncertainties here
		for(size_t i=0;i<stacks.size();i++){
			datasets_.at(datasetidx).addUncertainties(&stacks.at(i),bjetcat,false,std::vector<std::pair<TString, double> > ());
		}
	}
	if(fittedvalues)
		plm.associateCorrelationMatrix(corrmatr);

	float chi2=0;
	for(size_t i=0;i<stacks.size();i++){
		plm.addStack(&stacks.at(i));
		histoStack cp=stacks.at(i);
		cp.removeAllSystematics();
		chi2+=cp.chi2();
	}


	TString add="";
	if(fittedvalues)
		add="_fitted";
	if(prependToOutput.length()>0)
		add=(prependToOutput+"_fitdistr_").data()+toTString(bjetcat) +"bjets_"+  datasets_.at(datasetidx).getName()+add;
	else
		add=("fitdistr_")+toTString(bjetcat) +"bjets_"+  datasets_.at(datasetidx).getName()+add;

	formatter fmt;

	//plm.addTextBox(0.144,0.24,"#chi^{2}="+toTString(fmt.round(chi2,0.1)));
	// not include for now tb.drawToPad(&cv);


	plm.printToPdf(add.Data());

	//TCanvas cv;
	//plm.usePad(&cv);
	//plm.draw();
	//cv.Print(add+".png"); //add pdf afterwards otherwise it will be deleted



}

void ttbarXsecFitter::printVariations(size_t bjetcat,size_t datasetidx,const std::string& prependToOutput, bool postfitconstraints)const{
	if(datasetidx>=datasets_.size())
		throw std::out_of_range("ttbarXsecFitter::printControlStack: dataset index.");
	if(bjetcat >= datasets_.at(0).nBjetCat()){
		throw std::out_of_range("ttbarXsecFitter::printControlStack: bjetcat out of range or no input read, yet.");
	}
	if(debug)
		std::cout << "ttbarXsecFitter::printVariations " << bjetcat << " b-jets, " << datasets_.at(datasetidx).getName() <<std::endl;

	std::vector<histoStack> stacks=datasets_.at(datasetidx).getOriginalInputStacks(bjetcat);
	for(size_t i=0;i<stacks.size();i++){
		datasets_.at(datasetidx).addUncertainties(&stacks.at(i),bjetcat,false,std::vector<std::pair<TString, double> > ());
	}
	TString add="";
	if(prependToOutput.length()>0 && prependToOutput.find_last_of('/') != prependToOutput.size()-1)
		add=(prependToOutput+"_var_").data()+toTString(bjetcat) +"bjets_"+  datasets_.at(datasetidx).getName()+add;
	else if(prependToOutput.find_last_of('/') == prependToOutput.size()-1)
		add=(prependToOutput+"var_").data()+toTString(bjetcat) +"bjets_"+  datasets_.at(datasetidx).getName()+add;
	else
		add=("var_")+toTString(bjetcat) +"bjets_"+  datasets_.at(datasetidx).getName()+add;

	/*
    if(! postfitconstraints)
	  for(size_t i=0;i<stacks.size();i++){
		stacks.at(i).mergeVariationsFromFileInCMSSW(mergesystfile_);
		stacks.at(i).mergeVariationsFromFileInCMSSW(mergesystfile_,"merge for plots");
	}*/
	std::vector<TString> sys=stacks.at(0).getSystNameList();
	std::vector<variateHisto1D> fittedwithsys;
	std::cout << "producing variation plots for "<< bjetcat<< " bjets, "<< datasets_.at(datasetidx).getName() ;
	if(postfitconstraints)
		std::cout << " (postfit)";
	std::cout << std::endl;

	if(postfitconstraints){
		for(size_t i=0;i<stacks.size();i++){
			displayStatusBar(i,stacks.size()+sys.size());
			corrMatrix dummy;
			variateHisto1D tmpvh;
			histoStack tmpcp=applyParametersToStack(datasets_.at(datasetidx).getOriginalInputStacks(bjetcat).at(i),
					bjetcat,datasetidx,true,dummy,&tmpvh);
			if(!i)
				sys=tmpcp.getSystNameList();
			tmpvh.setName("fitted");
			fittedwithsys.push_back(tmpvh);
		}
	}
	else{
		displayStatusBar(0,stacks.size()+sys.size());
	}
	for(size_t j=0;j<sys.size();j++){
		displayStatusBar(stacks.size()+j,stacks.size()+sys.size());
		const TString& sysname=sys.at(j);

		if(sysname != "TOPMASS")continue; //DEBUGHERE

		std::vector<histo1D> nominals;
		std::vector<histo1D> ups;
		std::vector<histo1D> downs;
		for(size_t i=0;i<stacks.size();i++){
			std::vector<histo1D> tmp;
			if(postfitconstraints){


				histo1D nomtmp=fittedwithsys.at(i).exportContainer(fittedparas_);
				nomtmp.removeAllSystematics();
				nomtmp.setAllErrorsZero(true);
				size_t paraindex=std::find(parameternames_.begin(),parameternames_.end(),sysname)-parameternames_.begin();
				if(paraindex<parameternames_.size()){

					std::vector<double> fitparacp=fittedparas_;
					fitparacp.at(paraindex)+=fitter_.getParameterErrUp()->at(paraindex);
					nomtmp.addErrorContainer(sysname+"_up",fittedwithsys.at(i).exportContainer(fitparacp));
					fitparacp.at(paraindex)-=fitter_.getParameterErrUp()->at(paraindex);
					fitparacp.at(paraindex)+=fitter_.getParameterErrDown()->at(paraindex);
					nomtmp.addErrorContainer(sysname+"_down",fittedwithsys.at(i).exportContainer(fitparacp));
					nomtmp.setName("fitted");
					nominals.push_back(nomtmp);
					fitparacp.at(paraindex)=1;
					nomtmp=fittedwithsys.at(i).exportContainer(fitparacp);
					nomtmp.setName(sysname +" up");
					nomtmp.removeAllSystematics();
					ups.push_back(nomtmp);
					fitparacp.at(paraindex)=-1;
					nomtmp=fittedwithsys.at(i).exportContainer(fitparacp);
					nomtmp.removeAllSystematics();
					nomtmp.setName(sysname +" down");
					downs.push_back(nomtmp);
				}
				else{
					std::cout << "did not find " << sysname << std::endl;
				}

			}
			else{
				if(sysname != "TOPMASS")
					tmp=stacks.at(i).getFullMCContainer().produceVariations(sysname);
				else
					tmp=stacks.at(i).getFullMCContainer().produceVariations(sysname,
							"m_{t}^{MC}=172.5 GeV",
							"m_{t}^{MC}=178.5 GeV",
							"m_{t}^{MC}=166.5 GeV");
				nominals.push_back(tmp.at(0));
				ups.push_back(tmp.at(1));
				downs.push_back(tmp.at(2));
			}
		}
		plotterMultiCompare pl;
		pl.readStyleFromFileInCMSSW("/src/TtZAnalysis/Analysis/configs/fitTtBarXsec/plotterMultiCompare_standard.txt");
		pl.readTextBoxesInCMSSW("/src/TtZAnalysis/Analysis/configs/fitTtBarXsec/plotterMultiStack_standard.txt",
				toString(bjetcat)+"btag"+toString( datasets_.at(datasetidx).getName()));


		pl.setNominalPlots(&nominals);
		pl.setComparePlots(&ups,0);
		pl.setComparePlots(&downs,1);

		TString outsysname=textFormatter::makeCompatibleFileName(sysname.Data());
		if(postfitconstraints)
			outsysname+="_postfit";
		pl.printToPdf(add.Data() + (std::string)"_"+ outsysname.Data());
		pl.saveToTFile(add+"_"+ outsysname+".root");
	}
	std::cout << std::endl;

}


int ttbarXsecFitter::fit(std::vector<float>& xsecs, std::vector<float>& errup ,std::vector<float>& errdown){

	bool onlychecks=false;

	if(debug)
		std::cout << "ttbarXsecFitter::fit" <<std::endl;
	if(datasets_.size()<1){
		throw std::logic_error("ttbarXsecFitter::fit: First read in datasets");
	}
	if(!datasets_.at(0).readyForFit())
		createContinuousDependencies();
	checkSizes();
	if(debug){
		std::cout << "fitting parameters: "<<std::endl;
		for(size_t i=0;i<parameternames_.size();i++)
			std::cout << parameternames_.at(i) << std::endl;
		std::cout << "\nprior correlations:" << std::endl;
		for(size_t i=0;i<priorcorrcoeff_.size();i++)
			std::cout << priorcorrcoeff_.at(i).first << " " << priorcorrcoeff_.at(i).second << std::endl;
	}
	//no except: sizes ok
	//	fittedparas_.at(xsecidx8_)=-8.; //other starting value closer to measured value TEST

	fitter_.setParameterNames(parameternames_);
	fitter_.setRequireFitFunction(false);
	std::vector<double> stepwidths;
	stepwidths.resize(fittedparas_.size(),0.1);

	for(size_t i=0;i<datasets_.size();i++){
		stepwidths.at(datasets_.at(i).xsecIdx())=0.5;
	}

	fitter_.setParameters(fittedparas_,stepwidths);
	//set fixed paras
	for(size_t i=0;i<priors_.size();i++){
		if(priors_.at(i) == prior_parameterfixed)
			fitter_.setParameterFixed(i,true);
		else
			fitter_.setParameterFixed(i,false);
		if(priors_.at(i) == prior_box){
			fitter_.setParameterLowerLimit(i,-1);
			fitter_.setParameterUpperLimit(i,1);
		}
		else if(priors_.at(i) == prior_narrowboxleft){
			fitter_.setParameterLowerLimit(i,-1);
			fitter_.setParameterUpperLimit(i,0);
		}
		else if(priors_.at(i) == prior_narrowboxright){
			fitter_.setParameterLowerLimit(i,0);
			fitter_.setParameterUpperLimit(i,1);
		}
	}
	//set lower limit for xsecs (=> 0)
	for(size_t i=0;i<datasets_.size();i++){
		fitter_.setParameterLowerLimit(datasets_.at(i).xsecIdx(),-datasets_.at(i).xsecOffset());
	}

	fitter_.setMinimizer(simpleFitter::mm_minuit2);
	if(fittedparas_.size() != ndependencies_)
		throw std::logic_error("ttbarXsecFitter::fit: fittedparas.size() != ndep");
	functor_ = ROOT::Math::Functor(this,&ttbarXsecFitter::toBeMinimized,ndependencies_);//ROOT::Math::Functor f(this,&ttbarXsecFitter::toBeMinimized,ndependencies_);
	fitter_.setMinFunction(&functor_);

	//pre fit to get rough estimate
	fitter_.setStrategy(0);
	fitter_.setTolerance(1);

	if(!onlychecks)
		fitter_.fit();
	else
		std::cout << "Warning fitting in sloppy mode for checks!" <<std::endl;
	if(fitter_.wasSuccess())
		fitter_.feedErrorsToSteps();

	if(!silent_)
		std::cout << "First rough fit done" <<std::endl;
	if(!onlychecks){
		fitter_.setStrategy(2);
		fitter_.setTolerance(0.1);
	}
	size_t masspos=std::find(parameternames_.begin(),parameternames_.end(),"TOPMASS")-parameternames_.begin();
	if(!nominos_){
		for(size_t i=0;i<datasets_.size();i++)
			fitter_.setAsMinosParameter(datasets_.at(i).xsecIdx());
		if((TString)getenv("USER")=="kiesej"){
			if(masspos < parameternames_.size() && priors_.at(masspos)!=prior_parameterfixed ){
				fitter_.setAsMinosParameter(masspos);
			}
		}
		else{
			masspos=parameternames_.size();
		}
	}

	if(debug)
		simpleFitter::printlevel=simpleFitter::pl_verb;
	fitter_.fit();
	if(debug)
		simpleFitter::printlevel=simpleFitter::pl_silent;

	//feed back


	if(!silent_){
		for(size_t i=0;i<datasets_.size();i++){
			size_t xsecidx=datasets_.at(i).xsecIdx();
			double xsecoff=datasets_.at(i).xsecOffset();
			std::cout << "fitted xsecs: "<< datasets_.at(i).getName()<< ": "<<fitter_.getParameters()->at(xsecidx)+xsecoff <<
					"+" << fitter_.getParameterErrUp()->at(xsecidx) *100/(fitter_.getParameters()->at(xsecidx)+xsecoff) <<
					"-" << fitter_.getParameterErrDown()->at(xsecidx) *100/(fitter_.getParameters()->at(xsecidx)+xsecoff) <<
					std::endl;
			for(size_t j=0;j<datasets_.size();j++){
				if (i==j) continue;
				std::cout << "correlation between " << datasets_.at(i).getName() << " and " <<
						datasets_.at(j).getName() << " : " <<
						fitter_.getCorrelationCoefficient(xsecidx,datasets_.at(j).xsecIdx()) <<std::endl;
			}
		}
		if(masspos < parameternames_.size()){
			std::cout << "fitted mass: "<<fitter_.getParameters()->at(masspos) <<
					"+" << fitter_.getParameterErrUp()->at(masspos) <<
					"-" << fitter_.getParameterErrDown()->at(masspos)<<
					std::endl;
			for(size_t i=0;i<datasets_.size();i++){
				std::cout << "Corr coeff to xsec " << datasets_.at(i).getName() << " " <<
						fitter_.getCorrelationCoefficient(datasets_.at(i).xsecIdx(),masspos) << std::endl;
			}
		}
	}
	else if(fitter_.wasSuccess() && masspos < parameternames_.size()){
		tempdata.push_back(fitter_.getParameters()->at(masspos));
		tempdata.push_back(fitter_.getParameterErrUp()->at(masspos));
		tempdata.push_back(fitter_.getParameterErrDown()->at(masspos));
	}
	if(fitter_.wasSuccess()){
		fitsucc_=true;
		if(parameterwriteback_){
			fittedparas_ = *fitter_.getParameters();
		}
		xsecs.clear();
		errup.clear();
		errdown.clear();
		for(size_t i=0;i<datasets_.size();i++){
			size_t xsecidx=datasets_.at(i).xsecIdx();
			double xsecoff=datasets_.at(i).xsecOffset();
			xsecs.push_back(fitter_.getParameters()->at(xsecidx)+xsecoff);
			errup.push_back(fitter_.getParameterErrUp()->at(xsecidx));
			errdown.push_back(fitter_.getParameterErrDown()->at(xsecidx));
		}
		if(!nominos_ && !fitter_.minosWasSuccess()){
			throw std::runtime_error("ttbarXsecFitter::fit(): Minos error failed!");

		}
		if(!silent_){
			for(size_t i=0;i<datasets_.size();i++){
				std::cout << "eps_b: " << getEps(false,i).getBin(1).getContent() << " postfit: "
						<< getEps(true,i).getBin(1).getContent() << " " << datasets_.at(i).getName()<<std::endl;
				std::cout << "C_b:   " << getCb(false,i).getBin(1).getContent() << " postfit: "
						<< getCb(true,i).getBin(1).getContent() << " " << datasets_.at(i).getName()<<std::endl;
				std::cout << "eps_emu:   " << getEps_emu(false,i)<< " postfit: "
						<< getEps_emu(true,i)<< " " << datasets_.at(i).getName()<<std::endl;//acceptance()
				std::cout << "acceptance(extr):   " << datasets_.at(i).acceptance_extr().getNominal()<< " postfit: "
						<< datasets_.at(i).acceptance_extr().getValue(fittedparas_)<< " " << datasets_.at(i).getName()<<std::endl;
			}
		}

		return 0;
	}
	//else
	if(!silent_)
		std::cout << "ttbarXsecFitter::fit(): Fit failed" <<std::endl;
	throw std::runtime_error("ttbarXsecFitter::fit(): Fit failed");
	return -1;
}
int ttbarXsecFitter::fit(){
	std::vector<float> dummy;
	return fit(dummy,dummy,dummy);
}

histo1D ttbarXsecFitter::getCb (bool fittedvalues,size_t datasetidx)const{
	if(datasetidx>=datasets_.size())
		throw std::out_of_range("ttbarXsecFitter::getCb: dataset index out of range");

	//if(eighttev) return container_c_b_.at(0); else return container_c_b_.at(1);
	variateHisto1D temp=datasets_.at(datasetidx).container_c_b();

	if(fittedvalues)
		return temp.exportContainer(fittedparas_);
	else
		return temp.exportContainer();

}
histo1D ttbarXsecFitter::getEps(bool fittedvalues, size_t datasetidx)const{
	if(datasetidx>=datasets_.size())
		throw std::out_of_range("ttbarXsecFitter::getEps: dataset index out of range");

	//{if(eighttev) return container_eps_b_.at(0); else return container_eps_b_.at(1);}
	variateHisto1D temp=datasets_.at(datasetidx).container_eps_b();

	if(fittedvalues)
		return temp.exportContainer(fittedparas_);
	else
		return temp.exportContainer();

}
float ttbarXsecFitter::getEps_emu(bool fittedvalues,size_t datasetidx)const{
	if(datasetidx>=datasets_.size())
		throw std::out_of_range("ttbarXsecFitter::getEps: dataset index out of range");

	//{if(eighttev) return container_eps_b_.at(0); else return container_eps_b_.at(1);}
	const extendedVariable* temp=&datasets_.at(datasetidx).eps_emu();

	if(fittedvalues)
		return temp->getValue(fittedparas_);
	else
		return temp->getNominal();

}

histo2D  ttbarXsecFitter::getCorrelations()const{
	histo2D out;
	if(fitsucc_)
		fitter_.fillCorrelationCoefficients(&out);
	return out;
}


double ttbarXsecFitter::getXsec(size_t datasetidx)const{
	if(datasetidx>=datasets_.size())
		throw std::out_of_range("ttbarXsecFitter::getXsec: dataset index out of range");
	if(!fitsucc_)
		throw std::out_of_range("ttbarXsecFitter::getXsec: first perform successful fit");
	double fitted= fitter_.getParameters()->at(datasets_.at(datasetidx).xsecIdx())+datasets_.at(datasetidx).xsecOffset();

	double acceptancecorr=datasets_.at(datasetidx).acceptance().getValue(fittedparas_)/
			datasets_.at(datasetidx).acceptance_extr().getValue(fittedparas_);
	return fitted*acceptancecorr;

}
double ttbarXsecFitter::getVisXsec(size_t datasetidx)const{
	if(datasetidx>=datasets_.size())
		throw std::out_of_range("ttbarXsecFitter::getXsec: dataset index out of range");
	if(!fitsucc_)
		throw std::out_of_range("ttbarXsecFitter::getXsec: first perform successful fit");
	double fitted= fitter_.getParameters()->at(datasets_.at(datasetidx).xsecIdx())+datasets_.at(datasetidx).xsecOffset();
	double acceptance=datasets_.at(datasetidx).acceptance().getValue(fittedparas_);
	return fitted*acceptance;
}
void ttbarXsecFitter::cutAndCountSelfCheck(size_t datasetidx)const{
	if(datasetidx>=datasets_.size())
		throw std::out_of_range("ttbarXsecFitter::cutAndCountSelfCheck: dataset index out of range");
	datasets_.at(datasetidx).cutAndCountSelfCheck(std::vector<std::pair<TString, double> >());
}
void ttbarXsecFitter::dataset::cutAndCountSelfCheck(const std::vector<std::pair<TString, double> >& priorcorrcoeff)const{
	//very specific function TROUBLE (just a marker if something makes problems)
	if(debug)
		std::cout << "ttbarXsecFitter::dataset::cutAndCountSelfCheck" << std::endl;
	//get the input for >1 jet, >0 bjets
	if(inputstacks_.size()<2)
		throw std::logic_error("ttbarXsecFitter::dataset::cutAndCountSelfCheck(): first read in stacks (bjets)");
	if(inputstacks_.at(0).size()<4)
		throw std::logic_error("ttbarXsecFitter::dataset::cutAndCountSelfCheck(): first read in stacks (add jets)");

	std::vector< std::vector<histoStack> > incp=inputstacks_;
	for(size_t i=0;i<incp.size();i++)
		for(size_t j=0;j<incp.at(i).size();j++){
			addUncertainties(&(incp.at(i).at(j)),i,false,priorcorrcoeff);
			incp.at(i).at(j).mergeVariationsFromFileInCMSSW(parent_-> mergesystfile_,"merge for cutandcount");
		}

	histo1D signh,bghist,datah,vispsh;
	//get nominal value
	float data=0,signal=0,generated=0,background=0,
			alldils=0,alldild=0,alldilbg=0,
			alltwojetss=0,alltwojetsd=0,alltwojetsbg=0;
	float visps=0;
	generated=xsecoff_*lumi_;
	float nplots=0;
	for(size_t nbjet=0;nbjet<nBjetCat();nbjet++){
		size_t minjet=0;
		if(nbjet==1) minjet=1;
		for(size_t njet=0;njet<4;njet++){
			vispsh+=incp.at(nbjet).at(njet).getSignalContainer1DUnfold().getGenContainer().getIntegralBin(true);
			visps+=incp.at(nbjet).at(njet).getSignalContainer1DUnfold().getGenContainer().integral(true);
			nplots++;
			if(nbjet<1) continue;
			if(njet<minjet) continue;
			signal+=incp.at(nbjet).at(njet).getSignalContainer().integral(true);
			background+=incp.at(nbjet).at(njet).getBackgroundContainer().integral(true);
			data+=incp.at(nbjet).at(njet).getDataContainer().integral(true);

			signh += incp.at(nbjet).at(njet).getSignalContainer().getIntegralBin(true);
			bghist+=incp.at(nbjet).at(njet).getBackgroundContainer().getIntegralBin(true);
			datah+=incp.at(nbjet).at(njet).getDataContainer().getIntegralBin(true);
		}
	}
	for(size_t nbjet=0;nbjet<nBjetCat();nbjet++){
		size_t minjet=0;
		for(size_t njet=minjet;njet<4;njet++){
			alldils+=incp.at(nbjet).at(njet).getSignalContainer().integral(true);
			alldilbg+=incp.at(nbjet).at(njet).getBackgroundContainer().integral(true);
			alldild+=incp.at(nbjet).at(njet).getDataContainer().integral(true);
		}
	}
	for(size_t nbjet=0;nbjet<nBjetCat();nbjet++){
		size_t minjet=0;
		if(!nbjet) minjet=2;
		if(nbjet==1) minjet=1;
		if(nbjet==2) minjet=0;
		for(size_t njet=minjet;njet<4;njet++){
			alltwojetss+=incp.at(nbjet).at(njet).getSignalContainer().integral(true);
			alltwojetsbg+=incp.at(nbjet).at(njet).getBackgroundContainer().integral(true);
			alltwojetsd+=incp.at(nbjet).at(njet).getDataContainer().integral(true);
		}
	}


	visps/=(float)totalvisgencontsread_;
	float xsecnom = (data-background)/(signal/generated * lumi_);
	//float xsecvisnom= (data-background)/(signal/visps);
	std::cout << "nominal cross section C&C "<<getName()<<" " << xsecnom <<std::endl;
	float xsecvis=(data-background)/(signal/visps * lumi_);
	std::cout << "vis cross section C&C "<<getName()<<" " << xsecvis <<std::endl;


	vispsh*= (1/(float)totalvisgencontsread_);
	//ignore MC stat
	bghist.removeStatFromAll(true);
	signh.removeStatFromAll(true);
	vispsh.removeStatFromAll(true);
	datah.removeStatFromAll(false);
	histo1D xsec = (datah - bghist)/(signh * (lumi_/generated ));
	//xsec.removeStatFromAll();
	//std::cout << "xsec " << getName() <<std::endl;
	//xsec.coutBinContent(1,"pb");

	//std::cout << xsec.makeTableFromBinContent(1,true,true,0.1).getTable() << std::endl;

	vispsh.setErrorZeroContaining("Lumi");
	histo1D xsecvish = (datah - bghist)/((signh /vispsh)  *lumi_);
	//	xsecvish.removeStatFromAll(false);
	std::cout << "\n\nxsec vis " << getName() <<std::endl;
	//xsecvish.coutBinContent(1,"pb");
	std::cout << xsecvish.makeTableFromBinContent(1,true,true,0.1).getTable() << "\n"<< std::endl;
	std::cout << xsecvish.makeTableFromBinContent(1,false,true,0.01).getTable() << "\n"<<std::endl;
	std::cout << xsecvish.makeTableFromBinContent(1,false,false,0.01).getTable() << "\n"<<std::endl;

	std::cout << "extrapolate yourself, please" <<std::endl;

	std::cout
	<< "\n"       << "Yields data:    "
	<< data      <<"\n       bg:      "
	<< background<<"\n       sig:     "
	<< signal
	<<"\n all dil data:  "<< alldild
	<<"\n all dil sig:  "<< alldils
	<<"\n all dil bg:    "<< alldilbg
	<<"\n two jets data:  "<< alltwojetsd
	<<"\n two jets sig:  "<< alltwojetss
	<<"\n two jets bg:    "<< alltwojetsbg
	<<"\n" <<std::endl;

	//old impl
	/*
	 * float totalup2=0;
	float totaldown2=0;
	 *
	for(size_t i=0;i<sysnames.size();i++){
		TString nameup=*sysnames.at(i)+"_up";
		TString namedown=*sysnames.at(i)+"_down";
		int idxup=incp.at(0).at(0).getDataContainer().getSystErrorIndex(nameup);
		int idxdown=incp.at(0).at(0).getDataContainer().getSystErrorIndex(namedown);

		float signalup=0, signaldown=0, backgroundup=0, backgrounddown=0;
		float vispsup=0,vispsdown=0;
		for(size_t nbjet=1;nbjet<nBjetCat();nbjet++){
			size_t minjet=0;
			if(nbjet==1) minjet=1;
			for(size_t njet=minjet;njet<4;njet++){

				signalup+=incp.at(nbjet).at(njet).getSignalContainer().integral(false,idxup);
				signaldown+=incp.at(nbjet).at(njet).getSignalContainer().integral(false,idxdown);
				backgroundup+=incp.at(nbjet).at(njet).getBackgroundContainer().integral(false,idxup);
				backgrounddown+=incp.at(nbjet).at(njet).getBackgroundContainer().integral(false,idxdown);
			}
		}
		float xsecup = (data-backgroundup)/(signalup/generated * lumi_);
		float xsecdown = (data-backgrounddown)/(signaldown/generated * lumi_);
		float reluncup=(xsecup/xsecnom -1) * 100 ;
		float reluncdown= (xsecdown/xsecnom -1) * 100 ;
		//loop
		std::cout << nameup << " \t" << reluncup<<"%\n"
				<<  namedown << " \t" << reluncdown<<"%" <<std::endl;
		bool dummy;
		float up=getMaxVar(true,reluncup,reluncdown,dummy);
		float down=getMaxVar(false,reluncup,reluncdown,dummy);
		totalup2+=up*up;
		totaldown2+=down*down;
	}
	std::cout << "Xsec (C&C): "
			<< xsecnom << " +" << sqrt(totalup2)
			<< " -" << sqrt(totaldown2)
			<< "\n"       << "Yields data:    "
			<< data      <<"\n       bg:      "
			<< background<<"\n       sig:     "
			<< signal
			<<"\n all dil data:  "<< alldild
			<<"\n all dil sig:  "<< alldils
			<<"\n all dil bg:    "<< alldilbg
			<<"\n two jets data:  "<< alltwojetsd
			<<"\n two jets sig:  "<< alltwojetss
			<<"\n two jets bg:    "<< alltwojetsbg
			<<"\n" <<std::endl;
	 */
}

double ttbarXsecFitter::getXsecOffset(size_t datasetidx)const{
	if(datasetidx>=datasets_.size())
		throw std::out_of_range("ttbarXsecFitter::getXsecOffset: dataset index out of range");
	return datasets_.at(datasetidx).xsecOffset();
}


double ttbarXsecFitter::getXsecRatio(size_t datasetidx1,size_t datasetidx2, double & errup, double& errdown)const{
	if(datasetidx1>=datasets_.size() || datasetidx2>=datasets_.size())
		throw std::out_of_range("ttbarXsecFitter::getXsecRatio: dataset index out of range");
	if(!fitsucc_){
		throw std::out_of_range("ttbarXsecFitter::getXsecRatio: first perform successful fit");
	}
	const std::vector<dataset::systematic_unc> * unc1=&datasets_.at(datasetidx1).postFitSystematicsFullSimple();
	const std::vector<dataset::systematic_unc> * unc2=&datasets_.at(datasetidx2).postFitSystematicsFullSimple();

	if(unc1->size()<1|| unc2->size()<1)
		throw std::out_of_range("ttbarXsecFitter::getXsecRatio: first evaluate individual systematics contributions");

	double xsec1=getXsec(datasetidx1) ;
	double xsec2=getXsec(datasetidx2);

	//correlation coefficients without the extrapolation uncertainty:


	double ratio=getXsec(datasetidx1) / getXsec(datasetidx2);
	size_t xsecidx1=datasets_.at(datasetidx1).xsecIdx();
	size_t xsecidx2=datasets_.at(datasetidx2).xsecIdx();
	/*
	 * assume same unc for vis and full in fitted PS
	 */
	double errup1 = fitter_.getParameterErrUp()->at(xsecidx1);
	double errdown1 = fitter_.getParameterErrDown()->at(xsecidx1);
	double errup2 = fitter_.getParameterErrUp()->at(xsecidx2);
	double errdown2 = fitter_.getParameterErrDown()->at(xsecidx2);
	double corrcoeff= fitter_.getCorrelationCoefficient(xsecidx1,xsecidx2);
	//assumes positive correlation! (choice of up/down)
	double err2up =  square(errup1 / xsec1) + square(errup2 / xsec2) - 2* (corrcoeff * errup1*errup2)/(xsec1*xsec2);
	double err2down = square(errdown1 / xsec1) + square(errdown2 / xsec2) - 2* (corrcoeff * errdown1*errdown2)/(xsec1*xsec2);
	err2up=err2up * ratio*ratio;
	err2down=err2down * ratio*ratio;

	//add extrapolation uncertainties (inter-unc-correlation 1)
	//debug check
	if(unc1->size() != unc2->size())
		throw std::logic_error("ttbarXsecFitter::getXsecRatio: uncertity breakdown not same size");

	for(size_t i=0;i<unc1->size();i++){
		//debug check
		//get correlation to xsec
		if(! unc1->at(i).name.Contains("(extr)")) continue;
		if(unc1->at(i).name != unc2->at(i).name)
			throw std::logic_error("ttbarXsecFitter::getXsecRatio: uncertianty ordering broken");


		//fully correlated
		double applyup=(xsec1 + xsec1*unc1->at(i).errup) / (xsec2 + xsec2*unc2->at(i).errup);
		double applydown=(xsec1 + xsec1*unc1->at(i).errdown) / (xsec2 + xsec2*unc2->at(i).errdown);

		if(applyup - ratio >0){
			if(applydown - ratio < 0){
				err2up+= square(applyup - ratio);
				err2down+=square(ratio-applydown);
			}
			else if(applyup>applydown)
				err2up+= square(applyup - ratio);
			else
				err2up+= square(applydown - ratio);
		}
		else{
			if(applydown - ratio > 0){
				err2up+= square(applydown - ratio);
				err2down+=square(ratio-applyup);
			}
			else if(applyup<applydown)
				err2down+= square(ratio-applyup);
			else
				err2down+= square(ratio-applydown);
		}
	}

	errup=sqrt(err2up);
	errdown=sqrt(err2down);
	return ratio;
}

double ttbarXsecFitter::getParaError(size_t idx)const{
	if(idx>=fitter_.getParameters()->size())
		throw std::out_of_range("ttbarXsecFitter::getParaError");
	return fitter_.getParameterErr(idx);
}
TString ttbarXsecFitter::getParaName(size_t idx)const{
	if(idx>=fitter_.getParameterNames()->size())
		throw std::out_of_range("ttbarXsecFitter::getParaName");
	return fitter_.getParameterNames()->at(idx);
}
void ttbarXsecFitter::getParaErrorContributionToXsec(int idxin, size_t datasetidx,double&up,double&down,bool& anticorr){
	if(idxin>=(int)fitter_.getParameters()->size())
		throw std::out_of_range("ttbarXsecFitter::getParaErrorContributionToXsec: index out of range");
	if(datasetidx>=datasets_.size())
		throw std::out_of_range("ttbarXsecFitter::getParaErrorContributionToXsec: dataset index out of range");

	size_t xsecidx=datasets_.at(datasetidx).xsecIdx() ;
	double xsec=getXsec(datasetidx);
	if(!nosystbd_){
		if(idxin>=0){

			size_t idx=idxin;
			anticorr=fitter_.getCorrelationCoefficient(xsecidx,idx)<0;

			fitter_.getParameterErrorContribution(idx,xsecidx,up,down);
			up/=xsec;
			down/=xsec;
			return;
		}
		else{ //stat
			fitter_.getStatErrorContribution(xsecidx,up,down);
			up/=xsec;
			down/=xsec;
			return;
		}
	}
	else{
		up=1;
		down=1;
		return;
	}
}

histoStack ttbarXsecFitter::applyParametersToStack(const histoStack& stack, size_t bjetcat,
		size_t datasetidx, bool fitted, corrMatrix& corrmatr, variateHisto1D* fullmc)const{
	if(debug)
		std::cout << "ttbarXsecFitter::applyParametersToStack" << std::endl;
	if(datasetidx >= datasets_.size()){
		throw std::out_of_range("ttbarXsecFitter::applyParametersToStack: dataset idx out of range");
	}
	if(bjetcat >= dataset::nBjetCat()){
		throw std::out_of_range("ttbarXsecFitter::applyParametersToStack: b-jet cat>2, must be <2");
	}
	if(fitted && !fitsucc_){
		throw std::out_of_range("ttbarXsecFitter::applyParametersToStack: first perform fit successfully");
	}

	histoStack out=stack;
	std::vector<TString> sysnames=stack.getDataContainer().getSystNameList();
	if(std::find(sysnames.begin(),sysnames.end(),("BG_0_bjets_DY")) == sysnames.end())
		datasets_.at(datasetidx).addUncertainties(&out,bjetcat,removesyst_,priorcorrcoeff_);
	//otherwise uncertaintes are already added

	//map the indicies in a later step!!

	dataset cpdts = datasets_.at(datasetidx);
	cpdts.resetTotVisGenCount();
	cpdts.readStack(out,bjetcat);
	cpdts.adaptIndices(datasets_.at(datasetidx));

	cpdts.createContinuousDependencies();

	histo1D data,signal,background;
	std::vector<double> fittedparas;
	std::vector<double> constr;
	std::vector<TString> names;//=*fitter_.getParameterNames();

	fittedparas.resize(fittedparas_.size(),0);
	names=*fitter_.getParameterNames();
	constr.resize(fittedparas_.size(),1);
	if(fitted){
		fittedparas=fittedparas_;
		constr=*fitter_.getParameterErrUp(); //anyway symm
	}

	//for(size_t i=0;i<datasets_.size();i++)
	//	constr.at(datasets_.at(i).xsecIdx())=0;




	if(debug)
		std::cout << "ttbarXsecFitter::applyParametersToStack: data: "<< datasets_.at(datasetidx).data(bjetcat).getNDependencies() << std::endl;
	data=cpdts.data(bjetcat).exportContainer( fittedparas,constr,names );

	if(debug)
		std::cout << "ttbarXsecFitter::applyParametersToStack: background" << std::endl;
	background=cpdts.background(bjetcat).exportContainer( fittedparas,constr,names );

	//shape
	variateHisto1D tmpvar=cpdts.signalshape(bjetcat);

	extendedVariable integral=datasets_.at(datasetidx).signalshape(bjetcat).getIntegral();
	tmpvar/=integral;

	if(debug)
		std::cout << "ttbarXsecFitter::applyParametersToStack: normalization" << std::endl;
	double multi = cpdts.lumi() * getXsec(datasetidx);

	//adapt for indicies consistency!

	tmpvar *= multi ; //add xsec and lumi
	tmpvar *= datasets_.at(datasetidx).acceptance() ; //in case this wasnt a unfold histo
	tmpvar *= datasets_.at(datasetidx).eps_emu() ;//to get right reco
	tmpvar *= datasets_.at(datasetidx).omega_b(bjetcat); //add bjet norm
	tmpvar *= stack.getSignalContainer().integral(true) /  datasets_.at(datasetidx).signalIntegral(bjetcat);

	if(fullmc)
		*fullmc=cpdts.background(bjetcat) + tmpvar;

	signal=tmpvar.exportContainer(fittedparas,constr,names );
	signal.setYAxisName(stack.getSignalContainer().getYAxisName());
	signal.setXAxisName(stack.getSignalContainer().getXAxisName());
	data.setYAxisName(stack.getSignalContainer().getYAxisName());
	data.setXAxisName(stack.getSignalContainer().getXAxisName());
	background.setYAxisName(stack.getSignalContainer().getYAxisName());
	background.setXAxisName(stack.getSignalContainer().getXAxisName());

	out=histoStack();

	out.push_back(data,"data",1,1,99);
	out.push_back(background,"background",kGreen-9,1,2);
	if(topontop_)
		out.push_back(signal,"signal",633,1,3);
	else
		out.push_back(signal,"signal",633,1,1);

	out.setName(stack.getName() + "_" + datasets_.at(datasetidx).getName());

	//return corr matrix
	corrmatr = fitter_.getCorrelationMatrix();


	if(debug)
		std::cout << "ttbarXsecFitter::applyParametersToStack: done" << std::endl;
	return out;
}


void ttbarXsecFitter::printAdditionalControlplots(const std::string& inputfile, const std::string & configfile,const std::string& prependToOutput)const{

	//read file
	fileReader fr;
	fr.setComment("#");
	fr.setDelimiter(":");
	//read plots
	std::vector< std::vector<std::string> > plotnames; //0: main, 1+ add
	std::vector<size_t> nbjets;
	std::vector<std::string> stylefiles;
	std::vector<TString> allsys=datasets_.at(0).getSystNames();
	bool hasmtvar=std::find(allsys.begin(),allsys.end(),"TOPMASS")!=allsys.end();
	fr.readFile(configfile);
	for(size_t line=0;line<fr.nLines();line++){
		if(fr.nEntries(line)>1){
			nbjets.push_back(fr.getData<size_t>(line,0));

			textFormatter fmt;
			fmt.setComment("#");
			fmt.setDelimiter("=");
			std::vector<std::string> subnames;
			std::vector<std::string> subplots=fmt.getFormatted(fr.getData<std::string>(line,1));
			if(subplots.size()<2)
				subnames=subplots;
			else{
				fmt.setDelimiter("+");
				subnames.push_back(subplots.at(0));
				subplots=fmt.getFormatted(subplots.at(1));
				subnames << subplots;
			}
			plotnames.push_back(subnames);
			if(fr.nEntries(line)>2)
				stylefiles.push_back(fr.getData<std::string>(line,2));
			else
				stylefiles.push_back("");

		}
	}
	histoStackVector  hsv;hsv.readFromFile(inputfile);
	for(size_t i=0;i<nbjets.size();i++){
		if(plotnames.at(i).size()<1)continue;
		plotterControlPlot pl;
		if( inputfile.find("8TeV")!=std::string::npos)
			pl.readTextBoxesInCMSSW("/src/TtZAnalysis/Analysis/configs/general/noCMS_boxes.txt","CMSSplit03Left");
		else if(inputfile.find("7TeV")!=std::string::npos)
			pl.readTextBoxesInCMSSW("/src/TtZAnalysis/Analysis/configs/general/noCMS_boxes.txt","CMSSplit03Left7TeV");

		if(hasmtvar)
			pl.setSystLabel("#splitline{MC stat+syst}{+#Deltam_{t}^{MC}}");
		histoStack stack,poststack;
		size_t bjets=nbjets.at(i);
		if(bjets>2)
			bjets=0;
		corrMatrix correlations;
		for(size_t j=0;j<plotnames.at(i).size();j++){
			std::cout << plotnames.at(i).at(j) << std::endl;
			if(plotnames.at(i).size()==1){
				const histoStack& tmp=hsv.getStack(plotnames.at(i).at(j).data());
				stack=applyParametersToStack(tmp,bjets,0,false, correlations);
				std::cout << "created single a" <<std::endl;
				poststack=applyParametersToStack(tmp,bjets,0,true, correlations);
				std::cout << "created single" <<std::endl;
			}
			else if(j==0){
				j++;
				const histoStack& stmp=hsv.getStack(plotnames.at(i).at(j).data());
				stack=applyParametersToStack(stmp,j-1+bjets,0,false, correlations);
				poststack=applyParametersToStack(stmp,j-1+bjets,0,true, correlations);
				std::cout << "created first "<< plotnames.at(i).at(j) <<std::endl;
			}
			else{
				const histoStack& stmp=hsv.getStack(plotnames.at(i).at(j).data());
				stack=stack+applyParametersToStack(stmp,j-1+bjets,0,false, correlations);
				poststack=poststack+applyParametersToStack(stmp,j-1+bjets,0,true, correlations);
				std::cout << "added  "<< plotnames.at(i).at(j) <<std::endl;
			}
		}
		//if(plotnames.at(i).size()!=1){
			stack.setName(plotnames.at(i).at(0).data());
			poststack.setName(stack.getName()+"_postfit");
		//}
		TString name=stack.getName();
		name.ReplaceAll(" ","_");
		stack.setName(name);
		TFile f((TString)prependToOutput+textFormatter::makeCompatibleFileName(("prepostfit_"+stack.getName()+".root").Data()),"RECREATE");
		TCanvas cv(stack.getName());
		pl.setStack(&stack);
		pl.printToPdf(prependToOutput+textFormatter::makeCompatibleFileName( stack.getName().Data()));
		pl.usePad(&cv);
		pl.draw();
		cv.Write();
		if(fitsucc_){
			cv.SetName(poststack.getName());
			pl.setStack(&poststack);
			pl.associateCorrelationMatrix(correlations);
			pl.printToPdf(prependToOutput+textFormatter::makeCompatibleFileName(poststack.getName().Data()));
			pl.draw();
			cv.Write();
		}
		f.Close();
	}
}


void ttbarXsecFitter::createSystematicsBreakdowns(const TString& paraname){
	for(size_t i=0;i<datasets_.size();i++)
		createSystematicsBreakdown(i,paraname);
}

void ttbarXsecFitter::createSystematicsBreakdown(size_t datasetidx, const TString& paraname){
	if(datasetidx>=datasets_.size())
		throw std::out_of_range("ttbarXsecFitter::createSystematicsBreakdown: dataset index out of range");
	//size_t xsecidx=datasets_.at(datasetidx).xsecIdx();
	std::cout << "creating systematics breakdowns for " << datasets_.at(datasetidx).getName() <<std::endl;


	formatter fmt;
	TString cmsswbase=getenv("CMSSW_BASE");
	fmt.readInNameTranslateFile((cmsswbase+"/src/TtZAnalysis/Analysis/configs/general/SystNames.txt").Data());

	//bool completetable=false; //make configureable later

	size_t paraindex=datasets_.at(datasetidx).xsecIdx();
	if(paraname.Length()>0){
		paraindex = std::find(fitter_.getParameterNames()->begin(),fitter_.getParameterNames()->end(),paraname) -fitter_.getParameterNames()->begin();
		if(paraindex==fitter_.getParameterNames()->size())
			throw std::runtime_error(((std::string)"ttbarXsecFitter::createSystematicsBreakdown: for para "+paraname.Data()+" failed. parameter not found."  ));
	}

	datasets_.at(datasetidx).postFitSystematicsFull().clear();
	datasets_.at(datasetidx).postFitSystematicsFullSimple().clear();

	float xsec=getXsec(datasetidx);
	if(paraname.Length()>0)
		xsec=fitter_.getParameter(paraindex);
	float parfullvarmulti=1;
	if(paraname=="TOPMASS"){
		xsec+=172.5;
		parfullvarmulti=6;
	}
	//read in merge stuff
	fileReader fr;
	fr.setDelimiter("=");
	fr.setComment("%");
	fr.setStartMarker("[merge in table]");
	fr.setEndMarker("[end - merge in table]");
	fr.readFile(cmsswbase.Data()+ mergesystfile_);

	std::vector<std::pair< TString , std::vector<size_t> > > mergeasso;
	//std::vector<size_t> allcrosscorr;

	//first merge dataset dependend contributions
	std::vector< std::pair <TString , std::vector<TString> > > plannedmerges;
	for(size_t i=0;i<fr.nLines();i++){
		TString merged=fr.getData<TString>(i,0);
		textFormatter fmt2;
		fmt2.setDelimiter("+");
		std::vector<TString> asso=fmt2.getFormatted<TString>(fr.getData<std::string>(i,1));
		plannedmerges.push_back(std::pair <TString , std::vector<TString> > (merged ,asso));
	}
	//create dataset dependent parts
	std::vector< std::pair <TString , std::vector<size_t> > > datasetdepmerges;
	for(size_t i=0;i<parameternames_.size();i++){
		if((paraname.Length()<1 && isXSecIdx(i)) || (paraname.Length()>0 && paraindex==i)) continue;
		if(parameternames_.at(i).EndsWith(datasets_.at(0).getName())){
			std::vector<size_t>  merges;
			merges.push_back(i);
			TString paraname=parameternames_.at(i);
			paraname.ReplaceAll("_"+datasets_.at(0).getName(),"");
			size_t mergeidx = std::find(parameternames_.begin(),parameternames_.end(),paraname) - parameternames_.begin();
			if(mergeidx<parameternames_.size()){
				merges.push_back(mergeidx);
			}
			for(size_t j=1;j<datasets_.size();j++){
				mergeidx = std::find(parameternames_.begin(),parameternames_.end(),
						paraname+"_"+datasets_.at(j).getName()) - parameternames_.begin();
				if(mergeidx<parameternames_.size()){
					merges.push_back(mergeidx);
				}
			}
			datasetdepmerges.push_back(std::pair<TString , std::vector<size_t> >(paraname ,merges) );
		}
	}

	//create index equivalents
	std::vector< std::pair <TString , std::vector<size_t> > > plannedmergesindx;
	for(size_t i=0;i<plannedmerges.size();i++){
		const TString & mergedname = plannedmerges.at(i).first;
		const std::vector<TString> & tomergenames=plannedmerges.at(i).second;
		//search in datasetdepmerges
		std::vector<size_t> indicies;
		for(size_t j=0;j<tomergenames.size();j++){
			bool foundindatasetmerges=false;
			for(size_t k=0;k<datasetdepmerges.size();k++){
				const TString & dtsmergename=datasetdepmerges.at(k).first;
				if(tomergenames.at(j) == dtsmergename){
					foundindatasetmerges=true;
					indicies<<datasetdepmerges.at(k).second; //add full vector
					//delete from the other vector
					datasetdepmerges.erase(datasetdepmerges.begin()+k);
				}
			}
			if(!foundindatasetmerges){
				size_t newidx=std::find(parameternames_.begin(),parameternames_.end(), tomergenames.at(j))-parameternames_.begin();
				if(newidx<parameternames_.size()){
					indicies<<newidx;
				}
				else{
					std::string errstr="ttbarXsecFitter::createSystematicsBreakdown: index for ";
					errstr+=tomergenames.at(j).Data();
					errstr+=" not found. E.g. check for duplicates";
					throw std::out_of_range(errstr);
				}
			}
		}
		plannedmergesindx.push_back(std::pair<TString , std::vector<size_t> > (mergedname, indicies ));
	}

	//for consistency, add remaining ones
	plannedmergesindx<<datasetdepmerges;
	std::vector<size_t> alltobemerged;
	for(size_t i=0;i<plannedmergesindx.size();i++)
		alltobemerged << plannedmergesindx.at(i).second;

	for(size_t i=0;i<parameternames_.size();i++){
		if((paraname.Length()<1 && isXSecIdx(i)) || (paraname.Length()>0 && paraindex==i))continue;
		if(std::find(alltobemerged.begin(),alltobemerged.end(),i) == alltobemerged.end()){
			plannedmergesindx.push_back(std::pair<TString,std::vector<size_t> >(parameternames_.at(i),std::vector<size_t> (1,i)  )  );
		}
	}

	if(debug){
		std::cout << "merge configuration: "<<std::endl;
		for(size_t i=0;i<plannedmergesindx.size();i++){
			std::cout <<  plannedmergesindx.at(i).first << ": " <<std::endl;
			for(size_t j=0;j<plannedmergesindx.at(i).second.size();j++){
				std::cout <<  parameternames_.at(plannedmergesindx.at(i).second.at(j)) <<" ";
			}
			std::cout << '\n'<< std::endl;
		}
	}
	std::cout << "creating simplified breakdown table for " << datasets_.at(datasetidx).getName()<<" "<< paraname<<std::endl;

	for(size_t i=0;i<plannedmergesindx.size();i++){
		if(!debug)
			displayStatusBar(i,plannedmergesindx.size());
		TString name=translatePartName(fmt, plannedmergesindx.at(i).first);

		double errup=999,errdown=999;

		//redo!
		dataset::systematic_unc tmpunc;
		//	if(std::find(addfullerrors_.begin(),addfullerrors_.end(),i)  != addfullerrors_.end()){
		//		name+= " (vis)";
		//	}
		if(!nosystbd_){
			fitter_.getParameterErrorContributions(plannedmergesindx.at(i).second ,paraindex,errup,errdown);
			errup/=xsec;
			errdown/=xsec;
			errup*=parfullvarmulti;
			errdown*=parfullvarmulti;
			if(debug)
				std::cout << name << ": " << errup << " " << errdown<< std::endl;
		}
		tmpunc.name=name;
		tmpunc.errdown=errdown;
		tmpunc.errup=errup;
		tmpunc.constr= 999; //fitter_.getParameterErr(i);
		tmpunc.pull= 999;//fitter_.getParameters()->at(i);
		datasets_.at(datasetidx).postFitSystematicsFullSimple().push_back(tmpunc);
	}

	//make fast individual contributions table

	for(size_t i=0;i<getNParameters();i++){

		double errup,errdown;
		double corr=fitter_.getCorrelationCoefficient(i, paraindex);
		double xsecerr=fitter_.getParameterErr(paraindex);
		errup=corr*xsecerr/xsec;
		errup*=parfullvarmulti;
		errdown=-errup;
		TString name=translatePartName(fmt, parameternames_.at(i));
		dataset::systematic_unc tmpunc;
		tmpunc.name=name;
		tmpunc.errdown=errdown;
		tmpunc.errup=errup;
		tmpunc.constr= fitter_.getParameterErr(i) ; //fitter_.getParameterErr(i);
		tmpunc.pull= fittedparas_.at(i);//fitter_.getParameters()->at(i);
		datasets_.at(datasetidx).postFitSystematicsFull().push_back(tmpunc);
	}

	dataset::systematic_unc tmpunc;

	//statistics
	double errupstat,errdownstat;
	//bool anticorr=false;//dummy
	fitter_.getStatErrorContribution(paraindex,errupstat,errdownstat);
	errupstat/=xsec;
	errdownstat/=xsec;
	errupstat*=parfullvarmulti;
	errdownstat*=parfullvarmulti;

	tmpunc.name="Stat";
	tmpunc.errup=errupstat;
	tmpunc.errdown=errdownstat;
	tmpunc.pull=999;
	tmpunc.constr=999;

	datasets_.at(datasetidx).postFitSystematicsFull().push_back(tmpunc);
	datasets_.at(datasetidx).postFitSystematicsFullSimple().push_back(tmpunc);


	double fullpsxsec=xsec;//getXsec(datasetidx);

	double fullrelxsecerrup=fitter_.getParameterErrUp()->at(paraindex)/fullpsxsec * parfullvarmulti;
	double fullrelxsecerrdown=fitter_.getParameterErrDown()->at(paraindex)/fullpsxsec * parfullvarmulti;

	tmpunc.name="Total vis";
	if(paraname.Length()>0)
		tmpunc.name="Total "+paraname;
	tmpunc.errup=fullrelxsecerrup;
	tmpunc.errdown=fullrelxsecerrdown;
	tmpunc.pull=999;
	tmpunc.constr=999;

	datasets_.at(datasetidx).postFitSystematicsFull().push_back(tmpunc);
	datasets_.at(datasetidx).postFitSystematicsFullSimple().push_back(tmpunc);


	std::vector<double> fulladdtoerrup2,fulladdtoerrdown2;

	if(paraname.Length()<1){

		std::cout << "\nadding extrapolation uncertainties..." <<std::endl;

		for(size_t i=0;i<addfullerrors_.size();i++){
			double errup2=0,errdown2=0;
			TString name=translatePartName(fmt,addfullerrors_.at(i).name );
			name+=" (extr)";
			float exup=100,exdown=100;
			for(size_t j=0;j<addfullerrors_.at(i).indices.size();j++){
				size_t idx=addfullerrors_.at(i).indices.at(j);
				float min=addfullerrors_.at(i).restmin.at(j);
				float max=addfullerrors_.at(i).restmax.at(j);

				exup=getExtrapolationError(datasetidx,idx,true,min,max);
				exdown=getExtrapolationError(datasetidx,idx,false,min,max);

				bool corranti;
				double tmp=getMaxVar(true,exup,exdown,corranti);
				fulladdtoerrup2.push_back(tmp*tmp);
				errup2+=tmp*tmp;
				tmp=getMaxVar(false,exup,exdown,corranti);
				fulladdtoerrdown2.push_back(tmp*tmp);
				errdown2+=tmp*tmp;
			}

			tmpunc.name=name;
			if(addfullerrors_.at(i).indices.size()>1){
				tmpunc.errdown=-sqrt(errdown2);
				tmpunc.errup=sqrt(errup2);
			}
			else{
				tmpunc.errdown=exdown;
				tmpunc.errup=exup;
			}
			tmpunc.pull=999;
			tmpunc.constr=999;

			datasets_.at(datasetidx).postFitSystematicsFull().push_back(tmpunc);
			datasets_.at(datasetidx).postFitSystematicsFullSimple().push_back(tmpunc);

		}

	}

	for(size_t i=0;i<fulladdtoerrup2.size();i++)
		fullrelxsecerrup=sqrt(fullrelxsecerrup*fullrelxsecerrup+fulladdtoerrup2.at(i));
	for(size_t i=0;i<fulladdtoerrdown2.size();i++)
		fullrelxsecerrdown=sqrt(fullrelxsecerrdown*fullrelxsecerrdown+fulladdtoerrdown2.at(i));

	tmpunc.name="Total";
	tmpunc.errup=fullrelxsecerrup;
	tmpunc.errdown=fullrelxsecerrdown;

	datasets_.at(datasetidx).postFitSystematicsFull().push_back(tmpunc);
	datasets_.at(datasetidx).postFitSystematicsFullSimple().push_back(tmpunc);


}

texTabler ttbarXsecFitter::makeSystBreakDownTable(size_t datasetidx,bool detailed,const TString& paraname){

	/*
	 * Split this function in a part where the systematic contributions are
	 * calculated and saved per dataset (for extrapol unc minos errors up/down)
	 * and then a function to print them to a table
	 */

	if(datasetidx>=datasets_.size())
		throw std::out_of_range("ttbarXsecFitter::makeSystBreakdown: dataset index out of range");


	if((datasets_.at(datasetidx).postFitSystematicsFull().size()<1) )
		createSystematicsBreakdown(datasetidx,paraname);


	size_t paraindex=datasets_.at(datasetidx).xsecIdx();
	if(paraname.Length()>0){
		paraindex = std::find(fitter_.getParameterNames()->begin(),fitter_.getParameterNames()->end(),paraname) -fitter_.getParameterNames()->begin();
		if(paraindex==fitter_.getParameterNames()->size())
			throw std::runtime_error(((std::string)"ttbarXsecFitter::createSystematicsBreakdown: for para "+paraname.Data()+" failed. parameter not found."  ));
	}


	float xsec=getXsec(datasetidx);
	if(paraname.Length()>0)
		xsec=fitter_.getParameter(paraindex);
	float tableprecision=0.1;
	if(paraname=="TOPMASS"){
		xsec*=6;
		xsec+=172.5;
		tableprecision=0.01;
	}

	bool symmetrize=false;

	formatter fmt;
	TString cmsswbase=getenv("CMSSW_BASE");
	fmt.readInNameTranslateFile((cmsswbase+"/src/TtZAnalysis/Analysis/configs/general/SystNames.txt").Data());
	texTabler table(" l | c | c | c ");
	if(!fitsucc_)
		return table;

	table << "Name" << "Pull" << "Constr / $\\sigma$" << "Contribution [\\%]";
	table << texLine();
	std::vector<dataset::systematic_unc> * unc=&datasets_.at(datasetidx).postFitSystematicsFull();

	if(!detailed){
		unc=&datasets_.at(datasetidx).postFitSystematicsFullSimple();
		symmetrize=true;
	}
	else
		tableprecision=0.001;


	for(size_t i=0;i<unc->size();i++){
		dataset::systematic_unc * sys=&unc->at(i);

		if(symmetrize && !(sys->name == "Total" || sys->name == "Total fitted")
				&& !(sys->name.Contains("(extr)")))
			sys->symmetrize();

		bool anticorr=false;
		getMaxVar(true,sys->errup,sys->errdown,anticorr);
		TString errstr;
		if(sys->errdown>900 && sys->errup > 900){
			errstr= " ";
		}
		else if(fabs(sys->errdown) == fabs(sys->errup)){
			errstr="${"+fmt.toFixedCommaTString(100*sys->errup,tableprecision)+ "}$";
		}
		else{
			if(anticorr)
				errstr="$\\mp^{"+fmt.toFixedCommaTString(100*fabs(sys->errdown),tableprecision)+"}_{"+fmt.toFixedCommaTString(100*fabs(sys->errup),tableprecision)+ "}$";
			else
				errstr="$\\pm^{"+fmt.toFixedCommaTString(100*fabs(sys->errup),tableprecision)+ "}_{"+fmt.toFixedCommaTString(100*fabs(sys->errdown),tableprecision)+"}$";;
		}
		if(sys->name == "Total" || sys->name == "Total fitted")
			table <<texLine(1);
		if(sys->pull < 900)
			table << sys->name << fmt.round( sys->pull, tableprecision)<<
			fmt.round( sys->constr, 0.1)	<< errstr;
		else
			table << sys->name << " "<< " "	<< errstr;
		if(sys->name == "Total vis"){
			table <<texLine(1);
			table <<fmt.translateName( getParaName(datasets_.at(datasetidx).xsecIdx()) ) +" vis"<<
					" " << " "
					<< fmt.toTString(fmt.round(getVisXsec(datasetidx),tableprecision/10))+" pb";
			table <<texLine(1);
		}
	}
	table <<texLine(1);
	//float fullxsec=getXsec(datasetidx);
	/*if(!visibleps_){
		table <<fmt.translateName( getParaName(datasets_.at(datasetidx).xsecIdx()) )<<
				" " << " "
				<< fmt.toTString(fmt.round(fullxsec,0.1))+" pb";
	}
	else{*/
	if((paraname.Length()>0)){
		table <<fmt.translateName( getParaName(paraindex ))<<
				" " << " "
				<< fmt.toTString(fmt.round(xsec,tableprecision))+" ";
	}
	else{
		table <<fmt.translateName( getParaName(datasets_.at(datasetidx).xsecIdx()) )<<
				" " << " "
				<< fmt.toTString(fmt.round(xsec,tableprecision*10))+" pb";
		//}
	}
	table << texLine();
	return table;



}

texTabler ttbarXsecFitter::makeCorrTable() const{

	TString format=" l ";
	histo2D corr=getCorrelations();
	for(size_t i=1;i<corr.getBinsX().size()-1;i++)
		format+=" | c ";
	texTabler tab(format);
	if(!fitsucc_)
		return tab;

	formatter Formatter;
	TString cmsswbase=getenv("CMSSW_BASE");
	Formatter.readInNameTranslateFile((cmsswbase+"/src/TtZAnalysis/Analysis/configs/general/SystNames.txt").Data());
	for(size_t j=0;j<corr.getBinsY().size()-1;j++){
		for(size_t i=0;i<corr.getBinsX().size()-1;i++){
			//names
			if(i && !j)
				tab << "";//Formatter.translateName(parameternames_.at(i-1));
			else if(j && !i)
				tab << Formatter.translateName(parameternames_.at(j-1));
			else if (!j && !i)
				tab << "";
			else if(i<=j){
				float content=corr.getBinContent(i,j);
				if(fabs(content)<0.3)
					tab << Formatter.toFixedCommaTString(corr.getBinContent(i,j),0.01);
				else
					tab <<  "\\textbf{" +Formatter.toFixedCommaTString(corr.getBinContent(i,j),0.01) +"}";
			}
			else
				tab << "";
		}
		tab<<texLine(); //row done
	}

	return tab;
}

graph ttbarXsecFitter::getResultsGraph(size_t datasetidx,const float x_coord)const{
	if(datasetidx>=datasets_.size())
		throw std::out_of_range("ttbarXsecFitter::getResultsGraph: dataset index out of range");


	if((datasets_.at(datasetidx).postFitSystematicsFull().size()<1) )
		throw std::out_of_range("ttbarXsecFitter::getResultsGraph: first create syst breakdown");

	graph g;
	if(!fitsucc_)
		return g;

	float xsec=getXsec(datasetidx);
	float fiterrup=fitter_.getParameterErrUp()->at(datasets_.at(datasetidx).xsecIdx());
	float fiterrdown=fitter_.getParameterErrDown()->at(datasets_.at(datasetidx).xsecIdx());

	g.addPoint(x_coord,xsec);
	graph gerr=g;
	gerr.setPointYContent(0,xsec+fiterrup);
	g.addErrorGraph("fit_up",gerr);
	gerr.setPointYContent(0,xsec+fiterrdown);
	g.addErrorGraph("fit_down",gerr);

	const std::vector<dataset::systematic_unc> * unc=&datasets_.at(datasetidx).postFitSystematicsFullSimple();

	for(size_t i=0;i<unc->size();i++){
		const dataset::systematic_unc * sys=&unc->at(i);
		if(! sys->name.Contains("(extr)")) continue;

		gerr.setPointYContent(0,xsec+ xsec*sys->errup);
		g.addErrorGraph(sys->name+"_up",gerr);
		gerr.setPointYContent(0,xsec- xsec*sys->errdown);
		g.addErrorGraph(sys->name+"_down",gerr);
	}
	return g;
}

// can not be const due to root limitations
// do size checks before!
double ttbarXsecFitter::toBeMinimized(const double * variations){
	static size_t ncalls=0;
	static double lastout=10000;
	ncalls++;
	if(variations[0]!=variations[0]){
		std::cout << "Exception in call "<<ncalls<<std::endl;
		throw std::runtime_error("ttbarXsecFitter::toBeMinimized: at least one nan input from minuit");
	}
	double out=0;
	for(size_t ndataset=0;ndataset<datasets_.size();ndataset++){
		dataset * set=&datasets_.at(ndataset);

		for(size_t nbjet=0;nbjet<dataset::nBjetCat();nbjet++){
			if(exclude0bjetbin_){
				if(nbjet==0) continue;
			}
			//make sure its normalized
			//the influence usually is <0.001%, but still more correct that way
			double shapeintegral=set->signalshape(nbjet).getIntegral(variations);  //includes UFOF
			double omega_b= set->omega_b(nbjet).getValue(variations);
			double acceptance= set->acceptance().getValue(variations);
			double eps_emu=set->eps_emu().getValue(variations);

			for(size_t bin=0;bin<set->signalshape(nbjet).getNBins();bin++){
				//std::cout << bin << "/"<< signalshape_nbjet_.at(nbjet).getNBins()<<" " << nbjet << " " << std::endl;
				double lumi_xsec    = set->lumi() * (variations[set->xsecIdx()] +set->xsecOffset()) ;
				double signal=lumi_xsec* acceptance * eps_emu* omega_b * set->signalshape(nbjet).getBin(bin)->getValue(variations) / shapeintegral;

				double nbackground = set->background(nbjet).getBin(bin)->getValue(variations);

				double backgroundstat=set->background(nbjet).getBinErr(bin);

				double data = set->data(nbjet).getBin(bin)->getValue(variations);

				double datastat = set->data(nbjet).getBinErr(bin);

				//this  might happen for some variations, fix it to physics values

				double predicted = signal+nbackground;
				if(predicted<0)predicted=0;

				if(lhmode_ == lhm_poissondatastat)   {
					if(data<0)data=0; //safety
					//if(data==0)
					if(predicted>0)
						out+=-2*logPoisson(data, predicted); //unstable...
					else if(data>0)
						out+=100;
				}

				else if(lhmode_ ==lhm_chi2datastat)   {
					if(datastat<=0) continue;
					out+= (data-predicted)*(data-predicted) / (datastat*datastat); //chi2 approach
				}
				else if(lhmode_ ==lhm_chi2datamcstat)	{
					if((datastat*datastat + backgroundstat*backgroundstat)<=0) continue;
					out+= (data-predicted)*(data-predicted) / (datastat*datastat + backgroundstat*backgroundstat); //chi2 approach with bg errors
				}

				if(out != out){
					std::cout << "nan produced..." <<std::endl;
					ZTOP_COUTVAR(bin);
					ZTOP_COUTVAR(nbjet);
					ZTOP_COUTVAR(omega_b);
					ZTOP_COUTVAR(data);
					ZTOP_COUTVAR(datastat);
					ZTOP_COUTVAR(nbackground);
					ZTOP_COUTVAR(backgroundstat);
					ZTOP_COUTVAR(signal);
					ZTOP_COUTVAR(predicted);
					ZTOP_COUTVAR(out);
					for(size_t i=0;i<parameternames_.size();i++)
						std::cout << parameternames_.at(i) << ":\t"<<variations[i]<<std::endl;
				}
			}
		}
	}
	if(out==std::numeric_limits<double>::infinity()){
		out=lastout*10; //this is only supposed to happen for extreme variations during fit process->check pulls
	}
	lastout=out;
	//constraints
	for(size_t sys=0;sys<ndependencies_;sys++){
		bool isxsec=false;
		for(size_t i=0;i<datasets_.size();i++)
			if(sys == datasets_.at(i).xsecIdx()){
				isxsec=true;
				break;
			}
		if(isxsec) continue;


		//put else here if lumi gets treated differently
		if(priors_[sys] == prior_gauss){
			out+= simpleFitter::nuisanceGaus(variations[sys]);}
		//else if(priors_[sys] == prior_box){
		//	out+= simpleFitter::nuisanceBox(variations[sys]);
		//}
		else if(priors_[sys] == prior_float){
			//don't do anything
		}
		//else if(priors_[sys] == prior_narrowboxleft){
		//	double var=variations[sys];
		//	if(var>=0) var+=1;
		//	out+= simpleFitter::nuisanceBox(var);
		//}
		//else if(priors_[sys] == prior_narrowboxright){
		//	double var=variations[sys];
		//	if(var<=0) var-=1;
		//	out+= simpleFitter::nuisanceBox(var);
		//}
		//the fitter will handle this case, delta should be zero in all cases
		//	else if(priors_[sys] == prior_parameterfixed){
		//		double deltafixed=variations[sys]-fitter_.getParameter(sys);
		//		out+=deltafixed*deltafixed*1e9; //make it a bit smooth
		//	}

	}


	if(out!=out){
		throw std::runtime_error("ttbarXsecFitter::toBeMinimized NAN");
	}
	if(out<0)
		out=0;
	if(out<DBL_EPSILON && out !=0){
		out=DBL_EPSILON;
		std::cout << "restricted out!" <<std::endl;
	}
	if(out!=out){
		throw std::runtime_error("ttbarXsecFitter::toBeMinimized NAN");
	}
	return out;
}
void ttbarXsecFitter::checkSizes()const{
	if(debug)
		std::cout << "ttbarXsecFitter::checkSizes" <<std::endl;
	std::vector<TString> chkv;
	for(size_t i=0;i<datasets_.size();i++){
		datasets_.at(i).checkSizes();
		//sizes ok, check for parameter consistency
		for(size_t b=0;b<dataset::nBjetCat();b++){
			if(chkv.size()>0){
				if(chkv == datasets_.at(i).signalshape(b).getSystNames())
					continue;
				else{
					throw std::runtime_error("ttbarXsecFitter::checkSizes: parameters inconsistent");
				}
			}
			else
				chkv= datasets_.at(i).signalshape(b).getSystNames();
		}
	}
	if(debug)
		std::cout << "ttbarXsecFitter::checkSizes: done" <<std::endl;
}

//the whole function is obsolete once the class works as expected!
void ttbarXsecFitter::dataset::checkSizes()const{

	if(debug)
		std::cout << "ttbarXsecFitter::dataset::checkSizes: "<<name_ <<std::endl;
	if(signalshape_nbjet_.size() < 1){
		throw std::logic_error("ttbarXsecFitter::checkSizes: no input");
	}
	if(signalshape_nbjet_.size() != background_nbjet_.size())
		throw std::logic_error("ttbarXsecFitter::checkSizes: leptonacceptance_nbjet_.size() != background_nbjet_.size()");
	if(signalshape_nbjet_.size() != data_nbjet_.size())
		throw std::logic_error("ttbarXsecFitter::checkSizes: leptonacceptance_nbjet_.size() != data_nbjet.size()");


	for(size_t i=0;i<signalshape_nbjet_.size() ;i++){
		if(signalshape_nbjet_.at(i).getBins() != background_nbjet_.at(i).getBins())
			throw std::logic_error("leptonacceptance_nbjet_.at(i).getBins() != background_nbjet_.at(i).getBins()");
		if(signalshape_nbjet_.at(i).getBins() != data_nbjet_.at(i).getBins())
			throw std::logic_error("leptonacceptance_nbjet_.at(i).getBins() != data_nbjet.at(i).getBins()");
	}
	if(signalshape_nbjet_.at(0).getNDependencies() != background_nbjet_.at(0).getNDependencies())
		throw  std::logic_error("ttbarXsecFitter::checkSizes: ndep broken");
	if(signalshape_nbjet_.at(0).getNDependencies() != data_nbjet_.at(0).getNDependencies())
		throw  std::logic_error("ttbarXsecFitter::checkSizes: ndep broken");
	if(signalshape_nbjet_.at(0).getNDependencies() != omega_nbjet_.at(0).getNDependencies())
		throw  std::logic_error("ttbarXsecFitter::checkSizes: ndep broken");

	if(debug)
		std::cout << "ttbarXsecFitter::dataset::checkSizes: "<<name_ << " done" <<std::endl;

}//ttbarXsecFitter::checkSizes()


double ttbarXsecFitter::getExtrapolationError( size_t datasetidx, size_t paraidx, bool up, const float& min,const float& max){
	/*std::vector<double> paracopy=fittedparas_;
		float nom=var->getValue(paracopy);
		if(i==0)
			std::cout << "---->> Epsilon emu (%): " << nom*100 << std::endl;
		paracopy.at(idx)=1;
		float up=var->getValue(paracopy);

		paracopy.at(idx)=-1;
		float down=var->getValue(para */
	if(paraidx >= parameternames_.size()){
		throw std::out_of_range("ttbarXsecFitter::getExtrapolationError: parameter index.");
	}
	if(datasetidx>=datasets_.size()){
		throw std::out_of_range("ttbarXsecFitter::getExtrapolationError: dataset index.");
	}




	//new (17.3.)

	//in the new implementation everything that matters is the effect on acceptance_extr.

	extendedVariable extracp=datasets_.at(datasetidx).acceptance_extr();

	double optpara=fitter_.getParameter(paraidx);
	std::vector<double> paras=fittedparas_;

	if(up && optpara>max)
		return 0;
	if(!up && optpara<min)
		return 0;

	if(up)
		paras.at(paraidx)=max;
	else
		paras.at(paraidx)=min;

	double nominal=extracp.getValue(fittedparas_);
	double varied=extracp.getValue(paras);
	//double xsec=getXsec(datasetidx);
	double xsecerr = nominal / varied -1 ; // ((extrapolfactorF/extrapolfactorFull)*variedxsec - nominalxsec ) / nominalxsec;

	return xsecerr;

}

variateHisto1D ttbarXsecFitter::dataset::createLeptonJetAcceptance(const std::vector<histo1D>& signals,
		const std::vector<histo1D>& signalpsmig,
		const std::vector<histo1D>& signalvisPSgen,
		size_t bjetcategory)
{
	// this one can use histo1D operators!
	if(debug)
		std::cout << "ttbarXsecFitter::dataset::createLeptonJetAcceptance: "<< bjetcategory <<", "<< name_<<std::endl;

	if(signals.size()<1)
		throw std::logic_error("ttbarXsecFitter::createLeptonJetAcceptance  stack vector empty.");

	size_t bjetbin=bjetcategory;
	size_t minbjetbin=0;
	size_t maxbjetbin=3;



	float gennorm = 1/(lumi_*xsecoff_);

	//unused	size_t bjet0bin=minbjetbin;
	size_t bjet1bin=minbjetbin+1;
	size_t bjet2bin=minbjetbin+2;

	histo1D signal=signals.at(bjetbin);//.getSignalContainer();
	//if(norm_nbjet_global)
	signal=signal.getIntegralBin(includeUFOF);
	histo1D signalintegral=signals.at(minbjetbin); //stack->at(minbjetbin).getSignalContainer();
	//if(norm_nbjet_global)
	signalintegral=signalintegral.getIntegralBin(includeUFOF);
	for(size_t i=minbjetbin+1;i<maxbjetbin;i++){
		histo1D tmpsig=signals.at(i); //stack->at(i).getSignalContainer();
		//if(norm_nbjet_global)
		tmpsig=tmpsig.getIntegralBin(includeUFOF);
		signalintegral += tmpsig;
	}



	signalintegral_.resize(nBjetCat(),0);
	signalintegral_.at(bjetbin)=signals.at(bjetbin).integral(true);

	histo1D psmigint;
	for(size_t i=minbjetbin;i<maxbjetbin;i++){
		histo1D tmp=signalpsmig.at(i);
		tmp=tmp.getIntegralBin(includeUFOF);
		psmigint+=tmp;
	}
	histo1D visgenint;
	for(size_t i=minbjetbin;i<maxbjetbin;i++){
		histo1D tmp=signalvisPSgen.at(i);
		tmp=tmp.getIntegralBin(includeUFOF);
		visgenint+=tmp;
	}
	visgenint *= (1 /((double)totalvisgencontsread_));
	//this also scales with lumi due to technical reasons. remove this dependence
	visgenint.setErrorZeroContaining("Lumi");

	//need signal integral per bin -> same distributions for fixed add jet multi...
	bool tmpmultiplyStatCorrelated = histoContent::multiplyStatCorrelated;
	bool tmpaddStatCorrelated = histoContent::addStatCorrelated;
	bool tmpdivideStatCorrelated = histoContent::divideStatCorrelated;
	histoContent::multiplyStatCorrelated = true;
	histoContent::addStatCorrelated = true;
	histoContent::divideStatCorrelated = true;


	histo1D one=signal.createOne();
	histo1D acceptance,leptonreco;
	if(visgenint.integral(true)>0){
		acceptance =visgenint*gennorm;
		leptonreco =signalintegral/visgenint;
	}
	else{
		std::cout << "Warning no visible phase space defined. Will merge acceptance and dilepton efficiencies -> ok for control plots" <<std::endl;
		leptonreco=signalintegral.createOne();
		acceptance=signalintegral*gennorm;
	}

	variateHisto1D tmp;
	tmp.import(acceptance);
	acceptance_extr_=*tmp.getBin(1);
	for(size_t i=0;i<parent_->addfullerrors_.size();i++){
		for(size_t j=0;j<parent_->addfullerrors_.at(i).indices.size();j++)
			acceptance.setErrorZeroContaining(acceptance.getSystErrorName( parent_->addfullerrors_.at(i).indices.at(j)));
	}
	tmp.import(acceptance);
	acceptance_=*tmp.getBin(1);
	tmp.import(leptonreco);
	eps_emu_=*tmp.getBin(1);


	histo1D onebjetsignal = signals.at(bjet1bin);
	onebjetsignal=onebjetsignal.getIntegralBin(includeUFOF);
	histo1D twobjetsignal = signals.at(bjet2bin);
	twobjetsignal=twobjetsignal.getIntegralBin(includeUFOF);

	histo1D correction_b =  ((signalintegral * twobjetsignal) * 4.)
																																																																						                																																																																																																																																																																																																																																																																																																																																																																																																																																		/ ( (onebjetsignal + (twobjetsignal * 2.)) * (onebjetsignal + (twobjetsignal * 2.)));

	correction_b.removeStatFromAll();

	container_c_b_.import(correction_b);

	histo1D eps_b = twobjetsignal / (correction_b * signalintegral);
	eps_b.sqrt();
	eps_b.removeStatFromAll();

	container_eps_b_.import(eps_b);

	histo1D omega_1=    (eps_b * 2.  - (correction_b * (eps_b * eps_b)) * 2.);
	omega_1.removeStatFromAll();
	histo1D omega_2=    (correction_b * eps_b) * eps_b ;
	omega_2.removeStatFromAll();
	histo1D omega_0=    (one - omega_1 - omega_2);
	omega_0.removeStatFromAll();

	variateHisto1D tmpvarc;
	if(bjetcategory == 0){
		tmpvarc.import(omega_0);
	}
	else if(bjetcategory == 1){
		tmpvarc.import(omega_1);
	}
	else if(bjetcategory == 2){
		tmpvarc.import(omega_2);
	}

	if(omega_nbjet_.size() <= bjetbin)
		omega_nbjet_.resize(bjetbin+1);
	omega_nbjet_.at(bjetbin) = *tmpvarc.getBin(1);


	histoContent::multiplyStatCorrelated=tmpmultiplyStatCorrelated;
	histoContent::addStatCorrelated=tmpaddStatCorrelated;
	histoContent::divideStatCorrelated=tmpdivideStatCorrelated;

	variateHisto1D out;
	histo1D normedsignal = signals.at(bjetbin);//stack->at(bjetbin).getSignalContainer(); //for shape
	normedsignal.normalize(true,true); //normalize to 1 incl syst
	out.import(normedsignal);


	if(debug)
		std::cout << "ttbarXsecFitter::dataset::createLeptonJetAcceptance: done " << std::endl;
	return out;
}


const size_t & ttbarXsecFitter::dataset::xsecIdx()const{
	if(xsecidx_==9999){
		throw std::logic_error("ttbarXsecFitter::datasets::xsecIdx: first create index");
	}
	return xsecidx_;
}
void ttbarXsecFitter::dataset::createXsecIdx(){
	//search for it
	if(signalconts_nbjets_.size()<1){
		throw std::logic_error("ttbarXsecFitter::datasets::createXsecIdx: first read-in stacks");
	}
	std::vector<TString> sysnames=signalconts_nbjets_.at(0).getSystNameList();
	size_t idx=std::find(sysnames.begin(),sysnames.end(),"Xsec_"+name_)-sysnames.begin();
	if(idx == sysnames.size())
		throw std::logic_error("ttbarXsecFitter::datasets::createXsecIdx: index not found");
	xsecidx_=idx;
}

void  ttbarXsecFitter::dataset::readStacks(const std::string configfilename,const std::pair<TString,
		TString>&replaceininfiles,bool removesyst, std::vector<std::pair<TString, double> >& priorcorr){
	if(debug)
		std::cout << "ttbarXsecFitter::dataset::readStacks" <<std::endl;

	fileReader fr;
	fr.setComment("$");
	fr.setDelimiter("|");
	//8 TeV
	fr.setStartMarker(("[ "+name_+ " ]").Data());
	fr.setEndMarker(("[ end - "+name_+ " ]").Data());
	fr.readFile(configfilename);
	std::vector<std::vector<histoStack> >  out;
	totalvisgencontsread_=0;

	std::cout << "reading and preparing " << name_ << " stacks..." <<std::endl;
	size_t bjetcount=0,jetcount=0,oldbjetcount=0;
	TString oldfilename="";
	histoStackVector csv;
	std::vector<histoStack> tmpstacks;
	for(size_t line=0;line<fr.nLines();line++){
		if(!debug)
			displayStatusBar(line,fr.nLines());
		if(fr.nEntries(line) < 1) continue;
		if(fr.getData<TString>(line,0) == "bjet++"){
			bjetcount++;
			jetcount=0;
			continue;
		}
		if(fr.nEntries(line) < 2 || ! fr.nEntries(line) % 2){
			std::cout << "ttbarXsecFitter::readInput: Line format wrong:\n" << fr.getReJoinedLine(line) << std::endl;
		}
		if(oldbjetcount!=bjetcount){
			out.push_back(tmpstacks);
			oldbjetcount=bjetcount;
			tmpstacks.clear();
		}

		inputstacks_.resize(nBjetCat(),std::vector<histoStack>());

		for(size_t entry=0;entry<fr.nEntries(line);entry++){
			TString filename=fr.getData<TString>(line,entry);
			if(replaceininfiles.first.Length()>0)
				filename.ReplaceAll(replaceininfiles.first,replaceininfiles.second);
			entry++;
			TString plotname=fr.getData<TString>(line,entry);
			bool newcsv=oldfilename!=filename;
			oldfilename=filename;
			if(newcsv)
				csv.readFromFile(filename.Data()); //new file

			//csv.listStacks();
			histoStack tmpstack;
			try{
				tmpstack=csv.getStack(plotname);
			}
			catch(std::exception &ex){
				csv.listStacks();
				ex.what();
				throw std::runtime_error("stack not found");
			}
			if(!tmpstack.is1DUnfold())
				throw std::runtime_error("ttbarXsecFitter::readStacks: Stack is has no unfolding information!");
			if(tmpstack.getContribution1DUnfold("data").isDummy())
				throw std::runtime_error("ttbarXsecFitter::readStacks: Stack has no data entry!");
			if(tmpstack.getSignalIdxs().size() <1)
				throw std::runtime_error("ttbarXsecFitter::readStacks: No signal defined!");




			inputstacks_.at(bjetcount).push_back(tmpstack); //BEFORE ADDIND UNC!

			addUncertainties(&tmpstack,bjetcount,removesyst,priorcorr);


			/*	if(newcsv) //not necessary if same csv where everything is ordered in the same manner
				for(size_t i=0;i<out.size();i++){
					tmpstack.equalizeSystematicsIdxs(out.at(i));
				}
			 */
			tmpstacks.push_back(tmpstack);
			//stack=stack.append(tmpstack);
			//	std::cout << "ttbarXsecFitter::readStacks: added "
			//			<< " stack(s) for nbjets: " << bjetcount << " njets: " << jetcount <<std::endl;
		}
		jetcount++;
	}
	if(out.size()>0)
		out.push_back(tmpstacks);//last push back
	if(out.size()<1)
		std::cout << "ttbarXsecFitter::readStacks: no plots for " << name_ << " TeV added." <<std::endl;
	else if(out.size() != 3)
		throw std::runtime_error("ttbarXsecFitter::readStacks: tried to add less or more than (allowed) 3 nbjets categories");

	for(size_t nbjet=0;nbjet<out.size();nbjet++)
		readStackVec(out.at(nbjet),nbjet);

	if(debug){
		std::cout << " ttbarXsecFitter::dataset::readStacks: done" <<std::endl;
	}

}


void ttbarXsecFitter::dataset::readStack(const histoStack& stack, size_t nbjet){
	std::vector<histoStack> vec;
	vec.push_back(stack);
	readStackVec(vec,nbjet);
}
void ttbarXsecFitter::dataset::readStackVec(const std::vector<histoStack> & in,size_t nbjet){
	if(nbjet > 2){
		throw std::out_of_range("ttbarXsecFitter::dataset::readStackVec: nbjet out of range");
	}
	if(debug)
		std::cout << "ttbarXsecFitter::dataset::readStackVec: " << nbjet <<std::endl;

	size_t maxnbjetcat=3;
	signalconts_nbjets_.resize(maxnbjetcat);
	signalvisgenconts_nbjets_.resize(maxnbjetcat);
	signalpsmigconts_nbjets_.resize(maxnbjetcat);
	backgroundconts_nbjets_.resize(maxnbjetcat);
	dataconts_nbjets_.resize(maxnbjetcat);
	histo1D sign,signvisps,signpsmig,backgr,data;
	for(size_t j=0;j<in.size();j++){
		if(in.at(j).is1DUnfold()){
			sign          =sign.append(in.at(j).getSignalContainer1DUnfold().getRecoContainer());
			signvisps     =signvisps.append(in.at(j).getSignalContainer1DUnfold().getGenContainer());
			totalvisgencontsread_++;
			signpsmig=signpsmig.append(in.at(j).getSignalContainer1DUnfold().getBackground());
		}
		else if(in.at(j).is1D()){
			sign          =sign.append(in.at(j).getSignalContainer());
			signvisps     =signvisps.append(in.at(j).getSignalContainer());
			signvisps.setAllZero();
			signpsmig     =sign;
		}
		backgr      =backgr.append(in.at(j).getBackgroundContainer());
		data          =data.append(in.at(j).getDataContainer());
	}
	signalconts_nbjets_.at(nbjet) = sign;
	signalvisgenconts_nbjets_.at(nbjet)=signvisps;
	signalpsmigconts_nbjets_.at(nbjet)=signpsmig;
	backgroundconts_nbjets_.at(nbjet) = backgr;
	dataconts_nbjets_.at(nbjet)=data;

}

/*
 * Assumes consistent ordering within same dataset - is the case if input is read normally
 */
void ttbarXsecFitter::dataset::equalizeIndices(dataset & rhs){

	histo1D::equalizeSystematics(signalconts_nbjets_,rhs.signalconts_nbjets_);
	histo1D::equalizeSystematics(signalvisgenconts_nbjets_,rhs.signalvisgenconts_nbjets_);
	histo1D::equalizeSystematics(signalpsmigconts_nbjets_,rhs.signalpsmigconts_nbjets_);
	histo1D::equalizeSystematics(dataconts_nbjets_,rhs.dataconts_nbjets_);
	histo1D::equalizeSystematics(backgroundconts_nbjets_,rhs.backgroundconts_nbjets_);


}
void ttbarXsecFitter::dataset::adaptIndices(const dataset & rhs){
	for(size_t i=0;i<signalconts_nbjets_.size();i++){
		signalconts_nbjets_.at(i).adaptSystematicsIdxs(rhs.signalconts_nbjets_.at(i));
		signalvisgenconts_nbjets_.at(i).adaptSystematicsIdxs(rhs.signalvisgenconts_nbjets_.at(i));
		signalpsmigconts_nbjets_.at(i).adaptSystematicsIdxs(rhs.signalpsmigconts_nbjets_.at(i));
		dataconts_nbjets_.at(i).adaptSystematicsIdxs(rhs.dataconts_nbjets_.at(i));
		backgroundconts_nbjets_.at(i).adaptSystematicsIdxs(rhs.backgroundconts_nbjets_.at(i));
	}
}
size_t ttbarXsecFitter::getDatasetIndex(const TString & name)const{
	for(size_t i=0;i<datasets_.size();i++)
		if(name==datasets_.at(i).getName())
			return i;
	throw std::runtime_error("ttbarXsecFitter::getDatasetIndex: dataset name not found");
}

void ttbarXsecFitter::dataset::addUncertainties(histoStack * stack,size_t nbjets,bool removesyst,
		const std::vector<std::pair<TString,double> >& priorcorrcoeff)const{ //can be more sophisticated

	if(debug)
		std::cout << "ttbarXsecFitter::dataset::addUncertainties: " <<name_ << ", "<< nbjets <<std::endl;

	std::vector<TString> dymerges;
	dymerges.push_back("DY#rightarrow#tau#tau");
	dymerges.push_back("DY#rightarrowll");
	//excludefromglobalvar.push_back("Z#rightarrowll");
	stack->mergeLegends(dymerges,"DY",432,false);

	std::vector<TString> excludefromglobal;
	excludefromglobal.push_back("t#bar{t}V");
	excludefromglobal.push_back("DY");

	if(parent_->topontop_){
		stack->setLegendOrder("t#bar{t}",90);
		stack->setLegendOrder("tW/#bar{t}W#rightarrowll",89);
		stack->setLegendOrder("DY",88);
		stack->setLegendOrder("VV",87);
		stack->setLegendOrder("QCD/Wjets",86);
		stack->setLegendOrder("t#bar{t}V",85);
		stack->setLegendOrder("t#bar{t}bg",84);
	}


	//debug
	//	stack->mergeLegends("t#bar{t}(#tau)","t#bar{t}","t#bar{t}",633,true);

	if(debug)
		std::cout << "ttbarXsecFitter::addUncertainties: merged DY" <<std::endl;

	try{
		stack->mergeVariationsFromFileInCMSSW(parent_->mergesystfile_);
	}
	catch(...){}
	if(debug)
		std::cout << "ttbarXsecFitter::addUncertainties: merged variations" <<std::endl;

	stack->addRelErrorToBackgrounds(0.3,false,"BG",excludefromglobal); //more sophisticated approaches can be chosen here - like DY float etc

	if(getName().Contains("7TeV"))
		stack->addRelErrorToContribution(0.6,"t#bar{t}V","BG");
	else
		stack->addRelErrorToContribution(0.3,"t#bar{t}V","BG");

	float dy0bjetserr=0,dy1bjetserr=0,dy2bjetserr=0;
	if(nbjets==0)
		dy0bjetserr=0.3;
	else if(nbjets==1)
		dy1bjetserr=0.3;
	else if(nbjets==2)
		dy2bjetserr=0.3;


	//hardcoded scaling of QCD/Wjets....
	try{
		int wjetsqcdidx=stack->getContributionIdx("QCD/Wjets");
		stack->multiplyNorm(wjetsqcdidx,parent_->wjetsrescalefactor_);
	}
	catch(...){
		if(parent_->wjetsrescalefactor_!=(float)1.){
			std::cout << "Warning. wjetsrescalefactor!=1 but no QCD/Wjets contribution found" <<std::endl;
		}
	}

	stack->addRelErrorToContribution(dy0bjetserr,"DY","BG_0_bjets");
	stack->addRelErrorToContribution(dy1bjetserr,"DY","BG_1_bjets");
	stack->addRelErrorToContribution(dy2bjetserr,"DY","BG_2_bjets");

	std::vector<TString> allsys=stack->getDataContainer().getSystNameList();

	float addlumiunc=0;
	addlumiunc=unclumi_/100;
	stack->addGlobalRelMCError("Lumi" ,addlumiunc);

	if(removesyst){
		if(std::find(allsys.begin(),allsys.end(),"TOPMASS")!=allsys.end())
			stack->removeAllSystematics("TOPMASS");
		else
			stack->removeAllSystematics();
		//}catch(...){}
	}




	//split uncertainties here
	if(!removesyst){
		for(size_t i=0;i<priorcorrcoeff.size();i++){
			double corrcoeff = priorcorrcoeff.at(i).second * priorcorrcoeff.at(i).second;
			stack->splitSystematic(priorcorrcoeff.at(i).first,corrcoeff,priorcorrcoeff.at(i).first,priorcorrcoeff.at(i).first+"_"+getName());
		}
	}


	///fake uncertainty
	stack->addGlobalRelMCError("Xsec_"+name_,0);
	if(debug)
		std::cout << "ttbarXsecFitter::addUncertainties: done" <<std::endl;



}

const std::vector<histoStack>& ttbarXsecFitter::dataset::getOriginalInputStacks(const size_t & nbjet)const{
	if(nbjet>=inputstacks_.size())
		throw std::out_of_range("ttbarXsecFitter::dataset::getOriginalInputStacks: nbjet too large or no stacks read");

	return inputstacks_.at(nbjet);
}


TString ttbarXsecFitter::translatePartName(const formatter& fmt,const TString& name)const{
	TString namecp=name;
	TString datasetname="";
	bool wasin=false;
	for(size_t i=0;i<datasets_.size();i++){
		if(namecp.EndsWith(datasets_.at(i).getName())){
			wasin=true;
			datasetname=datasets_.at(i).getName();
			namecp.ReplaceAll("_"+datasetname,"");
			break;
		}
	}
	if(wasin)
		return fmt.translateName(namecp) + " ("+datasetname+")";
	else
		return fmt.translateName(namecp);
}
bool ttbarXsecFitter::isXSecIdx(const size_t& idx)const{
	for(size_t i=0;i<datasets_.size();i++){
		if(idx == datasets_.at(i).xsecIdx())
			return true;
	}
	return false;
}

}
