#include "TH1D.h"
#include "TH2D.h"
#include "TtZAnalysis/Tools/interface/effTriple.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TString.h"
#include "TPad.h"
#include "TPaveText.h"
#include <vector>
#include "TtZAnalysis/Tools/interface/histoStyle.h"

#ifndef histoStyle_h
#define histoStyle_h

TH1D * divideByBinWidth(TH1D * h){
	TH1D * out= new TH1D(*h);
	for(int i=1;i<h->GetNbinsX();i++){
		double cont=h->GetBinContent(i)/h->GetBinWidth(i);
		double err=h->GetBinError(i)/h->GetBinWidth(i);
		out->SetBinContent(i,cont);
		out->SetBinError(i,err);
	}
	return out;
}


void setGStyle(){
	gStyle->SetOptTitle(0);
	gStyle->SetOptStat(0);

	gPad->SetLeftMargin(0.15);
	gPad->SetBottomMargin(0.15);

	gStyle->SetPaintTextFormat("5.3f");
}


void setCommon(TH2D &h){
	h.GetYaxis()->SetTitleSize(0.06);
	h.GetXaxis()->SetTitleSize(0.05);
	h.GetYaxis()->SetLabelSize(0.05);
	h.GetXaxis()->SetLabelSize(0.05);
	h.SetMarkerSize(2.0);
}

void setCommon(TH1D & h){
	h.GetYaxis()->SetTitleSize(0.06);
	h.GetXaxis()->SetTitleSize(0.05);
	h.GetYaxis()->SetLabelSize(0.05);
	h.GetXaxis()->SetLabelSize(0.05);
	h.SetMarkerSize(1);
	h.SetLineWidth(2);
}

void applyEfficiencyStyleMC(TH1D & h){
	h.SetAxisRange(0.5,1.1,"Y");
	setCommon(h);
	h.SetMarkerStyle(22);
	h.SetMarkerColor(kRed);
	h.SetLineColor(kRed);
}

void applyEfficiencyStyleData(TH1D & h){
	h.SetAxisRange(0.5,1.1,"Y");
	setCommon(h);
	h.SetMarkerStyle(20);
	h.SetMarkerColor(1);
	h.SetLineColor(1);
}

void applySFStyle(TH1D & h){
	h.SetAxisRange(0.5,1.1,"Y");
	setCommon(h);
	h.SetMarkerStyle(33);
	h.SetMarkerColor(8);
	h.SetLineColor(8);
}


void applyNumStyle(TH1D & h){
	setCommon(h);
	h.SetMarkerStyle(20);
	h.SetMarkerColor(1);
	h.SetLineColor(1);
}
void applyDenStyle(TH1D & h){
	setCommon(h);
	h.SetMarkerStyle(20);
	h.SetMarkerColor(kGreen-1);
	h.SetLineColor(kGreen-1);
}



void applyEfficiencyStyleMC(TH2D & h){
	setCommon(h);
}

void applyEfficiencyStyleData(TH2D & h){
	setCommon(h);
}

void applySFStyle(TH2D & h){
	setCommon(h);
}
void applyNumStyle(TH2D & h){
	setCommon(h);
}
void applyDenStyle(TH2D & h){
	setCommon(h);
}


////


void applyEfficiencyStyleMC(ztop::histWrapper & h){
	if(h.isTH1D())
		applyEfficiencyStyleMC(h.getTH1D());
	else
		applyEfficiencyStyleMC(h.getTH2D());
}

void applyEfficiencyStyleData(ztop::histWrapper & h){
	if(h.isTH1D())
		applyEfficiencyStyleData(h.getTH1D());
	else
		applyEfficiencyStyleData(h.getTH2D());
}

void applySFStyle(ztop::histWrapper & h){
	if(h.isTH1D())
		applySFStyle(h.getTH1D());
	else
		applySFStyle(h.getTH2D());
}

void applyNumStyle(ztop::histWrapper & h){
	if(h.isTH1D())
		applyNumStyle(h.getTH1D());
	else
		applyNumStyle(h.getTH2D());
}

void applyDenStyle(ztop::histWrapper & h){
	if(h.isTH1D())
		applyDenStyle(h.getTH1D());
	else
		applyDenStyle(h.getTH2D());
}

void plotRaw(std::vector<ztop::effTriple> trips, TString add="", TString addlabel="", TString dir = "./"){

	using namespace ztop;
	using namespace std;

	if(dir != "./")
		system(("mkdir -p "+dir).Data());

	TCanvas * c=new TCanvas();
	c->SetBatch();
	TLegend * leg=new TLegend(0.68,0.68,0.90,0.90);
	leg->SetFillStyle(0);
	leg->SetBorderSize(0);

	gStyle->SetOptTitle(0);
	gStyle->SetOptStat(0);

	gPad->SetLeftMargin(0.15);
	gPad->SetBottomMargin(0.15);

	gStyle->SetPaintTextFormat("5.3f");

	TPaveText *label = new TPaveText();

	label -> SetX1NDC(gStyle->GetPadLeftMargin());
	label -> SetY1NDC(1.0-gStyle->GetPadTopMargin());
	label -> SetX2NDC(1.0-gStyle->GetPadRightMargin());
	label -> SetY2NDC(1.0);
	label -> SetTextFont(42);
	//

	label->SetFillStyle(0);
	label->SetBorderSize(0);
	label->SetTextAlign(22);


	textBoxes txs;
	if(addlabel.Contains("8TeV"))
		txs.readFromFileInCMSSW("/src/TtZAnalysis/Analysis/configs/general/noCMS_boxes.txt","CMSnoSplitLeft");
	else if(addlabel.Contains("7TeV"))
		txs.readFromFileInCMSSW("/src/TtZAnalysis/Analysis/configs/general/noCMS_boxes.txt","CMSnoSplitLeft7TeV");
	else
		label -> AddText(Form(addlabel));

	for(size_t i=0;i<trips.size();i++){
		c->Clear();
		leg->Clear();


		histWrapper num=trips.at(i).getNum();
		histWrapper den=trips.at(i).getDen();
		applyNumStyle(num);
		applyDenStyle(den);
		num.setName(num.getName()+add);
		den.setName(den.getName()+add);
		num.write();
		den.write();
		if(den.isTH1D()){
			TH1D * h1=divideByBinWidth(&(num.getTH1D()));
			leg->AddEntry(h1,"numerator"  ,"pe");
			if(num.getFormatInfo().Contains("smallmarkers"))
				h1->SetMarkerSize(h1->GetMarkerSize()*0.5);
			h1->Draw("e1");
			TH1D * h=divideByBinWidth(&(den.getTH1D()));
			leg->AddEntry(h,"denominator"  ,"pe");
			h->Draw("e1,same");
			leg->Draw("same");
			//label->Draw("same");


			TString canvasname=num.getName();
			canvasname.ReplaceAll("_num","");
			canvasname.ReplaceAll(add,"");
			c->SetName(canvasname+"_numden"+add);
			c->SetTitle(canvasname+"_numden"+add);
			txs.drawToPad(c);
			c->Write();
			c->Print(dir+canvasname+"_numden"+add+".pdf");
			c->Clear();
			delete h1; delete h;

		}
		else{
			c->Clear();
			TH2D * h=&(num.getTH2D());
			TString canvasname=num.getName();
			if(num.getFormatInfo().Contains("smallmarkers"))
				h->SetMarkerSize(h->GetMarkerSize()*0.5);
			c->SetName(canvasname+"_c");
			c->SetTitle(canvasname+"_c");
			h->Draw("colz,text,e");
			//label->Draw("same");
			c->Write();
			c->Print(dir+canvasname+".pdf");
			txs.drawToPad(c);
			c->Clear();
			h=&(den.getTH2D());
			if(den.getFormatInfo().Contains("smallmarkers"))
				h->SetMarkerSize(h->GetMarkerSize()*0.5);
			canvasname=den.getName();
			c->SetName(canvasname);
			c->SetTitle(canvasname);
			h->Draw("colz,text,e");
			label->Draw("same");
			c->Write();
			c->Print(dir+canvasname+".pdf");

		}
	}
	delete c;
	delete leg;
	delete label;
}


void plotAll(std::vector<ztop::histWrapper>  hvec, TString addlabel="", TString dir = "./"){

	//num,den,eff
	size_t i=0;

	if(dir != "./")
		system(("mkdir -p "+dir).Data());



	TCanvas * c=new TCanvas();
	c->SetBatch();
	TLegend * leg=new TLegend(0.35,0.15,0.70,0.42);
	leg->SetFillStyle(0);
	leg->SetBorderSize(0);


	gStyle->SetOptTitle(0);
	gStyle->SetOptStat(0);

	gPad->SetLeftMargin(0.15);
	gPad->SetBottomMargin(0.15);

	gStyle->SetPaintTextFormat("5.3f");

	TPaveText *label = new TPaveText();

	label -> SetX1NDC(gStyle->GetPadLeftMargin());
	label -> SetY1NDC(1.0-gStyle->GetPadTopMargin());
	label -> SetX2NDC(1.0-gStyle->GetPadRightMargin());
	label -> SetY2NDC(1.0);
	label -> SetTextFont(42);

std::cout << "label:" << addlabel << std::endl;
	ztop::textBoxes txs;
	if(addlabel.Contains("8TeV"))
		txs.readFromFileInCMSSW("/src/TtZAnalysis/Analysis/configs/general/noCMS_boxes.txt","CMSnoSplitLeft");
	else if(addlabel.Contains("7TeV"))
		txs.readFromFileInCMSSW("/src/TtZAnalysis/Analysis/configs/general/noCMS_boxes.txt","CMSnoSplitLeft7TeV");
	else
		label -> AddText(Form(addlabel));



	label->SetFillStyle(0);
	label->SetBorderSize(0);
	label->SetTextAlign(22);
	//  label->Draw("same");

	while(i<hvec.size()){
		c->Clear();
		leg->Clear();


		if(hvec.at(i).isTH1D()){ //do 1D plotting
			TH1D * h=&(hvec.at(i).getTH1D());
			leg->AddEntry(h,"data efficiency"  ,"pe");
			h->GetYaxis()->SetTitle("#epsilon,SF");
                        h->GetYaxis()->SetRangeUser(0,1.1);
			if(hvec.at(i).getFormatInfo().Contains("smallmarkers"))
				h->SetMarkerSize(h->GetMarkerSize()*0.5);
			h->Draw("e1");
			i++;
			h=&(hvec.at(i).getTH1D());
			//leg->AddEntry(h,"MC efficiency"  ,"pe");
			//h->Draw("e1,same");
			i++;
			h=&(hvec.at(i).getTH1D());
			//leg->AddEntry(h,"scale factor"  ,"pe");
			//h->Draw("e1,same");

			TString canvasname=hvec.at(i).getName();
			canvasname.ReplaceAll("_eff","");
			c->SetName(canvasname);
			c->SetTitle(canvasname);

			leg->Draw("same");
			//label->Draw("same");
			txs.drawToPad(c);
			c->Write();
			c->Print(dir+canvasname+".pdf");

			i++; //next
		}
		else{                    //do 2d plotting
			size_t j=i;
			while(j<i+3){
				c->Clear();
				TH2D * h=&(hvec.at(j).getTH2D());
				if(hvec.at(j).getFormatInfo().Contains("smallmarkers"))
					h->SetMarkerSize(h->GetMarkerSize()*0.5);
				TString canvasname=hvec.at(j).getName();
				c->SetName(canvasname);
				if(canvasname.Contains("_mc")){
					label -> Clear();
					label -> AddText(Form(addlabel+" MC"));
				}
				c->SetTitle(canvasname);
				h->Draw("colz,text,e");
				txs.drawToPad(c);
				label->Draw("same");
				c->Write();
				c->Print(dir+canvasname+".pdf");
				label -> Clear();
				//label -> AddText(Form(addlabel));
				j++;
			}
			i+=3; //next
		}

	}

	delete c;
	delete label;
}




#endif
