#include "../interface/containerStackVector.h"
#include <omp.h>

bool ztop::container1D::debug =false;

namespace ztop{


std::vector<containerStackVector*> containerStackVector::csv_list;
bool containerStackVector::csv_makelist=false;
bool containerStackVector::debug=false;
bool containerStackVector::fastadd=true;

containerStackVector::containerStackVector(){
	if(csv_makelist)csv_list.push_back(this);
}
containerStackVector::containerStackVector(TString Name){
	name_=Name;
	if(csv_makelist)csv_list.push_back(this);
}
containerStackVector::~containerStackVector(){
	for(unsigned int i=0;i<csv_list.size();i++){
		if(csv_list[i] == this) csv_list.erase(csv_list.begin()+i);
		break;
	}
}
void containerStackVector::listStacks(){
	for(std::vector<ztop::containerStack>::iterator stack=stacks_.begin();stack<stacks_.end();++stack){
		std::cout << stack->getName() << std::endl;
	}
}

void containerStackVector::setLegendOrder(TString leg, size_t no){
	for(std::vector<ztop::containerStack>::iterator s=stacks_.begin();s<stacks_.end();++s){
		s->setLegendOrder(leg,no);
	}
}

void containerStackVector::add(ztop::container1D & container, TString leg , int color , double norm,int legor){
	bool found=false;
	for(std::vector<ztop::containerStack>::iterator s=stacks_.begin();s<stacks_.end();++s){
		if(s->getName() == container.getName()){
			if(container1D::debug)
				std::cout << "containerStackVector::add: adding " << s->getName() << " to existing stack"  <<std::endl;
			s->push_back(container, leg, color, norm);
			if(legor>-1)
				s->setLegendOrder(leg,(size_t)legor);
			found=true;
			break;
		}
	}
	if(!found){
		if(container1D::debug)
			std::cout << "containerStackVector::add: creating new stack " << container.getName()  <<std::endl;
		ztop::containerStack newstack(container.getName());
		newstack.push_back(container, leg, color, norm);
		if(legor>-1)
			newstack.setLegendOrder(leg,(size_t)legor);
		stacks_.push_back(newstack);
	}
}
void containerStackVector::add(ztop::container2D & container, TString leg , int color , double norm,int legor){
	bool found=false;
	for(std::vector<ztop::containerStack>::iterator s=stacks_.begin();s<stacks_.end();++s){
		if(s->getName() == container.getName()){
			if(container1D::debug)
				std::cout << "containerStackVector::add: adding " << s->getName() << " to existing stack"  <<std::endl;
			s->push_back(container, leg, color, norm);
			if(legor>-1)
				s->setLegendOrder(leg,(size_t)legor);
			found=true;
			break;
		}
	}
	if(!found){
		if(container1D::debug)
			std::cout << "containerStackVector::add: creating new stack " << container.getName()  <<std::endl;
		ztop::containerStack newstack(container.getName());
		newstack.push_back(container, leg, color, norm);
		if(legor>-1)
			newstack.setLegendOrder(leg,(size_t)legor);
		stacks_.push_back(newstack);
	}
}
void containerStackVector::add(ztop::container1DUnfold & container, TString leg , int color , double norm,int legor){
	bool found=false;
	for(std::vector<ztop::containerStack>::iterator s=stacks_.begin();s<stacks_.end();++s){
		if(s->getName() == container.getName()){
			if(container1D::debug)
				std::cout << "containerStackVector::add: adding " << s->getName() << " to existing stack"  <<std::endl;
			s->push_back(container, leg, color, norm);
			if(legor>-1)
				s->setLegendOrder(leg,(size_t)legor);
			found=true;
			break;
		}
	}
	if(!found){
		if(container1D::debug)
			std::cout << "containerStackVector::add: creating new stack " << container.getName()  <<std::endl;
		ztop::containerStack newstack(container.getName());
		newstack.push_back(container, leg, color, norm);
		if(legor>-1)
			newstack.setLegendOrder(leg,(size_t)legor);
		stacks_.push_back(newstack);
	}
}

void containerStackVector::addList(TString leg, int color, double norm,int legor){
	for(unsigned int i=0;i<container1D::c_list.size();i++){
		add(*container1D::c_list[i],leg,color,norm,legor);
	}
}
void containerStackVector::addList2D(TString leg, int color, double norm,int legor){
	for(unsigned int i=0;i<container2D::c_list.size();i++){
		add(*container2D::c_list[i],leg,color,norm,legor);
	}
}
void containerStackVector::addList1DUnfold(TString leg, int color, double norm,int legor){
	for(unsigned int i=0;i<container1DUnfold::c_list.size();i++){
		container1DUnfold::c_list[i]->flush();
		add(*container1DUnfold::c_list[i],leg,color,norm,legor);
	}
}


/**
 * call AFTER!!! adding new stacks
 */
void containerStackVector::addSignal(const TString & signame){
	size_t size=stacks_.size();
#pragma omp parallel for
	for(size_t i=0;i<size;i++){
		stacks_.at(i).addSignal(signame);
	}
}








ztop::containerStack containerStackVector::getStack(TString name){
	ztop::containerStack defout("DUMMY");
	for(std::vector<ztop::containerStack>::iterator s=stacks_.begin();s<stacks_.end();++s){
		if(name == s->getName()){
			return *s;
		}
	}
	std::cout << "containerStackVector::getStack: "<< name << " not found. returning empty Stack" << std::endl;
	return defout;
}
void containerStackVector::removeContribution(TString contribution){
	for(std::vector<ztop::containerStack>::iterator stack=stacks_.begin(); stack < stacks_.end(); ++stack){
		stack->removeContribution(contribution);
	}
}

void containerStackVector::addMCErrorStackVector(TString sysname, ztop::containerStackVector stackvec){
	if(!fastadd){
		for(std::vector<containerStack>::iterator istack=stacks_.begin();istack<stacks_.end(); ++istack){
			for(std::vector<containerStack>::iterator estack=stackvec.stacks_.begin();estack<stackvec.stacks_.end(); ++estack){
				if(istack->getName() == estack->getName()){
					istack->addMCErrorStack(sysname,*estack);
					break;
				}
			}
		}
	}
	else{//fastadd requires same ordering of all stacks (usually the case)
		for(size_t i=0;i<stacks_.size();i++)
			stacks_.at(i).addMCErrorStack(sysname,stackvec.stacks_.at(i));
	}
}
void containerStackVector::addMCErrorStackVector(ztop::containerStackVector stackvec){
	addMCErrorStackVector(stackvec.getSyst(), stackvec);
}
void containerStackVector::addGlobalRelMCError(TString sysname,double error){
	for(std::vector<containerStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
		stack->addGlobalRelMCError(sysname,error);
	}
}
void containerStackVector::addRelSystematicsFrom(ztop::containerStackVector stackvec){
	if(!fastadd){
		for(std::vector<containerStack>::iterator istack=stacks_.begin();istack<stacks_.end(); ++istack){
			for(std::vector<containerStack>::iterator estack=stackvec.stacks_.begin();estack<stackvec.stacks_.end(); ++estack){
				if(istack->getName() == estack->getName()){
					istack->addRelSystematicsFrom(*estack);
					break;
				}
			}
		}
	}
	else{//fastadd requires same ordering of all stacks (usually the case)
		for(size_t i=0;i<stacks_.size();i++)
			stacks_.at(i).addRelSystematicsFrom(stackvec.stacks_.at(i));
	}
}

void containerStackVector::removeError(TString name){
	for(std::vector<containerStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
		stack->removeError(name);
	}
}
void containerStackVector::renameSyst(TString old, TString New){
	for(std::vector<containerStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
		stack->renameSyst(old,New);
	}
}

void containerStackVector::multiplyNorm(TString legendname, double multi, TString step){
	for(std::vector<containerStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
		if(stack->getName().Contains(step)){
			stack->multiplyNorm(legendname, multi);

		}
	}
}

void containerStackVector::multiplyNorms(TString legendname, std::vector<double> scalefactors, std::vector<TString> identifier,bool showmessages){
	if(! (identifier.size() == scalefactors.size())){
		std::cout << "containerStackVector::multiplyNorms: identifiers and scalefactors must be same size!" << std::endl;
	}
	else{
		unsigned int count=0;
		for(std::vector<containerStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
			for(unsigned int i=0;i<scalefactors.size();i++){
				if(stack->getName().Contains(identifier[i])){
					stack->multiplyNorm(legendname, scalefactors[i]);
					if(showmessages) std::cout << "rescaling " << stack->getName() << "  " <<legendname << " by " << scalefactors[i] << " for " << std::endl;
					count++;
				}
			}
		}
		if(count < identifier.size()) std::cout << "containerStackVector::multiplyNorms: warning: not all identifiers found!" << std::endl;
		else if(count > identifier.size() && showmessages) std::cout << "containerStackVector::multiplyNorms: warning: identifiers where ambiguous! Scaled more than one stack per identifier (intended?)" << std::endl;
	}
}

void containerStackVector::multiplyAllMCNorms(double multiplier){
	for(std::vector<containerStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
		stack->multiplyAllMCNorms(multiplier);
	}
}

void containerStackVector::writeAllToTFile(TString filename, bool recreate, TString treename){
	if(debug)
		std::cout << "containerStackVector::writeAllToTFile(TString filename, bool recreate, TString treename)" << std::endl;

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
	if(t->GetBranch("allContainerStackVectors")){ //branch does  exist
		bool temp=csv_makelist;
		csv_makelist=false;
		containerStackVector * csv = this;
		t->SetBranchAddress("allContainerStackVectors",&csv);
		csv_makelist=temp;
	}
	else{
		t->Branch("allContainerStackVectors",this);
		std::cout << "containerStackVector::writeAllToTFile: added branch" << std::endl;
	}
	if(debug)
		std::cout << "containerStackVector::writeAllToTFile: got branch" << std::endl;

	t->Fill();
	if(debug)
		std::cout << "containerStackVector::writeAllToTFile: filled branch" << std::endl;

	t->Write("",TObject::kOverwrite);
	if(debug)
		std::cout << "containerStackVector::writeAllToTFile: written branch" << std::endl;

	delete t;

	bool batch=containerStack::batchmode;
	containerStack::batchmode=true;
	if(debug)
		std::cout << "containerStackVector::writeAllToTFile: preparing plots" << std::endl;

	TDirectory * d = f->mkdir(name + "_ratio",name + "_ratio");
	d->cd();
	for(std::vector<containerStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
		TCanvas * c=stack->makeTCanvas();
		if(c){
			c->Write();
			delete c;
		}
	}
	if(debug)
		std::cout << "containerStackVector::writeAllToTFile: ratio plots drawn" << std::endl;

	TDirectory * d2 = f->mkdir(name+ "_sgbg",name+ "_sgbg");
	d2->cd();
	for(std::vector<containerStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
		TCanvas * c2=stack->makeTCanvas(containerStack::plotmode::sigbg);
		if(c2){
			c2->Write();
			delete c2;
		}
	}
	if(debug)
		std::cout << "containerStackVector::writeAllToTFile: normal plots drawn" << std::endl;


	f->Close();
	delete f;

	containerStack::batchmode=batch;
	if(debug)
		std::cout << "containerStackVector::writeAllToTFile: finished" << std::endl;

}

void containerStackVector::writeAllToTFile(TFile * f, TString treename){
	TString name;
	if(name_=="") name="no_name";
	else name = name_;

	TH1::AddDirectory(kFALSE);

	f->cd();
	TTree * t=0;
	if(f->Get(treename)){
		t = (TTree*) f->Get(treename);
	}
	else{
		t = new TTree(treename,treename);
	}
	if(t->GetBranch("allContainerStackVectors")){ //branch does exist
		bool temp=csv_makelist;
		csv_makelist=false;
		containerStackVector * csv = this;
		t->SetBranchAddress("allContainerStackVectors",&csv);
		csv_makelist=temp;
	}
	else{
		t->Branch("allContainerStackVectors",this);
		std::cout << "containerStackVector::writeAllToTFile: added branch" << std::endl;
	}
	t->Fill();
	t->Write("",TObject::kOverwrite);
	delete t;

	bool batch=containerStack::batchmode;
	containerStack::batchmode=true;


	TDirectory * d = f->mkdir(name + "_ratio",name + "_ratio");
	d->cd();
	for(std::vector<containerStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
		TCanvas * c=stack->makeTCanvas();
		if(c){
			c->Write();
			delete c;
		}
	}
	//d.Close();
	// f.cd();
	TDirectory * d2 = f->mkdir(name+ "_sgbg",name+ "_sgbg");
	d2->cd();
	for(std::vector<containerStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
		TCanvas * c2=stack->makeTCanvas(containerStack::plotmode::sigbg);
		if(c2){
			c2->Write();
			delete c2;
		}
	}

	containerStack::batchmode=batch;

}


void containerStackVector::loadFromTree(TTree * t, TString name){
	/*
    containerStackVector * csv=0;
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
    if(!found) std::cout << "containerStackVector::loadFromTree: " << name << " not found in tree " << t->GetName() << std::endl;
	 */
	containerStackVector csv;
	if(csv.getFromTree(t,name))
		*this=*(csv.getFromTree(t,name));
}

containerStackVector * containerStackVector::getFromTree(TTree * t, TString name){
	containerStackVector * csv=0;
	bool found=false;
	t->SetBranchAddress("allContainerStackVectors", &csv);
	for(float n=0;n<t->GetEntries();n++){
		t->GetEntry(n);
		TString vname=toTString(csv->getName());
		// std::cout << "check name: (" << vname << ") searching for: ("<< name<< ")"<<std::endl;
		//  vname=name;
		//std::cout << name.CompareTo(toTString(vname)) << std::endl;
		if(vname == name){
			//this=csv;
			//	std::cout << "found " << name << std::endl;
			return csv;
			found=true;
			break;
		}
	}
	if(!found){
		std::cout << "containerStackVector::getFromTree: " << name << " not found in tree " << t->GetName() << "\nreturning 0 pointer!" <<std::endl;
	}
	return 0;
}


void containerStackVector::listAllInTree(TTree * t){
	containerStackVector * csv=0;
	t->SetBranchAddress("allContainerStackVectors", &csv);
	for(float n=0;n<t->GetEntries();n++){
		t->GetEntry(n);
		std::cout << csv->getName() << std::endl;
	}
}

}
