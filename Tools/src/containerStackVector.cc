#include "../interface/containerStackVector.h"
#include <omp.h>
#include "../interface/fileReader.h"
#include "../interface/indexMap.h"

#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "../interface/plotterControlPlot.h"
#include "../interface/plotterMultiplePlots.h"
#include "TStyle.h"

bool ztop::container1D::debug =false;

namespace ztop{



std::vector<containerStackVector*> containerStackVector::csv_list;
bool containerStackVector::csv_makelist=false;
bool containerStackVector::debug=false;
bool containerStackVector::fastadd=false;
int containerStackVector::treesplitlevel=99;

containerStackVector::containerStackVector(){
	if(csv_makelist)csv_list.push_back(this);
}
containerStackVector::containerStackVector(TString Name){
	name_=Name;
	if(csv_makelist)csv_list.push_back(this);
}
containerStackVector::~containerStackVector(){
	stacks_.clear();
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

std::vector<TString> containerStackVector::getStackNames(bool withoutstep){
	fileReader fr;
	fr.setComment(" step ");
	std::vector<TString> out;
	indexMap<std::string> used;
	for(std::vector<ztop::containerStack>::iterator stack=stacks_.begin();stack<stacks_.end();++stack){
		TString tmptst=stack->getName();
		std::string tmp=tmptst.Data();
		if(withoutstep) fr.trimcomments(tmp);
		if(withoutstep && used.getIndex(tmp) < used.size()){ // not new
			//do nothing
		}
		else{
			out.push_back((TString) tmp);
			used.push_back(tmp);
		}
	}
	return out;
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
			s->push_back(container, leg, color, norm,legor);
			found=true;
			break;
		}
	}
	if(!found){
		if(container1D::debug)
			std::cout << "containerStackVector::add: creating new stack " << container.getName()  <<std::endl;
		ztop::containerStack newstack(container.getName());
		newstack.push_back(container, leg, color, norm,legor);
		stacks_.push_back(newstack);
	}
}
void containerStackVector::add(ztop::container2D & container, TString leg , int color , double norm,int legor){
	bool found=false;
	for(std::vector<ztop::containerStack>::iterator s=stacks_.begin();s<stacks_.end();++s){
		if(s->getName() == container.getName()){
			if(container1D::debug)
				std::cout << "containerStackVector::add: adding " << s->getName() << " to existing stack"  <<std::endl;
			s->push_back(container, leg, color, norm,legor);
			found=true;
			break;
		}
	}
	if(!found){
		if(container1D::debug)
			std::cout << "containerStackVector::add: creating new stack " << container.getName()  <<std::endl;
		ztop::containerStack newstack(container.getName());
		newstack.push_back(container, leg, color, norm,legor);
		stacks_.push_back(newstack);
	}
}
void containerStackVector::add(ztop::container1DUnfold & container, TString leg , int color , double norm,int legor){
	bool found=false;
	for(std::vector<ztop::containerStack>::iterator s=stacks_.begin();s<stacks_.end();++s){
		if(s->getName() == container.getName()){
			if(container1D::debug)
				std::cout << "containerStackVector::add: adding " << s->getName() << " to existing stack"  <<std::endl;
			s->push_back(container, leg, color, norm,legor);
			found=true;
			break;
		}
	}
	if(!found){
		if(container1D::debug)
			std::cout << "containerStackVector::add: creating new stack " << container.getName()  <<std::endl;
		ztop::containerStack newstack(container.getName());
		newstack.push_back(container, leg, color, norm,legor);
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








const ztop::containerStack& containerStackVector::getStack(const TString& name,size_t startidx)const{
	//try same index first
	if(startidx<stacks_.size()){
		if(name == stacks_.at(startidx).getName()){
			return stacks_.at(startidx);
		}
	}

	std::vector<ztop::containerStack>::const_iterator it=stacks_.end();
	bool abort = false;
#pragma omp parallel for
	for(std::vector<ztop::containerStack>::const_iterator s=stacks_.begin();s<stacks_.end();++s){
#pragma omp flush (abort)
		if (!abort) {
			if(name == s->getName()){
#pragma omp critical (containerStackVector_searchstack_const)
				{
					it=s;
					abort=true;
#pragma omp flush (abort)
				}
			}
		}
	}
	if(it == stacks_.end()){
		throw std::out_of_range("containerStackVector::getStack: stack not found");
	}
	return *it;
}

ztop::containerStack& containerStackVector::getStack(const TString& name,size_t startidx){
	if(startidx<stacks_.size()){
		if(name == stacks_.at(startidx).getName()){
			return stacks_.at(startidx);
		}
	}
	std::vector<ztop::containerStack>::iterator it=stacks_.end();
	bool abort = false;
#pragma omp parallel for
	for(std::vector<ztop::containerStack>::iterator s=stacks_.begin();s<stacks_.end();++s){
#pragma omp flush (abort)
		if (!abort) {
			if(name == s->getName()){
#pragma omp critical (containerStackVector_searchstack)
				{
					it=s;
					abort=true;
#pragma omp flush (abort)
				}
			}
		}
	}
	if(it == stacks_.end()){
		throw std::out_of_range("containerStackVector::getStack: stack not found");
	}
	return *it;
}


void containerStackVector::removeContribution(TString contribution){
	for(std::vector<ztop::containerStack>::iterator stack=stacks_.begin(); stack < stacks_.end(); ++stack){
		stack->removeContribution(contribution);
	}
}

void containerStackVector::addMCErrorStackVector(const TString &sysname, const ztop::containerStackVector & stackvec){
	addErrorStackVector(sysname , stackvec);
}
void containerStackVector::addErrorStackVector(const TString &sysname,const  ztop::containerStackVector & stackvec){
#pragma omp parallel for
	for(std::vector<containerStack>::iterator istack=stacks_.begin();istack<stacks_.end(); ++istack){
		try{
			istack->addMCErrorStack(sysname,stackvec.getStack(istack->getName(),istack-stacks_.begin()));
		}catch(...){}
	}
}

void containerStackVector::addMCErrorStackVector(const ztop::containerStackVector& stackvec){
	addMCErrorStackVector(stackvec.getSyst(), stackvec);
}
void containerStackVector::addGlobalRelMCError(TString sysname,double error){
#pragma omp parallel for
	for(std::vector<containerStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
		stack->addGlobalRelMCError(sysname,error);
	}
}
void containerStackVector::getRelSystematicsFrom(const ztop::containerStackVector& stackvec){
#pragma omp parallel for
	for(std::vector<containerStack>::iterator istack=stacks_.begin();istack<stacks_.end(); ++istack){
		try{
			const containerStack & stack=stackvec.getStack(istack->getName(),istack-stacks_.begin());
			istack->getRelSystematicsFrom(stack);
		}catch(...){}
	}
}
void containerStackVector::addRelSystematicsFrom(const ztop::containerStackVector& stackvec,bool ignorestat,bool strict){
#pragma omp parallel for
	for(std::vector<containerStack>::iterator istack=stacks_.begin();istack<stacks_.end(); ++istack){
		try{
			const containerStack & stack=stackvec.getStack(istack->getName(),istack-stacks_.begin());
			istack->addRelSystematicsFrom(stack);
		}catch(...){
			std::cout << "containerStackVector::addRelSystematicsFrom: Stack " << istack->getName() << " not found." <<std::endl;
		}
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

std::vector<size_t> containerStackVector::removeSpikes(bool inclUFOF,
		int limittoindex,float strength,float sign,float threshold){
	std::vector<size_t> out,temp;
	for(std::vector<containerStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
		temp=stack->removeSpikes(inclUFOF,limittoindex,strength,sign,threshold);
		out.insert(out.end(),temp.begin(),temp.end());
	}
	return out;
}


void containerStackVector::multiplyNorm(TString legendname, double multi, TString step){
	for(std::vector<containerStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
		if(stack->getName().Contains(step)){
			stack->multiplyNorm(legendname, multi);

		}
	}
}

void containerStackVector::multiplyNorms(TString legendname, std::vector<double> scalefactors, std::vector<TString> identifier,bool showmessages){
	if((identifier.size() != scalefactors.size())){
		std::cout << "containerStackVector::multiplyNorms: identifiers and scalefactors must be same size!" << std::endl;
	}
	else{
		unsigned int count=0;
		for(std::vector<containerStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
			for(unsigned int i=0;i<scalefactors.size();i++){
				if(stack->getName().EndsWith(identifier.at(i))){
					if(showmessages) std::cout << "rescaling " << stack->getName() << "  " <<legendname << " by " << scalefactors[i] << std::endl;
					stack->multiplyNorm(legendname, scalefactors.at(i));
					if(showmessages) std::cout << "rescaling done" << std::endl;
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

void containerStackVector::writeAllToTFile(TString filename, bool recreate, bool onlydata,TString treename){
	if(debug)
		std::cout << "containerStackVector::writeAllToTFile(TString filename, bool recreate, TString treename)" << std::endl;
#pragma omp critical (containerStackVector_writeAllToTFile)
	{
		AutoLibraryLoader::enable();
		TH1::AddDirectory(false);
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
			t = new TTree(treename,treename,treesplitlevel);
		}
		if(t){
			if(debug)
				std::cout << "containerStackVector::writeAllToTFile: created/opened file and tree" << std::endl;
		}
		else{
			std::cout << "containerStackVector::writeAllToTFile: creating/opening tree failed" << std::endl;
		}


		if(t->GetBranch("containerStackVectors")){ //branch does  exist
			if(debug) std::cout << "containerStackVector::writeAllToTFile: opened branch" << std::endl;
			bool temp=csv_makelist;
			csv_makelist=false;
			containerStackVector * csv = this;
			t->SetBranchAddress("containerStackVectors",&csv);
			csv_makelist=temp;
		}
		else{
			t->Branch("containerStackVectors",this);
			if(debug) std::cout << "containerStackVector::writeAllToTFile: added branch" << std::endl;
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

		if(onlydata){
			f->Close();
			delete f;

		}
		else{
			bool batch=containerStack::batchmode;
			containerStack::batchmode=true;
			if(debug)
				std::cout << "containerStackVector::writeAllToTFile: preparing plots" << std::endl;

			TDirectory * d = f->mkdir(name + "_ratio",name + "_ratio");
			d->cd();

			//use a file reader to strip stuff
			fileReader fr;
			fr.setComment(" step ");
			indexMap<std::string> dirs;
			std::vector<TDirectory *> dirpv;

			plotterControlPlot pl;
			std::string cmsswbase=getenv("CMSSW_BASE");
			pl.readStyleFromFile(cmsswbase+"/src/TtZAnalysis/Tools/styles/controlPlots_standard.txt");

			gStyle->SetOptStat(0);

			for(std::vector<containerStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){

				if(stack->is1DUnfold()){
					TDirectory * din=d->mkdir(stack->getName(),stack->getName());
					din->cd();
					TCanvas *c = new TCanvas(stack->getName());
					pl.setTitle(stack->getName());
					pl.usePad(c);
					pl.setStack(&*stack);
					pl.draw();

					/* old impl
                        TCanvas * c=stack->makeTCanvas();
					 */
					if(c){
						//tdir->WriteObject(c,c->GetName());
						c->Write();
						// stack->cleanMem();
						delete c;
					}
					pl.cleanMem();
					container1DUnfold cuf=stack->getSignalContainer1DUnfold();
					containerStack rebinned=stack->rebinXToBinning(cuf.getGenBins());
					rebinned.setName(stack->getName()+"_genbins");
					c = new TCanvas(rebinned.getName());
					pl.setTitle(rebinned.getName());
					pl.usePad(c);
					pl.setStack(&rebinned);
					pl.draw();

					/* old impl
                       TCanvas * c=stack->makeTCanvas();
					 */
					if(c){
						//tdir->WriteObject(c,c->GetName());
						c->Write();
						// stack->cleanMem();
						delete c;
					}
					pl.cleanMem();

					cuf.checkCongruentBinBoundariesXY();
					TH1D * pur=cuf.getPurity().getTH1D("purity",false,false,false);
					TH1D * stab=cuf.getStability().getTH1D("stability",false,false,false);
					if(pur){
						c =new TCanvas("purity_stab","purity_stab");
						pur->SetMarkerColor(kBlue);
						stab->SetMarkerColor(kRed); //put to plotter after testing
						pur->GetYaxis()->SetRangeUser(0,1);
						pur->Draw();
						stab->Draw("same");
						c->Write();
						delete c;
						delete pur;
						delete stab;
					}
					c =new TCanvas("MResp","MResp");
					TH2D * resp=cuf.getResponseMatrix().getTH2D("respMatrix_nominal",false,true);
					resp->Draw("colz");
					c->Write();
					delete c;
					delete resp;
					c =new TCanvas("Gen","Gen");
					container1D gen= cuf.getGenContainer();
					plotterMultiplePlots plgen;
					plgen.readStyleFromFileInCMSSW("/src/TtZAnalysis/Tools/styles/multiplePlots.txt");
					plgen.usePad(c);
					plgen.addPlot(&gen,true);
					plgen.draw();
					c->Write();
					delete c;
					TDirectory * dsys=din->mkdir("sys","sys");
					dsys->cd();
					for(size_t i=0;i<cuf.getSystSize();i++){
						c =new TCanvas("MResp_"+cuf.getSystErrorName(i),"MResp_"+cuf.getSystErrorName(i));
						resp=cuf.getResponseMatrix().getTH2DSyst("respMatrix_"+cuf.getSystErrorName(i),i,false,true);;
						resp->Draw("colz");
						c->Write();
						stack->cleanMem();
						delete c;
						delete resp;
					}
					d->cd();
				}
				else if(stack->is1D()){
					std::string dirname=stack->getName().Data();
					fr.trimcomments(dirname);
					size_t diridx=dirs.getIndex(dirname);
					TDirectory * tdir=0;
					if(diridx>=dirs.size()){ //new dir
						tdir=d->mkdir(dirname.data(),dirname.data());
						dirpv.push_back(tdir);
						dirs.push_back(dirname);
					}
					//d->mkdir(); //cd dir with idx
					tdir=dirpv.at(diridx);
					tdir->cd();
					//containerStack::debug=true;
					// plotterControlPlot::debug=true;
					TCanvas *c = new TCanvas(stack->getName());
					pl.setTitle(stack->getName());
					pl.usePad(c);
					pl.setStack(&*stack);
					pl.draw();

					/* old impl
            TCanvas * c=stack->makeTCanvas();
					 */
					if(c){
						//tdir->WriteObject(c,c->GetName());
						c->Write();
						// stack->cleanMem();
						delete c;
					}
					pl.cleanMem();
					d->cd();
				}
				else if(stack->is2D()){
					//make extra 2D directory
					TDirectory * dir2d=0;
					std::string dir2dname="2D_plots";
					size_t dir2didx=dirs.getIndex(dir2dname);
					if(dir2didx >= dirs.size()){
						dir2d=d->mkdir(dir2dname.data(),dir2dname.data());
						dirpv.push_back(dir2d);
						dirs.push_back(dir2dname);
					}
					dir2d=dirpv.at(dir2didx);

					//everything here is pretty bad style
					std::string dirname=stack->getName().Data();

					fr.trimcomments(dirname);

					std::string stepname((std::string)stack->getName(),dirname.length());

					size_t diridx=dirs.getIndex(dirname);
					TDirectory * tdir=0;
					if(diridx>=dirs.size()){ //new dir
						tdir=dir2d->mkdir(dirname.data(),dirname.data());
						dirpv.push_back(tdir);
						dirs.push_back(dirname);
					}
					//d->mkdir(); //cd dir with idx
					tdir=dirpv.at(diridx);
					tdir->cd();
					fr.trim(stepname);

					TDirectory * stdir=tdir->mkdir(stepname.data(),stepname.data());
					stdir->cd();

					for(size_t plot=0;plot<stack->size();plot++){

						///replace with proper plotting tool at some point
						container2D  c2d= stack->getContainer2D(plot);

						TH2D * th2d=c2d.getTH2D(stack->getLegend(plot),true,true);
						if(th2d){

							TString canvasname=stack->getLegend(plot);
							canvasname+=+"_"+stack->getName();
							TCanvas *  c= new TCanvas(canvasname);
							th2d->Draw("colz");
							c->Write(canvasname);

							delete c;
							delete th2d;
						}
					}

					//in addition for signal and BG summed
					container2D  c2d= stack->getSignalContainer2D();
					TH2D * th2d=c2d.getTH2D("signal",true,true);
					if(th2d){
						TCanvas *  c= new TCanvas("signal_"+stack->getName());
						th2d->Draw("colz");
						c->Write("signal_"+stack->getName());

						delete c;
						delete th2d;
					}
					c2d= stack->getBackgroundContainer2D();
					th2d=c2d.getTH2D("background",true,true);
					if(th2d){
						TCanvas *  c= new TCanvas("background_"+stack->getName());
						th2d->Draw("colz");
						c->Write("background_"+stack->getName());

						delete c;
						delete th2d;
					}

					d->cd();
				}
			}
			if(debug)
				std::cout << "containerStackVector::writeAllToTFile: ratio plots drawn" << std::endl;


			f->Close();
			delete f;
			containerStack::batchmode=batch;
		}//not onlydata done
		if(debug)
			std::cout << "containerStackVector::writeAllToTFile: finished" << std::endl;
	}
}


void containerStackVector::printAll(TString namestartswith,TString directory,TString extension){
	for(std::vector<containerStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
		if(stack->is1D() || stack->is1DUnfold() ){
			if(namestartswith=="" || stack->getName().BeginsWith(namestartswith)){
				TCanvas * c=stack->makeTCanvas(containerStack::plotmode::ratio);
				if(c){
					TString newname=stack->getName();
					newname.ReplaceAll(" ","_");
#pragma omp critical (containerStackVector_printAll)
					{
						c->Print(directory+newname+extension);
						stack->cleanMem();

						delete c;
					}
				}
			}
		}
	}
}

void containerStackVector::writeAllToTFile(TFile * f, TString treename){
	AutoLibraryLoader::enable();
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
		t = new TTree(treename,treename,treesplitlevel);
	}
	if(t->GetBranch("containerStackVectors")){ //branch does exist
		bool temp=csv_makelist;
		csv_makelist=false;
		containerStackVector * csv = this;
		t->SetBranchAddress("containerStackVectors",&csv);
		csv_makelist=temp;
	}
	else{
		t->Branch("containerStackVectors",this);
		if(debug)std::cout << "containerStackVector::writeAllToTFile: added branch" << std::endl;
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
			stack->cleanMem();
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
			stack->cleanMem();
			delete c2;
		}
	}

	containerStack::batchmode=batch;

}

//only for compat reasons
void containerStackVector::loadFromTree(TTree * t, const TString& name){
	AutoLibraryLoader::enable();
	getFromTree(t,name);

}

containerStackVector * containerStackVector::getFromTree(TTree * t, const TString& name){
#pragma omp critical (containerStackVector_getFromTree)
	{
		AutoLibraryLoader::enable();
		if(!t || t->IsZombie()){
			throw std::runtime_error("containerStackVector::loadFromTree: tree not ok");
		}
		ztop::containerStackVector * cuftemp=0; //new containerStackVector(); //shouldn't make sense.. but seems to decrease TBasket memleaks
		if(!t->GetBranch("containerStackVectors")){
			throw std::runtime_error("containerStackVector::loadFromTree: branch containerStackVectors not found");
		}
		bool found=false;
		size_t count=0;
		t->SetBranchAddress("containerStackVectors", &cuftemp);
		if(name == "*" && t->GetEntries()>1){
			throw std::runtime_error("containerStackVector::getFromTree: No csv name specified but more than one object in file");
		}
		for(float n=0;n<t->GetEntries();n++){
			t->GetEntry(n);
			if(name == "*" || cuftemp->getName()==(name)){
				found=true;
				count++;
				*this=*cuftemp;
			}
		}
		delete cuftemp;
		if(!found){
			throw std::runtime_error("containerStackVector::loadFromTree: no containerStackVector with name not found");
		}
		if(count>1){
			std::cout << "containerStackVector::loadFromTree: found more than one object with name "
					<< getName() << ", took the first one." << std::endl;
		}
	}
	return this;

}
void containerStackVector::loadFromTFile(TFile * f,const TString& csvname,TString treename){
	if(!f || f->IsZombie()){
		throw std::runtime_error("containerStackVector::loadFromTFile: file not ok.");
	}
	f->cd();
	TTree * ttemp = (TTree*)f->Get(treename);
	loadFromTree(ttemp,csvname);
	delete ttemp;
}
void containerStackVector::loadFromTFile(const TString& filename,const TString& csvname,TString treename){
#pragma omp critical (containerStackVector_loadFromTFile)
	{
		AutoLibraryLoader::enable(); //to avoid warnings
		TFile * ftemp=new TFile(filename,"read");
		loadFromTFile(ftemp,csvname,treename);
		ftemp->Close();
		delete ftemp;
	}
}


void containerStackVector::listAllInTree(TTree * t){
	AutoLibraryLoader::enable();
	containerStackVector * csv=0;
	t->SetBranchAddress("containerStackVectors", &csv);
	for(float n=0;n<t->GetEntries();n++){
		t->GetEntry(n);
		std::cout << csv->getName() << std::endl;
	}
	delete csv;
}



}
