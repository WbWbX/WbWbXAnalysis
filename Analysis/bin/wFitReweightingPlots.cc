/*
 * wFitReweightingPlots.cc
 *
 *  Created on: 21 Jun 2016
 *      Author: jkiesele
 */




#include "WbWbXAnalysis/Tools/interface/applicationMainMacro.h"
#include "WbWbXAnalysis/Tools/interface/histoStackVector.h"
#include "WbWbXAnalysis/Tools/interface/plotterMultiplePlots.h"
#include "WbWbXAnalysis/Tools/interface/textFormatter.h"
#include "math.h"
#include "TF2.h"
#include "TH2D.h"
#include <fstream>
#include "TFitResult.h"
#include "TColor.h"
#include "TStyle.h"
#include "WbWbXAnalysis/Analysis/interface/wNLOReweighter.h"

invokeApplication(){
	using namespace ztop;

	const TString outpath=parser->getOpt<TString>("o","fitout","output path")+"/";
	system(("mkdir -p "+outpath).Data());

	const bool noplots=parser->getOpt<bool>("p",false);
	std::vector<std::string> infiles=parser->getRest<std::string>();
	if(infiles.size()<1)
		parser->coutHelp();

	std::string infile=infiles.at(0);

	histoStackVector *hsv=new histoStackVector();
	hsv->readFromFile(infile);

	const TString prefix="rewhist_";
	const TString  prefix1d="1D_rewhist_";
	//hsv.listStacks();
	std::vector<TString> stacknames=hsv->getStackNames(false);
	std::vector<histo2D> selectedhistos;
	const Int_t npar = wNLOReweighter::wdxsec_npars;
	Double_t f2params[wNLOReweighter::wdxsec_npars];
	Double_t fphiparams[wNLOReweighter::wdxsec1DPhi_npars];
	Double_t fthetaparams[wNLOReweighter::wdxsec1DCosTheta_npars];


	TF2 *f2 = new TF2("f2",wNLOReweighter::wdxsec,-1,1,-M_PI,M_PI, npar);
	TF1 *fphi = new TF1("fphi",wNLOReweighter::wdxsec1DPhi,-M_PI,M_PI, wNLOReweighter::wdxsec1DPhi_npars);
	TF1 *ftheta = new TF1("ftheta",wNLOReweighter::wdxsec1DCosTheta,-1,1, wNLOReweighter::wdxsec1DCosTheta_npars);

	fphi->SetParameters(fphiparams);
	ftheta->SetParameters(fthetaparams);

	bool has1Dplots=false;
	for(size_t i=0;i<stacknames.size();i++){
		if(stacknames.at(i).BeginsWith(prefix1d)){ has1Dplots=true;break;}
	}


	TFile rootfile(outpath+"/rewhistos.root","RECREATE");

	std::ofstream outfile((outpath+"data.dat").Data());
	outfile <<  "$$$detamin, detamax, ptmin, ptmax, dummy, dummy, F_0, ..., F_7, F: A_i=F_i/F \n";
	for(size_t i=0;i<stacknames.size();i++){
		if(stacknames.at(i).BeginsWith(prefix)){
			selectedhistos.push_back(hsv->getStack(stacknames.at(i)).getSignalContainer2D());
			histo2D & selectedhisto=selectedhistos.at(selectedhistos.size()-1);
			std::cout << stacknames.at(i) << std::endl;
			TH2D* th=selectedhisto.getTH2D("",false,true);
			std::cout << "project..." <<std::endl;
			f2params[8]=selectedhisto.projectToX(true).integral(true);
			f2->SetParameters(f2params);
			//f2->SetParLimits(8,0,1e8); //fix to positive
			TFitResultPtr  fitr=th->Fit(f2,"S");
			//format for output
			textFormatter tf;
			TString startwith=stacknames.at(i);
			startwith.ReplaceAll(prefix,"");
			//get ranges
			tf.setDelimiter("_");
			tf.setComment("");
			std::vector<std::string> ranges=tf.getFormatted(startwith.Data());
			for(size_t range=0;range<ranges.size();range++){
				tf.setDelimiter(":");
				std::vector<std::string> minmax=tf.getFormatted(ranges.at(range));
				if(minmax.size()==2){
					outfile<< textFormatter::fixLength(minmax.at(0),10)  << textFormatter::fixLength(minmax.at(1),10) ;
				}
			}

			for(size_t j=0;j<npar;j++)
				outfile << " "<<textFormatter::fixLength(toString(f2->GetParameter(j)),10);
			if(has1Dplots)
				outfile << " "<< textFormatter::makeCompatibleFileName(("1D_"+stacknames.at(i)).Data()) ;
			outfile << "\n";

			TCanvas cv(stacknames.at(i));
			cv.SetBottomMargin(0.15);
			cv.SetLeftMargin(0.15);
			cv.SetRightMargin(0.2);

			gStyle->SetOptStat(0);
			Int_t NCont = 255;
			const int NRGBs2=2;
			Double_t stops[NRGBs2] = { 0.00, 1 };
			Double_t red[NRGBs2]   = { 1, 0 };
			Double_t green[NRGBs2] = { 1, 0 };
			Double_t blue[NRGBs2]  = { 1, 0 };

			TColor::CreateGradientColorTable(NRGBs2, stops, red, green, blue, NCont);
			gStyle->SetNumberContours(NCont);
			th->Draw("colz");

			NCont=20;
			const Int_t NRGBs = 5;
			Double_t bstops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
			Double_t bred[NRGBs]   = { 1, 0.00, 0.87, 1.00, 0.51 };
			Double_t bgreen[NRGBs] = { 1, 0.81, 1.00, 0.20, 0.00 };
			Double_t bblue[NRGBs]  = { 1, 1.00, 0.12, 0.00, 0.00 };

			TColor::CreateGradientColorTable(NRGBs, bstops, bred, bgreen, bblue, NCont);
			gStyle->SetNumberContours(NCont);

			if(!noplots){
				f2->Draw("cont1 same");
				cv.Print(outpath+stacknames.at(i)+".pdf");
			}

			//make pull
			histo2D h2d=selectedhisto;
			h2d.setZAxisName("pull");
			histo1D pulls(histo1D::createBinning(9,-10,10),"pull","nBins");
			for(size_t xi=1;xi<h2d.getBinsX().size()-1;xi++){
				for(size_t j=1;j<h2d.getBinsY().size()-1;j++){
					float content=selectedhisto.getBinContent(xi,j);
					float err=selectedhisto.getBinError(xi,j,true); //only stat
					float centrex=0,centrey=0;
					selectedhisto.getBinCenter(xi,j,centrex,centrey);
					float func=f2->Eval(centrex, centrey);
					float pull= (func-content)/err;
					pulls.fill(pull);
					h2d.getBin(xi,j).setContent(pull);
				}
			}


			Double_t bbstops[NRGBs] = { 0.00, 0.34, 0.5, 0.84, 1.00 };
			Double_t bbred[NRGBs]   = { 0.00, 0.00, 1., 1.00, 0.51 };
			Double_t bbgreen[NRGBs] = { 0.00, 0.81, 1., 0.20, 0.00 };
			Double_t bbblue[NRGBs]  = { 0.51, 1.00, 1., 0.00, 0.00 };

			TColor::CreateGradientColorTable(NRGBs, bbstops, bbred, bbgreen, bbblue, NCont);
			gStyle->SetNumberContours(NCont);
			th=h2d.getTH2D("",false,true);
			th->GetZaxis()->SetRangeUser(-10,10);
			th->Draw("colz");
			formatter fmt;
			double chi2=fitr->Chi2();
			TString chi2s=toTString(fmt.round( chi2,0.1));
			TLatex* lat=new TLatex(0.15,0.03,"#chi^{2}="+chi2s);
			lat->SetNDC(true);
			lat->Draw("same");
			chi2s=toTString(fmt.round( chi2/((h2d.getBinsY().size()-2)*(h2d.getBinsX().size()-2)),0.1));
			lat=new TLatex(0.3,0.03,"#chi^{2}/nbins="+chi2s);
			lat->SetNDC(true);
			lat->Draw("same");
			th->GetZaxis()->SetRangeUser(-10,10);
			gPad->RedrawAxis();
			if(!noplots)
				cv.Print(outpath+stacknames.at(i)+"_pull.pdf");

			plotterMultiplePlots pl;
			pl.addPlot(&pulls,false);
			pl.setLastNoLegend();
			if(!noplots)
				pl.printToPdf((outpath+stacknames.at(i)+"_pulls").Data());

			std::cout << "\nfitted:"<<std::endl;
			for(size_t par=0;par<npar;par++){
				std::cout << "A"+toTString(par)+": " << f2->GetParameter(par)/f2->GetParameter(npar-1)<<
						std::endl;
			}

			//1D projection
			histo2D h2d2=selectedhisto;
			histo1D thetadep=h2d2.projectToX(false);
			thetadep.setYAxisName("Events/binwidth");
			histo1D phidep=h2d2.projectToY(false);
			phidep.setYAxisName("Events/binwidth");

			cv.Clear();
			TH1D * h = thetadep.getTH1D("",true,true);
			h->Fit(ftheta);
			if(!noplots){
				h->Draw();
				ftheta->Draw("same");
				cv.Print((outpath+stacknames.at(i)+"_costheta.pdf"));

				cv.Clear();
				h = phidep.getTH1D("",true,true);
				h->Fit(fphi);
				h->Draw();
				fphi->Draw("same");
				cv.Print((outpath+stacknames.at(i)+"_phi.pdf"));
				cv.Clear();
			}
		}
		else if(stacknames.at(i).BeginsWith(prefix1d)){
			histo1D hist=hsv->getStack(stacknames.at(i)).getSignalContainer();
			hist.removeAllSystematics();
			histo1D symmetric=hist;
			for(size_t hbin=0;hbin<hist.getBins().size();hbin++){

				size_t rbin=hist.getBins().size()-1-hbin;
				if(rbin<hbin){break;}
				float cleft=hist.getBinContent(hbin);
				float right=hist.getBinContent(rbin);
				float cont=(cleft+right)/2;
				symmetric.setBinContent(hbin,cont);
				symmetric.setBinContent(rbin,cont);
				if(rbin==hbin){break;}
			}
			histo1D scalefactorhist=symmetric/hist;
			TH1D * thist=scalefactorhist.getTH1D(stacknames.at(i),false);
			thist->Write();
			scalefactorhist=hist/symmetric;
			thist=scalefactorhist.getTH1D(stacknames.at(i)+"_inv",false);
			thist->Write();
		}
	}
	outfile.close();

	rootfile.Close();

	delete hsv;

	std::cout << "fit done, testing read-back" <<std::endl;

	//test read back

	wNLOReweighter tr;
	tr.readParameterFile((outpath+"data.dat").Data());//test
	tr.makeTestPlots((outpath+"/").Data());


	//make a plot with the variation of all parameters
	TCanvas cv2;
	double onlyA7[]={0,0,0,0,0,0,0,1,0};
	f2->SetParameters(onlyA7);
	if(selectedhistos.size()>0){
		selectedhistos.at(0).getTH2D()->Draw("AXIS");
		f2->Draw("cont1 same");
		cv2.Print(outpath+"A7Var.pdf");
	}
	delete f2;

	return 0;
}
