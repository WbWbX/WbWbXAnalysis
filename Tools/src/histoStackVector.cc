#include "../interface/histoStackVector.h"
#include <omp.h>
#include "../interface/fileReader.h"
#include "../interface/indexMap.h"

#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "../interface/plotterControlPlot.h"
#include "../interface/plotterMultiplePlots.h"
#include "TStyle.h"
#include "TRandom3.h"
#include <string>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>

bool ztop::histo1D::debug =false;

namespace ztop{



std::vector<histoStackVector*> histoStackVector::csv_list;
bool histoStackVector::csv_makelist=false;
bool histoStackVector::debug=false;
bool histoStackVector::fastadd=false;
int histoStackVector::treesplitlevel=99;

histoStackVector::histoStackVector(){
	if(csv_makelist)csv_list.push_back(this);
}
histoStackVector::histoStackVector(TString Name){
	name_=Name;
	if(csv_makelist)csv_list.push_back(this);
}
histoStackVector::~histoStackVector(){
	stacks_.clear();
	for(unsigned int i=0;i<csv_list.size();i++){
		if(csv_list[i] == this) csv_list.erase(csv_list.begin()+i);
	}
}


bool histoStackVector::operator == (const histoStackVector& rhs)const{

	if(name_!=rhs.name_)
		return false;
	if(isSyst_!=rhs.isSyst_)
		return false;
	if(tempsig_!=rhs.tempsig_)
		return false;
	if(stacks_!=rhs.stacks_)
		return false;
	return true;
}

void histoStackVector::listStacks(){
	for(std::vector<ztop::histoStack>::iterator stack=stacks_.begin();stack<stacks_.end();++stack){
		std::cout << stack->getName() << std::endl;
	}
}

std::vector<TString> histoStackVector::getStackNames(bool withoutstep){
	fileReader fr;
	fr.setComment(" step ");
	std::vector<TString> out;
	indexMap<std::string> used;
	for(std::vector<ztop::histoStack>::iterator stack=stacks_.begin();stack<stacks_.end();++stack){
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

void histoStackVector::setLegendOrder(TString leg, size_t no){
	for(std::vector<ztop::histoStack>::iterator s=stacks_.begin();s<stacks_.end();++s){
		s->setLegendOrder(leg,no);
	}
}

void histoStackVector::add(ztop::histo1D & container, TString leg , int color , double norm,int legor){
	bool found=false;
	for(std::vector<ztop::histoStack>::iterator s=stacks_.begin();s<stacks_.end();++s){
		if(s->getName() == container.getName()){
			if(histo1D::debug)
				std::cout << "histoStackVector::add: adding " << s->getName() << " to existing stack"  <<std::endl;
			s->push_back(container, leg, color, norm,legor);
			found=true;
			break;
		}
	}
	if(!found){
		if(histo1D::debug)
			std::cout << "histoStackVector::add: creating new stack " << container.getName()  <<std::endl;
		ztop::histoStack newstack(container.getName());
		newstack.push_back(container, leg, color, norm,legor);
		stacks_.push_back(newstack);
	}
}
void histoStackVector::add(ztop::histo2D & container, TString leg , int color , double norm,int legor){
	bool found=false;
	for(std::vector<ztop::histoStack>::iterator s=stacks_.begin();s<stacks_.end();++s){
		if(s->getName() == container.getName()){
			if(histo1D::debug)
				std::cout << "histoStackVector::add: adding " << s->getName() << " to existing stack"  <<std::endl;
			s->push_back(container, leg, color, norm,legor);
			found=true;
			break;
		}
	}
	if(!found){
		if(histo1D::debug)
			std::cout << "histoStackVector::add: creating new stack " << container.getName()  <<std::endl;
		ztop::histoStack newstack(container.getName());
		newstack.push_back(container, leg, color, norm,legor);
		stacks_.push_back(newstack);
	}
}
void histoStackVector::add(ztop::histo1DUnfold & container, TString leg , int color , double norm,int legor){
	bool found=false;
	for(std::vector<ztop::histoStack>::iterator s=stacks_.begin();s<stacks_.end();++s){
		if(s->getName() == container.getName()){
			if(histo1D::debug)
				std::cout << "histoStackVector::add: adding " << s->getName() << " to existing stack"  <<std::endl;
			s->push_back(container, leg, color, norm,legor);
			found=true;
			break;
		}
	}
	if(!found){
		if(histo1D::debug)
			std::cout << "histoStackVector::add: creating new stack " << container.getName()  <<std::endl;
		ztop::histoStack newstack(container.getName());
		newstack.push_back(container, leg, color, norm,legor);
		stacks_.push_back(newstack);
	}
}

void histoStackVector::addList(TString leg, int color, double norm,int legor){
	for(unsigned int i=0;i<histo1D::c_list.size();i++){
		add(*histo1D::c_list[i],leg,color,norm,legor);
	}
}
void histoStackVector::addList2D(TString leg, int color, double norm,int legor){
	for(unsigned int i=0;i<histo2D::c_list.size();i++){
		add(*histo2D::c_list[i],leg,color,norm,legor);
	}
}
void histoStackVector::addList1DUnfold(TString leg, int color, double norm,int legor){
	for(unsigned int i=0;i<histo1DUnfold::c_list.size();i++){
		histo1DUnfold::c_list[i]->flush();
		add(*histo1DUnfold::c_list[i],leg,color,norm,legor);
	}
}


/**
 * call AFTER!!! adding new stacks
 */
void histoStackVector::addSignal(const TString & signame){
	size_t size=stacks_.size();
//#pragma omp parallel for
	for(size_t i=0;i<size;i++){
		stacks_.at(i).addSignal(signame);
	}
}








const ztop::histoStack& histoStackVector::getStack(const TString& name,size_t startidx)const{
	//try same index first
	if(startidx<stacks_.size()){
		if(name == stacks_.at(startidx).getName()){
			return stacks_.at(startidx);
		}
	}
	std::cout << "searching further..." <<std::endl;
	std::vector<ztop::histoStack>::const_iterator it=stacks_.end();
	bool abort = false;
#pragma omp parallel for
	for(std::vector<ztop::histoStack>::const_iterator s=stacks_.begin();s<stacks_.end();++s){
#pragma omp flush (abort)
		if (!abort) {
			if(name == s->getName()){
#pragma omp critical (histoStackVector_searchstack_const)
				{
					it=s;
					abort=true;
#pragma omp flush (abort)
				}
			}
		}
	}
	if(it == stacks_.end()){
		std::string errstr="histoStackVector::getStack: stack ";
		errstr+=name.Data();
		errstr+=" not found";
		throw std::out_of_range(errstr);
	}
	return *it;
}

ztop::histoStack& histoStackVector::getStack(const TString& name,size_t startidx){
	if(startidx<stacks_.size()){
		if(name == stacks_.at(startidx).getName()){
			return stacks_.at(startidx);
		}
	}
	std::vector<ztop::histoStack>::iterator it=stacks_.end();
	bool abort = false;
#pragma omp parallel for
	for(std::vector<ztop::histoStack>::iterator s=stacks_.begin();s<stacks_.end();++s){
#pragma omp flush (abort)
		if (!abort) {
			if(name == s->getName()){
#pragma omp critical (histoStackVector_searchstack)
				{
					it=s;
					abort=true;
#pragma omp flush (abort)
				}
			}
		}
	}
	if(it == stacks_.end()){
		std::string errstr="histoStackVector::getStack: stack ";
		errstr+=name.Data();
		errstr+=" not found.";
		throw std::out_of_range(errstr);
	}
	return *it;
}


void histoStackVector::removeContribution(TString contribution){
	for(std::vector<ztop::histoStack>::iterator stack=stacks_.begin(); stack < stacks_.end(); ++stack){
		stack->removeContribution(contribution);
	}
}

void histoStackVector::addMCErrorStackVector(const TString &sysname, const ztop::histoStackVector & stackvec){
	addErrorStackVector(sysname , stackvec);
}
void histoStackVector::addErrorStackVector(const TString &sysname,const  ztop::histoStackVector & stackvec){
	for(size_t i=0;i<stacks_.size();i++){
		stacks_.at(i).addMCErrorStack(sysname,stackvec.getStack(stacks_.at(i).getName(),i));
	}
}

void histoStackVector::addMCErrorStackVector(const ztop::histoStackVector& stackvec){
	addMCErrorStackVector(stackvec.getSyst(), stackvec);
}
void histoStackVector::addGlobalRelMCError(TString sysname,double error){
#pragma omp parallel for
	for(std::vector<histoStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
		stack->addGlobalRelMCError(sysname,error);
	}
}
void histoStackVector::getRelSystematicsFrom(const ztop::histoStackVector& stackvec){
	//#pragma omp parallel for
	for(std::vector<histoStack>::iterator istack=stacks_.begin();istack<stacks_.end(); ++istack){
		try{
			const histoStack & stack=stackvec.getStack(istack->getName(),istack-stacks_.begin());
			istack->getRelSystematicsFrom(stack);
		}catch(...){}
	}
}
void histoStackVector::addRelSystematicsFrom(const ztop::histoStackVector& stackvec,bool ignorestat,bool strict){
	//#pragma omp parallel for
	for(std::vector<histoStack>::iterator istack=stacks_.begin();istack<stacks_.end(); ++istack){
		try{
			const histoStack & stack=stackvec.getStack(istack->getName(),istack-stacks_.begin());
			istack->addRelSystematicsFrom(stack,ignorestat,strict);
		}catch(std::exception& e){
			std::cout << "histoStackVector::addRelSystematicsFrom: Stack " << istack->getName() << " not found." <<std::endl;
			std::cout << e.what()<<std::endl;
		}
	}
}

void histoStackVector::removeError(TString name){
	for(std::vector<histoStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
		stack->removeError(name);
	}
}
void histoStackVector::renameSyst(TString old, TString New){
	for(std::vector<histoStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
		stack->renameSyst(old,New);
	}
}

std::vector<size_t> histoStackVector::removeSpikes(bool inclUFOF,
		int limittoindex,float strength,float sign,float threshold){
	std::vector<size_t> out,temp;
	for(std::vector<histoStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
		temp=stack->removeSpikes(inclUFOF,limittoindex,strength,sign,threshold);
		out.insert(out.end(),temp.begin(),temp.end());
	}
	return out;
}


void histoStackVector::multiplyNorm(TString legendname, double multi, TString step){
	for(std::vector<histoStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
		if(stack->getName().Contains(step)){
			stack->multiplyNorm(legendname, multi);

		}
	}
}

void histoStackVector::multiplyNorms(TString legendname, std::vector<double> scalefactors, std::vector<TString> identifier,bool showmessages){
	if((identifier.size() != scalefactors.size())){
		std::cout << "histoStackVector::multiplyNorms: identifiers and scalefactors must be same size!" << std::endl;
	}
	else{
		unsigned int count=0;
		for(std::vector<histoStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
			for(unsigned int i=0;i<scalefactors.size();i++){
				if(stack->getName().EndsWith(identifier.at(i))){
					if(showmessages) std::cout << "rescaling " << stack->getName() << "  " <<legendname << " by " << scalefactors[i] << std::endl;
					stack->multiplyNorm(legendname, scalefactors.at(i));
					if(showmessages) std::cout << "rescaling done" << std::endl;
					count++;
				}
			}
		}
		if(count < identifier.size()) std::cout << "histoStackVector::multiplyNorms: warning: not all identifiers found!" << std::endl;
		else if(count > identifier.size() && showmessages) std::cout << "histoStackVector::multiplyNorms: warning: identifiers where ambiguous! Scaled more than one stack per identifier (intended?)" << std::endl;
	}
}

void histoStackVector::multiplyAllMCNorms(double multiplier){
	for(std::vector<histoStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
		stack->multiplyAllMCNorms(multiplier);
	}
}

void histoStackVector::addPseudoData(TRandom3 * rand){
	if(stacks_.size()<1)
		return;
	bool hasdata=true;
	try{
		stacks_.at(0).getDataIdx();
	}catch(...){
		hasdata=false;
	}
	if(hasdata){
		std::cout << "histoStackVector::addPseudoData: Warning: Trying to add pseudo data to already existing data." <<std::endl;
	}
	for(size_t i=0;i<stacks_.size();i++){

		if(stacks_.at(i).is1D()){
			histo1D c=stacks_.at(i).getFullMCContainer();
			c.setAllErrorsZero(false);
			c.createStatFromContent();
			c=c.createPseudoExperiment(rand);
			stacks_.at(i).push_back(c,"data",0,1,999);
		}
		if(stacks_.at(i).is1DUnfold()){ //no else here!
			histo1DUnfold c=stacks_.at(i).getFullMCContainer1DUnfold();
			histo1D c2=c.getRecoContainer();
			c2.setAllErrorsZero(false);
			c2.createStatFromContent();
			c2=c2.createPseudoExperiment(rand);
			c.setRecoContainer(c2);
			stacks_.at(i).push_back(c,"data",0,1,999);
		}
		else if(stacks_.at(i).is2D()){
			histo2D c=stacks_.at(i).getFullMCContainer2D();
			c.setAllErrorsZero(false);
			c.createStatFromContent();
			c=c.createPseudoExperiment(rand);
			stacks_.at(i).push_back(c,"data",0,1,999);
		}
	}
}

void histoStackVector::writeAllToTFile(TString filename, bool recreate, bool onlydata,TString treename){
	if(debug)
		std::cout << "histoStackVector::writeAllToTFile(TString filename, bool recreate, TString treename)" << std::endl;

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
			std::cout << "histoStackVector::writeAllToTFile: created/opened file and tree" << std::endl;
	}
	else{
		std::cout << "histoStackVector::writeAllToTFile: creating/opening tree failed" << std::endl;
	}


	if(t->GetBranch("histoStackVectors")){ //branch does  exist
		if(debug) std::cout << "histoStackVector::writeAllToTFile: opened branch" << std::endl;
		bool temp=csv_makelist;
		csv_makelist=false;
		histoStackVector * csv = this;
		t->SetBranchAddress("histoStackVectors",&csv);
		csv_makelist=temp;
	}
	else{
		t->Branch("histoStackVectors",this);
		if(debug) std::cout << "histoStackVector::writeAllToTFile: added branch" << std::endl;
	}
	if(debug)
		std::cout << "histoStackVector::writeAllToTFile: got branch" << std::endl;

	t->Fill();
	if(debug)
		std::cout << "histoStackVector::writeAllToTFile: filled branch" << std::endl;

	t->Write("",TObject::kOverwrite);
	if(debug)
		std::cout << "histoStackVector::writeAllToTFile: written branch" << std::endl;

	delete t;

	if(onlydata){
		f->Close();
		delete f;

	}
	else{
		//bool batch=histoStack::batchmode;
		//histoStack::batchmode=true;
		if(debug)
			std::cout << "histoStackVector::writeAllToTFile: preparing plots" << std::endl;

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

		for(std::vector<histoStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){

			if(stack->is1DUnfold()){
				TDirectory * din=d->mkdir(stack->getName(),stack->getName());
				din->cd();
				TCanvas *c = new TCanvas(stack->getName());
				c->SetBatch(true);
				pl.setTitle(stack->getName());
				pl.usePad(c);
				pl.setStack(&*stack);
				try{
					pl.draw();
				}catch(std::exception & e){
					std::cout << e.what() << std::endl;
					std::cout << "could not draw, clsoing file now"<<std::endl;
					f->Close();
					if(c)
						delete c;
					delete f;
					return;
				}

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
				histo1DUnfold cuf=stack->getSignalContainer1DUnfold();
				histoStack rebinned=stack->rebinXToBinning(cuf.getGenBins());
				rebinned.setName(stack->getName()+"_genbins");
				c = new TCanvas(rebinned.getName());
				c->SetBatch(true);
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
					c->SetBatch(true);
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
				c->SetBatch(true);
				histo1D gen= cuf.getGenContainer();
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
					c->SetBatch(true);
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
				//histoStack::debug=true;
				// plotterControlPlot::debug=true;
				TCanvas *c = new TCanvas(stack->getName());
				c->SetBatch(true);
				pl.setTitle(stack->getName());
				pl.usePad(c);
				pl.setStack(&*stack);
				try{
					pl.draw();
				}catch(std::exception & e){
					std::cout << e.what() << std::endl;
					std::cout << "could not draw, clsoing file now"<<std::endl;
					f->Close();
					if(c)
						delete c;
					delete f;
					return;
				}

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
					histo2D  c2d= stack->getContainer2D(plot);

					TH2D * th2d=c2d.getTH2D(stack->getLegend(plot),true,true);
					if(th2d){

						TString canvasname=stack->getLegend(plot);
						canvasname+=+"_"+stack->getName();
						TCanvas *  c= new TCanvas(canvasname);
						c->SetBatch(true);
						th2d->Draw("colz");
						c->Write(canvasname);

						delete c;
						delete th2d;
					}
				}

				//in addition for signal and BG summed
				histo2D  c2d= stack->getSignalContainer2D();
				TH2D * th2d=c2d.getTH2D("signal",true,true);
				if(th2d){
					TCanvas *  c= new TCanvas("signal_"+stack->getName());
					c->SetBatch(true);
					th2d->Draw("colz");
					c->Write("signal_"+stack->getName());

					delete c;
					delete th2d;
				}
				c2d= stack->getBackgroundContainer2D();
				th2d=c2d.getTH2D("background",true,true);
				if(th2d){
					TCanvas *  c= new TCanvas("background_"+stack->getName());
					c->SetBatch(true);
					th2d->Draw("colz");
					c->Write("background_"+stack->getName());

					delete c;
					delete th2d;
				}

				d->cd();
			}
		}
		if(debug)
			std::cout << "histoStackVector::writeAllToTFile: ratio plots drawn" << std::endl;


		f->Close();
		delete f;

	}//not onlydata done
	if(debug)
		std::cout << "histoStackVector::writeAllToTFile: finished" << std::endl;

}


void histoStackVector::printAll(TString namestartswith,TString directory,TString extension){
	plotterControlPlot pl;
	for(std::vector<histoStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
		if(stack->is1D() || stack->is1DUnfold() ){
			if(namestartswith=="" || stack->getName().BeginsWith(namestartswith)){
				TCanvas * c=new TCanvas();
				pl.usePad(c);
				pl.setStack(&*stack);
				pl.draw();
				if(c){
					TString newname=stack->getName();
					newname.ReplaceAll(" ","_");
#pragma omp critical (histoStackVector_printAll)
					{
						c->Print(directory+newname+extension);
						stack->cleanMem();

						delete c;
					}
				}
				pl.cleanMem();
			}
		}
	}
}

void histoStackVector::writeAllToTFile(TFile * f, TString treename){
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
	if(t->GetBranch("histoStackVectors")){ //branch does exist
		bool temp=csv_makelist;
		csv_makelist=false;
		histoStackVector * csv = this;
		t->SetBranchAddress("histoStackVectors",&csv);
		csv_makelist=temp;
	}
	else{
		t->Branch("histoStackVectors",this);
		if(debug)std::cout << "histoStackVector::writeAllToTFile: added branch" << std::endl;
	}
	t->Fill();
	t->Write("",TObject::kOverwrite);
	delete t;

}

//only for compat reasons
void histoStackVector::loadFromTree(TTree * t, const TString& name){
	AutoLibraryLoader::enable();
	getFromTree(t,name);

}

histoStackVector * histoStackVector::getFromTree(TTree * t, const TString& name){
#pragma omp critical (histoStackVector_getFromTree)
	{
		AutoLibraryLoader::enable();
		if(!t || t->IsZombie()){
			throw std::runtime_error("histoStackVector::loadFromTree: tree not ok");
		}
		ztop::histoStackVector * cuftemp=0; //new histoStackVector(); //shouldn't make sense.. but seems to decrease TBasket memleaks
		if(!t->GetBranch("histoStackVectors")){
			throw std::runtime_error("histoStackVector::loadFromTree: branch histoStackVectors not found");
		}
		bool found=false;
		size_t count=0;
		t->SetBranchAddress("histoStackVectors", &cuftemp);
		if(name == "*" && t->GetEntries()>1){
			throw std::runtime_error("histoStackVector::getFromTree: No csv name specified but more than one object in file");
		}
		for(float n=0;n<t->GetEntries();n++){
			t->GetEntry(n);
			if(name == "*" || cuftemp->getName()==(name)){
				found=true;
				count++;
				clear();
				*this=*cuftemp;
			}
		}
		delete cuftemp;
		if(!found){
			throw std::runtime_error("histoStackVector::loadFromTree: no histoStackVector with name not found");
		}
		if(count>1){
			std::cout << "histoStackVector::loadFromTree: found more than one object with name "
					<< getName() << ", took the first one." << std::endl;
		}
	}
	return this;

}
void histoStackVector::loadFromTFile(TFile * f,const TString& csvname,TString treename){
	if(!f || f->IsZombie()){
		throw std::runtime_error("histoStackVector::loadFromTFile: file not ok.");
	}
	f->cd();
	TTree * ttemp = (TTree*)f->Get(treename);
	loadFromTree(ttemp,csvname);
	delete ttemp;
}
void histoStackVector::loadFromTFile(const TString& filename,const TString& csvname,TString treename){
#pragma omp critical (histoStackVector_loadFromTFile)
	{
		AutoLibraryLoader::enable(); //to avoid warnings
		TFile * ftemp=new TFile(filename,"read");
		loadFromTFile(ftemp,csvname,treename);
		ftemp->Close();
		delete ftemp;
	}
}



void histoStackVector::writeToFile(const std::string& filename)const{
	std::ofstream saveFile;
	IO::openOutFile(saveFile,filename);
	{
		boost::iostreams::filtering_ostream out;
		boost::iostreams::zlib_params parms;
		parms.level=boost::iostreams::zlib::best_speed;
		out.push(boost::iostreams::zlib_compressor(parms));
		out.push(saveFile);
		{
			writeToStream(out);
		}
	}
	saveFile.close();
}
void histoStackVector::readFromFile(const std::string& filename){
	std::ifstream saveFile;
	IO::openInFile(saveFile,filename);
	{
		boost::iostreams::filtering_istream in;
		boost::iostreams::zlib_params parms;
		//parms.level=boost::iostreams::zlib::best_speed;
		in.push(boost::iostreams::zlib_decompressor(parms));
		in.push(saveFile);
		{
			readFromStream(in);
		}
	}
	saveFile.close();
}



void histoStackVector::listAllInTree(TTree * t){
	AutoLibraryLoader::enable();
	histoStackVector * csv=0;
	t->SetBranchAddress("histoStackVectors", &csv);
	for(float n=0;n<t->GetEntries();n++){
		t->GetEntry(n);
		std::cout << csv->getName() << std::endl;
	}
	delete csv;
}



}
