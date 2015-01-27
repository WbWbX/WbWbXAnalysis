/*
 * fitTtBarXsec.cc
 *
 *  Created on: Oct 14, 2014
 *      Author: kiesej
 */


#include "TtZAnalysis/Tools/interface/applicationMainMacro.h"
#include "../interface/ttbarXsecFitter.h"
#include <iostream>
#include <string>
#include "TtZAnalysis/Tools/interface/containerStack.h"
#include "TtZAnalysis/Tools/interface/plotterControlPlot.h"
#include "TtZAnalysis/Tools/interface/plotterMultiplePlots.h"
#include "TtZAnalysis/Tools/interface/plotterCompare.h"
#include "TtZAnalysis/Tools/interface/plotter2D.h"
#include "TFile.h"
#include <TError.h>

invokeApplication(){
	using namespace ztop;

	parser->bepicky=true;
	const TString lhmode = parser->getOpt<TString>("m","poissondata",
			"modes for the likelihood assumption in the fit.\n possible: chi2data, chi2datamc, poissondata\ndefault: poissondata");
	const TString inputconfig = parser->getOpt<TString>("i","ttbarXsecFitter_mll.txt","input config (located in TtZAnalysis/Analysis/configs");
	const bool onlyMC=parser->getOpt<bool>("-onlyMC",false,"use plain MC, no data. Compare sum(MC) to MC. useful to check if a fit could work theoretically");
	if(lhmode!="chi2datamc" && lhmode!="chi2data" && lhmode!="poissondata"){
		parser->coutHelp();
		exit(-1);
	}
	const bool exclude0bjetbin  = parser->getOpt<bool>("-exclude0bjet",false,"Excludes 0,3+ bjet bin from the fit");
	const TString outfile = parser->getOpt<TString>("o","xsecFit","output file name");
	const int npseudoexp = parser->getOpt<int>("p",0,"number of pseudo experiments");
	const bool debug = parser->getOpt<bool>("d",false,"switches on debug output");
	const TString pseudoOpts = parser->getOpt<TString>("-pdopts","",
			"additional options for pseudodata:\nGaus: use Gaussian random distribution\n");
	const bool systbreak =! parser->getOpt<bool>("-nosyst",false,"switches off systematics breakdown");
	const bool nominos = parser->getOpt<bool>("-nominos",false,"switches off systematics breakdown");
	const float topmass = parser->getOpt<float>("-topmass",172.5,"Set top mass");

	parser->doneParsing();



	simpleFitter::printlevel=-1; //for now

	ttbarXsecFitter mainfitter(19741,2.6,5050,2.2);
	mainfitter.setXsec8(getTtbarXsec(topmass,8));

	TString mtrepl="_"+toTString(172.5)+"_";TString mtrplwith="_"+toTString(topmass)+"_";
	mainfitter.setReplaceInInfile(mtrepl,mtrplwith);

	mainfitter.setXsec7(getTtbarXsec(topmass,7));

	if(lhmode=="chi2datamc")
		mainfitter.setLikelihoodMode(ttbarXsecFitter::lhm_chi2datamcstat);
	if(lhmode=="chi2data")
		mainfitter.setLikelihoodMode(ttbarXsecFitter::lhm_chi2datastat);
	if(lhmode=="poissondata")
		mainfitter.setLikelihoodMode(ttbarXsecFitter::lhm_poissondatastat);

	mainfitter.setExcludeZeroBjetBin(exclude0bjetbin);
	mainfitter.setUseMCOnly(onlyMC);
	mainfitter.setNoMinos(nominos);
	mainfitter.setNoSystBreakdown(!systbreak);


	std::string cmsswbase=getenv("CMSSW_BASE");
	//extendedVariable::debug=true;
	ttbarXsecFitter::debug=debug;



	if(npseudoexp>0){
		mainfitter.setRemoveSyst(true);
		mainfitter.setSilent(true);
	}
	mainfitter.readInput((cmsswbase+"/src/TtZAnalysis/Analysis/configs/fitTtBarXsec/"+inputconfig).Data());
	extendedVariable::debug=false;


	////priors
	if(npseudoexp==0){
		try{
			mainfitter.setPrior("TT_SCALE",ttbarXsecFitter::prior_box);
			//mainfitter.setPrior("TOPPT",ttbarXsecFitter::prior_float);
			mainfitter.setPrior("TT_GENPOWPY",ttbarXsecFitter::prior_narrowboxright); //allow PH OR MG but nothing like PH++
			mainfitter.setPrior("CR",ttbarXsecFitter::prior_narrowboxleft); //allow down but not up
			mainfitter.setPrior("TT_MATCH",ttbarXsecFitter::prior_box);
			//mainfitter.setPrior("JES_Flavor",ttbarXsecFitter::prior_box);
			mainfitter.setPrior("TOPMASS",ttbarXsecFitter::prior_float);

			//mainfitter.setPrior("BTAGH",ttbarXsecFitter::prior_float);
			//mainfitter.setPrior("BTAGL",ttbarXsecFitter::prior_float);
		}catch(...){
			std::cout << "Some priors could not be set"<<std::endl;
		}
		try{
			mainfitter.addFullExtrapolError("TT_SCALE");
			mainfitter.addFullExtrapolError("TT_MATCH");
			//mainfitter.addFullExtrapolError("TOPPT");
		}
		catch(...){
			std::cout << "Extrapolation could not be set"<<std::endl;
		}

	}
	//ttbarXsecFitter::debug=true;
	bool doplotting=false;

	variateContainer1D::debug=false;
	if(npseudoexp){
		size_t failcount=0;
		gErrorIgnoreLevel = 3000;

		container1D pseudoout7(container1D::createBinning(40,-8,8)), pseudoout8(container1D::createBinning(40,-8,8));
		plotterControlPlot pl;
		pl.readStyleFromFileInCMSSW("/src/TtZAnalysis/Analysis/configs/fitTtBarXsec/controlPlots_combined.txt");
		TCanvas cvv;

		cvv.Print(outfile+"_pd0_7.pdf(");
		cvv.Print(outfile+"_pd1_7.pdf(");
		cvv.Print(outfile+"_pd2_7.pdf(");
		cvv.Print(outfile+"_pd0_8.pdf(");
		cvv.Print(outfile+"_pd1_8.pdf(");
		cvv.Print(outfile+"_pd2_8.pdf(");
		pl.usePad(&cvv);

		//configure mode
		container1D::pseudodatamodes pdmode=container1D::pseudodata_poisson;
		if(pseudoOpts.Contains("Gaus"))
			pdmode=container1D::pseudodata_gaus;
		std::cout << std::endl;
		for(int i=0;i<npseudoexp;i++){
			displayStatusBar(i,npseudoexp);
			float xsec7,xsec8,errup7,errup8,errdown7,errdown8;
			mainfitter.createPseudoDataFromMC(pdmode);
			mainfitter.createContinuousDependencies();
			try{
				mainfitter.fit(xsec8,errup8,errdown8,xsec7,errup7,errdown7);
			}catch(...){
				failcount++;
			}
			if(errup7 != 0 && errdown7!=0){
				if(xsec7>0)
					pseudoout7.fill(xsec7/errup7);
				else
					pseudoout7.fill(xsec7/errdown7);
			}
			if(errup8 != 0 && errdown8!=0){
				if(xsec8>0)
					pseudoout8.fill(xsec8/errup8);
				else
					pseudoout8.fill(xsec8/errdown8);
			}
			if(i<21){
				for(size_t nbjet=0;nbjet<3;nbjet++){
					for(size_t seveneight=0;seveneight<2;seveneight++){
						if(!mainfitter.has7TeV())
							seveneight++;
						double dummychi2;
						containerStack stack=mainfitter.produceStack(false,nbjet,seveneight,dummychi2);
						pl.setStack(&stack);
						pl.draw();
						if(seveneight)
							cvv.Print(outfile+"_pd"+nbjet+ "_7.pdf");
						else
							cvv.Print(outfile+"_pd"+nbjet+ "_8.pdf");
					}
				}
			}
		}
		std::cout << std::endl;
		cvv.Print(outfile+"_pd0_7.pdf)");
		cvv.Print(outfile+"_pd1_7.pdf)");
		cvv.Print(outfile+"_pd2_7.pdf)");
		cvv.Print(outfile+"_pd0_8.pdf)");
		cvv.Print(outfile+"_pd1_8.pdf)");
		cvv.Print(outfile+"_pd2_8.pdf)");

		pseudoout7.writeToTFile("pseudoout7.root");
		pseudoout8.writeToTFile("pseudoout8.root");
		//fit
		for(size_t i=0;i<2;i++){ //both pulls
			container1D * c=&pseudoout7;
			if(i)
				c=&pseudoout8;
			graph tofit; tofit.import(c,true);
			graphFitter fitter;
			fitter.readGraph(&tofit);
			fitter.setFitMode(graphFitter::fm_gaus);
			fitter.setParameter(0,0.4*c->integral(false)); //norm
			fitter.setParameter(1,0); //xshift
			fitter.setParameter(2,1); //width
			//fitter.printlevel =2;
			//graphFitter::debug=true;

			fitter.fit();
			//	bool succ=	fitter.wasSuccess();
			graph fitted=fitter.exportFittedCurve(500);
			formatter fmt;
			float roundpeak=fmt.round(fitter.getParameter(1),0.01);
			float roundwidth=fmt.round(fitter.getParameter(2),0.01);

			TCanvas * cv=new TCanvas();
			plotterMultiplePlots plm;
			plm.readStyleFromFileInCMSSW("/src/TtZAnalysis/Analysis/configs/fitTtBarXsec/multiplePlots_pulls.txt");
			plm.usePad(cv);
			plm.addPlot(c);
			plm.setLastNoLegend();
			fitted.setName("#splitline{peak: "+fmt.toTString(roundpeak) + "}{width: " +toTString(roundwidth)+"}");
			plm.addPlot(&fitted);
			plm.draw();
			if(i)
				cv->Print(outfile+"_pull8.pdf");
			else
				cv->Print(outfile+"_pull7.pdf");
			delete cv;
		}
		std::cout << "Pseudodata run done. " << failcount << " failed out of " << npseudoexp << std::endl;

		return 0;
	}
	else{
		doplotting=true;
		//ttbarXsecFitter::debug=true;
		if(debug){
			simpleFitter::printlevel=1;
		}
		mainfitter.fit();
	}


	///////plotting here

	if(doplotting){
		TFile * f = new TFile(outfile+".root","RECREATE");

		//compare input stacks before and after

		containerStack stack;
		plotterControlPlot pl;
		pl.readStyleFromFileInCMSSW("/src/TtZAnalysis/Analysis/configs/fitTtBarXsec/controlPlots_combined.txt");
		TCanvas c;
		c.Print(outfile+".pdf(");
		pl.usePad(&c);
		for(size_t seveneight=0;seveneight<2;seveneight++){
			for(size_t nbjet=0;nbjet<3;nbjet++){
				if(!mainfitter.has7TeV())
					seveneight++;
				double chi2=0;
				containerStack stack=mainfitter.produceStack(false,nbjet,seveneight,chi2);
				pl.setStack(&stack);
				textBoxes tb;
				tb.add(0.7,0.73,"#chi^{2}="+toTString(chi2));
				pl.draw();
				tb.drawToPad(c.cd(1),true);
				c.Print(outfile+".pdf");
				tb.clear();
				stack=mainfitter.produceStack(true,nbjet,seveneight,chi2);
				pl.setStack(&stack);
				tb.add(0.7,0.5,"#chi^{2}="+toTString(chi2));
				pl.draw();
				tb.drawToPad(c.cd(1),true);
				c.Print(outfile+".pdf");
			}
			plotterCompare plc;
			plc.usePad(&c);
			plc.readStyleFromFileInCMSSW("/src/TtZAnalysis/Analysis/configs/fitTtBarXsec/comparePlots_Cb.txt");
			c.SetName("c_b");
			container1D ctmp=mainfitter.getCb(false,seveneight);
			ctmp.setName("C_{b}  pre-fit");
			plc.setNominalPlot(&ctmp);
			ctmp=mainfitter.getCb(true,true);
			ctmp.setName("C_{b} post-fit");
			plc.setComparePlot(&ctmp,0);
			plc.draw();
			c.Print(outfile+".pdf");
			c.Write();
			plc.clearPlots();

			c.SetName("eps_b");
			ctmp=mainfitter.getEps(false,seveneight);
			ctmp.setName("#epsilon_{b}  pre-fit");
			plc.readStyleFromFileInCMSSW("/src/TtZAnalysis/Analysis/configs/fitTtBarXsec/comparePlots_epsb.txt");
			plc.setNominalPlot(&ctmp);
			ctmp=mainfitter.getEps(true,true);
			ctmp.setName("#epsilon_{b} post-fit");
			plc.setComparePlot(&ctmp,0);
			plc.draw();
			c.Print(outfile+".pdf");
			c.Write();
		}


		c.SetName("correlations");
		plotter2D pl2d;
		pl2d.usePad(&c);
		container2D corr2d=mainfitter.getCorrelations();
		pl2d.setPlot(&corr2d);
		pl2d.draw();
		c.Print(outfile+".pdf");
		c.Write();

		texTabler tab8=mainfitter.makeSystBreakdown(8);
		tab8.writeToFile(outfile+"_tab8.tex");
		tab8.writeToPdfFile(outfile+"_tab8.pdf");
		std::cout << tab8.getTable() <<std::endl;
		if(mainfitter.has7TeV()){
			texTabler tab7=mainfitter.makeSystBreakdown(7);
			tab7.writeToFile(outfile+"_tab7.tex");
			tab7.writeToPdfFile(outfile+"_tab7.pdf");
			std::cout << tab7.getTable() <<std::endl;
		}


		texTabler corr=mainfitter.makeCorrTable();
		corr.writeToFile(outfile+"_tabCorr.tex");
		corr.writeToPdfFile(outfile+"_tabCorr.pdf");
		//std::cout << corr.getTable() <<std::endl;

		c.Print(outfile+".pdf)");
		f->Close();
		delete f;
	}
	return 0;
}
