#include "TtZAnalysis/Tools/interface/containerStackVector.h"
#include "TtZAnalysis/Tools/interface/containerUF.h"

top::container1DStackVector rescaleDY(top::container1DStackVector vec, std::vector<TString> contributions, double scalescale=1, bool textout=true, TString identifier="dilepton invariant massZ "){
  std::vector<TString> ident;
  std::vector<double> scales;
  for(int i=4;i<=9;i++){
    TString stepstring="step "+toTString(i);
    double dymc = 0;
    for(unsigned int i=0i<contributiions.size();i++){
      dymc += vec.getStack(identifier+stepstring).getContribution(contributions.at(i)).integral();
    }
    double d = vec.getStack(identifier+stepstring).getContribution("data").integral();
    std::vector<TString> allbutdyanddata;
    allbutdyanddata << "data" << contribution;
    double ddyee = vec.getStack(identifier+stepstring).getContributionsBut(allbutdyanddata).integral();
    if(dymcee==0) dymcee=1;
    double scale = (dee-ddyee)/dymcee;
    scales << scale*scalescale;
    ident << stepstring;
    if(textout) std::cout << "Scalefactor for "<< vec.getName() << stepstring << ": " << scale << std::endl;
  }
  top::container1DStackVector rescaled=vec.;
  for(unsigned int i=0i<contributiions.size();i++){
    rescaled.multiplyNorms(contributions.at(i), scales, ident);
  }
  return rescaled;
}

void addBGScalingErrors(){



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

  TFile * inputFile         = new TFile("analysis_output.root");

  double lumi7TeV=5;
  double lumi8TeV=12;

  TString ttstep="8";  // 8: 1btag, 9: 2btag, 10, not yet ... all only have one bin

  TString treename="stored_objects";

  TTree * storedObjects   = (TTree*) inputfile->Get  (treename);

 
  vector<container1DStackVector> vecsfull; //without DY rescaling .. ee8TeV ee7TeV mumu8TeV mumu7TeV comb8TeV comb7TeV MUST be kept
  vector<container1DStackVector> dyVecFull, dyVecFullWithDYErrors;

  container1DStackVector tempvec,tempvec2;

  vecsfull << tempvec.loadFromTree(storedObjects, "8TeV_allunc_ee")
	   << tempvec.loadFromTree(storedObjects, "7TeV_allunc_ee")
	   << tempvec.loadFromTree(storedObjects, "8TeV_allunc_mumu")
	   << tempvec.loadFromTree(storedObjects, "7TeV_allunc_mumu");
  vecsfull << tempvec.loadFromTree(storedObjects, "8TeV_allunc_ee") + tempvec2.loadFromTree(storedObjects, "8TeV_allunc_mumu")
	   << tempvec.loadFromTree(storedObjects, "7TeV_allunc_ee") + tempvec2.loadFromTree(storedObjects, "7TeV_allunc_mumu");


  //////do the DY rescaling


  TFile * outfile = new TFile("fullcontrol_and_xsec_graphs.root","RECREATE");

  vector<TString> dycontributions;
  dycontributions << "Z#rightarrowll" << "DY#rightarroqll";

  for(unsinged int i=0;i<vecsfull.size();i++){
    //scale DY
    container1DStackVector temp=rescaleDY(vecsfull.at(i), dycontributions);
    temp.setName(temp.getName() + "_dyScaled");
    temp.writeAllToTFile  (outfile->GetName());
    dyVecFull << temp;

    //vary DY
    container1DStackVector scaledtempup,scaledtempdown;
    scaledtempup   = rescaleDY(vecsfull.at(i), dycontributions, 1.5, false);
    scaledtempdown = rescaleDY(vecsfull.at(i), dycontributions, 0.5, false);


    //assume no correlation between the variations.. might be wrong? would be nice if ;)
    if(scaledtemp.getName().Contains("7TeV")){
      temp.addMCErrorStackVector("7TeV_DY_up",scaledtempup);
      temp.addMCErrorStackVector("7TeV_DY_down",scaledtempdown);
    }
    else{
      temp.addMCErrorStackVector("8TeV_DY_up",scaledtempup);
      temp.addMCErrorStackVector("8TeV_DY_down",scaledtempdown);
    }
    temp.setName(temp.getName() + "_dyErrors");
    temp.writeAllToTFile  (outfile->GetName());
    dyVecFullWithDYErrors << temp;

  }





  //// fill stuff to containerUF and do the "unfolding" selected distr is the same for Z and tt after gen filter?! 

  TString ttgenerateddistr = "generated filtered events";
  TString ttselecteddistr  = "ttbar final selection step "+ttstep;  // 8: 1btag, 9: 2btag, 10, not yet ... all only have one bin
  TString Zgenerateddistr  = "generated filtered events";
  TString Zselecteddistr   = "Z final selection step 20";
  TString ttSignalName     = "t#bar{t}"; 
  TString ZSignalName      = "Z#rightarrow ll";

  container1DUF ttee8TeV  (dyVecFullEE8TeV.getStack(ttgenerateddistr),   dyVecFullEE8TeV.getStack(ttselecteddistr),   ttSignalName, lumi8TeV);
  container1DUF ttmumu8TeV(dyVecFullMuMu8TeV.getStack(ttgenerateddistr), dyVecFullMuMu8TeV.getStack(ttselecteddistr), ttSignalName, lumi8TeV);
  container1DUF ttee7TeV  (dyVecFullEE7TeV.getStack(ttgenerateddistr),   dyVecFullEE7TeV.getStack(ttselecteddistr),   ttSignalName, lumi7TeV);
  container1DUF ttmumu7TeV(dyVecFullMuMu7TeV.getStack(ttgenerateddistr), dyVecFullMuMu7TeV.getStack(ttselecteddistr), ttSignalName, lumi7TeV);

  container1DUF Zee8TeV  (dyVecFullEE8TeV.getStack  (Zgenerateddistr),   dyVecFullEE8TeV.getStack(Zselecteddistr),   ZSignalName, lumi8TeV);
  container1DUF Zmumu8TeV(dyVecFullMuMu8TeV.getStack(Zgenerateddistr), dyVecFullMuMu8TeV.getStack(Zselecteddistr),   ZSignalName, lumi8TeV);
  container1DUF Zee7TeV  (dyVecFullEE7TeV.getStack  (Zgenerateddistr),   dyVecFullEE7TeV.getStack(Zselecteddistr),   ZSignalName, lumi7TeV);
  container1DUF Zmumu7TeV(dyVecFullMuMu7TeV.getStack(Zgenerateddistr), dyVecFullMuMu7TeV.getStack(Zselecteddistr),   ZSignalName, lumi7TeV);


  //// because I am curious, double ratio ;)


  container1DUF doubleratioEE = (ttee8TeV / Zee8TeV) / (ttee7TeV / Zee7TeV);
  std::cout << "dielectron double ratio: " << ttstep << std::endl;
  doubleratioEE.coutBinContents();
  doubleratioEE.writeTGraph("ee_dRatio_"+ttstep,false);
  std::cout << "dielectron double ratio systematics: "  << ttstep<< std::endl;
  container1DUF eesyst = doubleratioEE.breakDownRelSystematicsInBin(doubleratioEE.getBinNo(1));
  eesyst.coutBinContents();
  eesyst.writeTGraph("ee_dRatio_sys_"+ttstep,false);


  container1DUF doubleratioMUMU = (ttmumu8TeV / Zmumu8TeV) / (ttmumu7TeV / Zmumu7TeV);
  std::cout << "dimuon double ratio: " << ttstep << std::endl;
  doubleratioMUMU.coutBinContents();
  doubleratioMUMU.writeTGraph("mumu_dRatio_"+ttstep,false);
  std::cout << "dimuon double ratio systematics: "  << ttstep<< std::endl;
  container1DUF mumusyst = doubleratioMUMU.breakDownRelSystematicsInBin(doubleratioMUMU.getBinNo(1));
  mumusyst.coutBinContents();
  mumusyst.writeTGraph("mumu_dRatio_sys_"+ttstep,false);

  outfile->Close();

  // combined??



  ///// clear memory
  eeinputfile8TeV->Close();
  mumuinputfile8TeV->Close();
  eeinputfile7TeV->Close();
  mumuinputfile7TeV->Close();
  
}
