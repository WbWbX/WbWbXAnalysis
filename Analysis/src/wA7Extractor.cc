/*
 * wA7Extractor.cc
 *
 *  Created on: 20 Jun 2016
 *      Author: jkiesele
 */

#include "../interface/wA7Extractor.h"
#include "WbWbXAnalysis/Tools/interface/histoStackVector.h" //all histos
#include "WbWbXAnalysis/Tools/interface/textFormatter.h"
#include "WbWbXAnalysis/Tools/interface/plotterControlPlot.h"
#include "WbWbXAnalysis/Tools/interface/plotterCompare.h"
#include "WbWbXAnalysis/Tools/interface/plotterMultiplePlots.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"


#include "WbWbXAnalysis/Tools/interface/semiconst.h"

#include <cmath>

#include <csignal>


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

	bool usechi2=true;

	double out=0;
	bool breakcond=false;
	for(size_t i=0;i<mcdependencies_.size();i++){
		variateHisto1D * mc=&mcdependencies_.at(i);
		histo1D * data=&datahistos_.at(i);
		const size_t nbins=data->getBins().size();
		for(size_t bin=1;bin<nbins-1;bin++){ //ignore UF/OF
			double mcval=mc->getBin(bin)->getValue(variations);
			double datac=data->getBinContent(bin);
			if(mcval<0)mcval=0;
			if(datac<0)datac=0;
			if(usechi2){
				double delta = mcval-datac;
				double mcerr=mc->getBinErr(bin) * mcval/mc->getBin(bin)->getNominal();
				double err2 = mcval /*pearson*/ + mcerr*mcerr;
				if(err2==0)err2=1e-5;
				out+= delta*delta/err2;
			}
			else{
				if(mcval)
					out+=-2*logPoisson(datac, mcval);
				else if (datac>0)
					out+=1e3;
			}
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

	if(debug)
		std::cout << "wA7Extractor::loadPlots" <<std::endl;

	/*
	 *
	 * Implement symmetric stat enhancement here.
	 *
	 */


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


	//make histo with means
	//- get bins
	std::vector<float> bins;
	for(size_t i=0;i<stacknames.size();i++){
		if(stacknames.at(i).BeginsWith(isoplots)){
			textFormatter tf;
			tf.setDelimiter("_");
			std::vector<std::string> stmp=tf.getFormatted(stacknames.at(i).Data());
			std::string range=stmp.at(2);
			tf.setDelimiter("-");
			stmp=tf.getFormatted(range);
			float lowedge=atof(stmp.at(0).data());
			float highedge=atof(stmp.at(1).data());

			if(bins.size()<1){
				bins.push_back(lowedge);
			}
			bins.push_back(highedge);
		}
	}
	detaranges_=bins;

	histo1D datadep=histo1D(bins,"delta eta data dep","#Delta#eta(l,j)","Mean(2p_{T}^{#perp}/M_{W})");
	histo1D mcdep=histo1D(bins,"delta eta mc dep","#Delta#eta(l,j)","Mean(2p_{T}^{#perp}/M_{W})");

	std::vector<TString> exclude;
	exclude.push_back("QCD");
	exclude.push_back("signal");
	exclude.push_back("data");
	size_t bin=1;
	for(size_t i=0;i<stacknames.size();i++){
		if(stacknames.at(i).BeginsWith(isoplots)){
			std::cout << stacknames.at(i) << std::endl;
			histoStack stack=hsv->getStack(stacknames.at(i));
			float err=0;
			float mean=stack.getDataContainer().getMean(err);
			std::cout << "mean data: " << mean;
			std::cout <<  " +- "<< err << std::endl;
			datadep.setBinContent(bin,mean);
			datadep.setBinError(bin,err);
			//histo1D::debug=true;
			mean=stack.getSignalContainer().getMean(err);
			std::cout << "mean MC: " << mean;
			std::cout <<  " +- "<< err << std::endl;
			mcdep.setBinContent(bin,mean);
			mcdep.setBinError(bin,err);
			bin++;

			//stack.removeError(wparaPrefix_+"7_minus_down");//actually not needed
			stack.addRelErrorToBackgrounds(0.3,false,"BGvar",exclude); //excludes signal and data anyway

			selectedStacks.push_back(stack);

		}
	}
	histo1D::c_makelist=false;

	plotterCompare plc;
	plc.setNominalPlot(&datadep);
	plc.setLastNoLegend();
	plc.printToPdf("compareMeanPrefitData");
	plc.setComparePlot(&mcdep,0);
	plc.setLastNoLegend();
	plc.printToPdf("compareMeanPrefit");

	//now search for corresponding non-iso plots
	for(size_t i=0;i<selectedStacks.size();i++){
		TString searchname=selectedStacks.at(i).getName();
		searchname.ReplaceAll(isoplots,nonisoplots);
		histoStack stack=hsv->getStack(searchname);
		//stack.removeError(wparaPrefix_+"7_minus_down");//actually not needed
		stack.addRelErrorToBackgrounds(0.3,false,"BGvar",exclude); //excludes signal and data anyway

		nonisoStacks.push_back(stack);
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
		nonisodata*=0.8; //good choice usually the fit output for better starting paras
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
		pl.addPlot(&asymm,false);
		pl.setLastNoLegend();
		pl.addPlot(&symmmc,false);
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

void wA7Extractor::printVariations(const std::string& outdir)const{
	if(debug)
		std::cout << "wA7Extractor::printVariations" << std::endl;
	if(inputstacks_.size()<1)
		return;
	system(("mkdir -p "+outdir).data());
	plotterCompare plc;
	//maybe style file
	TFile * f = 0;
	if(outdir.length()>0)
		f =new TFile((outdir+"/vars.root").data(),"RECREATE");
	else
		f =new TFile("vars.root","RECREATE");

	std::vector<TString> sysnames=inputstacks_.at(0).getSystNameList();
	for(size_t i=0;i<inputstacks_.size();i++){
		histo1D mc=inputstacks_.at(i).getFullMCContainer();
		std::string plotname = textFormatter::makeCompatibleFileName( inputstacks_.at(i).getName().Data());
		if(outdir.length()>0)
			plotname="/"+plotname;
		for(size_t sys=0;sys<sysnames.size();sys++){
			std::string thisplotname = plotname+ sysnames.at(sys).Data();
			TCanvas cv(thisplotname.data());
			plc.usePad(&cv);
			thisplotname+=".pdf";
			std::vector<histo1D> var=mc.produceVariations(sysnames.at(sys));
			plc.setNominalPlot(&var.at(0));
			plc.setComparePlot(&var.at(1),0);
			plc.setComparePlot(&var.at(2),1);
			plc.draw();
			cv.Write();
			cv.Print((outdir+thisplotname).data());
			plc.clearPlots();
		}
	}
	f->Close();
	delete f;


}

void wA7Extractor::fitAll(){

	if(debug)
		std::cout << "wA7Extractor::fitAll" << std::endl;

	//use to do systematics etc.
	createVariates(inputstacks_,false,0); //can be used for pseudo experiments

	std::vector<TString> sysnames=mcdependencies_.at(0).getSystNames();
	size_t a7index=0;
	for(size_t i=0;i<sysnames.size();i++){
		//std::cout << sysnames.at(i)  << std::endl;
		if(sysnames.at(i) == wparaPrefix_+"7"){ a7index=i;break;}
		if(i==sysnames.size()-1)
			throw std::runtime_error("wA7Extractor::fitAll: could not find index for A7");
	}
	if(!npseudo_){
		if(debug)
			std::cout << "wA7Extractor::fitAll: nominal" <<std::endl;
		fit(-1,false,false,true); //only nominal
		graph resultgraph=fit(-1,false,false); //only nominal
		//get stat contribution

		float fullerr=fitter_.getParameterErr(a7index);
		float A7=fitter_.getParameter(a7index)+1;
		std::vector<double> nominalparameters=*fitter_.getParameters();

		std::cout << "A7ren (includes +1 ): " << A7 << " +-" << fullerr<<std::endl;
		//fitter_.getParameterNames()->at(a7index)="A7";
		fitter_.makeCorrTable().writeToPdfFile("correlations.pdf");

		//systematics:
		simpleFitter::printlevel=-1;
		//make PDF Gaussian NNPDF style
		float sumvariances=0;
		float npdf=0;
		for(size_t i=0;i<sysnames.size();i++){
			npdf+=2;
			if(!sysnames.at(i).BeginsWith("PDF"))continue;
			fit(i,true,false);
			float err=fitter_.getParameter(a7index)+1-A7;
			sumvariances+=err*err;
			fit(i,false,false);
			err=fitter_.getParameter(a7index)+1-A7;
			sumvariances+=err*err;
		}

		sleep(5);
		float pdferr=std::sqrt(1/npdf * sumvariances);
		graph pdferrg=resultgraph;
		pdferrg.setPointYContent(0,A7+pdferr);
		resultgraph.addErrorGraph("PDF_up",pdferrg);
		pdferrg.setPointYContent(0,A7-pdferr);
		resultgraph.addErrorGraph("PDF_down",pdferrg);


		//scale variations: envelope
		float scaleerrup=0,scaleerrdown=1000;

		for(size_t i=0;i<sysnames.size();i++){
			if(!sysnames.at(i).BeginsWith("SCALE"))continue;
			fit(i,true,false);
			float err=fitter_.getParameter(a7index)+1;
			if(scaleerrup<err)scaleerrup=err;
			if(scaleerrdown>err)scaleerrdown=err;
			fit(i,false,false);
			err=fitter_.getParameter(a7index)+1;
			if(scaleerrup<err)scaleerrup=err;
			if(scaleerrdown>err)scaleerrdown=err;
		}
		pdferrg.setPointYContent(0,scaleerrup);
		resultgraph.addErrorGraph("SCALE_up",pdferrg);
		pdferrg.setPointYContent(0,scaleerrdown);
		resultgraph.addErrorGraph("SCALE_down",pdferrg);



		for(size_t i=0;i<sysnames.size();i++){
			if((sysnames.at(i).BeginsWith("QCD")&& sysnames.at(i).EndsWith("QCD"))
					|| (sysnames.at(i).BeginsWith("signal")&& sysnames.at(i).EndsWith("signal")))
				continue;
			if(sysnames.at(i).BeginsWith("PDF"))continue;
			if(sysnames.at(i).BeginsWith("SCALE"))continue;

			graph vargraph=fit(i,true,false);
			formatter fmt;
			float err=fitter_.getParameter(a7index)+1-A7;
			std::string sysnamefix=textFormatter::fixLength(sysnames.at(i).Data(),10);
			sysnamefix+=" ";
			std::cout << "syst: " << sysnamefix << " up " << fmt.round(err/A7*100,0.01)<<"%" << std::endl;
			resultgraph.addErrorGraph(sysnames.at(i)+"_up",vargraph);

			vargraph=fit(i,false,false);
			err=fitter_.getParameter(a7index)+1-A7;
			std::cout << "syst: " << sysnamefix << " down " << fmt.round(err/A7*100,0.01)<<"%" << std::endl;
			resultgraph.addErrorGraph(sysnames.at(i)+"_down",vargraph);
		}

		resultgraph.writeToFile("resultgraph.ztop");
		std::cout << "total error: +" << resultgraph.getPointYErrorUp(0,false)<< " " << resultgraph.getPointYErrorDown(0,false)  << std::endl;

		resultgraph.coutAllContent(true);

		std::vector<double> fitted(sysnames.size() ,0);
		fitted.at(a7index)=A7;

		//post fit means plot
		histo1D datameans(detaranges_,"data","#Delta#eta(l,j)","Mean(2p_{T}^{#perp}/M_{W})");
		histo1D mcfitted(detaranges_,"fitted MC","#Delta#eta(l,j)","Mean(2p_{T}^{#perp}/M_{W})");
		for(size_t i=0;i<mcdependencies_.size();i++){
			histo1D data=datahistos_.at(i);
			histo1D mcfittedsingle=mcdependencies_.at(i).exportContainer(fitted);
			float err;
			datameans.setBinContent(i+1,data.getMean(err));
			datameans.setBinError(i+1,err);
			mcfitted.setBinContent(i+1,mcfittedsingle.getMean(err));
			mcfitted.setBinError(i+1,err);
		}

		plotterCompare plc;
		plc.setNominalPlot(&datameans);
		plc.setLastNoLegend();
		plc.setComparePlot(&mcfitted,0);
		plc.setLastNoLegend();
		plc.printToPdf("compareMeanPostfit");

	}



	TRandom3 randomizer;
	histo1D pullh(histo1D::createBinning(17,-8,8),"pull","pull","nExp");
	histo1D pullhabs(histo1D::createBinning(13,-0.3,0.3),"pull_abs","pull abs","nExp");
	if(npseudo_)
		std::cout << "pseudo experiments..." << std::endl;

	semiconst<float> mca7(0);
	for(size_t nexp=0;nexp<npseudo_;nexp++){
		//break;
		if(nexp){
			displayStatusBar(nexp,npseudo_,100,true);
			createVariates(inputstacks_,true,&randomizer);
			fit(-1,false,false);
		}
		else{
			createVariates(inputstacks_,true,&randomizer);
			fit(-1,false,false);
			if(fabs(fitter_.getParameters()->at(a7index))>fitter_.getParameterErr(a7index)){
				mca7.unlock();
				mca7=fitter_.getParameters()->at(a7index);
			}
			std::cout << "probable bias: "<< fitter_.getParameters()->at(a7index) <<" +- " << fitter_.getParameterErr(a7index) <<std::endl;
			std::cout <<std::endl;
		}
		float pull=0;
		float deva7=fitter_.getParameter(a7index);
		if(deva7>0)
			pull=(deva7)/fabs(fitter_.getParameterErrUp()->at(a7index));
		else
			pull=deva7/fabs(fitter_.getParameterErrDown()->at(a7index));
		//pull=deva7;
		//std::cout << deva7 <<" " << pull<< std::endl;
		pullhabs.fill(deva7);
		pullh.fill(pull);
	}
	if(npseudo_){
		for(int i=0;i<2;i++){
			histo1D * ph=&pullh;
			if(i)
				ph=&pullhabs;
			std::cout << std::endl<< "pull plot" <<std::endl;
			plotterMultiplePlots pl;
			pullh.writeToFile((ph->getName()+".ztop").Data());
			pl.addPlot(ph,false);
			pl.setLastNoLegend();

			graphFitter gf;
			graph fitg;
			fitg.import(ph,false);
			pl.readStyleFromFileInCMSSW("/src/WbWbXAnalysis/Analysis/configs/wExtractA7/multiplePlots_pulls.txt");

			gf.setFitMode(graphFitter::fm_gaus);
			gf.readGraph(&fitg);
			gf.setParameter(0,ph->getYMax(false));
			float dummy;
			std::cout << "Mean: " << ph->getMean(dummy) << std::endl;
			gf.setParameter(1,ph->getMean(dummy)); //xshift
			gf.setParameter(2,1); //width
			if(i)
				gf.setParameter(2,0.05); //width
			gf.setStrategy(0);
			gf.fit();
			formatter fmt;
			float roundpeak=fmt.round(gf.getParameter(1),0.01);
			float roundwidth=fmt.round(gf.getParameter(2),0.01);

			graph fitted=gf.exportFittedCurve(200,pullh.getBins().at(1),pullh.getBins().at(pullh.getBins().size()-1));
			fitted.setName("#splitline{peak: "+toTString(roundpeak) + "}{width: " +toTString(roundwidth)+"}");

			pl.addPlot(&fitted);

			pl.printToPdf(ph->getName().Data());
		}
	}

}

void signalHandler(int signum){

	std::cout << "Interrupt signal (" << signum << ") received.\n";

	// cleanup and close up stuff here
	// terminate program

	exit(signum);
}


graph wA7Extractor::fit(int sysindex, bool up, bool fixallbuta7, bool fixa7){

	if(debug)
		std::cout << "wA7Extractor::fit" << std::endl;

	if(mcdependencies_.size()<1)
		throw std::logic_error("wA7Extractor::fit: no input"); //make some reasonable output here
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
	graph out;

	std::vector<TString> systnames=mcdependencies_.at(0).getSystNames();


	fittedparas_=std::vector<double>(systnames.size(), 0); //SET ZERO
	std::vector<double> stepwidths(fittedparas_.size(),1);

	fitter_.setParameters(fittedparas_,stepwidths);
	fitter_.setRequireFitFunction(false);
	fitter_.setParameterNames(systnames);


	size_t a7idx=0;
	for(size_t i=0;i<fittedparas_.size();i++){
		if(systnames.at(i) == (wparaPrefix_+"7")){
			a7idx=i;
			if(fixa7){
				fitter_.setParameter(i,0);
				fittedparas_.at(i)=0;
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
				if(systnames.at(i)=="WparaA7_oops_up"){
					fitter_.setParameter(i,5);
					fittedparas_.at(i)=5;}
				else{
					fitter_.setParameter(i,1);
					fittedparas_.at(i)=1;}
			}
			else{
				if(systnames.at(i)=="WparaA7_oops_down"){
					fitter_.setParameter(i,-5);
					fittedparas_.at(i)=-5;
				}
				else{
					fitter_.setParameter(i,-1);
					fittedparas_.at(i)=-1;}
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

	if(debug)
		simpleFitter::printlevel=2;
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



	out.addPoint(1,fittedparas_.at(a7idx)+1,fitter_.getParameterErr(a7idx),"");

	if(npseudo_)
		return out;

	//if(!fixallbuta7){
	//DEBUG
	if(sysindex<0 && !fixallbuta7){
		std::cout << "fitted values: "<<std::endl;
		for(size_t i=0;i<fittedparas_.size();i++){
			std::cout << textFormatter::fixLength(systnames.at(i).Data(), 15) << ": " << fittedparas_.at(i) << " +" <<  fitter_.getParameterErrUp()->at(i)<<" -"
					<< fitter_.getParameterErrDown()->at(i)<<  std::endl;
		}

		//DEBUG
		TString prefix="";
		if(fixa7)
			prefix="A7fix_";
		else if (sysindex>-1){
			prefix=systnames.at(sysindex)+"_";
			if(up)
				prefix+="up_";
			else
				prefix+="down_";
		}
		for(size_t i=0;i<mcdependencies_.size();i++){
			histo1D mc=mcdependencies_.at(i).exportContainer(fittedparas_);
			histo1D data=datahistos_.at(i);
			histoStack stack;
			stack.push_back(mc,"MC",413,1,1);
			stack.push_back(data,"data",kBlack,1,99);
			plotterControlPlot pl;
			pl.readStyleFromFileInCMSSW("/src/WbWbXAnalysis/Analysis/configs/wExtractA7/prefitplots_style.txt");
			pl.setStack(&stack);
			pl.printToPdf((prefix+"fitted_"+toTString(i)).Data());

			stack.clear();
			stack.push_back(data,"data",kBlack,1,99);

			mc=mcdependencies_.at(i).exportContainer();
			stack.push_back(mc,"MC",412,1,1);
			pl.clearPlots();
			pl.cleanMem();
			pl.setStack(&stack);
			pl.printToPdf((prefix+"prefitted_"+toTString(i)).Data());
		}
	}
	return out;
}


void wA7Extractor::createVariates(const std::vector<histoStack>& inputstacks, bool onlyMC,
		TRandom3* rand){
	if(debug)
		std::cout << "wA7Extractor::createVariates" << std::endl;

	//make randomization here
	datahistos_.clear();
	mcdependencies_.clear();

	for(size_t i=0;i<inputstacks.size();i++){
		//add variation of QCD and signal
		if(debug)
			std::cout << "adding var to: " <<inputstacks.at(i).getName() <<std::endl;
		histoStack stack=inputstacks.at(i);
		histoStack stackcopy=stack;
		if(rand)
			stack.poissonSmearMC(rand,true,true);
		std::vector<float> newbins=enhanceStatistics(stack);

		for(size_t j=0;j<inputstacks.size();j++){
			float relerr=0;
			if(i==j)
				relerr=0.8;
			stack.addRelErrorToContribution(relerr,"QCD","QCD"+toTString(j));
			stack.addRelErrorToContribution(relerr,"signal","signal"+toTString(j));
			if(relerr && debug)
				std::cout << "added sig/QCD var to: " <<inputstacks.at(i).getName() <<std::endl;
		}

		//histoStack origstack=stack;

		variateHisto1D varmc;
		histo1D fullmc=stack.getFullMCContainer();
		varmc.import(fullmc);

		mcdependencies_.push_back(varmc);

		histo1D data;//=stack.getDataContainer();
		if(onlyMC){
			histoStack origstack=normfitted_inputstacks_.at(i);
			//enhanceStatistics(origstack);//potentially dangerous but ok since norm fitted is only scaled

			//histo1D fullmc2=origstack.getFullMCContainer();
			histo1D fullmc2=stackcopy.getFullMCContainer();
			data=fullmc2;
			data.createStatFromContent();
			if(rand)
				data=data.createPseudoExperiment(rand);
			if(newbins.size()>1)
				data=data.rebinToBinning(newbins);
		}
		else{
			data=stack.getDataContainer();
			data=data.rebinToBinning(newbins);
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

std::vector<float> wA7Extractor::enhanceStatistics(histoStack& stack)const{


	std::vector<size_t> sigidxs=stack.getSignalIdxs();
	if(sigidxs.size()>1 || sigidxs.size()<1)
		throw std::runtime_error("wA7Extractor::enhanceStatistics: too many signals or too less");



	size_t sigidx=sigidxs.at(0);

	histo1D signalhist=stack.getSignalContainer();
	if(enhancestat_)
		signalhist=signalhist.rebinToRelStat(mcstatthresh_);
	std::vector<float> bins(signalhist.getBins().begin()+1,signalhist.getBins().end()) ;

	//can be done for all but data
	for(size_t i=0;i<stack.size();i++){

		histo1D& hist=stack.getContainer(i);


		//continue;
		if(0 && stack.getDataIdx()!=i){
			histo1D nominal=hist.getSystContainer(-1);

			//need this for all syst!
			histo1D relvar=hist.getRelErrorsContainer();
			relvar.removeStatFromAll(true); //the a7 var is (almost) stat fully correlated

			histo1D mirroredone=nominal;
			mirroredone.mirror();

			nominal+=mirroredone;
			nominal*=relvar; //here we have the syst again
			nominal*=0.5;

			stack.getContainer(i)=nominal;
		}
		hist=hist.rebinToBinning(bins);
		//hist=hist.rebinToBinning(hist);
	}






	if(!npseudo_){
		histo1D signal=stack.getContainer(sigidx);

		histo1D a7oneh=signal;

		histo1D a7up=a7oneh.getSystContainer(a7oneh.getSystErrorIndex("WparaA7_up")) ;
		histo1D a7down=a7oneh.getSystContainer(a7oneh.getSystErrorIndex("WparaA7_down")) ;
		histo1D nom=a7oneh.getSystContainer(-1);
		signal.removeAllSystematics();
		plotterCompare pl;
		pl.setNominalPlot(&nom );
		pl.setComparePlot(&a7up ,0);
		pl.setComparePlot(&a7down ,1);
		pl.setComparePlot(&signal, 2);
		pl.printToPdf(textFormatter::makeCompatibleFileName(("variation"+stack.getName()).Data()));
	}

	return bins;
}

}






