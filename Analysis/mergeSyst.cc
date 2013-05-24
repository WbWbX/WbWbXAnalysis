#include "TtZAnalysis/Tools/interface/containerStackVector.h"
#include <vector>
#include "TString.h"
#include "TFile.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"

TString stripStuff(TString s){
  if(s.EndsWith(".root")){ //cut ".root"
      s.ReplaceAll(".root","");
    }
    //get rid of dir
    size_t slashpos=s.Last('/');
    s = TString(&s(slashpos+1),s.Length()-slashpos);
    return s;
}

int main(int argc, char* argv[]){
  using namespace std;
  AutoLibraryLoader::enable();

  // vector<TString> filenameids;
  if(argc <4){
    cout << "at least one nominal sample (first) and sys variations up and down need to be specified!" << endl;
    return -1;
  }
  TFile * fnominal= new TFile((TString)argv[1],"read"); //this is the nominal (or suppoed to be!...)
  TTree * tnominal=(TTree*)fnominal->Get("stored_objects");
  ztop::container1DStackVector vnominal;
  vnominal.loadFromTree(tnominal,stripStuff((TString)argv[1]));

  vector<TFile *> sysfiles;
  vector<TTree *> systrees;
  vector<TString> names;
  vector<ztop::container1DStackVector> sysvecs;

  for(int i=2;i<argc;i++){ 
    if((TString)argv[i] == "-o"){ //output
      i++;
      continue;
    }
    TFile * ftemp=new TFile((TString)argv[i],"read");

    TString name=stripStuff((TString)argv[i]);
    cout << "add " << name <<endl;
    names.push_back(name);
    sysfiles.push_back(ftemp);
    TTree * ttemp = (TTree*)ftemp->Get("stored_objects");
    systrees.push_back(ttemp);
    ztop::container1DStackVector vtemp;
    vtemp.loadFromTree(ttemp,name);
    vnominal.addMCErrorStackVector(vtemp);
  }
  TString output=vnominal.getName()+"_sys";
  for(int i=2;i<argc-1;i++){ 
    if((TString)argv[i] == "-o"){ //output
      output=(TString)argv[i+1];
      
      break;
    }
  }
  vnominal.setName(output);
  vnominal.writeAllToTFile(output+".root",true);

  return 0;
}
