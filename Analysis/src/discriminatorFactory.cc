/*
 * discriminatorFactory.cc
 *
 *  Created on: Mar 26, 2014
 *      Author: kiesej
 */



#include "../interface/discriminatorFactory.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include <stdexcept>
#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TtZAnalysis/Tools/interface/container2D.h"


#define VAR_TO_TSTRING(variable) ztop::toTString(#variable)


namespace ztop{

bool discriminatorFactory::debug=false;
std::vector<discriminatorFactory*> discriminatorFactory::c_list;
bool discriminatorFactory::c_makelist=false;


discriminatorFactory::discriminatorFactory(): nbins_(0),uselh_(false),step_(9999999),systidx_(-1),maxcomb_(1),savecorrplots_(true)
{
    if(c_makelist)
        c_list.push_back(this);
}

discriminatorFactory::discriminatorFactory(const std::string & id): nbins_(0),uselh_(false),id_(id),step_(9999999),systidx_(-1),maxcomb_(1)
{
    if(c_makelist)
        c_list.push_back(this);
}

discriminatorFactory::~discriminatorFactory(){
    for(size_t i=0;i<c_list.size();i++){
        if(c_list.at(i) == this){
            c_list.erase((c_list.begin()+i));
        }
    }
    //  clear();
    //memory leak...
}



void discriminatorFactory::clear(){
    vars_.clear();
    ranges_.clear();
    offsets_.clear();
    for(size_t i=0;i<tobefilled_.size();i++){
        if(tobefilled_.at(i)){
            delete tobefilled_.at(i);
            tobefilled_.at(i)=0;
        }
    }
    /*
    for(size_t i=0;i<tobefilledcorr_.size();i++){
        if(tobefilledcorr_.at(i)){
            delete tobefilledcorr_.at(i);
            tobefilledcorr_.at(i)=0;
        }
    }
    */
    tobefilledcorr_.clear();
    tobefilled_.clear();
    histpointers_.clear(); //never point to "NEW"
    //


}

void discriminatorFactory::setSystematics(const TString& sysname){
    if(!uselh_){
        systidx_=-1;
        return;
    }
    if(histpointers_.size()<1){
        throw std::logic_error("discriminatorFactory::setSystematics: first add at least one variable!");
    }

    if(sysname!="nominal"){
        systidx_=histpointers_.at(0)->getSystErrorIndex(sysname); //they should be the same for all of them!

        if(systidx_ == (int)histpointers_.at(0)->getSystSize()){
            systidx_=-1; //in case you want to ignore the exception
            throw std::runtime_error("discriminatorFactory::setSystematics: Systematics could not be found");
        }
        std::cout << "discriminatorFactory::setSystematics: loaded " << histpointers_.at(0)->getSystErrorName(systidx_) <<std::endl;
    }
    else{
        systidx_=-1;
    }
}



/**
 * if create is true, create new container
 * plots will be named "<id_>_discr_plotname step X" and tagged with isLHDiscr_tag
 * if create is false, the variable will be associated to a plot according to its name
 * ("LH_<id_>_discr_plotname step X" )
 * var should be from the event pointer
 */
void discriminatorFactory::addVariable( float * const * var,const TString& varshort,const float& rangemin, const float& rangemax){
    if(debug)
        std::cout << "discriminatorFactory::addVariable" <<std::endl;

    if(step_ > 9999998){
        throw std::logic_error("discriminatorFactory::addVariable: first define step!");
    }
    if(nbins_<1){
        throw std::logic_error("discriminatorFactory::addVariable: first set number of bins!");
    }
    if(rangemin>rangemax){
        throw std::logic_error("discriminatorFactory::addVariable: minimum range for value should be smaller than max!");
    }

    if(debug)
        std::cout << "discriminatorFactory::addVariable: creating name" <<std::endl;

    TString varstr=varshort;
    varstr.ReplaceAll("#","");
    varstr.ReplaceAll("/",".DIV.");
    TString fullname=id_+"_discr_"+varstr+" step "+toTString(step_);




    if(uselh_){

        if(debug)
            std::cout << "discriminatorFactory::addVariable: searching for likelihood histos" <<std::endl;


        for(size_t i=0;i<likelihoods_.size();i++){
            if(likelihoods_.at(i).getName() == "LH_"+fullname){
                maxcomb_ *= likelihoods_.at(i).getYMax(false);
                histpointers_.push_back(&likelihoods_.at(i));
                //make sure a plot is added to the control plots!
                // sy
                likelihoods_.at(i).addTag(taggedObject::dontAddSyst_tag);
                container1D::c_list.push_back(&likelihoods_.at(i));
                if(debug)
                    std::cout << "discriminatorFactory::addVariable: loaded likelihood for " << fullname
                    <<"\nBins: "<< likelihoods_.at(i).getNBins()<<std::endl;
                break;
            }
        }

    }
    //make bins
    std::vector<float> bins;//bins << 0 << 0.2 << 1;
    for(float i=rangemin;i<rangemax*(1.+0.9/nbins_);i+= 1.*(rangemax-rangemin)/nbins_) bins.push_back(i);


    if(debug)
        std::cout << "discriminatorFactory::addVariable: creating all correlation plots" <<std::endl;
    tobefilledcorr_.push_back(std::vector<container2D *>());

    //if more than one var add correlation plots
    if(vars_.size()>0){
        bool c2dmakelist=container2D::c_makelist;
        container2D::c_makelist=savecorrplots_;
        for(size_t i=0;i<tobefilled_.size();i++){ //not the last one thats the one just added
            std::vector<float> newybins(tobefilled_.at(i)->getBins().begin()+1,tobefilled_.at(i)->getBins().end());
            TString newyaxisname=tobefilled_.at(i)->getXAxisName();
            TString newxaxisname= varshort;
            TString newname= id_+"_discr_"+newxaxisname+"_vs_"+newyaxisname+" step "+toTString(step_);
            container2D * c2dtmp= new container2D(bins,newybins,newname,newxaxisname,newyaxisname);

            tobefilledcorr_.at(tobefilledcorr_.size()-1).push_back(c2dtmp);
        }
        container2D::c_makelist=c2dmakelist;
    }

    if(debug)
        std::cout << "discriminatorFactory::addVariable: creating new plot" <<std::endl;

    //creates new plots for likelihoods

    /*
     * Listing is switched on. That makes sure that the containers will be stored afterwards in a
     * containerStackVector and treated as all other control plots (ecpet for the fact that they are tagged)
     */
    bool tmpkl=container1D::c_makelist;
    container1D::c_makelist=true;
    if(debug)
        std::cout << "discriminatorFactory::addVariable: pushing back new plot" <<std::endl;
    tobefilled_.push_back(new container1D(bins,fullname,varshort,"N_{evt}/bw",false));
    container1D::c_makelist=tmpkl;
    size_t lastidx=tobefilled_.size()-1;

    tobefilled_.at(lastidx)->addTag(taggedObject::isLHDiscr_tag);


    if(debug)
        std::cout << "discriminatorFactory::addVariable: added plot for " << fullname <<std::endl;






    vars_.push_back(var);

    if(debug)
        std::cout << "discriminatorFactory::addVariable: "<< vars_.size()<< " vars with " << tobefilled_.size()
        << " plots and "<< histpointers_.size()<< " likelihoods." <<std::endl;

}
void discriminatorFactory::fill(const float& weight){
    // if(uselh_) return;

    if(vars_.size() != tobefilled_.size()){
        throw std::logic_error("discriminatorFactory::fill: something went wrong with addVariable"); //DEBUG
    }
    size_t size=vars_.size();
    for(size_t i=0;i<size;i++){
        if(vars_.at(i) && tobefilled_.at(i)){
            tobefilled_.at(i)->fill(**vars_.at(i),weight );
            for(size_t j=0;j<tobefilledcorr_.at(i).size();j++){
                //if(i==j) continue;
                if(vars_.at(j) && tobefilledcorr_.at(i).at(j))
                    tobefilledcorr_.at(i).at(j)->fill(**vars_.at(i),**vars_.at(j),weight);
            }
        }
    }
}



/**
 * this is the return function. Only returns something if setUseLikelihoods is true and LH are filled
 * (note:) put in some safety checks
 */
float discriminatorFactory::getCombinedLikelihood()const{ //a plot needs to be added to control plots. NOT done automatically
    if(!uselh_) return 1;

    //conts_.at(i) not points to the likelihood histos
    //
    size_t csize=histpointers_.size();
    if(csize <1){
        return 1;
    }
    float combLH=1;
    for(size_t i=0;i<csize;i++){
        if(!vars_.at(i) || !*vars_.at(i)) continue;
        // float tofill=( **vars_.at(i) + offsets_.at(i) )/ranges_.at(i);
        size_t bin=histpointers_.at(i)->getBinNo(**vars_.at(i) );
        if(histpointers_.at(i)->getBinContent(bin,systidx_)/maxcomb_ < 0.0001){
            std::cout << "discriminatorFactory::getCombinedLikelihood: returned 0 for " << histpointers_.at(i)->getName() << " input: " << **vars_.at(i)
                    << " skip value "<<std::endl;
        }
        else{
            combLH *= histpointers_.at(i)->getBinContent(bin,systidx_);
        }
    }
    return combLH/maxcomb_;
}



void discriminatorFactory::extractLikelihoods(const containerStackVector &csv){
    //search for stacks that have the right tag
    if(debug)
        std::cout << "discriminatorFactory::extractLikelihoods" <<std::endl;

    bool tmpkl=container1D::c_makelist;
    container1D::c_makelist=false; //dont list copies here


    std::vector<container1D> signconts,bgconts;


    const std::vector<containerStack> * stacks=&csv.getVector();

    if(debug)
        std::cout << "discriminatorFactory::extractLikelihoods: "<< stacks->size() << " stacks available. " << std::endl;


    for(size_t ist=0;ist<stacks->size();ist++){
        const containerStack* stack=&stacks->at(ist);
        if(stack->hasTag(taggedObject::isLHDiscr_tag) && stack->is1D() && stack->getName().BeginsWith(id_)){
            container1D temp=stack->getSignalContainer();
            if(temp.isDummy()){
                throw std::logic_error("discriminatorFactory::extractLikelihoods: inputs are dummy");
            }
            temp.setName(stack->getName());
            signconts.push_back(temp);
            temp=stack->getBackgroundContainer();
            temp.setName(stack->getName());
            bgconts.push_back(temp);



            if(debug)
                std::cout << "discriminatorFactory::extractLikelihoods: got "<< stack->getName() << std::endl;

        }
    }

    /* create a likelihood for each.
     * keep in mind transformStatToSyst (how do we access it later on?) FIXME
     *
     * push back all in likelihoods_
     */
    likelihoods_.clear(); //clear old
    for(size_t i=0;i<signconts.size();i++){
        container1D temp;
        temp=signconts.at(i) / (signconts.at(i)+bgconts.at(i));
        temp.setName("LH_"+signconts.at(i).getName());
        temp.addTag(taggedObject::dontDivByBW_tag);
        if(temp.getYMax(false) > 1){
            std::cout << "discriminatorFactory::extractLikelihoods: likelihood > 1 for " << temp.getName() <<std::endl;
            throw std::logic_error("discriminatorFactory::extractLikelihoods: likelihood > 1!");
        }
        if(temp.getYMin(false) < 0){
            std::cout << "discriminatorFactory::extractLikelihoods: likelihood < 0 for " << temp.getName() <<std::endl;
            throw std::logic_error("discriminatorFactory::extractLikelihoods: likelihood < 0!");
        }
        likelihoods_.push_back(temp);
    }

    container1D::c_makelist=tmpkl;
    // throw std::runtime_error("discriminatorFactory::extractLikelihoods: To Be Implemented");
}
void discriminatorFactory::extractAllLikelihoods(const containerStackVector & csv){
    for(size_t i=0;i<c_list.size();i++)
        c_list.at(i)->extractLikelihoods(csv);
}


void discriminatorFactory::writeToTFile(TFile * f)const{
    AutoLibraryLoader::enable();
    /*
     * make sure that all pointer vectors are cleared before doing this
     * make a copy of object, clear all pointers and write
     */
    bool tempmakelist=c_makelist;
    c_makelist=false;
    discriminatorFactory copy=*this;
    copy.clear();
    if(!f || f->IsZombie()){
        throw std::runtime_error("discriminatorFactory::writeToTFile: file not ok");
    }
    f->cd();
    TTree * t = (TTree*)f->Get("discriminatorFactorys");
    if(!t || t->IsZombie())//create
        t = new TTree("discriminatorFactorys","discriminatorFactorys");

    ztop::discriminatorFactory * cufpointer=0;//&copy;
    if(t->GetBranch("discriminatorFactorys")){
        t->SetBranchAddress("discriminatorFactorys", &cufpointer);

        //check if there are more

        for(float n=0;n<t->GetEntries();n++){
            t->GetEntry(n);
            if(cufpointer->id_==copy.id_){
                if(*cufpointer == copy){
                    if(debug)
                        std::cout << "discriminatorFactory::writeToTFile: found same already, doing nothing" << std::endl;
                    delete t; //do nothing if it would be just a 1:1 copy
                    c_makelist=tempmakelist;
                    return;
                }
                else{
                    throw std::logic_error("discriminatorFactory::writeToTFile: You must not write two different objects with the same id to the same file");
                }
            }
        }

    }
    else{
        t->Branch("discriminatorFactorys",&cufpointer);
        t->SetBranchAddress("discriminatorFactorys", &cufpointer);
    }
    cufpointer=&copy;
    if(debug)
        std::cout << "writing " << getIdentifier() << " to " << f->GetName() << std::endl;

    t->Fill();
    t->Write(t->GetName(),TObject::kOverwrite);
    delete t;

    c_makelist=tempmakelist;
}

void discriminatorFactory::writeToTFile(const TString& filename )const{
    TFile * ftemp=new TFile(filename,"update");
    if(!ftemp || ftemp->IsZombie()){
        delete ftemp;
        ftemp=new TFile(filename,"create");
    }
    writeToTFile(ftemp);
    ftemp->Close();
    delete ftemp;
}

void discriminatorFactory::writeAllToTFile(TFile *f){
    for(size_t i=0;i<c_list.size();i++)
        c_list.at(i)->writeToTFile(f);
}
void discriminatorFactory::writeAllToTFile(const TString& filename){
    TFile * ftemp=new TFile(filename,"update");
    if(!ftemp || ftemp->IsZombie()){
        delete ftemp;
        ftemp=new TFile(filename,"create");
    }
    for(size_t i=0;i<c_list.size();i++)
        c_list.at(i)->writeToTFile(ftemp);
    ftemp->Close();
    delete ftemp;
}
void discriminatorFactory::writeAllToTFile(const TString& filename,const std::vector<discriminatorFactory> &vec){
    TFile * ftemp=new TFile(filename,"update");
    if(!ftemp || ftemp->IsZombie()){
        delete ftemp;
        ftemp=new TFile(filename,"create");
    }
    for(size_t i=0;i<vec.size();i++)
        vec.at(i).writeToTFile(ftemp);
    ftemp->Close();
    delete ftemp;
}

/**
 * In a first step only works if a containerStack exists for each plot in a containerStackVector
 * just search for one. If there are more than one (usually not) that contain discr. info, give a warning
 *
 */
void discriminatorFactory::readFromTFile( TFile * f ,const std::string& id){

    /*
     * just to be sure: clear all pointer vectors here
     */
    bool tempmakelist=c_makelist;
    c_makelist=false;
    clear();
    likelihoods_.clear();
    /*
     * make sure the containers to be used are tagged as LH
     *  container.hasTag(taggedObject::isLHDiscr_tag);
     */
    if(!f || f->IsZombie()){
        throw std::runtime_error("discriminatorFactory::readFromTFile: file not ok");
    }
    AutoLibraryLoader::enable();
    TTree * t = (TTree*)f->Get("discriminatorFactorys");


    if(!t || t->IsZombie()){
        throw std::runtime_error("discriminatorFactorys::readFromTFile: tree not ok");
    }
    ztop::discriminatorFactory * cuftemp=0;
    if(!t->GetBranch("discriminatorFactorys")){
        throw std::runtime_error("discriminatorFactory::loadFromTree: branch discriminatorFactorys not found");
    }
    bool found=false;
    size_t count=0;
    bool tmpkl=container1D::c_makelist;
    container1D::c_makelist=false; //dont list copies here

    t->SetBranchAddress("discriminatorFactorys", &cuftemp);
    for(float n=0;n<t->GetEntries();n++){
        t->GetEntry(n);
        if(cuftemp->id_==id){
            found=true;
            count++;
            *this=*cuftemp;
        }
    }
    if(!found){
        std::cout << "searching for: " << id << "... error!" << std::endl;
        throw std::runtime_error("discriminatorFactory::readFromTFile: discriminatorFactory with name not found");
    }
    if(count>1){
        std::cout << "discriminatorFactory::readFromTFile: found more than one object with name "
                << id << ", took the last one." << std::endl;
    }

    delete t;
    c_makelist=tempmakelist;
    container1D::c_makelist=tmpkl;

}
void discriminatorFactory::readFromTFile( const TString& filename ,const std::string& id){
    AutoLibraryLoader::enable();
    TFile * f = new TFile(filename,"READ");
    readFromTFile(f,id);
    f->Close();
    delete f;
}

std::vector<discriminatorFactory> discriminatorFactory::readAllFromTFile( TFile * f){

    if(!f || f->IsZombie()){
        throw std::runtime_error("discriminatorFactory::readFromTFile: file not ok");
    }
    AutoLibraryLoader::enable();
    TTree * t = (TTree*)f->Get("discriminatorFactorys");
    std::vector<discriminatorFactory>  out;

    if(!t || t->IsZombie()){
        throw std::runtime_error("discriminatorFactorys::readFromTFile: tree not ok");
    }
    ztop::discriminatorFactory * cuftemp=0;
    if(!t->GetBranch("discriminatorFactorys")){
        throw std::runtime_error("discriminatorFactory::loadFromTree: branch discriminatorFactorys not found");
    }


    bool tmpkl=container1D::c_makelist;
    container1D::c_makelist=false; //dont list copies here

    t->SetBranchAddress("discriminatorFactorys", &cuftemp);
    for(float n=0;n<t->GetEntries();n++){
        t->GetEntry(n);
        if(debug)
            std::cout << "discriminatorFactory::readAllFromTFile: reading "<< cuftemp->getIdentifier() <<std::endl;
        out.push_back(*cuftemp);
    }
    delete t;
    container1D::c_makelist=tmpkl;

    return out;

}
std::vector<discriminatorFactory> discriminatorFactory::readAllFromTFile(const TString& filename){
    std::vector<discriminatorFactory> out;

    TFile * f = new TFile(filename,"READ");
    out=readAllFromTFile(f);
    f->Close();
    delete f;
    return out;

}

bool discriminatorFactory::operator == (const discriminatorFactory&rhs)const{
    return nbins_==rhs.nbins_

            && uselh_==rhs.uselh_
            && id_==rhs.id_
            && step_==rhs.step_
            && likelihoods_==rhs.likelihoods_
            && tobefilled_==rhs.tobefilled_
            && vars_==rhs.vars_
            && histpointers_==rhs.histpointers_
            && systidx_==rhs.systidx_;

}


}
