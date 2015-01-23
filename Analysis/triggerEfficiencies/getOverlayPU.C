#include "TFile.h"
#include "TH1D.h"
#include "TGraphAsymmErrors.h"
#include "TCanvas.h"



void getOverlayPU(){
	 TFile * f = new TFile("inclCorrDYHS_looseMu_trigger_2013-09-06_15:24__GI/RunCOR/raw.root","READ");
	 TFile * f2 = new TFile("fixedAllLowMCStat_looseMu_trigger_2013-09-06_14:30__GI/RunCOR/raw.root","READ");
	 TH1D *hmc=(TH1D*) f->Get("vertex_multi_den_mc");
	 TH1D *hmc2=(TH1D*) f2->Get("vertex_multi_den_mc");
	 TH1D *hd=(TH1D*) f->Get("vertex_multi_den");

	 TH1 * hmcn=((TH1 *)hmc->Clone())->DrawNormalized();
	 TGraphAsymmErrors * g= new TGraphAsymmErrors(hmcn);
	 TH1 * hmcn2=((TH1 *)hmc2->Clone())->DrawNormalized();
	 TGraphAsymmErrors * g2= new TGraphAsymmErrors(hmcn2);

	 TH1 * hdn=((TH1 *)hd->Clone())->DrawNormalized();
	 hdn->SetMarkerColor(kBlack);
	 hdn->SetLineColor(kBlack);
	 g->SetLineColor(kBlack);
	 g->SetMarkerColor(kBlue);
	 g->SetFillStyle(3005);
	 g->SetFillColor(kBlue);
	 g2->SetLineColor(kBlack);
	 g2->SetMarkerColor(kRed);
	 g2->SetFillStyle(3005);
	 g2->SetFillColor(kRed);

	 TCanvas * c = new TCanvas();

	 hdn->Draw("e1");
	 g->Draw("2,same");
	 g->Draw("PX,same");
	 g2->Draw("2,same");
	 g2->Draw("PX,same");
	 hdn->Draw("same,e1");






}
