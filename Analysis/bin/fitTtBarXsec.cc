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
#include "TtZAnalysis/Tools/interface/histoStack.h"
#include "TtZAnalysis/Tools/interface/plotterControlPlot.h"
#include "TtZAnalysis/Tools/interface/plotterMultiplePlots.h"
#include "TtZAnalysis/Tools/interface/plotterCompare.h"
#include "TtZAnalysis/Tools/interface/plotter2D.h"
#include "TFile.h"
#include <TError.h>
#include <TtZAnalysis/Tools/interface/plotterMultiColumn.h>
#include "TtZAnalysis/Tools/interface/fileReader.h"
#include "TtZAnalysis/Tools/interface/histoStackVector.h"

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
	const int npseudoexp = parser->getOpt<int>("p",0,"number of pseudo experiments");
	const bool debug = parser->getOpt<bool>("d",false,"switches on debug output");
	const TString pseudoOpts = parser->getOpt<TString>("-pdopts","",
			"additional options for pseudodata:\nGaus: use Gaussian random distribution\n");
	const bool fitsystematics =! parser->getOpt<bool>("-nosyst",false,"removes systematics");
	const bool onlytotalerror = parser->getOpt<bool>("-onlytotal",false,"no syst breakdown");
	const bool onlycontrolplots = parser->getOpt<bool>("-onlycontrol",false,"only control plots");
	const bool nominos = parser->getOpt<bool>("-nominos",false,"switches off systematics breakdown");
	const float topmass = parser->getOpt<float>("-topmass",0,"Set top mass");
	const float tmpcheck = parser->getOpt<bool>("M",false,"Quick temp check");
	const bool candc = parser->getOpt<bool>("-cutandcount",false,"also produce fast cut and count (2 jets 1 b-tag) result");

	const bool printplots = ! parser->getOpt<bool>("-noplots",false,"switches off all plotting");
	const bool dummyrun =  parser->getOpt<bool>("-dummyrun",false,"fit will not be performed, only dummy values will be returned (for testing of plots etc)");

	const bool variationplots = parser->getOpt<bool>("-varplots",false,"switches on variation plots");

	TString outfile;

	outfile = parser->getOpt<TString>("o","xsecFit","output file name");


	parser->doneParsing();
	std::string cmsswbase=getenv("CMSSW_BASE");


	const std::string fullcfgpath=
			inputconfig.BeginsWith("/") || inputconfig.BeginsWith("./") ? "" :
					(cmsswbase+"/src/TtZAnalysis/Analysis/configs/fitTtBarXsec/");
	if(!fileExists((fullcfgpath+inputconfig).Data())){
		std::cout << "fitTtBarXsec: input file not found. \nAvailable files in " <<cmsswbase+"/src/TtZAnalysis/Analysis/configs/fitTtBarXsec:"<<std::endl;
		system(("ls "+fullcfgpath).data());
		return -1;
	}

	ttbarXsecFitter mainfitter;
	mainfitter.setDummyFit(dummyrun);

	if(lhmode=="chi2datamc")
		mainfitter.setLikelihoodMode(ttbarXsecFitter::lhm_chi2datamcstat);
	if(lhmode=="chi2data")
		mainfitter.setLikelihoodMode(ttbarXsecFitter::lhm_chi2datastat);
	if(lhmode=="poissondata")
		mainfitter.setLikelihoodMode(ttbarXsecFitter::lhm_poissondatastat);

	if(topmass)
		mainfitter.setReplaceTopMass(topmass);

	mainfitter.setExcludeZeroBjetBin(exclude0bjetbin);
	mainfitter.setUseMCOnly(onlyMC);
	mainfitter.setNoMinos(nominos);
	mainfitter.setNoSystBreakdown((onlytotalerror));
	mainfitter.setIgnorePriors(!fitsystematics);
	mainfitter.setRemoveSyst(!fitsystematics);

	//extendedVariable::debug=true;
	ttbarXsecFitter::debug=debug;



	if(npseudoexp>0){
		mainfitter.setRemoveSyst(true);
		mainfitter.setSilent(true);
		mainfitter.setIgnorePriors(true);
	}
	//simpleFitter::printlevel=1;

	mainfitter.readInput((fullcfgpath+inputconfig).Data());
	std::cout << "Input file sucessfully read. Free for changes." << std::endl;

	if(onlycontrolplots){

		//get list of input files, fast brute force -> can be changed since input is read by
		// fitter before, now

		fileReader fr;
		fr.setComment("#");
		fr.setDelimiter(",");
		fr.setStartMarker("");
		fr.setEndMarker("");
		fr.readFile((fullcfgpath+inputconfig).Data());
		std::vector<std::string>  filestoprocess;
		std::vector<std::string>  datasets;
		for(size_t i=0;i<fr.nLines();i++){
			if(fr.nEntries(i)){
				for(size_t j=0;j<fr.nEntries(i);j++){
					std::string tmp=fr.getData<std::string>(i,j);
					if(tmp.find(".ztop") != std::string::npos){
						textFormatter ftm;
						ftm.setDelimiter("|");
						std::vector<std::string> formline=ftm.getFormatted(tmp);
						for(size_t lentr=0;lentr<formline.size();lentr++){
							if(fileExists(formline.at(lentr).data())){
								if(std::find(filestoprocess.begin(),filestoprocess.end(),formline.at(lentr)) == filestoprocess.end()){

									filestoprocess.push_back(formline.at(lentr));
									//get dataset name
									ftm.setDelimiter("_");
									std::vector<std::string> nameparts=ftm.getFormatted(formline.at(lentr));
									for(size_t part=0;part<nameparts.size();part++){
										if(nameparts.at(part).find("TeV")!=std::string::npos){

											datasets.push_back(nameparts.at(part));
										}
									}
								}
							}
						}
					}
				}
			}
		}

		std::string specialplotsfile=cmsswbase+"/src/TtZAnalysis/Analysis/configs/fitTtBarXsec/selected_controlplots.txt";


		//get files

		for(size_t file=0;file<filestoprocess.size();file++){
			TString ctrplotsoutir=outfile+"_controlPlots";
			//ctrplotsoutir+="_"+datasets.at(file);
			system(((TString)"mkdir -p " +ctrplotsoutir).Data());
			histoStackVector vec;
			vec.readFromFile(filestoprocess.at(file));
			fileReader specialcplots;
			specialcplots.setComment("$");
			std::cout << datasets.at(file) << std::endl;
			//fileReader::debug=true;
			specialcplots.setStartMarker("[additional plots "+ datasets.at(file)+ "]");
			specialcplots.setEndMarker("[end - additional plots]");
			specialcplots.setDelimiter(",");
			specialcplots.readFile(specialplotsfile);
			//fileReader::debug=false;
			//get plot names
			std::vector<std::string> plotnames= specialcplots.getMarkerValues("plot");

			std::string fracfile=specialcplots.dumpFormattedToTmp();

			for(size_t stackit=0;stackit<plotnames.size();stackit++){
				histoStack stack=vec.getStack(plotnames.at(stackit).data());
				plotterControlPlot pl;
				//plotterControlPlot::debug=true;
				//histoStack::debug=true;
				pl.readStyleFromFileInCMSSW(specialcplots.getValue<std::string>("defaultStyle"));
				std::string mergelgdefs=(std::string)getenv("CMSSW_BASE")+(std::string)"/src/TtZAnalysis/Analysis/configs/fitTtBarXsec/controlPlots_mergeleg.txt";
				pl.addStyleFromFile( mergelgdefs,"[merge for control plots]","[end - merge for control plots]");
				//	textBoxes::debug=true;
				if(stackit+1<plotnames.size())
					pl.addStyleFromFile(fracfile,  "[plot - " + plotnames.at(stackit)+"]", "[plot - "+ plotnames.at(stackit+1) +"]" );
				else
					pl.addStyleFromFile(fracfile,  "[plot - " + plotnames.at(stackit) +"]","[end - additional plots]");
				//pl.readTextBoxesInCMSSW("/src/TtZAnalysis/Analysis/configs/general/CMS_boxes.txt","CMSSplit03Left");
				size_t datasetidx=mainfitter.getDatasetIndex(datasets.at(file).data() );
				mainfitter.addUncertainties(&stack, datasetidx);
				pl.setStack(&stack);
				pl.printToPdf((ctrplotsoutir+"/"+stack.getFormattedName()+"_"+datasets.at(file)).Data());
				pl.saveAsCanvasC((ctrplotsoutir+"/"+stack.getFormattedName()+"_"+datasets.at(file)).Data());
			}
			system(("rm -f "+fracfile).data());
		}

		return 0;
	} //onlycontrolplots


	//simpleFitter::printlevel=-1; //for now



	if(candc){
		std::cout << "C&C cross check:" <<std::endl;
		for(size_t i=0;i<mainfitter.nDatasets();i++){
			mainfitter.cutAndCountSelfCheck(i);
			std::cout << std::endl;
		}
	}
	bool fitsucc=true;
	//ttbarXsecFitter::debug=true;

	if(npseudoexp){
		size_t failcount=0;
		gErrorIgnoreLevel = 3000;
		size_t ndatasets=mainfitter.nDatasets();

		std::vector<histo1D> pulls;
		pulls.resize(ndatasets,histo1D(histo1D::createBinning(40,-8,8)));

		//plotterControlPlot pl;
		TCanvas cvv;

		//	pl.usePad(&cvv);

		//configure mode
		histo1D::pseudodatamodes pdmode=histo1D::pseudodata_poisson;
		if(pseudoOpts.Contains("Gaus"))
			pdmode=histo1D::pseudodata_gaus;
		std::cout << std::endl;
		for(int i=0;i<npseudoexp;i++){
			if(i>0)
				displayStatusBar(i,npseudoexp);
			std::vector<float> xsecs,errup,errdown;
			mainfitter.createPseudoDataFromMC(pdmode);
			mainfitter.createContinuousDependencies();
			if(!i)
				std::cout << "creating pseudo experiments...\n" <<std::endl;
			bool succ=true;
			try{
				mainfitter.fit(xsecs,errup,errdown);
			}catch(...){
				failcount++;
				succ=false;
			}
			if(succ && ! tmpcheck){
				for(size_t ndts=0;ndts<ndatasets;ndts++){
					double pull=xsecs.at(ndts)-mainfitter.getXsecOffset(ndts);
					if(pull>0)
						pull/=fabs(errup.at(ndts));
					else
						pull/=fabs(errdown.at(ndts));
					pulls.at(ndts).fill ( pull);
				}
			}

			if(succ && i<3 && 0){ //off for testing
				for(size_t nbjet=0;nbjet<3;nbjet++){
					for(size_t ndts=0;ndts<ndatasets;ndts++){
						mainfitter.printControlStack(false,nbjet,ndts,"pd_"+toString(i));
					}
				}
			}
		}
		std::cout << std::endl;
		histo1D abspull=(histo1D::createBinning(40,-1,1));
		if(tmpcheck){

			const std::vector<float> & par=mainfitter.tempdata;
			for(size_t i=0;i<par.size();i++){
				abspull.fill(par.at(i)*6); //HARDCODED
				double pull = par.at(i) / (std::max(fabs(par.at(i+1)),fabs(par.at(i+2))));

				++i;++i;
				for(size_t ndts=0;ndts<ndatasets;ndts++){
					pulls.at(ndts).fill ( pull);
				}
			}
		}

		//fit
		for(size_t i=0;i<pulls.size()+1;i++){ //both pulls
			histo1D * c=0;
			if(i<pulls.size())
				c=&pulls.at(i);
			else
				c=&abspull;
			graph tofit; tofit.import(c,true);
			graphFitter fitter;
			fitter.readGraph(&tofit);
			fitter.setFitMode(graphFitter::fm_gaus);
			fitter.setParameter(0,0.5*c->integral(false)); //norm
			fitter.setParameter(1,0); //xshift
			fitter.setParameter(2,1); //width
			fitter.setStrategy(1);
			if(i==pulls.size())
				fitter.setParameter(2,0.05); //width

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
			std::string outfilefull="";
			if(i<pulls.size()){
				TFile f(outfile+"_pull_" + mainfitter.datasetName(i)+ ".root","RECREATE");
				cv->Write();
				f.Close();
				cv->Print(outfile+"_pull_" + mainfitter.datasetName(i)+ ".pdf");
				outfilefull=toString(outfile+"_pull_" + mainfitter.datasetName(i)+".ztop");
			}
			else{
				TFile f(outfile+"_pull_abs.root","RECREATE");
				cv->Write();
				f.Close();
				cv->Print(outfile+"_pull_abs.pdf");
				outfilefull=toString(outfile+"_pull_abs.ztop");
			}
			delete cv;
			c->writeToFile(outfilefull); //for post-fits

		}
		std::cout << "Pseudodata run done. " << failcount << " failed out of " << npseudoexp << std::endl;

		return 0;
	}
	else{
		//ttbarXsecFitter::debug=false;
		//simpleFitter::printlevel=1; //for now
		try{
			mainfitter.fit();
		}
		catch(std::exception &e){
			std::cout << "Fit failed, will do plots only" <<std::endl;
			fitsucc=false;
			e.what();
		}
	}


	///////plotting here

	TFile * f = new TFile(outfile+".root","RECREATE");

	//compare input stacks before and after

	if(printplots){
		histoStack stack;
		//plotterControlPlot pl;
		//pl.readStyleFromFileInCMSSW("/src/TtZAnalysis/Analysis/configs/fitTtBarXsec/controlPlots_combined.txt");
		TCanvas c;
		c.Print(outfile+".pdf(");
		//pl.usePad(&c);
		for(size_t ndts=0;ndts<mainfitter.nDatasets();ndts++){
			TString dir=outfile+"_vars/";
			system( ("mkdir -p "+dir).Data());
			for(size_t nbjet=0;nbjet<3;nbjet++){

				mainfitter.printControlStack(false,nbjet,ndts,outfile.Data());
				mainfitter.printControlStack(true,nbjet,ndts,outfile.Data());

				if(variationplots){
					mainfitter.printVariations(nbjet,ndts,dir.Data());
					mainfitter.printVariations(nbjet,ndts,dir.Data(),true);
				}
			}

		}


		c.SetName("correlations");
		plotter2D pl2d;
		pl2d.readStyleFromFileInCMSSW("src/TtZAnalysis/Analysis/configs/mtFromXsec2/plot2D_0to1.txt");
		pl2d.usePad(&c);
		histo2D corr2d=mainfitter.getCorrelations();
		pl2d.setPlot(&corr2d);
		if(fitsucc){
			pl2d.draw();
			c.Print(outfile+".pdf");
			c.Write();
		}

		c.Print(outfile+".pdf)");
	}
	for(size_t ndts=0;ndts<mainfitter.nDatasets();ndts++){
		TString dtsname=mainfitter.datasetName(ndts);
		texTabler tab;

		tab=mainfitter.makeSystBreakDownTable(ndts);
		tab.writeToFile(outfile+"_tab" +dtsname + ".tex");
		tab.writeToPdfFile(outfile+"_tab" +dtsname + ".pdf");
		std::cout << tab.getTable() <<std::endl;
		tab=mainfitter.makeSystBreakDownTable(ndts,false); //vis PS
		tab.writeToFile(outfile+"_tab_simple" +dtsname + ".tex");
		tab.writeToPdfFile(outfile+"_tab_simple" +dtsname + ".pdf");
		std::cout << tab.getTable() <<std::endl;

		//graph out
		graph resultsgraph=mainfitter.getResultsGraph(ndts,topmass);
		if(fitsucc)
			resultsgraph.writeToFile((outfile+"_graph" +dtsname + ".ztop").Data());

	}
	if(mainfitter.nDatasets()>1 && fitsucc){
		std::cout << "producing ratio..." <<std::endl;
		for(size_t ndts=0;ndts<mainfitter.nDatasets();ndts++){
			for(size_t ndts2=ndts;ndts2<mainfitter.nDatasets();ndts2++){
				if(ndts2==ndts) continue;
				double errup,errdown;
				formatter fmt;
				double ratio=mainfitter.getXsecRatio(ndts,ndts2,errup,errdown);//full ps
				texTabler ratiotab(" c | l l l");
				ratiotab << "Ratio: " + mainfitter.datasetName(ndts) + "/" + mainfitter.datasetName(ndts2) ;
				ratiotab << fmt.round(ratio,0.01) << "$\\pm$" << "$^{" + fmt.toTString(fmt.round(errup,0.01)) +"}_{" +
						fmt.toTString(fmt.round(errdown,0.01)) +"}$";
				ratiotab.writeToFile(outfile+"_ratio_" +mainfitter.datasetName(ndts) +"_"+mainfitter.datasetName(ndts2)+".tex");
				ratiotab.writeToPdfFile(outfile+"_ratio_" +mainfitter.datasetName(ndts) +"_"+mainfitter.datasetName(ndts2)+".pdf");

			}
		}
	}
	std::cout << "writing correlation table..." <<std::endl;
	texTabler corr=mainfitter.makeCorrTable();
	corr.writeToFile(outfile+"_tabCorr.tex");
	corr.writeToPdfFile(outfile+"_tabCorr.pdf");
	//std::cout << corr.getTable() <<std::endl;

	f->Close();
	delete f;
	if(printplots){
		std::cout << "making additional plots (can be canceled)..." <<std::endl;
		//make some plots with and without constraints
		//test area
		for(size_t ndts=0;ndts<mainfitter.nDatasets();ndts++){
			std::string infile="emu_";
			infile+=mainfitter.datasetName(ndts).Data();
			infile+="_172.5_nominal_syst.ztop";
			std::string dir=outfile.Data();
			dir+="_";
			dir+=mainfitter.datasetName(ndts).Data();
			system(("mkdir -p "+dir).data());
			dir+="/";
			mainfitter.printAdditionalControlplots(infile,cmsswbase+"/src/TtZAnalysis/Analysis/configs/fitTtBarXsec/prefit_postfit_plots.txt",dir);


		}
	}
	std::cout << "fitTtBarXsec done" << std::endl;
	return 0;
}
