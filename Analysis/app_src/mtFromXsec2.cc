/*
 * mtFromXSec2.cc
 *
 *  Created on: Feb 20, 2014
 *      Author: kiesej
 */
#include "TtZAnalysis/Tools/interface/optParser.h"
#include "TtZAnalysis/Tools/interface/graph.h"
#include "TtZAnalysis/Tools/interface/container.h"
#include "../interface/mtExtractor.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"

#include "TString.h"
#include <vector>
#include "TFile.h"

#include "TtZAnalysis/Tools/interface/textFormatter.h"
#include "TCanvas.h"
#include "TPavesText.h"
#include "TLatex.h"
#include "TtZAnalysis/Tools/interface/plotterControlPlot.h"

#include "TtZAnalysis/Tools/interface/fileReader.h"
#include "TStyle.h"
#include "TtZAnalysis/Tools/interface/formatter.h"

#include "TtZAnalysis/Tools/interface/plotterMultiplePlots.h"
#include "TtZAnalysis/Tools/interface/parameterExtractor.h"
#include "TColor.h"
#include <iomanip>
#include "TError.h"

#include "../interface/paraExtrPull.h"
#include "TRandom3.h"
#include "TtZAnalysis/Tools/interface/systAdder.h"

#include <omp.h>

//very ugly function

void makePseudoExperiments(ztop::mtExtractor* ex,size_t niter,float evalpoint, TString output){
	using namespace ztop;


	//one could make this a loop for calibration

	std::vector<float> binning;
	for(float i=-6;i<=6;i+=0.25) binning<<i;

	TRandom3* random=new TRandom3();




	//hopefully this hack disables all that unnecessary utput of tfitter....
	gErrorIgnoreLevel = kWarning;
	output.ReplaceAll("summary.pdf","");

	TFile * f  = new TFile(output+".root","RECREATE");
	TCanvas cv1;

	//for a straight line

	ex->getExtractor()->setFastMode(true); //we dont need graphical output for each pull

	parameterExtractor::debug=false;
	//
	//get functional form -> later you might want to fit some TBI inputs here
	//now just dummys

	paraExtrPull pex;
	pex.setExtractor(ex->getExtractor());

	pex.setRandom(random);
	pex.setNIterations(niter);
	//pex.setFunctionalForm(-243.761,-614.668,5.90438);
	//overwrites func form

	std::cout << "external parameters or " << ex->getExtractor()->getFittedParasB().size() << " bins" <<std::endl;


	std::vector<ztop::graph> mcuncorr=ex->makeSignalBinGraphs();
	std::vector<ztop::graph> mccorr=ex->makeBGBinGraphs();
	const std::vector<ztop::graph>& databingraphs=ex->getDataBingraphs();
	pex.setDataGraphInput(databingraphs);
	pex.setUncorrelatedMCInput(mcuncorr);
	pex.setCorrelatedMCInput(mccorr);
	pex.setFitParasTrue(ex->getExtractor()->getFittedParasB());
	//pex.setFunctionalForm(offsetatdefmtop,slope,0.0001);


	std::cout << "creating pull " <<std::endl;

	std::vector<container1D > allpulls;
	size_t npulls=3;
	std::vector<float> evalpoints;

	evalpoints << 169.5 << 172.5 << 175.5;

	for(size_t i=0;i<npulls;i++){

		pex.setEvalPoint(evalpoints.at(i));
		container1D  ctemp(binning);
		ctemp.setName("pull"+i);
		ctemp.setXAxisName("m_{t,out}-m_{t,in} / #Delta_{stat}");
		//do stuff
		pex.setFillContainer1D(&ctemp);

		pex.fill();

		ctemp.writeToTFile(f);

		allpulls.push_back(ctemp);

	}


	plotterMultiplePlots plotterm;
	plotterm.readStyleFromFileInCMSSW("src/TtZAnalysis/Analysis/configs/topmass/multiplePlots_pulls.txt");
	plotterm.usePad(&cv1);
	plotterm.cleanMem();
	plotterm.clear();

	for(size_t i=0;i<allpulls.size();i++){

		//Fit the stuff
		container1D & ctemp= allpulls.at(i);

		ctemp *= (((float)i+1)/2);

		graph tofit; tofit.import(&ctemp,true);

		graphFitter fitter;
		fitter.readGraph(&tofit);
		fitter.setFitMode(graphFitter::fm_gaus);
		fitter.setParameter(0,0.4*ctemp.integral(false)); //norm
		fitter.setParameter(1,0); //xshift
		fitter.setParameter(2,1); //width
		//fitter.printlevel =2;
		//graphFitter::debug=true;

		fitter.fit();
		bool succ=	fitter.wasSuccess();

		if(!succ){
			std::cout << "Fit was not successful, ignore results!" <<std::endl;
		}

		std::cout << "peak:  " << fitter.getParameter(1) <<std::endl;
		std::cout << "width: " << fitter.getParameter(2) <<std::endl;

		formatter fmt;
		float roundpeak=fmt.round(fitter.getParameter(1),0.01);
		float roundwidth=fmt.round(fitter.getParameter(2),0.01);


		graph fitted=fitter.exportFittedCurve(500);

		plotterm.addPlot(&tofit);
		plotterm.setLastNoLegend();
		fitted.setName("#splitline{m_{t}=" + toTString(evalpoints.at(i)) + " GeV}{peak: "+toTString(roundpeak) + ", width: " +toTString(roundwidth)+"}");
		plotterm.addPlot(&fitted);
		//plotterm.setLastNoLegend();
	}

	cv1.Clear();
	cv1.SetName("withfit");
	plotterm.draw();
	cv1.Write();

	cv1.Print(output+"_fit.pdf");

	f->Close();
	//last
	//delete ctemp;
}
TString replaceAllExtraChars(const TString & in){
	TString out=in;
	out.ReplaceAll(" ","_");
	out.ReplaceAll("\\","_");
	out.ReplaceAll("#","_");
	out.ReplaceAll("(","_");
	out.ReplaceAll(")","_");
	out.ReplaceAll("}","_");
	out.ReplaceAll("{","_");

	return out;
}


std::vector<TString> filelist;

void WriteAndPrint(TCanvas*c, TFile &f,TString outputpdf, TString outdir){
	outdir=replaceAllExtraChars(outdir); //ReplaceAll(" ","_");
	outputpdf=replaceAllExtraChars(outputpdf);

	static std::vector<size_t> doublecount;
	if(doublecount.size() != filelist.size())
		doublecount.resize(filelist.size(),0);

	TString outname=c->GetName();
	outname=replaceAllExtraChars(outname);
	std::vector<TString>::iterator flit=std::find(filelist.begin(),filelist.end(),outdir+"/" + outname);
	if(flit != filelist.end()){
		size_t prevpos = flit-filelist.begin();
		doublecount.at(prevpos) ++;
		outname+="_"+ztop::toTString(doublecount.at(prevpos));
	}

	c->Print(outputpdf+".pdf");
	f.WriteTObject(c);
	c->Print(outdir+"/" + outname+".eps");
	filelist.push_back(outdir+"/" + outname);

}


int main(int argc, char* argv[]){
	using namespace ztop;

	//get options
	//plotname, (fitmode), minbin, maxbin, excludebin
	std::string cmsswbase=getenv("CMSSW_BASE");
	// optParser::debug=true;
	optParser parse(argc,argv);
	parse.bepicky=true;
	const  TString plotname  =    parse.getOpt<TString>("p","m_lb pair comb unfold step 8","input plot name");
	TString output    =    parse.getOpt<TString>("o","","output file name extra");
	TString outdirectory   =    parse.getOpt<TString>("-dir","mtextract","output directory name");
	const TString mode    =    parse.getOpt<TString>("m","fit","mode (fit, fitintersect,chi2)");
	const TString fitmode    =    parse.getOpt<TString>("f","pol2","fit mode in root format");
	const float defmtop    =    parse.getOpt<float>("-defmt",172.5,"use other default mtop (used for calibration)");
	const TString usepdf    =    parse.getOpt<TString>("-pdf","MSTW200","chose a PDF to be used (if external input is fed): default: MSTW200");
	const bool rescaletonnlonnll = parse.getOpt<bool>   ("K",false,"rescale NLO norm to NNLO+NNLL using pdf dependent k-factor");
	const int minbin          =  parse.getOpt<int>    ("-minbin",-1,"minimum bin number to be considered in fit");
	const int maxbin          =  parse.getOpt<int>    ("-maxbin",-1,"maximum bin number to be considered in fit");
	const int excludebin      =  parse.getOpt<int>    ("-excludebin",-1,"exclude bin from fit");
	const bool usefolding     = ! parse.getOpt<bool>   ("-nofold",false,"dont! use folding of genertor input");
	const bool nosyst     =  parse.getOpt<bool>   ("-nosyst",false,"skip systematics");
	const bool onlysignalstat     =  parse.getOpt<bool>   ("-onlysignalstat",false,"skip systematics");
	std::string extconfigfile      =  parse.getOpt<std::string>    ("-config",
			((TString)cmsswbase+"/src/TtZAnalysis/Analysis/configs/mtExtractor_config.txt").Data(),
			"specify config file for external gen input");
	const bool normalize  = parse.getOpt<bool>   ("-normalize",false,"normalize all distributions");
	const bool makepdfs   =  parse.getOpt<bool>   ("-makepdfs",false,"converts output from eps to pdf");

	const bool onlyCP =  ! parse.getOpt<bool>   ("-extract",false,"really perform extraction");

	const TString extprefix =  parse.getOpt<TString>("-prefix","mcfm_","external file prefix (everything before topmass)");

	const float createpseudoexp = parse.getOpt<float>("-pseudo",-1,"number of pseudo eperiments to generate based on prediction input");

	std::vector<TString> inputfiles = parse.getRest< TString >();

	parse.doneParsing();

	if(output.Length()>0)
		outdirectory = output +"_" + outdirectory;





	//requirements
	if(inputfiles.size() <1 || (mode!="fit" && mode!="chi2"&&mode!="fitintersect")){

		parse.coutHelp();
		std::cout << "specify at least one input file" <<std::endl;
		return -1;
	}

	std::cout << "starting mt extraction..." <<std::endl;


	//fileReader::debug=true;
	/*
	 *
    void setPlot(const TString& pl){plotnamedata_=pl;}
    void setMinBin(const int& pl){minbin_=pl;}
    void setMaxBin(const int& pl){excludebin_=pl;}
    void setExcludeBin(const int& pl){excludebin_=pl;}
    void setInputFilesData(const std::vector<TString>& pl){inputfiles_=pl;}
	 *
	 */

	//do this for each channel independently. Right now for testing reasons skip channel asso
	// maybe then also implement all plots.. but later
	mtExtractor extractor;

	extractor.setConfigFile(extconfigfile);
	extractor.setExternalPrefix(extprefix);


	extractor.setDefMTop(defmtop);
	extractor.setUseNormalized(normalize);

	if(onlysignalstat){
		extractor.setIgnoreBGStat(true);
		extractor.setIgnoreDataStat(true);
	}

	//mtExtractor::debug=true;
	// histoContent::debug=true;

	// parameterExtractor::debug=true;
	// container1D::debug=true;

	if(mode=="fitintersect")
		extractor.getExtractor()->setLikelihoodMode(parameterExtractor::lh_fitintersect);
	else if(mode=="chi2")
		extractor.getExtractor()->setLikelihoodMode(parameterExtractor::lh_chi2);
	else if(mode=="fit")
		extractor.getExtractor()->setLikelihoodMode(parameterExtractor::lh_fit);



	extractor.setFitUncertaintyModeData(parameterExtractor::fitunc_statcorrgaus);
	extractor.setFitUncertaintyModeMC(parameterExtractor::fitunc_statcorrgaus);

	extractor.getExtractor()->setFitFunctions(fitmode);
	extractor.getExtractor()->setConfidenceLevelFitInterval(0.68);
	extractor.getExtractor()->setIntersectionGranularity(2500);

	extractor.setPlot(plotname);
	extractor.setExcludeBin(excludebin);
	extractor.setMinBin(minbin);
	extractor.setMaxBin(maxbin);
	extractor.setUseFolding(usefolding);
	extractor.setInputFiles(inputfiles);
	const bool isexternalgen=extractor.getIsExternalGen();
	extractor.setExternalGenInputPDF(usepdf);
	extractor.setRescaleNLOPred(rescaletonnlonnll);

	//  extractor.getExtractor()->setLikelihoodMode(parameterExtractor::lh_chi2);
	extractor.setFitMode(fitmode);

	if(outdirectory.Length()>0)
		outdirectory+="_"+plotname;
	else
		outdirectory=plotname;

	if(isexternalgen){
		std::cout << "is external...."<<std::endl;
		TString newpostfix=extprefix;
		if(newpostfix.EndsWith("_"))
			newpostfix = TString(newpostfix,newpostfix.Length()-1);
		if(newpostfix.Contains("_todk")){
			newpostfix.ReplaceAll("_todk",""); //for consistency with old naming scheme
		}
		outdirectory+="_"+usepdf+"_"+newpostfix;
		if(rescaletonnlonnll)
			outdirectory+="_kfact";
	}
	outdirectory.ReplaceAll("unfold ","");

	outdirectory.ReplaceAll(" ","_");
	if(normalize)
		outdirectory+="_normd";

	if(output.Length()>0)
		output+="_summary.pdf";
	else
		output+="summary.pdf";

	system(("mkdir -p "+outdirectory).Data());

	system(("mkdir -p "+outdirectory).Data());
	output=outdirectory+"/"+output;


	std::vector<float> allmasses,nominals,errups,errdowns;
	TFile * fforgraphs=new TFile(output+"_graphs.root","RECREATE");

	extractor.setBinGraphOutFile(fforgraphs);
	TFile f(output+".root","RECREATE");

	TCanvas*c=new TCanvas();
	c->SetBatch(true);
	c->Print(output+".pdf(");


	//print configuration on first page
	TString configString=extractor.printConfig();
	// configString.ReplaceAll("\n"," \\ ");

	textFormatter formattext;
	formattext.setDelimiter("\n");
	formattext.setComment("");
	formattext.setTrim(""); //only get lines

	std::vector<std::string> confstrings=formattext.getFormatted(configString.Data());


	TPavesText * configurationtext= new TPavesText(0.1,0.1,0.9,0.9);//;extractor.printConfig();
	for(size_t i=0;i<confstrings.size();i++){
		configurationtext->AddText(confstrings.at(i).data());
	}

	configurationtext->Draw();
	c->SetName("config");
	WriteAndPrint(c,f,output,outdirectory);
	c->Clear();
	delete configurationtext;

	if(!onlyCP){
		extractor.setup();

		///for pseudodata
		if(createpseudoexp>0){
			std::cout << " creating toys. first fit real MC " <<std::endl;
			extractor.createBinLikelihoods(-1,false);
			std::cout << "entered creation of toys mode\nthis will create toys and pulls and will then exit" <<std::endl;

			makePseudoExperiments(&extractor,(size_t)createpseudoexp,defmtop,output+"pulls");
			return 1;
		}


		//for MC
		extractor.drawXsecDependence(c,false);
		c->SetName("dependence_theo");
		WriteAndPrint(c,f,output,outdirectory);
		extractor.cleanMem();
		c->Clear();
		//for data
		extractor.drawXsecDependence(c,true);
		c->SetName("dependence_data");
		WriteAndPrint(c,f,output,outdirectory);
		extractor.cleanMem();
		c->Clear();

		extractor.drawIndivBins(c);
		c->SetName("indivibins_allsys");
		WriteAndPrint(c,f,output,outdirectory);
		extractor.cleanMem();
		c->Clear();

		float chi2nommin=0;

		//for all systematics
		for(int i=-1;i<extractor.getDataSystSize();i++){

			//dirty hack
			if(i>-1 && extractor.getIsExternalGen()
					&& (extractor.getSystName((size_t)i).BeginsWith("CR")
							|| extractor.getSystName((size_t)i).BeginsWith("UE"))){
				std::cout << "!!!!!!!!!!!skipping CR or UE syst: "<< extractor.getSystName((size_t)i) << std::endl;
				continue;

			}

			extractor.createBinLikelihoods(i,false);

			if(i>=0 && nosyst) break;

			TString systname="_nominal";
			if(i>-1)
				systname="_"+extractor.getSystName(i);


			extractor.drawBinsPlusFits(c,i);
			c->SetName("bins_plus_fits"+systname);
			WriteAndPrint(c,f,output,outdirectory);
			c->Clear();

			extractor.drawBinLikelihoods(c);
			c->SetName("bins_chi2"+systname);
			WriteAndPrint(c,f,output,outdirectory);
			c->Clear();
			//}
			extractor.cleanMem();
			extractor.createGlobalLikelihood();
			extractor.fitGlobalLikelihood();
			if(i<0)
				chi2nommin=extractor.drawGlobalLikelihood(c);
			else
				extractor.drawGlobalLikelihood(c);
			c->SetName("global_chi2"+systname);
			WriteAndPrint(c,f,output,outdirectory);
			extractor.cleanMem();
			c->Clear();

		}

		//close


		//  extractor.drawIndivBins(c);
		//  extractor.overlayBinFittedFunctions(c);


		float nom,errup,errdown,totup,totdown;

		c->Clear();
		extractor.drawResultGraph(c,&nom,&errdown,&errup,&totdown,&totup);


		TLatex * tb= new TLatex(0.15, 0.05, "#chi^{2}_{min,nom}/N_{dof}="+toTString(chi2nommin)+ "/" + toTString(extractor.getNDof())
				+ "=" +toTString(chi2nommin / extractor.getNDof()));
		tb->SetTextSize(0.04);
		tb->SetNDC(true);
		tb->Draw("same");

		c->SetName("results");
		WriteAndPrint(c,f,output,outdirectory);


		std::vector<TString> sysvars = extractor.getSystNameList();
		for(size_t i=0;i<sysvars.size();i++){
			c->SetName("variation_"+sysvars.at(i));
			extractor.drawSystVariation(c,sysvars.at(i));
			WriteAndPrint(c,f,output,outdirectory);
		}


		//results table
		resultsSummary::debug=true;
		systAdder::debug=true;
		texTabler table=extractor.makeSystBreakdown();
		table.writeToFile(outdirectory+"/sys_breakdown.tex");

		table=extractor.makeSystBreakdown(false,false,0.00001);
		table.writeToFile(outdirectory+"/sys_breakdown_detailed.tex");

		table=extractor.makeSystBreakdown(false,false,0.00001,false,true);
		table.writeToFile(outdirectory+"/sys_breakdown_stat.tex");



		c->SetName("spread_inlay");
		extractor.drawSpreadWithInlay(c);
		WriteAndPrint(c,f,output,outdirectory);


		extractor.reset(); //resets for next iteration, keeps config and input files, just deleted temp stuff


		std::cout<< /*std::setprecision(2) << */"\n\nextracted nominal mass: " << nom << " +" << errup << " -" <<errdown << " (stat)  "
				<<  " +" << totup << " -" << totdown<< " (total)\n\n"<<std::endl;

		std::cout << "[Mtop feedback]" << std::endl;
		std::cout << "__MTOP__ = " << nom <<std::endl;
		std::cout << "__MStatUp__ = " << errup <<std::endl;
		std::cout << "__MStatDown__ = " << errdown <<std::endl;
		std::cout << "[end Mtop feedback]" << std::endl;

		c->Clear();
		c->cd();

		TString texttoadd="Extracted mass: +" + toTString(nom) + " +" + toTString(errup) + " -" + toTString(errdown) + " (stat) +" + toTString(totup) +" -" + toTString(totdown) +" (total)";
		ofstream outs;
		outs.open((outdirectory+"/resulttext.txt").Data());
		outs << texttoadd;
		outs.close();



	}



	/////////////////////////////////additional control plots./////////////////////////////////
	///print selected plots here or in extractor??
	fileReader specialcplots;
	specialcplots.setComment("$");
	specialcplots.setStartMarker("[additional plots]");
	specialcplots.setEndMarker("[end additional plots]");
	specialcplots.setDelimiter(",");
	specialcplots.readFile(extconfigfile);

	//get plot names
	std::vector<std::string> plotnames= specialcplots.getMarkerValues("plot");



	TString defmtfile=extractor.getDefMtopDataFile();
	std::cout << "loading additional plots from " << defmtfile<<std::endl;
	containerStackVector defmtvec;
	defmtvec.loadFromTFile(defmtfile);
	gStyle->SetOptStat(0);
	if(onlyCP){
		for(size_t i=0;i<plotnames.size();i++){
			std::cout << plotnames.at(i)<<std::endl;;
		}
		system(((TString)"mkdir -p controlPlots").Data());

		for(size_t i=0;i<plotnames.size();i++){
			fileReader extraconfig;
			extraconfig.setComment("$");
			extraconfig.setDelimiter(",");
			extraconfig.setStartMarker("[plot - " + plotnames.at(i) +"]");
			if(i<plotnames.size()-1)
				extraconfig.setEndMarker("[plot - "+ plotnames.at(i+1) +"]");
			else
				extraconfig.setEndMarker("[end additional plots]");
			extraconfig.readFile(extconfigfile);
			std::string tmpfile=extraconfig.dumpFormattedToTmp();
			containerStack stack2=defmtvec.getStack(plotnames.at(i));


			TString name=stack2.getName();
			///////definitions for backgrounds to be merged:
			std::vector<TString> dyvv; dyvv << "Z#rightarrowll" << "DY#rightarrow#tau#tau" << "DY#rightarrowll" << "VV";
			std::vector<TString> signals; signals << "t#bar{t}" << "t#bar{t}(#tau)";
			std::vector<TString> fakes; fakes << "QCD" << "t#bar{t}bg";


			stack2.mergeLegends(dyvv,"DY/VV",868);
			stack2.mergeLegends(signals,"t#bar{t} signal",633,true);
			stack2.mergeLegends(fakes,"non-W/Z",400);



			std::cout << "plotting: " <<name <<std::endl;
			name.ReplaceAll(" ","_");
			TCanvas cv(name);
			if(stack2.is1D() || stack2.is1DUnfold()){
				plotterControlPlot pl;
				//textBoxes::debug=true;
				pl.setTextBoxNDC(true);
				pl.readStyleFromFileInCMSSW("/src/TtZAnalysis/Tools/styles/controlPlots_standard.txt");
				pl.addStyleFromFile(tmpfile);
				pl.readTextBoxesInCMSSW("/src/TtZAnalysis/Analysis/configs/general/CMS_boxes.txt","CMSSplit03Left");
				pl.setStack(&stack2);
				pl.usePad(&cv);
				pl.draw();
				WriteAndPrint(&cv,f,output,"controlPlots/");
			}
			else if(stack2.is2D()){//TBI

			}
			std::cout << "done plotting " << name << "   " << tmpfile << std::endl;
			TString syscall="rm -f "+tmpfile;
			system(syscall.Data());


		}


		//make yield table
		std::cout << "making yields table" <<std::endl;

		containerStack stack=defmtvec.getStack("total step 8");
		texTabler yieldtable("l | rcr");
		yieldtable << "Process" << "\\multicolumn{3}{c}{Events} ";
		yieldtable.newLine();
		yieldtable << texLine(1);
		std::map<TString,histoBin> yields= stack.getAllContentsInBin(1,-1,false);
		for(std::map<TString,histoBin>::iterator contrib=yields.begin();contrib!=yields.end();++contrib){
			if(contrib->first == "total MC" || contrib->first == "data"){
				continue;
			}

			formatter fmt;
			TString entry=contrib->first;
			entry.ReplaceAll("#","\\");
			entry.ReplaceAll(" ","\\ ");
			entry.ReplaceAll("rightarrow","rightarrow ");
			float value=fmt.round(contrib->second.getContent(),1);
			TString valuestr=fmt.toTString(value);
			float stat=fmt.round(contrib->second.getStat(),1);

			yieldtable << "$"+entry+"$"<<  valuestr << "$\\pm$" << fmt.toTString(stat);
		}

		for(std::map<TString,histoBin>::iterator contrib=yields.begin();contrib!=yields.end();++contrib){
			if(contrib->first == "total MC" || contrib->first == "data"){
				yieldtable << texLine(1);
				formatter fmt;
				TString entry=contrib->first;
				entry.ReplaceAll("#","\\");
				entry.ReplaceAll(" ","\\ ");
				entry.ReplaceAll("rightarrow","rightarrow ");

				float value=fmt.round(contrib->second.getContent(),1);
				TString valuestr=fmt.toTString(value);
				float stat=fmt.round(contrib->second.getStat(),1);
				if(contrib->first != "data")
					yieldtable << "$"+entry+"$"<<  valuestr << "$\\pm$" << fmt.toTString(stat);
				else
					yieldtable << "$"+entry+"$"<< valuestr << " " << " ";
			}
		}

		//std::cout << yieldtable.getTable() << std::endl;
		yieldtable.writeToFile("controlPlots/yields.tex");


	}


	std::cout << "making response matrix" <<std::endl;

	///make response matrices
	containerStack stack=defmtvec.getStack(plotname);
	const container1DUnfold signcuf=stack.produceUnfoldingContainer();
	container2D mresp=signcuf.getNormResponseMatrix();



	//fast print
	c->Clear();
	c->SetLogy(0);
	c->SetName("response_matrix");
	c->SetBottomMargin(0.15);
	c->SetLeftMargin(0.15);
	c->SetRightMargin(0.2);
	TString xaxisnamenew= mresp.getXAxisName();
	xaxisnamenew.ReplaceAll("reco","gen");
	mresp.setXAxisName(xaxisnamenew);
	mresp=mresp.cutRightX(200);
	TH2D * mrespth2d=mresp.getTH2D("",false,true);
	c->cd();
	////
	const Int_t NRGBs = 5;
	const Int_t NCont = 255;

	Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
	Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
	Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
	Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
	TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
	gStyle->SetNumberContours(NCont);
	////
	mrespth2d->GetZaxis()->SetRangeUser(0,0.11);
	mrespth2d->GetZaxis()->SetTitle("");
	mrespth2d->GetZaxis()->SetTitleSize(0.06);
	mrespth2d->GetZaxis()->SetLabelSize(0.05);
	mrespth2d->GetYaxis()->SetTitleSize(0.06);
	mrespth2d->GetYaxis()->SetLabelSize(0.05);
	mrespth2d->GetXaxis()->SetTitleSize(0.06);
	mrespth2d->GetXaxis()->SetLabelSize(0.05);
	mrespth2d->Draw("colz");
	textBoxes tb2d;
	tb2d.readFromFileInCMSSW("/src/TtZAnalysis/Analysis/configs/general/CMS_boxes.txt","CMSnoSplitLeft2DSim");
	for(size_t i=0;i<tb2d.size();i++)
			tb2d.at(i).setColor(0);
	tb2d.drawToPad(c);


	WriteAndPrint(c,f,output,outdirectory);
	delete mrespth2d;


	////efficiency

	container1D efficiency =signcuf.getEfficiency();// mresp.projectToX(false);
	efficiency=efficiency.cutRight(200);
	container1D ineff=mresp.projectToX(true)-efficiency;
	efficiency.removeAllSystematics();
	efficiency.transformToEfficiency();
	//TString tmp=efficiency.getName();
	//efficiency.setXAxisName(tmp.ReplaceAll("reco","gen"));
	ineff.setAllErrorsZero();
	plotterMultiplePlots effplotter;
	effplotter.readStyleFromFileInCMSSW("src/TtZAnalysis/Analysis/configs/topmass/multiplePlots_efficiency.txt");
	c->SetName("efficiency");
	effplotter.usePad(c);
	effplotter.addPlot(&efficiency,false);
	// effplotter.addPlot(&ineff,false);
	effplotter.draw();
	WriteAndPrint(c,f,output,outdirectory);
	effplotter.cleanMem();
	//////////////////////////////////////////////////////////////////
	//end print
	//////////////////////////////////////////////////////////////////

	c->Clear();
	c->Print(output+".pdf)");
	f.Close();
	delete c;
	fforgraphs->Close();
	delete fforgraphs;
	textFormatter tf;
	if(makepdfs){
		//lets see..
#pragma omp parallel for
		for(size_t i=0;i<filelist.size();i++){
			TString syscall="epstopdf --outfile="+filelist.at(i)+".pdf "+ filelist.at(i) +".eps";
			system(syscall.Data());
			syscall="rm "+filelist.at(i) +".eps";
			system(syscall.Data());
		}
	}
	return 0;
}
