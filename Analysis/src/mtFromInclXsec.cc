/*
 * mtFromInclXsec.cc
 *
 *  Created on: Mar 24, 2015
 *      Author: kiesej
 */



#include "../interface/mtFromInclXsec.h"
#include "TtZAnalysis/Tools/interface/formatter.h"
namespace ztop{

bool mtFromInclXsec::debug=false;

void mtFromInclXsec::readInFiles(const std::vector<std::string>& ins){
	std::cout << "reading..."<<std::endl;
	if(ins.size()<1)
		throw std::logic_error("mtFromInclXsec::readInFiles: input file list empty");
	exppoints_.clear();

	exppoints_.readFromFile(ins.at(0));
	for(size_t i=1;i<ins.size();i++){
		const std::string & infile=ins.at(i);
		graph tmpg;
		tmpg.readFromFile(infile);
		exppoints_.mergeWith(tmpg);
	}
	if(debug)
		for(size_t i=0;i<exppoints_.getNPoints();i++){
			std::cout << "added point: " << exppoints_.getPointXContent(i) << ',' << exppoints_.getPointYContent(i) <<std::endl;
		}
}

void mtFromInclXsec::scaleFitUnc(const float scale, const std::string& uncname){
	if(exppoints_.getNPoints()<3)
		throw std::logic_error("mtFromInclXsec::scaleFitUnc first read-in files");

	size_t idxup  =exppoints_.getSystErrorIndex(uncname+"_up");
	size_t idxdown=exppoints_.getSystErrorIndex(uncname+"_down");

	if(idxup>=exppoints_.getSystSize())
		throw std::out_of_range("mtFromInclXsec::scaleFitUnc: unc name not found");

	graph newexp=exppoints_.getNominalGraph();
	for(size_t i=0;i<exppoints_.getSystSize();i++){
		float weight=1;
		if(i==idxup || i==idxdown)
			weight = scale;
		newexp.addErrorGraph(exppoints_.getSystErrorName(i),exppoints_.getSystGraph(i),weight);
	}
	exppoints_=newexp;
}

graphFitter mtFromInclXsec::getMassDependence(const TString& syst)const{
	if(exppoints_.getNPoints() != 3)
		throw std::logic_error("mtFromInclXsec::getMassDependence: exactly 3 points needed!");
	size_t idxup,idxdown;

	graph newexp=exppoints_;
	newexp.removeAllXYSyst();
	idxup=exppoints_.getSystErrorIndex(envunc_+"_up");
	idxdown=exppoints_.getSystErrorIndex(envunc_+"_down");




	if(idxup>=exppoints_.getSystSize() || idxdown>=exppoints_.getSystSize() ){
		throw std::runtime_error("mtFromInclXsec::getMassDependence: envelope uncerainty not found");
	}



	graphFitter gf;
	//gf.setFitMode(graphFitter::fm_pol2);
	//graphFitter::debug=true;
	//simpleFitter::printlevel=2;

	gf.setFitMode(graphFitter::fm_exp);
	gf.setParameterUpperLimit(1,-100);
	gf.setParameterUpperLimit(2,0);
	gf.setParameter(0,170);
	//gf.setStrategy(2);

	graph fitgr;
	if(syst == "total_up"){
		fitgr=exppoints_;
		fitgr=fitgr.addAllSysQuad("tot");
		fitgr=fitgr.getSystGraph(fitgr.getSystErrorIndex("tot_up"));
	}
	else if(syst == "total_down"){
		fitgr=exppoints_;
		fitgr=fitgr.addAllSysQuad("tot");
		fitgr=fitgr.getSystGraph(fitgr.getSystErrorIndex("tot_down"));
	}
	else if(syst.Length()>0){
		fitgr=exppoints_.getSystGraph(exppoints_.getSystErrorIndex(syst));
	}
	else{
		fitgr=exppoints_;
	}
	fitgr.removeAllXYSyst();
	//fitgr.addErrorGraph(exppoints_.getSystErrorName(idxup),exppoints_.getSystGraph(idxup));
	//fitgr.addErrorGraph(exppoints_.getSystErrorName(idxdown),exppoints_.getSystGraph(idxdown));
	gf.readGraph(&fitgr);

	gf.setStrategy(0);
	gf.fit();
	gf.feedErrorsToSteps();
	gf.setStrategy(1);
	if(syst.Length()<1)
		std::cout << "fitted curve: xsec(m_t) = "
		<< "\\text{exp}\\left(" << gf.getParameter(2) << "\\cdot (m_t/\\GeV + " << gf.getParameter(1) <<" ) " << "\\right)+"<< gf.getParameter(0)<<std::endl;

	if(!gf.wasSuccess())
		throw std::runtime_error("mtFromInclXsec::getMassDependence: fit failed");
	return gf;
}

graph mtFromInclXsec::getMassDepGraphTheo(sys_enum sys, size_t npoints)const{
	graph out;

	const float mintopmass=165.5 ;//169;// 165.5;
	const float maxtopmass= 179.5 ;//178;// 179.5;
	if(sys==sys_nominal)
		out= predicted_.getDependence(mintopmass, maxtopmass, npoints, top_prediction::err_nominal);
	else if(sys==sys_totdown)
		out= predicted_.getDependence(mintopmass, maxtopmass, npoints, top_prediction::err_maxdown);
	else if(sys==sys_totup)
		out= predicted_.getDependence(mintopmass, maxtopmass, npoints, top_prediction::err_maxup);

	return out;
}

void  mtFromInclXsec::extract(){
	std::cout << "extract..."<<std::endl;
	//get nominal point
	float nommass=172.5;
	size_t nompoint=1000;
	for(size_t i=0;i<exppoints_.getNPoints();i++){
		if(exppoints_.getPointXContent(i) == nommass){
			std::cout << "nominal mass at " << i <<std::endl;
			nompoint=i;
			break;
		}
	}
	if(nompoint>999)
		throw std::runtime_error("mtFromInclXsec::extract: nominal mass (172.5) not found");

	float xsecerrup= exppoints_.getPointYErrorUp(nompoint,false); //exppoints_.getPointYContent(nompoint,idxup)-exppoints_.getPointYContent(nompoint);
	float xsecerrdown=exppoints_.getPointYErrorDown(nompoint,false);//exppoints_.getPointYContent(nompoint,idxdown)-exppoints_.getPointYContent(nompoint);
	float xsecnom=exppoints_.getPointYContent(nompoint);


	const float mintopmass=165.5 ;//169;// 165.5;
	const float maxtopmass= 179.5 ;//178;// 179.5;
	const float ndiv=500;
	const float minxsec= predicted_.getXsec(maxtopmass);
	const float maxxsec= predicted_.getXsec(mintopmass);
	//float scaleerrth,pdferrth;
	//getTtbarXsec(nommass,energy_,&scaleerrth,&pdferrth);
	//	predicted_.getXsec((maxtopmass+mintopmass)/2,top_prediction::err
	//	double tottheoerr=sqrt(scaleerrth*scaleerrth+pdferrth*pdferrth);


	std::vector<float> bins=histo1D::createBinning(ndiv,mintopmass,maxtopmass);
	std::vector<float> binsy=histo1D::createBinning(ndiv,minxsec,maxxsec);
	theolh_=  histo2D(bins,binsy,"","m_{t} [GeV]","#sigma_{t#bar{t}} [pb]","L_{pred}(m_{t},#sigma_{t#bar{t}})");
	explh_ =  histo2D(bins,binsy,"","m_{t} [GeV]","#sigma_{t#bar{t}} [pb]","L_{exp}(m_{t},#sigma_{t#bar{t}})");
	//jointlh_ =  histo2D(bins,binsy,"","m_{t} [GeV]","#sigma_{t#bar{t}} [pb]","L_{joint}(m_{t},#sigma_{t#bar{t}})");

	predicted_.exportLikelihood(&theolh_,false);

	//fill experimental LH
	graphFitter expdep=getMassDependence();
	for(size_t i=1;i<=explh_.getNBinsX();i++){
		for(size_t j=1;j<=explh_.getNBinsY();j++){
			//centerx+=1;
			float centery,centerx;
			explh_.getBinCenter(i,j,centerx,centery);
			double lhexp=LH_Exp(centerx,centery,expdep,xsecerrup/xsecnom,xsecerrdown/xsecnom);
			explh_.getBin(i,j).setContent(lhexp);
		}
	}
	jointlh_=theolh_*explh_;
	jointlh_.setZAxisName("L_{joint}(m_{t},#sigma_{t#bar{t}})");
	jointlh_*= 1/jointlh_.getMax();


	//get max value and uncertainty

	onesigmajoint_=createOneSigmaPoints(jointlh_);

	size_t ybin,xbin;
	jointlh_.getMax(xbin,ybin);

	jointlh_.getBinCenter(xbin,ybin,mt_,intxsec_);

	mtup_=onesigmajoint_.getXMax() -mt_;
	mtdown_=mt_- onesigmajoint_.getXMin() ;



	size_t xdown=jointlh_.getBinNoX(mt_-mtdown_*2)+1;
	size_t xup=jointlh_.getBinNoX(mt_+mtdown_*2)+1;
	size_t ydown=jointlh_.getBinNoY(xsecnom-xsecerrdown*3)+1;
	size_t yup=jointlh_.getBinNoY(xsecnom+xsecerrup*3)+1;

	ZTOP_COUTVAR(xdown);
	ZTOP_COUTVAR(xup);
	ZTOP_COUTVAR(ydown);
	ZTOP_COUTVAR(yup);

	std::vector<float> newxbins(bins.begin()+xdown,bins.begin()+xup);
	std::vector<float> newybins(binsy.begin()+ydown,binsy.begin()+yup);

	histo2D tmp=jointlh_;
	tmp.setBinning(newxbins,newybins);

	tmp.copyContentFrom(jointlh_);
	//jointlh_=tmp;

	formatter fmt;

	std::cout << predicted_.idString() <<"\n"
			<< "mt: $" << fmt.toFixedCommaTString(mt_,0.1) << "\\pm^{"
			<< fmt.toFixedCommaTString(mtup_,0.1) << "}_{"
			<< fmt.toFixedCommaTString(mtdown_,0.1) <<"}\\GeV$" <<std::endl;



}


const histo2D&  mtFromInclXsec::getExpLikelihood()const{
	return explh_;
}
const histo2D& mtFromInclXsec::getTheoLikelihood()const{
	return theolh_;
}

const histo2D&  mtFromInclXsec::getJointLikelihood()const{
//2sigma left/right. 3 sigma up/down

	return jointlh_;
}

const graph& mtFromInclXsec::getExpPoints()const{
	return exppoints_;
}
graph mtFromInclXsec::createOneSigmaPoints(const histo2D&in)const{
	graph out;
	float onesigma=(float)exp(-0.5);

	for(size_t i=1;i<=in.getNBinsX();i++){
		for(size_t j=1;j<=in.getNBinsY();j++){
			if(isApprox(in.getBinContent(i,j),onesigma,0.01)){
				float cx,cy;
				in.getBinCenter(i,j,cx,cy);
				out.addPoint(cx,cy);
			}
		}
	}
	return out;
}

graph mtFromInclXsec::getResult()const{
	return result_;
}

double mtFromInclXsec::LH_Exp(const float& mtop,const float& xsec,
		const ztop::graphFitter & gf, const float& errup,const float& errdown)const{

	double xsecmeas=gf.getFitOutput(mtop);

	double xsecerr=errup*xsecmeas;
	double amberr=0;
	if(xsec<xsecmeas){
		xsecerr=errdown*xsecmeas;
		amberr=gf.getFitOutput(mtop+mtexpunc_)-xsecmeas;
	}
	else{
		amberr=gf.getFitOutput(mtop-mtexpunc_)-xsecmeas;
	}
	xsecerr= std::sqrt( sq(amberr) + sq(xsecerr));

	//add mtpole mtmc ambiguity

	//double mtoperr=0.5;
	double reldiff=(xsecmeas-xsec)/xsecerr;
	double lnLHxsec = sq(reldiff)/2;

	double combined = exp( - lnLHxsec );
	return combined;

}

}
