/*
 * container2D.cc
 *
 *  Created on: Jun 24, 2013
 *      Author: kiesej
 */



#include "../interface/container2D.h"



namespace ztop{

bool container2D::debug=false;

container2D::container2D( std::vector<float> xbins,std::vector<float> ybins, TString name,TString xaxisname,TString yaxisname, bool mergeufof){
	//create for each ybin (plus UF OF) a container1D

	xaxisname_=xaxisname;
	yaxisname_=yaxisname;

	float lastbin;
	for(size_t i=0;i<ybins.size();i++){
		if(i<1){
			lastbin=ybins.at(i);
		}
		else if(lastbin >= ybins.at(i)){
			std::cout << "container2D::setBins: bins must be in increasing order!" << std::endl;
			std::exit(EXIT_FAILURE);
		}
	}


	ybins_.push_back(0); //UF
	ybins_.insert(ybins_.end(),ybins.begin(),ybins.end()); //OF is the last one


	for(size_t i=0;i<ybins.size()+1;i++){ //OF and UF
		container1D temp(xbins,"","","",mergeufof);
		temp.setDivideBinomial(divideBinomial_);
		conts_.push_back(temp);
	}
	xbins_=conts_.at(0).getBins();

	if(container2D::debug){
		std::cout << "containers size: " << conts_.size() << " ybins size: " << ybins_.size() << std::endl;
	}
}

double container2D::getBinContent(int xbin,int ybin){
	if((unsigned int)xbin>xbins_.size() || (unsigned int)ybin> ybins_.size()){
		std::cout << "container2D::getBinContent: xbin or ybin is out of range!! return -1" << std::endl;
		return -1.;
	}
	return conts_.at(ybin).getBinContent(xbin);
}
double container2D::getBinEntries(int xbin, int ybin){
	if(ybin<0 || (unsigned int)ybin>ybins_.size()){
		std::cout << "container2D::getBinEntries: ybin out of range! return -1" << std::endl;
		return -1.;
	}
	return conts_.at(ybin).getBinEntries(xbin);
}
double container2D::getBinErrorUp(int xbin, int ybin, bool onlystat,TString limittosys){
	if(ybin<0 || (unsigned int)ybin>ybins_.size()){
		std::cout << "container2D::getBinErrorUp: ybin out of range! return -1" << std::endl;
		return -1.;
	}
	return conts_.at(ybin).getBinErrorUp(xbin);
}
double container2D::getBinErrorDown(int xbin, int ybin, bool onlystat,TString limittosys){
	if(ybin<0 || (unsigned int)ybin>ybins_.size()){
		std::cout << "container2D::getBinErrorDown: ybin out of range! return -1" << std::endl;
		return -1.;
	}
	return conts_.at(ybin).getBinErrorDown(xbin);
}
double container2D::getBinError(int xbin, int ybin, bool onlystat,TString limittosys){
	if(ybin<0 || (unsigned int)ybin>ybins_.size()){
		std::cout << "container2D::getBinError: ybin out of range! return -1" << std::endl;
		return -1.;
	}
	return conts_.at(ybin).getBinError(xbin);
}
/**
 * on long term, put this in own class responsible for all drawing stuff (same for 1D containers)
 * maybe something like a histodrawer class (handles 2D, 1D, UF, ...) and each object has own style
 * then, container1D, 2D, 1DUF can be independent of root
 * Stack could be, too, but again needs drawer class (can use different drawer class objects internally)
 * may even use other graphic representations as root
 * in principle drawing of systematics etc can be managed by the class .setDrawStatOnly, .drawSyst...., .setStyle, .setStyle.setMarker/Line.... (style is own new struct/class)
 */
TH2D * container2D::getTH2D(TString name, bool dividebybinwidth){
	if(getNBinsX() < 1 || getNBinsY() <1)
		return 0; //there is not even one real, non OF or UF container

	std::cout << "container2D::getTH2D in only for testing phase" << std::endl;

	TH2D * h=new TH2D(name_,name_,getNBinsX(),&(xbins_.at(1)),getNBinsY(),&(ybins_.at(1)));
	double entries=0;
	for(int xbin=1;xbin<=getNBinsX();xbin++){ // 0 underflow, genBins+1 overflow NOT INCLUDED YET!!
		for(int ybin=1;ybin<=getNBinsY();ybin++){
			entries+=getBinEntries(xbin,ybin);

			double cont=getBinContent(xbin,ybin);
			//if(dividebybinwidth && >0 && i<getNBins()+1) cont=cont/getBinWidth(i);
			h->SetBinContent(xbin,ybin,cont);
			double err=getBinError(xbin,ybin);
			//if(dividebybinwidth && i>0 && i<getNBins()+1) err=err/getBinWidth(i);
			h->SetBinError(xbin,ybin,err);

		}
	}
	h->SetEntries(entries);

	return h;
}

void container2D::setDivideBinomial(bool binomial){
	for(size_t i=0;i<conts_.size();i++)
		conts_.at(i).setDivideBinomial(binomial);
	divideBinomial_=binomial;
}


container2D container2D::operator + (const container2D & second){
	if(second.xbins_ != xbins_ || second.ybins_ != ybins_){
		std::cout << "container2D::operator +: "<< name_ << " and " << second.name_<<" must have same binning! returning *this" << std::endl;
		return *this;
	}
	container2D out=second;
	for(size_t i=0;i<conts_.size();i++){
		out.conts_.at(i) = conts_.at(i) + second.conts_.at(i);
	}
	return out;
}
container2D container2D::operator - (const container2D & second){
	if(second.xbins_ != xbins_ || second.ybins_ != ybins_){
		std::cout << "container2D::operator -: "<< name_ << " and " << second.name_<<" must have same binning! returning *this"  << std::endl;
		return *this;
	}
	container2D out=second;
	for(size_t i=0;i<conts_.size();i++){
		out.conts_.at(i) = conts_.at(i) - second.conts_.at(i);
	}
	return out;
}
container2D container2D::operator / (const container2D & second){
	if(second.xbins_ != xbins_ || second.ybins_ != ybins_){
		std::cout << "container2D::operator /: "<< name_ << " and " << second.name_<<" must have same binning! returning *this"  << std::endl;
		return *this;
	}
	if(second.divideBinomial_!=divideBinomial_){
		std::cout << "container2D::operator /: "<< name_ << " and " << second.name_<<" must have same divide option (divideBinomial). returning *this" <<  std::endl;
		return *this;
	}
	container2D out=second;
	for(size_t i=0;i<conts_.size();i++){
		out.conts_.at(i) = conts_.at(i) / second.conts_.at(i);
	}
	return out;
}
container2D container2D::operator * (const container2D & second){
	if(second.xbins_ != xbins_ || second.ybins_ != ybins_){
		std::cout << "container2D::operator *: "<< name_ << " and " << second.name_<<" must have same binning! returning *this"  << std::endl;
		return *this;
	}
	container2D out=second;
	for(size_t i=0;i<conts_.size();i++){
		out.conts_.at(i) = conts_.at(i) * second.conts_.at(i);
	}
	return out;
}
container2D container2D::operator * (double val){
	container2D out=*this;
	for(size_t i=0;i<conts_.size();i++){
		out.conts_.at(i) = conts_.at(i) * val;
	}
	return out;
}
container2D container2D::operator * (float val){
	return *this * (double)val;
}
container2D container2D::operator * (int val){
	return *this * (double) val;
}
void container2D::addErrorContainer(const TString & sysname,const container2D & second ,double weight,bool ignoreMCStat){
	if(second.xbins_ != xbins_ || second.ybins_ != ybins_){
		std::cout << "container2D::addErrorContainer "<< name_ << " and " << second.name_<<" must have same binning!"  << std::endl;
		return;
	}
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i).addErrorContainer(sysname, second.conts_.at(i),weight, ignoreMCStat);
	}
}
void container2D::addErrorContainer(const TString & sysname,const container2D & second ,bool ignoreMCStat){
	addErrorContainer(sysname,second , 1,ignoreMCStat);
}
void container2D::addRelSystematicsFrom(const container2D & second){
	if(second.xbins_ != xbins_ || second.ybins_ != ybins_){
		std::cout << "container2D::addRelSystematicsFrom "<< name_ << " and " << second.name_<<" must have same binning!"  << std::endl;
		return;
	}
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i).addRelSystematicsFrom(second.conts_.at(i));
	}
}
void container2D::reset(){
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i).reset();
	}
}
void container2D::clear(){
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i).clear();
	}
}

}//namespace
