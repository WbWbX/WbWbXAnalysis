/*
 * ttbarXsecFitter.cc
 *
 *  Created on: Oct 9, 2014
 *      Author: kiesej
 */


#include "../interface/ttbarXsecFitter.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "TtZAnalysis/Tools/interface/containerStack.h"
#include "TtZAnalysis/Tools/interface/containerStackVector.h"
#include "TtZAnalysis/Tools/interface/fileReader.h"
#include "Math/Functor.h"
#include "TtZAnalysis/Tools/interface/plotterControlPlot.h"
#include "TtZAnalysis/Tools/interface/texTabler.h"


namespace ztop{

bool ttbarXsecFitter::debug=false;

void ttbarXsecFitter::readInput(const std::string & configfilename){
	if(debug)
		std::cout << "ttbarXsecFitter::readInput" <<std::endl;
	fitsucc_=false;
	std::vector<containerStack>   TeV8Stacks = readStacks(configfilename,"8");
	std::vector<containerStack>   TeV7Stacks = readStacks(configfilename,"7");

	//size is exactly 3 and is required to be the same for 3 b-jet categories
	if(TeV7Stacks.size() > 0 ){
		for(size_t i=0;i<TeV8Stacks.size();i++){
			for(size_t j=0;j<TeV7Stacks.at(i).size();j++){
				TeV8Stacks.at(i).equalizeSystematicsIdxs(TeV7Stacks.at(j));
			}
		}
	}
	//syst indicies are all the same. possibly zeros have been added

	//just push back the 7 TeV entries in nbjets after the 8 TeV ones and set last8TeVentry_
	last8TeVentry_=TeV8Stacks.size()-1;
	TeV8Stacks.insert(TeV8Stacks.end(),TeV7Stacks.begin(),TeV7Stacks.end());

	//for debugging//
	plotterControlPlot pl;
	TCanvas c;
	pl.usePad(&c);


	size_t bjetcount=0;
	bool eighttev=true;
	for(size_t it=0;it<TeV8Stacks.size();it++){
		if(it == last8TeVentry_+1){
			bjetcount=0;
			eighttev=false;
		}
		pl.setStack(&TeV8Stacks.at(it));
		pl.draw();
		TString name=TeV8Stacks.at(it).getName()+" "+toTString(it);
		if(eighttev) name+="8TeV.pdf";
		else name+="7TeV.pdf";
		name.ReplaceAll(" ","_");
		c.Print(name);
		//	continue;

		//create stuff here
		//	variateContainer1D::debug=true;
		//	extendedVariable::debug=true;
		//	simpleFitter::printlevel=1;
		signalshape_nbjet_.push_back(createLeptonJetAcceptance(&TeV8Stacks,bjetcount,eighttev));

		container1D temp=TeV8Stacks.at(it).getContainer( TeV8Stacks.at(it).getDataIdx());
		if(useMConly_){
			temp=TeV8Stacks.at(it).getFullMCContainer();
			temp.setAllErrorsZero(false);
		}
		//its ok to do that several times, anyway fast


		variateContainer1D tmpvarc;
		tmpvarc.import(temp);
		data_nbjet_.push_back(tmpvarc);

		temp=TeV8Stacks.at(it).getBackgroundContainer();
		tmpvarc.import(temp);
		background_nbjet_.push_back(tmpvarc);

		bjetcount++;
	}

	//set some starting paras:
	std::vector<TString> systs=signalshape_nbjet_.at(0).getSystNames();
	lumiidx8_=std::find(systs.begin(),systs.end(),"Lumi8") - systs.begin();
	lumiidx7_=std::find(systs.begin(),systs.end(),"Lumi7") - systs.begin();
	xsecidx8_=std::find(systs.begin(),systs.end(),"Xsec8") - systs.begin();
	xsecidx7_=std::find(systs.begin(),systs.end(),"Xsec7") - systs.begin();

	ndependencies_=systs.size();

	fittedparas_.resize(ndependencies_,0);
	priors_.resize(ndependencies_,prior_gauss);
	parameternames_=systs;


}

void ttbarXsecFitter::setPrior(const TString& sysname, priors prior){
	if(debug)
		std::cout << "ttbarXsecFitter::setPrior" <<std::endl;

	if(signalshape_nbjet_.size()<1){
		throw std::logic_error("ttbarXsecFitter::setPrior: first read in plots");
	}
	std::vector<TString> systs=signalshape_nbjet_.at(0).getSystNames();
	size_t idx=std::find(systs.begin(),systs.end(),sysname) - systs.begin();
	if(idx == systs.size())
		throw std::out_of_range("ttbarXsecFitter::setPrior: requested variation not found");

	priors_.at(idx) = prior;

}

containerStack ttbarXsecFitter::produceStack(bool fittedvalues,size_t bjetcat,bool eighttev)const{
	if(debug)
		std::cout << "ttbarXsecFitter::produceStack" <<std::endl;
	checkSizes();
	if(bjetcat > 2)
		throw std::logic_error("ttbarXsecFitter::produceStack: allowed b-jet categories: 0,1,2. 0 includes 3+");

	if(last8TeVentry_ >= signalshape_nbjet_.size()-1 && !eighttev)
		throw std::logic_error("ttbarXsecFitter::produceStack: request for 7 TeV stack but no input for 7 TeV");

	if(!eighttev)
		bjetcat+=last8TeVentry_+1;
	size_t nbjet=bjetcat;

	containerStack out;
	container1D data,signal,background;
	std::vector<double> fittedparas;
	if(fittedvalues)
		fittedparas=fittedparas_;
	else
		fittedparas.resize(fittedparas_.size(),0);

	variateContainer1D::debug=true;
	data=data_nbjet_.at(nbjet).exportContainer( fittedparas );

	background=background_nbjet_.at(nbjet).exportContainer( fittedparas );

	double multi = lumi8_ * (fittedparas[xsecidx8_] + xsecoff8_);
	if(nbjet>last8TeVentry_)
		multi = lumi7_ * (fittedparas[xsecidx7_]+ xsecoff7_);
	if(norm_nbjet_global_)
		multi*=normalization_nbjet_.at(nbjet).getValue(fittedparas);
	multi/=signalshape_nbjet_.at(nbjet).getIntegral(&fittedparas[0]);
	container1D exportc=signalshape_nbjet_.at(nbjet).exportContainer( fittedparas );
	signal=  exportc * multi ;


	out.push_back(data,"data",1,1,99);
	out.push_back(background,"background",601,1,2);
	out.push_back(signal,"signal",633,1,1);

	return out;
}


int ttbarXsecFitter::fit(){
	if(debug)
		std::cout << "ttbarXsecFitter::fit" <<std::endl;
	checkSizes();
	//no except: sizes ok
	//	fittedparas_.at(xsecidx8_)=-10.; //other starting value

	fitter_.setParameterNames(parameternames_);
	fitter_.setRequireFitFunction(false);
	std::vector<double> stepwidths;
	stepwidths.resize(fittedparas_.size(),1e-8);
	fitter_.setParameters(fittedparas_,stepwidths);
	fitter_.setMinimizer(simpleFitter::mm_minuit2);
	fitter_.setMaxCalls(4e8);
	ROOT::Math::Functor f(this,&ttbarXsecFitter::toBeMinimized,ndependencies_);
	fitter_.setMinFunction(&f);

	fitter_.setTolerance(0.1);

	fitter_.addMinosParameter(xsecidx7_);
	fitter_.addMinosParameter(xsecidx8_);

	simpleFitter::printlevel=1;

	fitter_.fit();

	if(fitter_.wasSuccess()){
		fittedparas_ = *fitter_.getParameters();

		fitsucc_=true;
		return 0;
	}
	//else
	std::cout << "ttbarXsecFitter::fit(): Fit failed" <<std::endl;
	throw std::runtime_error("ttbarXsecFitter::fit(): Fit failed");
	return -1;
}


container1D ttbarXsecFitter::getCb (bool fittedvalues, bool eighttev)const{
	//if(eighttev) return container_c_b_.at(0); else return container_c_b_.at(1);
	variateContainer1D temp=container_c_b_.at(0);
	if(!eighttev)
		temp=container_c_b_.at(1);

	if(fittedvalues)
		return temp.exportContainer(fittedparas_);
	else
		return temp.exportContainer();

}
container1D ttbarXsecFitter::getEps(bool fittedvalues, bool eighttev)const{
	//{if(eighttev) return container_eps_b_.at(0); else return container_eps_b_.at(1);}
	variateContainer1D temp=container_eps_b_.at(0);
	if(!eighttev)
		temp=container_eps_b_.at(1);

	if(fittedvalues)
		return temp.exportContainer(fittedparas_);
	else
		return temp.exportContainer();

}

container2D  ttbarXsecFitter::getCorrelations()const{
	container2D out;
	fitter_.fillCorrelationCoefficients(&out);
	return out;
}



double ttbarXsecFitter::getParaError(size_t idx)const{
	if(idx>=fitter_.getParameters()->size())
		throw std::out_of_range("ttbarXsecFitter::getParaError");
	return fitter_.getParameterErr(idx);
}
TString ttbarXsecFitter::getParaName(size_t idx)const{
	if(idx>=fitter_.getParameterNames()->size())
		throw std::out_of_range("ttbarXsecFitter::getParaName");
	return fitter_.getParameterNames()->at(idx);
}
double ttbarXsecFitter::getParaErrorContributionToXsec(size_t idx, double sevenoreight)const{
	if(idx>=fitter_.getParameters()->size())
		throw std::out_of_range("ttbarXsecFitter::getParaName");
	size_t xsecidx=getXsecIdx(sevenoreight);
	double xsecoffset=xsecoff8_;
	if(xsecidx==xsecidx7_)xsecoffset=xsecoff7_;
	double errxsec=fitter_.getParameterErr(xsecidx);
	double corr  =fitter_.getCorrelationCoefficient(xsecidx,idx);
	double paraerr=fitter_.getParameterErr(idx);
	if(idx == xsecidx)
		return paraerr/(fitter_.getParameter(xsecidx) + xsecoffset);
	return corr*paraerr*errxsec/(fitter_.getParameter(xsecidx) + xsecoffset);

	////new approach
	/*
	 * Fix parameter to para+paraerror -> get xsec and compare to nominal
	 * same for para-paraerror
	 * should take care of non-linearities
	 */


}
size_t ttbarXsecFitter::getXsecIdx(double sevenoreight)const{
	size_t xsecidx=xsecidx8_;
	if(isApprox(sevenoreight,7.))
		xsecidx=xsecidx7_;
	return xsecidx;
}

texTabler ttbarXsecFitter::makeSystBreakdown(double sevenoreight)const{
	size_t xsecidx=getXsecIdx(sevenoreight);
	formatter fmt;

	TString cmsswbase=getenv("CMSSW_BASE");
	fmt.readInNameTranslateFile((cmsswbase+"/src/TtZAnalysis/Analysis/configs/general/SystNames.txt").Data());
	texTabler table(" l | c | c | c ");
	table << "Name" << "Pull" << "Constr / $\\sigma$" << "Contribution [\\%]";
	table << texLine();
	for(size_t i=0;i<getNParameters();i++){
		if(i==xsecidx7_) continue;
		if(i==xsecidx8_) continue;
		TString name=getParaName(i);
		table << fmt.translateName( getParaName(i) )<< fmt.round( fitter_.getParameters()->at(i), 0.01)
								<< fmt.round(getParaError(i),0.01) << fmt.round(100*getParaErrorContributionToXsec(i,xsecidx),0.01);

	}
	//add total
	table << texLine();
	double xsecoffset=xsecoff8_;
	if(xsecidx==xsecidx7_)xsecoffset=xsecoff7_;
	table << "Total" << " " << " " << fmt.round(100*getParaErrorContributionToXsec(xsecidx,xsecidx),0.01);
	table << texLine(2);
	table <<"$\\sigma_{t \\bar{t}}$" << " " << " " << fmt.toTString(fmt.round((fitter_.getParameter(xsecidx) + xsecoffset),0.1))+" pb";
	table << texLine();
	return table;

}

// can not be const due to root limitations
// do size checks before!
double ttbarXsecFitter::toBeMinimized(const double * variations){
	double out=0;
	for(size_t nbjet=0;nbjet<signalshape_nbjet_.size();nbjet++){
		if(exclude0bjetbin_){
			if(nbjet==0) continue;
			if(nbjet==last8TeVentry_+1) continue; //7TeV
		}
		//make sure its normalized
		//the influence usually is <0.001%, but still more correct that way
		double shapeintegral=signalshape_nbjet_.at(nbjet).getIntegral(variations);

		double bjetcat_norm= normalization_nbjet_.at(nbjet).getValue(variations);
		for(size_t bin=0;bin<signalshape_nbjet_.at(nbjet).getNBins();bin++){

			double norm    = lumi8_ * (variations[xsecidx8_] +xsecoff8_) ;
			if(nbjet>last8TeVentry_)
				norm    = lumi7_ * (variations[xsecidx7_] + xsecoff7_);
			double signal=norm* signalshape_nbjet_.at(nbjet).getBin(bin)->getValue(variations) / shapeintegral;
			if(norm_nbjet_global_)
				signal*=bjetcat_norm;

			double nbackground = background_nbjet_.at(nbjet).getBin(bin)->getValue(variations);

			double backgroundstat=background_nbjet_.at(nbjet).getBinErr(bin);

			double data = data_nbjet_.at(nbjet).getBin(bin)->getValue(variations);

			if(data<=0) continue;

			double predicted = signal+nbackground;

			if(lhmode_ == lhm_poissondatastat)   out+=-2*logPoisson(data, format_.round(predicted,1)); //doesn't (yet) work

			else if(lhmode_ ==lhm_chi2datastat)   out+= (data-predicted)*(data-predicted) / (data); //chi2 approach
			else if(lhmode_ ==lhm_chi2datamcstat)	out+= (data-predicted)*(data-predicted) / (data + backgroundstat*backgroundstat); //chi2 approach with bg errors
		}
	}

	//constraints
	for(size_t sys=0;sys<ndependencies_;sys++){

		if(sys == xsecidx8_) continue;
		if(sys == xsecidx7_){
			if(last8TeVentry_ < signalshape_nbjet_.size()-1){ //7 TeV is also included
				continue;
			} //else just constrain it to its starting value
		}
		if(sys == lumiidx8_ || sys == lumiidx7_) {
			//here you could do something special
		}
		//put else here if lumi gets treated differently
		if(priors_[sys] == prior_gauss){
			out+= simpleFitter::nuisanceGaus(variations[sys]);}
		else if(priors_[sys] == prior_box){
			out+= simpleFitter::nuisanceBox(variations[sys]);
		}
		else if(priors_[sys] == prior_float){
			//don't do anything
		}
		else if(priors_[sys] == prior_narrowboxleft){
			double var=variations[sys];
			if(var>0) var+=1;
			out+= simpleFitter::nuisanceBox(var);
		}
		else if(priors_[sys] == prior_narrowboxright){
			double var=variations[sys];
			if(var<0) var-=1;
			out+= simpleFitter::nuisanceBox(var);
		}
		else if(priors_[sys] == prior_parameterfixed){
			double deltafixed=variations[sys]-fitter_.getParameter(sys);
			out+=fabs(deltafixed)*1e5; //make it a bit smooth
		}

	}


	return out;
}

//the whole function is obsolete once the class works as expected!
void ttbarXsecFitter::checkSizes()const{
	if(debug)
		std::cout << "ttbarXsecFitter::checkSizes" <<std::endl;
	if(signalshape_nbjet_.size() < 1){
		throw std::logic_error("ttbarXsecFitter::checkSizes: no input");
	}
	if(signalshape_nbjet_.size() != background_nbjet_.size())
		throw std::logic_error("ttbarXsecFitter::checkSizes: leptonacceptance_nbjet_.size() != background_nbjet_.size()");
	if(signalshape_nbjet_.size() != data_nbjet_.size())
		throw std::logic_error("ttbarXsecFitter::checkSizes: leptonacceptance_nbjet_.size() != data_nbjet.size()");


	for(size_t i=0;i<signalshape_nbjet_.size() ;i++){
		if(signalshape_nbjet_.at(i).getBins() != background_nbjet_.at(i).getBins())
			throw std::logic_error("leptonacceptance_nbjet_.at(i).getBins() != background_nbjet_.at(i).getBins()");
		if(signalshape_nbjet_.at(i).getBins() != data_nbjet_.at(i).getBins())
			throw std::logic_error("leptonacceptance_nbjet_.at(i).getBins() != data_nbjet.at(i).getBins()");
	}


}//ttbarXsecFitter::checkSizes()


variateContainer1D ttbarXsecFitter::createLeptonJetAcceptance(const std::vector<containerStack> *stack, size_t bjetcategory, bool eighttev){
	// this one can use container1D operators!
	if(debug)
		std::cout << "ttbarXsecFitter::createLeptonJetAcceptance" <<std::endl;

	if(stack->size()<1)
		throw std::logic_error("ttbarXsecFitter::createLeptonJetAcceptance  stack vector empty.");

	size_t bjetbin=bjetcategory;
	size_t minbjetbin=0;
	size_t maxbjetbin=3;


	float gennorm = 1/(lumi8_*xsecoff8_);
	if(!eighttev){
		gennorm = 1/(lumi7_*xsecoff7_);
		bjetbin += last8TeVentry_ +1;
		minbjetbin = last8TeVentry_ +1;
		maxbjetbin = stack->size();
	}
	//unused	size_t bjet0bin=minbjetbin;
	size_t bjet1bin=minbjetbin+1;
	size_t bjet2bin=minbjetbin+2;

	container1D signal=stack->at(bjetbin).getSignalContainer();
	if(norm_nbjet_global_) signal=signal.getIntegralBin();
	container1D signalintegral=stack->at(minbjetbin).getSignalContainer();
	if(norm_nbjet_global_) signalintegral=signalintegral.getIntegralBin();
	for(size_t i=minbjetbin+1;i<maxbjetbin;i++){
		container1D tmpsig=stack->at(i).getSignalContainer();
		if(norm_nbjet_global_) tmpsig=tmpsig.getIntegralBin();
		signalintegral += tmpsig;
	}

	//need signal integral per bin -> same distributions for fixed add jet multi...

	histoContent::multiplyStatCorrelated = true;
	histoContent::addStatCorrelated = true;
	histoContent::divideStatCorrelated = true;

	container1D one=signal.createOne();
	container1D leptonacceptance = signalintegral * gennorm; //this includes lumi unc but cancels the xsec and lumi abs value!
	container1D onebjetsignal = stack->at(bjet1bin).getSignalContainer();
	if(norm_nbjet_global_) onebjetsignal=onebjetsignal.getIntegralBin();
	container1D twobjetsignal = stack->at(bjet2bin).getSignalContainer();
	if(norm_nbjet_global_) twobjetsignal=twobjetsignal.getIntegralBin();

	container1D correction_b =  ((signalintegral * twobjetsignal) * 4.)
																				/ ( (onebjetsignal + (twobjetsignal * 2.)) * (onebjetsignal + (twobjetsignal * 2.)));

	correction_b.removeStatFromAll();

	if(eighttev)
		container_c_b_.at(0).import(correction_b);
	else
		container_c_b_.at(1).import(correction_b);

	container1D eps_b = twobjetsignal / (correction_b * signalintegral);
	eps_b.sqrt();
	eps_b.removeStatFromAll();

	if(eighttev)
		container_eps_b_.at(0).import(eps_b);
	else
		container_eps_b_.at(1).import(eps_b);

	container1D lepjetcont1bjet=     (eps_b * 2.  - (correction_b * (eps_b * eps_b)) * 2.);
	lepjetcont1bjet.removeStatFromAll();
	container1D lepjetcont2bjets=    (correction_b * eps_b) * eps_b ;
	lepjetcont2bjets.removeStatFromAll();
	container1D lepjetcont0bjet=   leptonacceptance* (one - lepjetcont1bjet - lepjetcont2bjets);
	lepjetcont0bjet.removeStatFromAll();
	lepjetcont1bjet *= leptonacceptance;
	lepjetcont1bjet.removeStatFromAll();
	lepjetcont2bjets*= leptonacceptance;
	lepjetcont2bjets.removeStatFromAll();


	//the stat uncertainties will be screwed up, but they are not used anyway

	variateContainer1D out;
	if(norm_nbjet_global_){
		// disentangle shape and global norm here
		container1D normedsignal = stack->at(bjetbin).getSignalContainer(); //for shape
		normedsignal.normalize(true,true); //normalize to 1 incl syst
		out.import(normedsignal);

		//	TCanvas c;
		//	normedsignal.drawFullPlot();
		//	c.Print("test"+toTString(bjetbin) +".pdf");

		if(normalization_nbjet_.size() <= bjetbin)
			normalization_nbjet_.resize(bjetbin+1);

		variateContainer1D tmpvarc;
		if(bjetcategory == 0){
			tmpvarc.import(lepjetcont0bjet);
		}
		else if(bjetcategory == 1){
			tmpvarc.import(lepjetcont1bjet);
		}
		else if(bjetcategory == 2){
			tmpvarc.import(lepjetcont2bjets);
		}
		normalization_nbjet_.at(bjetbin) = *tmpvarc.getBin(1);
	}
	else{
		if(bjetcategory == 0){
			out.import(lepjetcont0bjet);
		}
		else if(bjetcategory == 1){
			out.import(lepjetcont1bjet);
		}
		else if(bjetcategory == 2){
			out.import(lepjetcont2bjets);
		}

	}
	return out;
}


std::vector<containerStack>   ttbarXsecFitter::readStacks(const std::string configfilename, const TString energy)const{
	if(debug)
		std::cout << "ttbarXsecFitter::readStacks" <<std::endl;
	if(energy != "8" && energy != "7")
		throw std::logic_error("ttbarXsecFitter::readStacks: energy should be 8 or 7 (String number)");

	bool eighttev= energy == "8";

	fileReader fr;
	fr.setComment("#");
	fr.setDelimiter("|");
	//8 TeV
	fr.setStartMarker(("[ "+energy+ " TeV ]").Data());
	fr.setEndMarker(("[ end - "+energy+ " TeV ]").Data());
	fr.readFile(configfilename);
	std::vector<containerStack>   out;
	containerStack stack;
	std::cout << "reading and preparing " << energy << " TeV stacks..." <<std::endl;
	size_t bjetcount=0,jetcount=0,oldbjetcount=0;
	for(size_t line=0;line<fr.nLines();line++){
		displayStatusBar(line,fr.nLines());
		if(fr.nEntries(line) < 1) continue;
		if(fr.getData<TString>(line,0) == "bjet++"){
			bjetcount++;
			jetcount=0;
			continue;
		}
		if(fr.nEntries(line) < 2 || ! fr.nEntries(line) % 2){
			std::cout << "ttbarXsecFitter::readInput: Line format wrong:\n" << fr.getReJoinedLine(line) << std::endl;
		}
		if(oldbjetcount!=bjetcount){
			out.push_back(stack);
			oldbjetcount=bjetcount;
			stack = containerStack(); //clear
		}

		for(size_t entry=0;entry<fr.nEntries(line);entry++){
			TString filename=fr.getData<TString>(line,entry);
			entry++;
			TString plotname=fr.getData<TString>(line,entry);
			containerStackVector csv;
			csv.loadFromTFile(filename);
			//csv.listStacks();
			containerStack tmpstack;
			try{
				tmpstack=csv.getStack(plotname);
			}
			catch(std::exception &ex){
				csv.listStacks();
				ex.what();
				throw std::runtime_error("stack not found");
			}
			addUncertainties(&tmpstack,bjetcount,eighttev);

			for(size_t i=0;i<out.size();i++){
				tmpstack.equalizeSystematicsIdxs(out.at(i));
			}
			stack=stack.append(tmpstack);
			//	std::cout << "ttbarXsecFitter::readStacks: added "
			//			<< " stack(s) for nbjets: " << bjetcount << " njets: " << jetcount <<std::endl;
		}
		jetcount++;
	}
	std::cout << std::endl;
	if(out.size()>0)
		out.push_back(stack);//last push back
	std::cout << out.size() << std::endl;
	if(out.size()<1)
		std::cout << "ttbarXsecFitter::readStacks: no plots for " << energy << " TeV added." <<std::endl;
	else if(out.size() != 3)
		throw std::runtime_error("ttbarXsecFitter::readStacks: tried to add less or more than (allowed) 3 nbjets categories");

	return out;

}


void ttbarXsecFitter::addUncertainties(containerStack * stack,size_t nbjets,bool eighttev)const{ //can be more sophisticated
	if(debug)
		std::cout << "ttbarXsecFitter::addUncertainties" <<std::endl;

	std::vector<TString> excludefromglobalvar;
	excludefromglobalvar.push_back("DY#rightarrow#tau#tau");
	excludefromglobalvar.push_back("DY#rightarrowll");
	excludefromglobalvar.push_back("Z#rightarrowll");

	stack->mergeLegends(excludefromglobalvar,"DY",432,false);

	if(debug)
		std::cout << "ttbarXsecFitter::addUncertainties: merged DY" <<std::endl;


	stack->mergeVariationsFromFileInCMSSW("/src/TtZAnalysis/Analysis/configs/general/SystNames.txt");

	if(debug)
		std::cout << "ttbarXsecFitter::addUncertainties: merged variations" <<std::endl;

	stack->addRelErrorToBackgrounds(0.3,false,"BG","DY"); //more sophisticated approaches can be chosen here - like DY float etc

	float dy0bjetserr=0,dy1bjetserr=0,dy2bjetserr=0;
	if(nbjets==0)
		dy0bjetserr=0.3;
	else if(nbjets==1)
		dy1bjetserr=0.3;
	else if(nbjets==2)
		dy2bjetserr=0.3;

	stack->addRelErrorToContribution(dy0bjetserr,"DY","BG_0_bjets");
	stack->addRelErrorToContribution(dy1bjetserr,"DY","BG_1_bjets");
	stack->addRelErrorToContribution(dy2bjetserr,"DY","BG_2_bjets");

	float addlumiunc8=0;
	float addlumiunc7=0;

	if(eighttev)
		addlumiunc8=unclumi8_/100;
	else
		addlumiunc7=unclumi7_/100;

	stack->addGlobalRelMCError("Lumi8",addlumiunc8);
	stack->addGlobalRelMCError("Lumi7",addlumiunc7);


	///fake uncertainty
	stack->addGlobalRelMCError("Xsec8",0);
	stack->addGlobalRelMCError("Xsec7",0);
	if(debug)
		std::cout << "ttbarXsecFitter::addUncertainties: done" <<std::endl;



}

}
