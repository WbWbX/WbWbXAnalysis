#include "../interface/containerStackVector.h"

namespace top{


  std::vector<container1DStackVector*> container1DStackVector::csv_list;
  bool container1DStackVector::csv_makelist=false;

  container1DStackVector::container1DStackVector(){
    if(csv_makelist)csv_list.push_back(this);
  }
  container1DStackVector::container1DStackVector(TString Name){
    name_=Name;
    if(csv_makelist)csv_list.push_back(this);
  }
  container1DStackVector::~container1DStackVector(){
    for(unsigned int i=0;i<csv_list.size();i++){
      if(csv_list[i] == this) csv_list.erase(csv_list.begin()+i);
      break;
    }
  }
  void container1DStackVector::listStacks(){
    for(std::vector<top::container1DStack>::iterator stack=stacks_.begin();stack<stacks_.end();++stack){
      std::cout << stack->getName() << std::endl;
    }
  }

  void container1DStackVector::add(top::container1D & container, TString leg , int color , double norm){
    bool found=false;
    for(std::vector<top::container1DStack>::iterator s=stacks_.begin();s<stacks_.end();++s){
      if(s->getName() == container.getName()){
	s->push_back(container, leg, color, norm);
	found=true;
	break;
      }
    }
    if(!found){
      top::container1DStack newstack(container.getName());
      newstack.push_back(container, leg, color, norm);
      stacks_.push_back(newstack);
    }
  }

  void container1DStackVector::addList(TString leg, int color, double norm){
    for(unsigned int i=0;i<container1D::c_list.size();i++){
      add(*container1D::c_list[i],leg,color,norm);
    }
  }

  top::container1DStack container1DStackVector::getStack(TString name){
    top::container1DStack defout("DUMMY");
    for(std::vector<top::container1DStack>::iterator s=stacks_.begin();s<stacks_.end();++s){
      if(name == s->getName()){
        return *s;
      }
    }
    std::cout << "container1DStackVector::getStack: "<< name << " not found. returning empty Stack" << std::endl;
    return defout;
  }
  void container1DStackVector::removeContribution(TString contribution){
    for(std::vector<top::container1DStack>::iterator stack=stacks_.begin(); stack < stacks_.end(); ++stack){
      stack->removeContribution(contribution);
    }
  }

  void container1DStackVector::addMCErrorStackVector(TString sysname,top::container1DStackVector stackvec, bool ignoreMCStat){
    for(std::vector<container1DStack>::iterator istack=stacks_.begin();istack<stacks_.end(); ++istack){
      for(std::vector<container1DStack>::iterator estack=stackvec.stacks_.begin();estack<stackvec.stacks_.end(); ++estack){
	if(istack->getName() == estack->getName()){
	  istack->addMCErrorStack(sysname,*estack,ignoreMCStat);
	  break;
	}
      }
    }
  }
  void container1DStackVector::addGlobalRelMCError(TString sysname,double error){
    for(std::vector<container1DStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
      stack->addGlobalRelMCError(sysname,error);
    }
  }
  void container1DStackVector::addRelSystematicsFrom(top::container1DStackVector stackvec){
   for(std::vector<container1DStack>::iterator istack=stacks_.begin();istack<stacks_.end(); ++istack){
      for(std::vector<container1DStack>::iterator estack=stackvec.stacks_.begin();estack<stackvec.stacks_.end(); ++estack){
	if(istack->getName() == estack->getName()){
	  istack->addRelSystematicsFrom(*estack);
	  break;
	}
      }
    }
  }

  void container1DStackVector::removeError(TString name){
    for(std::vector<container1DStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
      stack->removeError(name);
    }
  }
  void container1DStackVector::renameSyst(TString old, TString New){
    for(std::vector<container1DStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
      stack->renameSyst(old,New);
    }
  }
  void container1DStackVector::multiplyNorms(TString legendname, std::vector<double> scalefactors, std::vector<TString> identifier,bool showmessages){
    if(! (identifier.size() == scalefactors.size())){
      std::cout << "container1DStackVector::multiplyNorms: identifiers and scalefactors must be same size!" << std::endl;
    }
    else{
      unsigned int count=0;
      for(std::vector<container1DStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
	for(unsigned int i=0;i<scalefactors.size();i++){
	  if(stack->getName().Contains(identifier[i])){
	    stack->multiplyNorm(legendname, scalefactors[i]);
	    if(showmessages) std::cout << "rescaling " << stack->getName() << "  " <<legendname << " by " << scalefactors[i] << " for " << std::endl;
	    count++;
	  }
	}
      }
      if(count < identifier.size()) std::cout << "container1DStackVector::multiplyNorms: warning: not all identifiers found!" << std::endl;
      else if(count > identifier.size() && showmessages) std::cout << "container1DStackVector::multiplyNorms: warning: identifiers where ambiguous! Scaled more than one stack per identifier (intended?)" << std::endl;
    }
  }

  void container1DStackVector::writeAllToTFile(TString filename, bool recreate, TString treename){
    TString name;

    if(name_=="") name="no_name";
    else name = name_;

    TString upre="UPDATE";
    if(recreate == true) upre="RECREATE";

    TFile *f = new TFile(filename,upre);
    f->cd();
    TTree * t=0;
    if(f->Get(treename)){
      t = (TTree*) f->Get(treename);
    }
    else{
      t = new TTree(treename,treename);
    }
    if(t->GetBranch("allContainerStackVectors")){ //branch does not exist yet
      bool temp=csv_makelist;
      csv_makelist=false;
      container1DStackVector * csv = this;
      t->SetBranchAddress("allContainerStackVectors",&csv);
      csv_makelist=temp;
    }
    else{
      t->Branch("allContainerStackVectors",this);
      std::cout << "container1DStackVector::writeAllToTFile: added branch" << std::endl;
    }
    t->Fill();
    t->Write("",TObject::kOverwrite);
    delete t;

    TDirectory * d = f->mkdir(name + "_ratio",name + "_ratio");
    d->cd();
    for(std::vector<container1DStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
      TCanvas * c=stack->makeTCanvas();
      c->Write();
      delete c;
    }
    //d.Close();
    // f.cd();
    TDirectory * d2 = f->mkdir(name,name);
    d2->cd();
    for(std::vector<container1DStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
      TCanvas * c2=stack->makeTCanvas(false);
      c2->Write();
      delete c2;
    }


    f->Close();
    delete f;
    // delete d;
    // delete d2;
  }

  void container1DStackVector::writeAllToTFile(TFile * f, TString treename){
    TString name;
    if(name_=="") name="no_name";
    else name = name_;

    f->cd();
    TTree * t=0;
    if(f->Get(treename)){
      t = (TTree*) f->Get(treename);
    }
    else{
    t = new TTree(treename,treename);
    }
    if(t->GetBranch("allContainerStackVectors")){ //branch does not exist yet
      bool temp=csv_makelist;
      csv_makelist=false;
      container1DStackVector * csv = this;
      t->SetBranchAddress("allContainerStackVectors",&csv);
      csv_makelist=temp;
    }
    else{
      t->Branch("allContainerStackVectors",this);
      std::cout << "container1DStackVector::writeAllToTFile: added branch" << std::endl;
    }
    t->Fill();
    t->Write("",TObject::kOverwrite);
    delete t;
    
    TDirectory * d = f->mkdir(name + "_ratio",name + "_ratio");
    d->cd();
    for(std::vector<container1DStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
      TCanvas * c=stack->makeTCanvas();
      c->Write();
      delete c;
  }
    //d.Close();
    // f.cd();
    TDirectory * d2 = f->mkdir(name,name);
    d2->cd();
    for(std::vector<container1DStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
      TCanvas * c2=stack->makeTCanvas(false);
      c2->Write();
    delete c2;
    }
  }
  
  
  void container1DStackVector::loadFromTree(TTree * t, TString name){
    
    container1DStackVector * csv=0;
    t->SetBranchAddress("allContainerStackVectors", &csv);
    bool found=false;
    for(float n=0;n<t->GetEntries();n++){
      t->GetEntry(n);
      if(csv->getName() == name){
	*this=*csv; //copy
	found=true;
	break;
      }
    }
    if(!found) std::cout << "container1DStackVector::loadFromTree: " << name << " not found in tree " << t->GetName() << std::endl;
  }
  void container1DStackVector::listAllInTree(TTree * t){
    container1DStackVector * csv=0;
    t->SetBranchAddress("allContainerStackVectors", &csv);
    for(float n=0;n<t->GetEntries();n++){
      t->GetEntry(n);
      std::cout << csv->getName() << std::endl;
    }
  }

}
