/*
 * checkPU.C
 *
 *  Created on: Sep 6, 2013
 *      Author: kiesej
 */


#include "TFile.h"
#include "TString.h"
#include "TH1D.h"
#include "TGraph.h"
#include <vector>
//#include "boost/filesystem.hpp"
#include <iostream>
#include "TGraphAsymmErrors.h"
#include "TCanvas.h"
#include "TLegend.h"

bool dirExists(TString dirname){/*
	using namespace boost::filesystem;
	if (exists( dirname.Data()))
		return true;
	return false;
	*/
	return true;
}
void makeOverlayPlot(TString dir){
	TFile * f= new TFile(dir+"/raw.root","UPDATE");
	TCanvas *c=new TCanvas();
	c->SetBatch(true);
	TH1D *hd=(TH1D*) f->Get("vertex_multi_den");
	TH1D *hmc=(TH1D*) f->Get("vertex_multi_den_mc");


	TH1 * hmcn=hmc->DrawNormalized();
	TGraphAsymmErrors * g= new TGraphAsymmErrors(hmcn);

	TH1 * hdn=hd->DrawNormalized();
	hdn->SetMarkerColor(kBlack);
	hdn->SetLineColor(kBlack);
	g->SetLineColor(kBlack);
	g->SetMarkerColor(kRed);
	g->SetFillStyle(3005);
	g->SetFillColor(kRed);

	TLegend *leg = new TLegend((Double_t)0.65,(Double_t)0.65,(Double_t)0.95,(Double_t)0.90);
	leg->Clear();
	leg->SetFillStyle(0);
	leg->SetBorderSize(0);
	leg->AddEntry(hdn,"data","ep");
	leg->AddEntry(g,"MC","ep");


	hdn->Draw("e1");
	g->Draw("2,same");
	g->Draw("PX,same");
	hdn->Draw("same,e1");
	leg->Draw("same");
	c->SetName("PU_den_check_"+dir);



	c->Write();
	c->Print("PU_den_check_"+dir+".pdf");
	c->Clear();
	delete c;
	f->Close();
	delete f;
}

void checkPU(int argc, char* argv[]){

	std::vector<TString> dirs;
	for(int i=1;i<argc;i++){
		if(dirExists((TString)argv[i]))
			dirs.push_back((TString)argv[i]);
		else
			std::cout << "dir " << (TString)argv[i] << " does not exist. skipping" << std::endl;
	}

	for(size_t i=0;i<dirs.size();i++){
		makeOverlayPlot(dirs.at(i));
	}

}

//for compiling
int main(int argc, char* argv[]){
	checkPU(argc,argv);
	return 0;
}
