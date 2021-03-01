/*
 * paraExtrPull.cc
 *
 *  Created on: Aug 15, 2014
 *      Author: kiesej
 */


#include "../interface/paraExtrPull.h"
#include <stdexcept>
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "WbWbXAnalysis/Tools/interface/graphFitter.h"
#include "TCanvas.h"
#include "TFile.h"


namespace ztop{

void paraExtrPull::fill(){
	if(!ex_ || ! cont_ || !random_)
		throw std::logic_error("paraExtrPull::fill: first set configure extractor,random gen and container to be filled");

	//std::cout << std::endl;



	//create functional forms
	if(corrtrueinput_.size()>0){

		if(corrtrueinput_.size() != datatrueinput_.size() || uncorrtrueinput_.size() != datatrueinput_.size()){
			throw std::out_of_range("paraExtrPull::fill: datatrueinput_ and trueinput_ must be same size");

		}

		fits_.clear();
		corrmcscale_.clear();
		uncorrmcscale_.clear();
		datascale_.clear();
		reluncorrcontr_.clear();

		//	std::cout << "paraExtrPull::fill: reading " << trueinput_.size() << " input graphs " <<std::endl;

		for(size_t gidx=0;gidx<corrtrueinput_.size();gidx++){

			//std::cout << "input graph " << gidx<< " has " << trueinput_.at(gidx).getNPoints() << " points " <<std::endl;

			if(corrtrueinput_.at(gidx).getNPoints()>1){

				///very specifc......

				float contentmccorr=0,contentmcuncorr=0;
				if(corrtrueinput_.at(gidx).getNPoints()>3){
					float contentmc=corrtrueinput_.at(gidx).getPointYContent(3);
					contentmccorr=contentmc;
					float scalemc=0;
					if(contentmc!=0){

						float statmc=corrtrueinput_.at(gidx).getPointYStat(3);
						scalemc=statmc*statmc/contentmc;
					}
					corrmcscale_.push_back(scalemc);
					std::cout << "automatically setting corr MC scale "<< gidx <<" to "<<scalemc << std::endl;
				}
				else{
					corrmcscale_.push_back(1);
				}
				if(uncorrtrueinput_.at(gidx).getNPoints()>3){
					float contentmc=uncorrtrueinput_.at(gidx).getPointYContent(3);
					contentmcuncorr=contentmc;
					float scalemc=0;
					if(contentmc!=0){

						float statmc=uncorrtrueinput_.at(gidx).getPointYStat(3);
						scalemc=statmc*statmc/contentmc;
					}
					uncorrmcscale_.push_back(scalemc);

					std::cout << "automatically setting uncorr MC scale "<< gidx <<" to "<<scalemc << std::endl;
				}
				else{
					uncorrmcscale_.push_back(1);
				}

				reluncorrcontr_.push_back(contentmccorr/(contentmccorr+contentmcuncorr));

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


	size_t nNanchecks_=100;
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
		ex_->setFallBackScaleB(corrmcscale_.at(0));


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
	float corrfluct=0;
	float corrstaterr=0;
	if(corrmcscale_.at(idx)!=0){
		float mean=reluncorrcontr_.at(idx)*getFunctionout(evalpoint_,idx)/corrmcscale_.at(idx);
		//	float seed=(idx+1)*(idx+1);
		//seed+=random_->GetSeed();
		//random_->SetSeed(seed);
		if(poisson)
			corrfluct=random_->Poisson(mean);
		else
			corrfluct=random_->Gaus(mean,sqrt(mean));
		//mcpoint=mean;
		corrstaterr=sqrt(corrfluct)*corrmcscale_.at(idx);

		corrfluct-=mean;
		corrfluct*=corrmcscale_.at(idx);
	}

	//add small variation due to signal mc stat ( 3 times more - will be fitted away)
	for(size_t i=0;i<points_.size();i++){
		float uncorrfluct=0,uncorrstaterr=0;
		const float funcpoint=(1-reluncorrcontr_.at(idx))*getFunctionout(points_.at(i),idx);
		//	seed+=random_->GetSeed();
		//	random_->SetSeed(seed);
		if(uncorrmcscale_.at(idx)!=0){
			float uncorrmean=funcpoint/uncorrmcscale_.at(idx);

			if(poisson)
				uncorrfluct=random_->Poisson(uncorrmean);
			else
				uncorrfluct=random_->Gaus(uncorrmean,sqrt(uncorrmean));

			uncorrstaterr=sqrt(uncorrfluct)* uncorrmcscale_.at(idx);
			uncorrfluct-=uncorrmean;
			uncorrfluct*= uncorrmcscale_.at(idx);
		}
		float fillpoint=corrfluct+uncorrfluct+getFunctionout(points_.at(i),idx);
		if(fillpoint<0)fillpoint=0;

		g.setPointContents(i,true,points_.at(i),fillpoint);
		g.setPointContents(i,false,0,sqrt(uncorrstaterr*uncorrstaterr + corrstaterr*corrstaterr));
	}
	if(g.isEmpty())throw std::runtime_error("paraExtrPull::generateMCPoints generated graph empty");

	//debug
	/*
	std::cout << "graph for index: " << idx;
	TCanvas c;

	TFile f("tmp.root","RECREATE");
	g.getTGraph()->Draw("AP");
	c.Print("temp.pdf");
	c.Write();
	f.Close();
	throw std::runtime_error("paraExtrPull::begug point reached");
*/
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

