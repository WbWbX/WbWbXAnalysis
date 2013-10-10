/*
 * containerUnfoldTest.C
 *
 *  Created on: Aug 19, 2013
 *      Author: kiesej
 */


#include "TtZAnalysis/Tools/interface/container1DUnfold.h"
#include "TtZAnalysis/Tools/interface/containerUnfolder.h"
#include "TtZAnalysis/Tools/interface/containerStack.h"
#include "TRandom.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"

void divideByBinWidth(TH1* hu){
	for(int i=0;i<hu->GetNbinsX();i++){
		hu->SetBinContent(i, hu->GetBinContent(i)/hu->GetBinWidth(i));
		hu->SetBinError(i, hu->GetBinError(i)/hu->GetBinWidth(i));
	}
}

std::vector<float> subdivide(const std::vector<float> & bins, size_t div){
	std::vector<float> out;
	for(size_t i=0;i<bins.size()-1;i++){
		float width=bins.at(i+1)-bins.at(i);
		if((int)i<(int)bins.size()-2){
			for(size_t j=0;j<div;j++)
				out.push_back(bins.at(i)+j*width/div);
		}
		else{
			for(size_t j=0;j<=div;j++)
				out.push_back(bins.at(i)+j*width/div);
		}
	}
	return out;
}
void drawFullTG(const ztop::container1D * c,int color, int markerstyle,TString opt){
	TH1D *h =c->getTH1D("",true,true);
	h->SetLineColor(color);
	h->SetMarkerColor(color);
	h->SetMarkerStyle(markerstyle);
	TGraphAsymmErrors *g=c->getTGraph("",true,false);
	g->SetLineColor(color);
	g->SetMarkerColor(color);
	g->SetMarkerStyle(markerstyle);
	h->Draw("e1"+opt);
	g->Draw("P,same"+opt);
}

TCanvas * makeFullCheckCanvas(const ztop::container1DUnfold& cont){
	TCanvas *c = new TCanvas();
	//gen

	drawFullTG(&cont.getGenContainer(),kRed,23,"");
	drawFullTG(&cont.getRecoContainer(),kBlack,20,"same");
	drawFullTG(&cont.getUnfolded(),kBlue,23,"same");
	drawFullTG(&cont.getRefolded(),kGreen,34,"same");

	return c;
}


void generateAndFill(ztop::container1DUnfold * c,double syst,float nentries){
	double detRes=0.1;
	double detOff=0.1;
	double effCorr=0.2;
	TRandom * r=new TRandom(123123*nentries); //mc and data uncorrelated

	for(float entry=0;entry<nentries;entry++){

		double gen=r->BreitWigner(10,4);
		double reco=gen* (1 + r->Gaus(detOff,detRes)/gen); //some resolution and offset effect
		double selcrit=r->Gaus(1,3)-gen*effCorr;
		double weight=1;
		c->fillGen(gen,weight);
		if(selcrit<0) continue;
		reco=reco+syst;
		c->fillReco(reco,weight);
	}

	delete r;
}

void test(){

	using namespace std;
	using namespace ztop;

	bool isMC=true;

	vector<float> genbins,recobins;

	genbins << 0 << 3 << 9 << 11 << 13 << 15 << 18 << 28;
	recobins << 0 << 2 << 4 << 5 << 5.5 << 6 << 6.5 << 7 << 7.5 << 8 << 10 << 11 << 12 << 13 << 14 << 16 << 17 << 18 << 19 << 20 << 21 << 23 << 25 << 28;

	recobins=subdivide(genbins,5);


	recobins=subdivide(genbins,10);

	container1DUnfold::c_makelist=true;
	container1DUnfold::debug = true;

	container1DUnfold tc(genbins,recobins,"nominal","p_{T}","events");
	container1DUnfold tcsup(genbins,recobins,"syst_up","p_{T}","events");
	container1DUnfold tcsdown(genbins,recobins,"syst_down","p_{T}","events");


	tc.setMC(true);
	tcsup.setMC(true);
	tcsdown.setMC(true);

	container1DUnfold tcd=tc;
	tcd.setMC(false);
	tcd.clear();
	container1DUnfold tcdsup=tcsup;
	container1DUnfold tcdsdown=tcsdown;
	tcsup.clear();
	tcsdown.clear();

	float nentries=10e5;
	double norm=0.05;

	//double syst=2.4;

	cout << "generating MC" <<endl;
	generateAndFill(&tc,0,nentries);
	generateAndFill(&tcsup,0.2,nentries);
	generateAndFill(&tcsdown,-0.1,nentries);

	cout << "generating \"data\"" <<endl;
	generateAndFill(&tcd,0,nentries*norm);

	double bgcont=0.9;
	double bgshape=4;
	cout << "generating \"BG\"" <<endl;
	generateAndFill(&tcd,bgshape,nentries*norm*bgcont);
	container1DUnfold tcmcbg=tcd;
	tcmcbg.clear();
	generateAndFill(&tcmcbg,bgshape,nentries*bgcont);

	tcdsup=tcd;
	tcdsdown=tcd;
	ztop::container1DUnfold::flushAllListed();


	cout << "doing norm" <<endl;







	bool systematics=false;
	cout << "adding syst mc" <<endl;
	if(systematics){
		tc.addErrorContainer("Sys_up",tcsup);
		tc.addErrorContainer("Sys_down",tcsdown);
		tc.addGlobalRelError("Lumi",0.05);
		tcmcbg.addGlobalRelError("Lumi",0.05);
		tcmcbg.addGlobalRelError("Sys",0.0);

		cout << "adding syst data" <<endl;

		tcd.addErrorContainer("Sys_up",tcdsup);
		tcd.addErrorContainer("Sys_down",tcdsdown);
		tcd.addGlobalRelError("Lumi",0.); //trick!
	}

	containerStack cstack("stack");
	cstack.push_back(tcd,"data",1,1);
	cstack.setLegendOrder("data",2);
	cstack.push_back(tcmcbg,"BG",kBlue,norm);
	cstack.setLegendOrder("BG",1);
	cstack.push_back(tc,"signal",kRed,norm);
	cstack.setLegendOrder("signal",0);


	tc=tc*norm; //normalize all MC contributions!
	tcsup=tcsup*norm;
	tcsdown=tcsdown*norm;
	tcmcbg=tcmcbg*norm;


	bool manualdraw=true;
	containerUnfolder unfolder,unfolder2;
	containerUnfolder::debug=true;
	cout << "drawing" <<endl;
	if(manualdraw){
		TGraphAsymmErrors* gmccp=tc.getRecoContainer().getTGraph("mc_reco",true,false,false);

		tc.setRecoContainer(tcd.getRecoContainer());
		tc.setBackground(tcmcbg.getRecoContainer());

		unfolder.unfold(tc);
		makeFullCheckCanvas(tc);

/*
		tc.setBinByBin(false);

		gStyle->SetOptStat(0);

		TCanvas * c = new TCanvas();
		TH2D * h=tc.getTH2D();
		h->Draw("COLZ");
		sleep(1);
		c->Print("2d.pdf");
		TH1D * h1d=tc.getRecoContainer().getTH1D("data",true,true);
		h1d->Draw();
		//sleep(1);
		c->Print("data.pdf");
		TH1D * h1=tc.getGenContainer().getTH1D("gen",true,true);

		h1->Draw();
		//h1->GetYaxis()->SetRangeUser(0,250e3*nentries/1e6);
		//sleep(1);
		c->Print("gen.pdf");
		cout << "do unfolding" << endl;



		unfolder.unfold(tc);



		TH1::SetDefaultSumw2();
		cout << "get output" << endl;
		c->Clear();
		TH1* hu=tc.getUnfolded().getTH1D("unfolded",true,true);
		TGraphAsymmErrors* hus=tc.getUnfolded().getTGraph("unfolded_s",true,false,false);

		hu->SetLineColor(kBlue);
		hus->SetLineColor(kBlue);
		hu->SetMarkerColor(kBlue);
		hu->SetMarkerStyle(23);
		h1->SetMarkerColor(kRed);
		h1->SetLineColor(kRed);
		//h1d->Draw("AXIS");


		TH1* hrf=tc.getRefolded().getTH1D("refolded",true,true);
		TGraphAsymmErrors* hrfs=tc.getRefolded().getTGraph("refolded_s",true,false);

		hrf->SetMarkerStyle(23);
		hrf->SetMarkerColor(kBlue-3);
		hrfs->SetMarkerColor(kBlue);
		hrfs->SetMarkerStyle(23);
		hrfs->SetLineColor(kBlue);
		hrf->SetLineColor(kBlue-3);

		//hu->GetYaxis()->SetRangeUser(0,250e3*nentries/1e6);
		h1d->SetMarkerColor(kBlack);

		gmccp->SetMarkerColor(kGreen);
		gmccp->SetLineColor(kGreen);
		//gmccp->SetFillStyle();
		h1->Draw(""); //mcgen
		TH1D * mcbg=tcmcbg.getRecoContainer().getTH1D("mcbg");
		mcbg->SetMarkerColor(kGray);
		mcbg->Draw("same");
		hus->Draw("same,e1");
		hu->Draw("same,e1");

		hrfs->Draw("same,e1");
		hrf->Draw("same,e1");

		gmccp->Draw("same,e1");//mc reco

		h1d->Draw("same,e1");


		cout << "sleep a bit" << endl;
		//sleep(5);


		c->Print("unfolded.eps");
		TCanvas * c2=new TCanvas();
		c2->cd();

		h1d->Draw("AXIS");
		gmccp->Draw("same,0");

		c2->Print("sysvar.eps");
*/
		/*c->Clear();
	tc.lcurve()->Draw("AL");
	tc.bestLcurve()->SetMarkerColor(kRed);
	tc.bestLcurve()->Draw("*");
	c->Print("bestLCurve.eps");
	tc.logTauX()->Draw();
	tc.bestLogTauLogChi2()->SetMarkerColor(kRed);
	tc.bestLogTauLogChi2()->Draw("*");
	c->Print("bestLogTauLogChi2.eps");*/
	}

	containerStack::debug=true;
	containerStack::batchmode=false;
	std::vector<TString> signals;signals << "signal";
	container1DUnfold cstackUF =cstack.produceUnfoldingContainer(signals);
	unfolder2.unfold(cstackUF);
	makeFullCheckCanvas(cstackUF);
	TCanvas * c=cstack.makeTCanvas(containerStack::ratio);
	c->Draw();
}




void containerUnfoldTest(){
	test();
}
void cut(){
	containerUnfoldTest();
}
