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

plotterControlPlot::plotterControlPlot(): plotterBase(), divideat_(0),
		stackp_(0),/*tempdataentry_(0),*/invertplots_(false),psmigthresh_(0),mcsysstatleg_(true),nolegend_(false),
		systlabel_("MC syst+stat"){

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

void plotterControlPlot::addStyleFromFile(const std::string& file,const std::string&  marker,const std::string&  endmarker){
	fileReader extraconfig;
	extraconfig.setComment("$");
	extraconfig.setDelimiter(",");
	extraconfig.setStartMarker(marker);
	extraconfig.setEndMarker(endmarker);

	extraconfig.readFile(file);
	std::string tmpfile=extraconfig.dumpFormattedToTmp();
	plotterBase::addStyleFromFile(tmpfile);
	system(("rm -f " + tmpfile).data());
}

void plotterControlPlot::readStylePriv(const std::string& infile,bool requireall){

	if(debug) std::cout <<"plotterControlPlot::readStyleFromFile" << std::endl;

	fileReader fr;
	fr.setComment("$");
	fr.setDelimiter(",");
	fr.setStartMarker("[plotterControlPlot]");
	fr.setEndMarker("[end - plotterControlPlot]");
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
	//never required
	yspacemulti_= fr.getValue<float>("ySpaceMulti",yspacemulti_);
	nolegend_= fr.getValue<bool>("nolegend",nolegend_);
	//merges are never required
	fr.clear();
	fr.setStartMarker("[merge legends]");
	fr.setEndMarker("[end - merge legends]");
	fr.readFile(infile);
	for(size_t line=0;line<fr.nLines();line++){
		if(fr.nEntries(line) < 3) continue; //not valid entry
		legendmerge mleg;
		mleg.mergedname = fr.getData<TString>(line,0);
		mleg.mergedcolor = fr.getData<int>(line,1);
		for(size_t entr=2;entr<fr.nEntries(line);entr++){
			mleg.tobemerged.push_back(fr.getData<TString>(line,entr));
		}
		mergelegends.push_back(mleg);
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
	corrm_=0;
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


float plotterControlPlot::getXAxisLowerLimit()const{
	if(!stackp_){
		throw std::logic_error("plotterControlPlot::getXAxisLowerLimit: containerStack not set");
	}
	if(!stackp_->checkDrawDimension()){
		throw std::logic_error( "plotterControlPlot::getXAxisLowerLimit: only available for 1dim stacks!" );
	}
	return stackp_->getDataContainer().getBins().at(1);
}
float plotterControlPlot::getXAxisHigherLimit()const{
	if(!stackp_){
		throw std::logic_error("plotterControlPlot::getXAxisHigherLimit: containerStack not set");
	}
	if(!stackp_->checkDrawDimension()){
		throw std::logic_error( "plotterControlPlot::getXAxisHigherLimit: only available for 1dim stacks!" );
	}
	return stackp_->getDataContainer().getBins().at(stackp_->getDataContainer().getBins().size()-1);
}


float plotterControlPlot::getYSpaceMulti(bool logar,bool divbw)const{
	if(!logar)
		return yspacemulti_;

	if(!stackp_)
		throw std::logic_error("plotterControlPlot::getYSpaceMulti: cannot derive if no stack assiciated");

	float ymax=stackp_->getYMax(divbw);
	float ymin=stackp_->getYMin(divbw);

	//float logofdiff=log(ymax-ymin)/(2.30258509299404590e+00);
	float logmax=log(ymax)/(2.30258509299404590e+00);
	float logmin=log(ymin)/(2.30258509299404590e+00);

	logmax+=(logmax-logmin)*(yspacemulti_-1);

	return pow(10,logmax)/ymax;
}

///plotting

void plotterControlPlot::preparePad(){
	if(debug) std::cout <<"plotterControlPlot::preparePad" << std::endl;
	if(!stackp_){
		throw std::logic_error("plotterControlPlot::preparePad: containerStack not set");
	}
	if(!stackp_->checkDrawDimension()){
		throw std::logic_error( "plotterControlPlot::preparePad: only available for 1dim stacks!");
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
	if(nolegend_) return;
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

	const histoStack * usestack=stackp_;
	histoStack mergedlegstack; //keep empty if not needed
	if(mergelegends.size()>0){
		mergedlegstack=*stackp_;//copy
		for(size_t i=0;i<mergelegends.size();i++){
			mergedlegstack.mergeLegends(mergelegends.at(i).tobemerged,mergelegends.at(i).mergedname, mergelegends.at(i).mergedcolor,true);
		}
		usestack=&mergedlegstack;
	}

	size_t dataentry= std::find(usestack->legends_.begin(),usestack->legends_.end(),usestack->dataleg_) - usestack->legends_.begin();

	if(debug)std::cout <<  "found dataentry at position "<< dataentry<< " of " <<  usestack->size()-1<< std::endl;

	if(dataentry == usestack->size()){
		std::cout <<  "plotterControlPlot::drawControlPlot: no data entry found for " << usestack->getName() <<std::endl;
		throw std::runtime_error("plotterControlPlot::drawControlPlot: no data entry found");
	}
	bool divbbw= upperstyle_.divideByBinWidth;
	if(usestack->hasTag(taggedObject::dontDivByBW_tag)) divbbw=false;
	TH1 * axish=addObject(usestack->getContainer(dataentry).getAxisTH1D());//("",divbbw,false,false));
	plotStyle upperstyle=upperstyle_;
	upperstyle.absorbYScaling(getSubPadYScale(1));
	//new

	upperstyle.absorbXScaling(getSubPadXScale(1));

	float ymin=usestack->getYMin(divbbw);
	float ymax=usestack->getYMax(divbbw);

	if(upperstyle.yAxisStyle()->applyAxisRange() && upperstyle.yAxisStyle()->min<0)
		upperstyle.yAxisStyle()->min=(ymax-ymin)/10000;

	//axish->Draw("AXIS");
	if(ymax<=0)
		ymax=1/1.2;
	axish->GetYaxis()->SetRangeUser(0.0001,ymax*getYSpaceMulti(upperstyle.yAxisStyle()->log,divbbw));
	upperstyle.applyAxisStyle(axish);
	axish->Draw("AXIS");
	if(debug)std::cout <<  "axis drawn" <<std::endl;

	//prepare data
	plot* dataplottemp =  new plot(&usestack->getContainer(dataentry),divbbw);
	tempplots_.push_back(dataplottemp);
	datastyleupper_.applyContainerStyle(dataplottemp);

	if(debug)std::cout <<  "setting up legend" <<std::endl;
	//set up legend here....
	tmplegp_=addObject(new TLegend((Double_t)0.65,(Double_t)0.50,(Double_t)0.95,(Double_t)0.90));
	tmplegp_->Clear();
	tmplegp_->SetFillStyle(0);
	tmplegp_->SetBorderSize(0);

	if(datastyleupper_.legendDrawStyle != "none")
		tmplegp_->AddEntry(dataplottemp->getSystGraph(),usestack->getLegend(dataentry),datastyleupper_.legendDrawStyle);

	std::vector<size_t> signalidxs=usestack->getSignalIdxs();


	if(usestack->size()>1){ //its not only data

		std::vector<TObject *> sortedout;
		std::vector<size_t> sorted=usestack->getSortedIdxs(invertplots_); //invert
		std::vector<size_t> nisorted=usestack->getSortedIdxs(!invertplots_); //invert
		std::vector<TH1 *> stackedhistos;

		histo1D sumcont=usestack->getContainer(dataentry);
		sumcont.clear();
		bool tmpaddStatCorrelated=histoContent::addStatCorrelated;
		histoContent::addStatCorrelated=false;
		size_t firstmccount=0;
		if(dataentry==0) firstmccount++;

		std::vector<TString> legendentries;
		bool foundPSmig=false;
		if(sorted.size() != usestack->size())
			throw std::out_of_range("plotterControlPlot::drawControlPlot: serious: sorted.size() != usestack->size()");

		bool plotWithNegativeEntries=false;
		if(usestack->is1D()){
			const histo1D& fullMC=usestack->getFullMCContainer();
			float max=fullMC.getYMax(true);
			float min=fullMC.getYMin(true);
			if(min<0 && -min>0.05*max){
				plotWithNegativeEntries=true;
				std::cout << "plotterControlPlot::drawControlPlot: plot "<<usestack->getName()
							<<  " has significant negative entries. Will plot sum MC and no stacked histo" <<std::endl;


			}
		}
		//check if it might be a control plot going well below 0, indicating a difference plot
		//this is not a standard stack plot. Stacking will not work here!
		if(plotWithNegativeEntries){ //only valid here, 2D is anyway not stacked
			histo1D fullMC=usestack->getFullMCContainer();
			histo1D fulldata=usestack->getDataContainer();
			sumcont=fullMC;
			TH1D * h=addObject(fullMC.getTH1D("MC "+usestack->getName()+"_stack_h",divbbw,true,true)); //no errors
			h->SetFillColor(kRed);
			stackedhistos.push_back(h);
			legendentries.push_back("MC");
			//data
			stackedhistos.push_back(0);
			legendentries.push_back(usestack->dataleg_);
			axish->GetYaxis()->SetRangeUser(1.1*fullMC.getYMin(true),ymax*getYSpaceMulti(upperstyle.yAxisStyle()->log,divbbw));

		}
		else{
			for(size_t it=0;it<usestack->size();it++){ //it is the right ordering
				size_t i=sorted.at(it);
				if(i != dataentry){
					histo1D tempcont;
					if(usestack->is1DUnfold()){ //special treatment
						tempcont = usestack->getContainer1DUnfold(i).getRecoContainer();

						//is not signal


						if(std::find(signalidxs.begin(),signalidxs.end(),i)==signalidxs.end()){//is not signal
							sumcont+=tempcont;
							TH1D * h=addObject(sumcont.getTH1D(usestack->getLegend(i)+" "+usestack->getName()+"_stack_h",divbbw,true,true)); //no errors
							if(!h)
								continue;
							mcstyleupper_.applyContainerStyle(h,false);
							h->SetFillColor(usestack->colors_.at(i));
							stackedhistos.push_back(h);
							legendentries.push_back(usestack->getLegend(i));
						}
						else{//this is signal but PS migrations!
							tempcont = usestack->getContainer1DUnfold(i).getBackground();
							sumcont+=tempcont;
							histo1D visSig=usestack->getContainer1DUnfold(i).getVisibleSignal();
							if(tempcont.integral(true) / visSig.integral(true) >psmigthresh_){
								foundPSmig=true;
								TH1D * h=addObject(sumcont.getTH1D(usestack->getLegend(i)+" PSmig "+usestack->getName()+"_stack_h",divbbw,true,true)); //no errors
								mcstylepsmig_.applyContainerStyle(h,false);
								h->SetFillColor(1);//usestack->colors_.at(i)+5);
								stackedhistos.push_back(h);
								legendentries.push_back("");
							}
							tempcont = visSig;
							sumcont+=tempcont;
							TH1D * hsig=addObject(sumcont.getTH1D(usestack->getLegend(i)+" "+usestack->getName()+"_stack_h",divbbw,true,true));
							mcstyleupper_.applyContainerStyle(hsig,false);
							hsig->SetFillColor(usestack->colors_.at(i));
							stackedhistos.push_back(hsig);
							legendentries.push_back(usestack->getLegend(i));
							if(debug)
								std::cout << "plotterControlPlot::drawControlPlot: added to stack: " << usestack->getLegend(i) <<std::endl;

						}



					}
					else{
						tempcont = usestack->getContainer(i);
						sumcont+=tempcont;
						TH1D * h=addObject(sumcont.getTH1D(usestack->getLegend(i)+" "+usestack->getName()+"_stack_h",divbbw,true,true)); //no errors
						if(!h)
							continue;
						mcstyleupper_.applyContainerStyle(h,false);
						h->SetFillColor(usestack->colors_.at(i));
						stackedhistos.push_back(h);
						legendentries.push_back(usestack->getLegend(i));
						if(debug)
							std::cout << "plotterControlPlot::drawControlPlot: added to stack: " << usestack->getLegend(i) <<std::endl;
					}


				}
				else{
					stackedhistos.push_back(0);
					legendentries.push_back(usestack->dataleg_);
				}
			}
		}
		//draw in inverse order


		for(size_t i=stackedhistos.size()-1; true;i--){

			if(stackedhistos.at(i)){ //not data
				stackedhistos.at(i)->GetXaxis()->SetTickLength(0);
				stackedhistos.at(i)->GetXaxis()->SetLabelSize(0);
				stackedhistos.at(i)->Draw(mcstyleupper_.rootDrawOpt+"same");
				if(legendentries.at(i)!="" && mcstyleupper_.legendDrawStyle != "none"){
					tmplegp_->AddEntry(stackedhistos.at(i),legendentries.at(i),mcstyleupper_.legendDrawStyle);
				}
				if(debug)
					std::cout << "plotterControlPlot::drawControlPlot: drawn: " << legendentries.at(i) <<std::endl;

			}
			if(!i)
				break;

		}
		//make errors (use sumcont)
		if(corrm_)
			sumcont.mergeAllErrors("mergederr",false,*corrm_);
		TG * mcerr=addObject(sumcont.getTGraph(usestack->getName()+"mcerr_cp",divbbw,false,false,false));
		mcstyleupper_.applyContainerStyle(mcerr,true);
		mcerr->GetXaxis()->SetTickLength(0);
		if(mcsysstatleg_)
			tmplegp_->AddEntry(mcerr,systlabel_,"f");
		if(usestack->is1DUnfold() && foundPSmig){ //
			TH1D * dummy=addObject(new TH1D());
			mcstylepsmig_.applyContainerStyle(dummy,false);
			dummy->SetFillColor(1);
			tmplegp_->AddEntry(dummy,"PS migrations",mcstylepsmig_.legendDrawStyle);
		}
		mcerr->Draw("same"+mcstyleupper_.sysRootDrawOpt);

		histoContent::addStatCorrelated=tmpaddStatCorrelated;
	}
	//plot data now
	if(datastyleupper_.rootDrawOpt.Contains("X0"))
		dataplottemp->removeXErrors();
	dataplottemp->getSystGraph()->Draw(datastyleupper_.sysRootDrawOpt+"same");
	dataplottemp->getStatGraph()->Draw(datastyleupper_.rootDrawOpt+"same");

}
void plotterControlPlot::drawRatioPlot(){
	/*
	 *
	 * For #geq indicators: copy axis to new axis without labels and ticks and name
	 * last bin with #geq and blanks
	 *
	 */


	//TVirtualPad* thispad=getPad()->cd(2);
	if(stackp_->size() < 2){
		std::cout << "plotterControlPlot::drawRatioPlot: did not find both data and MC, skipping ratio plot." <<std::endl;
		return;
	}
	histo1D fullmc=stackp_->getFullMCContainer(); //dont div by bw
	if(stackp_->is1DUnfold())
		fullmc=stackp_->getFullMCContainer1DUnfold().getRecoContainer();

	histo1D fullmcerr=fullmc.getRelErrorsContainer();
	if(corrm_)
		fullmcerr.mergeAllErrors("merged",false,*corrm_);

	plot * mcerr=new plot(&fullmcerr,false);
	tempplots_.push_back(mcerr);

	TH1* axish=addObject(fullmc.convertToGraph(false).getAxisTH1(false,true));
	plotStyle ratiostyle=ratiostyle_;
	ratiostyle.absorbYScaling(getSubPadYScale(2));
	axish->Draw("AXIS");
	ratiostyle.applyAxisStyle(axish);
	axish->GetYaxis()->CenterTitle(true);
	axish->Draw("AXIS");

	//// experimental part
	//TGaxis * gaxis=addObject(new TGaxis());
	//TAxis * axiscp = addObject(new TAxis(* axish->GetXaxis()));

	//should be put in something like getbincenterNDC(bin,pad)
	//the axis is at leftmargin,bottommargin in pad coords

	///

	mcstyleratio_.applyContainerStyle(mcerr);
	TG* g =mcerr->getStatGraph();
	TG* gs=mcerr->getSystGraph();

	gs->Draw("same"+mcstyleratio_.sysRootDrawOpt);
	g->Draw("same"+mcstyleratio_.rootDrawOpt);



	///data
	histo1D datac=stackp_->getDataContainer();
	fullmc.removeStatFromAll(true);
	datac.normalizeToContainer(fullmc);

	plot * datar=new plot(&datac,false);
	if(datastyleratio_.rootDrawOpt.Contains("X0"))
		datar->removeXErrors();

	tempplots_.push_back(datar);
	datastyleratio_.applyContainerStyle(datar);
	g =datar->getStatGraph();
	gs=datar->getSystGraph();

	gs->Draw("same"+datastyleratio_.sysRootDrawOpt);
	g->Draw("same"+datastyleratio_.rootDrawOpt);

	//experimental part...

	/*thispad->GetListOfPrimitives()->Print();

	float ux=datac.getBinCenter(datac.getBins().size()-2);
	float uy=ratiostyle_.yAxisStyle()->min;
	float ndcx, ndcy;
	convertUserCoordsToNDC(thispad,ux,uy,ndcx,ndcy);
	addTextBox(ndcx,ndcy-ratiostyle_.yAxisStyle()->labelOffset- ratiostyle_.yAxisStyle()->labelSize,"#geq   ",
			ratiostyle_.yAxisStyle()->labelSize);
	textboxes_.at(textboxes_.size()-1).setAlign(32);
	 */

	TLine * l = addObject(new TLine(datac.getXMin(),1,datac.getXMax(),1));
	l->Draw("same");

}


}
