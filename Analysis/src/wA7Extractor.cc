/*
 * wA7Extractor.cc
 *
 *  Created on: 20 Jun 2016
 *      Author: jkiesele
 */

#include "../interface/wA7Extractor.h"
#include "TtZAnalysis/Tools/interface/histoStackVector.h" //all histos
#include "TtZAnalysis/Tools/interface/textFormatter.h"
#include "TtZAnalysis/Tools/interface/plotterControlPlot.h"
#include "TtZAnalysis/Tools/interface/plotterMultiplePlots.h"


namespace ztop{

double wA7Extractor::toBeMinimized(const double * variations){
	//variations of other parameters than QCD, Wjets norm and asymmetry-parameters are
	//fixed or removed or something before
	//no explicit fixing/constraints here
	//make size check before


	double out=0;
	for(size_t i=0;i<mcdependencies_.size();i++){
		variateHisto1D * mc=&mcdependencies_.at(i);
		histo1D * data=&datahistos_.at(i);
		const size_t nbins=data->getBins().size();
		for(size_t bin=0;bin<nbins;bin++){
			double mcval=mc->getBin(bin)->getValue(variations);
			if(mcval<0)mcval=0;
			out+=-2*logPoisson(data->getBinContent(bin), mcval);
		}
	}
	return out;
}


void wA7Extractor::readConfig(const std::string& infile){/*TBI*/}


void wA7Extractor::loadPlots(const std::string& infile){
	histoStackVector * hsv=new histoStackVector();
	hsv->readFromFile(infile);

	const TString isoplots="pttrans_deta_";
	const TString nonisoplots="pttrans_noiso_deta_";



	//get lists;
	std::vector<TString> stacknames=hsv->getStackNames(false);

	std::vector<histoStack> selectedStacks;
	std::vector<histoStack> nonisoStacks;
	//dynamically produce binning
	/*
	 * actually, it doesn't matter...
	 */
	//search for plots
	histo1D::c_makelist=true;
	for(size_t i=0;i<stacknames.size();i++){
		if(stacknames.at(i).BeginsWith(isoplots)){
			selectedStacks.push_back(hsv->getStack(stacknames.at(i)));
			std::cout << stacknames.at(i) << std::endl;
		}
	}
	histo1D::c_makelist=false;
	//now search for corresponding non-iso plots
	for(size_t i=0;i<selectedStacks.size();i++){
		TString searchname=selectedStacks.at(i).getName();
		searchname.ReplaceAll(isoplots,nonisoplots);
		nonisoStacks.push_back(hsv->getStack(searchname));
	}
	hsv->clear();
	std::vector<TString>fitcontr;
	fitcontr.push_back("QCD");
	fitcontr.push_back("signal");

	orig_inputstacks_=selectedStacks;
	orig_inputstacks_noniso_=nonisoStacks;

	//replace QCD by noniso data-MC
	for(size_t i=0;i<selectedStacks.size();i++){

		histo1D nonisodata=nonisoStacks.at(i).getDataContainer();
		histo1D nonisoMCother=nonisoStacks.at(i).getContributionsBut("QCD");
		nonisodata-=nonisoMCother;
		histo1D & href=selectedStacks.at(i).getContainer(selectedStacks.at(i).getContributionIdx("QCD"));
		href=nonisodata;
		inputstacks_.push_back(selectedStacks.at(i));

		//what follows is just for first plotting and testing

		hsv->getVector().push_back(selectedStacks.at(i));

		//fitted shape
		histoStack st=selectedStacks.at(i).fitAllNorms(fitcontr,false);
		st.setName(st.getName()+"_fitted");
		hsv->getVector().push_back(st);

	}
	hsv->writeAllToTFile("onlyFit_plots.root",true,false);
	delete hsv;
}

void wA7Extractor::fitAll(){

	//use to do systematics etc.
	createVariates(inputstacks_); //can be used for pseudo experiments
	fit(-1,false); //only nominal

	std::vector<TString> sysnames=mcdependencies_.at(0).getSystNames();
	size_t a7index=0;
	for(size_t i=0;i<sysnames.size();i++){
		if(sysnames.at(i) == "WparaA7"){ a7index=i;break;}
		if(i==sysnames.size()-1)
			throw std::runtime_error("wA7Extractor::fitAll: could not find index for A7");
	}

	TRandom3 randomizer;
	histo1D pullh(histo1D::createBinning(50,-3,3),"","pull","A.U.");
	for(size_t nexp=0;nexp<10;nexp++){
		break;
		//displayStatusBar(nexp,100);
		std::vector<histoStack> pseudoinp=inputstacks_;
	/*	for(size_t i=0;i<pseudoinp.size();i++){
			histoStack& s=pseudoinp.at(i);
			histo1D& datah=s.getContainer(s.getContributionIdx("data"));
			datah=s.getFullMCContainer();
			datah.createStatFromContent();
			datah=datah.createPseudoExperiment(&randomizer);
			s.poissonSmearMC(&randomizer,false);//no syst
		}

		move randomization to createVariates on histogram level!
		*/
		createVariates(pseudoinp);
		fit(-1,false);
		float pull=0;
		float newa7=fitter_.getParameter(a7index) ;
		if(newa7>0)
			pull=newa7/fabs(fitter_.getParameterErrUp()->at(a7index));
		else
			pull=newa7/fabs(fitter_.getParameterErrDown()->at(a7index));
		std::cout<< newa7 << " " << pull << std::endl;
		pullh.fill(pull);
	}
	plotterMultiplePlots pl;
	pl.addPlot(&pullh);
	pl.printToPdf("pull");

}


void wA7Extractor::fit(int sysindex, bool up){

	if(mcdependencies_.size()<1)
		return; //make some reasonable output here
	//fix parameters here
	//all but....
	//and safe parameter indices
	//parameterasso_;

	/*
	 *
	//std::vector<histo1D>
	std::vector<histoStack> orig_inputstacks_;
	std::vector<histoStack> orig_inputstacks_noniso_;

	std::vector<variateHisto1D> mcdependencies_;
	std::vector<histo1D> datahistos_;
	 */

	//fix  systematics
	//do fit.
	//move systematic
	//do fit.
	//move syst.
	//do fit.....
	std::vector<TString> systnames=mcdependencies_.at(0).getSystNames();

	fittedparas_=std::vector<double>(systnames.size(), 0); //SET ZERO
	std::vector<double> stepwidths(fittedparas_.size(),1);

	fitter_.setParameters(fittedparas_,stepwidths);
	fitter_.setRequireFitFunction(false);

	for(size_t i=0;i<fittedparas_.size();i++){
		if(systnames.at(i).BeginsWith("Wpara"))
			fitter_.setAsMinosParameter(i);
		if(systnames.at(i).BeginsWith("Wpara") || systnames.at(i).BeginsWith("signal")|| systnames.at(i).BeginsWith("QCD"))
			continue;

		if(sysindex>=0 && (int)i == sysindex){
			if(up)
				fittedparas_.at(i)=1;
			else
				fittedparas_.at(i)=-1;
		}
		fitter_.setParameterFixed(i,true);
	}



	fitter_.setMinimizer(simpleFitter::mm_minuit2);

	//pre fit to get rough estimate
	fitter_.setStrategy(0);
	fitter_.setTolerance(1);
	functor_ = ROOT::Math::Functor(this,&wA7Extractor::toBeMinimized,fittedparas_.size());//ROOT::Math::Functor f(this,&ttbarXsecFitter::toBeMinimized,ndependencies_);
	fitter_.setMinFunction(&functor_);
	fitter_.fit();
	if(fitter_.wasSuccess())
		fitter_.feedErrorsToSteps();
	else
		throw std::runtime_error("wA7Extractor::fit: rough fit failed");
	fitter_.setStrategy(2);
	fitter_.setTolerance(0.1);

	fitter_.fit();
	if(fitter_.wasSuccess())
		fittedparas_=*fitter_.getParameters();

return;
	//DEBUG
	std::cout << "fitted values: "<<std::endl;
	for(size_t i=0;i<fittedparas_.size();i++){
		std::cout << systnames.at(i) << ": " << fittedparas_.at(i) << "+" <<  fitter_.getParameterErrUp()->at(i)<<" -"
				<< fitter_.getParameterErrDown()->at(i)<<  std::endl;
	}
	//DEBUG
	for(size_t i=0;i<mcdependencies_.size();i++){
		histo1D mc=mcdependencies_.at(i).exportContainer(fittedparas_);
		histo1D data=datahistos_.at(i);
		histoStack stack;
		stack.push_back(mc,"MC",413,1,1);
		stack.push_back(data,"data",kBlack,1,99);
		plotterControlPlot pl;
		pl.setStack(&stack);
		pl.printToPdf(("fitted_"+toTString(i)).Data());

		stack.clear();
		stack.push_back(data,"data",kBlack,1,99);
		mc=mcdependencies_.at(i).exportContainer(std::vector<double>(fittedparas_.size(),0));
		stack.push_back(mc,"MC",412,1,1);
		pl.clearPlots();
		pl.cleanMem();
		pl.setStack(&stack);
		pl.printToPdf(("prefitted_"+toTString(i)).Data());
	}

}


void wA7Extractor::createVariates(const std::vector<histoStack>& inputstacks){

	//make randomization here
	datahistos_.clear();
	mcdependencies_.clear();
	std::vector<TString> exclude;
	exclude.push_back("QCD");
	exclude.push_back("signal");
	for(size_t i=0;i<inputstacks.size();i++){
		histoStack stack=inputstacks.at(i);
		datahistos_.push_back(stack.getDataContainer());
		//add variation of QCD and signal
		for(size_t j=0;j<inputstacks.size();j++){
			float relerr=0;
			if(i==j)
				relerr=1;
			stack.addRelErrorToContribution(relerr,"QCD","QCD"+toTString(j));
			stack.addRelErrorToContribution(relerr,"signal","signal"+toTString(j));
		}
		stack.addRelErrorToBackgrounds(0.3,false,"BGvar",exclude); //excludes signal and data anyway

		variateHisto1D varmc;
		varmc.import(stack.getFullMCContainer());

		mcdependencies_.push_back(varmc);
	}

	//add signal and QCD variation




}

}
