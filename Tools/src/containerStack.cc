#include "../interface/containerStack.h"
#include <algorithm>
#include <vector>

#include "../interface/systAdder.h"

namespace ztop{

std::vector<ztop::containerStack*> containerStack::cs_list;
bool containerStack::cs_makelist=false;
bool containerStack::batchmode=true;
bool containerStack::debug=false;
bool containerStack::addErrorsStrict=true;

containerStack::containerStack() : taggedObject(taggedObject::type_containerStack), dataleg_("data"), mode(notset){
	setName("");
	if(cs_makelist)cs_list.push_back(this);
}
containerStack::containerStack(TString name) : taggedObject(taggedObject::type_containerStack), dataleg_("data"), mode(notset) {
	mode=notset;
	setName(name);
	if(cs_makelist) cs_list.push_back(this);
}
containerStack::~containerStack(){

	containers_.clear();
	containers2D_.clear();
	containers1DUnfold_.clear();
	legends_.clear();
	colors_.clear();
	norms_.clear();
	for(unsigned int i=0;i<cs_list.size();i++){
		if(cs_list[i] == this) cs_list.erase(cs_list.begin()+i);
		break;
	}
}
void containerStack::push_back(const ztop::container1D& cont,const TString& legend, int color, double norm, int legord){
	if(mode==notset)
		mode=dim1;
	else if(mode!=dim1){
		std::cout << "containerStack::push_back: trying to add 1D container to non-1D stack! ("<<name_<<")" << std::endl;
		return;
	}
	if(cont.hasTag()){
		if(debug) std::cout << "containerStack::push_back: adding tag" << std::endl;
		addTagsFrom(&cont);
	}

	bool wasthere=false;
	if(legends_.size()>0){
		//quick binning check
		if(cont.getBins() != containers_.at(0).getBins())
			throw std::logic_error("containerStack::push_back: binning has to be the same");
	}

	for(unsigned int i=0;i<legends_.size();i++){
		if(legend == legends_[i]){
			if(containerStack::debug)
				std::cout << "containerStack::push_back(1D): found same legend ("<<  legend <<"), adding " << cont.getName() << std::endl;
			containers_[i] += cont * norm;
			containers_[i].setName(legend);
			norms_[i]=1;//deprecated
			legorder_[i]=legord;
			wasthere=true;
			break;
		}
	}
	if(!wasthere){
		if(containerStack::debug)
			std::cout << "containerStack::push_back (1D): new legend ("<<  legend <<"), adding " << cont.getName() << std::endl;
		container1D cp=cont;
		cp.setName(legend);
		containers_.push_back(cp*norm);
		legends_.push_back(legend);
		colors_.push_back(color);
		norms_.push_back(1);
		legorder_.push_back(legord);
	}
}

void containerStack::push_back(const ztop::container2D& cont,const TString& legend, int color, double norm, int legord){
	if(mode==notset)
		mode=dim2;
	else if(mode!=dim2){
		std::cout << "containerStack::push_back(2D): trying to add 2D container to non-2D stack!  ("<<name_<<")" << std::endl;
		return;
	}
	bool wasthere=false;
	if(cont.hasTag()){
		addTagsFrom(&cont);
	}
	for(unsigned int i=0;i<legends_.size();i++){
		if(legend == legends_[i]){
			if(containerStack::debug)
				std::cout << "containerStack::push_back(2D): found same legend ("<<  legend <<"), adding " << cont.getName() << std::endl;
			containers2D_[i] +=  cont * norm;
			containers2D_[i].setName(legend);
			norms_[i]=1;
			legorder_[i]=legord;
			wasthere=true;
			break;
		}
	}
	if(!wasthere){
		if(containerStack::debug)
			std::cout << "containerStack::push_back (2D): new legend ("<<  legend <<"), adding " << cont.getName() << std::endl;
		container2D cp=cont;
		cp.setName(legend);
		containers2D_.push_back(cp*norm);
		legends_.push_back(legend);
		colors_.push_back(color);
		norms_.push_back(1);
		legorder_.push_back(legord);
	}
}
void containerStack::push_back(const ztop::container1DUnfold& cont,const TString& legend, int color, double norm, int legord){
	if(mode==notset)
		mode=unfolddim1;
	else if(mode!=unfolddim1){
		std::cout << "containerStack::push_back: trying to add 1DUnfold container to non-1DUnfold stack!  ("<<name_<<")"<<  std::endl;
		return;
	}
	if(cont.hasTag()){
		addTagsFrom(&cont);
	}
	container1D cont1d=cont.getControlPlot();
	bool wasthere=false;
	for(unsigned int i=0;i<legends_.size();i++){
		if(legend == legends_[i]){
			if(containerStack::debug)
				std::cout << "containerStack::push_back(1DUF): found same legend ("<<  legend <<"), adding " << cont.getName() << std::endl;
			containers1DUnfold_[i] += cont * norm;
			containers1DUnfold_[i].setName(legend);
			containers_[i] += cont1d * norm;
			containers_[i].setName(getName());
			norms_[i]=1;
			legorder_[i]=legord;
			wasthere=true;
			break;
		}
	}
	if(!wasthere){
		if(containerStack::debug)
			std::cout << "containerStack::push_back (1DUF): new legend ("<<  legend <<"), adding " << cont.getName() << std::endl;
		container1DUnfold cp=cont;
		cp.setName(legend);
		containers1DUnfold_.push_back(cp * norm);
		cont1d.setName(getName());
		containers_.push_back(cont1d * norm);
		legends_.push_back(legend);
		colors_.push_back(color);
		norms_.push_back(1);
		legorder_.push_back(legord);
	}

}

void containerStack::setLegendOrder(const TString &leg, const size_t& no){
	for(size_t i=0;i<legends_.size();i++){
		if(legends_.at(i)==leg){
			if(legorder_.size()>i){
				legorder_.at(i)=no;
				return;
			}
			else{
				std::cout << "containerStack::setLegendOrder: size doesn't match" <<std::endl;
				return;
			}
		}
	}
}


void containerStack::mergeLegends(const std::vector<TString>& tobemerged,const TString & mergedname, int mergedColor, bool allowsignals){
	std::vector<size_t> idxstbm;
	for(size_t i=0;i<tobemerged.size();i++){
		idxstbm.push_back(getContributionIdx(tobemerged.at(i)));
		if(debug)	std::cout << i << std::endl;
	}

	if(idxstbm.size()<2) return;
	size_t inidx=idxstbm.at(0);

	std::vector<TString> newsignals;
	bool newsignentry=false;
	for(size_t i=0;i<signals_.size();i++){
		const TString & signname=signals_.at(i);
		if(std::find(tobemerged.begin(),tobemerged.end(),signname) != tobemerged.end()){
			newsignentry=true;
		}
		else{
			newsignals.push_back(signname);
		}
	}if(newsignentry){
		if(!allowsignals)
			throw std::runtime_error("containerStack::mergeLegends: try to merge signals without allowing explicitely for it");
		newsignals.push_back(mergedname);
	}

	std::vector<TString> legs;
	std::vector<int> cols;
	std::vector<int> legos;
	std::vector<double>  norms;
	std::vector<container1D> newconts;
	std::vector<container1DUnfold> newcontsuf;
	std::vector<container2D> newconts2d;

	size_t newidx=0;

	for(size_t i=0;i<legends_.size();i++){
		if(std::find(idxstbm.begin(),idxstbm.end(),i) == idxstbm.end() || i==inidx){ // use lateron
			if(is1D()||is1DUnfold()){
				newconts.push_back(containers_.at(i) *norms_.at(i));
				if(i==inidx)newidx=newconts.size()-1;
			}
			if(is1DUnfold()){
				newcontsuf.push_back(containers1DUnfold_.at(i)*norms_.at(i));
				if(i==inidx)newidx=newcontsuf.size()-1;
			}
			if(is2D()){
				newconts2d.push_back( containers2D_.at(i)*norms_.at(i));
				if(i==inidx)newidx=newconts2d.size()-1;
			}
			if(debug)std::cout << "reusing " << legends_.at(i) <<std::endl;
			legs.push_back(legends_.at(i));
			cols.push_back(colors_.at(i));
			legos.push_back(legorder_.at(i));
			norms.push_back(1);
		}
	}
	for(size_t i=0;i<legends_.size();i++){
		if(std::find(idxstbm.begin(),idxstbm.end(),i) != idxstbm.end() && i!=inidx){
			if(is1D()||is1DUnfold()){
				if(debug)std::cout << "adding  " << legends_.at(i) << " to " << legs.at(newidx)<<std::endl;
				newconts.at(newidx)+=(containers_.at(i) *norms_.at(i));
			}
			if(is1DUnfold()){
				if(debug)std::cout << "adding  " << legends_.at(i) << " to " << legs.at(newidx)<<std::endl;
				newcontsuf.at(newidx)+=(containers1DUnfold_.at(i)*norms_.at(i));
			}
			if(is2D()){
				newconts2d.at(newidx)+=( containers2D_.at(i)*norms_.at(i));
			}
		}
	}
	//remove

	if(debug){
		//debug
		if(legs.size() != cols.size())
			throw std::logic_error("legs colz");
		if(cols.size() != legos.size())
			throw std::logic_error("legos colz");
		if(norms.size() != cols.size())
			throw std::logic_error("norms colz");

		std::cout << "cols: " << cols.size() << " \n"
				<< "legs: " << legs.size() << " \n"
				<< "legos: " << legos.size() << " \n"
				<< "norms: " << norms.size() << " \n"
				<< "newconts: " << newconts.size() << " \n"
				<< "newcontsuf: " << newcontsuf.size() << " \n"
				<< "newconts2d: " << newconts2d.size() << " \n"
				<< "newsignals: " << newsignals.size() << " \n" <<std::endl;
	}
	legends_=legs;
	legends_.at(newidx) = mergedname;
	colors_=cols;
	colors_.at(newidx) = mergedColor;
	legorder_=legos;
	norms_=norms;
	containers_=newconts;
	containers1DUnfold_=newcontsuf;
	containers2D_=newconts2d;
	signals_=newsignals;


}
void containerStack::mergeLegends(const TString& tobemergeda,const TString & tobemergedb,const TString & mergedname, int mergedColor, bool allowsignal){
	std::vector<TString> tbm;
	tbm.push_back(tobemergeda);
	tbm.push_back(tobemergedb);
	mergeLegends(tbm,mergedname,mergedColor,allowsignal);
}

/**
 * names without "up" and "down"
 */
void containerStack::mergeVariations(const std::vector<TString>& names, const TString & outname,bool linearly){
	for(size_t i=0;i<containers_.size();i++)
		containers_.at(i).mergeVariations(names,outname,linearly);
	for(size_t i=0;i<containers2D_.size();i++)
		containers2D_.at(i).mergeVariations(names,outname,linearly);
	for(size_t i=0;i<containers1DUnfold_.size();i++)
		containers1DUnfold_.at(i).mergeVariations(names,outname,linearly);


}

void containerStack::mergeVariationsFromFileInCMSSW(const std::string& filename){
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


void containerStack::removeContribution(TString legendname){
	bool found=false;
	std::vector<TString>::iterator leg=legends_.begin();
	std::vector<int>::iterator col=colors_.begin();
	std::vector<int>::iterator lego=legorder_.begin();
	std::vector<double>::iterator norm=norms_.begin();
	size_t rmpos=0;
	for(size_t i=0;i<legends_.size();i++){
		if(*leg == legendname){
			legends_.erase(leg);
			colors_.erase(col);
			norms_.erase(norm);
			legorder_.erase(lego);
			rmpos=i;
			found=true;
			break;
		}
		++leg;++col;++norm;++lego;

	}
	if(mode==dim1 || mode==unfolddim1)
		containers_.erase(containers_.begin()+rmpos);
	else if(mode==dim2)
		containers2D_.erase(containers2D_.begin()+rmpos);
	else if(mode==unfolddim1)
		containers1DUnfold_.erase(containers1DUnfold_.begin()+rmpos);

	if(!found) std::cout << "containerStack::removeContribution: " << legendname << " not found." <<std::endl;

}


int containerStack::getContributionIdx(TString legname) const{
	for(size_t i=0;i<legends_.size();i++){
		if(legname == legends_.at(i))
			return i;
	}
	std::cout << "containerStack::getContributionIdx: " << legname << " not found!" <<std::endl;
	throw std::out_of_range("containerStack::getContributionIdx: contribution does not exists");
	return -1;
}

ztop::container1D containerStack::getContribution(TString contr) const{
	ztop::container1D out;
	int idx=getContributionIdx(contr);
	if(idx<0 || mode != dim1)
		return out;
	out=containers_.at(idx);
	out*= norms_.at(idx);
	return out;
}
ztop::container2D containerStack::getContribution2D(TString contr)const{
	ztop::container2D out;
	int idx=getContributionIdx(contr);
	if(idx<0 || mode != dim2)
		return out;
	out=containers2D_.at(idx);
	out*= norms_.at(idx);
	return out;
}
ztop::container1DUnfold containerStack::getContribution1DUnfold(TString contr)const{
	ztop::container1DUnfold out;
	int idx=getContributionIdx(contr);
	if(idx<0 || mode != unfolddim1)
		return out;
	out=containers1DUnfold_.at(idx);
	out*= norms_.at(idx);
	return out;
}
ztop::container1D containerStack::getContributionsBut(TString contr)const{
	ztop::container1D out;
	if(mode != dim1)
		return out;
	out=containers_.at(0);
	out.clear();
	int i=0;
	for(std::vector<TString>::const_iterator name=legends_.begin();name<legends_.end();++name){
		if(contr != *name){
			container1D temp=containers_[i];
			out=out+temp * norms_[i];
		}
		i++;
	}
	return out;
}
ztop::container2D containerStack::getContributions2DBut(TString contr) const{
	ztop::container2D out;
	if(mode != dim2)
		return out;
	out=containers2D_.at(0);
	out.clear();
	int i=0;
	for(std::vector<TString>::const_iterator name=legends_.begin();name<legends_.end();++name){
		if(contr != *name){
			container2D temp=containers2D_[i];
			out=out+temp * norms_[i];
		}
		i++;
	}
	return out;
}
ztop::container1DUnfold containerStack::getContributions1DUnfoldBut(TString contr) const{
	ztop::container1DUnfold out;
	if(mode != dim2)
		return out;
	out=containers1DUnfold_.at(0);
	out.clear();
	int i=0;
	for(std::vector<TString>::const_iterator name=legends_.begin();name<legends_.end();++name){
		if(contr != *name){
			container1DUnfold temp=containers1DUnfold_[i];
			out=out+temp * norms_[i];
		}
		i++;
	}
	return out;
}
ztop::container1D containerStack::getContributionsBut(std::vector<TString> contr) const{
	ztop::container1D out;
	if(mode!=dim1)
		return out;
	out=containers_.at(0);
	out.clear();
	for(unsigned int i=0;i<legends_.size();i++){
		bool get=true;
		for(std::vector<TString>::const_iterator name=contr.begin();name<contr.end();++name){
			if(*name == legends_[i]){
				get=false;
				break;
			}
		}
		if(get){
			container1D temp=containers_[i];
			out +=  temp* norms_[i];
		}
	}
	return out;
}
ztop::container2D containerStack::getContributions2DBut(std::vector<TString> contr) const{
	ztop::container2D out;
	if(mode!=dim2)
		return out;
	out=containers2D_.at(0);
	out.clear();
	for(unsigned int i=0;i<legends_.size();i++){
		bool get=true;
		for(std::vector<TString>::const_iterator name=contr.begin();name<contr.end();++name){
			if(*name == legends_[i]){
				get=false;
				break;
			}
		}
		if(get){
			container2D temp=containers2D_[i] ;
			out = out+ temp* norms_[i];
		}
	}
	return out;
}
ztop::container1DUnfold containerStack::getContributions1DUnfoldBut(std::vector<TString> contr) const{
	ztop::container1DUnfold out;
	if(mode!=unfolddim1)
		return out;
	out=containers1DUnfold_.at(0);
	out.clear();
	for(unsigned int i=0;i<legends_.size();i++){
		bool get=true;
		for(std::vector<TString>::const_iterator name=contr.begin();name<contr.end();++name){
			if(*name == legends_[i]){
				get=false;
				break;
			}
		}
		if(get){
			container1DUnfold temp=containers1DUnfold_[i] ;
			out = out+ temp* norms_[i];
		}
	}
	return out;
}

void containerStack::multiplyNorm(size_t i , double multi){
	if(i>=size()){
		throw std::out_of_range("containerStack::multiplyNorm: out of range");
	}
	if(dim1){
		containers_.at(i)*=multi;
	}
	else if(dim2){
		containers2D_.at(i)*=multi;
	}
	else if(unfolddim1){
		containers_.at(i)*=multi;
		containers1DUnfold_.at(i)*=multi;
	}
}

void containerStack::multiplyNorm(TString legendentry, double multi){
	int i=0;
	for(std::vector<TString>::iterator name=legends_.begin();name<legends_.end();++name){
		if(legendentry == *name){
			if(dim1){
				containers_.at(i)*=multi;
			}
			else if(dim2){
				containers2D_.at(i)*=multi;
			}
			else if(unfolddim1){
				containers_.at(i)*=multi;
				containers1DUnfold_.at(i)*=multi;
			}
			break;
		}
		i++;
	}

}

void containerStack::multiplyAllMCNorms(double multiplier){
	for(unsigned int i=0;i<legends_.size();i++){
		if(legends_.at(i)!=dataleg_){
			if(dim1){
				containers_.at(i)*=multiplier;
			}
			else if(dim2){
				containers2D_.at(i)*=multiplier;
			}
			else if(unfolddim1){
				containers_.at(i)*=multiplier;
				containers1DUnfold_.at(i)*=multiplier;
			}
		}
	}
}

void containerStack::addGlobalRelMCError(TString sysname,double error){
	for(unsigned int i=0;i<containers_.size();i++){
		if(legends_[i]!=dataleg_) containers_[i].addGlobalRelError(sysname,error);
		else containers_[i].addGlobalRelError(sysname,0);
	}
	for(unsigned int i=0;i<containers2D_.size();i++){
		if(legends_[i]!=dataleg_) containers2D_[i].addGlobalRelError(sysname,error);
		else containers2D_[i].addGlobalRelError(sysname,0);
	}
	for(unsigned int i=0;i<containers1DUnfold_.size();i++){
		if(legends_[i]!=dataleg_) containers1DUnfold_[i].addGlobalRelError(sysname,error);
		else containers1DUnfold_[i].addGlobalRelError(sysname,0);
	}
}
void containerStack::addRelErrorToContribution(double err, const TString& contributionname, TString nameprefix){
	size_t idx=getContributionIdx(contributionname);
	TString sysname=nameprefix+"_"+contributionname;
	containerStack tmp=*this;
	tmp.multiplyNorm(idx,(1+err));
	addErrorStack(sysname+"_up",tmp);
	tmp=*this;
	tmp.multiplyNorm(idx,(1-err));
	addErrorStack(sysname+"_down",tmp);
}

void containerStack::addRelErrorToBackgrounds(double err ,bool aspartials , TString nameprefix,const TString& excludecontr){
	std::vector<TString> vec;
	vec.push_back(excludecontr);
	addRelErrorToBackgrounds(err,aspartials,nameprefix,vec);
}
void containerStack::addRelErrorToBackgrounds(double err ,bool aspartials , TString nameprefix,const std::vector<TString> excludecontr){
	std::vector<size_t> excludeidxs;
	for(size_t i=0;i<excludecontr.size();i++)
		excludeidxs.push_back(getContributionIdx(excludecontr.at(i)));
	addRelErrorToBackgrounds(err,aspartials,nameprefix,excludeidxs);
}

void containerStack::addRelErrorToBackgrounds(double err ,bool aspartials , TString nameprefix,const std::vector<size_t> excludeidxs){
	std::vector<size_t> signals = getSignalIdxs();
	size_t dataidx=std::find(legends_.begin(),legends_.end(),dataleg_)-legends_.begin();
	signals.insert(signals.end(),excludeidxs.begin(),excludeidxs.end());
	signals.push_back(dataidx);
	if(!nameprefix.EndsWith("_"))
		nameprefix+="_";

	if(aspartials)
		nameprefix+=(TString)container1D_partialvariationIDString+"_";

	if(is1D()){
		for(size_t i=0;i<containers_.size();i++){
			if(std::find(signals.begin(),signals.end(),i) != signals.end()) continue;
			//add error to all of them
			for(size_t j=0;j<containers_.size();j++){
				if(j==i)
					containers_.at(j).addGlobalRelError(nameprefix+legends_.at(i),err);
				else
					containers_.at(j).addGlobalRelError(nameprefix+legends_.at(i),0);
			}
		}
	}
	else if(is2D()){
		for(size_t i=0;i<containers2D_.size();i++){
			if(std::find(signals.begin(),signals.end(),i) != signals.end()) continue;
			for(size_t j=0;j<containers_.size();j++){
				if(j==i)
					containers2D_.at(j).addGlobalRelError(nameprefix+legends_.at(i),err);
				else
					containers2D_.at(j).addGlobalRelError(nameprefix+legends_.at(i),0);
			}
		}
	}
	else if(is1DUnfold()){
		for(size_t i=0;i<containers1DUnfold_.size();i++){
			if(std::find(signals.begin(),signals.end(),i) != signals.end()) continue;
			for(size_t j=0;j<containers_.size();j++){
				if(j==i)
					containers1DUnfold_.at(j).addGlobalRelError(nameprefix+legends_.at(i),err);
				else
					containers1DUnfold_.at(j).addGlobalRelError(nameprefix+legends_.at(i),0);
			}
		}
	}

}

void containerStack::mergePartialVariations(const TString& identifier, bool strictpartialID){

	for(size_t i=0;i<containers_.size();i++){
		containers_.at(i).mergePartialVariations(identifier,strictpartialID);
	}
	for(size_t i=0;i<containers2D_.size();i++){
		containers2D_.at(i).mergePartialVariations(identifier,strictpartialID);
	}
	for(size_t i=0;i<containers1DUnfold_.size();i++){
		containers1DUnfold_.at(i).mergePartialVariations(identifier,strictpartialID);
	}
}

void containerStack::addMCErrorStack(const TString &sysname,const containerStack &errorstack){
	addErrorStack( sysname, errorstack);
}

void containerStack::addErrorStack(const TString & sysname,const containerStack& errorstack){
	if(mode!=errorstack.mode){
		std::cout << "containerStack::addErrorStack: stacks must have same type" << std::endl;
		return;
	}
	if(debug){
		std::cout << "containerStack::addErrorStack: " << name_ << std::endl;
		std::cout << "legends lhs: ";
		for(size_t i=0;i<legends_.size();i++)
			std::cout << legends_.at(i) << " ";
		std::cout << std::endl;
		std::cout << "legends rhs: ";
		for(size_t i=0;i<errorstack.legends_.size();i++)
			std::cout << errorstack.legends_.at(i) << " ";
		std::cout << std::endl;
	}

	for(unsigned int i=0; i<size();i++){
		/*bool found=false;
		for(unsigned int j=0;j<errorstack.size();j++){
			if(legends_[i] == errorstack.legends_[j]){ //costs performance!
		 */
		std::vector<TString>::const_iterator it=std::find(errorstack.legends_.begin(),errorstack.legends_.end(),legends_[i]);
		if(it!=errorstack.legends_.end()){
			size_t j=it-errorstack.legends_.begin();
			//found=true;
			if(mode==dim1 || mode==unfolddim1){
				//errorstack.containers_[j] = errorstack.containers_[j] * errorstack.norms_[j]; //normalize (in case there is any remultiplication done or something)
				if(errorstack.norms_[j]!=1 || norms_[i]!=1)
					throw std::logic_error("containerStack: all entries in the (deprecated) norms_ vector need to be 1!!!");
				if(containers_[i].addErrorContainer(sysname,errorstack.containers_[j])<0){
					std::cout << "containerStack::addErrorStack: Problem in " << name_ <<std::endl;
				}
			}
			if(mode==dim2){
				if(errorstack.norms_[j]!=1 || norms_[i]!=1)
					throw std::logic_error("containerStack: all entries in the (deprecated) norms_ vector need to be 1!!!");
				if(containers2D_[i].addErrorContainer(sysname,errorstack.containers2D_[j])<0){
					std::cout << "containerStack::addErrorStack: Problem in " << name_ <<std::endl;
				}
			}
			if(mode==unfolddim1){
				if(errorstack.norms_[j]!=1 || norms_[i]!=1)
					throw std::logic_error("containerStack: all entries in the (deprecated) norms_ vector need to be 1!!!");
				if(containers1DUnfold_[i].addErrorContainer(sysname,errorstack.containers1DUnfold_[j])<0){
					std::cout << "containerStack::addErrorStack: Problem in " << name_ <<std::endl;
				}
			}
		}//legfound
		/*} */
		else{ //if(!found)
			if(addErrorsStrict){
				throw std::runtime_error("containerStack::addErrorStack: legend in one stack not found. To allow this, switch containerStack::addErrorsStrict to false");
			}
			std::cout << "containerStack::addErrorStack: "<< name_ << " legend " << legends_[i] << " not found. adding 0 error!" << std::endl;
			if(mode==dim1 || mode==unfolddim1){
				containers_[i].addErrorContainer(sysname,containers_[i]);
			}
			if(mode==dim2){
				containers2D_[i].addErrorContainer(sysname,containers2D_[i]);
			}
			if(mode==unfolddim1){
				containers1DUnfold_[i].addErrorContainer(sysname,containers1DUnfold_[i]);
			}

		}
	}
}

void containerStack::getRelSystematicsFrom(const ztop::containerStack & stack){
	for(size_t j=0;j<stack.legends_.size();j++){
		for(unsigned int i=0;i<legends_.size();i++){
			if(legends_[i] == stack.legends_.at(j)){
				if(debug)
					std::cout << "containerStack::getRelSystematicsFrom: adding to stack " << name_ << std::endl;
				if(i<containers_.size() && j<stack.containers_.size())
					containers_[i].getRelSystematicsFrom(stack.containers_.at(j));
				if(i<containers2D_.size() && j<stack.containers2D_.size())
					containers2D_[i].getRelSystematicsFrom(stack.containers2D_.at(j));
				if(i<containers1DUnfold_.size() && j<stack.containers1DUnfold_.size())
					containers1DUnfold_[i].getRelSystematicsFrom(stack.containers1DUnfold_.at(j));

				break;
			}
		}
	}
}
void containerStack::addRelSystematicsFrom(const ztop::containerStack & stack,bool ignorestat,bool strict){

	for(size_t j=0;j<stack.legends_.size();j++){
		for(unsigned int i=0;i<legends_.size();i++){
			if(legends_[i] == stack.legends_.at(j)){
				if(debug)
					std::cout << "containerStack::addRelSystematicsFrom: adding to stack " << name_ << std::endl;
				if(i<containers_.size() && j<stack.containers_.size())
					containers_[i].addRelSystematicsFrom(stack.containers_.at(j),ignorestat,strict);
				if(i<containers2D_.size() && j<stack.containers2D_.size())
					containers2D_[i].addRelSystematicsFrom(stack.containers2D_.at(j),ignorestat,strict);
				if(i<containers1DUnfold_.size() && j<stack.containers1DUnfold_.size())
					containers1DUnfold_[i].addRelSystematicsFrom(stack.containers1DUnfold_.at(j),ignorestat,strict);

				break;
			}
		}
	}
}

void containerStack::removeError(TString sysname){
	for(unsigned int i=0; i<containers_.size();i++){
		containers_[i].removeError(sysname);
	}
	for(unsigned int i=0; i<containers2D_.size();i++){
		containers2D_[i].removeError(sysname);
	}
	for(unsigned int i=0; i<containers1DUnfold_.size();i++){
		containers1DUnfold_[i].removeError(sysname);
	}
}

void containerStack::removeAllSystematics(){
	for(unsigned int i=0; i<containers_.size();i++){
		containers_[i].removeAllSystematics();
	}
	for(unsigned int i=0; i<containers2D_.size();i++){
		containers2D_[i].removeAllSystematics();
	}
	for(unsigned int i=0; i<containers1DUnfold_.size();i++){
		containers1DUnfold_[i].removeAllSystematics();
	}
}

void containerStack::renameSyst(TString old, TString New){
	for(unsigned int i=0; i<containers_.size();i++){
		containers_[i].renameSyst(old,New);
	}
	for(unsigned int i=0; i<containers2D_.size();i++){
		containers2D_[i].renameSyst(old,New);
	}
	for(unsigned int i=0; i<containers1DUnfold_.size();i++){
		containers1DUnfold_[i].renameSyst(old,New);
	}

}
std::vector<size_t> containerStack::removeSpikes(bool inclUFOF,int limittoindex,
		float strength,float sign,float threshold){

	//not defined for container1D
	std::vector<size_t> out,temp;

	for(unsigned int i=0; i<containers2D_.size();i++){
		temp=containers2D_[i].removeSpikes(inclUFOF,limittoindex,strength,sign,threshold);
		out.insert(out.end(),temp.begin(),temp.end());
	}
	for(unsigned int i=0; i<containers1DUnfold_.size();i++){
		temp=containers1DUnfold_[i].removeSpikes(inclUFOF,limittoindex,strength,sign,threshold);
		out.insert(out.end(),temp.begin(),temp.end());
	}
	return out;
}



ztop::container1D containerStack::getFullMCContainer()const{
	container1D out;
	if(containers_.size()<1)
		return out;
	out=containers_[0];
	out.clear();
	for(unsigned int i=0;i<containers_.size();i++){
		if(legends_[i] != dataleg_) {
			container1D temp=containers_[i];
			out = out + temp*norms_[i];
		}
	}
	return out;
}
ztop::container2D containerStack::getFullMCContainer2D()const{
	container2D out;
	if(containers2D_.size()<1)
		return out;
	out=containers2D_[0];
	out.clear();
	for(unsigned int i=0;i<containers2D_.size();i++){
		if(legends_[i] != dataleg_){
			container2D temp= containers2D_[i];
			out = out + temp*norms_[i];
		}
	}
	return out;
}
ztop::container1DUnfold containerStack::getFullMCContainer1DUnfold()const {
	container1DUnfold out;
	if(containers1DUnfold_.size()<1)
		return out;
	out=containers1DUnfold_[0];
	out.clear();
	for(unsigned int i=0;i<containers1DUnfold_.size();i++){
		if(legends_[i] != dataleg_){
			container1DUnfold temp=containers1DUnfold_[i];
			out = out + temp*norms_[i];
		}
	}
	return out;
}

THStack * containerStack::makeTHStack(TString stackname){
	if(!checkDrawDimension()){
		std::cout << "containerStack::makeTHStack: only available for 1dim stacks!" <<std::endl;
		return 0;
	}
	if(debug)
		std::cout << "containerStack::makeTHStack: ..." <<std::endl;
	TH1::AddDirectory(false);
	if(stackname == "") stackname = name_+"_s";
	THStack *tstack = addObject(new THStack(stackname,stackname));

	std::vector<size_t> sorted=getSortedIdxs(false);
	if(debug)
		std::cout << "containerStack::makeTHStack: sorted entries" <<std::endl;

	if(debug)
		std::cout << "containerStack::makeTHStack: size="<<containers_.size() <<std::endl;
	for(unsigned int it=0;it<size();it++){
		if(debug)
			std::cout << "containerStack::makeTHStack: get "<< it;
		size_t i=sorted.at(it);
		if(getLegend(i) != dataleg_){
			if(debug)
				std::cout <<"->"<<i << ": " << getLegend(i)<< std::endl;
			container1D tempcont = getContainer(i);

			tempcont = tempcont * getNorm(i);
			TH1D * htemp=(tempcont.getTH1D(getLegend(i)+" "+getName()+"_stack_h"));
			if(!htemp)
				continue;
			TH1D * h = addObject((TH1D*)htemp->Clone());
			delete htemp;
			h->SetFillColor(getColor(i));
			for(int bin = 1 ; bin < h->GetNbinsX()+1;bin++){
				h->SetBinError(bin,0);
			}

			tstack->Add(h);

		}
	}
	if(debug)
		std::cout << "containerStack::makeTHStack: stack produced" <<std::endl;
	return  tstack;
}
TLegend * containerStack::makeTLegend(bool inverse){
	if(!checkDrawDimension()){
		std::cout << "containerStack::makeTLegend: only available for 1dim stacks!" <<std::endl;
		return 0;
	}
	if(debug)
		std::cout << "containerStack::makeTLegend: drawing..." <<std::endl;
	TLegend *leg = addObject(new TLegend((Double_t)0.65,(Double_t)0.50,(Double_t)0.95,(Double_t)0.90));
	leg->Clear();
	leg->SetFillStyle(0);
	leg->SetBorderSize(0);

	std::vector<size_t> sorted=getSortedIdxs(inverse);

	for(unsigned int it=0;it<size();it++){
		size_t i=sorted.at(it);
		container1D tempcont = containers_[i];
		tempcont = tempcont * getNorm(i);
		TH1D * h = addObject((TH1D*)tempcont.getTH1D(getLegend(i)+" "+getName()+"_leg")->Clone());
		h->SetFillColor(getColor(i));
		if(getLegend(i) != dataleg_) leg->AddEntry(h,getLegend(i),"f");
		else leg->AddEntry(h,getLegend(i),"ep");
		//  delete h;
	}

	return leg;
}
void containerStack::drawControlPlot(TString name, bool drawxaxislabels, double resizelabels) {
	if(!checkDrawDimension()){
		std::cout << "containerStack::drawControlPlot: only available for 1dim(unfold) stacks!" <<std::endl;
		return;
	}
	if(debug)
		std::cout << "containerStack::drawControlPlot: drawing " <<name_ <<std::endl;
	TH1::AddDirectory(false);
	if(name=="") name=name_;
	int dataentry=0;
	bool gotdentry=false;
	bool gotuf=false;
	bool gotof=false;
	for(unsigned int i=0;i<size();i++){ // get datalegend and check if underflow or overflow in any container
		if(getLegend(i) == dataleg_){
			dataentry=i;
			gotdentry=true;
			if(gotof && gotuf) break;
		}
		if(containers_[i].getOverflow() < -0.9){
			gotof=true;
			if(gotdentry && gotuf) break;
		}
		if(containers_[i].getUnderflow() < -0.9){
			gotuf=true;
			if(gotdentry && gotof) break;
		}
	}
	float multiplier=1;
	double ylow,yhigh,xlow,xhigh;
	if(gPad){
		gPad->GetPadPar(xlow,ylow,xhigh,yhigh);
		multiplier = (float)1/(yhigh-ylow);
	}
	containers_[dataentry].setLabelSize(resizelabels * multiplier);
	TGraphAsymmErrors * g = addObject(containers_[dataentry].getTGraph(name));
	TH1D * h =addObject(containers_[dataentry].getTH1D(name+"_h")); // needed to be able to set log scale etc.

	float xmax=containers_[dataentry].getXMax();
	float xmin=containers_[dataentry].getXMin();
	h->Draw("AXIS");
	if(!drawxaxislabels){
		h->GetXaxis()->SetLabelSize(0);
	}
	else {
		if(gPad){
			gPad->SetLeftMargin(0.15);
			gPad->SetBottomMargin(0.15);
		}
	}
	g->Draw("P");
	makeTHStack(name)->Draw("same");
	makeMCErrors()->Draw("2");
	makeTLegend()->Draw("same");
	g->Draw("e1,P,same");
	if(gPad) gPad->RedrawAxis();

	if(gPad && drawxaxislabels && containers_[dataentry].getNBins() >0){
		float yrange=fabs(gPad->GetUymax()-gPad->GetUymin());
		float xrange = fabs(xmax-xmin);
		if(gotuf){
			TLatex * la = addObject(new TLatex(containers_[dataentry].getBinCenter(1)-xrange*0.06,gPad->GetUymin()-0.15*yrange,"#leq"));
			la->Draw("same");
		}
		if(gotof){
			TLatex * la2 = addObject(new TLatex(containers_[dataentry].getBinCenter(containers_[dataentry].getNBins())+xrange*0.06,gPad->GetUymin()-0.15*yrange,"#leq"));
			la2->Draw("same");
		}
	}

}

TGraphAsymmErrors * containerStack::makeMCErrors(){
	TGraphAsymmErrors * g =addObject(getFullMCContainer().getTGraph());
	g->SetFillStyle(3005);
	return g;
}

void containerStack::drawRatioPlot(TString name,double resizelabels){
	if(!checkDrawDimension()){
		std::cout << "containerStack::drawRatioPlot: only available for 1dim stacks!" <<std::endl;
		return;
	}
	if(debug)
		std::cout << "containerStack::drawRatioPlot: drawing..." <<std::endl;
	TH1::AddDirectory(false);
	//prepare container
	if(name=="") name=name_;
	int dataentry=0;
	bool gotdentry=false;
	bool gotuf=false;
	bool gotof=false;
	for(unsigned int i=0;i<size();i++){ // get datalegend and check if underflow or overflow in any container
		if(getLegend(i) == dataleg_){
			dataentry=i;
			gotdentry=true;
			if(gotof && gotuf) break;
		}
		if(containers_[i].getOverflow() < -0.9){
			gotof=true;
			if(gotdentry && gotuf) break;
		}
		if(containers_[i].getUnderflow() < -0.9){
			gotuf=true;
			if(gotdentry && gotof) break;
		}
	}
	container1D data = containers_[dataentry];
	data.setShowWarnings(false);
	container1D mc = getFullMCContainer();
	mc.setShowWarnings(false);
	//data.setDivideBinomial(false);
	//mc.setDivideBinomial(false);
	/*
	container1D ratio=data;
	ratio.clear();
	ratio.setShowWarnings(false);
	container1D relmcerrstat=data;
	relmcerrstat.clear();
	container1D relmcerrsys=relmcerrstat;
	container1D mccopy=mc;
	for(size_t i=0; i<ratio.getNBins()+1; i++){ //includes underflow right now, doesn't matter
		relmcerrstat.setBinContent(i,1.);
		double errfullup=mc.getBinErrorUp(i)/mc.getBinContent(i);
		double errfulldown=mc.getBinErrorDown(i) / mc.getBinContent(i);
		double errstatup=mc.getBinErrorUp(i,true)/mc.getBinContent(i);
		double errstatdown=mc.getBinErrorDown(i,true) / mc.getBinContent(i);
		double errsysup=sqrt(errfullup*errfullup - errstatup*errstatup);
		double errsysdown=sqrt(errfulldown*errfulldown - errstatdown*errstatdown);
		if(mc.getBinContent(i) ==0){
			errfullup=0;errfulldown=0;errsysup=0;errsysdown=0;
		}
		relmcerrstat.setBinErrorUp(i,errfullup);
		relmcerrstat.setBinErrorDown(i,errfulldown);
		relmcerrsys.setBinContent(i,1.);
		relmcerrsys.setBinErrorUp(i,errsysup);
		relmcerrsys.setBinErrorDown(i,errsysdown);
		//set mc error to zero for the ratio plot
		mccopy.setBinErrorUp(i,0);
		mccopy.setBinErrorDown(i,0);
	}
	bool temp=histoContent::divideStatCorrelated;
	histoContent::divideStatCorrelated =false;
	ratio = data / mccopy;
	histoContent::divideStatCorrelated =temp;
	 */
	container1D ratio=data;
	ratio.normalizeToContainer(mc);
	mc=mc.getRelErrorsContainer();

	float multiplier=1;
	double ylow,yhigh,xlow,xhigh;
	if(gPad){
		gPad->GetPadPar(xlow,ylow,xhigh,yhigh);
		multiplier = (float)1/(yhigh-ylow);
	}
	//   multiplier = multiplier * resizelabels;
	ratio.setLabelSize(multiplier * resizelabels);

	TGraphAsymmErrors * gratio = addObject(ratio.getTGraph(name,false,false,true)); //don't divide by binwidth, no x errors
	// rescale axis titles etc.
	TH1D * h = addObject(ratio.getTH1D(name+"_h_r",false)); //no bw div
	h->GetYaxis()->SetTitle("data/MC");
	h->GetYaxis()->SetRangeUser(0.5,1.5);
	h->GetYaxis()->SetNdivisions(505);
	h->GetXaxis()->SetTickLength(0.03 * multiplier);
	h->Draw("AXIS");
	gratio->Draw("P");/*
	TGraphAsymmErrors * gmcerr = relmcerrstat.getTGraph(name+"_relerrstat",false,false,false);
	TGraphAsymmErrors * gmcerrsys = relmcerrsys.getTGraph(name+"_relerrsys",false,false,false); */
	TGraphAsymmErrors * gmcerr = addObject(mc.getTGraph(name+"_relerrstat",false,false,false));
	TGraphAsymmErrors * gmcerrsys = addObject(mc.getTGraph(name+"_relerrsys",false,false,false,true));

	gmcerr->SetFillStyle(3005);
	gmcerrsys->SetFillStyle(1001);
	gmcerrsys->SetFillColor(kCyan-10);
	gmcerrsys->Draw("2,same");
	gmcerr->Draw("2,same");
	gratio->Draw("P,same");

	TLine * l = addObject(new TLine(mc.getXMin(),1,mc.getXMax(),1));
	l->Draw("same");
	float xmax=containers_[dataentry].getXMax();
	float xmin=containers_[dataentry].getXMin();
	if(gPad && containers_[dataentry].getNBins() >0){
		float yrange=fabs(gPad->GetUymax()-gPad->GetUymin());
		float xrange = fabs(xmax-xmin);
		if(gotuf){
			TLatex * la = addObject(new TLatex(containers_[dataentry].getBinCenter(1)-xrange*0.06,gPad->GetUymin()-0.15*yrange,"#leq"));
			la->SetTextSize(la->GetTextSize() * multiplier * resizelabels);
			la->Draw("same");
		}
		if(gotof){
			TLatex * la2 = addObject(new TLatex(containers_[dataentry].getBinCenter(containers_[dataentry].getNBins())+xrange*0.06,gPad->GetUymin()-0.15*yrange,"#leq"));
			la2->SetTextSize(la2->GetTextSize() * multiplier * resizelabels);
			la2->Draw("same");
		}
	}
}
/**
 * requires all norms=1
 * returns at 0: left to right
 *         at 1: right to left
 *   idx is index of signal. data is being ignored
 */
std::vector<container1D> containerStack::getPEffPlots(size_t idx)const{
	if(idx>=containers_.size()){
		std::cout << "containerStack::getPEffPlot: idx out of range, return empty" <<std::endl;
		return std::vector<container1D>();
	}
	container1D sigcont=containers_.at(0);
	sigcont.clear();
	container1D bgcont=sigcont;
	container1D peff=sigcont; //, ...
	//sig,bg
	for(size_t i=0;i<containers_.size();i++){
		if(i==idx)
			sigcont=containers_.at(i);
		else if(legends_.at(i)!=dataleg_)
			bgcont+=containers_.at(i);
	}
	//cont ready
	//make lhs
	size_t binssize=bgcont.getBins().size();
	double integralsig=0,integralbg=0;

	double fullsignintegral=sigcont.integral(true);
	//NOT PARALLEL! NEEDS TO BE SEQUENTIAL
	for(size_t i=0;i<binssize;i++){
		integralsig+=sigcont.getBinContent(i);
		integralbg+=bgcont.getBinContent(i);
		double content=integralsig/fullsignintegral * integralsig/(integralsig+integralbg);
		if(content!=content)content=0;
		peff.getBin(i).setContent(content); //eff * purity
	}
	// HERE WE ARE  peff.
	peff.setNames("p*eff","","P*#epsilon");
	std::vector<container1D> out;
	out.push_back(peff);

	integralsig=0;integralbg=0;
	for(size_t i=binssize;i>0;i--){
		integralsig+=sigcont.getBinContent(i-1);
		integralbg+=bgcont.getBinContent(i-1);
		double content=integralsig/fullsignintegral * integralsig/(integralsig+integralbg);
		if(content!=content)content=0;
		peff.getBin(i-1).setContent(content); //eff * purity
	}
	out.push_back(peff);

	return out; //default
}

void containerStack::drawPEffPlot(TString name,double resizelabels){
	if(signals_.size()<1){
		std::cout << "containerStack::drawPEffPlot: no signal set, doing nothing" <<std::endl;
		return;
	}
	if(containers_.size()<1){
		std::cout << "containerStack::drawPEffPlot: no 1dmin containers available. Doing nothing" <<std::endl;
		return;
	}
	if(debug)
		std::cout << "containerStack::drawPEffPlot: drawing..." <<std::endl;
	std::vector<size_t> sidx=getSignalIdxs();
	TH1::AddDirectory(false);
	std::vector<std::vector<container1D> > plotss;
	double ymax=0;
	for(size_t i=0;i<sidx.size();i++){
		std::vector<container1D> plots=getPEffPlots(sidx.at(i));
		for(size_t j=0;j<plots.size();j++)
			if(ymax<plots.at(j).getYMax(false)) ymax=plots.at(j).getYMax(false);
		plotss.push_back(plots);
	}
	for(size_t i=0;i<sidx.size();i++){
		std::vector<container1D> & plots=plotss.at(i);
		if(plots.size()>1){
			if(i==0){
				float multiplier=1;
				double ylow,yhigh,xlow,xhigh;
				if(gPad){
					gPad->GetPadPar(xlow,ylow,xhigh,yhigh);
					multiplier = (float)1/(yhigh-ylow);
				}
				plots.at(0).setLabelSize(resizelabels*multiplier);
			}
			TH1D * h1=addObject(plots.at(0).getTH1D("",false,true));
			TH1D * h2=addObject(plots.at(1).getTH1D("",false,true));
			h2->SetLineStyle(9);
			h1->SetLineColor(colors_.at(sidx.at(i)));
			h2->SetLineColor(colors_.at(sidx.at(i)));
			h1->SetLineWidth(2);
			h2->SetLineWidth(2);
			h1->SetMarkerStyle(1);
			h2->SetMarkerStyle(1);
			h1->SetOption("");
			h2->SetOption("");
			if(i==0){
				h1->GetYaxis()->SetNdivisions(505);
				h1->GetYaxis()->SetRangeUser(ymax*0.00000000001,ymax*1.05);
				h1->Draw();
			}
			else{
				h1->Draw("same");
			}
			h2->Draw("same");
		}
	}

}


void containerStack::drawSBGPlot(TString name,double resizelabels,bool invert){
	if(!checkDrawDimension()){
		std::cout << "containerStack::drawRatioPlot: only available for 1dim stacks!" <<std::endl;
		return;
	}
	if(debug)
		std::cout << "containerStack::drawSBGPlot: drawing..." <<std::endl;
	TH1::AddDirectory(false);
	//prepare container
	if(name=="") name=name_;
	//int dataentry=0;
	bool gotdentry=false;
	bool gotuf=false;
	bool gotof=false;
	for(unsigned int i=0;i<size();i++){ // get datalegend and check if underflow or overflow in any container
		if(getLegend(i) == dataleg_){
			//dataentry=i;
			gotdentry=true;
			if(gotof && gotuf) break;
		}
		if(containers_[i].getOverflow() < -0.9){
			gotof=true;
			if(gotdentry && gotuf) break;
		}
		if(containers_[i].getUnderflow() < -0.9){
			gotuf=true;
			if(gotdentry && gotof) break;
		}
	}
	//prepare container
	std::vector<container1D> vsgbg;
	double totmax=-999;
	for(size_t i=0;i<legends_.size();i++){
		if(legends_[i]==dataleg_) continue;
		container1D sig=containers_[i];
		container1D bg=sig;
		bg.clear();
		container1D sgbg=bg;
		//get all other contributions
		for(size_t j=0;j<legends_.size();j++){
			if(i==j) continue;
			if(legends_[j]==dataleg_) continue;
			bg += containers_[j];
		}
		sig.setAllErrorsZero();
		std::vector<float> bins=sig.getBins();
		double sinteg=0, bginteg=0;
		for(size_t j=0;j<bins.size();j++){
			size_t bin=j;
			if(invert) bin=bins.size()-1-j;
			sinteg  += sig.getBinContent(bin);
			bginteg += bg.getBinContent(bin);
			double snr=sinteg/(sinteg+bginteg);
			if(snr!=snr) snr=0; //nan protection
			if(totmax < snr) totmax=snr;
			sgbg.setBinContent(bin,snr);
		}
		sgbg.normalize();
		vsgbg.push_back(sgbg);
	}

	float multiplier=1;
	double ylow,yhigh,xlow,xhigh;
	if(gPad){
		gPad->GetPadPar(xlow,ylow,xhigh,yhigh);
		multiplier = (float)1/(yhigh-ylow);
	}
	bool firstdraw=true;
	for(size_t i=0;i<vsgbg.size();i++){
		//get TH1Ds
		container1D * c=&vsgbg[i];
		if(firstdraw)
			c->setLabelSize(multiplier * resizelabels);
		TH1D * h=addObject(c->getTH1D(""));
		h->GetYaxis()->SetRangeUser(0,totmax*1.05);
		h->SetLineColor(colors_[i]);
		h->SetLineWidth(2);
		if(firstdraw){
			if(invert)
				h->GetYaxis()->SetTitle("#frac{sg}{sg+bg}#leftarrow");
			else
				h->GetYaxis()->SetTitle("#frac{sg}{sg+bg}#rightarrow");
			h->Draw();
			firstdraw=false;
		}
		else{
			h->Draw("same");
		}
	}
}
/*
TCanvas * containerStack::makeTCanvas(bool drawratioplot){
	if(drawratioplot) return makeTCanvas(ratio);
	else return makeTCanvas(normal);
}
 */
TCanvas * containerStack::makeTCanvas(plotmode plotMode){
	if(!checkDrawDimension()){
		std::cout << "containerStack::makeTCanvas: only available for 1dim stacks!" <<std::endl;
		return 0;
	}
	TH1::AddDirectory(false);
	TString canvasname=name_+"_c";
	if(mode==unfolddim1)
		canvasname = canvasname + "_uf";
	TCanvas * c = new TCanvas(canvasname,canvasname);
	if(containerStack::batchmode)
		c->SetBatch();
	TH1D * htemp=(new TH1D("sometemphisto"+name_,"sometemphisto"+name_,2,0,1)); //creates all gPad stuff etc and prevents seg vio, which happens in some cases; weird
	htemp->Draw();
	if(debug)
		std::cout << "containerStack::makeTCanvas: prepared canvas "<< canvasname << ", drawing plots" <<std::endl;
	gStyle->SetOptTitle(0);
	if(plotMode == normal){
		c->cd();
		drawControlPlot();
	}
	else{
		double labelresize=1.;
		double divideat=0.3;
		c->Divide(1,2);
		c->SetFrameBorderSize(0);
		c->cd(1)->SetLeftMargin(0.15);
		c->cd(1)->SetBottomMargin(0.03);
		c->cd(1)->SetPad(0,divideat,1,1);
		drawControlPlot(name_,false,labelresize);
		c->cd(2)->SetBottomMargin(0.4 * 0.3/divideat);
		c->cd(2)->SetLeftMargin(0.15);
		c->cd(2)->SetTopMargin(0);
		c->cd(2)->SetPad(0,0,1,divideat);
		if(plotMode == ratio){
			drawRatioPlot("",labelresize);
			c->cd(2)->RedrawAxis();
		}
		else if(plotMode == sigbg)
			drawPEffPlot("",labelresize);
		c->cd(1)->RedrawAxis();
	}
	/*else{
		c->Divide(1,3);
		c->SetFrameBorderSize(0);
		c->cd(1)->SetLeftMargin(0.15);
		c->cd(1)->SetBottomMargin(0.0);
		c->cd(2)->SetLeftMargin(0.15);
		c->cd(2)->SetBottomMargin(0.0);
		c->cd(3)->SetLeftMargin(0.15);
		c->cd(3)->SetBottomMargin(0.4);
		drawControlPlot(name_,false,1);
		c->cd(2);
		drawSBGPlot("",1,false);
		c->cd(3);
		drawSBGPlot("",1,true);
		c->cd(1)->RedrawAxis();
	}*/
	if(gStyle){
		gStyle->SetOptTitle(0);
		gStyle->SetOptStat(0);
	}
	delete htemp;
	return c;
}

std::map<TString, histoBin> containerStack::getAllContentsInBin(size_t bin,int syst,bool print)const{


	if(is1D()){

		if(containers_.size() < 1 || bin >= containers_.at(0).getBins().size()){
			throw std::out_of_range("containerStack::getAllContentsInBin: bin out of range (getNBins()) or no 1D content");
		}
		if(syst>=(int)containers_.at(0).getSystSize()){
			throw std::out_of_range("containerStack::getAllContentsInBin: syst index out of range");
		}
		std::map<TString, histoBin>  out;
		if(print) std::cout << "Content of bin " << bin << " for containerStack " << name_ << std::endl;
		for(size_t i=0;i<legends_.size();i++){
			if(legends_.at(i)==dataleg_) continue;
			if(print) std::cout <<legends_.at(i) << ": " << containers_.at(i).getBinContent(bin) << std::endl;
			out[legends_.at(i)] = containers_.at(i).getBin(bin,syst);
		}

		out["total MC"]=getFullMCContainer().getBin(bin,syst);
		out[dataleg_]=containers_.at(getDataIdx()).getBin(bin,syst);


		return out;
	}
	else if(is1DUnfold()){
		if(containers1DUnfold_.size() < 1 || bin >= containers1DUnfold_.at(0).getRecoContainer().getBins().size()){
			throw std::out_of_range("containerStack::coutAllInBin: bin out of range (getNBins()) or no 1D content");
		}
		if(syst>=(int)containers1DUnfold_.at(0).getSystSize()){
			throw std::out_of_range("containerStack::coutAllInBin: syst index out of range");
		}
		std::map<TString, histoBin>  out;
		if(print) std::cout << "Content of bin " << bin << " for containerStack " << name_ << std::endl;
		for(size_t i=0;i<legends_.size();i++){
			if(legends_.at(i)==dataleg_) continue;
			if(print) std::cout <<legends_.at(i) << ": " << containers1DUnfold_.at(i).getRecoContainer().getBinContent(bin) << std::endl;
			out[legends_.at(i)] = containers1DUnfold_.at(i).getRecoContainer().getBin(bin,syst);
		}

		out["total MC"]=getFullMCContainer1DUnfold().getRecoContainer().getBin(bin,syst);
		out[dataleg_]=containers1DUnfold_.at(getDataIdx()).getRecoContainer().getBin(bin,syst);

		return out;

	}


	throw std::runtime_error("containerStack::getAllContentsInBin: only available for 1dim stacks");
}


containerStack containerStack::rebinXToBinning(const std::vector<float> & newbins)const{

	containerStack out=*this;
	for(size_t i=0;i<containers_.size();i++)
		out.containers_.at(i)=containers_.at(i).rebinToBinning(newbins);

	for(size_t i=0;i<containers2D_.size();i++)
		out.containers2D_.at(i)=containers2D_.at(i).rebinXToBinning(newbins);

	for(size_t i=0;i<containers1DUnfold_.size();i++)
		out.containers1DUnfold_.at(i)=containers1DUnfold_.at(i).rebinToBinning(newbins);

	return out;
}


containerStack containerStack::rebinYToBinning(const std::vector<float> & newbins)const{
	containerStack out=*this;
	for(size_t i=0;i<containers2D_.size();i++)
		out.containers2D_.at(i)=containers2D_.at(i).rebinYToBinning(newbins);
	return out;
}


//just perform functions on the containers with same names
ztop::containerStack containerStack::operator + (containerStack stack){
	if(mode!=stack.mode){
		std::cout << "containerStack::operator +: stacks must be of same dimension, returning input" << std::endl;
		return *this;
	}
	for(size_t j=0;j<stack.legends_.size();j++){
		for(unsigned int i=0;i<legends_.size();i++){
			if(legends_[i] == stack.legends_.at(j)){
				if(debug)
					std::cout << "containerStack::addRelSystematicsFrom: adding to stack " << name_ << std::endl;
				if(i<containers_.size() && j<stack.containers_.size())
					stack.containers_[j]+=containers_.at(i);
				if(i<containers2D_.size() && j<stack.containers2D_.size())
					stack.containers2D_[j]+=containers2D_.at(i);
				if(i<containers1DUnfold_.size() && j<stack.containers1DUnfold_.size())
					stack.containers1DUnfold_[j]+=containers1DUnfold_.at(i);

				break;
			}
		}
	}

	return stack;
}
ztop::containerStack containerStack::operator - (containerStack stack){
	if(mode!=stack.mode){
		std::cout << "containerStack::operator -: stacks must be of same dimension, returning input" << std::endl;
		return *this;
	}
	for(size_t j=0;j<stack.legends_.size();j++){
		for(unsigned int i=0;i<legends_.size();i++){
			if(legends_[i] == stack.legends_.at(j)){
				if(debug)
					std::cout << "containerStack::addRelSystematicsFrom: adding to stack " << name_ << std::endl;
				if(i<containers_.size() && j<stack.containers_.size())
					stack.containers_[j]-=containers_.at(i);
				if(i<containers2D_.size() && j<stack.containers2D_.size())
					stack.containers2D_[j]-=containers2D_.at(i);
				if(i<containers1DUnfold_.size() && j<stack.containers1DUnfold_.size())
					stack.containers1DUnfold_[j]-=containers1DUnfold_.at(i);

				break;
			}
		}
	}

	return stack;
}
ztop::containerStack containerStack::operator / (containerStack  stack){
	if(mode!=stack.mode){
		std::cout << "containerStack::operator /: stacks must be of same dimension, returning input" << std::endl;
		return stack;
	}
	for(size_t j=0;j<stack.legends_.size();j++){
		for(unsigned int i=0;i<legends_.size();i++){
			if(legends_[i] == stack.legends_.at(j)){
				if(debug)
					std::cout << "containerStack::addRelSystematicsFrom: adding to stack " << name_ << std::endl;
				if(i<containers_.size() && j<stack.containers_.size())
					stack.containers_[j]/=containers_.at(i);
				if(i<containers2D_.size() && j<stack.containers2D_.size())
					stack.containers2D_[j]/=containers2D_.at(i);
				if(i<containers1DUnfold_.size() && j<stack.containers1DUnfold_.size())
					stack.containers1DUnfold_[j]/=containers1DUnfold_.at(i);

				break;
			}
		}
	}
	return stack;
}
ztop::containerStack containerStack::operator * (containerStack  stack){
	if(mode!=stack.mode){
		std::cout << "containerStack::operator /: stacks must be of same dimension, returning input" << std::endl;
		return *this;
	}
	for(size_t j=0;j<stack.legends_.size();j++){
		for(unsigned int i=0;i<legends_.size();i++){
			if(legends_[i] == stack.legends_.at(j)){
				if(debug)
					std::cout << "containerStack::addRelSystematicsFrom: adding to stack " << name_ << std::endl;
				if(i<containers_.size() && j<stack.containers_.size())
					stack.containers_[j]*=containers_.at(i);
				if(i<containers2D_.size() && j<stack.containers2D_.size())
					stack.containers2D_[j]*=containers2D_.at(i);
				if(i<containers1DUnfold_.size() && j<stack.containers1DUnfold_.size())
					stack.containers1DUnfold_[j]*=containers1DUnfold_.at(i);

				break;
			}
		}
	}
	return stack;
}
ztop::containerStack containerStack::operator * (double scalar){
	ztop::containerStack out=*this;
	for(unsigned int i=0;i<containers_.size();i++) out.containers_[i] *=  scalar;
	for(unsigned int i=0;i<containers2D_.size();i++) out.containers2D_[i] *=  scalar;
	for(unsigned int i=0;i<containers1DUnfold_.size();i++) out.containers1DUnfold_[i] *= scalar;
	return out;
}
ztop::containerStack containerStack::operator * (float scalar){
	return *this * (double)scalar;
}
ztop::containerStack containerStack::operator * (int scalar){
	return *this * (double)scalar;
}


containerStack containerStack::append(const containerStack& rhs)const{

	if(rhs.containers_.size() <1){
		throw std::logic_error("containerStack::append: only works for 1D stacks");
	}
	if(containers_.size()<1) //empty stack
		return rhs;

	containerStack out=*this;
	std::vector<size_t> rhsused;

	for(size_t leg=0;leg<out.legends_.size();leg++){
		size_t rhspos=std::find(rhs.legends_.begin(), rhs.legends_.end(), out.legends_.at(leg)) - rhs.legends_.begin();
		container1D temp;
		if(rhspos < rhs.legends_.size()){ //found same entry
			temp=rhs.containers_.at(rhspos);
			temp*=rhs.norms_.at(rhspos);
			rhsused.push_back(leg);
		}
		else{
			temp=rhs.containers_.at(0);
			temp.setAllErrorsZero();
		}
		out.containers_.at(leg) *= out.norms_.at(leg);
		out.containers_.at(leg) = out.containers_.at(leg).append(temp);
	}

	for(size_t leg=0;leg<rhs.legends_.size();leg++){
		if(std::find(rhsused.begin(), rhsused.end(),leg) != rhsused.end()) continue;
		container1D temp=out.containers_.at(0);
		temp.setAllZero();
		temp=temp.append(rhs.containers_.at(leg));
		out.push_back(temp,rhs.legends_.at(leg),rhs.colors_.at(leg),rhs.norms_.at(leg),rhs.legorder_.at(leg));

	}

	return out;
}


void containerStack::equalizeSystematicsIdxs(containerStack& rhs){

	//no need to look at legends, syst are the same in stack by construction
	if(rhs.size() < 1 || size() <1)
		throw std::logic_error("containerStack::equalizeSystematicsIdxs: one is empty");

	for(size_t i=0;i<containers_.size();i++){
		containers_.at(i).equalizeSystematicsIdxs(rhs.containers_.at(0));
	}
	// after this->containers_.at(0) now has a superset of syst,
	// it will drive the new ordering of rhs
	for(size_t i=0;i<rhs.containers_.size();i++){
		rhs.containers_.at(i).equalizeSystematicsIdxs(containers_.at(0));
	}
	if(!is1D()) //FIXME
		throw std::runtime_error("containerStack::equalizeSystematicsIdxs: only implemented for 1D stacks (FIXME)");
}

bool containerStack::setsignal(const TString& sign){
	for(size_t i=0;i<legends_.size();i++){
		if(sign == legends_[i]){
			signals_.clear();
			signals_.push_back(sign);
			return true;
		}
	}
	std::cout << "containerStack::setsignal: signalname " << sign << " not found in legend entries. doing nothing" <<std::endl;
	return false;
}
bool containerStack::addSignal(const TString& sign){
	if(std::find(signals_.begin(),signals_.end(),sign)!=signals_.end())
		return true; //already in
	for(size_t i=0;i<legends_.size();i++){
		if(sign == legends_[i]){
			signals_.push_back(sign);
			return true;
		}
	}
	std::cout << "containerStack::setsignal: signalname " << sign << " not found in legend entries. doing nothing" <<std::endl;
	return false;
}
bool containerStack::setsignals(const std::vector<TString> & signs){
	if(!checkSignals(signs)){
		std::cout << "containerStack::setsignals: not all signals found in legend entries" <<std::endl;
		return false;
	}
	signals_=signs;
	return true;
}
std::vector<size_t> containerStack::getSignalIdxs() const{
	std::vector<size_t> out;
	for(size_t i=0;i<signals_.size();i++){
		out.push_back((size_t)(std::find(legends_.begin(),legends_.end(),signals_.at(i)) - legends_.begin()));
	}
	return out;
}

size_t containerStack::getDataIdx()const{

	for(size_t i=0;i<legends_.size();i++){
		if(dataleg_ == legends_.at(i)){
			return i;
		}
	}
	throw std::runtime_error("containerStack::getDataIdx: no data index!");
}


float containerStack::getYMax(bool dividebybinwidth)const{
	float max=-99999;
	if(mode == dim1){
		if(containers_.size()<1) return 0;
		for(size_t i=0;i<containers_.size();i++){
			float temp=containers_.at(i).getYMax(dividebybinwidth);
			if(max<temp)max=temp;
		}
	}
	else if(mode == dim2){
		return 0;
	}
	else if(mode == unfolddim1){
		if(containers1DUnfold_.size()<1) return 0;
		for(size_t i=0;i<containers1DUnfold_.size();i++){
			float temp=containers1DUnfold_.at(i).getRecoContainer().getYMax(dividebybinwidth);
			if(max<temp)max=temp;
		}
	}
	return max;
}

container1D containerStack::getSignalContainer()const{
	if(mode != dim1 && mode !=unfolddim1){
		if(debug) std::cout << "containerStack::getSignalContainer: return dummy" <<std::endl;
		return container1D();
	}
	std::vector<size_t> sidx=getSignalIdxs();
	container1D out;
	if(containers_.size()>0){
		out=containers_.at(0);
		out.clear();
	}
	if(sidx.size()>0) out=getContainer(sidx.at(0));
	for(size_t i=1;i<sidx.size();i++)
		out+=getContainer(sidx.at(i));
	return out;
}
container1D containerStack::getBackgroundContainer()const{
	if(mode != dim1 && mode !=unfolddim1){
		if(debug) std::cout << "containerStack::getBackgroundContainer: return dummy" <<std::endl;
		return container1D();
	}
	std::vector<size_t> sidx=getSignalIdxs();
	container1D out;
	if(containers_.size()>0){
		out=containers_.at(0);
		out.clear();
	}
	bool first=true;
	for(size_t i=0;i<containers_.size();i++){
		if(std::find(sidx.begin(),sidx.end(),i) != sidx.end() || getLegend(i) == dataleg_){
			continue;
		}
		if(first){
			out=containers_.at(i);
			first=false;
		}
		else{
			out+=containers_.at(i);
		}
	}

	return out;

}
container1D containerStack::getDataContainer()const{
	return getContainer(getDataIdx());
}
container2D containerStack::getSignalContainer2D()const{
	if(mode != dim2){
		if(debug) std::cout << "containerStack::getSignalContainer2D: return dummy" <<std::endl;
		return container2D();
	}
	std::vector<size_t> sidx=getSignalIdxs();
	container2D out;
	if(containers2D_.size()>0){
		out=containers2D_.at(0);
		out.clear();
	}
	if(sidx.size()>0) out=getContainer2D(sidx.at(0));
	for(size_t i=1;i<sidx.size();i++)
		out+=getContainer2D(sidx.at(i));
	return out;
}

container2D containerStack::getBackgroundContainer2D()const{
	if(mode != dim2){
		if(debug) std::cout << "containerStack::getBackgroundContainer2D: return dummy" <<std::endl;
		return container2D();
	}
	std::vector<size_t> sidx=getSignalIdxs();
	container2D out;
	if(containers2D_.size()>0){
		out=containers2D_.at(0);
		out.clear();
	}
	bool first=true;
	for(size_t i=0;i<containers2D_.size();i++){
		if(std::find(sidx.begin(),sidx.end(),i) != sidx.end() || getLegend(i) == dataleg_){
			continue;
		}
		if(first){
			out=containers2D_.at(i);
			first=false;
		}
		else{
			out+=containers2D_.at(i);
		}
	}

	return out;

}

container1DUnfold containerStack::produceUnfoldingContainer(const std::vector<TString> &sign) const{// if sign empty use "setsignal"
	/*
   ad all signal -> cuf
   add all recocont_ w/o signaldef -> set BG of cuf
   (add all) with datadef -> setData in cuf
	 */


	container1DUnfold out;
	if(mode != unfolddim1 || containers1DUnfold_.size()<1){
		std::cout << "containerStack::produceUnfoldingContainer: No unfolding information, return empty" <<std::endl;
		return out;
	}
	if(debug)
		checknorms();
	bool temp=container1DUnfold::c_makelist;
	container1DUnfold::c_makelist=false;
	bool temp2=container1D::c_makelist;
	container1D::c_makelist=false;
	bool temp3=histoContent::addStatCorrelated;
	histoContent::addStatCorrelated=false;

	std::vector<TString> signals=signals_;

	if(sign.size()>0)
		signals=sign;
	out=containers1DUnfold_.at(0);
	out.clear();
	container1D background=containers1DUnfold_.at(0).getRecoContainer();
	background.clear();
	container1D data=background;

	//find signal

	for(size_t leg=0;leg<legends_.size();leg++){
		bool issignal=false;
		const container1DUnfold * cuf=&containers1DUnfold_.at(leg);
		for(size_t sig=0;sig<signals.size();sig++){
			if(signals.at(sig) == legends_.at(leg)){
				if(debug)
					std::cout << "containerStack::produceUnfoldingContainer: identified " << legends_.at(leg) << " as signal -> add to signal, norm: "<< norms_[leg] <<std::endl;
				container1DUnfold normcuf= *cuf;
				normcuf = normcuf*norms_[leg];
				out = out + normcuf;
				issignal=true;
				break;
			}
		}
		if(!issignal && legends_.at(leg) != dataleg_){
			if(debug)
				std::cout << "containerStack::produceUnfoldingContainer: identified as BG: "<< legends_.at(leg) <<  ", norm: "<< norms_[leg] << std::endl;
			container1D bg=cuf->getRecoContainer();
			background = background + bg*norms_[leg];
		}
		if(legends_.at(leg) == dataleg_){
			if(debug)
				std::cout << "containerStack::produceUnfoldingContainer: identified as data: "<< legends_.at(leg) << std::endl;
			container1D d=cuf->getRecoContainer();
			data = data + d*norms_[leg];
		}
	}
	//migration matrix, eff, background prepared -> finished
	//background needs to be ADDED!!!
	out.setBackground(out.getBackground()+background);
	out.setRecoContainer(data);
	container1DUnfold::c_makelist=temp;
	container1D::c_makelist=temp2;
	histoContent::addStatCorrelated=temp3;

	return out;
}
container1DUnfold containerStack::produceXCheckUnfoldingContainer(const std::vector<TString> & sign) const{ /* if sign empty use "setsignal"
   just adds all with signaldef (to be then unfolded and a comparison plot to be made gen vs unfolded gen) -> SWITCH OFF SYSTEMATICS HERE! (should be independent)

 */
	//define operators (propagate from containers) for easy handling
	// also define += etc. and use container += if better, needs to be implemented (later)
	container1DUnfold out;
	if(mode != unfolddim1 || containers1DUnfold_.size()<1){
		std::cout << "containerStack::produceXCheckUnfoldingContainer: No unfolding information, return empty" <<std::endl;
		return out;
	}
	if(debug)
		checknorms();

	bool temp=container1DUnfold::c_makelist;
	container1DUnfold::c_makelist=false;
	bool temp2=container1D::c_makelist;
	container1D::c_makelist=false;
	bool temp3=histoContent::addStatCorrelated;
	histoContent::addStatCorrelated=false;

	std::vector<TString> signals=signals_;
	if(sign.size()>0)
		signals=sign;
	out=containers1DUnfold_.at(0);
	out.clear();
	for(size_t leg=0;leg<legends_.size();leg++){
		const container1DUnfold * cuf=&containers1DUnfold_.at(leg);
		for(size_t sig=0;sig<signals.size();sig++){
			if(signals.at(sig) == legends_.at(leg)){ //so damn not fast
				container1DUnfold normcuf=*cuf;
				normcuf = normcuf*norms_[leg];
				out = out + normcuf;
				break;
			}
		}
	}
	container1DUnfold::c_makelist=temp;
	container1D::c_makelist=temp2;
	histoContent::addStatCorrelated=temp3;
	return out;
}

bool containerStack::checknorms() const{
	bool out=true;
	for(size_t i=0;i<norms_.size();i++){
		if(norms_[i] != 1){
			if(debug){
				std::cout << "containerStack::checknorms: Norm for " << legends_[i] << " is not 1; " << norms_[i] << std::endl;
				out=false;
			}
			out=false;
			throw std::logic_error("containerStack: obsolete norms_ vector is supposed to have only 1 as entries");
		}
	}
	return out;
}


container1DUnfold containerStack::getSignalContainer1DUnfold(const std::vector<TString> &sign) const{
	bool mklist=container1DUnfold::c_makelist;
	container1DUnfold::c_makelist=false;
	if(mode!=unfolddim1){
		std::cout << "containerStack::getSignalContainer1DUnfold: bo container1DUnfold content, return empty" <<std::endl;
		container1DUnfold c;
		container1DUnfold::c_makelist=mklist;
		return c;
	}
	std::vector<TString> signals=sign;
	if(signals.size()<1)
		signals=signals_;
	container1DUnfold out;
	if(containers1DUnfold_.size()>0)
		out=containers1DUnfold_.at(0);
	out.clear();
	for(size_t i=0;i<signals.size();i++){
		container1DUnfold ctemp=getContribution1DUnfold(signals.at(i));
		out = out + ctemp;
	}

	container1DUnfold::c_makelist=mklist;
	return out;
}
TString containerStack::listContributions()const{
	TString out;
	out+=getName()+" contributions:\n";

	for(size_t i=0;i<legends_.size();i++){
		out += legends_.at(i)+"\n";
	}
	return out;
}


/**
 * calls TH1::Chi2Test on full MC histo and data hist
 */
double containerStack::chi2Test(Option_t* option, Double_t* res) const{
	if(!(is1D() || is1DUnfold()))
		return 0;

	TH1D * data = getContainer(getDataIdx()).getTH1D("",false);
	TH1D * mc= getFullMCContainer().getTH1D("",false);

	double out=data->Chi2Test(mc,option,res);
	delete data;
	delete mc;
	return out;
}
double containerStack::chi2(size_t * ndof)const{
	if(!(is1D() || is1DUnfold()))
		return 0;
	//double out;
	container1D data=getDataContainer();
	container1D mc=getFullMCContainer();
	return data.chi2(mc,ndof);
}

/**
 * calls TH1::KolmogorovTest on full MC histo and data hist
 */
double containerStack::kolmogorovTest(Option_t* option) const{
	if(!(is1D() || is1DUnfold()))
		return 0;

	TH1D * data = getContainer(getDataIdx()).getTH1D();
	TH1D * mc= getFullMCContainer().getTH1D();

	double out=data->KolmogorovTest(mc,option);
	delete data;
	delete mc;
	return out;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////private member functions////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * rewrite: always true
 */
int containerStack::checkLegOrder() const{ //depre
	return 0;
	/*
	for(std::map<TString,size_t>::const_iterator it=legorder_.begin();it!=legorder_.end();++it){
		if(it->second > size()){
			std::cout << "containerStack::checkLegOrder: "<< name_ << ": legend ordering numbers (" <<it->second << ") need to be <= size of stack! ("<< size()<<")" <<std::endl;
			return -1;
		}
	}
	return 0; */
}

std::vector<size_t> containerStack::getSortedIdxs(bool inverse) const{

	//new implementation
	std::vector<int> inputlegord=legorder_;
	std::vector<int> sortedilo=inputlegord;
	if(inverse)
		std::sort(sortedilo.begin(),sortedilo.end(),std::greater<int>());
	else
		std::sort(sortedilo.begin(),sortedilo.end());

	std::vector<size_t> legasso;
	//protect size etc

	for(size_t i=0;i<sortedilo.size();i++){
		//get the position in input
		size_t pos=std::find(inputlegord.begin(),inputlegord.end(),sortedilo.at(i))-inputlegord.begin();
		legasso.push_back(pos);
	}
	//just prepend all others
	for(size_t i=legasso.size();i<legends_.size();i++)
		legasso.push_back(i);

	if(debug){
		std::cout << "containerStack::sortEntries: ";
		for(size_t i=0;i<legasso.size();i++)
			std::cout << legasso.at(i) << " " << legends_.at(i) << " ";
		std::cout<<std::endl;
	}

	return legasso;

}

bool containerStack::checkDrawDimension() const{
	return (mode==dim1 || mode==unfolddim1);
}

bool containerStack::checkSignals(const std::vector<TString> &signs) const{
	size_t count=0;
	for(size_t j=0;j< signs.size();j++){
		bool succ=false;
		const TString & sign=signs.at(j);
		for(size_t i=0;i<legends_.size();i++){
			if(sign == legends_[i]){
				succ=true;
				break;
			}
		}
		if(succ)
			count++;
	}
	if(count != signs.size()){
		return false;
	}
	return true;
}

}//namespace
