#include "TtZAnalysis/Tools/interface/containerStackVector.h"
#include "TtZAnalysis/Tools/interface/containerUF.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include <cstdlib>

#include "makePlotsScript.h"


//////// change all these 4 thingies to a vector!! is easier to handle (esp if emu gets involved for some reason)

void getCrossSections(){

  using namespace std;
  using namespace ztop;


  bool writeAllOut=true;

  // get input files
  // select some stuff
  // make some correlation stuff (containerUF)
  // plots some xsecs and systematics
  // remove and add systematics if you like

  // load files and full information from containerstackverctors

  TString inputdir="/scratch/hh/dust/naf/cms/user/kieseler/Analysis/CMSSW_5_3_7/src/TtZAnalysis/Analysis/newFull_2013-02-18_23:13";

  TString outdir="plots_2102_fC";
  TString Tsyst_cmd=("mkdir "+outdir);
  const char * syst_cmd= Tsyst_cmd;
  system(syst_cmd);

  TFile * inputFile         = new TFile(inputdir+"/analysis_output.root");

  double lumi7TeV=5100; //lumi uncertainty is added in Analyzer.C
  double lumi8TeV=12200;

  TString ttstep="8";  // 8: 1btag, 9: 2btag, 10, not yet ... all only have one bin

  TString treename="stored_objects";

  TTree * storedObjects   = (TTree*) inputFile->Get  (treename);

 
  vector<container1DStackVector> vecsfull; //without DY rescaling .. ee8TeV ee7TeV mumu8TeV mumu7TeV comb8TeV comb7TeV MUST be kept
  vector<container1DStackVector> dyVecFull, fullbgerrors;

  container1DStackVector tempvec,tempvec2;

  tempvec.loadFromTree(storedObjects, "8TeV_allErrors_ee");
  vecsfull << tempvec;
  tempvec.loadFromTree(storedObjects, "7TeV_allErrors_ee");
  vecsfull << tempvec;
  tempvec.loadFromTree(storedObjects, "8TeV_allErrors_mumu");
  vecsfull << tempvec;
  tempvec.loadFromTree(storedObjects, "7TeV_allErrors_mumu");
  vecsfull << tempvec;

  
	   
  // vecsfull << tempvec.loadFromTree(storedObjects, "8TeV_allunc_ee") + tempvec2.loadFromTree(storedObjects, "8TeV_allunc_mumu")
  //	   << tempvec.loadFromTree(storedObjects, "7TeV_allunc_ee") + tempvec2.loadFromTree(storedObjects, "7TeV_allunc_mumu");

  // operator + not (yet) defined for csv; runs into problems when using containers to get efficiencies

  //////do the DY rescaling
  TString outfile=outdir+"/fullcontrol_and_xsec_graphs.root";

  if(writeAllOut){
  TFile * tempfile = new TFile(outfile,"RECREATE");
  tempfile->Close();
  delete tempfile;
  }

  for(unsigned int i=0;i<vecsfull.size();i++){

    TString add="8TeV_";
    if(vecsfull.at(i).getName().Contains("7TeV"))
      add="7TeV_";

    add="";

    cout << "doing ttbar background variation for " << vecsfull.at(i).getName() << endl;

    vector<TString> bgcontr; bgcontr << "t#bar{t}bg" << "singleTop" << "VV" << "Wjets" << "DY#rightarrow#tau#tau" << "Z#rightarrowll"  << "DY#rightarrowll";
    vector<double> scales;   scales  << 0.3          << 0.3         << 0.3  << 0.3     << 0.5                     << 0.5               << 0.5;


    container1DStackVector temp3=vecsfull.at(i);

    for(unsigned int j=0;j<bgcontr.size();j++){

      container1DStackVector temp=vecsfull.at(i);
      container1DStackVector temp2=vecsfull.at(i);
      
      temp.multiplyNorm( bgcontr.at(j),1+scales.at(j),"step "+ttstep);
      temp2.multiplyNorm(bgcontr.at(j),1-scales.at(j),"step "+ttstep);
      
      //Z selection: step 20 just scale by 100%

      //  cout <<       bgcontr.at(j) << endl;

      temp3.addMCErrorStackVector(add+bgcontr.at(j)+"_up",temp);
      temp3.addMCErrorStackVector(add+bgcontr.at(j)+"_down",temp2);
    }

    cout << "doing Z background variation for " << vecsfull.at(i).getName() << endl;

    vector<TString> Zbg; Zbg << "t#bar{t}"<< "t#bar{t}#tau" << "t#bar{t}bg" << "singleTop" << "VV" << "Wjets" << "DY#rightarrow#tau#tau";
    for(unsigned int j=0;j<bgcontr.size();j++){

      container1DStackVector temp=vecsfull.at(i);
      container1DStackVector temp2=vecsfull.at(i);
      
      temp.multiplyNorm( Zbg.at(j),2,"step 20");
      temp2.multiplyNorm(Zbg.at(j),0,"step 20");


      temp3.addMCErrorStackVector(add+bgcontr.at(j)+"_Z_up",temp);
      temp3.addMCErrorStackVector(add+bgcontr.at(j)+"_Z_down",temp2);

    }



    temp3.setName(temp3.getName() + "_BGErrors");
    fullbgerrors << temp3;
    cout << "writing " << temp3.getName() << endl;
    if(writeAllOut) temp3.writeAllToTFile  (outfile,false);
  }
  






    
  //// fill stuff to containerUF and do the "unfolding" selected distr is the same for Z and tt after gen filter?! 

  TString ttgenerateddistr = "generated filtered events";
  TString ttselecteddistr  = "ttbar selection step "+ttstep;  // 8: 1btag, 9: 2btag, 10, not yet ... all only have one bin
  TString Zgenerateddistr  = "generated filtered events";
  TString Zselecteddistr   = "Z final selection step 20";
  TString ttSignalName     = "t#bar{t}"; 
  TString ZSignalName      = "Z#rightarrowll";

  vector<container1DUF> xsecs;

  for(unsigned int i=0;i<fullbgerrors.size();i++){
    double templumi=0;
    if(fullbgerrors.at(i).getName().Contains("7TeV"))
      templumi=lumi7TeV;
    else
      templumi=lumi8TeV;
    //ttbar xsecs
    container1DUF temp(fullbgerrors.at(i).getStack(ttgenerateddistr), fullbgerrors.at(i).getStack(ttselecteddistr),   ttSignalName, templumi);
    temp.setName("ttbar__"+fullbgerrors.at(i).getName());
    std::cout << "\n this was for " << temp.getName() <<std::endl;
    xsecs << temp;
    //Z xsecs
    container1DUF tempZ(fullbgerrors.at(i).getStack(Zgenerateddistr), fullbgerrors.at(i).getStack(Zselecteddistr),   ZSignalName, templumi);
    tempZ.setName("Z__"+fullbgerrors.at(i).getName());
    std::cout << "\n this was for " << tempZ.getName() <<std::endl;
    xsecs << tempZ;
    cout << "\n" << endl;
  }

  std::cout << "UNFOLDING DONE!!\n\n\n\n" <<std::endl;

  
  TCanvas * c =new TCanvas();
  TH1D * h;
  container1DUF sys;
  for(unsigned int i=0;i<xsecs.size();i++){
    cout << "\n" << xsecs.at(i).getName() << endl;
    xsecs.at(i).coutBinContents();
    sys = xsecs.at(i).breakDownRelSystematicsInBin(1);
    sys.coutBinContents();
    h=sys.getTH1D("",false);
    h->GetXaxis()->SetLabelSize(0.015);
    h->Draw("AXIS");
    sys.getTGraph("",false)->Draw("same");
    c->Print(outdir+"/"+xsecs.at(i).getName()+".pdf");
    cout << "\n\n" << endl;
    xsecs.at(i).removeError("MC_stat_up");
    xsecs.at(i).removeError("MC_stat_down");
    //  break;
  }

  // do ratios:

  // ee 8TeV tt,Z
  // ee 7TeV tt,Z
  // mumu 8TeV tt,Z
  // mumu 7TeV tt,Z

  std::cout << xsecs.size() << std::endl;


//dont forget renamng MC_stat syst/

  //single ratios
  container1DUF doubleRee=xsecs.at(0) / xsecs.at(1) / (xsecs.at(2) / xsecs.at(3));
  container1DUF doubleRmumu=xsecs.at(4) / xsecs.at(5) / (xsecs.at(6) / xsecs.at(7));

  c->Clear();

  TFile file(outfile,"UPDATE");
  file.ls();

  TLine *l = new TLine(0, 1.23, 1, 1.23);

  std::cout << "double Ratio ee" <<std::endl;
  doubleRee.coutBinContents();
  h=doubleRee.getTH1D("doubleR_ee",false);
  h->Draw("AXIS");
  h->Write();
  doubleRee.getTGraph("",false)->Draw("same");
  l->SetLineColor(kRed);
  l->Draw("same");
  c->Print(outdir+"/doubleRee.pdf");
  sys = doubleRee.breakDownRelSystematicsInBin(1);
  h=sys.getTH1D("",false);
  h->GetXaxis()->SetLabelSize(0.015);
  h->Draw("AXIS");
  sys.getTGraph("",false)->Draw("same");
  c->Print(outdir+"/doubleRee_sys.pdf");
 
  std::cout << "double Ratio mumu" <<std::endl;
  doubleRmumu.coutBinContents();
  doubleRmumu.getTH1D("doubleR_mumu",false)->Draw("AXIS");
  doubleRmumu.getTGraph("",false)->Draw("same");
  l->SetLineColor(kRed);
  l->Draw("same");
  c->Print(outdir+"/doubleRmumu.pdf");
  sys = doubleRmumu.breakDownRelSystematicsInBin(1);
  h=sys.getTH1D("",false);
  h->GetXaxis()->SetLabelSize(0.015);
  h->Draw("AXIS");
  sys.getTGraph("",false)->Draw("same");
  c->Print(outdir+"/doubleRmumu_sys.pdf");
 


  // 

  /*
  //do other stuff by looping over xsec.at(i) and as for .Contains(7TeV / 8TeV) && Contains(Z__ bzw ttbar__) (ee, mumu)

  // vector<TString> s7TeVee,s7TeVmumu,s8TeVee,s8TeVmumu;
  // s7TeVee   << "ee"   << "7TeV";
  // s7TeVmumu << "mumu" << "7TeV";
  // s8TeVee << "ee" << "8TeV";
  // s8TeVmumu << "mumu" << "8TeV";

  // for(unsigned int i=0;i<xsecs.size();i++){
  //   for(unsigned int j=0;j<xsecs.size();j++){
  //     /// do the double ratios
  //     if(xsecs.at(i).nameContains(s7TeVee)){
  // 	//	cout << s7TeVee << endl;
  // 	//	xsecs.at(i).coutBinContents(); 
  //     }
  //     else if(xsecs.at(i).nameContains(s8TeVee)){
  // 	//	cout << s8TeVee << endl;
  // 	xsecs.at(i).coutBinContents(); 
  //     }
      
  //   }
  // }


  //// because I am curious, double ratio ;)

    /*
  container1DUF doubleratioEE = (ttee8TeV / Zee8TeV) / (ttee7TeV / Zee7TeV);
  std::cout << "dielectron double ratio ttstep: " << ttstep << std::endl;
  doubleratioEE.coutBinContents();
  doubleratioEE.writeTGraph("ee_dRatio_ttstep"+ttstep,false);
  std::cout << "dielectron double ratio systematics ttstep: "  << ttstep<< std::endl;
  container1DUF eesyst = doubleratioEE.breakDownRelSystematicsInBin(doubleratioEE.getBinNo(1));
  eesyst.coutBinContents();
  eesyst.writeTGraph("ee_dRatio_sys_ttstep"+ttstep,false);


  container1DUF doubleratioMUMU = (ttmumu8TeV / Zmumu8TeV) / (ttmumu7TeV / Zmumu7TeV);
  std::cout << "dimuon double ratio ttstep: " << ttstep << std::endl;
  doubleratioMUMU.coutBinContents();
  doubleratioMUMU.writeTGraph("mumu_dRatio_ttstep"+ttstep,false);
  std::cout << "dimuon double ratio systematics ttstep: "  << ttstep<< std::endl;
  container1DUF mumusyst = doubleratioMUMU.breakDownRelSystematicsInBin(doubleratioMUMU.getBinNo(1));
  mumusyst.coutBinContents();
  mumusyst.writeTGraph("mumu_dRatio_sys_ttstep"+ttstep,false);

  outfile->Close();

  // combined??


  
  ///// clear memory
  eeinputfile8TeV->Close();
  mumuinputfile8TeV->Close();
  eeinputfile7TeV->Close();
  mumuinputfile7TeV->Close();
    */
  
  makePlotsScript(outdir);
 
}
