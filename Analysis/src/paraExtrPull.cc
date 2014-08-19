/*
 * paraExtrPull.cc
 *
 *  Created on: Aug 15, 2014
 *      Author: kiesej
 */


#include "../interface/paraExtrPull.h"
#include <stdexcept>
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "TtZAnalysis/Tools/interface/graphFitter.h"
#include "TCanvas.h"
namespace ztop{

void paraExtrPull::fill(){
	if(!ex_ || ! cont_ || !random_)
		throw std::logic_error("paraExtrPull::fill: first set configure extractor,random gen and container to be filled");

	//std::cout << std::endl;



	//create functional forms
	if(trueinput_.size()>0){

		if(trueinput_.size() != datatrueinput_.size()){
			throw std::out_of_range("paraExtrPull::fill: datatrueinput_ and trueinput_ must be same size");

		}

		fits_.clear();
		mcscale_.clear();
		datascale_.clear();

		//	std::cout << "paraExtrPull::fill: reading " << trueinput_.size() << " input graphs " <<std::endl;

		for(size_t gidx=0;gidx<trueinput_.size();gidx++){

			//std::cout << "input graph " << gidx<< " has " << trueinput_.at(gidx).getNPoints() << " points " <<std::endl;

			if(trueinput_.at(gidx).getNPoints()>1){

				///very specifc......
				if(trueinput_.at(gidx).getNPoints()>3){
					float contentmc=trueinput_.at(gidx).getPointYContent(4);
					float statmc=trueinput_.at(gidx).getPointYStat(4);
					float scalemc=statmc*statmc/contentmc;
					mcscale_.push_back(scalemc);
					std::cout << "automatically setting MC scale "<< gidx <<" to "<<scalemc << std::endl;
				}
				else{
					mcscale_.push_back(1);
				}

				if(datatrueinput_.at(gidx).getNPoints()>1){
					//just get the scaling
					float content=datatrueinput_.at(gidx).getPointYContent(4);
					float stat=datatrueinput_.at(gidx).getPointYStat(4);
					float scale=stat*stat/content;
					datascale_.push_back(scale);
					std::cout << "automatically setting data scale "<< gidx <<" to "<<scale << std::endl;
				}
				else{
					datascale_.push_back(1);
				}
			}

		}

	}
	failcount_=0;
	std::cout << "paraExtrPull::fill: ready" <<std::endl;


	size_t nNanchecks_=0;
	ex_->setSelfHeal(true);

	ex_->setAllowExternalScaleA(true);

	for(size_t i=0;i<niter_+nNanchecks_;i++){

		//displayStatusBar(i,niter_);
		std::vector<graph> dataintoex;
		std::vector<graph> mcintoex;


		for(size_t bin=0;bin<datascale_.size();bin++){

			graph mc=  generateMCPoints(bin);
			mcintoex.push_back(mc);
			graph data=generateDataPoints(bin);
			dataintoex.push_back(data);

		}
		ex_->setInputA(dataintoex);
		ex_->setInputB(mcintoex);

		ex_->setFallBackScaleA(datascale_.at(0));
		ex_->setFallBackScaleB(mcscale_.at(0));


		graph lho;
		float statlow=0,stathi=0;
		double min=ex_->getLikelihoodMinimum(0,&statlow,&stathi,true,&lho);
		ex_->clear();

		//if(i<nNanchecks_) continue;


		float pull = min-evalpoint_;
		//std::cout << "min: "<< min <<  " pull: " <<pull << " statlow: " << statlow << " stathi " <<stathi <<std::endl;
		if(statlow<0 || stathi<0){

			std::cout << "WARNING!!!! Extraction failed! WILL IGNORE PSEUDOEXP! - try to increase number of nan checks!" <<std::endl;

			/*	TCanvas c;
			ex_->getFittedGraphsA().at(0).getTGraph()->Draw("AP");
			c.Print("temp.pdf");
			ex_->getFittedGraphsB().at(0).getTGraph()->Draw("AP");
			c.Print("tempmc.pdf");
			lho.getTGraph()->Draw("AP");
			c.Print("tempLH.pdf");



			throw std::runtime_error("paraExtrPull::fill: stat estimation failed");
*/


			failcount_++;
		}
		else{
			if(pull < 0)
				pull/=(double)statlow;
			else
				pull/=(double)stathi;

			cont_->fill(pull);

		}

	}
	std::cout << std::endl;
	std::cout << " Failed pseudoExp: " <<failcount_ << " out of " << niter_ << std::endl;
	std::cout << "Excluded bins: " ;
	for(size_t i=0;i<ex_->getNanIdxs().size();i++){
		std::cout << ex_->getNanIdxs().at(i) << " " ;

	}
	std::cout << std::endl;
}

float paraExtrPull::getFunctionout(float xval,size_t idx)const{
	if(idx>=externalfitparas_.size()){
		throw std::out_of_range("paraExtrPull::getFunctionout: index out of range");
	}
	//float out=fits_.at(idx).getFitOutput(xval);
	double off=externalfitparas_.at(idx).at(0);
	double a=externalfitparas_.at(idx).at(1);
	double b=externalfitparas_.at(idx).at(2);
	double xshift=externalfitparas_.at(idx).at(3);
	double yshift=externalfitparas_.at(idx).at(4);
	double x=(xval-xshift);


	float out= off + x*a + x*x*b +yshift;
	//std::cout << "paraExtrPull::getFunctionout("<< xval << ")="<<out << std::endl;
	return out;
}


graph paraExtrPull::generateMCPoints(size_t idx)const{

	bool poisson=true;

	graph g(points_.size());

	//generate correlated MC statistics

	float mean=getFunctionout(evalpoint_,idx)/mcscale_.at(idx);
	float mcpoint=0;
//	float seed=(idx+1)*(idx+1);
	//seed+=random_->GetSeed();
	//random_->SetSeed(seed);
	if(poisson)
		mcpoint=random_->Poisson(mean);
	else
		mcpoint=random_->Gaus(mean,sqrt(mean));
	//mcpoint=mean;
	float fakestat=sqrt(mcpoint)*mcscale_.at(idx);
	mcpoint-=mean;
	mcpoint*=mcscale_.at(idx);
	if(mcpoint<0)mcpoint=0;

	//add small variation due to signal mc stat ( 3 times more - will be fitted away)
	for(size_t i=0;i<points_.size();i++){
		float funcpoint=getFunctionout(points_.at(i),idx);
	//	seed+=random_->GetSeed();
	//	random_->SetSeed(seed);
		if(poisson)
			mcpoint=random_->Poisson(funcpoint/mcscale_.at(idx) * 3) * mcscale_.at(idx)/3;
		else
			mcpoint=mcpoint+ funcpoint;
		if(mcpoint<0) mcpoint=0;
		g.setPointContents(i,true,points_.at(i),mcpoint);
		g.setPointContents(i,false,0,fakestat);
	}

	return g;
}

graph paraExtrPull::generateDataPoints(size_t idx)const{

	bool poisson=true;


	graph g(points_.size());
	float funcpoint=getFunctionout(evalpoint_,idx);
	//std::cout << funcpoint << std::endl;
	float mean=funcpoint/datascale_.at(idx);
	float datapoint=0;//random_->Poisson(mean);
	//float seed=(idx+1)*(idx+1);
	//seed+=random_->GetSeed();
	//random_->SetSeed(seed);
	if(poisson)
		datapoint=random_->Poisson(mean);
	else
		datapoint=random_->Gaus(mean,sqrt(mean));

	float fakestat=sqrt(datapoint)*datascale_.at(idx);
	datapoint*=datascale_.at(idx);
	if(datapoint<0)datapoint=0;
	//std::cout << "datapoint: " << datapoint <<std::endl;


	for(size_t i=0;i<points_.size();i++){

		g.setPointContents(i,true,points_.at(i),datapoint);
		g.setPointContents(i,false,0,fakestat); //this will be used to determine the scale later
	}

	return g;
}


}

