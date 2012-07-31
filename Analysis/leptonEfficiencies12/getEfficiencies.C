#include <vector>
#include "TTree.h"
#include "TFile.h"
#include <iostream>
#include <algorithm>
#include <utility>
#include "TH2D.h"
#include "TROOT.h"
#include <stdlib.h>
#include "../../plugins/miscUtils.h"


//TString inputfile_="test.root";


double getSignalContribution(TH1D * h, double &syserr, TString someoptions=""){ //options: (CaC: cut and count without fitting; only stat error)


  using namespace std;
  
  using namespace RooFit;
  double nSignalOS;
  
  if(someoptions == "CaC"){
    nSignalOS = h->Integral();
    syserr = sqrt(h->getIntegral);
  }
  else{
    double rangemin=h->GetXaxis()->GetXmin();
    double rangemax=h->GetXaxis()->GetXmax();
    
    RooRealVar mass("mass","mass",rangemin,rangemax);
    mass.setRange("cutcut",rangemin,rangemax); 
    
    RooDataHist roohist("roohist","roohist",mass,Import(*h));
    TString fitname=h->GetName();
    
    RooPlot * frame = mass.frame(Title("Fit " + fitname));
    roohist.plotOn(frame,MarkerColor(1),MarkerSize(0.9),MarkerStyle(21)); 
    roohist.statOn(frame);
    
    //signal parameter
    RooRealVar nsig("nsig","nsig",1000.,0.,10000000.);
    
    // VOIGTIAN FUNCTION
    RooRealVar mean("mean","mean",91.2, 20.0,120.0);
    RooRealVar width("width","width",5.0, 0.0, 100.0);
    RooRealVar sigmaV("sigmaV","sigmaV",5.0, 0.0, 100.0);
    RooVoigtian voigt("voigt","voigt",mass,mean,width,sigmaV);
    
    //======// Parameters for CrystalBall 
    RooRealVar m0("M_{ll}", "Bias", 114., 80, 120,"GeV"); 
    RooRealVar sigma("#sigma_{CB}","Width", 9.2,3.0,10.0);//,"GeV/c^{2}"); 
    RooRealVar cut("#alpha","Cut", 6., 5., 7.);
    RooRealVar power("#gamma","Power", 10., 1., 20.);
    
    RooCBShape CrystalBall("CrystalBall", "A  Crystal Ball Lineshape", mass, m0,sigma, cut, power);
    
    //======//Parameters for Breit-Wigner Distribution
    RooRealVar mRes("M_{ll}", "Z Resonance  Mass", 91.2, 89,94);//,"GeV/c^{2}"); 
    RooRealVar Gamma("#Gamma", "#Gamma", 4.0, 1.0,20.0);//,"GeV/c^{2}"); 
    RooBreitWigner BreitWigner("BreitWigner","A Breit-Wigner Distribution",mass,mRes,Gamma);
    
    // SIGNAL MODEL
    RooFFTConvPdf ResolutionModel("Convolution","Convolution", mass,  BreitWigner, CrystalBall);
    
    
    //BG model
    RooRealVar nbkg("nbkg","nbkg",10.,0.,200000.,"GeV");
    RooRealVar bkg_slope("bkg_slope","slope of the background exponential mass PDF",-0.1,0.1);
    RooExponential bkgModel("bkgModel","background mass PDF",mass,bkg_slope);
    
    //add sig + bg
    RooAddPdf pdfFinal("pdfFinal","pdfFinal",RooArgList(voigt,bkgModel),RooArgList(nsig,nbkg));
    
    RooFitResult *fitResult = pdfFinal.fitTo(roohist, RooFit::Save(true),
					     RooFit::Extended(true), RooFit::PrintLevel(-1));
    
    fitResult->Print();//"v"); //verbose
    pdfFinal.plotOn(frame,LineColor(4));
    pdfFinal.plotOn(frame,Components("bkgModel"),LineColor(kRed),LineStyle(kDashed));
    pdfFinal.paramOn(frame); 
    
    nSignalOS    = nsig.getVal();
    double signError = nsig.getError();
    
    cout << "\n\n\nsignal contribution: " << nSignalOS << " +- " << signError <<  "    background contribution: " << nbkg.getVal() << endl;
    
    
    TCanvas c = TCanvas(fitname+" Zmass",fitname + " Zmass",800,400) ;
    c.cd() ; gPad->SetLeftMargin(0.15);
    frame->Draw();
    c.Write();
    
    syserr=signError;
  }
  
  return nSignalOS;
}


void writeEfficiencyHistos(TString infile_, TString outfile_){
  using namespace std;

  TFile * in = new TFile(infile_);
  TTree *t = (TTree*)in->Get("binning");

   vector<float>   *etabins;
   vector<float>   *ptbins;
   Int_t           netabins;
   Int_t           nptbins;

   TBranch        *b_etabins;   //!
   TBranch        *b_ptbins;   //!
   TBranch        *b_netabins;   //!
   TBranch        *b_nptbins;   //!

   t->SetBranchAddress("etabins", &etabins, &b_etabins);
   t->SetBranchAddress("ptbins", &ptbins, &b_ptbins);
   t->SetBranchAddress("netabins", &netabins, &b_netabins);
   t->SetBranchAddress("nptbins", &nptbins, &b_nptbins);
   t->GetEntry(0);

   const Int_t dimeta=netabins;
   const Int_t dimpt=nptbins;
   double binseta[dimeta];
   double binspt[dimpt];
   int Netabins=netabins-1;
   int Nptbins=nptbins-1;
  
   copy(etabins->begin(), etabins->end(), binseta);
   copy(ptbins->begin(), ptbins->end(), binspt);

   TH2D *h_effdata=new TH2D("efficiency data", "eff data", Netabins, binseta , Nptbins, binspt );
   TH2D *h_effmc=new TH2D("efficiency mc", "eff mc", Netabins, binseta , Nptbins, binspt );
   TH2D *h_corr=new TH2D("correctionfactor", "Ccorr", Netabins, binseta , Nptbins, binspt );
 

   TFile * out = new TFile(outfile_,"RECREATE");

   TString getsignaloptions="CaC";

   for(unsigned int i=0; i<Netabins;++i){
     for(unsigned int j=0;j<Nptbins;++j){
       double syserrprobed=0;
       double syserrpassed=0;
       double probed=getSignalContribution((TH1D*)in->Get(toTString(i) + " " + toTString(j) + " probedData"), syserrprobed,getsignaloptions);
       double passed=getSignalContribution((TH1D*)in->Get(toTString(i) + " " + toTString(j) + " passedData"), syserrpassed,getsignaloptions);

       double eff=passed / probed;
       double efferr=sqrt(eff * (1-eff)/probed + pow(syserrpassed/probed,2) + pow(syserrprobed*eff/probed,2)); 

       h_effdata->SetCellContent(i+1,j+1,eff);
       h_effdata->SetCellError  (i+1,j+1,efferr);

       probed=getSignalContribution((TH1D*)in->Get(toTString(i) + " " + toTString(j) + " probedMC"), syserrprobed,getsignaloptions);
       passed=getSignalContribution((TH1D*)in->Get(toTString(i) + " " + toTString(j) + " passedMC"), syserrpassed,getsignaloptions);

       double effmc=passed / probed;
       double efferrmc=sqrt(eff * (1-eff)/probed + pow(syserrpassed/probed,2) + pow(syserrprobed*eff/probed,2)); 

       h_effmc->SetCellContent(i+1,j+1,effmc);
       h_effmc->SetCellError  (i+1,j+1,efferrmc);

       double corrfact=eff/effmc;
       double corrfacterr=sqrt(pow(efferr/effmc,2) + pow(corrfact/effmc * efferrmc,2));

       h_corr->SetCellContent(i+1,j+1,corrfact);
       h_corr->SetCellError(i+1,j+1,corrfacterr);

       cout << "bins(eta,pt): " << i << "," << j << "  efficiency data: " << eff << "+-" << efferr << endl;
       cout << "                   efficiency mc: " << effmc << "+-" << efferrmc << endl;
       cout << "               correction factor: " << corrfact << "+-" << corrfacterr << endl;
     }
   }



   h_effdata->Write();
   h_effmc->Write();
   h_corr->Write();

   in->Close();
   out->Close();

}


void getEfficiencies(){
	
  gSystem->Load("libRooFit");
  writeEfficiencyHistos("InvMass_Elecs_ID.root","testEffHistos.root");

}





