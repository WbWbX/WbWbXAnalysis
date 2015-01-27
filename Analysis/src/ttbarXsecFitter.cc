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
#include "limits.h"

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
			for(size_t j=0;j<TeV7Stacks.size();j++){
				TeV8Stacks.at(i).equalizeSystematicsIdxs(TeV7Stacks.at(j));
			}
		}
	}
	//syst indicies are all the same. possibly zeros have been added

	//just push back the 7 TeV entries in nbjets after the 8 TeV ones and set last8TeVentry_
	if(TeV7Stacks.size()>0)
		last8TeVentry_=TeV8Stacks.size()-1;
	TeV8Stacks.insert(TeV8Stacks.end(),TeV7Stacks.begin(),TeV7Stacks.end());

	//for debugging//
	plotterControlPlot pl;
	TCanvas c;
	pl.usePad(&c);

	signalconts_nbjets_.resize(TeV8Stacks.size());
	backgroundconts_nbjets_.resize(TeV8Stacks.size());
	dataconts_nbjets_.resize(TeV8Stacks.size());

	//create temps:
	bool eighttev=true;
	for(size_t it=0;it<TeV8Stacks.size();it++){ //print what you got
		if(it == last8TeVentry_+1){
			eighttev=false;
		}
		pl.setStack(&TeV8Stacks.at(it));
		pl.draw();
		TString name=TeV8Stacks.at(it).getName()+" "+toTString(it);
		if(eighttev) name+="8TeV.pdf";
		else name+="7TeV.pdf";
		name.ReplaceAll(" ","_");
		c.Print(name);

		signalconts_nbjets_.at(it) = TeV8Stacks.at(it).getSignalContainer();
		backgroundconts_nbjets_.at(it) = TeV8Stacks.at(it).getBackgroundContainer();
		dataconts_nbjets_.at(it)=TeV8Stacks.at(it).getDataContainer();

	}

	//done with input
	//set some starting paras:
	std::vector<TString> systs=signalconts_nbjets_.at(0).getSystNameList();
	lumiidx8_=std::find(systs.begin(),systs.end(),"Lumi8") - systs.begin();
	lumiidx7_=std::find(systs.begin(),systs.end(),"Lumi7") - systs.begin();
	xsecidx8_=std::find(systs.begin(),systs.end(),"Xsec8") - systs.begin();
	xsecidx7_=std::find(systs.begin(),systs.end(),"Xsec7") - systs.begin();

	ndependencies_=systs.size();

	fittedparas_.resize(ndependencies_,0);
	priors_.resize(ndependencies_,prior_gauss);
	parameternames_=systs;

	if(debug){
		std::cout << "ttbarXsecFitter::readInput: done. Combined " << TeV8Stacks.size() << " stacks.\n";
		std::cout << "7 TeV Stacks starting at >" << last8TeVentry_<<std::endl;
	}
}

void ttbarXsecFitter::createPseudoDataFromMC(container1D::pseudodatamodes mode){
	if(!random_){ //first call
		random_ = new TRandom3();
		datacontsorig_nbjets_=dataconts_nbjets_; //safe originals
		backgroundcontsorig_nbjets_=backgroundconts_nbjets_; //safe originals
		signalcontsorig_nbjets_=signalconts_nbjets_; //safe originals

		//there are no syst anymore (only dummies)
		for(size_t i=0;i<priors_.size();i++){
			if(i!=xsecidx7_ && i != xsecidx8_)
				setPrior(parameternames_.at(i),prior_parameterfixed);
		}

		//minimize output
		std::cout << "\nEntering pseudo-experiments mode\n" <<std::endl;
		if(!debug)simpleFitter::printlevel=-1;
		//debug=false;
	}
	for(size_t i=0;i<dataconts_nbjets_.size();i++){
		container1D tmpmc=backgroundcontsorig_nbjets_.at(i) + signalcontsorig_nbjets_.at(i);
		dataconts_nbjets_.at(i)=tmpmc.createPseudoExperiment(random_,&datacontsorig_nbjets_.at(i),mode);
		backgroundconts_nbjets_.at(i)=backgroundcontsorig_nbjets_.at(i).createPseudoExperiment(random_,0,mode);
		signalconts_nbjets_.at(i)=signalcontsorig_nbjets_.at(i).createPseudoExperiment(random_,0,mode);
	}
}


void ttbarXsecFitter::createContinuousDependencies(){
	if(debug)
		std::cout << "ttbarXsecFitter::createContinuousDependencies" <<std::endl;
	if(last8TeVentry_<100 && lumi7_ <1){
		throw std::logic_error("ttbarXsecFitter::createContinuousDependencies: using 7 TeV data but 7 TeV lumi not set!");
	}
	size_t bjetcount=0;
	bool eighttev=true;
	signalshape_nbjet_.clear();
	data_nbjet_.clear();
	background_nbjet_.clear();
	for(size_t it=0;it<signalconts_nbjets_.size();it++){
		if(it == last8TeVentry_+1){
			bjetcount=0;
			eighttev=false;
			if(debug)
				std::cout << "ttbarXsecFitter::createContinuousDependencies: started 7 TeV indicies" <<std::endl;
		}

		signalshape_nbjet_.push_back(createLeptonJetAcceptance(signalconts_nbjets_,bjetcount,eighttev));

		container1D temp=dataconts_nbjets_.at(it);
		if(useMConly_){
			temp=signalconts_nbjets_.at(it) + backgroundconts_nbjets_.at(it);
			temp.setAllErrorsZero(false);
		}

		variateContainer1D tmpvarc;
		tmpvarc.import(temp);
		data_nbjet_.push_back(tmpvarc);

		tmpvarc.import(backgroundconts_nbjets_.at(it));
		background_nbjet_.push_back(tmpvarc);

		bjetcount++;
	}
}

bool ttbarXsecFitter::includes7TeV()const{
	if(last8TeVentry_ < signalshape_nbjet_.size()-1){ //7 TeV is also included
		return true;
	} //else just constrain it to its starting value
	return false;
}

void ttbarXsecFitter::setPrior(const TString& sysname, priors prior){
	if(debug)
		std::cout << "ttbarXsecFitter::setPrior" <<std::endl;

	if(signalconts_nbjets_.size()<1){
		throw std::logic_error("ttbarXsecFitter::setPrior: first read in plots");
	}
	std::vector<TString> systs=signalconts_nbjets_.at(0).getSystNameList();
	size_t idx=std::find(systs.begin(),systs.end(),sysname) - systs.begin();
	if(idx == systs.size())
		throw std::out_of_range("ttbarXsecFitter::setPrior: requested variation not found");

	priors_.at(idx) = prior;


}
void ttbarXsecFitter::addFullExtrapolError(const TString& sysname){
	if(debug)
		std::cout << "ttbarXsecFitter::addFullError" <<std::endl;

	if(signalconts_nbjets_.size()<1){
		throw std::logic_error("ttbarXsecFitter::setPrior: first read in plots");
	}
	std::vector<TString> systs=signalconts_nbjets_.at(0).getSystNameList();
	size_t idx=std::find(systs.begin(),systs.end(),sysname) - systs.begin();
	if(idx == systs.size())
		throw std::out_of_range("ttbarXsecFitter::addFullError: requested variation not found");
	if(std::find(addfullerrors_.begin(),addfullerrors_.end(),idx) == addfullerrors_.end()){//new
		addfullerrors_.push_back(idx);
	}


}

containerStack ttbarXsecFitter::produceStack(bool fittedvalues,size_t bjetcat,bool eighttev,double& chi2)const{
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


	if(debug)
		std::cout << "ttbarXsecFitter::produceStack: data: "<< data_nbjet_.at(nbjet).getNDependencies() << std::endl;
	data=data_nbjet_.at(nbjet).exportContainer( fittedparas );

	if(debug)
		std::cout << "ttbarXsecFitter::produceStack: background" << std::endl;
	background=background_nbjet_.at(nbjet).exportContainer( fittedparas );

	if(debug)
		std::cout << "ttbarXsecFitter::produceStack: normalization" << std::endl;
	double multi = lumi8_ * (fittedparas[xsecidx8_] + xsecoff8_);
	if(nbjet>last8TeVentry_)
		multi = lumi7_ * (fittedparas[xsecidx7_]+ xsecoff7_);
	if(norm_nbjet_global_)
		multi*=normalization_nbjet_.at(nbjet).getValue(fittedparas);

	if(debug)
		std::cout << "ttbarXsecFitter::produceStack: normalization (2)" << std::endl;
	multi/=signalshape_nbjet_.at(nbjet).getIntegral(&fittedparas[0]);

	if(debug)
		std::cout << "ttbarXsecFitter::produceStack: signalshape" << std::endl;
	container1D exportc=signalshape_nbjet_.at(nbjet).exportContainer( fittedparas );
	signal=  exportc * multi ;


	out.push_back(data,"data",1,1,99);
	out.push_back(background,"background",601,1,2);
	out.push_back(signal,"signal",633,1,1);

	chi2=out.chi2();

	return out;
}


int ttbarXsecFitter::fit(float& xsec8,float&errup8,float&errdown8,float& xsec7,float&errup7,float&errdown7){
	if(debug)
		std::cout << "ttbarXsecFitter::fit" <<std::endl;
	if(signalshape_nbjet_.size()<1)
		createContinuousDependencies();
	checkSizes();
	//no except: sizes ok
	//	fittedparas_.at(xsecidx8_)=-8.; //other starting value closer to measured value TEST

	fitter_.setParameterNames(parameternames_);
	fitter_.setRequireFitFunction(false);
	std::vector<double> stepwidths;
	stepwidths.resize(fittedparas_.size(),0.1);
	stepwidths.at(xsecidx8_)=1;
	stepwidths.at(xsecidx7_)=1;


	fitter_.setParameters(fittedparas_,stepwidths);
	//set fixed paras
	for(size_t i=0;i<priors_.size();i++){
		if(priors_.at(i) == prior_parameterfixed)
			fitter_.setParameterFixed(i,true);
		else
			fitter_.setParameterFixed(i,false);
	}
	fitter_.setMinimizer(simpleFitter::mm_minuit2);
	if(fittedparas_.size() != ndependencies_)
		throw std::logic_error("ttbarXsecFitter::fit: fittedparas.size() != ndep");
	functor_ = ROOT::Math::Functor(this,&ttbarXsecFitter::toBeMinimized,ndependencies_);//ROOT::Math::Functor f(this,&ttbarXsecFitter::toBeMinimized,ndependencies_);
	fitter_.setMinFunction(&functor_);

	//fitter_.setParameterFixed(lumiidx7_);
	//if(includes7TeV())

	//pre fit to get rough estimate
	fitter_.setStrategy(0);
	fitter_.setTolerance(1);
	fitter_.fit();
	fitter_.feedErrorsToSteps();
	if(!silent_)
		std::cout << "First rough fit done" <<std::endl;
	fitter_.setStrategy(2);
	fitter_.setTolerance(0.01);
	if(!nominos_){
		fitter_.setAsMinosParameter(xsecidx7_);
		fitter_.setAsMinosParameter(xsecidx8_);
	}
	fitter_.fit();
	//feed back


	if(!silent_)
		std::cout << "fitted xsecs:\n7TeV: "<<fitter_.getParameters()->at(xsecidx7_)+xsecoff7_ <<
		"+" << fitter_.getParameterErrUp()->at(xsecidx7_) *100/(fitter_.getParameters()->at(xsecidx7_)+xsecoff7_) <<
		"-" << fitter_.getParameterErrDown()->at(xsecidx7_) *100/(fitter_.getParameters()->at(xsecidx7_)+xsecoff7_) <<
		"\n8TeV: "<< fitter_.getParameters()->at(xsecidx8_)+xsecoff8_ <<
		"+" << fitter_.getParameterErrUp()->at(xsecidx8_) *100/(fitter_.getParameters()->at(xsecidx8_)+xsecoff8_) <<
		"-" << fitter_.getParameterErrDown()->at(xsecidx8_) *100/(fitter_.getParameters()->at(xsecidx8_)+xsecoff8_) <<
		std::endl;
	if(fitter_.wasSuccess()){
		fitsucc_=true;
		if(parameterwriteback_){
			fittedparas_ = *fitter_.getParameters();
		}
		xsec8=fittedparas_.at(xsecidx8_);
		errup8=fitter_.getParameterErrUp()->at(xsecidx8_);
		errdown8=fitter_.getParameterErrUp()->at(xsecidx8_);
		xsec7=fittedparas_.at(xsecidx7_);
		errup7=fitter_.getParameterErrUp()->at(xsecidx7_);
		errdown7=fitter_.getParameterErrUp()->at(xsecidx7_);

		if(!nominos_ && !fitter_.minosWasSuccess()){
			throw std::runtime_error("ttbarXsecFitter::fit(): Minos error failed!");
		}

		return 0;
	}
	//else
	if(!silent_)
		std::cout << "ttbarXsecFitter::fit(): Fit failed" <<std::endl;
	throw std::runtime_error("ttbarXsecFitter::fit(): Fit failed");
	return -1;
}
int ttbarXsecFitter::fit(){
	float dummy;
	return fit(dummy,dummy,dummy,dummy,dummy,dummy);
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
void ttbarXsecFitter::getParaErrorContributionToXsec(int idxin, double sevenoreight,double&up,double&down,bool& anticorr){
	if(idxin>=(int)fitter_.getParameters()->size())
		throw std::out_of_range("ttbarXsecFitter::getParaErrorContributionToXsec: index out of range");

	size_t xsecidx=getXsecIdx(sevenoreight);
	double xsecoffset=xsecoff8_;
	if(xsecidx==xsecidx7_)xsecoffset=xsecoff7_;
	double xsec=fitter_.getParameter(xsecidx)+xsecoffset;
	if(!nosystbd_){
		if(idxin>=0){

			size_t idx=idxin;
			anticorr=fitter_.getCorrelationCoefficient(xsecidx,idx)<0;

			fitter_.getParameterErrorContribution(idx,xsecidx,up,down);
			up/=xsec;
			down/=xsec;
			return;
		}
		else{ //stat
			fitter_.getStatErrorContribution(xsecidx,up,down);
			up/=xsec;
			down/=xsec;
			return;
		}
	}
	else{
		up=1;
		down=1;
		return;
	}
}
size_t ttbarXsecFitter::getXsecIdx(double sevenoreight)const{
	size_t xsecidx=xsecidx8_;
	if(isApprox(sevenoreight,7.))
		xsecidx=xsecidx7_;
	return xsecidx;
}

texTabler ttbarXsecFitter::makeSystBreakdown(double sevenoreight){
	size_t xsecidx=getXsecIdx(sevenoreight);
	formatter fmt;
	bool eighttev=true;
	TString cmsswbase=getenv("CMSSW_BASE");
	fmt.readInNameTranslateFile((cmsswbase+"/src/TtZAnalysis/Analysis/configs/general/SystNames.txt").Data());
	texTabler table(" l | c | c | c ");
	table << "Name" << "Pull" << "Constr / $\\sigma$" << "Contribution [\\%]";
	table << texLine();
	if(xsecidx==xsecidx7_){
		std::cout << "Evaluating systematic uncertainties for 7 TeV (breakdown)... " <<std::endl;
		eighttev=false;
	}
	if(xsecidx==xsecidx8_)
		std::cout << "Evaluating systematic uncertainties for 8 TeV (breakdown)... " <<std::endl;
	for(size_t i=0;i<getNParameters();i++){
		if(i==xsecidx7_) continue;
		if(i==xsecidx8_) continue;
		TString name=fmt.translateName( getParaName(i));
		double errup,errdown;
		bool anitcorr;
		getParaErrorContributionToXsec(i,sevenoreight,errup,errdown,anitcorr);
		TString errstr;
		if(anitcorr)
			errstr="$\\mp{"+fmt.toTString(fmt.round(100*errdown,0.1))+"}$";
		else
			errstr="$\\pm{"+fmt.toTString(fmt.round(100*errup,0.1))+ "}$";


		if(std::find(addfullerrors_.begin(),addfullerrors_.end(),i)!=addfullerrors_.end())
			name+=" (vis)";
		std::cout <<name<< " +" << errup*100 << "% -" << errdown*100 << "% "<<std::endl;

		table << name << fmt.round( fitter_.getParameters()->at(i), 0.1)<<
				fmt.round( fitter_.getParameterErr(i), 0.1)	<< errstr;

	}
	table <<texLine(1);
	///add extrapolation in addition:
	double addtoerrup2=0,addtoerrdown2=0;
	for(size_t i=0;i<addfullerrors_.size();i++){
		bool anticorr=false;
		size_t idx=addfullerrors_.at(i);
		TString name=fmt.translateName( getParaName(idx) );
		name+=" (extr)";
		extendedVariable * var=&eps_emu_.at(0);
		if(!eighttev)
			var=&eps_emu_.at(1);
		std::vector<double> paracopy=fittedparas_;
		float nom=var->getValue(paracopy);
		if(i==0)
			std::cout << "---->> Epsilon emu (%): " << nom*100 << std::endl;
		paracopy.at(idx)=1;
		float up=var->getValue(paracopy);
		paracopy.at(idx)=-1;
		float down=var->getValue(paracopy);
		if(nom==0){
			if(up!=0 || down!=0)
				throw std::runtime_error("ttbarXsecFitter::makeSystBreakdown: variable is zero but variations not");
		}
		float relup=(up-nom)/nom;
		float reldown=(down-nom)/nom;
		if(relup>0){
			addtoerrup2+=relup*relup;
			addtoerrdown2+=reldown*reldown;
		}
		else{
			addtoerrdown2+=relup*relup;
			addtoerrup2+=reldown*reldown;
			anticorr=true;
		}
		TString errstr;
		if(anticorr)
			errstr="$\\mp^{"+fmt.toTString(fmt.round(100*sqrt(addtoerrdown2),0.1))
			+"}_{"+fmt.toTString(fmt.round(100*sqrt(addtoerrup2),0.1)) +"}$";
		else
			errstr="$\\pm^{"+fmt.toTString(fmt.round(100*sqrt(addtoerrup2),0.1))
			+"}_{"+fmt.toTString(fmt.round(100*sqrt(addtoerrdown2),0.1)) +"}$";

		table << name << "-" <<
				"-"	<< errstr;
	}

	//add stat
	double errupstat,errdownstat;
	bool anticorr=false;//dummy
	getParaErrorContributionToXsec(-1,sevenoreight,errupstat,errdownstat,anticorr);
	TString errstrstat="$\\pm{"+fmt.toTString(fmt.round(100*errdownstat,0.1))+ "}$";
	table << texLine();
	table << "Stat" << " " << " " << errstrstat;

	//add total
	table << texLine();
	double xsecoffset=xsecoff8_;
	if(xsecidx==xsecidx7_)xsecoffset=xsecoff7_;

	double fullrelxsecerrup=fitter_.getParameterErrUp()->at(xsecidx)/(fitter_.getParameter(xsecidx) + xsecoffset);
	fullrelxsecerrup=sqrt(fullrelxsecerrup*fullrelxsecerrup+addtoerrup2);
	double fullrelxsecerrdown=fitter_.getParameterErrDown()->at(xsecidx)/(fitter_.getParameter(xsecidx) + xsecoffset);
	fullrelxsecerrdown=sqrt(fullrelxsecerrdown*fullrelxsecerrdown+addtoerrdown2);

	TString errstrxsec="$\\pm^{"+ fmt.toTString(fmt.round(100*fullrelxsecerrup,0.1)) +
			"}_{"+ fmt.toTString(fmt.round(100*fullrelxsecerrdown,0.1))+"}$";
	table << "Total" << " " << " "
			<< errstrxsec;
	table << texLine(2);
	table <<"$\\sigma_{t \\bar{t}}$" << " " << " " << fmt.toTString(fmt.round((fitter_.getParameter(xsecidx) + xsecoffset),0.1))+" pb";
	table << texLine();
	return table;

}

texTabler ttbarXsecFitter::makeCorrTable() const{
	TString format=" l ";
	container2D corr=getCorrelations();
	for(size_t i=1;i<corr.getBinsX().size()-1;i++)
		format+=" | c ";
	texTabler tab(format);

	formatter Formatter;
	TString cmsswbase=getenv("CMSSW_BASE");
	Formatter.readInNameTranslateFile((cmsswbase+"/src/TtZAnalysis/Analysis/configs/general/SystNames.txt").Data());
	for(size_t j=0;j<corr.getBinsY().size()-1;j++){
		for(size_t i=0;i<corr.getBinsX().size()-1;i++){
			//names
			if(i && !j)
				tab << "";//Formatter.translateName(parameternames_.at(i-1));
			else if(j && !i)
				tab << Formatter.translateName(parameternames_.at(j-1));
			else if (!j && !i)
				tab << "";
			else if(i<=j){
				float content=corr.getBinContent(i,j);
				if(fabs(content)<0.3)
					tab << Formatter.round(corr.getBinContent(i,j),0.01);
				else
					tab <<  "\\textbf{" +Formatter.toTString(Formatter.round(corr.getBinContent(i,j),0.01)) +"}";
			}
			else
				tab << "";
		}
		tab<<texLine(); //row done
	}

	return tab;
}

// can not be const due to root limitations
// do size checks before!
double ttbarXsecFitter::toBeMinimized(const double * variations){
	static size_t ncalls=0;
	static double lastout=10000;
	ncalls++;
	if(variations[0]!=variations[0]){
		std::cout << "Exception in call "<<ncalls<<std::endl;
		throw std::runtime_error("ttbarXsecFitter::toBeMinimized: at least one nan input from minuit");
	}
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
			//std::cout << bin << "/"<< signalshape_nbjet_.at(nbjet).getNBins()<<" " << nbjet << " " << std::endl;
			double norm    = lumi8_ * (variations[xsecidx8_] +xsecoff8_) ;
			if(nbjet>last8TeVentry_)
				norm    = lumi7_ * (variations[xsecidx7_] + xsecoff7_);
			double signal=norm* signalshape_nbjet_.at(nbjet).getBin(bin)->getValue(variations) / shapeintegral;
			if(norm_nbjet_global_)
				signal*=bjetcat_norm;


			double nbackground = background_nbjet_.at(nbjet).getBin(bin)->getValue(variations);


			double backgroundstat=background_nbjet_.at(nbjet).getBinErr(bin);

			double data = data_nbjet_.at(nbjet).getBin(bin)->getValue(variations);
			//this is obsolete, strictly speaking, but safer in case somethin is normalized or weighted events are used
			double datastat = data_nbjet_.at(nbjet).getBinErr(bin);

			//this  might happen for some variations, fix it to physics values
			if(nbackground<0) nbackground=0;
			if(signal<0)signal=0;

			double predicted = signal+nbackground;

			//debug



			if(lhmode_ == lhm_poissondatastat)   {
				double roundpred=predicted;//format_.round(predicted,1);
				if(roundpred<0)roundpred=0;//safety
				if(data<0)continue; //safety
				out+=-2*logPoisson(data, roundpred); //unstable...
			}

			else if(lhmode_ ==lhm_chi2datastat)   {
				if(datastat<=0) continue;
				out+= (data-predicted)*(data-predicted) / (datastat*datastat); //chi2 approach
			}
			else if(lhmode_ ==lhm_chi2datamcstat)	{
				if((datastat*datastat + backgroundstat*backgroundstat)<=0) continue;
				out+= (data-predicted)*(data-predicted) / (datastat*datastat + backgroundstat*backgroundstat); //chi2 approach with bg errors
			}

			/* debug if a call fails			if(ncalls==284 && bin == 32){
				ZTOP_COUTVAR(bin);
				ZTOP_COUTVAR(nbjet);
				ZTOP_COUTVAR(bjetcat_norm);
				ZTOP_COUTVAR(data);
				ZTOP_COUTVAR(datastat);
				ZTOP_COUTVAR(nbackground);
				ZTOP_COUTVAR(backgroundstat);
				ZTOP_COUTVAR(signal);
				ZTOP_COUTVAR(predicted);
				ZTOP_COUTVAR(out);
				for(size_t i=0;i<parameternames_.size();i++)
					std::cout << parameternames_.at(i) << ":\t"<<variations[i]<<std::endl;
			}
			 */
		}
	}
	if(out==std::numeric_limits<double>::infinity()){
		out=lastout*10; //this is only supposed to happen for extreme variations during fit process->check pulls
	}
	lastout=out;
	//constraints
	for(size_t sys=0;sys<ndependencies_;sys++){

		if(sys == xsecidx8_) continue;
		if(sys == xsecidx7_){
			if(includes7TeV()) //7 TeV is also included
				continue;
			//else just constrain it to its starting value
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
		//the fitter will handle this case, delta should be zero in all cases
		//	else if(priors_[sys] == prior_parameterfixed){
		//		double deltafixed=variations[sys]-fitter_.getParameter(sys);
		//		out+=deltafixed*deltafixed*1e9; //make it a bit smooth
		//	}

	}
	if(out!=out){
		throw std::runtime_error("ttbarXsecFitter::toBeMinimized NAN");
	}
	if(out<DBL_EPSILON && out !=0){
		out=DBL_EPSILON;
		std::cout << "restricted out!" <<std::endl;
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
	if(signalshape_nbjet_.at(0).getNDependencies() != background_nbjet_.at(0).getNDependencies())
		throw  std::logic_error("ttbarXsecFitter::checkSizes: ndep broken");
	if(signalshape_nbjet_.at(0).getNDependencies() != data_nbjet_.at(0).getNDependencies())
		throw  std::logic_error("ttbarXsecFitter::checkSizes: ndep broken");
	if(signalshape_nbjet_.at(0).getNDependencies() != normalization_nbjet_.at(0).getNDependencies())
		throw  std::logic_error("ttbarXsecFitter::checkSizes: ndep broken");


}//ttbarXsecFitter::checkSizes()


variateContainer1D ttbarXsecFitter::createLeptonJetAcceptance(const std::vector<container1D>& signals, size_t bjetcategory, bool eighttev){
	// this one can use container1D operators!
	if(debug)
		std::cout << "ttbarXsecFitter::createLeptonJetAcceptance: "<< bjetcategory <<", "<< eighttev<<std::endl;

	if(signals.size()<1)
		throw std::logic_error("ttbarXsecFitter::createLeptonJetAcceptance  stack vector empty.");

	size_t bjetbin=bjetcategory;
	size_t minbjetbin=0;
	size_t maxbjetbin=3;


	float gennorm = 1/(lumi8_*xsecoff8_);
	if(!eighttev){
		gennorm = 1/(lumi7_*xsecoff7_);
		bjetbin += last8TeVentry_ +1;
		minbjetbin = last8TeVentry_ +1;
		maxbjetbin = signals.size();//stack->size();
		if(debug){
			std::cout << "ttbarXsecFitter::createLeptonJetAcceptance: 7TeV: set: " <<std::endl;
			std::cout << "bjetbin: " << bjetbin<<std::endl;
			std::cout << "minbjetbin: " << minbjetbin<<std::endl;
			std::cout << "maxbjetbin: " << maxbjetbin<<std::endl;
		}

	}
	//unused	size_t bjet0bin=minbjetbin;
	size_t bjet1bin=minbjetbin+1;
	size_t bjet2bin=minbjetbin+2;

	container1D signal=signals.at(bjetbin);//.getSignalContainer();
	if(norm_nbjet_global_) signal=signal.getIntegralBin();
	container1D signalintegral=signals.at(minbjetbin); //stack->at(minbjetbin).getSignalContainer();
	if(norm_nbjet_global_) signalintegral=signalintegral.getIntegralBin();
	for(size_t i=minbjetbin+1;i<maxbjetbin;i++){
		container1D tmpsig=signals.at(i); //stack->at(i).getSignalContainer();
		if(norm_nbjet_global_) tmpsig=tmpsig.getIntegralBin();
		signalintegral += tmpsig;
	}

	//need signal integral per bin -> same distributions for fixed add jet multi...

	histoContent::multiplyStatCorrelated = true;
	histoContent::addStatCorrelated = true;
	histoContent::divideStatCorrelated = true;

	container1D one=signal.createOne();
	container1D leptonacceptance = signalintegral * gennorm; //this includes lumi unc but cancels the xsec and lumi abs value!


	container1D onebjetsignal = signals.at(bjet1bin);//stack->at(bjet1bin).getSignalContainer();
	if(norm_nbjet_global_) onebjetsignal=onebjetsignal.getIntegralBin();
	container1D twobjetsignal = signals.at(bjet2bin);//stack->at(bjet2bin).getSignalContainer();
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

	//epsemu (extrapolation)
	variateContainer1D tmp;
	tmp.import(leptonacceptance);
	if(eighttev)
		eps_emu_.at(0)=*tmp.getBin(1);
	else
		eps_emu_.at(1)=*tmp.getBin(1);


	//the stat uncertainties will be screwed up, but they are not used anyway
	bool tmpvarcdebug=variateContainer1D::debug;
	//	if(debug)
	//		variateContainer1D::debug=true;
	variateContainer1D out;
	if(norm_nbjet_global_){
		// disentangle shape and global norm here
		container1D normedsignal = signals.at(bjetbin);//stack->at(bjetbin).getSignalContainer(); //for shape
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
	variateContainer1D::debug=tmpvarcdebug;
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
	TString oldfilename="";
	containerStackVector csv;
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
			if(replaceininfiles_.first.Length()>0)
				filename.ReplaceAll(replaceininfiles_.first,replaceininfiles_.second);
			entry++;
			TString plotname=fr.getData<TString>(line,entry);
			bool newcsv=oldfilename!=filename;
			oldfilename=filename;
			if(newcsv)
				csv.loadFromTFile(filename); //new file

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
			if(tmpstack.getContribution("data").isDummy())
				throw std::runtime_error("ttbarXsecFitter::readStacks: Stack has no data entry!");
			if(tmpstack.getSignalIdxs().size() <1)
				throw std::runtime_error("ttbarXsecFitter::readStacks: No signal defined!");

			addUncertainties(&tmpstack,bjetcount,eighttev);
			if(newcsv) //not necessary if same csv where everything is ordered in the same manner
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

	try{
		stack->mergeVariationsFromFileInCMSSW("/src/TtZAnalysis/Analysis/configs/fitTtBarXsec/combine_syst.txt");
	}
	catch(...){}
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

	//energy specific
	if(eighttev){
		addlumiunc8=unclumi8_/100;

	}
	else{
		addlumiunc7=unclumi7_/100;

	}

	if(removesyst_)
		stack->removeAllSystematics();

	stack->addGlobalRelMCError("Lumi8",addlumiunc8);
	stack->addGlobalRelMCError("Lumi7",addlumiunc7);

	///fake uncertainty
	stack->addGlobalRelMCError("Xsec8",0);
	stack->addGlobalRelMCError("Xsec7",0);
	if(debug)
		std::cout << "ttbarXsecFitter::addUncertainties: done" <<std::endl;



}

}
