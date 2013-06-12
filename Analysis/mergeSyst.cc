#include "TtZAnalysis/Tools/interface/containerStackVector.h"
#include <vector>
#include "TString.h"
#include "TFile.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
TString stripRoot(TString s){
  if(s.EndsWith(".root")){ //cut ".root"
    s.ReplaceAll(".root","");
  }
  return s;
}

TString stripStuff(TString s){
  if(s.EndsWith(".root")){ //cut ".root"
    s.ReplaceAll(".root","");
  }
  //get rid of dir
  if(s.Contains("/")){
    size_t slashpos=s.Last('/');
    s = TString(&s(slashpos+1),s.Length()-slashpos);
  }
  return s;
}
TString getChannel(TString s){
  if(s.Contains("ee_")) return "ee";
  if(s.Contains("emu_")) return "emu";
  if(s.Contains("mumu_")) return "mumu";
}

int main(int argc, char* argv[]){
  using namespace std;
  AutoLibraryLoader::enable();

  // vector<TString> filenameids;
  if(argc <4){
    cout << "at least one nominal sample (first) and sys variations up and down need to be specified!" << endl;
    return -1;
  }
  //vector<TFile *> sysfiles;
  // vector<TTree *> systrees;
  vector<TString> names;
  ztop::container1DStackVector vnominal_ee;
  ztop::container1DStackVector vnominal_emu;
  ztop::container1DStackVector vnominal_mumu;
  vector<ztop::container1DStackVector> sysvecs_ee;
  vector<ztop::container1DStackVector> sysvecs_emu;
  vector<ztop::container1DStackVector> sysvecs_mumu;

  //read in files

  TString output;


  for(int i=1;i<argc;i++){ 
    if((TString)argv[i] == "-o"){ //output
      output=(TString)argv[i+1];
      i++;
      continue;
    }
    TFile * ftemp=new TFile(stripRoot((TString)argv[i])+"_plots.root","read");
    TTree * ttemp = (TTree*)ftemp->Get("stored_objects");
    TString name=stripStuff((TString)argv[i]);
    ztop::container1DStackVector vtemp;

    std::cout << "available: " << std::endl;
    vtemp.listAllInTree(ttemp);

    std::cout << "loading " << name << std::endl;
    

    vtemp.loadFromTree(ttemp,name);

    std::cout << vtemp.getName() << std::endl;

    TString chan=getChannel(name);

    ftemp->Close();
    delete ftemp;

    bool isnominal=name.Contains("nominal");
    if(isnominal){
      if(chan =="ee")
	vnominal_ee=vtemp;
      else if(chan =="emu")
	vnominal_emu=vtemp;
      else if(chan =="mumu")
	vnominal_mumu=vtemp;
    }
    else{
      if(chan =="ee")
	sysvecs_ee.push_back(vtemp);
      else if(chan =="emu")
	sysvecs_emu.push_back(vtemp);
      else if(chan =="mumu")
	sysvecs_mumu.push_back(vtemp);
    }
  }

  for(size_t i=0;i<sysvecs_ee.size();i++){
    vnominal_ee.addMCErrorStackVector(sysvecs_ee.at(i));
  }
  for(size_t i=0;i<sysvecs_emu.size();i++){
    vnominal_emu.addMCErrorStackVector(sysvecs_emu.at(i));
  }
  for(size_t i=0;i<sysvecs_mumu.size();i++){
    vnominal_mumu.addMCErrorStackVector(sysvecs_mumu.at(i));
  }

  //
  if(output!="")
    output = "_"+output; 

  vnominal_ee.setName(vnominal_ee.getName()+output);
  vnominal_ee.writeAllToTFile(vnominal_ee.getName()+output+"_syst.root",true);

  vnominal_emu.setName(vnominal_emu.getName()+output);
  vnominal_emu.writeAllToTFile(vnominal_emu.getName()+output+"_syst.root",true);

  vnominal_mumu.setName(vnominal_mumu.getName()+output);
  vnominal_mumu.writeAllToTFile(vnominal_mumu.getName()+output+"_syst.root",true);

  return 0;
}
