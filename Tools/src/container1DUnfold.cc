/*
 * container1DUnfold.cc
 *
 *  Created on: Aug 15, 2013
 *      Author: kiesej
 */

#include "../interface/container1DUnfold.h"

/*
 * multicore usage for unfolding to be implemented
 */
#include <omp.h>

namespace ztop{


bool container1DUnfold::debug=false;
std::vector<container1DUnfold*> container1DUnfold::c_list;
bool container1DUnfold::c_makelist=false;
bool container1DUnfold::printinfo=false;

container1DUnfold::container1DUnfold(): container2D(), xaxis1Dname_(""), yaxis1Dname_(""),tempgen_(TEMPGENDEFAULT),recofill_(true),isMC_(false),flushed_(false),unfold_(0){
	if(c_makelist){
		c_list.push_back(this);
	}
}
container1DUnfold::container1DUnfold( std::vector<float> genbins, std::vector<float> recobins, TString name,TString xaxisname,TString yaxisname, bool mergeufof):
				container2D( genbins , recobins , name,xaxisname+"_reco",xaxisname+"_gen",mergeufof), xaxis1Dname_(xaxisname), yaxis1Dname_(yaxisname),
				tempgen_(TEMPGENDEFAULT),recofill_(true),isMC_(false),flushed_(false),unfold_(0) {
	//bins are set, containers created, at least conts_[0] exists with all options (binomial, mergeufof etc)
	gencont_=conts_.at(0);
	gencont_.clear();
	std::vector<float> databins=ybins_;
	databins.erase(databins.begin());
	datacont_ = container1D(databins,name_+"_data",xaxis1Dname_,yaxis1Dname_,mergeufof_);
	if(c_makelist){
		c_list.push_back(this);
	}
}

container1DUnfold::~container1DUnfold(){
	for(size_t i=0;i<c_list.size();i++){
		if(this==c_list.at(i))
			c_list.erase(c_list.begin()+i);
		break;
	}
	if(unfold_) delete unfold_;
	for(size_t i=0;i<unfolds_.size();i++)
		if(unfolds_.at(i)) delete unfolds_.at(i);
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
	out.datacont_=datacont_+second.datacont_;
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
	out.datacont_=datacont_-second.datacont_;
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
	out.datacont_=datacont_/second.datacont_;
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
	out.datacont_=datacont_*second.datacont_;
	out.unfolded_=unfolded_*second.unfolded_;
	out.refolded_=refolded_*second.refolded_;
	return out;
}
container1DUnfold container1DUnfold::operator * (double val){
	container1DUnfold out=*this;
	for(size_t i=0;i<conts_.size();i++){
		out.conts_.at(i) = conts_.at(i) * val;
	}
	out.gencont_=gencont_ * val;
	out.datacont_=datacont_*val;
	out.unfolded_=unfolded_*val;
	out.refolded_=unfolded_*val;
	return out;
}
void container1DUnfold::setDivideBinomial(bool binomial){
	for(size_t i=0;i<conts_.size();i++)
		conts_.at(i).setDivideBinomial(binomial);
	gencont_.setDivideBinomial(binomial);
	datacont_.setDivideBinomial(binomial);
	unfolded_.setDivideBinomial(binomial);
	refolded_.setDivideBinomial(binomial);
	divideBinomial_=binomial;
}
void container1DUnfold::addErrorContainer(const TString & sysname,const container1DUnfold & cont,double weight,bool ignoreMCStat){
	if(xbins_ != cont.xbins_ || ybins_ != cont.ybins_){
		std::cout << "container1DUnfold::addErrorContainer: " << name_ << " and " << cont.name_ << " must have same x and y axis!" << std::endl;
	}
	for(size_t i=0;i<conts_.size();i++)
		conts_.at(i).addErrorContainer(sysname,cont.conts_.at(i),weight,ignoreMCStat);
	gencont_.addErrorContainer(sysname,cont.gencont_,weight,ignoreMCStat);
	datacont_.addErrorContainer(sysname,cont.datacont_,weight,ignoreMCStat);
	unfolded_.addErrorContainer(sysname,cont.unfolded_,weight,ignoreMCStat);
	refolded_.addErrorContainer(sysname,cont.refolded_,weight,ignoreMCStat);
}
void container1DUnfold::addErrorContainer(const TString & sysname,const container1DUnfold & cont,bool ignoreMCStat){
	addErrorContainer(sysname,cont,1,ignoreMCStat);
}

void container1DUnfold::addGlobalRelError(TString name,double relerr){
	for(size_t i=0;i<conts_.size();i++)
		conts_.at(i).addGlobalRelError(name,relerr);
	gencont_.addGlobalRelError(name,relerr);
	datacont_.addGlobalRelError(name,relerr);
	unfolded_.addGlobalRelError(name,relerr);
	refolded_.addGlobalRelError(name,relerr);
}

void container1DUnfold::addRelSystematicsFrom(const container1DUnfold & cont){
	if(xbins_ != cont.xbins_ || ybins_ != cont.ybins_){
		std::cout << "container1DUnfold::addRelSystematicsFrom: " << name_ << " and " << cont.name_ << " must have same x and y axis!" << std::endl;
	}
	for(size_t i=0;i<conts_.size();i++)
		conts_.at(i).addRelSystematicsFrom(cont.conts_.at(i));
	gencont_.addRelSystematicsFrom(cont.gencont_);
	datacont_.addRelSystematicsFrom(cont.datacont_);
	unfolded_.addRelSystematicsFrom(cont.datacont_);
	refolded_.addRelSystematicsFrom(cont.refolded_);
}
bool container1DUnfold::checkCongruentBinBoundaries(){
	for(size_t xbin=0;xbin<xbins_.size();xbin++){
		bool fits=false;
		for(size_t ybin=0;ybin<ybins_.size();ybin++){
			if((ybins_.at(ybin)-xbins_.at(xbin))/xbins_.at(xbin) < 0.0001){//relative
				fits=true;
			}
			else if(ybins_.at(ybin)<xbins_.at(xbin)){
				continue;
			}
			if(!fits)
				return false;
		}
	}
	return true;
}

container1D container1DUnfold::getPurity(){
	if(!checkCongruentBinBoundaries()){
		std::cout << "container1DUnfold::getPurity: plotting purity not possible because of non fitting bin boundaries for " << name_ << std::endl;
		return container1D();
	}
	/*if(ybins_.size()<2 || xbins_.size()<2){//not real container
		std::cout << "container1DUnfold::getPurity: plotting purity not possible because of too less bins for " << name_ << std::endl;
		return container1D();
	}
	bool temp=container1D::c_makelist;
	container1D::c_makelist=false;
	container1D nrecgen(xbins_),nrec(xbins_);
	nrecgen.setDivideBinomial(true); //not 100% true but ok if purity is large
	nrec.setDivideBinomial(true);
//use fill

	for(size_t ybin=0;ybin<ybins_.size();ybin++){//use: bin is lower inclusive border
		int xbin=0;
		if(ybin>0)
			xbin=nrecgen.getBinNo(ybins_.at(ybin));
		nrec.fill(ybin,projectBinContentToY(ybin,true));
		nrecgen.fill(ybin,getBinContent(xbin,ybin));
	}
	container1D out=nrecgen/nrec;
	out.setName(name_+"_purity");

	container1D::c_makelist=temp; */
	container1D out;
	return out;
}
container1D container1DUnfold::getStability(bool includeeff){
	/*if(!checkCongruentBinBoundaries()){
		std::cout << "container1DUnfold::getStability: plotting purity not possible because of non fitting bin boundaries for " << name_ << std::endl;
	}
	if(ybins_.size()<2 || xbins_.size()<2){//not real container
		std::cout << "container1DUnfold::getPurity: plotting purity not possible because of too less bins for " << name_ << std::endl;
		return container1D();
	}
	bool temp=container1D::c_makelist;
	container1D::c_makelist=false;
	container1D nrecgen(xbins_),ngen(xbins_);
	nrecgen.setDivideBinomial(true); //not 100% true but ok if purity is large
	nrec.setDivideBinomial(true);
	//use fill

	for(size_t ybin=0;ybin<ybins_.size();ybin++){//use: bin is lower inclusive border
		float value;
		if(ybin<1)
			value=ybins_.at(1)-10000; //force underflow, OF works on its own
		else
			value=ybins_.at(ybin);
		int xbin=nrecgen.getBinNo(value);

		ngen.fill(ybin,proje);
		nrecgen.fill(ybin,conts_.at(ybin).getBinContent(xbin));
		//check if boundaries match and fill other one

	}
	container1D out=nrecgen/nrec;
	out.setName(name_+"_purity");

	container1D::c_makelist=temp; */
	container1D out;
	return out;
}


/**
 * TH2D * prepareRespMatrix(bool nominal=true,unsigned int systNumber=0)
 * -prepares matrix and does some checks for nominal or systematic variation
 */
TH2D * container1DUnfold::prepareRespMatrix(bool nominal,unsigned int systNumber){ //for each syste
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
	double genint=gencont_.integral();
	double allint=0;
	for(size_t i=0;i<conts_.size();i++){
		allint+=conts_.at(i).integral();
	}
	if(fabs((genint-allint)/genint) > 0.001){
		std::cout << "container1DUnfold::prepareRespMatrix: something went wrong in the filling process" << std::endl;
		return 0;
	}

	if(nominal)
		return getTH2D("respMatrix_nominal",false,true);;

/*
 * systematics.
 * naming of histograms changed to make multithreading (maybe) possible
 * assume syst stat of same order as nominal stat.
 */
	TH2D * h=getTH2DSyst("respMatrix_"+getSystErrorName(systNumber),systNumber,false,true);
	return h;

}

int container1DUnfold::unfold(TUnfold::ERegMode regmode, bool LCurve){


	/*
	 *
	 * preparation nominal
	 */
	TH1::AddDirectory(false);
	if(debug)
		std::cout << "container1DUnfold::unfold: preparing response matrix" << std::endl;
	TH2* responsematrix=prepareRespMatrix();
	unfold_ = new unfolder(name_+"_nominal");
	TH1* datahist=getDataContainer().getTH1D(name_+"_datahist_nominal",false);
	if(debug)
		std::cout << "container1DUnfold::unfold: initialising unfolder" << std::endl;
	unfold_->init(responsematrix,datahist,TUnfold::kHistMapOutputHoriz,regmode);
	if(debug)
		std::cout << "container1DUnfold::unfold: scanning L-Curve" << std::endl;
	if(LCurve)
		unfold_->scanLCurve();
	else
		unfold_->scanTau();
	delete responsematrix;
	delete datahist;
	if(debug)
		std::cout << "container1DUnfold::unfold: getting output" << std::endl;
	if(unfold_->ready()){
		unfolded_.import(unfold_->getUnfolded());
		refolded_.import(unfold_->getReUnfolded());
	}
	else{
		std::cout << "container1DUnfold::unfold: unfolding for nominal failed!! skipping rest" << std::endl;
		return -1;
	}
	if(debug)
		std::cout << "container1DUnfold::unfold: unfolding done for nominal" << std::endl;
	//done for nominal

	/*
	 * prepare loop for systematics
	 */
	bool dodatasyst=true;
	if(getDataContainer().getSystSize() != 0 && getDataContainer().getSystSize() != getSystSize()){
		std::cout << "WARNING! container1DUnfold::unfold: ambiguous association of data systematics and MC systematics. "
		<< "data will be unfolded without systematic variations." << std::endl;
		dodatasyst=false;
	}
	if(getDataContainer().getSystSize() <1){
		std::cout << "container1DUnfold::unfold: dataContainer has no info about systematics. If intended, ok, if not, check!" << std::endl;
		dodatasyst=false;
	}
	if(debug)
		std::cout << "container1DUnfold::unfold: init systematics" << std::endl;
	for(size_t sys=0;sys<getSystSize();sys++){
		TH2* SysResponsematrix=prepareRespMatrix(false,sys);
		if(!SysResponsematrix){
			std::cout << "container1DUnfold::unfold: Response matrix for " << getSystErrorName(sys) << " could not be created. stopping unfolding!" << std::endl;
			return -2;
		}
		SysResponsematrix->Draw("COLZ");
		TH1* SysDatahist=0;
		if(dodatasyst)
			SysDatahist=getDataContainer().getTH1DSyst(name_+"_datahist_"+getSystErrorName(sys),sys,false,true);
		else
			SysDatahist=getDataContainer().getTH1D(name_+"_datahist_nominal_fake"+getSystErrorName(sys),false);
		unfolder * uf=new unfolder(name_+"_"+getSystErrorName(sys));
		int verb=uf->init(SysResponsematrix,SysDatahist);
		if(verb>10000){
			std::cout << "container1DUnfold::unfold: init of unfolder for "<< name_<< ": "
					<< getSystErrorName(sys) << " not successful. Will not unfold distribution"<< std::endl;
		}
		unfolds_.push_back(uf);
		delete SysResponsematrix;
		delete SysDatahist;
	}
	/*
	 * there might be an addional loop needed if MC independent data systematics are introduced not given in the list of syst.
	 * if filling is setup properly, this will not happen
	 */


	/*
	 * scanning L-Curve loop for systematics.
	 * parallelise this one!!
	 */
	if(debug)
		std::cout << "container1DUnfold::unfold: scanning L-Curve of systematics" << std::endl;
	for(size_t sys=0;sys<getSystSize();sys++){
		if(debug)
			std::cout << "container1DUnfold::unfold: scanning L-Curve of "<< name_ << ": " << getSystErrorName(sys) << std::endl;
		unfolds_.at(sys)->scanLCurve();
	}
	/*
	 * getting output and merging it into unfolded_ etc. should be ok in serial mode
	 */
	for(size_t sys=0;sys<getSystSize();sys++){
		TH1 * huf=unfolds_.at(sys)->getUnfolded();
		if(!huf){
			std::cout << "container1DUnfold::unfold: unfolding result for " << name_ << ": " << getSystErrorName(sys) << " could not be retrieved. stopping unfolding!" << std::endl;
			return -3;
		}
		container1D syscont;
		syscont.import(huf);
		delete huf;
		unfolded_.addErrorContainer(getSystErrorName(sys),syscont,true); //ignoreMCStat=true
	}



	    return 0;
}


///////////////

void container1DUnfold::flushAllListed(){
	for(size_t i=0;i<container1DUnfold::c_list.size();i++)
		container1DUnfold::c_list.at(i)->flush();
}



}


