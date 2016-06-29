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
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include <cmath>

namespace ztop{

/*
 * General comments
 *
 * Mirror template for A7=-A7 in pttrans, add to template for A7=A7 and get significanly more stat!
 *
 *
 */

histo1D createAsymm(const histo1D& datahist){

	std::vector<float> dhbinning(datahist.getBins().begin()+1,datahist.getBins().end());
	size_t dhbins=dhbinning.size();
	if(!dhbins%2)
		std::cout << "createAsymm: bin problem" <<std::endl;
	dhbins/=2;
	std::vector<float> bins(dhbinning.begin()+dhbins,dhbinning.end());

	histo1D out=datahist;
	out.setYAxisName("Asymm");
	out.setXAxisName("2 p_{T}^{T} / M_{W}");
	out.setBins(bins);
	out.setAllZero();
	for(size_t i=0;i<datahist.getBins().size();i++){
		size_t rightbin=datahist.getBins().size()-1-i;
		if(rightbin < i)break;
		size_t asymbin=out.getBins().size()-1-i;

		float npos=datahist.getBinContent(rightbin);
		float nneg=datahist.getBinContent(i);
		float diff=npos-nneg;
		float sum=npos+nneg;

		out.setBinContent(asymbin, diff/sum);


		float der_neg=-2*(npos)/(sum*sum);
		float der_pos=2*nneg/(sum*sum);

		float staterr_neg=der_neg*der_neg*datahist.getBin(i).getStat2();
		float staterr_pos=der_pos*der_pos*datahist.getBin(rightbin).getStat2();
		float err=std::sqrt(staterr_neg+staterr_pos);
		out.setBinStat(asymbin,err);
	//	std::cout << "asymm = "<< diff/sum << " +-" <<err << std::endl;

	}
	return out;

}


double wA7Extractor::toBeMinimized(const double * variations){
	//variations of other parameters than QCD, Wjets norm and asymmetry-parameters are
	//fixed or removed or something before
	//no explicit fixing/constraints here
	//make size check before


	double out=0;
	bool breakcond=false;
	for(size_t i=0;i<mcdependencies_.size();i++){
		variateHisto1D * mc=&mcdependencies_.at(i);
		histo1D * data=&datahistos_.at(i);
		const size_t nbins=data->getBins().size();
		for(size_t bin=1;bin<nbins-1;bin++){ //ignore UF/OF
			double mcval=mc->getBin(bin)->getValue(variations);
			if(mcval<0)mcval=0;
			double datac=data->getBinContent(bin);
			if(datac<0)datac=0;
			if(mcval)
				out+=-2*logPoisson(datac, mcval);
			else if (datac>0)
				out+=1e3;
			//else data==0 && mc==0 do nothing implicit

			if(out != out || isinf(out)){
				std::cout << "nan/inf produced..." <<std::endl;
				ZTOP_COUTVAR(bin);
				ZTOP_COUTVAR(datac);
				ZTOP_COUTVAR(mcval);
				ZTOP_COUTVAR(out);
				for(size_t i=0;i<fitter_.getParameterNames()->size();i++)
					std::cout << fitter_.getParameterNames()->at(i) << ":\t"<<variations[i]<<std::endl;
				breakcond=true;
				break;
			}
		}
		if(breakcond)break;
	}
	//std::cout << out << std::endl;
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
			float err=0;
			std::cout << "mean data: " << selectedStacks.at(selectedStacks.size()-1).getDataContainer().getMean(err) << std::endl;
			std::cout << "mean MC: " << selectedStacks.at(selectedStacks.size()-1).getSignalContainer().getMean(err)  << std::endl;
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

	std::vector<histo1D> data_asymm;

	//replace QCD by noniso data-MC
	for(size_t i=0;i<selectedStacks.size();i++){
		histo1D nonisodata=nonisoStacks.at(i).getDataContainer();
		histo1D nonisoMCother=nonisoStacks.at(i).getContributionsBut("QCD");
		nonisodata-=nonisoMCother;
		histo1D & href=selectedStacks.at(i).getContainer(selectedStacks.at(i).getContributionIdx("QCD"));
		href=nonisodata;
		inputstacks_.push_back(selectedStacks.at(i));



		//what follows is just for first plotting and testing
		histo1D asymm=createAsymm(selectedStacks.at(i).getDataContainer());
		asymm.setName("data");
		histo1D symmmc=createAsymm(selectedStacks.at(i).getFullMCContainer());
		symmmc.removeAllSystematics();
		symmmc.setName("MC");
		plotterMultiplePlots pl;
		pl.addPlot(&asymm);
		pl.setLastNoLegend();
		pl.addPlot(&symmmc);
		pl.setLastNoLegend();
		pl.printToPdf(textFormatter::makeCompatibleFileName(("dataasymm_"+selectedStacks.at(i).getName()).Data()));
		asymm.writeToFile(("h_dataasymm_"+selectedStacks.at(i).getName()+".ztop").Data());



		hsv->getVector().push_back(selectedStacks.at(i));

		//continue;
		//fitted shape
		std::vector<float> fittedparas;
		selectedStacks.at(i).fitAllNorms(fitcontr,false,fittedparas);
		for(size_t para=0;para<fittedparas.size();para++)
			selectedStacks.at(i).multiplyNorm(para,fittedparas.at(para));
		selectedStacks.at(i).setName(selectedStacks.at(i).getName()+"_fitted");
		hsv->getVector().push_back(selectedStacks.at(i));

	}
	normfitted_inputstacks_=selectedStacks;
	hsv->writeAllToTFile("onlyFit_plots.root",true,false);
	delete hsv;
}

void wA7Extractor::fitAll(){

	//use to do systematics etc.
	createVariates(inputstacks_,0,0); //can be used for pseudo experiments
	fit(-1,false,false,true); //only nominal
	fit(-1,false,false); //only nominal
	//get stat contribution
	std::vector<TString> sysnames=mcdependencies_.at(0).getSystNames();
	size_t a7index=0;
	for(size_t i=0;i<sysnames.size();i++){
		//std::cout << sysnames.at(i)  << std::endl;
		if(sysnames.at(i) == "WparaA7"){ a7index=i;break;}
		if(i==sysnames.size()-1)
			throw std::runtime_error("wA7Extractor::fitAll: could not find index for A7");
	}

	float fullerr=fitter_.getParameterErr(a7index);
	float A7=fitter_.getParameter(a7index);
	std::vector<double> nominalparameters=*fitter_.getParameters();

	std::cout << "A7ren (+1): " << 1+A7 << " +-" << fullerr<<std::endl;

	fitter_.getParameterNames()->at(a7index)="A7";
	fitter_.makeCorrTable().writeToPdfFile("correlations");

	TRandom3 randomizer;
	histo1D pullh(histo1D::createBinning(21,-8,8),"","pull","nExp");
	size_t npseudo=1000;
	std::cout << "pseudo experiments..." << std::endl;
	for(size_t nexp=0;nexp<npseudo;nexp++){
		//break;
		if(nexp){
			displayStatusBar(nexp,npseudo,100,true);
		}
		createVariates(normfitted_inputstacks_,0,&randomizer);
		fit(-1,false,true);
		if(!nexp){
			std::cout << "stat error: " << fitter_.getParameterErr(a7index) <<std::endl;
			std::cout <<std::endl;
		}
		float pull=0;
		float newa7=fitter_.getParameter(a7index) ;
		if(newa7>0)
			pull=newa7/fabs(fitter_.getParameterErrUp()->at(a7index));
		else
			pull=newa7/fabs(fitter_.getParameterErrDown()->at(a7index));
		//std::cout<< newa7 << " " << pull << std::endl;
		pullh.fill(pull);
	}
	std::cout << std::endl;
	plotterMultiplePlots pl;
	pullh.writeToFile("pull.ztop");
	pl.addPlot(&pullh,false);
	pl.setLastNoLegend();

	graphFitter gf;
	graph fitg;
	fitg.import(&pullh,false);
	pl.readStyleFromFileInCMSSW("/src/TtZAnalysis/Analysis/configs/wExtractA7/multiplePlots_pulls.txt");

	gf.setFitMode(graphFitter::fm_gaus);
	gf.readGraph(&fitg);
	gf.setParameter(0,pullh.getYMax(false));
	gf.setParameter(1,0); //xshift
	gf.setParameter(2,1); //width
	gf.setStrategy(1);
	gf.fit();
	formatter fmt;
	float roundpeak=fmt.round(gf.getParameter(1),0.01);
	float roundwidth=fmt.round(gf.getParameter(2),0.01);

	graph fitted=gf.exportFittedCurve(500,-6,6);
	fitted.setName("#splitline{peak: "+toTString(roundpeak) + "}{width: " +toTString(roundwidth)+"}");

	pl.addPlot(&fitted);

	pl.printToPdf("pull");

}


void wA7Extractor::fit(int sysindex, bool up, bool fixallbuta7, bool fixa7){

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
	fitter_.setParameterNames(systnames);



	for(size_t i=0;i<fittedparas_.size();i++){


		if(systnames.at(i).BeginsWith("Wpara")){
			if(fixa7){
				fitter_.setParameter(i,-1);
				fittedparas_.at(i)=-1;
				fitter_.setParameterFixed(i,true);
				continue;
			}
			else{
				fitter_.setAsMinosParameter(i);
				fitter_.setParameterFixed(i,false);
				continue;
			}
		}
		if(!fixallbuta7 && (systnames.at(i).BeginsWith("signal") || systnames.at(i).BeginsWith("QCD"))){
			fitter_.setParameterFixed(i,false);
			continue;
		}

		if(sysindex>=0 && (int)i == sysindex){
			if(up){
				fitter_.setParameter(i,1);
				fittedparas_.at(i)=1;
			}
			else{
				fitter_.setParameter(i,-1);
				fittedparas_.at(i)=-1;
			}
		}
		fitter_.setParameterFixed(i,true);
	}



	fitter_.setMinimizer(simpleFitter::mm_minuit2);

	//pre fit to get rough estimate
	fitter_.setStrategy(0);
	fitter_.setTolerance(1);
	functor_ = ROOT::Math::Functor(this,&wA7Extractor::toBeMinimized,fittedparas_.size());//ROOT::Math::Functor f(this,&ttbarXsecFitter::toBeMinimized,ndependencies_);
	fitter_.setMinFunction(&functor_);
	fitter_.setMaxCalls(1e6);
	fitter_.fit();
	if(fitter_.wasSuccess())
		fitter_.feedErrorsToSteps();
	else
		throw std::runtime_error("wA7Extractor::fit: rough fit failed");
	fitter_.setStrategy(2);
	fitter_.setTolerance(.1);

	fitter_.fit();
	if(fitter_.wasSuccess())
		fittedparas_=*fitter_.getParameters();
	else
		throw std::runtime_error("wA7Extractor::fit: fine fit failed");

	//if(!fixallbuta7){
	//DEBUG
	if(!fixallbuta7){
		std::cout << "fitted values: "<<std::endl;
		for(size_t i=0;i<fittedparas_.size();i++){
			std::cout << textFormatter::fixLength(systnames.at(i).Data(), 15) << ": " << fittedparas_.at(i) << " +" <<  fitter_.getParameterErrUp()->at(i)<<" -"
					<< fitter_.getParameterErrDown()->at(i)<<  std::endl;
		}

		//DEBUG
		TString prefix="";
		if(fixa7)
			prefix="A7fix_";
		for(size_t i=0;i<mcdependencies_.size();i++){
			histo1D mc=mcdependencies_.at(i).exportContainer(fittedparas_);
			histo1D data=datahistos_.at(i);
			histoStack stack;
			stack.push_back(mc,"MC",413,1,1);
			stack.push_back(data,"data",kBlack,1,99);
			plotterControlPlot pl;
			pl.readStyleFromFileInCMSSW("/src/TtZAnalysis/Analysis/configs/wExtractA7/prefitplots_style.txt");
			pl.setStack(&stack);
			pl.printToPdf((prefix+"fitted_"+toTString(i)).Data());

			stack.clear();
			stack.push_back(data,"data",kBlack,1,99);
			mc=mcdependencies_.at(i).exportContainer(std::vector<double>(fittedparas_.size(),0));
			stack.push_back(mc,"MC",412,1,1);
			pl.clearPlots();
			pl.cleanMem();
			pl.setStack(&stack);
			pl.printToPdf((prefix+"prefitted_"+toTString(i)).Data());
		}
	}
}


void wA7Extractor::createVariates(const std::vector<histoStack>& inputstacks, const std::vector<double>* nominalparas,
		TRandom3* rand){

	//make randomization here
	datahistos_.clear();
	mcdependencies_.clear();
	std::vector<TString> exclude;
	exclude.push_back("QCD");
	exclude.push_back("signal");
	exclude.push_back("data");

	for(size_t i=0;i<inputstacks.size();i++){
		//add variation of QCD and signal
		if(debug)
			std::cout << "adding var to: " <<inputstacks.at(i).getName() <<std::endl;
		histoStack stack=inputstacks.at(i);
		for(size_t j=0;j<inputstacks.size();j++){
			float relerr=0;
			if(i==j)
				relerr=0.8;
			stack.addRelErrorToContribution(relerr,"QCD","QCD"+toTString(j));
			stack.addRelErrorToContribution(relerr,"signal","signal"+toTString(j));
			if(relerr && debug)
				std::cout << "added sig/QCD var to: " <<inputstacks.at(i).getName() <<std::endl;
		}
		stack.addRelErrorToBackgrounds(0.3,false,"BGvar",exclude); //excludes signal and data anyway

		histoStack origstack=stack;
		if(rand)
			stack.poissonSmearMC(rand,true,true);

		variateHisto1D varmc;
		histo1D fullmc=stack.getFullMCContainer();
		varmc.import(fullmc);

		mcdependencies_.push_back(varmc);

		histo1D data;
		if(rand && nominalparas){//pseudoexp mode. Use MC for data, but scale QCD and signal first
			std::vector<double> usepars=*nominalparas;
			variateHisto1D varmc2;
			histo1D fullmc2=origstack.getFullMCContainer();
			varmc2.import(fullmc2);
			std::vector<TString> sysnames=varmc2.getSystNames();//not a performance problem
			for(size_t pidx=0;pidx<nominalparas->size();pidx++){
				//if(! sysnames.at(pidx).Contains("QCD") && !sysnames.at(pidx).Contains("signal"))
				usepars.at(pidx)=0;
			}
			//data=varmc2.exportContainer(usepars);
			data=fullmc2;
			data.createStatFromContent();
			data=data.createPseudoExperiment(rand);
		}
		else{
			data=stack.getDataContainer();
		}

		//data=fullmc;
		//data.setAllErrorsZero(false);

		datahistos_.push_back(data);
	}

	if(debug){
		std::vector<TString> sysnames=mcdependencies_.at(0).getSystNames();
		std::cout << "added variations: "<<std::endl;
		for(size_t i=0;i<sysnames.size();i++){
			std::cout << sysnames.at(i)<<std::endl;
		}
	}

	//add signal and QCD variation




}

}
