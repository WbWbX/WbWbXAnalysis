/*
 * variateContainer1D.cc
 *
 *  Created on: Aug 19, 2014
 *      Author: kiesej
 */

#include "../interface/variateContainer1D.h"

namespace ztop{

/*variateContainer1D(){}
	~variateContainer1D(){}

	void import(const container1D&);

	//exports for a certain set of variations
	container1D exportContainer(const std::vector<float> & variations);

private:
	std::vector<extendedVariable> contents_;
	std::vector<float> bins_;
 *
 */

bool variateContainer1D::debug=false;

void variateContainer1D::import(const container1D& cont){
	contents_.clear();errsup_.clear(),errsdown_.clear();
	contents_.resize(cont.getBins().size());
	errsup_.resize(cont.getBins().size());
	errsdown_.resize(cont.getBins().size());
	bins_=cont.getBins();
	std::vector<TString> variations=cont.getSystNameList();

	for(size_t sys=0;sys<variations.size();sys++){
		const TString& sysname=variations.at(sys);
		size_t idxdown=cont.getSystErrorIndex(sysname+"_down");
		size_t idxup=cont.getSystErrorIndex(sysname+"_up");
		for(size_t bin=0;bin<bins_.size();bin++){
			const float & content=cont.getBinContent(bin);
			const float & sysup=cont.getBinContent(bin,idxup);
			const float & sysdown=cont.getBinContent(bin,idxdown);

			contents_.at(bin).addDependence(sysdown,content ,sysup ,sysname);
			if(debug)
				std::cout << "variateContainer1D::import added: " << sysdown << " " << content << " " << sysup << " "<< sysname << std::endl;

		}
	}
	if(variations.size() == 0){
		for(size_t bin=0;bin<bins_.size();bin++){
			const float & content=cont.getBinContent(bin);
			contents_.at(bin).addDependence(content,content ,content ,"dummy");
		}
	}
	//only stat!
	for(size_t bin=0;bin<bins_.size();bin++){
		if(debug){
			std::cout << "variateContainer1D::import adding error: "<<bin << " " <<  cont.getBinErrorUp(bin,true) <<std::endl;
		}
		errsup_.at(bin) = cont.getBinErrorUp(bin,true);
		errsdown_.at(bin) = cont.getBinErrorDown(bin,true);
	}



}


void variateContainer1D::setBinErrUp(size_t bin,const float& err){
	if(bin<=bins_.size())
		throw std::out_of_range("variateContainer1D::setBinErrUp: bin out of range");
	errsup_.at(bin)=err;
}
void variateContainer1D::setBinErrDown(size_t bin,const float& err){
	if(bin<=bins_.size())
		throw std::out_of_range("variateContainer1D::setBinErrDown: bin out of range");
	errsdown_.at(bin)=err;
}


container1D variateContainer1D::exportContainer(const std::vector<float> & variations){

	if(contents_.size()<1){
		throw std::out_of_range("variateContainer1D::exportContainer: cannot export without input!");
	}
	std::vector<float> bins=bins_;
	bins.erase(bins.begin()); //let UF be created for output
	container1D out(bins);
	container1D zerotmp=out;

	for(size_t i=0;i<bins_.size();i++){
		out.setBinContent(i,contents_.at(i).getValue(variations)  );
		out.setBinErrorUp(i,errsup_.at(i));
		out.setBinErrorDown(i,errsdown_.at(i));
	}


	return out;
}


}


