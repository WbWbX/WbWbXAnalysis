/*
 * container2D.cc
 *
 *  Created on: Jun 24, 2013
 *      Author: kiesej
 */



#include "../interface/container2D.h"

namespace ztop{

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
		conts_.push_back(temp);
	}

	if(container1D::debug){
		std::cout << "containers size: " << conts_.size() << " ybins size: " << ybins_.size() << std::endl;
	}
}



}//namespace
