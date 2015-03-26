/*
 * plotterControlPlot.cc
 *
 *  Created on: Jan 15, 2014
 *      Author: kiesej
 */



#include "../interface/plotterControlPlot.h"
#include "../interface/histoStack.h"
#include "../interface/fileReader.h"
#include "../interface/plot.h"

namespace ztop{

plotterControlPlot::plotterControlPlot(): plotterBase(), divideat_(0), stackp_(0),tempdataentry_(0),invertplots_(false),psmigthresh_(0){
	readStyleFromFileInCMSSW("/src/TtZAnalysis/Tools/styles/controlPlots_standard.txt");
	gStyle->SetOptStat(0);
}

/**
 * expects entries:
 * [plotterControlPlot] defines divideat
 * [textBoxes - boxes]
 * [histoStyle - DataUpper]
 * [histoStyle - MCUpper]
 * [histoStyle - DataRatio]
 * [histoStyle - MCRatio]
 * [plotStyle - Upper]
 * [plotStyle - Ratio]
 */
void plotterControlPlot::addStyleFromFile(const std::string& infile){
	readStylePriv(infile,false);
}
void plotterControlPlot::readStyleFromFile(const std::string& infile){
	readStylePriv(infile,true);
}
void plotterControlPlot::readStylePriv(const std::string& infile,bool requireall){

	if(debug) std::cout <<"plotterControlPlot::readStyleFromFile" << std::endl;

	fileReader fr;
	fr.setComment("$");
	fr.setDelimiter(",");
	fr.setStartMarker("[plotterControlPlot]");
	fr.setEndMarker("[end plotterControlPlot]");
	fr.readFile(infile);
	fr.setRequireValues(false);

	if(requireall){
		if(fr.nLines()<1){
			std::cout << "plotterControlPlot::readStyleFromFile: no [plotterControlPlot] found in " << infile  <<std::endl;
			throw std::runtime_error("plotterControlPlot::readStyleFromFile: no [plotterControlPlot] found");
		}

		divideat_  = fr.getValue<float>("divideat");
		invertplots_  = fr.getValue<bool>("invertplots");
		psmigthresh_  = fr.getValue<float>("PSMigThreshold");
	}
	else{
		divideat_  = fr.getValue<float>("divideat",divideat_);
		invertplots_  = fr.getValue<bool>("invertplots",invertplots_);
		psmigthresh_  = fr.getValue<float>("PSMigThreshold",psmigthresh_);
	}


	upperstyle_.readFromFile(infile, "Upper",requireall);
	ratiostyle_.readFromFile(infile, "Ratio",requireall);
	datastyleupper_.readFromFile(infile, "DataUpper",requireall);
	datastyleratio_.readFromFile(infile, "DataRatio",requireall);
	mcstyleupper_.readFromFile(infile, "MCUpper",requireall);
	mcstyleratio_.readFromFile(infile, "MCRatio",requireall);
	//FIXME
	mcstylepsmig_=mcstyleupper_;
	mcstylepsmig_.readFromFile(infile, "MCUpperMigrations",false);
	//text boxes

	textboxes_.readFromFile(infile,"boxes");
	legstyle_.readFromFile(infile,"",requireall);

}

/**
 * clears plots, clears styles
 */
void plotterControlPlot::clear(){
	for(size_t i=0;i<tempplots_.size();i++){
		if(tempplots_.at(i)) delete tempplots_.at(i);
	}
	tempplots_.clear();
}
void plotterControlPlot::clearPlots(){
	for(size_t i=0;i<tempplots_.size();i++){
		if(tempplots_.at(i)) delete tempplots_.at(i);
	}
	tempplots_.clear();
}

void plotterControlPlot::cleanMem(){
	plotterBase::cleanMem();
	for(size_t i=0;i<tempplots_.size();i++){
		if(tempplots_.at(i)) delete tempplots_.at(i);
	}
	tempplots_.clear();
}

///plotting

void plotterControlPlot::preparePad(){
	if(debug) std::cout <<"plotterControlPlot::preparePad" << std::endl;
	if(!stackp_){
		throw std::logic_error("plotterControlPlot::preparePad: containerStack not set");
	}
	if(!stackp_->checkDrawDimension()){
		if(debug) std::cout << "plotterControlPlot::draw: only available for 1dim stacks!" <<std::endl;
		return;
	}
	cleanMem();
	TVirtualPad * c = getPad();
	c->Clear();
	c->Divide(1,2);
	c->cd(1)->SetPad(0,divideat_,1,1);
	c->cd(2)->SetPad(0,0,1,divideat_);
	upperstyle_.applyPadStyle(c->cd(1));
	ratiostyle_.applyPadStyle(c->cd(2));
}

void plotterControlPlot::refreshPad(){
	TVirtualPad * c = getPad();
	upperstyle_.applyPadStyle(c->cd(1));
	ratiostyle_.applyPadStyle(c->cd(2));
	c->cd(1)->RedrawAxis();
	c->cd(2)->RedrawAxis();
}


void plotterControlPlot::drawPlots(){
	TVirtualPad * c = getPad();
	c->cd(1);
	drawControlPlot();
	c->cd(2);
	drawRatioPlot();

}
// void drawTextBoxes(); // by base class
void plotterControlPlot::drawLegends(){
	if(debug) std::cout <<"plotterCompare::drawLegends" << std::endl;
	getPad()->cd(1);
	legstyle_.applyLegendStyle(tmplegp_);
	tmplegp_->Draw("same");
	//use templegps_

	/* ... */
}
/**
 * returns ordered pointers to objects to add to lagends
 */
void plotterControlPlot::drawControlPlot(){
	if(debug) std::cout << "plotterControlPlot::drawControlPlot " << stackp_->getName()<<std::endl;
	getPad()->cd(1);
	TH1::AddDirectory(false);
	//make axis histo
	//draw axis
	size_t dataentry= std::find(stackp_->legends_.begin(),stackp_->legends_.end(),stackp_->dataleg_) - stackp_->legends_.begin();

	if(debug)std::cout <<  "found dataentry at position "<< dataentry<< " of " <<  stackp_->size()-1<< std::endl;
	tempdataentry_=dataentry;
	if(dataentry == stackp_->size()){
		std::cout <<  "plotterControlPlot::drawControlPlot: no data entry found for " << stackp_->getName() <<std::endl;
		throw std::runtime_error("plotterControlPlot::drawControlPlot: no data entry found");
	}
	bool divbbw= upperstyle_.divideByBinWidth;
	if(stackp_->hasTag(taggedObject::dontDivByBW_tag)) divbbw=false;
	TH1 * axish=addObject(stackp_->getContainer(dataentry).getTH1D("",divbbw,false,false));
	plotStyle upperstyle=upperstyle_;
	upperstyle.absorbYScaling(getSubPadYScale(1));
	axish->Draw("AXIS");
	float ymax=stackp_->getYMax();
	if(ymax<=0)
		ymax=1/1.2;
	axish->GetYaxis()->SetRangeUser(0.0001,ymax*1.2);
	upperstyle.applyAxisStyle(axish);
	axish->Draw("AXIS");


	//prepare data
	plot* dataplottemp =  new plot(&stackp_->getContainer(dataentry),divbbw);
	tempplots_.push_back(dataplottemp);
	datastyleupper_.applyContainerStyle(dataplottemp);

	//set up legend here....
	tmplegp_=addObject(new TLegend((Double_t)0.65,(Double_t)0.50,(Double_t)0.95,(Double_t)0.90));
	tmplegp_->Clear();
	tmplegp_->SetFillStyle(0);
	tmplegp_->SetBorderSize(0);

	if(datastyleupper_.legendDrawStyle != "none")
		tmplegp_->AddEntry(dataplottemp->getSystGraph(),stackp_->getLegend(dataentry),datastyleupper_.legendDrawStyle);

	std::vector<size_t> signalidxs=stackp_->getSignalIdxs();


	if(stackp_->size()>1){ //its not only data

		std::vector<TObject *> sortedout;
		std::vector<size_t> sorted=stackp_->getSortedIdxs(invertplots_); //invert
		std::vector<size_t> nisorted=stackp_->getSortedIdxs(!invertplots_); //invert
		std::vector<TH1 *> stackedhistos;

		histo1D sumcont=stackp_->getContainer(dataentry);
		sumcont.clear();
		bool tmpaddStatCorrelated=histoContent::addStatCorrelated;
		histoContent::addStatCorrelated=false;
		size_t firstmccount=0;
		if(dataentry==0) firstmccount++;

		std::vector<TString> legendentries;
		bool foundPSmig=false;
		if(sorted.size() != stackp_->size())
			throw std::out_of_range("plotterControlPlot::drawControlPlot: serious: sorted.size() != stackp_->size()");

		for(size_t it=0;it<stackp_->size();it++){ //it is the right ordering
			size_t i=sorted.at(it);
			if(i != dataentry){
				histo1D tempcont;
				if(stackp_->is1DUnfold()){ //special treatment
					tempcont = stackp_->getContainer1DUnfold(i).getBackground();
					sumcont+=tempcont;
					//is not signal


					if(std::find(signalidxs.begin(),signalidxs.end(),i)==signalidxs.end()){//is not signal
						TH1D * h=addObject(sumcont.getTH1D(stackp_->getLegend(i)+" "+stackp_->getName()+"_stack_h",divbbw,true,true)); //no errors
						if(!h)
							continue;
						mcstyleupper_.applyContainerStyle(h,false);
						h->SetFillColor(stackp_->colors_.at(i));
						stackedhistos.push_back(h);
						legendentries.push_back(stackp_->getLegend(i));
					}
					else{//this is signal but PS migrations!
						histo1D visSig=stackp_->getContainer1DUnfold(i).getVisibleSignal();
						if(tempcont.integral(true) / visSig.integral(true) >psmigthresh_){
							foundPSmig=true;
							TH1D * h=addObject(sumcont.getTH1D(stackp_->getLegend(i)+" PSmig "+stackp_->getName()+"_stack_h",divbbw,true,true)); //no errors
							mcstylepsmig_.applyContainerStyle(h,false);
							h->SetFillColor(1);//stackp_->colors_.at(i)+5);
							stackedhistos.push_back(h);
							legendentries.push_back("");
						}
						tempcont = visSig;
						sumcont+=tempcont;
						TH1D * hsig=addObject(sumcont.getTH1D(stackp_->getLegend(i)+" "+stackp_->getName()+"_stack_h",divbbw,true,true));
						mcstyleupper_.applyContainerStyle(hsig,false);
						hsig->SetFillColor(stackp_->colors_.at(i));
						stackedhistos.push_back(hsig);
						legendentries.push_back(stackp_->getLegend(i));

					}



				}
				else{
					tempcont = stackp_->getContainer(i);
					sumcont+=tempcont;
					TH1D * h=addObject(sumcont.getTH1D(stackp_->getLegend(i)+" "+stackp_->getName()+"_stack_h",divbbw,true,true)); //no errors
					if(!h)
						continue;
					mcstyleupper_.applyContainerStyle(h,false);
					h->SetFillColor(stackp_->colors_.at(i));
					stackedhistos.push_back(h);
					legendentries.push_back(stackp_->getLegend(i));
				}


			}
			else{
				stackedhistos.push_back(0);
			}
		}
		//draw in inverse order


		for(size_t i=stackedhistos.size()-1; i+1>0;i--){

			if(stackedhistos.at(i)){ //not data

				stackedhistos.at(i)->Draw(mcstyleupper_.rootDrawOpt+"same");
				if(legendentries.at(i)!="" && mcstyleupper_.legendDrawStyle != "none")
					tmplegp_->AddEntry(stackedhistos.at(i),legendentries.at(i),mcstyleupper_.legendDrawStyle);
			}

		}
		//make errors (use sumcont)
		TG * mcerr=addObject(sumcont.getTGraph(stackp_->getName()+"mcerr_cp",divbbw,false,false,false));
		mcstyleupper_.applyContainerStyle(mcerr,true);

		tmplegp_->AddEntry(mcerr,"MC syst+stat","f");
		if(stackp_->is1DUnfold() && foundPSmig){ //
			TH1D * dummy=addObject(new TH1D());
			mcstylepsmig_.applyContainerStyle(dummy,false);
			dummy->SetFillColor(1);
			tmplegp_->AddEntry(dummy,"PS migrations",mcstylepsmig_.legendDrawStyle);
		}
		mcerr->Draw("same"+mcstyleupper_.sysRootDrawOpt);

		histoContent::addStatCorrelated=tmpaddStatCorrelated;
	}
	//plot data now
	dataplottemp->getSystGraph()->Draw(datastyleupper_.sysRootDrawOpt+"same");
	dataplottemp->getStatGraph()->Draw(datastyleupper_.rootDrawOpt+"same");

}
void plotterControlPlot::drawRatioPlot(){
	getPad()->cd(2);
	if(stackp_->size() < 2){
		std::cout << "plotterControlPlot::drawRatioPlot: did not find both data and MC, skipping ratio plot." <<std::endl;
		return;
	}
	histo1D fullmc=stackp_->getFullMCContainer(); //dont div by bw
	if(stackp_->is1DUnfold())
		fullmc=stackp_->getFullMCContainer1DUnfold().getRecoContainer();
	histo1D fullmcerr=fullmc.getRelErrorsContainer();

	plot * mcerr=new plot(&fullmcerr,false);
	tempplots_.push_back(mcerr);

	TH1* axish=addObject(fullmc.convertToGraph(false).getAxisTH1(false,true));
	plotStyle ratiostyle=ratiostyle_;
	ratiostyle.absorbYScaling(getSubPadYScale(2));
	axish->Draw("AXIS");
	ratiostyle.applyAxisStyle(axish);
	axish->GetYaxis()->CenterTitle(true);
	axish->Draw("AXIS");

	mcstyleratio_.applyContainerStyle(mcerr);
	TG* g =mcerr->getStatGraph();
	TG* gs=mcerr->getSystGraph();

	gs->Draw("same"+mcstyleratio_.sysRootDrawOpt);
	g->Draw("same"+mcstyleratio_.rootDrawOpt);

	///data
	histo1D datac=stackp_->getContainer(tempdataentry_);
	if(stackp_->is1DUnfold())
		datac=stackp_->getContainer1DUnfold(tempdataentry_).getRecoContainer();

	datac.normalizeToContainer(fullmc);

	plot * datar=new plot(&datac,false);
	tempplots_.push_back(datar);
	datar->removeXErrors();
	datastyleratio_.applyContainerStyle(datar);
	g =datar->getStatGraph();
	gs=datar->getSystGraph();

	gs->Draw("same"+datastyleratio_.sysRootDrawOpt);
	g->Draw("same"+datastyleratio_.rootDrawOpt);


	TLine * l = addObject(new TLine(datac.getXMin(),1,datac.getXMax(),1));
	l->Draw("same");

}


}
