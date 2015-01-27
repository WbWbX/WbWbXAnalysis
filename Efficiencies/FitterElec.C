#define treefolder "/scratch/hh/current/cms/user/cdiez/TreesJan/0604/"
// #ifndef __CINT__
//#include "/afs/naf/user/k/kieseler/scratch/roofit/inc/RooGlobalFunc.h"
// #endif
#include "TSystem.h"
#include "TH1D.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TF1.h"
#include "RooWorkspace.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooRealVar.h"
#include "RooFormulaVar.h"
#include "RooAddPdf.h"
#include "RooGlobalFunc.h"
#include "RooCategory.h"
#include "RooSimultaneous.h"
#include "RooPlot.h"
#include "RooFitResult.h"
#include "RooBinning.h"
#include "RooBinningCategory.h"
#include "RooMultiCategory.h"
#include "RooMappedCategory.h"
#include "RooThresholdCategory.h"
#include "Roo1DTable.h"
#include "RooMinuit.h"
#include "RooNLLVar.h"
#include "RooAbsDataStore.h"
#include "RooEfficiency.h"
#include "RooGaussian.h"
#include "RooChebychev.h"
#include "RooProdPdf.h"
#include "RooGenericPdf.h"
#include "RooExtendPdf.h"
#include "RooTrace.h"
#include "RooMsgService.h"
#include "Math/QuantFuncMathCore.h"



//#include "functions.h"
#include "TFitter.h"
struct t_probe{
  double pt, eta, d0, InvMass, id, iso;
  int isPF, mHits, isNotConv;
};


void FitterElec(){
  using namespace std;	
  gSystem->Load("libRooFit");
  using namespace RooFit;

  float etamin = 0.0; 
  float etamax = 0.8;


  TFile * f1 = new TFile("ElecEff_gsfRho_eta24Tight76106MET402Elec.root");

  TTree *t = (TTree*)(f1->Get("ProbeElecVar"));
  TH1F *h = new TH1F("mll","Passing probes; M_{ee} (GeV); Events/(1 GeV)",30,60,120);
  TH1F *h2 = new TH1F("mll2","Failing probes; M_{ee} (GeV); Events/(1 GeV)",30,60,120); 
  t->Draw("InvMass>>mll","iso < 50.1 && id>0.9 && d0 < 0.02 && isPF && mHits <=0  && isNotConv && pt>20. && pt<30. && InvMass > 60.&& abs(eta) < 1.5 && abs(eta) > 0.8");
  t->Draw("InvMass>>mll2","pt>20. && pt<30. && !(iso < 50.1 && id>0.9 && d0 < 0.02 && isPF && mHits <=0  && isNotConv)  && InvMass > 60. && abs(eta) < 1.5 && abs(eta) > 0.8");
 
  double rangemin=h->GetXaxis()->GetXmin();
  double rangemax=h->GetXaxis()->GetXmax();
	
  RooRealVar mass("mass","mass",rangemin,rangemax);
  mass.setRange("cutcut",76,116); 
  RooDataHist roohist("roohist","roohist",mass,Import(*h));
  RooDataHist roohist2("roohist2","roohist2",mass,Import(*h2));
  RooPlot * frame = mass.frame(Title("Passing probes"));
  RooPlot * frame2 = mass.frame(Title("Failing probes"));
  roohist.plotOn(frame,MarkerColor(1),MarkerSize(0.9),MarkerStyle(21)); 
  roohist.statOn(frame);
  roohist2.plotOn(frame2,MarkerColor(1),MarkerSize(0.9),MarkerStyle(21)); 
  roohist2.statOn(frame2);


  // VOIGTIAN FUNCTION
  RooRealVar mean("mean","mean",91.2, 20.0,120.0);
  RooRealVar width("width","width",1.0, 0.0, 100.0);
  RooRealVar sigmaV("sigmaV","sigmaV",5.0, 0.0, 100.0);
  RooVoigtian voigt("voigt","voigt",mass,mean,width,sigmaV);
 	

  //======// Parameters for CrystalBall 
  RooRealVar m0("M_{ll}", "Bias", 60., 20, 120,"GeV"); 
  RooRealVar sigma("#sigma_{CB}","Width", 19.2,0.0,30.0);//,"GeV/c^{2}"); 
  RooRealVar cut("#alpha","Cut", 8., 0., 25.);
  RooRealVar power("#gamma","Power", 100., 1.,450.);
  RooRealVar nsig("nsig","nsig",700.,0.,5500.);
  RooCBShape CrystalBall("CrystalBall", "A  Crystal Ball Lineshape", mass, m0,sigma, cut, power);
  //RooCBShape ResolutionModel("ResolutionModel", "Resolution Model", mass, m0,sigma, cut, power);
            
  //======//Parameters for Breit-Wigner Distribution
  RooRealVar mRes("M_{ll}", "Higgs Resonance  Mass", 60, 20.,160.);//,"GeV/c^{2}"); 
  RooRealVar Gamma("#Gamma", "#Gamma", 3.0, 0.0,20.0);//,"GeV/c^{2}"); 
  RooBreitWigner BreitWigner("BreitWigner","A Breit-Wigner Distribution",mass,mRes,Gamma);

  //      RooFFTConvPdf ResolutionModel("Convolution","Convolution", mass,  voigt, CrystalBall);
  //      RooFFTConvPdf ResolutionModel("Convolution","Convolution", mass, CrystalBall, voigt);
  RooFFTConvPdf ResolutionModel("Convolution","Convolution", mass,  BreitWigner, CrystalBall);


  RooRealVar m0f("M_{ll}", "Bias", 60., 20, 120,"GeV");
  RooRealVar sigmaf("#sigma_{CB}","Width", 9.2,0.0,30.0);//,"GeV/c^{2}"); 
  RooRealVar cutf("#alpha","Cut", 8., 0., 25.);
  RooRealVar powerf("#gamma","Power", 17., 1.,450.);
  RooRealVar nsigf("nsigf","nsigf",700.,0.,5500.);


  RooCBShape CrystalBallf("CrystalBallf", "A  Crystal Ball Lineshape", mass, m0f,sigmaf, cutf, powerf);
  //RooCBShape ResolutionModel("ResolutionModel", "Resolution Model", mass, m0,sigma, cut, power);

  //======//Parameters for Breit-Wigner Distribution
  RooRealVar mResf("M_{ll}", "Higgs Resonance  Mass", 61.2, 60.,160.);//,"GeV/c^{2}"); 
  RooRealVar Gammaf("#Gamma", "#Gamma", 3.0, 0.0,20.0);//,"GeV/c^{2}"); 
  RooBreitWigner BreitWignerf("BreitWignerf","A Breit-Wigner Distribution",mass,mResf,Gammaf);

  //      RooFFTConvPdf ResolutionModel("Convolution","Convolution", mass,  voigt, CrystalBall);
  //      RooFFTConvPdf ResolutionModel("Convolution","Convolution", mass, CrystalBall, voigt);

  RooFFTConvPdf ResolutionModelf("Convolutionf","Convolutionf", mass,  BreitWignerf, CrystalBallf);


  // Include BG
  RooRealVar nbkg("nbkg","nbkg",25.,0.,15000.);//V");
  RooRealVar nbkgf("nbkgf","nbkgf",50.,0.,150000.);//V");

  RooRealVar bkg_slope("bkg_slope","slope of the background exponential mass PDF",-10.5,0.0);
  RooExponential bkgModel("bkgModel","background mass PDF",mass,bkg_slope);

  RooRealVar c0("c0","coefficient #0", 1.0,-10.,10.) ;
  RooRealVar c1("c1","coefficient #1", 0.1,-10.,10.) ;
  RooRealVar p2("p2","coefficient #2",0.1,-10.,10.) ;
  RooRealVar p3("p3","coefficient #3",0.1,-10.,10.) ;
  RooChebychev bkgModelFail("bkgModelFail","background mass PDF",mass,RooArgList(c0,c1,p2,p3)) ; 

  RooRealVar p0("p0","coefficient #0", 1.0,-10.,10.) ;
  RooRealVar p1("p1","coefficient #1", 0.1,-10.,10.) ;
  RooRealVar p22("p22","coefficient #2",0.1,-10.,10.) ;
  RooRealVar p33("p33","coefficient #3",0.1,-10.,10.) ;

  //RooChebychev bkgModel("bkgModel","background mass PDF",mass,RooArgList(p0,p1));
  // background = new RooGenericPdf("exponential","exponential","TMath::Exp(@1+@2*@0+@3*@0*@0)",RooArgList(*x, *expCoeffa0_, *expCoeffa1_, *expCoeffa2_));
  //      RooGenericPdf  bkgModel("bkgModel","TMath::Exp(bkg_slope)*0.5*(TMath::Erf((t-1)/0.5)+1)",RooArgList(bkg_slope,c0));
  ///      RooRealVar cFail("cFail", "cFail", 0.1, -5, 5);
  ///      RooChebychev backgroundFail("backgroundFail", "backgroundFail", mass, cFail);
  ///      RooRealVar nbkg("nbkg","nbkg", 10000., 0., 1e10);
  ///      RooExtendPdf bkgModel("backgroundFailPdf", "backgroundFailPdf", backgroundFail, nbkg); 

  // Add Signal+BG
  RooAddPdf pdfFinal("pdfFinal","pdfFinal",RooArgList(ResolutionModel,bkgModel),RooArgList(nsig,nbkg));
  RooAddPdf pdfFinalFail("pdfFinalFail","pdfFinalFail",RooArgList(ResolutionModel,bkgModelFail),RooArgList(nsig,nbkgf));

  RooFitResult *fitResult = pdfFinal.fitTo(roohist, RooFit::Save(true),
					   RooFit::Extended(true), RooFit::PrintLevel(-1));

  fitResult->Print("v"); //verbose
  pdfFinal.plotOn(frame,LineColor(4));
  pdfFinal.plotOn(frame,Components("bkgModel"),LineColor(kRed),LineStyle(kDashed));
  pdfFinal.paramOn(frame); 

  double nSignalOS    = nsig.getVal();
  double BG = nbkg.getVal();
  double nSignalOSE    = nsig.getError();
  double BGE = nbkg.getError();

  // Yields:
  cout << nSignalOS <<"+/-"<< nsig.getError()<< " " << BG <<"+/-"<<nbkg.getError()<< endl;

 

  // Get integral in a restricted range   
  RooRealVar nsig2("nsig2","nsig2",10.,0.,10000.); 
  RooExtendPdf egauss2("egauss2","extended gaussian PDF w limited range",pdfFinal,nsig2,"cutcut") ;
  egauss2.fitTo(roohist);
  //      egauss2.plotOn(frame,LineColor(8));
  cout << nsig2<<"fitted number of events in data in range (76,106) = " << nsig2.getVal() <<"+/-"<<nsig2.getError() << endl ; 
 
  RooFitResult *fitResult2 = pdfFinalFail.fitTo(roohist2, RooFit::Save(true),
                                            RooFit::Extended(true), RooFit::PrintLevel(-1));
  fitResult2= pdfFinalFail.fitTo(roohist2, RooFit::Save(true),
                                            RooFit::Extended(true), RooFit::PrintLevel(-1));



  fitResult2->Print("v"); //verbose
  pdfFinalFail.plotOn(frame2,LineColor(4));
  pdfFinalFail.plotOn(frame2,Components("bkgModelFail"),LineColor(kRed),LineStyle(kDashed));
  pdfFinalFail.paramOn(frame2);

  double nSignalOS2    = nsigf.getVal();
  double BG2 = nbkgf.getVal();
  cout << nSignalOS <<"+/-"<< nSignalOSE<< " " << BG <<"+/-"<<BGE<< endl;
  
  // Yields:
  cout << nSignalOS2 <<"+/-"<< nsig.getError()<< " " << BG2 <<"+/-"<<nbkg.getError()<< endl;
  double eff = nSignalOS/(nSignalOS+nSignalOS2); 
  double error = eff*((nSignalOSE+nsig.getError())/(nSignalOS+nSignalOS2)+nsig.getError()/nSignalOS2);
  cout << eff<<"+/-"<< error<<endl; 
  cout << nsig2<<"fitted number of events in data in range (76,106) = " << nsig2.getVal() <<"+/-"<<nsig2.getError() << endl ; 
 

  TCanvas* c = new TCanvas("ZmassHisto","ZmassHisto",800,400) ;
  c->cd() ; gPad->SetRightMargin(0.25);
  frame->Draw();
  c->Print("pt20Passing0_08.eps");

  TCanvas* c2 = new TCanvas("ZmassHisto2","ZmassHisto2",800,400) ;
  c2->cd() ; gPad->SetRightMargin(0.25);
  frame2->Draw();
  c2->Print("pt20Failing0_08.eps");

}

#include "TtZAnalysis/Analysis/app_src/mainMacro.h"

invokeApplication(){
  FitterElec();
  return 1;
}
