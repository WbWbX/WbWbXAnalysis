#include "TtZAnalysis/Tools/interface/containerStackVector.h"
#include "TtZAnalysis/Tools/interface/containerUF.h"
#include "TtZAnalysis/Tools/interface/miscUtils.h"
#include <cstdlib>

top::container1DStackVector rescaleDY(top::container1DStackVector vec, std::vector<TString> contributions, double scalescale=1, bool textout=true, TString identifier="dilepton invariant massZ "){
 
  std::vector<TString> ident;
  std::vector<double> scales;
  for(int i=5;i<=9;i++){
    TString stepstring="step "+toTString(i);
    double dymc = 0;
    std::vector<TString> allbutdyanddata;

    for(unsigned int j=0;j<contributions.size();j++){
      dymc += vec.getStack(identifier+stepstring).getContribution(contributions.at(j)).integral();
      allbutdyanddata << contributions.at(j);
    }
    double d = vec.getStack(identifier+stepstring).getContribution("data").integral();
    allbutdyanddata << "data";
    double rest = vec.getStack(identifier+stepstring).getContributionsBut(allbutdyanddata).integral();
    if(rest==0) rest=1;
    double scale = (d-rest)/dymc;
    scales << scale*scalescale;
    ident << stepstring;
    if(textout) std::cout << "Scalefactor for "<< vec.getName() << " " << stepstring << ": " << scale << std::endl;
  }
  top::container1DStackVector rescaled=vec;
  for(unsigned int i=0;i<contributions.size();i++){
    rescaled.multiplyNorms(contributions.at(i), scales, ident);
  }
  return rescaled;

}



//////// change all these 4 thingies to a vector!! is easier to handle (esp if emu gets involved for some reason)

void getCrossSections(){

  using namespace std;
  using namespace top;

  // get input files
  // select some stuff
  // make some correlation stuff (containerUF)
  // plots some xsecs and systematics
  // remove and add systematics if you like

  // load files and full information from containerstackverctors

  TString inputdir="";

  TFile * inputFile         = new TFile("syncOv_2012-12-06_14:43/analysis_output.root");

  double lumi7TeV=5100; //lumi uncertainty is added in Analyzer.C
  double lumi8TeV=12100;

  TString ttstep="8";  // 8: 1btag, 9: 2btag, 10, not yet ... all only have one bin

  TString treename="stored_objects";

  TTree * storedObjects   = (TTree*) inputFile->Get  (treename);

 
  vector<container1DStackVector> vecsfull; //without DY rescaling .. ee8TeV ee7TeV mumu8TeV mumu7TeV comb8TeV comb7TeV MUST be kept
  vector<container1DStackVector> dyVecFull, dyVecFullWithDYErrors;

  container1DStackVector tempvec,tempvec2;

  tempvec.loadFromTree(storedObjects, "8TeV_btag_ee_ee");
  vecsfull << tempvec;
  //  tempvec.loadFromTree(storedObjects, "7TeV_btag_ee_ee");
  //  vecsfull << tempvec;
  tempvec.loadFromTree(storedObjects, "8TeV_btag_mumu_mumu");
  vecsfull << tempvec;
  //  tempvec.loadFromTree(storedObjects, "7TeV_btag_mumu_mumu");
  //  vecsfull << tempvec;

  
	   
  // vecsfull << tempvec.loadFromTree(storedObjects, "8TeV_allunc_ee") + tempvec2.loadFromTree(storedObjects, "8TeV_allunc_mumu")
  //	   << tempvec.loadFromTree(storedObjects, "7TeV_allunc_ee") + tempvec2.loadFromTree(storedObjects, "7TeV_allunc_mumu");

  // operator + not (yet) defined for csv; runs into problems when using containers to get efficiencies

  //////do the DY rescaling

  TString outfile="fullcontrol_and_xsec_graphs.root";
  TFile * tempfile = new TFile(outfile,"RECREATE");
  tempfile->Close();
  delete tempfile;

  vector<TString> dycontributions;
  dycontributions << "Z#rightarrowll" << "DY#rightarrowll";

  for(unsigned int i=0;i<vecsfull.size();i++){
    //scale DY

    cout << "scaling DY for " << vecsfull.at(i).getName() << endl;

    container1DStackVector temp=rescaleDY(vecsfull.at(i), dycontributions);
    temp.setName(temp.getName() + "_dyScaled");
    temp.writeAllToTFile  (outfile,false);
    dyVecFull << temp;

    // continue;
    //vary DY
    container1DStackVector scaledtempup,scaledtempdown;
    scaledtempup   = rescaleDY(vecsfull.at(i), dycontributions, 1.5, false);
    scaledtempdown = rescaleDY(vecsfull.at(i), dycontributions, 0.5, false);


    //assume no correlation between the variations.. might be wrong? would be nice if ;)
    if(scaledtempup.getName().Contains("7TeV")){
      temp.addMCErrorStackVector("7TeV_DY_up",scaledtempup);
      temp.addMCErrorStackVector("7TeV_DY_down",scaledtempdown);
    }
    else{
      temp.addMCErrorStackVector("8TeV_DY_up",scaledtempup);
      temp.addMCErrorStackVector("8TeV_DY_down",scaledtempdown);
    }
    temp.setName(temp.getName() + "_dyErrors");
    temp.writeAllToTFile  (outfile,false);
    dyVecFullWithDYErrors << temp;

  }


  vector<container1DStackVector> fullbgerrors=dyVecFullWithDYErrors;


  for(unsigned int i=0;i<dyVecFullWithDYErrors.size();i++){

    TString add="8TeV_";
    if(dyVecFullWithDYErrors.at(i).getName().Contains("7TeV"))
      add="7TeV_";

    cout << "doing background variation for " << dyVecFullWithDYErrors.at(i).getName() << endl;

    vector<TString> bgcontr; bgcontr << "t#bar{t}bg" << "singleTop" << "VV" << "Wjets" << "DY#rightarrow#tau#tau";
    vector<double> scales;   scales  << 0.3          << 0.3         << 0.3  << 0.3     << 0.5;


    container1DStackVector temp3=dyVecFullWithDYErrors.at(i);

    for(unsigned int j=0;j<bgcontr.size();j++){

      container1DStackVector temp=dyVecFullWithDYErrors.at(i);
      container1DStackVector temp2=dyVecFullWithDYErrors.at(i);
      
      temp.multiplyNorm( bgcontr.at(j),1+scales.at(j),"step "+ttstep);
      temp2.multiplyNorm(bgcontr.at(j),1-scales.at(j),"step "+ttstep);
      
      //Z selection: step 20 just scale by 100%
      
      temp.multiplyNorm( bgcontr.at(j),2,"step 20");
      temp2.multiplyNorm(bgcontr.at(j),0,"step 20");
      
      //  cout <<       bgcontr.at(j) << endl;

      temp3.addMCErrorStackVector(add+bgcontr.at(j)+"_up",temp);
      temp3.addMCErrorStackVector(add+bgcontr.at(j)+"_down",temp2);
    }
    temp3.setName(temp3.getName() + "_BGErrors");
    fullbgerrors << temp3;
    temp3.writeAllToTFile  (outfile,false);
  }


  //// fill stuff to containerUF and do the "unfolding" selected distr is the same for Z and tt after gen filter?! 

  TString ttgenerateddistr = "generated filtered events";
  TString ttselecteddistr  = "ttbar selection step "+ttstep;  // 8: 1btag, 9: 2btag, 10, not yet ... all only have one bin
  TString Zgenerateddistr  = "generated filtered events";
  TString Zselecteddistr   = "Z final selection step 20";
  TString ttSignalName     = "t#bar{t}"; 
  TString ZSignalName      = "Z#rightarrowll";

  vector<container1DUF> xsecs;

  for(unsigned int i=0;i<dyVecFullWithDYErrors.size();i++){
    double templumi=0;
    if(dyVecFullWithDYErrors.at(i).getName().Contains("7TeV"))
      templumi=lumi7TeV;
    else
      templumi=lumi8TeV;
    //ttbar xsecs
    container1DUF temp(dyVecFullWithDYErrors.at(i).getStack(ttgenerateddistr), dyVecFullWithDYErrors.at(i).getStack(ttselecteddistr),   ttSignalName, templumi);
    temp.setName("ttbar__"+dyVecFullWithDYErrors.at(i).getName());
    xsecs << temp;
    //Z xsecs
    container1DUF tempZ(dyVecFullWithDYErrors.at(i).getStack(Zgenerateddistr), dyVecFullWithDYErrors.at(i).getStack(Zselecteddistr),   ZSignalName, templumi);
    tempZ.setName("Z__"+dyVecFullWithDYErrors.at(i).getName());
    xsecs << tempZ;
  }

  for(unsigned int i=0;i<xsecs.size();i++){
    cout << "\n" << xsecs.at(i).getName() << endl;
    xsecs.at(i).coutBinContents();
    container1DUF sys = xsecs.at(i).breakDownRelSystematicsInBin(1);
    sys.coutBinContents();
  }

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

 
}
