#ifndef containerStackVector_h
#define containerStackVector_h

#include "containerStack.h"
#include <vector>
#include "TStyle.h"
#include "TDirectory.h"

// class to handle containerstacks and to add systematics to the MC contributions etc.
// writes plots with and without ratio to TFiles


namespace top{

  class container1DStackVector{

  public:
    container1DStackVector(){}
    container1DStackVector(TString Name){name_=Name;}
    ~container1DStackVector(){}
    void setName(TString Name){name_=Name;}
    void add(top::container1D &, TString, int, double);
    void addList(top::List<top::container1D> * , TString, int, double);
    std::vector<top::container1DStack> getVector(){return stacks_;}
    top::container1DStack getStack(TString);
    top::container1DStack getStack(unsigned int n){return stacks_[n];}

    void addMCErrorStackVector(TString,top::container1DStackVector, bool ignoreMCStat=true);
    void addGlobalRelMCError(TString,double);
    void removeError(TString);

    void multiplyNorms(TString, std::vector<double>, std::vector<TString>);   //! multiplies norm of all MC with legendname  ,  with factor  ,  for step identifier string

    void writeAllToTFile(TString, bool recreate=false);

    void clear(){stacks_.clear();name_="";}

  private:
    TString name_;
    std::vector<top::container1DStack> stacks_;



  };

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

  void container1DStackVector::addList(top::List<top::container1D> * list, TString leg, int color, double norm){
    for(unsigned int i=0;i<list->getList().size();i++){
      add(*(list->getList()[i]),leg,color,norm);
    }
  }

  top::container1DStack container1DStackVector::getStack(TString name){
    top::container1DStack defout("DUMMY");
    for(std::vector<top::container1DStack>::iterator s=stacks_.begin();s<stacks_.end();++s){
      if(name == s->getName()){
        return *s;
      }
    }
    return defout;
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
  void container1DStackVector::removeError(TString name){
    for(std::vector<container1DStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
      stack->removeError(name);
    }
  }
  void container1DStackVector::multiplyNorms(TString legendname, std::vector<double> scalefactors, std::vector<TString> identifier){
    if(! (identifier.size() == scalefactors.size())){
      std::cout << "container1DStackVector::multiplyNorms: identifiers and scalefactors must be same size!" << std::endl;
    }
    else{
      unsigned int count=0;
      for(std::vector<container1DStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
	for(unsigned int i=0;i<scalefactors.size();i++){
	  if(stack->getName().Contains(identifier[i])){
	    stack->multiplyNorm(legendname, scalefactors[i]);
	    count++;
	  }
	}
      }
      if(count < identifier.size()) std::cout << "container1DStackVector::multiplyNorms: warning: not all identifiers found!" << std::endl;
      else if(count > identifier.size()) std::cout << "container1DStackVector::multiplyNorms: warning: identifiers ambiguous!" << std::endl;
    }
  }

  void container1DStackVector::writeAllToTFile(TString filename, bool recreate){
    TString name;

    if(name_=="") name="no_name";
    else name = name_;

    TString upre="UPDATE";
    if(recreate == true) upre="RECREATE";

    TFile f = TFile(filename,upre);
    f.cd();
    TDirectory * d = f.mkdir(name + "_ratio",name + "_ratio");
    d->cd();
    for(std::vector<container1DStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
      TCanvas * c=stack->makeTCanvas();
      c->Write();
      delete c;
    }
    //d.Close();
    // f.cd();
    TDirectory * d2 = f.mkdir(name,name);
    d2->cd();
    for(std::vector<container1DStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
      TCanvas * c2=stack->makeTCanvas(false);
      c2->Write();
      delete c2;
    }
    f.Close();
    // delete d;
    // delete d2;
  }


}


#endif
