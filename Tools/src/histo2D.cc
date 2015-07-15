/*
 * histo2D.cc
 *
 *  Created on: Jun 24, 2013
 *      Author: kiesej
 */



#include "../interface/histo2D.h"
#include <algorithm>
#include "../interface/systAdder.h"
#include "../interface/textFormatter.h"
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>

namespace ztop{

bool histo2D::debug=false;
std::vector<histo2D*> histo2D::c_list;
bool histo2D::c_makelist=false;

histo2D::histo2D():taggedObject(taggedObject::type_container2D), divideBinomial_(true),
		mergeufof_(false),xaxisname_(""),yaxisname_(""),zaxisname_("") {
	setName("");
	std::vector<float> nobins;
	setBinning(nobins,nobins);
	if(c_makelist){
		c_list.push_back(this);
	}
}

histo2D::~histo2D(){
	for(size_t i=0;i<c_list.size();i++){
		if(this==c_list.at(i))
			c_list.erase(c_list.begin()+i);
	}
}
histo2D::histo2D(const histo2D&rhs){
	copyFrom(rhs);
}
histo2D& histo2D::operator=(const histo2D&rhs){
	copyFrom(rhs);
	return *this;
}

histo2D::histo2D(const std::vector<float> &xbins,const std::vector<float> &ybins, TString name,TString xaxisname,TString yaxisname,TString zaxisname, bool mergeufof)
:taggedObject(taggedObject::type_container2D),mergeufof_(mergeufof), xaxisname_(xaxisname), yaxisname_(yaxisname),zaxisname_(zaxisname)
{
	setName(name);
	//create for each ybin (plus UF OF) a histo1D
	setBinning(xbins,ybins);
	if(histo2D::c_makelist)
		c_list.push_back(this);
}

void histo2D::setBinning(const std::vector<float> &xbins, std::vector<float> ybins){



	bool temp=histo1D::c_makelist;
	histo1D::c_makelist=false;
	ybins_.clear();
	conts_.clear();
	xbins_.clear();

	std::sort(ybins.begin(),ybins.end());
	std::vector<float>::iterator it=std::unique(ybins.begin(),ybins.end());
	ybins.resize( std::distance(ybins.begin(),it) );

	ybins_.push_back(0); //UF
	ybins_.insert(ybins_.end(),ybins.begin(),ybins.end()); //OF is the last one
	//ybins_.push_back(0);

	for(size_t i=0;i<ybins_.size();i++){ //OF and UF
		histo1D temp(xbins,"",xaxisname_,"",mergeufof_);
		//temp.setDivideBinomial(divideBinomial_);
		conts_.push_back(temp);
	}
	xbins_=conts_.at(0).getBins();

	if(histo2D::debug){
		std::cout << "containers size: " << conts_.size() << " ybins size: " << ybins_.size() << std::endl;
	}
	histo1D::c_makelist=temp;
}

const float &histo2D::getBinContent(const size_t & xbin,const size_t & ybin) const{
	if(xbin>=xbins_.size() || ybin>= conts_.size()){
		throw std::out_of_range ("histo2D::getBinContent: xbin or ybin is out of range");// << std::endl;
	}
	return conts_.at(ybin).getBinContent(xbin);
}
const uint32_t &histo2D::getBinEntries(const size_t & xbin, const size_t & ybin) const{
	if(xbin>=xbins_.size() || ybin>= conts_.size()){
		throw std::out_of_range ("histo2D::getBinEntries: xbin or ybin is out of range");
	}
	return conts_.at(ybin).getBinEntries(xbin);
}

float histo2D::getBinArea(const size_t& binx, const size_t& biny)const{
	if(isDummy())
		throw std::out_of_range("histo2D::getBinArea: container is dummy");
	if(binx >= xbins_.size()-1 || biny >= ybins_.size()-1 || binx<1 || biny <1){ //doesn't work for underflow, overflow
		throw std::out_of_range("histo2D::getBinArea: bin out of range (or UF,OF)");
	}

	float xw= conts_.at(biny).getBinWidth(binx);
	float yw= ybins_.at(biny+1)-ybins_.at(biny);
	return xw*yw;

}
void histo2D::getBinCenter(const size_t binx,const size_t biny, float& centerx,float& centery)const{
	if(isDummy())
		throw std::out_of_range("histo2D::getBinCenter: container is dummy");
	if(binx >= xbins_.size()-1 || biny >= ybins_.size()-1 || binx<1 || biny <1){ //doesn't work for underflow, overflow
		throw std::out_of_range("histo2D::getBinCenter: bin out of range (or UF,OF)");
	}

	float xw= conts_.at(biny).getBinWidth(binx);
	float yw= ybins_.at(biny+1)-ybins_.at(biny);
	centerx= xbins_.at(binx)+xw/2;
	centery=ybins_.at(biny) + yw/2;
}
float histo2D::getMax(int syslayer)const{
	size_t dummy,dummy2;
	return getMax(dummy,dummy2,syslayer);
}
float histo2D::getMin(int syslayer)const{
	size_t dummy,dummy2;
	return getMin(dummy,dummy2,syslayer);
}

float histo2D::getMax(size_t& x, size_t& y,int syslayer)const{
	float max=-999999999;
	size_t binx;
	for(size_t i=0;i<conts_.size();i++){
		float tempmax=conts_.at(i).getYMax(binx,false,syslayer);
		if(tempmax > max){
			max=tempmax;
			x=binx;
			y=i;
		}
	}
	return max;
}
float histo2D::getMin(size_t& x, size_t& y,int syslayer)const{
	float min=999999999;
	size_t binx;
	for(size_t i=0;i<conts_.size();i++){
		float tempmin=conts_.at(i).getYMin(binx,false,syslayer);
		if(tempmin < min){
			min=tempmin;
			x=binx;
			y=i;
		}
	}
	return min;
}

histo2D histo2D::rebinXToBinning(const std::vector<float> & newbins)const{
	size_t maxbinssize=xbins_.size();
	if(newbins.size()>maxbinssize) maxbinssize=newbins.size();
	std::vector<float> sames(maxbinssize);
	std::vector<float>::iterator it=std::set_intersection(++xbins_.begin(),xbins_.end(),newbins.begin(),newbins.end(),sames.begin());//ignore UF
	sames.resize(it-sames.begin());
	//check if possible at all:
	if(sames.size()!=newbins.size()){
		std::cout << "container2D::rebinXToBinning: binning not compatible" <<std::endl;
		return *this;
	}
	histo2D out=*this;


	for(size_t i=0;i<conts_.size();i++)
		out.conts_.at(i)=out.conts_.at(i).rebinToBinning(newbins);

	out.xbins_=out.conts_.at(0).getBins();
	return out;

}
histo2D histo2D::rebinXtoBinning(const histo1D&cont)const{
	if(isDummy() || ybins_.size()<1){
		throw std::logic_error("container2D::rebinXtoBinning: container2D is dummy");
	}
	std::vector<float> newbins=conts_.at(0).getCongruentBinBoundaries(cont);
	return rebinXToBinning(newbins);
}
histo2D histo2D::rebinYtoBinning(const histo1D&cont)const{
	if(isDummy() || ybins_.size()<1){
		throw std::logic_error("container2D::rebinYtoBinning: container2D is dummy");
	}
	histo1D yproj=projectToY();
	std::vector<float> newbins=yproj.getCongruentBinBoundaries(cont);
	return rebinYToBinning(newbins);
}


histo2D histo2D::rebinYToBinning(const std::vector<float> & newbins) const{
	size_t maxbinssize=ybins_.size();
	if(newbins.size()>maxbinssize) maxbinssize=newbins.size();
	std::vector<float> sames(maxbinssize);
	std::vector<float>::iterator it=std::set_intersection(++ybins_.begin(),ybins_.end(),newbins.begin(),newbins.end(),sames.begin());//ignore UF
	sames.resize(it-sames.begin());
	//check if possible at all:
	if(sames.size()!=newbins.size()){
		std::cout << "container2D::rebinYToBinning: binning not compatible" <<std::endl;
		return *this;
	}
	histo2D out=*this;
	out.ybins_.clear();
	out.ybins_.push_back(0); //UF
	out.ybins_.insert(out.ybins_.end(),newbins.begin(),newbins.end());
	out.conts_.clear();


	for(size_t i=0;i<out.ybins_.size();i++){
		out.conts_.push_back(conts_.at(0));
		if(i>0) //UF remains the same
			out.conts_.at(i).contents_.setAllZero();
	}


	bool tempaddStatCorrelated=histoContent::addStatCorrelated;
	histoContent::addStatCorrelated=false;
	for(size_t i=1;i<ybins_.size();i++){
		size_t outbin=out.getBinNoY(ybins_.at(i));
		out.conts_.at(outbin)+=conts_.at(i);
	}
	histoContent::addStatCorrelated=tempaddStatCorrelated;

	return out;

}

float histo2D::getBinErrorUp(const size_t & xbin, const size_t & ybin, bool onlystat,TString limittosys) const{
	if(xbin>=xbins_.size() || ybin>= conts_.size()){
			throw std::out_of_range ("histo2D::getBinErrorUp: xbin or ybin is out of range");// << std::endl;
		}
	return conts_.at(ybin).getBinErrorUp(xbin,  onlystat, limittosys);
}
float histo2D::getBinErrorDown(const size_t & xbin, const size_t & ybin, bool onlystat,TString limittosys) const{
	if(xbin>=xbins_.size() || ybin>= conts_.size()){
				throw std::out_of_range ("histo2D::getBinErrorDown: xbin or ybin is out of range");// << std::endl;
			}
	return conts_.at(ybin).getBinErrorDown(xbin,  onlystat, limittosys);
}
float histo2D::getBinError(const size_t & xbin, const size_t & ybin, bool onlystat,TString limittosys) const{
	if(xbin>=xbins_.size() || ybin>= conts_.size()){
					throw std::out_of_range ("histo2D::getBinError: xbin or ybin is out of range");// << std::endl;
				}
	return conts_.at(ybin).getBinError(xbin,  onlystat, limittosys);
}

float histo2D::getSystError(unsigned int number, const size_t & xbin, const size_t & ybin) const{
	if(ybin>=ybins_.size() || xbin>=xbins_.size()){
		std::cout << "container2D::getSystError: xbin or ybin out of range" << std::endl;
	}
	const histo1D * c=&conts_.at(ybin);
	return c->getSystError(number,xbin);
}
float  histo2D::getSystErrorStat(unsigned int number, const size_t & xbin, const size_t & ybin) const{
	if(ybin>=ybins_.size() || xbin>=xbins_.size()){
		std::cout << "container2D::getSystError: xbin or ybin out of range" << std::endl;
		return 0;
	}
	const histo1D * c=&conts_.at(ybin);
	return c->getSystErrorStat(number,xbin);
}
const TString & histo2D::getSystErrorName(const size_t & number) const{
	if(conts_.size()<1){
		std::cout << "container2D::getSystError: number "<< number << " out of range (" << (int)getSystSize()-1 << ")"<< std::endl;
		return name_;
	}
	return conts_.at(0).getSystErrorName(number);
}
size_t histo2D::getSystErrorIndex(const TString& name)const{
	size_t ret;
	if(conts_.size()<1)
		throw std::out_of_range("container2D::getSystErrorIndex: container empty");

	size_t max=conts_.at(0).getSystSize();
	ret=conts_.at(0).getSystErrorIndex(name);
	if(ret==max)
		throw std::out_of_range("container2D::getSystErrorIndex: index not found");
	return ret;
}

void histo2D::mergeVariations(const std::vector<TString>& names, const TString & outname,bool linearly){
	for(size_t i=0;i<conts_.size();i++)
		conts_.at(i).mergeVariations(names,outname,linearly);
}
void histo2D::mergeVariationsFromFileInCMSSW(const std::string& filename){
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

void histo2D::splitSystematic(const size_t & number, const float& fracadivb,
		const TString & splinamea,  const TString & splinameb){
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i).splitSystematic(number,fracadivb, splinamea, splinameb);
	}
}
/*
 float histo2D::projectBinContentToY(const size_t & ybin,bool includeUFOF) const{
	if((size_t)ybin >= ybins_.size()){
		std::cout << "histo2D::projectBinToY: ybin out of range" << std::endl;
		return 0;
	}
	const histo1D * c=&conts_.at(ybin);
	return c->integral(includeUFOF);

}
 float histo2D::projectBinContentToX(const size_t & xbin,bool includeUFOF) const{
	if((size_t)xbin >= xbins_.size()){
		std::cout << "histo2D::projectBinToX: xbin out of range" << std::endl;
	}
	int lowestybin=0;
	int highestybin=ybins_.size()-1;
	if(!includeUFOF){
		lowestybin++;
		highestybin--;
	}
	if(lowestybin>=highestybin){
		std::cout << "histo2D::projectBinToX: cannot project to X, too less ybins" <<std::endl;
	}
	float content=0;
	for(int ybin=lowestybin;ybin<=highestybin;ybin++){
		content+=conts_.at(ybin).getBinContent(xbin);
	}
	return content;
}
 */
/**
 * histo2D::projectToX(bool includeUFOF=false)
 */
histo1D histo2D::projectToX(bool includeUFOF) const{
	bool tempc_makelist=histo1D::c_makelist;
	histo1D::c_makelist=false;
	if(ybins_.size()<2 || xbins_.size()<2){//not real container
		std::cout << "container2D::projectToX: plotting purity not possible because of too less bins for " << name_ << std::endl;
		histo1D::c_makelist=tempc_makelist;
		return histo1D();
	}
	size_t minc=0, maxc=ybins_.size()-1;
	if(!includeUFOF){
		minc++; maxc--;
	}
	bool temp=histoContent::addStatCorrelated; //bin stats NOT correlated
	histoContent::addStatCorrelated=false;
	histo1D out = conts_.at(0);
	out.clear(); //empty. only binning remains
	for(size_t i=minc;i<=maxc;i++)
		out += conts_[i];
	histoContent::addStatCorrelated=temp;
	histo1D::c_makelist=tempc_makelist;

	out.setXAxisName(xaxisname_);
	return out;
}
/**
 * histo2D::projectToY(bool includeUFOF=false)
 */
histo1D histo2D::projectToY(bool includeUFOF) const{
	bool tempc_makelist=histo1D::c_makelist;
	histo1D::c_makelist=false;
	if(ybins_.size()<2 || xbins_.size()<2){//not real container
		std::cout << "container2D::projectToY: plotting purity not possible because of too less bins for " << name_ << std::endl;
		histo1D::c_makelist=tempc_makelist;
		return histo1D();
	}
	std::vector<float> newbins=ybins_;
	newbins.erase(newbins.begin()); //erase underflow
	histo1D layercont(newbins,name_+"_yProjection",yaxisname_,"");//underflow recreated
	histo1D out;
	bool temp=histoContent::addStatCorrelated; //bin stats NOT correlated
	histoContent::addStatCorrelated=false;
	for(int systLayer=-1;systLayer<(int) conts_.at(0).contents_.layerSize();systLayer++){
		for(size_t ybin=0;ybin<ybins_.size();ybin++){
			const histo1D * ycont=&conts_[ybin];
			const float &content=ycont->integral(includeUFOF,systLayer);
			const float &staterr=ycont->integralStat(includeUFOF,systLayer);
			const size_t &entries=ycont->integralEntries(includeUFOF,systLayer);
			layercont.setBinContent(ybin,content); //fill in nominal
			layercont.setBinStat(ybin,staterr); //fill in nominal
			layercont.setBinEntries(ybin,entries);
		}
		if(systLayer>=0){
			const TString& name=getSystErrorName((size_t)systLayer);
			out.addErrorContainer(name,layercont);
		}
		else{
			out=layercont;
		}
	}
	histoContent::addStatCorrelated=temp;
	histo1D::c_makelist=tempc_makelist;
	return out;
}


histo1D histo2D::getYSlice(size_t ybinno) const{
	if(ybinno >= ybins_.size()){
		std::cout << "container2D::getYSlice: bin out of range for " <<name_ << std::endl;
		return histo1D();
	}
	histo1D out = conts_.at(ybinno);
	out.setName(name_+"_YSlice_"+toTString(ybinno));
	return out;
}
histo1D histo2D::getXSlice(size_t xbinno) const{
	if(xbinno >= xbins_.size()){
		std::cout << "container2D::getXSlice: bin out of range for " <<name_ << std::endl;
		return histo1D();
	}
	std::vector<float> rybins=ybins_;
	rybins.erase(rybins.begin()); //no UF
	histo1D out(rybins,name_+"_XSlice_"+toTString(xbinno),yaxisname_,"");
	for(int layer=-1;layer<(int)getSystSize();layer++){
		int outlayer=-1;
		if(layer>-1)
			outlayer=out.contents_.addLayer(getSystErrorName(layer));
		for(size_t bin=0;bin<ybins_.size();bin++){
			out.contents_.getBin(bin,outlayer) = conts_.at(bin).contents_.getBin(xbinno,layer);
		}
	}
	return out;
}
/**
 * do not allow different syst (as far as their names are concerned)
 */
void histo2D::setXSlice(size_t xbinno,const histo1D & in, bool UFOF){
	if(in.bins_ != ybins_){
		std::cout << "container2D::setXSlice: wrong binning! doing nothing"<< std::endl;
		return;
	}
	if(xbinno >= xbins_.size()){
		std::cout << "container2D::setXSlice: bin out of range" << std::endl;
		return;
	}
	if(debug){
		std::cout << "container2D::setXSlice: syst size in: "<< in.getSystSize() <<" syst size this: "<< getSystSize()<< std::endl;

	}
	if(in.getSystSize() == getSystSize()){
		//check whther same layers or add or something to be figured out
		for(int lay=-1;lay<(int)getSystSize();lay++){
			int layer=lay;
			if(layer>-1){
				layer=in.getSystErrorIndex(getSystErrorName(lay));}
			if(layer>=(int)getSystSize()){
				std::cout << "container2D::setXSlice: layer "<< getSystErrorName(lay) << " not found, systematics of input container don't match! output undefined" <<std::endl;
				return;
			}
			for(size_t bin=0;bin<ybins_.size();bin++){//do some other bin filling stuff
				if(!UFOF && (bin==0 || bin==ybins_.size()-1))
					continue;
				getBin(xbinno,bin,lay) = in.getBin(bin,layer);
			}
		}
		return; //all went fine
	}
	else if(debug){
		std::cout << "container2D::setXSlice: input syst:";
		for(size_t i=0;i<in.getSystSize();i++)
			std::cout << in.getSystErrorName(i)<< " ";
		std::cout << std::endl;
		std::cout << "container2D::setXSlice: this  syst:";
		for(size_t i=0;i<getSystSize();i++)
			std::cout << getSystErrorName(i)<< " ";
		std::cout << std::endl;
	}
	//nothing of the above - give error message

	std::cout << "container2D::setXSlice: systematics of input container don't match! doing nothing" <<std::endl;
}
/**
 * do not allow different syst (as far as their names are concerned)
 */
void histo2D::setYSlice(size_t ybinno,const histo1D & in, bool UFOF){
	if(in.bins_ != xbins_){
		std::cout << "container2D::setYSlice: wrong binning! doing nothing"<< std::endl;
		return;
	}
	if(ybinno >= ybins_.size()){
		std::cout << "container2D::setYSlice: bin out of range" << std::endl;
		return;
	}
	if(in.getSystSize() == getSystSize()){
		//check whther same layers or add or something to be figured out
		for(int lay=-1;lay<(int)getSystSize();lay++){
			int layer=lay;
			if(layer>-1){
				layer=in.getSystErrorIndex(getSystErrorName(lay));}
			if(layer>=(int)getSystSize()){
				std::cout << "container2D::setYSlice: layer "<< getSystErrorName(lay) << " not found, systematics of input container don't match! output undefined" <<std::endl;
				return;
			}
			for(size_t bin=0;bin<xbins_.size();bin++){//do some other bin filling stuff
				if(!UFOF && (bin==0 || bin==xbins_.size()-1))
					continue;
				getBin(bin,ybinno,lay) = in.getBin(bin,layer);
			}
		}
		return; //all went fine
	}
	//nothing of the above - give error message
	std::cout << "container2D::setYSlice: systematics of input container don't match at all! doing nothing" <<std::endl;
}


/**
 * on long term, put this in own class responsible for all drawing stuff (same for 1D containers)
 * maybe something like a histodrawer class (handles 2D, 1D, UF, ...) and each object has own style
 * then, histo1D, 2D, 1DUF can be independent of root
 * Stack could be, too, but again needs drawer class (can use different drawer class objects internally)
 * may even use other graphic representations as root
 * in principle drawing of systematics etc can be managed by the class .setDrawStatOnly, .drawSyst...., .setStyle, .setStyle.setMarker/Line.... (style is own new struct/class)
 */
TH2D * histo2D::getTH2D(TString name, bool dividebybinarea, bool onlystat) const{
	if(getNBinsX() < 1 || getNBinsY() <1)
		return 0; //there is not even one real, non OF or UF container
	if(name=="")name=name_;
	name=textFormatter::makeCompatibleFileName(name.Data());
	TH2D * h=new TH2D(name,name,getNBinsX(),&(xbins_.at(1)),getNBinsY(),&(ybins_.at(1)));
	size_t entries=0;
	for(size_t xbin=0;xbin<=getNBinsX()+1;xbin++){ // 0 underflow, genBins+1 overflow included!!
		for(size_t ybin=0;ybin<=getNBinsY()+1;ybin++){
			entries+=getBinEntries(xbin,ybin);
			float cont=getBinContent(xbin,ybin);
			float multi=1;
			if(dividebybinarea && ybin>0 && xbin>0 && ybin<=getNBinsY() && xbin<=getNBinsX())
				multi/=getBinArea(xbin,ybin);
			h->SetBinContent(xbin,ybin,cont*multi);
			float err=getBinError(xbin,ybin,onlystat);
			h->SetBinError(xbin,ybin,err*multi);

		}
	}
	h->GetXaxis()->SetTitle(xaxisname_);
	h->GetYaxis()->SetTitle(yaxisname_);
	h->GetZaxis()->SetTitle(zaxisname_);
	if(entries>0)
		h->SetEntries(entries);
	else
		h->SetEntries(getNBinsX()*getNBinsY());
	return h;
}
/**
 * TH2D * histo2D::getTH2DSyst(TString name, unsigned int systNo, bool dividebybinwidth=false, bool statErrors=false)
 */
TH2D * histo2D::getTH2DSyst(TString name, unsigned int systNo, bool dividebybinarea, bool statErrors) const{
	TH2D * h=getTH2D(name,dividebybinarea,true);
	if(!h) return 0;

	for(size_t xbin=0;xbin<=getNBinsX()+1;xbin++){ // 0 underflow, genBins+1 overflow included!!
		for(size_t ybin=0;ybin<=getNBinsY()+1;ybin++){
			float cont=getBinContent(xbin,ybin)+getSystError(systNo,xbin,ybin);
			float multi=1;
			if(dividebybinarea && ybin>0 && xbin>0 && ybin<=getNBinsY() && xbin<=getNBinsX())
				multi/=getBinArea(xbin,ybin);

			h->SetBinContent(xbin,ybin,cont*multi);
			float stat=getSystErrorStat(systNo,xbin,ybin);
			if(statErrors)
				h->SetBinError(xbin,ybin,stat*multi);
			else
				h->SetBinError(xbin,ybin,0);
		}
	}
	return h;

}

/**
 * imports TGraphAsymmErrors. Only works if the x-axis errors indicate the bin boundaries,
 * the graph binning corresponds to X axis binning, and all bins are the same size
 * Y errors will be represented as new systematic layers, the stat error will be set to 0.
 * The new systematic layers can be named- if not, default is Graphimp_<up/down>
 */
histo2D & histo2D::import(std::vector<TGraphAsymmErrors *> gs,bool isbinwidthdivided, const TString & newsystname){
	//check stuff
	bool includeofuf=true;
	if(gs.size()!= ybins_.size()){
		//can be without of/uf
		if(gs.size()== ybins_.size()-2){
			//special case, also allowed
			includeofuf=false;
		}
		else{
			throw std::logic_error("container2D::import: number of graphs need to be:\n -number of visible bins\n - number of total bins (1 UF, 1 OF)");
		}
	}
	//first check
	for(size_t i=0;i<gs.size()-1;i++){
		if(gs.at(i)->GetN() == gs.at(i+1)->GetN()){
			//		if(std::equal(gs.at(i)->GetEX(), &gs.at(i)->GetEX()[gs.at(i)->GetN()-1], gs.at(i+1)->GetEX())) continue;
		}
		//error here
	}
	std::cout << "herea"<< std::endl;

	for(size_t i=0;i<gs.size();i++){
		size_t cit=i;
		if(!includeofuf) cit++;
		conts_.at(cit).import(gs.at(i),isbinwidthdivided,newsystname);
	}
	std::cout << "hereb"<< std::endl;


	xbins_=conts_.at(1).getBins();
	if(!includeofuf){
		//set the rest
		histo1D tmp=conts_.at(1);
		tmp.setAllZero();
		conts_.at(0)=tmp;
		conts_.at(conts_.size()-1)=tmp;
	}

	std::cout << "herec"<< std::endl;
	return *this;

}




void histo2D::setDivideBinomial(bool binomial){
	std::cout << "container2D::setDivideBinomial: obsolete / switch on static histoContent options" <<std::endl;
	/*for(size_t i=0;i<conts_.size();i++)
		conts_.at(i).setDivideBinomial(binomial);
	divideBinomial_=binomial;*/
}

histo2D & histo2D::operator += (const histo2D & second){
	if((second.xbins_ != xbins_ || second.ybins_ != ybins_) && !isDummy()){
		std::cout << "container2D::operator +=: "<< name_ << " and " << second.name_<<" must have same binning! returning *this" << std::endl;
		return *this;
	}
	if(isDummy()){
		*this=second;
		return *this;
	}
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i) += second.conts_.at(i);
	}
	return *this;
}
histo2D histo2D::operator + (const histo2D & second)const{
	histo2D copy=*this;
	return copy+=second;
}
histo2D & histo2D::operator -= (const histo2D & second){
	if(second.xbins_ != xbins_ || second.ybins_ != ybins_){
		std::cout << "container2D::operator -: "<< name_ << " and " << second.name_<<" must have same binning! returning *this"  << std::endl;
		return *this;
	}
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i) -=  second.conts_.at(i);
	}
	return *this;
}
histo2D histo2D::operator - (const histo2D & second)const{
	histo2D copy=*this;
	return copy -= second;
}

histo2D & histo2D::operator /= (const histo2D & second){
	if(second.xbins_ != xbins_ || second.ybins_ != ybins_){
		std::cout << "container2D::operator /: "<< name_ << " and " << second.name_<<" must have same binning! returning *this"  << std::endl;
		return *this;
	}
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i) /= second.conts_.at(i);
	}
	return *this;
}
histo2D  histo2D::operator / (const histo2D & second)const{
	histo2D copy=*this;
	return copy/=second;
}
histo2D & histo2D::operator *= (float val){
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i) *= val;
	}
	return *this;
}
histo2D & histo2D::operator *= (const histo2D & second){
	if(second.xbins_ != xbins_ || second.ybins_ != ybins_){
		std::cout << "container2D::operator *=: "<< name_ << " and " << second.name_<<" must have same binning! returning *this"  << std::endl;
		return *this;
	}
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i) *=  second.conts_.at(i);
	}
	return *this;
}
histo2D histo2D::operator * (const histo2D & second)const{
	if(second.xbins_ != xbins_ || second.ybins_ != ybins_){
		std::cout << "container2D::operator *: "<< name_ << " and " << second.name_<<" must have same binning! returning *this"  << std::endl;
		return *this;
	}
	histo2D out=*this;
	out*=second;
	return out;
}
histo2D histo2D::operator * (float val)const{
	histo2D out=*this;
	out*=val;
	return out;
}
histo2D histo2D::operator * (double val)const{
	histo2D out=*this;
	out*=(float)val;
	return out;
}
histo2D histo2D::operator * (int val)const{
	histo2D out=*this;
	out*=(float)val;
	return out;
}
bool histo2D::isEqual (const histo2D &rhs)const{
	/*
	std::vector<ztop::histo1D> conts_; //! for each y axis bin one container
	std::vector<float> xbins_,ybins_;
	bool divideBinomial_;
	bool mergeufof_;

	TString xaxisname_;
	TString yaxisname_;
	 */
	if(conts_ != rhs.conts_)
		return false;
	if(xbins_ != rhs.xbins_)
		return false;
	if(ybins_ != rhs.ybins_)
		return false;
	if(divideBinomial_ != rhs.divideBinomial_)
		return false;
	if(mergeufof_ != rhs.mergeufof_)
		return false;
	if(xaxisname_ != rhs.xaxisname_)
		return false;
	if(yaxisname_ != rhs.yaxisname_)
		return false;
	return true;
}

histo2D histo2D::chi2container(const histo2D& rhs,size_t * ndof)const{
	if(getBinsX() != rhs.getBinsX() || getBinsY() != rhs.getBinsY() ){
		throw std::out_of_range("container2D::chi2container: bins don't match!");
	}
	histo2D out=*this;
	out.removeAllSystematics();
	out.setAllErrorsZero(true);
	out.setName(getName()+"_"+rhs.getName()+"_chi2");
	size_t ndofsum=0;
	for(size_t i=0;i<conts_.size();i++){
		size_t ndoftmp=0;
		out.conts_.at(i) = conts_.at(i).chi2container(rhs.conts_.at(i),&ndoftmp);
		out.conts_.at(i).setName("");
		ndofsum+=ndoftmp;
	}
	if(ndof)
		*ndof=ndofsum;
	return out;
}
float histo2D::chi2(const histo2D& rhs,size_t * ndof)const{
	if(getBinsX() != rhs.getBinsX() || getBinsY() != rhs.getBinsY() ){
		throw std::out_of_range("container2D::chi2container: bins don't match!");
	}
	size_t ndofsum=0;
	float chi2=0;
	for(size_t i=0;i<conts_.size();i++){
		size_t ndoftmp=0;
		chi2+=conts_.at(i).chi2(rhs.conts_.at(i),&ndoftmp);
		ndofsum+=ndofsum;
	}
	if(ndof)
		*ndof=ndofsum;
	return chi2;
}


histo2D histo2D::cutRightX(const float & val)const{
	if(conts_.size()<1)
		return histo2D();
	size_t binno=getBinNoX(val);
	if(binno<1){
		throw std::out_of_range("container2D::cutRightX: would cut full content. Probably not intended!?");
	}
	histo2D out=*this;
	for(size_t i=0;i<conts_.size();i++){
		out.conts_.at(i) = conts_.at(i).cutRight(val);
		if(i==0){
			out.xbins_=out.conts_.at(i).bins_;
		}
	}
	return out;
}

int histo2D::addErrorContainer(const TString & sysname,const histo2D & second ,float weight){
	if(second.xbins_ != xbins_ || second.ybins_ != ybins_){
		std::cout << "container2D::addErrorContainer "<< name_ << " and " << second.name_<<" must have same binning!"  << std::endl;
		return -1;
	}
	int out=0;
	for(size_t i=0;i<conts_.size();i++){
		int tmp=conts_.at(i).addErrorContainer(sysname, second.conts_.at(i),weight);
		if(tmp<0)out=tmp;
	}
	return out;
}
int histo2D::addErrorContainer(const TString & sysname,const histo2D & second ){
	return addErrorContainer(sysname,second , 1);
}
void histo2D::getRelSystematicsFrom(const histo2D & second){
	if(second.xbins_ != xbins_ || second.ybins_ != ybins_){
		std::cout << "container2D::getRelSystematicsFrom "<< name_ << " and " << second.name_<<" must have same binning!"  << std::endl;
		return;
	}
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i).getRelSystematicsFrom(second.conts_.at(i));
	}
}
void histo2D::addRelSystematicsFrom(const histo2D & second,bool ignorestat, bool strict){
	if(second.xbins_ != xbins_ || second.ybins_ != ybins_){
		std::cout << "container2D::addRelSystematicsFrom "<< name_ << " and " << second.name_<<" must have same binning!"  << std::endl;
		return;
	}
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i).addRelSystematicsFrom(second.conts_.at(i),ignorestat,strict);
	}
}

void histo2D::removeError(TString err){
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i).removeError(err);
	}
}
void histo2D::renameSyst(TString old, TString New){
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i).renameSyst(old,New);
	}
}
void histo2D::removeAllSystematics(const TString& exception){
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i).removeAllSystematics(exception);
	}
}
void histo2D::setAllErrorsZero(bool nominalstat){
	for(size_t i=0;i<conts_.size();i++)
		conts_.at(i).setAllErrorsZero(nominalstat);
}

void histo2D::createStatFromContent(){
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i).createStatFromContent();
	}
}
void histo2D::reset(){
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i).reset();
	}
}
void histo2D::clear(){
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i).clear();
	}
}
void histo2D::setAllZero(){
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i).setAllZero();
	}
}

void histo2D::addGlobalRelError(TString errname,float val){
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i).addGlobalRelError(errname,val);
	}
}
std::vector<size_t> histo2D::removeSpikes(bool inclUFOF,int limittoindex ,float strength,float sign,float threshold){
	//scan for bins

	if(ybins_.size() < 6 || xbins_.size() < 6){
		if(debug)
			std::cout << "container2D::removeSpikes: skipping due to less than 3 x or y bins" <<std::endl;
		return std::vector<size_t>();

	}

	int syssize=getSystSize();
	size_t minbiny=0,minbinx=0,maxbiny=ybins_.size(),maxbinx=xbins_.size();
	if(!inclUFOF){
		minbiny++;minbinx++;
		maxbinx--;maxbiny--;
	}

	std::vector<size_t> out;
	for(size_t i=minbiny;i<maxbiny;i++){
		for(size_t j=minbinx;j<maxbinx;j++){
			//bool reltonom=true;
			//const float& nominal=getBinContent(i,j);
			//if(nominal==0)reltonom=false;
			for(int sys=-1;sys<syssize;sys++){

				if(limittoindex>=0)
					if(limittoindex!=(int)sys) continue;

				float  syscontent=getBin(i,j,sys).getContent()/ getBinArea(i,j);
				float  sysstat=getBin(i,j,sys).getStat()/ getBinArea(i,j);



				//check if this is a real fluctuation
				float areaaverage=0;
				size_t miny=i,minx=j,maxy=i,maxx=j;
				if(i>minbiny) miny=i-1;
				if(j>minbinx) minx=j-1;
				if(i+1<maxbiny) maxy=i+1;
				if(j+1<maxbinx) maxx=j+1;


				size_t adjacentbins=(maxy+1-miny)* (maxx+1-minx);
				if(adjacentbins!=0)adjacentbins--;
				//require at least 5 surrounding bins (this excludes corner bins)
				if(adjacentbins<5) continue;

				float bindiffval=syscontent*100000000,nextbinval=0;

				for(size_t checky=miny;checky<=maxy;checky++){
					for(size_t checkx=minx;checkx<=maxx;checkx++){
						if(j==checkx && i==checky) continue;
						areaaverage+=getBin(checky,checkx,sys).getContent() / getBinArea(checkx,checky);

						if(fabs(syscontent - getBin(checky,checkx,sys).getContent() / getBinArea(checkx,checky)) < bindiffval){
							nextbinval=getBin(checky,checkx,sys).getContent() / getBinArea(checkx,checky);
							bindiffval=fabs(syscontent -nextbinval);
						}
					}
				}

				areaaverage/=(float)(adjacentbins);
				//float deltaavg=fabs((areaaverage-syscontent));
				//	float reldeltaavg=0;


				if(syscontent==0) //DEBUG dont change zeros right now
					continue;

				//	float scaletorealstat=syscontent/(sysstat*sysstat);

				//	if(areaaverage!=0){
				//		reldeltaavg=deltaavg/areaaverage;}

				float reldiff=0;
				if(nextbinval)
					reldiff=bindiffval/nextbinval;
				else if(areaaverage)
					reldiff=bindiffval/areaaverage;

				bool resetcontent=// (reldeltaavg!=0
						//&& reldeltaavg/threshold >= 1
						bindiffval - (sysstat) / sign <0
						&& (reldiff /threshold>1);//);//||	(reldeltaavg==0
				//	&& scaletorealstat*syscontent > threshold
				//	&& syscontent - (sysstat) / sign <0);




				if(resetcontent){
					//		float direction=1;//
					//if(syscontent-areaaverage <0) direction=-1;

					//still allow a bit of old fluctuation:
					float newcontent=areaaverage;// + deltaavg*direction * 1/strength;

					if(std::find(out.begin(),out.end(),sys) == out.end())
						out.push_back(sys);

					if(debug){
						std::cout << "container2D::smoothSystematics: found bin over threshold: "<<i <<" " << j;
						if(sys>-1)
							std::cout << "\nsystematics: " << getSystErrorName(sys);
						std::cout << "\ndelta to next bin: " << bindiffval//* getBinArea(i,j)
								<< "\nrelative difference to next bin/avg: " << reldiff
								<< "\nscaled to threshold: " << reldiff /threshold
								<< "\noldcontent: " <<syscontent* getBinArea(i,j)
						<< "\nre-setting content to: " << newcontent* getBinArea(i,j);
					}

					//leave old stat error, only scale

					getBin(i,j,sys).setContent(newcontent* getBinArea(i,j));
					if(syscontent!=0){
						getBin(i,j,sys).setStat(newcontent/syscontent*(sysstat)* getBinArea(i,j));
						if(debug)
							std::cout << "\nsetting new stat to " << newcontent/syscontent*(sysstat) <<std::endl;
					}
					else{
						getBin(i,j,sys).setStat(newcontent* getBinArea(i,j));//to still agree with 0
						if(debug)
							std::cout << "\nsetting new stat to " << newcontent <<std::endl;
					}



				}
			}
		}
	}
	return out;
}


histo2D histo2D::createPseudoExperiment(TRandom3* rand,const histo2D* c, histo1D::pseudodatamodes mode, int syst)const{
	histo2D out=*this;
	if((c) &&(c->getBinsX() != getBinsX() || c->getBinsY() != getBinsY())){
		throw std::out_of_range("container2D::createPseudoExperiment: binning does not match!");
	}
	for(size_t i=0;i<conts_.size();i++){
		if(c)
			out.conts_.at(i)=conts_.at(i).createPseudoExperiment(rand,&c->conts_.at(i),mode,syst);
		else
			out.conts_.at(i)=conts_.at(i).createPseudoExperiment(rand,0,mode,syst);
	}
	return out;
}


void histo2D::mergePartialVariations(const TString& identifier,bool strictpartialID){
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i).mergePartialVariations(identifier,strictpartialID);
	}
}
void histo2D::mergeAllErrors(const TString & mergedname){
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i).mergeAllErrors(mergedname);
	}
}
void histo2D::equalizeSystematicsIdxs(histo2D &rhs){ //sec
	if(conts_.size() == rhs.conts_.size()){
		for(size_t i=0;i<conts_.size();i++){
			conts_.at(i).equalizeSystematicsIdxs(rhs.conts_.at(i));
		}
	}
	else{
		throw std::logic_error("container2D::equalizeSystematicsIdxs: TBI for different conts_size()");
	}
}

std::vector<TString> histo2D::getSystNameList()const{
		if(isDummy())
			throw std::logic_error("histo2D::getSystNameList() histo dummy");
		return conts_.at(0).getSystNameList();
	}

void histo2D::copyFrom(const histo2D&rhs){
	conts_=rhs.conts_;
	xbins_=rhs.xbins_;
	ybins_=rhs.ybins_;
	divideBinomial_=rhs.divideBinomial_ ;
	mergeufof_=rhs.mergeufof_ ;
	xaxisname_=rhs.xaxisname_ ;
	yaxisname_=rhs.yaxisname_ ;
	zaxisname_=rhs.zaxisname_ ;
}


void histo2D::writeToFile(const std::string& filename)const{
	std::ofstream saveFile;
	saveFile.open(filename.data(), std::ios_base::binary | std::ios_base::trunc | std::fstream::out );
	{
		boost::iostreams::filtering_ostream out;
		boost::iostreams::zlib_params parms;
		//parms.level=boost::iostreams::zlib::best_speed;
		out.push(boost::iostreams::zlib_compressor(parms));
		out.push(saveFile);
		{
			writeToStream(out);
		}
	}
	saveFile.close();
}
void histo2D::readFromFile(const std::string& filename){
	std::ifstream saveFile;
	saveFile.open(filename.data(), std::ios_base::binary | std::fstream::in );
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


}//namespace
