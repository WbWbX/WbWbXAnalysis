/*
 * container2D.cc
 *
 *  Created on: Jun 24, 2013
 *      Author: kiesej
 */



#include "../interface/container2D.h"



namespace ztop{

bool container2D::debug=false;
std::vector<container2D*> container2D::c_list;
bool container2D::c_makelist=false;

container2D::container2D(): divideBinomial_(true),mergeufof_(false),xaxisname_(""),yaxisname_(""),name_("") {
	if(c_makelist){
		c_list.push_back(this);
	}
}

container2D::~container2D(){
	for(size_t i=0;i<c_list.size();i++){
		if(this==c_list.at(i))
			c_list.erase(c_list.begin()+i);
		break;
	}
}

container2D::container2D( std::vector<float> xbins,std::vector<float> ybins, TString name,TString xaxisname,TString yaxisname, bool mergeufof) : mergeufof_(mergeufof), xaxisname_(xaxisname), yaxisname_(yaxisname), name_(name)
{
	//create for each ybin (plus UF OF) a container1D
	bool temp=container1D::c_makelist;
	container1D::c_makelist=false;


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
	//ybins_.push_back(0);

	for(size_t i=0;i<ybins_.size();i++){ //OF and UF
		container1D temp(xbins,"","","",mergeufof);
		temp.setDivideBinomial(divideBinomial_);
		conts_.push_back(temp);
	}
	xbins_=conts_.at(0).getBins();

	if(container2D::debug){
		std::cout << "containers size: " << conts_.size() << " ybins size: " << ybins_.size() << std::endl;
	}
	container1D::c_makelist=temp;
	if(container2D::c_makelist)
		c_list.push_back(this);
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
	return conts_.at(ybin).getBinErrorUp(xbin,  onlystat, limittosys);
}
double container2D::getBinErrorDown(int xbin, int ybin, bool onlystat,TString limittosys){
	if(ybin<0 || (unsigned int)ybin>ybins_.size()){
		std::cout << "container2D::getBinErrorDown: ybin out of range! return -1" << std::endl;
		return -1.;
	}
	return conts_.at(ybin).getBinErrorDown(xbin,  onlystat, limittosys);
}
double container2D::getBinError(int xbin, int ybin, bool onlystat,TString limittosys){
	if(ybin<0 || (unsigned int)ybin>ybins_.size()){
		std::cout << "container2D::getBinError: ybin out of range! return -1" << std::endl;
		return -1.;
	}
	return conts_.at(ybin).getBinError(xbin,  onlystat, limittosys);
}

double  container2D::getSystError(unsigned int number, int xbin, int ybin){
	if((size_t)ybin>=ybins_.size() || (size_t)xbin>=xbins_.size()){
		std::cout << "container2D::getSystError: xbin or ybin out of range" << std::endl;
		return 0;
	}
	container1D * c=&conts_.at((size_t)ybin);
	return c->getSystError(number,xbin);
}
TString  container2D::getSystErrorName(unsigned int number){
	if(conts_.size()<1){
		std::cout << "container2D::getSystError: number "<< number << " out of range (" << getSystSize()-1 << ")"<< std::endl;
		return "";
	}
	if(conts_.at(0).getSystSize()<1){

		return "";
	}
	return conts_.at(0).getSystErrorName(number);
}
double container2D::projectBinContentToY(int ybin,bool includeUFOF){
	if((size_t)ybin >= ybins_.size()){
		std::cout << "container2D::projectBinToY: ybin out of range" << std::endl;
		return 0;
	}
	container1D * c=&conts_.at(ybin);
	return c->integral(includeUFOF);

}
double container2D::projectBinContentToX(int xbin,bool includeUFOF){
	if((size_t)xbin >= xbins_.size()){
		std::cout << "container2D::projectBinToX: xbin out of range" << std::endl;
	}
	int lowestybin=0;
	int highestybin=ybins_.size()-1;
	if(!includeUFOF){
		lowestybin++;
		highestybin--;
	}
	if(lowestybin>=highestybin){
		std::cout << "container2D::projectBinToX: cannot project to X, too less ybins" <<std::endl;
	}
	double content=0;
	for(int ybin=lowestybin;ybin<=highestybin;ybin++){
		content+=conts_.at(ybin).getBinContent(xbin);
	}
	return content;
}
container1D container2D::projectToX(bool includeUFOF){
	if(ybins_.size()<2 || xbins_.size()<2){//not real container
		std::cout << "container2D::projectToX: plotting purity not possible because of too less bins for " << name_ << std::endl;
		return container1D();
	}

return container1D();
}
container1D container2D::projectToY(bool includeUFOF){
	bool temp=container1D::c_makelist;
	container1D::c_makelist=false;
	if(ybins_.size()<2 || xbins_.size()<2){//not real container
		std::cout << "container2D::projectToY: plotting purity not possible because of too less bins for " << name_ << std::endl;
		container1D out;
		container1D::c_makelist=temp;
		return out;
	}
	std::vector<float> newbins=ybins_;
	newbins.erase(newbins.begin()); //erase underflow
	container1D out(newbins,name_+"_yProjection",yaxisname_,""); //underflow recreated
	for(int ybin=0;ybin<(int)ybins_.size();ybin++){
		int xbincont=ybin;
		container1D * ycont=&conts_.at(ybin);
		double content=ycont->integral(includeUFOF);
		//double staterr=ycont
		//ycont->sys

		///UNDER CONSTR
	}


	container1D::c_makelist=temp;
	return out;
}

/**
 * on long term, put this in own class responsible for all drawing stuff (same for 1D containers)
 * maybe something like a histodrawer class (handles 2D, 1D, UF, ...) and each object has own style
 * then, container1D, 2D, 1DUF can be independent of root
 * Stack could be, too, but again needs drawer class (can use different drawer class objects internally)
 * may even use other graphic representations as root
 * in principle drawing of systematics etc can be managed by the class .setDrawStatOnly, .drawSyst...., .setStyle, .setStyle.setMarker/Line.... (style is own new struct/class)
 */
TH2D * container2D::getTH2D(TString name, bool dividebybinwidth, bool onlystat){
	if(getNBinsX() < 1 || getNBinsY() <1)
		return 0; //there is not even one real, non OF or UF container
	if(dividebybinwidth)
		std::cout << "container2D::getTH2D: dividebybinwidth not supported yet!!" << std::endl;

	TH2D * h=new TH2D(name,name,getNBinsX(),&(xbins_.at(1)),getNBinsY(),&(ybins_.at(1)));
	double entries=0;
	for(int xbin=0;xbin<=getNBinsX()+1;xbin++){ // 0 underflow, genBins+1 overflow included!!
		for(int ybin=0;ybin<=getNBinsY()+1;ybin++){
			entries+=getBinEntries(xbin,ybin);
			double cont=getBinContent(xbin,ybin);
			h->SetBinContent(xbin,ybin,cont);
			double err=getBinError(xbin,ybin,onlystat);
			h->SetBinError(xbin,ybin,err);
		}
	}
	h->SetEntries(entries);

	return h;
}
/**
 * TH2D * container2D::getTH2DSyst(TString name, unsigned int systNo, bool dividebybinwidth=false, bool statErrors=false)
 */
TH2D * container2D::getTH2DSyst(TString name, unsigned int systNo, bool dividebybinwidth, bool statErrors){
	TH2D * h=getTH2D(name,dividebybinwidth,true);
	if(!h) return 0;

	for(int xbin=0;xbin<=getNBinsX()+1;xbin++){ // 0 underflow, genBins+1 overflow included!!
		for(int ybin=0;ybin<=getNBinsY()+1;ybin++){
			double cont=getBinContent(xbin,ybin)+getSystError(systNo,xbin,ybin);
			h->SetBinContent(xbin,ybin,cont);
			if(!statErrors)
				h->SetBinError(xbin,ybin,0);
		}
	}
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
void container2D::removeError(TString err){
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i).removeError(err);
	}
}
void container2D::renameSyst(TString old, TString New){
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i).renameSyst(old,New);
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
void container2D::addGlobalRelError(TString errname,double val){
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i).addGlobalRelError(errname,val);
	}
}

}//namespace
