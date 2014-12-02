/*
 * container1DUnfold.cc
 *
 *  Created on: Aug 15, 2013
 *      Author: kiesej
 */

#include "../interface/container1DUnfold.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "../interface/systAdder.h"


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

container1DUnfold::container1DUnfold(): container2D(),xaxis1Dname_(""), yaxis1Dname_(""),
		tempgen_(0),tempreco_(0),tempgenweight_(1),tempweight_(1),recofill_(false),
		genfill_(false),isMC_(false),flushed_(true),binbybin_(false),lumi_(1),congruentbins_(false),allowmultirecofill_(false){
	if(c_makelist){
		c_list.push_back(this);
	}
	type_=type_container1DUnfold;
}
container1DUnfold::container1DUnfold( std::vector<float> genbins, std::vector<float> recobins, TString name,TString xaxisname,TString yaxisname, bool mergeufof)
:container2D( recobins /*genbins*/ , recobins , name,xaxisname+" (gen)",xaxisname+" (reco)",mergeufof), xaxis1Dname_(xaxisname),
 yaxis1Dname_(yaxisname),tempgen_(0),tempreco_(0),tempgenweight_(1),tempweight_(1),recofill_(false),genfill_(false),
 isMC_(false),flushed_(true),binbybin_(false),lumi_(1),congruentbins_(false),allowmultirecofill_(false){
	//bins are set, containers created, at least conts_[0] exists with all options (binomial, mergeufof etc)



	genbins_=genbins; //can be changed and rebinned afterwards

	//everything in same binning for now
	std::vector<float> databins=ybins_;
	databins.erase(databins.begin());
	gencont_  = container1D(databins,name_+"_gen",xaxis1Dname_,yaxis1Dname_,mergeufof_);
	recocont_ = container1D(databins,name_+"_data",xaxis1Dname_,yaxis1Dname_,mergeufof_);
	unfolded_ = container1D(databins,name_+"_unfolded",xaxis1Dname_,yaxis1Dname_,mergeufof_);
	refolded_ = container1D(databins,name_+"_refolded",xaxis1Dname_,yaxis1Dname_,mergeufof_);
	congruentbins_=checkCongruentBinBoundariesXY();
	if(c_makelist){
		c_list.push_back(this);
	}
	type_=type_container1DUnfold;
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
	gencont_.setXAxisName(xaxis1Dname_+"(gen)");
	std::vector<float> databins=ybins_;
	databins.erase(databins.begin());
	recocont_ = container1D(databins,name_+"_data",xaxis1Dname_,yaxis1Dname_,mergeufof_);
	unfolded_ = container1D(databins,name_+"_unfolded",xaxis1Dname_,yaxis1Dname_,mergeufof_);
	refolded_ = container1D(databins,name_+"_refolded",xaxis1Dname_,yaxis1Dname_,mergeufof_);
	congruentbins_=checkCongruentBinBoundariesXY();
}
void container1DUnfold::setGenBinning(const std::vector<float> & genbins){
	if(checkCongruence(genbins,xbins_)){
		genbins_=genbins;
	}
	else{
		throw std::logic_error("container1DUnfold::setGenBinning: Binning does not match bin boundaries");
	}
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

void container1DUnfold::mergePartialVariations(const TString& identifier,bool strictpartialID){
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i).mergePartialVariations(identifier,strictpartialID);
	}
	recocont_.mergePartialVariations(identifier,strictpartialID);
	unfolded_.mergePartialVariations(identifier,strictpartialID);
	gencont_.mergePartialVariations(identifier,strictpartialID);
	refolded_.mergePartialVariations(identifier,strictpartialID);
}
void container1DUnfold::mergeAllErrors(const TString & mergedname){
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i).mergeAllErrors(mergedname);
	}
	recocont_.mergeAllErrors(mergedname);
	unfolded_.mergeAllErrors(mergedname);
	gencont_.mergeAllErrors(mergedname);
	refolded_.mergeAllErrors(mergedname);
}


void container1DUnfold::mergeVariations(const std::vector<TString>& names, const TString & outname,bool linearly){
	container2D::mergeVariations(names,outname,linearly);

	recocont_.mergeVariations(names,outname,linearly);
	unfolded_.mergeVariations(names,outname,linearly);
	gencont_.mergeVariations(names,outname,linearly);
	refolded_.mergeVariations(names,outname,linearly);
}
void container1DUnfold::mergeVariationsFromFileInCMSSW(const std::string& filename){
	systAdder adder;
	adder.readMergeVariationsFileInCMSSW(filename);
	size_t ntobemerged=adder.mergeVariationsSize();
	for(size_t i=0;i<ntobemerged;i++){

		TString mergedname=adder.getMergedName(i);
		std::vector<TString> tobemerged=adder.getToBeMergedName(i);
		bool linearly=adder.getToBeMergedLinearly(i);

		mergeVariations(tobemerged,mergedname,linearly);
	}
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
	//remove gen  OF
	for(int  sys=-1;sys<(int)getSystSize();sys++){
		getBin(xbins_.size()-1,0,sys).multiply(0);
	}

}
container1D container1DUnfold::getBackground() const{
	if(xbins_.size()<1){
		std::cout << "container1DUnfold::getBackground: No bins!" << std::endl;
		throw std::logic_error("container1DUnfold::getBackground: no bins");
	}
	container1D out=getXSlice(0);
	//add generator overflow
	//	for(int  sys=-1;sys<(int)getSystSize();sys++)
	//	out.getBin(out.getBins().size()-1,sys).add( getBin(xbins_.size()-1,0,sys));

	out+=getXSlice(xbins_.size()-1);
	out.setXAxisName(xaxis1Dname_);
	out.setYAxisName(yaxis1Dname_);
	return out;
}
container1D container1DUnfold::getVisibleSignal() const{
	if(xbins_.size()<1){
		std::cout << "container1DUnfold::getVisibleSignal: No bins!" << std::endl;
		throw std::logic_error("container1DUnfold::getVisibleSignal: no bins");
	}
	container1D out=projectToY(false);
	out.setXAxisName(xaxis1Dname_);
	out.setYAxisName(yaxis1Dname_);
	return  out;//no underflow included -> only visible part of response matrix -> generated and reconstructed in visible PS
}
container1DUnfold& container1DUnfold::operator += (const container1DUnfold & second){
	if(second.xbins_ != xbins_ || second.ybins_ != ybins_){
		std::cout << "container1DUnfold::operator +=: "<< name_ << " and " << second.name_<<" must have same binning! returning *this" << std::endl;
		return *this;
	}
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i) += second.conts_.at(i);
	}
	gencont_+=second.gencont_;
	recocont_+=second.recocont_;
	unfolded_+=second.unfolded_;
	refolded_+=second.refolded_;
	return *this;
}

container1DUnfold container1DUnfold::operator + (const container1DUnfold & second)const{
	container1DUnfold out=*this;
	out+=second;
	return out;
}
container1DUnfold& container1DUnfold::operator -= (const container1DUnfold & second){
	if(second.xbins_ != xbins_ || second.ybins_ != ybins_){
		std::cout << "container1DUnfold::operator -=: "<< name_ << " and " << second.name_<<" must have same binning! returning *this"  << std::endl;
		return *this;
	}

	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i) -= second.conts_.at(i);
	}
	gencont_-=second.gencont_;
	recocont_-=second.recocont_;
	unfolded_-=second.unfolded_;
	refolded_-=second.refolded_;

	return *this;
}
container1DUnfold container1DUnfold::operator - (const container1DUnfold & second)const{
	container1DUnfold out=*this;
	out-=second;
	return out;
}

container1DUnfold& container1DUnfold::operator /= (const container1DUnfold & second){
	if(second.xbins_ != xbins_ || second.ybins_ != ybins_){
		std::cout << "container1DUnfold::operator /: "<< name_ << " and " << second.name_<<" must have same binning! returning *this"  << std::endl;
		return *this;
	}
	if(second.divideBinomial_!=divideBinomial_){
		std::cout << "container1DUnfold::operator /: "<< name_ << " and " << second.name_<<" must have same divide option (divideBinomial). returning *this" <<  std::endl;
		return *this;
	}
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i) /= second.conts_.at(i);
	}
	gencont_/=second.gencont_;
	recocont_/=second.recocont_;
	unfolded_/=second.unfolded_;
	refolded_/=second.refolded_;

	return *this;
}
container1DUnfold container1DUnfold::operator / (const container1DUnfold & second)const{
	container1DUnfold out=*this;
	out/=second;
	return out;
}


container1DUnfold &container1DUnfold::operator *= (const container1DUnfold & second){
	if(second.xbins_ != xbins_ || second.ybins_ != ybins_){
		std::cout << "container1DUnfold::operator *: "<< name_ << " and " << second.name_<<" must have same binning! returning *this"  << std::endl;
		return *this;
	}

	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i) *= second.conts_.at(i);
	}
	gencont_*=second.gencont_;
	recocont_*=second.recocont_;
	unfolded_*=second.unfolded_;
	refolded_*=second.refolded_;

	return *this;
}

container1DUnfold container1DUnfold::operator * (const container1DUnfold & second)const{
	container1DUnfold out=*this;
	out*=second;
	return out;
}

container1DUnfold container1DUnfold::operator * (float val)const{
	container1DUnfold out=*this;
	out*=val;
	return out;
}
container1DUnfold& container1DUnfold::operator *= (float val){
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i) *= val;
	}
	gencont_*= val;
	recocont_*=val;
	unfolded_*=val;
	refolded_*=val;
	return *this;
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
int container1DUnfold::addErrorContainer(const TString & sysname,const container1DUnfold & cont,float weight){
	if(xbins_ != cont.xbins_ || ybins_ != cont.ybins_){
		std::cout << "container1DUnfold::addErrorContainer: " << name_ << " and " << cont.name_ << " must have same x and y axis!" << std::endl;
	}
	if(debug)
		std::cout << "container1DUnfold::addErrorContainer: adding for all 2d bins" <<std::endl;
	int out=0;
	int tmp=0;
	for(size_t i=0;i<conts_.size();i++){
		tmp=conts_.at(i).addErrorContainer(sysname,cont.conts_.at(i),weight);
		if(tmp<0) out=tmp;
	}
	if(debug)
		std::cout << "container1DUnfold::addErrorContainer: adding for gen and data distr" <<std::endl;

	tmp=gencont_.addErrorContainer(sysname,cont.gencont_,weight);
	if(tmp<0) out=tmp;
	tmp=recocont_.addErrorContainer(sysname,cont.recocont_,weight);
	if(tmp<0) out=tmp;
	if(debug)
		std::cout << "container1DUnfold::addErrorContainer: adding for unfolded and refolded distr" <<std::endl;
	tmp=unfolded_.addErrorContainer(sysname,cont.unfolded_,weight);
	if(tmp<0) out=tmp;
	tmp=refolded_.addErrorContainer(sysname,cont.refolded_,weight);
	if(tmp<0) out=tmp;
	return out;
}
int container1DUnfold::addErrorContainer(const TString & sysname,const container1DUnfold & cont){
	return addErrorContainer(sysname,cont,1);
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
void container1DUnfold::addRelSystematicsFrom(const container1DUnfold & cont,bool ignorestat,bool strict){
	if(xbins_ != cont.xbins_ || ybins_ != cont.ybins_){
		std::cout << "container1DUnfold::addRelSystematicsFrom: " << name_ << " and " << cont.name_ << " must have same x and y axis!" << std::endl;
	}
	for(size_t i=0;i<conts_.size();i++)
		conts_.at(i).addRelSystematicsFrom(cont.conts_.at(i),ignorestat,strict);
	gencont_.addRelSystematicsFrom(cont.gencont_,ignorestat,strict);
	recocont_.addRelSystematicsFrom(cont.recocont_,ignorestat,strict);
	unfolded_.addRelSystematicsFrom(cont.unfolded_,ignorestat,strict);
	refolded_.addRelSystematicsFrom(cont.refolded_,ignorestat,strict);
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

container1D container1DUnfold::fold(const container1D& input,bool addbackground) const{
	if(debug) std::cout << "container1DUnfold::fold" <<std::endl;


	//some checks
	if(input.isDummy()){
		throw std::logic_error("container1DUnfold::fold: input is dummy");
	}
	if(isDummy()){
		throw std::logic_error("container1DUnfold::fold: *this is dummy");
	}


	container2D normresp=getNormResponseMatrix();

	/*take care of syst layers. check whether all exist.
	 * in terms of performance improvements, assume input.layersize<*this.layersize
	 * This does not affect the fact that this function should also work if its
	 * the other way around - just slightly slower
	 */
	container1D out=projectToY(); //to get syst layers etc right
	container1D preparedinput=input;

	if(input.getBins() != conts_.at(0).getBins()){
		// throw std::logic_error("container1DUnfold::fold: input bins and response matrix bins not the same");
		if(input.getNBins() < conts_.at(0).getNBins()){
			//try to rebin
			container1DUnfold copy=*this;
			//this will only affect the 2d part... not tooo good, but .. who cares
			for(size_t i=0;i<conts_.size();i++){
				copy.conts_.at(i)=copy.conts_.at(i).rebinToBinning(preparedinput);
			}
			normresp=copy.getNormResponseMatrix();
			out.rebinToBinning(preparedinput);
		}
		else{ //input has more bins than output is supposed to have. Rebin preparedinput
			//try to rebin
			preparedinput.rebinToBinning(conts_.at(0));
		}
	}
	out.mergeLayers(preparedinput);

	std::vector<container1D> & copies=normresp.conts_;



	// prepare new response matrix
	// normalize each column to itself (including UF/OF). This then takes care of efficiencies
	// this step includes ALL systematics


	//do the M x vector  multiplication

	for(size_t row=0;row<copies.size();row++){ //UF (efficiencies) and OF (nothing) excluded
		//get layer asso
		std::map<size_t,size_t> layerasso=copies.at(row).mergeLayers(preparedinput);

		if(debug)
			std::cout << "container1DUnfold::fold: preparedInput layers: " << preparedinput.getSystSize() << std::endl;
		for(int sys=-1;sys<(int)copies.at(row).getSystSize();sys++){
			float sum=0, sumstat2=0,fakeentries=0;

			int inputsys=sys;
			if(sys>-1){
				inputsys=layerasso[(size_t)sys];

				if(debug)
					std::cout << "container1DUnfold::fold: folding for systematic var " <<  copies.at(row).getSystErrorName(sys)
					<< " >> " <<  preparedinput.getSystErrorName(inputsys) << std::endl;
			}
			size_t minbin=0,maxbin=copies.at(row).getBins().size();
			if(true){ //add background later!
				minbin=1;// dont include UF  (visPS migrations + all backgrounds)
				maxbin--;//dont include gen OF (also PS migrations)
			}
			for(size_t bin=minbin;bin<maxbin;bin++){
				sum+= copies.at(row).getBinContent(bin,sys)*preparedinput.getBinContent(bin,inputsys);
				//assume no stat correlation between resp matrix and input
				sumstat2+=copies.at(row).getBin(bin,sys).getStat2() * preparedinput.getBinContent(bin,inputsys) * preparedinput.getBinContent(bin,inputsys);
				sumstat2+=preparedinput.getBin(bin,inputsys).getStat2() * copies.at(row).getBinContent(bin,sys)*copies.at(row).getBinContent(bin,sys);
				fakeentries+=preparedinput.getBinEntries(bin,sys);
			}
			out.getBin(row,sys).setContent(sum);
			out.getBin(row,sys).setStat2(sumstat2);
			out.getBin(row,sys).setEntries(fakeentries);
		}
	}

	if(addbackground)
		out+=getBackground();

	out.setName(input.getName()+"_folded");
	out.setXAxisName(xaxis1Dname_);
	out.setYAxisName(yaxis1Dname_);
	return out;
}

container1D container1DUnfold::getPurity() const{
	bool mklist=container1D::c_makelist;
	container1D::c_makelist=false;
	if(!congruentbins_){
		container1D c;
		container1D::c_makelist=mklist;
		return c;
	}
	//new impl

	//rebin to out binning
	container1DUnfold rebinned=rebinToBinning(genbins_);
	container1D rec=rebinned.getVisibleSignal();
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
	purity.setXAxisName(xaxis1Dname_);
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

	container1D recgen=rebinned.getDiagonal();



	//asume a "good" response matrix with most elements being diagonal, so assume stat correlation between both
	//anyway only affect the error bars
	bool temp=histoContent::divideStatCorrelated;
	histoContent::divideStatCorrelated=true;
	container1D stability=recgen/gen;
	histoContent::divideStatCorrelated=temp;

	container1D::c_makelist=mklist;
	stability.setXAxisName(xaxis1Dname_);
	return stability;

}

container1D container1DUnfold::getEfficiency()const{
	container1D xprojection=projectToX(true);
	container1D xprojectionwouf=projectToX(false);
	bool tmp=histoContent::divideStatCorrelated;
	histoContent::divideStatCorrelated=true;
	xprojectionwouf =xprojectionwouf/ xprojection;
	xprojectionwouf.setYAxisName("#epsilon");
	xprojectionwouf.transformToEfficiency();
	xprojectionwouf.setXAxisName(xaxis1Dname_ +" (gen)");
	histoContent::divideStatCorrelated=tmp;
	return xprojectionwouf;
}

container2D container1DUnfold::getResponseMatrix()const{
	container2D out;

	out.conts_=conts_; //! for each y axis bin one container
	out.xbins_=xbins_;
	out.ybins_=ybins_;
	out.divideBinomial_=divideBinomial_;
	out.mergeufof_=mergeufof_;


	out.xaxisname_=xaxis1Dname_ + " (gen)";
	out.yaxisname_=xaxis1Dname_ + " (reco)";
	out.name_=name_+"_respMatrix";
	return out;
}
container2D container1DUnfold::getNormResponseMatrix()const{
	container2D out;

	out.conts_=conts_; //! for each y axis bin one container
	container1D xprojection=projectToX(true);

	for(size_t i=0;i<out.conts_.size();i++){
		out.conts_.at(i)/=xprojection;
	}
	out.xbins_=xbins_;
	out.ybins_=ybins_;
	out.divideBinomial_=divideBinomial_;
	out.mergeufof_=mergeufof_;

	out.xaxisname_=xaxis1Dname_ + " (gen)";
	out.yaxisname_=xaxis1Dname_ + " (reco)";
	out.name_=name_+"_normrespMatrix";
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
	TH2D * h=0;
#pragma omp critical //protect new TH2D
	{
		if(nominal)
			h= rebinned.getTH2D("respMatrix_nominal",false,true);
		else
			h=rebinned.getTH2DSyst("respMatrix_"+getSystErrorName(systNumber),systNumber,false,true);
	} //critical
	return h;

}



TString container1DUnfold::coutUnfoldedBinContent(size_t bin,const TString& units) const{
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
	out+=(getBackground()/treco).coutBinContent(bin,units);
	histoContent::divideStatCorrelated=temp;
	std::cout << "unfolded: " <<std::endl;
	out+="unfolded: \n";
	out+=getUnfolded().coutBinContent(bin,units);
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


////////////////////////////////////////////////FILLING////////////////////////////////////////////////

void container1DUnfold::flush(){ //only for MC
	if(!flushed_){
		if(genfill_)
			gencont_.fill(tempgen_,tempgenweight_);
		if(recofill_)
			recocont_.fill(tempreco_,tempweight_);

		if(genfill_ && !recofill_){ //put in Reco UF bins
			fill(tempgen_,ybins_[1]-100,tempgenweight_);}
		else if(recofill_ && !genfill_){ //put in gen underflow bins -> goes to background
			fill(xbins_[1]-100,tempreco_,tempweight_);}
		else if(genfill_ && recofill_){
			fill(tempgen_,tempreco_,tempweight_);
			fill(tempgen_,ybins_[1]-100,(tempgenweight_-tempweight_)); // w_gen * (1 - recoweight), tempweight_=fullweight=tempgenweight_*recoweight
		}
	}
	recofill_=false;
	genfill_=false;
	flushed_=true;

}


void container1DUnfold::fillGen(const float & val, const float & weight){
	if(genfill_){
		throw std::logic_error("container1DUnfold::fillGen: Attempt to fill gen (twice) without flushing");
	}
	tempgen_=val;
	tempgenweight_=weight;
	genfill_=true;
	flushed_=false;
}

void container1DUnfold::fillReco(const float & val, const float & weight){ //fills and resets tempgen_
	if(recofill_ && !allowmultirecofill_){
		throw std::logic_error("container1DUnfold::fillReco: Attempt to fill reco (twice) without flushing");
	}
	tempreco_=val;
	tempweight_=weight;
	recofill_=true;
	flushed_=false;
	if(allowmultirecofill_){
		if(genfill_)
			gencont_.fill(tempgen_,tempgenweight_);
		if(recofill_)
			recocont_.fill(tempreco_,tempweight_);

		if(genfill_ && !recofill_){ //put in Reco UF bins
			fill(tempgen_,ybins_[1]-100,tempgenweight_);}
		else if(recofill_ && !genfill_){ //put in gen underflow bins -> goes to background
			fill(xbins_[1]-100,tempreco_,tempweight_);}
		else if(genfill_ && recofill_){
			fill(tempgen_,tempreco_,tempweight_);
			fill(tempgen_,ybins_[1]-100,(tempgenweight_-tempweight_)); // w_gen * (1 - recoweight), tempweight_=fullweight=tempgenweight_*recoweight
		}

		//recofill_=false;
		flushed_=true;
	}

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
	AutoLibraryLoader::enable();
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

bool container1DUnfold::TFileContainsContainer1DUnfolds(TFile *f){
	if(!f || f->IsZombie()){
		throw std::runtime_error("container1DUnfold::TFileContainsContainer1DUnfolds: file not ok");
	}
	TTree * t = (TTree*)f->Get("container1DUnfolds");
	if(!t || t->IsZombie()){
		if(t) delete t;
		return false;
	}
	if(t->GetEntries()<1){
		delete t;
		return false;
	}
	delete t;
	return true;

}
bool container1DUnfold::TFileContainsContainer1DUnfolds(const TString & filename){
	TFile *  f = new TFile(filename, "READ");
	return TFileContainsContainer1DUnfolds(f);
}


}


