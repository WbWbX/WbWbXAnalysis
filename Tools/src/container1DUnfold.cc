/*
 * container1DUnfold.cc
 *
 *  Created on: Aug 15, 2013
 *      Author: kiesej
 */

#include "../interface/container1DUnfold.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"

/*
 * multicore usage for unfolding to be implemented
 */
#include <omp.h>
#include <stdexcept>
#include "TTree.h"
#include "TFile.h"
namespace ztop{


bool container1DUnfold::debug=false;
std::vector<container1DUnfold*> container1DUnfold::c_list;
bool container1DUnfold::c_makelist=false;
//bool container1DUnfold::printinfo=false;

/*
 *
   float tempgen_,tempreco_,tempgenweight_,tempweight_;
   bool recofill_,genfill_;

   bool isMC_;
 */

container1DUnfold::container1DUnfold(): container2D(), xaxis1Dname_(""), yaxis1Dname_(""),
        tempgen_(0),tempreco_(0),tempgenweight_(1),tempweight_(1),recofill_(false),
        genfill_(false),isMC_(false),flushed_(true),binbybin_(false),lumi_(1),congruentbins_(false){
    if(c_makelist){
        c_list.push_back(this);
    }
}
container1DUnfold::container1DUnfold( std::vector<float> genbins, std::vector<float> recobins, TString name,TString xaxisname,TString yaxisname, bool mergeufof):
	               															                                                                                                                        container2D( recobins /*genbins*/ , recobins , name,xaxisname+"_reco",xaxisname+"_gen",mergeufof), xaxis1Dname_(xaxisname),
	               															                                                                                                                        yaxis1Dname_(yaxisname),tempgen_(0),tempreco_(0),tempgenweight_(1),tempweight_(1),recofill_(false),genfill_(false),
	               															                                                                                                                        isMC_(false),flushed_(true),binbybin_(false),lumi_(1),congruentbins_(false) {
    //bins are set, containers created, at least conts_[0] exists with all options (binomial, mergeufof etc)

    genbins_=genbins; //can be changed and rebinned afterwards

    gencont_=conts_.at(0);
    gencont_.clear();
    std::vector<float> databins=ybins_;
    databins.erase(databins.begin());

    recocont_ = container1D(databins,name_+"_data",xaxis1Dname_,yaxis1Dname_,mergeufof_);
    unfolded_ = container1D(databins,name_+"_unfolded",xaxis1Dname_,yaxis1Dname_,mergeufof_);
    refolded_ = container1D(databins,name_+"_refolded",xaxis1Dname_,yaxis1Dname_,mergeufof_);
    congruentbins_=checkCongruentBinBoundariesXY();
    if(c_makelist){
        c_list.push_back(this);
    }
}
container1DUnfold::container1DUnfold( std::vector<float> genbins, TString name,TString xaxisname,TString yaxisname, bool mergeufof){
    std::vector<float> recobins=subdivide(genbins,10);
    container1DUnfold(genbins,recobins,name,xaxisname,yaxisname,mergeufof);
}

container1DUnfold::~container1DUnfold(){
    for(size_t i=0;i<c_list.size();i++){
        if(this==c_list.at(i))
            c_list.erase(c_list.begin()+i);
        break;
    }
}

void container1DUnfold::setBinning(const std::vector<float> & genbins,const std::vector<float> &recobins){
    genbins_=genbins;
    container2D::setBinning(recobins,recobins);
    gencont_=conts_.at(0);
    gencont_.clear();
    std::vector<float> databins=ybins_;
    databins.erase(databins.begin());
    recocont_ = container1D(databins,name_+"_data",xaxis1Dname_,yaxis1Dname_,mergeufof_);
    unfolded_ = container1D(databins,name_+"_unfolded",xaxis1Dname_,yaxis1Dname_,mergeufof_);
    refolded_ = container1D(databins,name_+"_refolded",xaxis1Dname_,yaxis1Dname_,mergeufof_);
    congruentbins_=checkCongruentBinBoundariesXY();
}

container1DUnfold container1DUnfold::rebinToBinning(const std::vector<float> & newbins)const{
    size_t maxbinssize=xbins_.size();
    if(newbins.size()>maxbinssize) maxbinssize=newbins.size();
    std::vector<float> sames(maxbinssize);
    std::vector<float>::iterator it=std::set_intersection(++xbins_.begin(),xbins_.end(),newbins.begin(),newbins.end(),sames.begin());//ignore UF
    sames.resize(it-sames.begin());
    //check if possible at all:
    if(sames.size()!=newbins.size()){
        std::cout << "container1DUnfold::rebinToBinning: binning not compatible" <<std::endl;
        return *this;
    }
    container1DUnfold out=*this;
    container2D tempc2d=out.rebinXToBinning(newbins);
    tempc2d=tempc2d.rebinYToBinning(newbins);
    out.conts_=tempc2d.conts_;
    out.xbins_=tempc2d.xbins_;
    out.ybins_=tempc2d.ybins_;

    out.recocont_=recocont_.rebinToBinning(newbins);
    out.gencont_=gencont_.rebinToBinning(newbins);
    out.unfolded_=unfolded_.rebinToBinning(newbins);
    out.refolded_=refolded_.rebinToBinning(newbins);

    return out;

}

void container1DUnfold::removeAllSystematics(){
    for(size_t i=0;i<conts_.size();i++){
        conts_.at(i).removeAllSystematics();
    }
    recocont_.removeAllSystematics();
    unfolded_.removeAllSystematics();
    gencont_.removeAllSystematics();
    refolded_.removeAllSystematics();
}

void container1DUnfold::setBinByBin(bool bbb){
    if(!congruentbins_){
        std::cout << "container1DUnfold::setBinByBin: only possible if congruent bin boundaries for reco/gen! doing nothing" <<std::endl;
        return;
    }
    binbybin_=bbb;
}

void container1DUnfold::setRecoContainer(const ztop::container1D &cont){
    if(cont.getBins() != recocont_.getBins()){
        std::cout << "container1DUnfold::setRecoContainer: wrong binning" <<std::endl;
        return;
    }
    recocont_=cont;
}


void container1DUnfold::setBackground(const container1D & cont){
    //background is supposed to be in x UF bin //no OFUF
    if(cont.getBins() != ybins_){
        std::cout << "container1DUnfold::setBackground: Binning not compatible. doing nothing" << std::endl;
        return;
    }
    if(xbins_.size()<1){
        std::cout << "container1DUnfold::setBackground: No X bins!" << std::endl;
    }
    setXSlice(0,cont,false);

}
container1D container1DUnfold::getBackground() const{
    if(xbins_.size()<1){
        std::cout << "container1DUnfold::getBackground: No X bins!" << std::endl;
    }
    return getXSlice(0);
}
container1DUnfold container1DUnfold::operator + (const container1DUnfold & second){
    if(second.xbins_ != xbins_ || second.ybins_ != ybins_){
        std::cout << "container1DUnfold::operator +: "<< name_ << " and " << second.name_<<" must have same binning! returning *this" << std::endl;
        return *this;
    }
    container1DUnfold out=second;
    for(size_t i=0;i<conts_.size();i++){
        out.conts_.at(i) = conts_.at(i) + second.conts_.at(i);
    }
    out.gencont_=gencont_+second.gencont_;
    out.recocont_=recocont_+second.recocont_;
    out.unfolded_=unfolded_+second.unfolded_;
    out.refolded_=refolded_+second.refolded_;
    return out;
}
container1DUnfold container1DUnfold::operator - (const container1DUnfold & second){
    if(second.xbins_ != xbins_ || second.ybins_ != ybins_){
        std::cout << "container1DUnfold::operator -: "<< name_ << " and " << second.name_<<" must have same binning! returning *this"  << std::endl;
        return *this;
    }
    container1DUnfold out=second;
    for(size_t i=0;i<conts_.size();i++){
        out.conts_.at(i) = conts_.at(i) - second.conts_.at(i);
    }
    out.gencont_=gencont_-second.gencont_;
    out.recocont_=recocont_-second.recocont_;
    out.unfolded_=unfolded_-second.unfolded_;
    out.refolded_=refolded_-second.refolded_;
    return out;
}
container1DUnfold container1DUnfold::operator / (const container1DUnfold & second){
    if(second.xbins_ != xbins_ || second.ybins_ != ybins_){
        std::cout << "container1DUnfold::operator /: "<< name_ << " and " << second.name_<<" must have same binning! returning *this"  << std::endl;
        return *this;
    }
    if(second.divideBinomial_!=divideBinomial_){
        std::cout << "container1DUnfold::operator /: "<< name_ << " and " << second.name_<<" must have same divide option (divideBinomial). returning *this" <<  std::endl;
        return *this;
    }
    container1DUnfold out=second;
    for(size_t i=0;i<conts_.size();i++){
        out.conts_.at(i) = conts_.at(i) / second.conts_.at(i);
    }
    out.gencont_=gencont_/second.gencont_;
    out.recocont_=recocont_/second.recocont_;
    out.unfolded_=unfolded_/second.unfolded_;
    out.refolded_=refolded_/second.refolded_;
    return out;
}
container1DUnfold container1DUnfold::operator * (const container1DUnfold & second){
    if(second.xbins_ != xbins_ || second.ybins_ != ybins_){
        std::cout << "container1DUnfold::operator *: "<< name_ << " and " << second.name_<<" must have same binning! returning *this"  << std::endl;
        return *this;
    }
    container1DUnfold out=second;
    for(size_t i=0;i<conts_.size();i++){
        out.conts_.at(i) = conts_.at(i) * second.conts_.at(i);
    }
    out.gencont_=gencont_*second.gencont_;
    out.recocont_=recocont_*second.recocont_;
    out.unfolded_=unfolded_*second.unfolded_;
    out.refolded_=refolded_*second.refolded_;
    return out;
}
container1DUnfold container1DUnfold::operator * (float val){
    container1DUnfold out=*this;
    for(size_t i=0;i<conts_.size();i++){
        out.conts_.at(i) = conts_.at(i) * val;
    }
    out.gencont_=gencont_ * val;
    out.recocont_=recocont_*val;
    out.unfolded_=unfolded_*val;
    out.refolded_=unfolded_*val;
    return out;
}
void container1DUnfold::setDivideBinomial(bool binomial){
    for(size_t i=0;i<conts_.size();i++)
        conts_.at(i).setDivideBinomial(binomial);
    gencont_.setDivideBinomial(binomial);
    recocont_.setDivideBinomial(binomial);
    unfolded_.setDivideBinomial(binomial);
    refolded_.setDivideBinomial(binomial);
    divideBinomial_=binomial;
}
void container1DUnfold::addErrorContainer(const TString & sysname,const container1DUnfold & cont,float weight){
    if(xbins_ != cont.xbins_ || ybins_ != cont.ybins_){
        std::cout << "container1DUnfold::addErrorContainer: " << name_ << " and " << cont.name_ << " must have same x and y axis!" << std::endl;
    }
    if(debug)
        std::cout << "container1DUnfold::addErrorContainer: adding for all 2d bins" <<std::endl;

    for(size_t i=0;i<conts_.size();i++)
        conts_.at(i).addErrorContainer(sysname,cont.conts_.at(i),weight);
    if(debug)
        std::cout << "container1DUnfold::addErrorContainer: adding for gen and data distr" <<std::endl;
    gencont_.addErrorContainer(sysname,cont.gencont_,weight);
    recocont_.addErrorContainer(sysname,cont.recocont_,weight);
    if(debug)
        std::cout << "container1DUnfold::addErrorContainer: adding for unfolded and refolded distr" <<std::endl;
    unfolded_.addErrorContainer(sysname,cont.unfolded_,weight);
    refolded_.addErrorContainer(sysname,cont.refolded_,weight);
}
void container1DUnfold::addErrorContainer(const TString & sysname,const container1DUnfold & cont){
    addErrorContainer(sysname,cont,1);
}

void container1DUnfold::addGlobalRelError(TString name,float relerr){
    for(size_t i=0;i<conts_.size();i++)
        conts_.at(i).addGlobalRelError(name,relerr);
    gencont_.addGlobalRelError(name,relerr);
    recocont_.addGlobalRelError(name,relerr);
    unfolded_.addGlobalRelError(name,relerr);
    refolded_.addGlobalRelError(name,relerr);
}

void container1DUnfold::getRelSystematicsFrom(const container1DUnfold & cont){
    if(xbins_ != cont.xbins_ || ybins_ != cont.ybins_){
        std::cout << "container1DUnfold::getRelSystematicsFrom: " << name_ << " and " << cont.name_ << " must have same x and y axis!" << std::endl;
    }
    for(size_t i=0;i<conts_.size();i++)
        conts_.at(i).getRelSystematicsFrom(cont.conts_.at(i));
    gencont_.getRelSystematicsFrom(cont.gencont_);
    recocont_.getRelSystematicsFrom(cont.recocont_);
    unfolded_.getRelSystematicsFrom(cont.unfolded_);
    refolded_.getRelSystematicsFrom(cont.refolded_);
}
bool container1DUnfold::checkCongruentBinBoundariesXY() const{
    if(xbins_.size() <2|| ybins_.size() <2)
        return false; //emtpy
    return checkCongruence(genbins_,ybins_);
}
/**
 * assumes  ybins >= xbins. should be the case for container1DUnfold
 */
histoBin container1DUnfold::getDiagonalBin(const size_t & xbin, int layer) const{
    ///NEW rewrite for symmetric internal binning (pretty simple)
    if(xbin>=xbins_.size()){
        std::cout << "container1DUnfold::getDiagonalBin: bin out of range" <<std::endl;
        throw std::out_of_range("container1DUnfold::getDiagonalBin: bin out of range");
    }
    if(!congruentbins_){
        std::cout << "container1DUnfold::getDiagonalBin: only works for congruent bin boundaries" <<std::endl;
        return histoBin();
    }
    if(xbin==xbins_.size()-1){ //overflow bin, just one
        return histoBin(getBin(xbin,ybins_.size()-1,layer));
    }

    //add up until reaches bin boundary
    size_t ybin=getBinNoY(xbins_.at(xbin));
    histoBin out(getBin(xbin,ybin,layer));
    for(ybin=getBinNoY(xbins_.at(xbin)+1);ybin<getBinNoY(xbins_.at(xbin+1)) ;ybin++){
        out.addToContent(getBin(xbin,ybin,layer).getContent());
        out.setEntries(out.getEntries() + getBin(xbin,ybin,layer).getEntries());
        out.setStat2(out.getStat2() + getBin(xbin,ybin,layer).getStat2());
    }
    return out;
}

container1D container1DUnfold::getDiagonal()const{
    container1D out=getRecoContainer();
    for(int sys=-1;sys<(int)getSystSize();sys++){
        for(size_t bin=0;bin<out.getBins().size();bin++){
            out.getBin(bin,sys) = getBin(bin,bin,sys);
        }
    }
    return out;
}

bool container1DUnfold::check(){ ///NEW

    return true;

    if(!binbybin_ && ybins_.size() == xbins_.size()){ // recobins==genbins and no binbybin -> rebin reco
        std::cout << "container1DUnfold::check: warning! rebinning (factor 4) reco part of " <<name_ << std::endl;
        std::vector<float> oldybins=ybins_;
        oldybins.erase(oldybins.begin()); //erase UF bin
        std::vector<float> newybins=subdivide(oldybins,4);
        setBinning(xbins_,newybins);
    }
    return true;
}
container1D container1DUnfold::getPurity() const{
    bool mklist=container1D::c_makelist;
    container1D::c_makelist=false;
    if(!congruentbins_){
        container1D c;
        container1D::c_makelist=mklist;
        return c;
    }
    //rebin to out binning
    container1DUnfold rebinned=rebinToBinning(genbins_);
    container1D rec=rebinned.getRecoContainer();//projectToY(true);//UFOF? include "BG"
    //  container1D gen=rebinned.getGenContainer();

    container1D recgen=rebinned.getDiagonal();

    ///NEW
    /*
    for(int sys=-1;sys<(int)getSystSize();sys++)
        for(size_t bin=0;bin<recgen.getBins().size();bin++){
            recgen.getBin(bin,sys) = getDiagonalBin(bin,sys);}
     */

    //assume a "good" response matrix with most elements being diagonal, so assume stat correlation between both
    bool temp=histoContent::divideStatCorrelated;
    histoContent::divideStatCorrelated=true;
    container1D purity=recgen/rec;
    histoContent::divideStatCorrelated=temp;
    container1D::c_makelist=mklist;
    return purity;


}

container1D container1DUnfold::getStability(bool includeeff) const{

    bool mklist=container1D::c_makelist;
    container1D::c_makelist=false;
    if(!congruentbins_){
        std::cout << "container1DUnfold::getPurity: plotting stability not possible because of non fitting bin boundaries for " << name_ << std::endl;
        container1D c;
        container1D::c_makelist=mklist;
        return c;
    }
    container1DUnfold rebinned=rebinToBinning(genbins_);
    container1D gen=rebinned.projectToX(includeeff);//UFOF? include "BG"

    //rec.rebinToBinning(getGenContainer());
    container1D recgen=rebinned.getDiagonal();

    ///NEW
    /*
    for(int sys=-1;sys<(int)getSystSize();sys++)
        for(size_t bin=0;bin<recgen.getBins().size();bin++)
            recgen.getBin(bin,sys) = getDiagonalBin(bin,sys);
     */

    //asume a "good" response matrix with most elements being diagonal, so assume stat correlation between both
    bool temp=histoContent::divideStatCorrelated;
    histoContent::divideStatCorrelated=true;
    container1D stability=recgen/gen;
    histoContent::divideStatCorrelated=temp;

    container1D::c_makelist=mklist;
    return stability;

}
container2D container1DUnfold::getResponseMatrix()const{
    container2D out;

    out.conts_=conts_; //! for each y axis bin one container
    out.xbins_=xbins_;
    out.ybins_=ybins_;
    out.divideBinomial_=divideBinomial_;
    out.mergeufof_=mergeufof_;

    out.xaxisname_=xaxis1Dname_ + " - gen";
    out.yaxisname_=xaxis1Dname_ + " - reco";
    out.name_=name_+"_respMatrix";
    return out;
}

/**
 * TH2D * prepareRespMatrix(bool nominal=true,unsigned int systNumber=0)
 * -prepares matrix and does some checks for nominal or systematic variation
 *
 * uses generator binning given
 */
TH2D * container1DUnfold::prepareRespMatrix(bool nominal,unsigned int systNumber) const{ //for each syste
    if(getNBinsX() < 1 || getNBinsY() <1)
        return 0;
    if(!nominal && systNumber>=getSystSize()){
        std::cout << "container1DUnfold::prepareRespMatrix: systNumber "<< systNumber << " out of range (" << getSystSize()-1 << ")"<< std::endl;
        return 0;
    }
    /* some consistency checks
     * check whether everything was filled consistently
     * if so, underflow+all conts should be the same as gencont.
     */
    float genint=gencont_.integral(false);
    float allint=0;
    for(size_t i=0;i<conts_.size();i++){
        allint+=conts_.at(i).integral(false);
    }
    if(fabs((genint-allint)/genint) > 0.01){
        std::cout << "container1DUnfold::prepareRespMatrix: something went wrong in the filling process (wrong UF/OF?)  of " << name_ <<  "\n genint: "<< genint
                << " response matrix int: "<< allint << std::endl;
        //return 0;
    }

    //prepare rebinned containers
    container2D rebinned=getResponseMatrix().rebinXToBinning(genbins_);


    if(nominal)
        return rebinned.getTH2D("respMatrix_nominal",false,true);

    /*
     * systematics.
     * naming of histograms changed to make multithreading (maybe) possible
     * assume syst stat of same order as nominal stat.
     */
    TH2D * h=rebinned.getTH2DSyst("respMatrix_"+getSystErrorName(systNumber),systNumber,false,true);
    return h;

}



TString container1DUnfold::coutUnfoldedBinContent(size_t bin) const{
    if(!isBinByBin()){
        std::cout << "container1DUnfold::coutUnfoldedBinContent: only implemented for bin-by-bin unfolded distributions so far, doing nothing" <<std::endl;
        return "";
    }
    if(bin >= unfolded_.getBins().size()){
        std::cout << "container1DUnfold::coutUnfoldedBinContent: bin out of range, doing nothing" << std::endl;
        return "";
    }
    TString out;
    std::cout << "relative background: " <<std::endl;
    out+="relative background: \n";
    container1D treco=getRecoContainer();
    bool temp=histoContent::divideStatCorrelated;
    histoContent::divideStatCorrelated=false;
    out+=(getBackground()/treco).coutBinContent(bin);
    histoContent::divideStatCorrelated=temp;
    std::cout << "unfolded: " <<std::endl;
    out+="unfolded: \n";
    out+=getUnfolded().coutBinContent(bin);
    return out;
}

///////////////

void container1DUnfold::flushAllListed(){
    for(size_t i=0;i<container1DUnfold::c_list.size();i++)
        container1DUnfold::c_list.at(i)->flush();
}

void container1DUnfold::setAllListedMC(bool ISMC){
    for(size_t i=0;i<container1DUnfold::c_list.size();i++)
        container1DUnfold::c_list.at(i)->setMC(ISMC);
}

void container1DUnfold::setAllListedLumi(float lumi){
    for(size_t i=0;i<container1DUnfold::c_list.size();i++)
        container1DUnfold::c_list.at(i)->setLumi(lumi);
}
void container1DUnfold::checkAllListed(){
    for(size_t i=0;i<container1DUnfold::c_list.size();i++)
        container1DUnfold::c_list.at(i)->check();
}

std::vector<float> container1DUnfold::subdivide(const std::vector<float> & bins, size_t div){
    std::vector<float> out;
    for(size_t i=0;i<bins.size()-1;i++){
        float width=bins.at(i+1)-bins.at(i);
        if((int)i<(int)bins.size()-2){
            for(size_t j=0;j<div;j++)
                out.push_back(bins.at(i)+j*width/div);
        }
        else{
            for(size_t j=0;j<=div;j++)
                out.push_back(bins.at(i)+j*width/div);
        }
    }
    return out;
}
/**
 * b needs to be larger than a
 */
bool container1DUnfold::checkCongruence(const std::vector<float>& a, const std::vector<float>& b) const{
    std::vector<float> sames(b.size()); //always larger than xbins
    std::vector<float>::iterator it=std::set_intersection(a.begin(),a.end(),b.begin(),b.end(),sames.begin());
    sames.resize(it-sames.begin());
    return sames.size() == a.size();
}



//////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////IO////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void container1DUnfold::loadFromTree(TTree *t, const TString & plotname){
    if(!t || t->IsZombie()){
        throw std::runtime_error("container1DUnfold::loadFromTree: tree not ok");
    }
    ztop::container1DUnfold * cuftemp=0;
    if(!t->GetBranch("container1DUnfolds")){
        throw std::runtime_error("container1DUnfold::loadFromTree: branch container1DUnfolds not found");
    }
    bool found=false;
    size_t count=0;

    t->SetBranchAddress("container1DUnfolds", &cuftemp);
    for(float n=0;n<t->GetEntries();n++){
        t->GetEntry(n);
        if(cuftemp->getName()==(plotname)){
            found=true;
            count++;
            *this=*cuftemp;
        }
    }

    if(!found){
        std::cout << "searching for: " << plotname << "... error!" << std::endl;
        throw std::runtime_error("container1DUnfold::loadFromTree: no container with name not found");
    }
    if(count>1){
        std::cout << "container1DUnfold::loadFromTree: found more than one object with name "
                << getName() << ", took the first one." << std::endl;
    }
}
void container1DUnfold::loadFromTFile(TFile *f, const TString & plotname){
    if(!f || f->IsZombie()){
        throw std::runtime_error("container1DUnfold::loadFromTFile: file not ok");
    }
    AutoLibraryLoader::enable();
    TTree * ttemp = (TTree*)f->Get("container1DUnfolds");
    loadFromTree(ttemp,plotname);
    delete ttemp;
}
void container1DUnfold::loadFromTFile(const TString& filename,
        const TString & plotname){
    TFile * ftemp=new TFile(filename,"read");
    loadFromTFile(ftemp,plotname);
    delete ftemp;
}

void container1DUnfold::writeToTree(TTree *t){
    if(!t || t->IsZombie()){
        throw std::runtime_error("container1DUnfold::writeToTree: tree not ok");
    }
    ztop::container1DUnfold * cufpointer=this;
    TBranch *b=0;
    if(t->GetBranch("container1DUnfolds")){
        t->SetBranchAddress("container1DUnfolds", &cufpointer, &b);
    }
    else{
        b=t->Branch("container1DUnfolds",&cufpointer);
    }

    t->Fill();
    t->Write(t->GetName(),TObject::kOverwrite);
}
void container1DUnfold::writeToTFile(TFile *f){
    if(!f || f->IsZombie()){
        throw std::runtime_error("container1DUnfold::loadFromTFile: file not ok");
    }
    f->cd();
    TTree * ttemp = (TTree*)f->Get("container1DUnfolds");
    if(!ttemp || ttemp->IsZombie())//create
        ttemp = new TTree("container1DUnfolds","container1DUnfolds");
    writeToTree(ttemp);
    delete ttemp;
}
void container1DUnfold::writeToTFile(const TString& filename){
    TFile * ftemp=new TFile(filename,"update");
    if(!ftemp || ftemp->IsZombie()){
        delete ftemp;
        ftemp=new TFile(filename,"create");
    }
    writeToTFile(ftemp);
    delete ftemp;
}


}


