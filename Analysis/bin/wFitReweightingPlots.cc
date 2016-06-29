/*
 * wFitReweightingPlots.cc
 *
 *  Created on: 21 Jun 2016
 *      Author: jkiesele
 */




#include "TtZAnalysis/Tools/interface/applicationMainMacro.h"
#include "TtZAnalysis/Tools/interface/histoStackVector.h"
#include "TtZAnalysis/Tools/interface/textFormatter.h"
#include "math.h"
#include "TF2.h"
#include "TH2D.h"
#include <fstream>
#include "TFitResult.h"
#include "TColor.h"
#include "TStyle.h"
#include "TtZAnalysis/Analysis/interface/wNLOReweighter.h"

invokeApplication(){
	using namespace ztop;

	const TString outpath=parser->getOpt<TString>("o","fitout","output path")+"/";
	system(("mkdir -p "+outpath).Data());

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
	TF2 *f2 = new TF2("f2",wNLOReweighter::wdxsec,-1,1,-M_PI,M_PI, npar);

	TFile rootfile(outpath+"/rewhistos.root","RECREATE");

	std::ofstream outfile((outpath+"data.dat").Data());
	outfile <<  "$$$detamin, detamax, ptmin, ptmax, dummy, dummy, F_0, ..., F_7, F: A_i=F_i/F \n";
	for(size_t i=0;i<stacknames.size();i++){
		if(stacknames.at(i).BeginsWith(prefix)){
			selectedhistos.push_back(hsv->getStack(stacknames.at(i)).getSignalContainer2D());
			std::cout << stacknames.at(i) << std::endl;
			TH2D* th=selectedhistos.at(i).getTH2D("",false,true);
			f2params[8]=selectedhistos.at(i).projectToX(true).integral(true);
			f2->SetParameters(f2params);
			//f2->SetParLimits(8,0,1e8); //fix to positive
			th->Fit(f2);
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
					outfile<< minmax.at(0) << " " << minmax.at(1) << " ";
				}
			}

			for(size_t j=0;j<npar;j++)
				outfile << " "<<f2->GetParameter(j);
			outfile << " "<< textFormatter::makeCompatibleFileName(("1D_"+stacknames.at(i)).Data()) ;
			outfile << "\n";

			TCanvas cv(stacknames.at(i));

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

			NCont=40;
			const Int_t NRGBs = 5;
			Double_t bstops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
			Double_t bred[NRGBs]   = { 1, 0.00, 0.87, 1.00, 0.51 };
			Double_t bgreen[NRGBs] = { 1, 0.81, 1.00, 0.20, 0.00 };
			Double_t bblue[NRGBs]  = { 1, 1.00, 0.12, 0.00, 0.00 };

			TColor::CreateGradientColorTable(NRGBs, bstops, bred, bgreen, bblue, NCont);
			gStyle->SetNumberContours(NCont);

			f2->Draw("cont1 same");
			cv.Print(outpath+stacknames.at(i)+".pdf");

			std::cout << "\nfitted:"<<std::endl;
			for(size_t par=0;par<npar-1;par++){
				std::cout << "A"+toTString(par)+": " << f2->GetParameter(par)/f2->GetParameter(npar-1)
																								<<std::endl;
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

	//test read back

	wNLOReweighter tr;
	tr.readParameterFile((outpath+"data.dat").Data());//test



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
