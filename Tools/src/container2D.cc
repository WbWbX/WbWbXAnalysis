/*
 * container2D.cc
 *
 *  Created on: Jun 24, 2013
 *      Author: kiesej
 */



#include "../interface/container2D.h"
#include <algorithm>


namespace ztop{

bool container2D::debug=false;
std::vector<container2D*> container2D::c_list;
bool container2D::c_makelist=false;

container2D::container2D():taggedObject(taggedObject::type_container2D), divideBinomial_(true),mergeufof_(false),xaxisname_(""),yaxisname_("") {
	setName("");
	std::vector<float> nobins;
	nobins.push_back(1);
	setBinning(nobins,nobins);
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

container2D::container2D(const std::vector<float> &xbins,const std::vector<float> &ybins, TString name,TString xaxisname,TString yaxisname, bool mergeufof) :
        												taggedObject(taggedObject::type_container2D),mergeufof_(mergeufof), xaxisname_(xaxisname), yaxisname_(yaxisname)
{
	setName(name);
	//create for each ybin (plus UF OF) a container1D
	setBinning(xbins,ybins);
	if(container2D::c_makelist)
		c_list.push_back(this);
}

void container2D::setBinning(const std::vector<float> &xbins, std::vector<float> ybins){
	if(xbins.size()<1 || ybins.size() <1){
		std::cout << "container2D::setBinning: bins have to be at least of size 1! ybins: "<<  ybins.size()<< " xbins: "<< xbins.size()<<" name: " <<name_ <<std::endl;
		throw std::logic_error("container2D::setBinning: bins have to be at least of size 1!");
	}


	bool temp=container1D::c_makelist;
	container1D::c_makelist=false;
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
		container1D temp(xbins,"",xaxisname_,"",mergeufof_);
		//temp.setDivideBinomial(divideBinomial_);
		conts_.push_back(temp);
	}
	xbins_=conts_.at(0).getBins();

	if(container2D::debug){
		std::cout << "containers size: " << conts_.size() << " ybins size: " << ybins_.size() << std::endl;
	}
	container1D::c_makelist=temp;
}

const float &container2D::getBinContent(const size_t & xbin,const size_t & ybin) const{
	if((unsigned int)xbin>xbins_.size() || (unsigned int)ybin> ybins_.size()){
		std::cout << "container2D::getBinContent: xbin or ybin is out of range!! return -1" << std::endl;
	}
	return conts_.at(ybin).getBinContent(xbin);
}
const size_t &container2D::getBinEntries(const size_t & xbin, const size_t & ybin) const{
	if(ybin>=ybins_.size()){
		std::cout << "container2D::getBinEntries: ybin out of range! return " << std::endl;
	}
	return conts_.at(ybin).getBinEntries(xbin);
}

float container2D::getBinArea(const size_t& binx, const size_t& biny)const{
	if(isDummy())
		return 1;
	if(binx >= xbins_.size()-1 || biny >= ybins_.size()-1 || binx<1 || biny <1){ //doesn't work for underflow, overflow
		throw std::out_of_range("container2D::getBinArea: bin out of range (or UF,OF)");
	}

	float xw= conts_.at(biny).getBinWidth(binx);
	float yw= ybins_.at(biny+1)-ybins_.at(biny);
	return xw*yw;

}

container2D container2D::rebinXToBinning(const std::vector<float> & newbins)const{
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
	container2D out=*this;


	for(size_t i=0;i<conts_.size();i++)
		out.conts_.at(i)=out.conts_.at(i).rebinToBinning(newbins);

	out.xbins_=out.conts_.at(0).getBins();
	return out;

}
container2D container2D::rebinXtoBinning(const container1D&cont)const{
	if(isDummy() || ybins_.size()<1){
		throw std::logic_error("container2D::rebinXtoBinning: container2D is dummy");
	}
	std::vector<float> newbins=conts_.at(0).getCongruentBinBoundaries(cont);
	return rebinXToBinning(newbins);
}
container2D container2D::rebinYtoBinning(const container1D&cont)const{
	if(isDummy() || ybins_.size()<1){
		throw std::logic_error("container2D::rebinYtoBinning: container2D is dummy");
	}
	container1D yproj=projectToY();
	std::vector<float> newbins=yproj.getCongruentBinBoundaries(cont);
	return rebinYToBinning(newbins);
}


container2D container2D::rebinYToBinning(const std::vector<float> & newbins) const{
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
	container2D out=*this;
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

float container2D::getBinErrorUp(const size_t & xbin, const size_t & ybin, bool onlystat,TString limittosys) const{
	if(ybin>=ybins_.size()){
		std::cout << "container2D::getBinErrorUp: ybin out of range! return -1" << std::endl;
	}
	return conts_.at(ybin).getBinErrorUp(xbin,  onlystat, limittosys);
}
float container2D::getBinErrorDown(const size_t & xbin, const size_t & ybin, bool onlystat,TString limittosys) const{
	if(ybin>=ybins_.size()){
		std::cout << "container2D::getBinErrorDown: ybin out of range! return -1" << std::endl;
	}
	return conts_.at(ybin).getBinErrorDown(xbin,  onlystat, limittosys);
}
float container2D::getBinError(const size_t & xbin, const size_t & ybin, bool onlystat,TString limittosys) const{
	if(ybin>=ybins_.size()){
		std::cout << "container2D::getBinError: ybin out of range! return -1" << std::endl;
	}
	return conts_.at(ybin).getBinError(xbin,  onlystat, limittosys);
}

float container2D::getSystError(unsigned int number, const size_t & xbin, const size_t & ybin) const{
	if(ybin>=ybins_.size() || xbin>=xbins_.size()){
		std::cout << "container2D::getSystError: xbin or ybin out of range" << std::endl;
	}
	const container1D * c=&conts_.at(ybin);
	return c->getSystError(number,xbin);
}
float  container2D::getSystErrorStat(unsigned int number, const size_t & xbin, const size_t & ybin) const{
	if(ybin>=ybins_.size() || xbin>=xbins_.size()){
		std::cout << "container2D::getSystError: xbin or ybin out of range" << std::endl;
		return 0;
	}
	const container1D * c=&conts_.at(ybin);
	return c->getSystErrorStat(number,xbin);
}
const TString & container2D::getSystErrorName(const size_t & number) const{
	if(conts_.size()<1){
		std::cout << "container2D::getSystError: number "<< number << " out of range (" << (int)getSystSize()-1 << ")"<< std::endl;
		return name_;
	}
	return conts_.at(0).getSystErrorName(number);
}
/*
 float container2D::projectBinContentToY(const size_t & ybin,bool includeUFOF) const{
	if((size_t)ybin >= ybins_.size()){
		std::cout << "container2D::projectBinToY: ybin out of range" << std::endl;
		return 0;
	}
	const container1D * c=&conts_.at(ybin);
	return c->integral(includeUFOF);

}
 float container2D::projectBinContentToX(const size_t & xbin,bool includeUFOF) const{
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
	float content=0;
	for(int ybin=lowestybin;ybin<=highestybin;ybin++){
		content+=conts_.at(ybin).getBinContent(xbin);
	}
	return content;
}
 */
/**
 * container2D::projectToX(bool includeUFOF=false)
 */
container1D container2D::projectToX(bool includeUFOF) const{
	bool tempc_makelist=container1D::c_makelist;
	container1D::c_makelist=false;
	if(ybins_.size()<2 || xbins_.size()<2){//not real container
		std::cout << "container2D::projectToX: plotting purity not possible because of too less bins for " << name_ << std::endl;
		container1D::c_makelist=tempc_makelist;
		return container1D();
	}
	size_t minc=0, maxc=ybins_.size()-1;
	if(!includeUFOF){
		minc++; maxc--;
	}
	bool temp=histoContent::addStatCorrelated; //bin stats NOT correlated
	histoContent::addStatCorrelated=false;
	container1D out = conts_.at(0);
	out.clear(); //empty. only binning remains
	for(size_t i=minc;i<=maxc;i++)
		out += conts_[i];
	histoContent::addStatCorrelated=temp;
	container1D::c_makelist=tempc_makelist;

	out.setXAxisName(xaxisname_);
	return out;
}
/**
 * container2D::projectToY(bool includeUFOF=false)
 */
container1D container2D::projectToY(bool includeUFOF) const{
	bool tempc_makelist=container1D::c_makelist;
	container1D::c_makelist=false;
	if(ybins_.size()<2 || xbins_.size()<2){//not real container
		std::cout << "container2D::projectToY: plotting purity not possible because of too less bins for " << name_ << std::endl;
		container1D::c_makelist=tempc_makelist;
		return container1D();
	}
	std::vector<float> newbins=ybins_;
	newbins.erase(newbins.begin()); //erase underflow
	container1D layercont(newbins,name_+"_yProjection",yaxisname_,"");//underflow recreated
	container1D out;
	bool temp=histoContent::addStatCorrelated; //bin stats NOT correlated
	histoContent::addStatCorrelated=false;
	for(int systLayer=-1;systLayer<(int) conts_.at(0).contents_.layerSize();systLayer++){
		for(size_t ybin=0;ybin<ybins_.size();ybin++){
			const container1D * ycont=&conts_[ybin];
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
	container1D::c_makelist=tempc_makelist;
	return out;
}


container1D container2D::getYSlice(size_t ybinno) const{
	if(ybinno >= ybins_.size()){
		std::cout << "container2D::getYSlice: bin out of range for " <<name_ << std::endl;
		return container1D();
	}
	container1D out = conts_.at(ybinno);
	out.setName(name_+"_YSlice_"+toTString(ybinno));
	return out;
}
container1D container2D::getXSlice(size_t xbinno) const{
	if(xbinno >= xbins_.size()){
		std::cout << "container2D::getXSlice: bin out of range for " <<name_ << std::endl;
		return container1D();
	}
	std::vector<float> rybins=ybins_;
	rybins.erase(rybins.begin()); //no UF
	container1D out(rybins,name_+"_XSlice_"+toTString(xbinno),yaxisname_,"");
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
void container2D::setXSlice(size_t xbinno,const container1D & in, bool UFOF){
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
void container2D::setYSlice(size_t ybinno,const container1D & in, bool UFOF){
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
 * then, container1D, 2D, 1DUF can be independent of root
 * Stack could be, too, but again needs drawer class (can use different drawer class objects internally)
 * may even use other graphic representations as root
 * in principle drawing of systematics etc can be managed by the class .setDrawStatOnly, .drawSyst...., .setStyle, .setStyle.setMarker/Line.... (style is own new struct/class)
 */
TH2D * container2D::getTH2D(TString name, bool dividebybinarea, bool onlystat) const{
	if(getNBinsX() < 1 || getNBinsY() <1)
		return 0; //there is not even one real, non OF or UF container

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
	if(entries>0)
	h->SetEntries(entries);
	else
		h->SetEntries(getNBinsX()*getNBinsY());
	return h;
}
/**
 * TH2D * container2D::getTH2DSyst(TString name, unsigned int systNo, bool dividebybinwidth=false, bool statErrors=false)
 */
TH2D * container2D::getTH2DSyst(TString name, unsigned int systNo, bool dividebybinarea, bool statErrors) const{
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
container2D & container2D::import(std::vector<TGraphAsymmErrors *> gs,bool isbinwidthdivided, const TString & newsystname){
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
		container1D tmp=conts_.at(1);
		tmp.setAllZero();
		conts_.at(0)=tmp;
		conts_.at(conts_.size()-1)=tmp;
	}

	std::cout << "herec"<< std::endl;
	return *this;

}




void container2D::setDivideBinomial(bool binomial){
	std::cout << "container2D::setDivideBinomial: obsolete / switch on static histoContent options" <<std::endl;
	/*for(size_t i=0;i<conts_.size();i++)
		conts_.at(i).setDivideBinomial(binomial);
	divideBinomial_=binomial;*/
}

container2D & container2D::operator += (const container2D & second){
	if(second.xbins_ != xbins_ || second.ybins_ != ybins_){
		std::cout << "container2D::operator +=: "<< name_ << " and " << second.name_<<" must have same binning! returning *this" << std::endl;
		return *this;
	}

	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i) += second.conts_.at(i);
	}
	return *this;
}
container2D container2D::operator + (const container2D & second){
	container2D copy=*this;
	return copy+=second;
}
container2D & container2D::operator -= (const container2D & second){
	if(second.xbins_ != xbins_ || second.ybins_ != ybins_){
		std::cout << "container2D::operator -: "<< name_ << " and " << second.name_<<" must have same binning! returning *this"  << std::endl;
		return *this;
	}
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i) -=  second.conts_.at(i);
	}
	return *this;
}
container2D container2D::operator - (const container2D & second){
	container2D copy=*this;
	return copy -= second;
}

container2D & container2D::operator /= (const container2D & second){
	if(second.xbins_ != xbins_ || second.ybins_ != ybins_){
		std::cout << "container2D::operator /: "<< name_ << " and " << second.name_<<" must have same binning! returning *this"  << std::endl;
		return *this;
	}
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i) /= second.conts_.at(i);
	}
	return *this;
}
container2D  container2D::operator / (const container2D & second){
	container2D copy=*this;
	return copy/=second;
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
container2D container2D::operator * (float val){
	container2D out=*this;
	for(size_t i=0;i<conts_.size();i++){
		out.conts_.at(i) *= val;
	}
	return out;
}
container2D container2D::operator * (int val){
	return *this * (float) val;
}
void container2D::addErrorContainer(const TString & sysname,const container2D & second ,float weight){
	if(second.xbins_ != xbins_ || second.ybins_ != ybins_){
		std::cout << "container2D::addErrorContainer "<< name_ << " and " << second.name_<<" must have same binning!"  << std::endl;
		return;
	}
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i).addErrorContainer(sysname, second.conts_.at(i),weight);
	}
}
void container2D::addErrorContainer(const TString & sysname,const container2D & second ){
	addErrorContainer(sysname,second , 1);
}
void container2D::getRelSystematicsFrom(const container2D & second){
	if(second.xbins_ != xbins_ || second.ybins_ != ybins_){
		std::cout << "container2D::getRelSystematicsFrom "<< name_ << " and " << second.name_<<" must have same binning!"  << std::endl;
		return;
	}
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i).getRelSystematicsFrom(second.conts_.at(i));
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
void container2D::removeAllSystematics(){
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i).removeAllSystematics();
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
void container2D::addGlobalRelError(TString errname,float val){
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i).addGlobalRelError(errname,val);
	}
}

void container2D::mergePartialVariations(const TString& identifier,bool strictpartialID){
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i).mergePartialVariations(identifier,strictpartialID);
	}
}
void container2D::mergeAllErrors(const TString & mergedname){
	for(size_t i=0;i<conts_.size();i++){
		conts_.at(i).mergeAllErrors(mergedname);
	}
}

}//namespace
