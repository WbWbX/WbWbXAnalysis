/*
 * mtFromInclXsec.cc
 *
 *  Created on: Mar 24, 2015
 *      Author: kiesej
 */



#include "../interface/mtFromInclXsec.h"
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



graphFitter mtFromInclXsec::getMassDependence(const TString& syst)const{
	if(exppoints_.getNPoints() != 3)
		throw std::logic_error("mtFromInclXsec::getMassDependence: exactly 3 points needed!");
	size_t idxup,idxdown;

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
	if(syst.Length()>0){
		fitgr=exppoints_.getSystGraph(exppoints_.getSystErrorIndex(syst));
	}
	else{
		fitgr=exppoints_;
	}
	fitgr.removeAllXYSyst();
	//fitgr.addErrorGraph(exppoints_.getSystErrorName(idxup),exppoints_.getSystGraph(idxup));
	//fitgr.addErrorGraph(exppoints_.getSystErrorName(idxdown),exppoints_.getSystGraph(idxdown));
	gf.readGraph(&fitgr);


	gf.fit();
	if(syst.Length()<1)
		std::cout << "fitted curve: xsec(m_t) = "
		<< "\\text{exp}\\left(" << gf.getParameter(2) << "\\cdot (m_t/\\GeV + " << gf.getParameter(1) <<" ) " << "\\right)+"<< gf.getParameter(0)<<std::endl;

	if(!gf.wasSuccess())
		throw std::runtime_error("mtFromInclXsec::getMassDependence: fit failed");
	return gf;
}

void  mtFromInclXsec::extract(){
	std::cout << "extract..."<<std::endl;
	size_t idxup,idxdown;

	idxup=exppoints_.getSystErrorIndex(envunc_+"_up");
	idxdown=exppoints_.getSystErrorIndex(envunc_+"_down");

	if(idxup>=exppoints_.getSystSize() || idxdown>=exppoints_.getSystSize() ){
		throw std::runtime_error("mtFromInclXsec::extract: envelope uncerainty not found");
	}

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

	float xsecerrup=exppoints_.getPointYContent(nompoint,idxup)-exppoints_.getPointYContent(nompoint);
	float xsecerrdown=exppoints_.getPointYContent(nompoint,idxdown)-exppoints_.getPointYContent(nompoint);



	const float mintopmass=165.5;
	const float maxtopmass=179.5;
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
	histo2D massup=explh_;
	histo2D massdown=explh_;
	histo2D  massnom=explh_;
	for(int sys=-1;sys<(int)exppoints_.getSystSize();sys++){
		if((size_t)sys==idxup || (size_t)sys==idxdown) continue;


		graphFitter sysfit;
		if(sys>-1.)
			sysfit=getMassDependence(exppoints_.getSystErrorName(sys)); //PERF
		else{
			std::cout << predicted_.idString() <<std::endl;
			sysfit=getMassDependence();
		}
		for(size_t i=1;i<=massup.getNBinsX();i++){
			for(size_t j=1;j<=massup.getNBinsY();j++){
				//centerx+=1;
				float centery,centerx;
				theolh_.getBinCenter(i,j,centerx,centery);
				double lhexp=LH_Exp(centerx,centery,sysfit,xsecerrup,xsecerrdown);
				massnom.getBin(i,j).setContent(lhexp);
				if(sys<0){
					lhexp=LH_Exp(centerx+1,centery,sysfit,xsecerrup,xsecerrdown);
					massup.getBin(i,j).setContent(lhexp);
					lhexp=LH_Exp(centerx-1,centery,sysfit,xsecerrup,xsecerrdown);
					massdown.getBin(i,j).setContent(lhexp);
				}
			}
		}
		if(sys<0){
			explh_=massnom;
		}
		else{
			explh_.addErrorContainer(exppoints_.getSystErrorName(sys),massnom);
		}
	}
	explh_.addErrorContainer("mtexp_up",massup);
	explh_.addErrorContainer("mtexp_down",massdown);

	histo2D splittheolh=theolh_;
	splittheolh.addErrorContainer("theo_up",splittheolh);
	splittheolh.addErrorContainer("theo_down",splittheolh);



	float binwidthhalf=(maxxsec-minxsec)/ndiv;

	predicted_.exportLikelihood(&theolh_);

	for(size_t i=1;i<=theolh_.getNBinsX();i++){
		for(size_t j=1;j<=theolh_.getNBinsY();j++){


			float centery,centerx;
			theolh_.getBinCenter(i,j,centerx,centery);
			double xsec= predicted_.getXsec(centerx);
			double xsecup= predicted_.getXsec(centerx,top_prediction::err_maxup);
			double xsecdown= predicted_.getXsec(centerx,top_prediction::err_maxdown);


			if(isApprox((float)xsec,centery,binwidthhalf))
				splittheolh.getBin(i,j).setContent(1);
			else
				splittheolh.getBin(i,j).setContent(0);
			if(isApprox((float)(xsecup),centery,binwidthhalf))
				splittheolh.getBin(i,j,0).setContent(1);
			if(isApprox((float)(xsecdown),centery,binwidthhalf))
				splittheolh.getBin(i,j,1).setContent(1);
		}
	}

	jointlh_=theolh_*explh_;

	histo2D forextr=explh_*splittheolh;

	//brute force max scan
	result_.clear();

	float maxmt=0,maxlh=0,dummy;
	float errleft=10000,errright=0;
	float onesigma=(float)exp(-0.5);
	for(size_t i=1;i<=forextr.getNBinsX();i++){
		for(size_t j=1;j<=forextr.getNBinsY();j++){

			if(forextr.getBinContent(i,j)>maxlh){
				maxlh=forextr.getBinContent(i,j);
				forextr.getBinCenter(i,j,maxmt,dummy);
			}
			if(isApprox(forextr.getBinContent(i,j),onesigma,0.01)){
				float dummy2;
				forextr.getBinCenter(i,j,dummy2,dummy);
				if(errleft>dummy2)errleft=dummy2;
				if(errright<dummy2)errright=dummy2;
			}
		}
	}
	std::cout << "mtop: "<< maxmt << "   (" << predicted_.idString() << ")"<<std::endl;
	result_.addPoint(1,maxmt);
	graph tmp = result_;
	tmp.setPointYContent(0,errright);
	result_.addErrorGraph("fit_up",tmp);
	tmp.setPointYContent(0,errleft);
	result_.addErrorGraph("fit_down",tmp);


	for(int i=0;i<(int)forextr.getSystSize();i++){

		size_t binx=0,biny=0;
		forextr.getMax(binx,biny,i);
		std::cout << binx << ","<<biny << std::endl;
		float cy,cx;
		forextr.getBinCenter(binx,biny,cx,cy);
		tmp.setPointYContent(0,cx);
		result_.addErrorGraph(forextr.getSystErrorName(i),tmp);
	}



}


const histo2D&  mtFromInclXsec::getExpLikelihood()const{
	return explh_;
}
const histo2D& mtFromInclXsec::getTheoLikelihood()const{
	return theolh_;
}

const histo2D&  mtFromInclXsec::getJointLikelihood()const{
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

	double xsecerr=errup;
	if(xsec<xsecmeas)
		xsecerr=errdown;
	//double mtoperr=0.5;
	double reldiff=(xsecmeas-xsec)/xsecerr;
	double lnLHxsec = sq(reldiff)/2;
	double lnLHmt = 0;//sq((mtopmeas-mtop)/mtoperr)/2;

	double combined = exp( - lnLHxsec - lnLHmt);
	return combined;

}

}
