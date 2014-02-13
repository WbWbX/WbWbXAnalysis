#include "../interface/containerStackVector.h"
#include <omp.h>
#include "../interface/fileReader.h"
#include "../interface/indexMap.h"

#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "../interface/plotterControlPlot.h"
#include "TStyle.h"

bool ztop::container1D::debug =false;

namespace ztop{


std::vector<containerStackVector*> containerStackVector::csv_list;
bool containerStackVector::csv_makelist=false;
bool containerStackVector::debug=false;
bool containerStackVector::fastadd=false;

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

void containerStackVector::addMCErrorStackVector(const TString &sysname, const ztop::containerStackVector & stackvec){
    addErrorStackVector(sysname , stackvec);
}
void containerStackVector::addErrorStackVector(const TString &sysname,const  ztop::containerStackVector & stackvec){
    if(!fastadd){
        for(std::vector<containerStack>::iterator istack=stacks_.begin();istack<stacks_.end(); ++istack){
            for(std::vector<containerStack>::const_iterator estack=stackvec.stacks_.begin();estack<stackvec.stacks_.end(); ++estack){
                if(istack->getName() == estack->getName()){
                    if(debug)
                        std::cout << "containerStackVector::addMCErrorStackVector: adding sys " << sysname
                        << " to stack " << istack->getName() << std::endl;
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
void containerStackVector::getRelSystematicsFrom(ztop::containerStackVector stackvec){
    if(!fastadd){
        for(std::vector<containerStack>::iterator istack=stacks_.begin();istack<stacks_.end(); ++istack){
            for(std::vector<containerStack>::iterator estack=stackvec.stacks_.begin();estack<stackvec.stacks_.end(); ++estack){
                if(istack->getName() == estack->getName()){
                    istack->getRelSystematicsFrom(*estack);
                    break;
                }
            }
        }
    }
    else{//fastadd requires same ordering of all stacks (usually the case)
        for(size_t i=0;i<stacks_.size();i++)
            stacks_.at(i).getRelSystematicsFrom(stackvec.stacks_.at(i));
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

void containerStackVector::writeAllToTFile(TString filename, bool recreate, bool onlydata,TString treename){
    if(debug)
        std::cout << "containerStackVector::writeAllToTFile(TString filename, bool recreate, TString treename)" << std::endl;

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
        t = new TTree(treename,treename);
    }
    if(t->GetBranch("containerStackVectors")){ //branch does  exist
        bool temp=csv_makelist;
        csv_makelist=false;
        containerStackVector * csv = this;
        t->SetBranchAddress("containerStackVectors",&csv);
        csv_makelist=temp;
    }
    else{
        t->Branch("containerStackVectors",this);
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

    if(onlydata){
        f->Close();
        delete f;
        return;
    }

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
            container1DUnfold cuf=stack->getSignalContainer();
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
    }
    if(debug)
        std::cout << "containerStackVector::writeAllToTFile: ratio plots drawn" << std::endl;

    /*
    TDirectory * d2 = f->mkdir(name+ "_sgbg",name+ "_sgbg");
    d2->cd();

    dirs.clear();
    dirpv.clear();

    for(std::vector<containerStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
        if(stack->is1D()){
            std::string dirname=stack->getName().Data();
            fr.trimcomments(dirname);
            size_t diridx=dirs.getIndex(dirname);
            TDirectory * tdir=0;
            if(diridx>=dirs.size()){ //new dir
                tdir=d2->mkdir(dirname.data(),dirname.data());
                dirpv.push_back(tdir);
                dirs.push_back(dirname);
            }
            //d->mkdir(); //cd dir with idx
            tdir=dirpv.at(diridx);
            tdir->cd();
            TCanvas * c=stack->makeTCanvas(containerStack::plotmode::sigbg);
            if(c){
                //tdir->WriteObject(c,c->GetName());
                c->Write();
                stack->cleanMem();
                delete c;
            }
            d2->cd();
        }
    }
    if(debug)
        std::cout << "containerStackVector::writeAllToTFile: sig/bg plots drawn" << std::endl;

     */
    f->Close();
    delete f;

    containerStack::batchmode=batch;
    if(debug)
        std::cout << "containerStackVector::writeAllToTFile: finished" << std::endl;

}


void containerStackVector::printAll(TString namestartswith,TString directory,TString extension){
    for(std::vector<containerStack>::iterator stack=stacks_.begin();stack<stacks_.end(); ++stack){
        if(stack->is1D() || stack->is1DUnfold() ){
            if(namestartswith=="" || stack->getName().BeginsWith(namestartswith)){
                TCanvas * c=stack->makeTCanvas(containerStack::plotmode::ratio);
                if(c){
                    TString newname=stack->getName();
                    newname.ReplaceAll(" ","_");
                    c->Print(directory+newname+extension);
                    stack->cleanMem();
                    delete c;
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
        t = new TTree(treename,treename);
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


void containerStackVector::loadFromTree(TTree * t, TString name){
    AutoLibraryLoader::enable();
    containerStackVector csv;
    containerStackVector * pcsv=csv.getFromTree(t,name);
    if(pcsv)
        *this=*pcsv;
    else
        throw std::runtime_error("containerStackVector::loadFromTree: no CSV found in tree");


}

containerStackVector * containerStackVector::getFromTree(TTree * t, TString name){
    AutoLibraryLoader::enable();
    if(!t || t->IsZombie()){
            throw std::runtime_error("containerStackVector::loadFromTree: tree not ok");
        }
        ztop::containerStackVector * cuftemp=0;
        if(!t->GetBranch("containerStackVectors")){
            throw std::runtime_error("containerStackVector::loadFromTree: branch containerStackVectors not found");
        }
        bool found=false;
        size_t count=0;
        t->SetBranchAddress("containerStackVectors", &cuftemp);
        for(float n=0;n<t->GetEntries();n++){
            t->GetEntry(n);
            std::cout << cuftemp->getName() << std::endl;
            if(cuftemp->getName()==(name)){
                found=true;
                count++;
                *this=*cuftemp;
            }
        }
       if(!found){
            throw std::runtime_error("containerStackVector::loadFromTree: no containerStackVector with name not found");
        }
        if(count>1){
            std::cout << "containerStackVector::loadFromTree: found more than one object with name "
                    << getName() << ", took the first one." << std::endl;
        }
        return cuftemp;
 /*   containerStackVector * csv=0;
    bool found=false;
    if(!t)
        return 0;
    if(!t->GetBranch("containerStackVectors"))
        return 0;
    t->SetBranchAddress("containerStackVectors", &csv);
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
    return 0; */
}
void containerStackVector::loadFromTFile(TFile * f,const TString& csvname,TString treename){
    if(!f || f->IsZombie()){
        throw std::runtime_error("containerStackVector::loadFromTFile: file not ok.");
    }
    TTree * ttemp = (TTree*)f->Get(treename);
    loadFromTree(ttemp,csvname);
    delete ttemp;
}
void containerStackVector::loadFromTFile(const TString& filename,const TString& csvname,TString treename){
    TFile * ftemp=new TFile(filename,"read");
    loadFromTFile(ftemp,csvname,treename);
    delete ftemp;
}


void containerStackVector::listAllInTree(TTree * t){
    AutoLibraryLoader::enable();
    containerStackVector * csv=0;
    t->SetBranchAddress("containerStackVectors", &csv);
    for(float n=0;n<t->GetEntries();n++){
        t->GetEntry(n);
        std::cout << csv->getName() << std::endl;
    }
}



}
