/*
 * predicttest.cc
 *
 *  Created on: May 26, 2015
 *      Author: kiesej
 */




#include "../interface/predictions.h"
#include "WbWbXAnalysis/Tools/interface/applicationMainMacro.h"
#include "WbWbXAnalysis/Tools/interface/fileReader.h"
#include "WbWbXAnalysis/Tools/interface/histo2D.h"
#include "WbWbXAnalysis/Tools/interface/plotter2D.h"
#include "WbWbXAnalysis/Tools/interface/plotterMultiplePlots.h"
#include "WbWbXAnalysis/Tools/interface/likelihood2D.h"
#include "WbWbXAnalysis/Tools/interface/resultCombiner.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLegendEntry.h"
#include "WbWbXAnalysis/Tools/interface/formatter.h"


//all errors relative
void createExpLikelihood(ztop::histo2D & h, const float& xsec, const float& xsecerrup, const float& xsecerrdo,
		const float& mt=0, const float& mterrup=0, const float & mterrdown=0, const float& corrmtxsec=0){

	const float& corrcoeff=corrmtxsec;

	for(size_t i=1;i<h.getBinsX().size()-1;i++){
		for(size_t j=1;j<h.getBinsY().size()-1;j++){
			float binxsec,binmt;
			h.getBinCenter(i,j,binmt,binxsec);
			float mterr=mterrup;
			if(binmt<mt) mterr=mterrdown;
			//			mterr*=mt;

			float xerr=xsecerrup;
			if(binxsec<xsec) xerr=xsecerrdo;
			//xerr*=xsec;

			float xsecdiff=(xsec-binxsec);
			float mtdiff=binmt-mt;
			float LH=0;
			if(mt>0)
				//low correlation between xsec and mt
				LH= exp(0.5*( -xsecdiff*xsecdiff/(xerr*xerr)  - mtdiff*mtdiff/(mterr*mterr) - 2*corrcoeff*(mtdiff*xsecdiff)/(xerr*mterr)));
			else
				LH= exp( -0.5* xsecdiff*xsecdiff/(xerr*xerr));
			h.getBin(i,j).setContent(LH);

		}
	}
}





void extract(const float & corrcoeff, const bool& seventev, const bool& onlycorr,
		const bool& pole, const bool& onlynnlo,
		float& mt, float& mterrup, float& mterrd, float& dmt, float&dmtup, float& dmtdown, const std::string& nnlopred,
		const TString& addtoplots,bool noplots, bool noalphas=false){

	using namespace ztop;




	std::vector<top_prediction> preds(3);//prednnlo,prednlo,predlo;
	//fileReader::debug=true;
	//std::string predfile=getenv("CMSSW_BASE")+(std::string)"/src/WbWbXAnalysis/Analysis/data/predicitions/ttbar_predictions.txt";
	std::string predfile=getenv("CMSSW_BASE")+(std::string)"/src/WbWbXAnalysis/Analysis/data/predicitions/ttbar_predictions.txt";
	//predfile=getenv("CMSSW_BASE")+(std::string)"/src/WbWbXAnalysis/Analysis/data/predicitions/toppp_pole.txt";

	//bool seventev=false;


	//put here mtmc parameter by hand quick and dirty 0.0505087+0.11111--0.11092,


	float mtmc=172.733313;
	float mtmcerrup=6*0.106637; //in GeV
	mtmcerrup*=mtmcerrup;
	mtmcerrup+= 0.07*0.07 + 0.06*0.06 + 0.18*0.18;//add para + 3 mass points unc + add stat
	mtmcerrup=std::sqrt(mtmcerrup);

	float mtmcerrdown=6*0.105873; //inGeV, positive!
	mtmcerrdown*=mtmcerrdown;
	mtmcerrdown+= 0.07*0.07 + 0.06*0.06 + 0.18*0.18; //add para + 3 mass points unc
	mtmcerrdown=std::sqrt(mtmcerrdown);

	float corrwithxsec=0.10;

	if(seventev)
		corrwithxsec=0.02;


	graph ingraph;
	ingraph.readFromFile("xsecFit_graph8TeV.ztop");
	if(seventev){
		ingraph.readFromFile("xsecFit_graph7TeV.ztop");
	}

	if(pole){
		//predfile=getenv("CMSSW_BASE")+(std::string)"/src/WbWbXAnalysis/Analysis/data/predicitions/toppp_pole.txt";

		if(seventev){
			if(onlynnlo){
				preds.at(0).readPrediction(predfile,nnlopred);
			}
			else{
				preds.at(0).readPrediction(predfile,"Top++_NNPDF30_nnlo_as_0118NNLO7000_pole");

				preds.at(1).readPrediction(predfile,"Top++_NNPDF30_nnlo_as_0118NLO7000_pole");
				preds.at(2).readPrediction(predfile,"Top++_NNPDF30_nnlo_as_0118LO7000_pole");}
		}
		else{
			if(onlynnlo)
				preds.at(0).readPrediction(predfile,nnlopred);
			else{
				preds.at(0).readPrediction(predfile,"Top++_NNPDF30_nnlo_as_0118NNLO8000_pole");

				preds.at(1).readPrediction(predfile,"Top++_NNPDF30_nnlo_as_0118NLO8000_pole");
				preds.at(2).readPrediction(predfile,"Top++_NNPDF30_nnlo_as_0118LO8000_pole");}
		}
	}
	else{
		if(seventev){

			if(onlynnlo){
				preds.at(0).readPrediction(predfile,nnlopred);
			}
			else
				preds.at(0).readPrediction(predfile,"Hathor_NNPDF30_nnlo_as_0118_NNLO_7000_MSbar");

			preds.at(1).readPrediction(predfile,"Hathor_NNPDF30_nnlo_as_0118_NLO_7000_MSbar");
			preds.at(2).readPrediction(predfile,"Hathor_NNPDF30_nnlo_as_0118_LO_7000_MSbar");
		}
		else{

			if(onlynnlo){
				preds.at(0).readPrediction(predfile,nnlopred);
			}
			else{
				preds.at(0).readPrediction(predfile,"Hathor_NNPDF30_nnlo_as_0118_NNLO_8000_MSbar");

				preds.at(1).readPrediction(predfile,"Hathor_NNPDF30_nnlo_as_0118_NLO_8000_MSbar");
				preds.at(2).readPrediction(predfile,"Hathor_NNPDF30_nnlo_as_0118_LO_8000_MSbar");
			}
		}
	}
	if(onlynnlo){ //testing
		preds.resize(1);
		preds.at(0).readPrediction(predfile,nnlopred);

	}
	std::vector<graph> contourgraphs,maxgraphs;
	histo2D exphist;

	//split in correlated and uncorrelated form the beginning on


	//read xsec from resultsgraph


	//full error
	float xsec=ingraph.getPointYContent(0);
	float xsecerrup=ingraph.getPointYErrorUp(0,false);
	float xsecerrdown=ingraph.getPointYErrorDown(0,false);

	histo1D forcorronly;
	forcorronly.import(ingraph);
	double spltfrac=corrcoeff*corrcoeff;
	forcorronly.splitSystematic(forcorronly.getSystErrorIndex("fit_down"),spltfrac,"fitcorr_down","fituncorr_down");
	forcorronly.splitSystematic(forcorronly.getSystErrorIndex("fit_up"),spltfrac,"fitcorr_up","fituncorr_up");
	forcorronly.removeError("fituncorr_up");
	forcorronly.removeError("fituncorr_down");

	float xseccorrup=forcorronly.getBinErrorUp(1);
	float xseccorrdown=forcorronly.getBinErrorDown(1);
	std::cout << "xsec:" << xsec << " +" << xsecerrup << " -" << xsecerrdown << std::endl;
	std::cout << "mtmc:" << mtmc << " +" << mtmcerrup << " -" << mtmcerrdown << std::endl;




	float maxmt=178;
	float minmt=145;

	size_t nbins=1200;


	exphist.setXAxisName("m_{t}^{MC} [GeV]");
	exphist.setYAxisName("#sigma_{t#bar{t}} [pb]");
	exphist.setZAxisName("L(m_{t}^{MC}, #sigma_{t#bar{t}})");

	exphist.setBinning(histo1D::createBinning(nbins,minmt,maxmt),histo1D::createBinning(nbins,xsec-xsecerrdown*2,xsec+xsecerrup*4));
	histo2D expfordraw=exphist;
	expfordraw.setBinning(histo1D::createBinning(nbins,mtmc-4*mtmcerrdown,mtmc+4*mtmcerrup),
			histo1D::createBinning(nbins,xsec-xsecerrdown*4,xsec+xsecerrup*4));
	float binwidth=sqrt(exphist.getBinArea(1,1));
	if(!onlycorr){
		createExpLikelihood(expfordraw,xsec,xsecerrup,xsecerrdown,mtmc,mtmcerrup,mtmcerrdown,corrwithxsec);
		createExpLikelihood(exphist,xsec,xsecerrup,xsecerrdown,mtmc,mtmcerrup,mtmcerrdown,corrwithxsec);
	}
	else{
		createExpLikelihood(expfordraw,xsec,xseccorrup,xseccorrdown,mtmc,mtmcerrup,mtmcerrdown,corrwithxsec);
		createExpLikelihood(exphist,xsec,xseccorrup,xseccorrdown,mtmc,mtmcerrup,mtmcerrdown,corrwithxsec);
	}



	likelihood2D lhexponly;
	lhexponly.import(expfordraw);
	graph expcontour=lhexponly.getSigmaContour(expfordraw.getBinNoX(mtmc),expfordraw.getBinNoY(xsec));

	plotter2D pl;
	TCanvas cvexp;
	pl.usePad(&cvexp);
	pl.readStyleFromFileInCMSSW("src/WbWbXAnalysis/Analysis/configs/mtFromXsec2/plot2D_0to1.txt");
	pl.setPlot(&expfordraw);
	if(seventev)
		pl.readTextBoxesInCMSSW("/src/WbWbXAnalysis/Analysis/configs/general/noCMS_boxes.txt","CMSnoSplitRight2D7TeV");
	else
		pl.readTextBoxesInCMSSW("/src/WbWbXAnalysis/Analysis/configs/general/noCMS_boxes.txt","CMSnoSplitRight2D");
	if(!noplots){
		pl.draw();
		expcontour.getTGraph()->Draw("L");
		cvexp.Print("2DLH_exp_"+addtoplots+".pdf");
		TString sssssyss= "pdftopng 2DLH_exp_"+addtoplots+".pdf 2DLH_exp_"+addtoplots+";convert 2DLH_exp_"+addtoplots+"-000001.png  -density 1200 -quality 100 2DLH_exp_"+addtoplots+".pdf; rm -f 2DLH_exp_"+addtoplots+"-000001.png";
		system(sssssyss.Data());
	}

	histo2D exp_msbar=exphist;
	//std::vector<float> newybins=exphist.getBinsY();
	//newybins.erase(newybins.begin()); //remove former underflow
	//	exp_msbar
	//
	if(pole)
		exp_msbar.setXAxisName("m_{t}^{pole}, m_{t}^{MC} [GeV]");
	else
		exp_msbar.setXAxisName("m_{t}^{#bar{MS}}, m_{t}^{MC} [GeV]");

	exp_msbar.setYAxisName("#sigma_{t#bar{t}} [pb]");
	exp_msbar.setZAxisName("L(m_{t}^{MC}, #sigma_{t#bar{t}})");
	histo2D exphist_ext=exp_msbar;
	histo2D predmsbar=exp_msbar;


	size_t x=0,y=0;
	exphist.getMax(x,y);
	float cx=0,cy=0;
	exphist.getBinCenter(x,y,cx,cy);

	//	std::cout << cx << " "<< cy << std::endl;

	//double xsecexp=xsec;
	//double mtmcexp=mtmc;


	likelihood2D lh_exp;
	lh_exp.import(exphist);

	//	std::cout << lh_exp.getGlobalMax(x,y) << std::endl;

	//cross check
	//std::cout << "xsec err:" <<lh_exp.getYErrorUp(x,y) <<" "<< lh_exp.getYErrorDown(x,y)  <<std::endl;

	double xsecexperrup=lh_exp.getYErrorUp(x,y) ;
	double xsecexperrdown=lh_exp.getYErrorDown(x,y) ;
	double mtmcexperrup=mtmcerrup;    //lh_exp.getXErrorUp(x,y) ;
	double mtmcexperrdown=mtmcerrdown;//lh_exp.getXErrorDown(x,y) ;

	for(size_t i=1;i<exp_msbar.getBinsX().size()-1;i++){
		for(size_t j=1;j<exp_msbar.getBinsY().size()-1;j++){
			float bcx=0,bcy=0;
			exp_msbar.getBinCenter(i,j,bcx,bcy);


			float expLHmsbar = 0;
			if(bcy>xsec)
				expLHmsbar =exp( -0.5 * ( bcy -  xsec)*( bcy -  xsec) / (xsecexperrup*xsecexperrup) );
			else
				expLHmsbar =exp( -0.5 * ( bcy -  xsec)*( bcy -  xsec) / (xsecexperrdown*xsecexperrdown) );

			exp_msbar.getBin(i,j).setContent(expLHmsbar);



			exphist_ext.getBin(i,j).setContent(exphist.getBinContent(exphist.getBinNoX(bcx),exphist.getBinNoY(bcy)));

		}
	}


	for(size_t predit=0;predit<preds.size();predit++){

		top_prediction * selectedpred= &preds.at(predit);


		std::cout << selectedpred->idString() <<std::endl;
		//produce the exp likelihood as func of msbar

		if(noalphas)
			selectedpred->exportLikelihood(&predmsbar,false,true,top_prediction::errn_alphas);
		else
			selectedpred->exportLikelihood(&predmsbar,false);
		if(!noplots){
			pl.setPlot(&predmsbar);
			pl.printToPdf("2DLH_pred_"+selectedpred->idString());
		}

		pl.setPlot(&exp_msbar);
		//	pl.printToPdf("2DLH_msbarexp");

		histo2D jointLH = predmsbar * exp_msbar;

		likelihood2D jointlh2d;
		jointlh2d.import(jointLH);

		pl.setPlot(&jointLH);
		if(!noplots){
			pl.printToPdf("2DLH_joint_"+(std::string)addtoplots.Data()+selectedpred->idString());
		}
		histo2D bothinone=(exphist_ext+predmsbar);
		pl.setPlot(&bothinone);
		if(!noplots){
			pl.printToPdf("2DLH_both_"+(std::string)addtoplots.Data()+selectedpred->idString());
		}

		jointlh2d.getGlobalMax(x,y);
		float mass=0;float dummy;
		jointlh2d.getBinCenter(x,y,mass,dummy);
		//std::cout << mass << std::endl;
		//std::cout << jointlh2d.getXErrorUp(x,y) <<" "<< jointlh2d.getXErrorDown(x,y)  <<std::endl;

		//pl.setPlot(&jointLH);



		std::cout << "producing contour" <<std::endl;
		graph g=jointlh2d.getSigmaContour(x,y);
		std::cout << "produced contour" <<std::endl;
		//graph g2=jointlh2d.getSigmaContour(x,y,2);

		contourgraphs.push_back(g);
		//get nominal point from noinal pred, not from LH

		graph gp;
		float fittedmass=0;
		if(pole)
			fittedmass=selectedpred->scanTopMass(xsec,160,180);
		else
			fittedmass=selectedpred->scanTopMass(xsec,140,170);
		gp.addPoint(fittedmass,xsec);
		//std::cout << "extracted mass: " <<fittedmass ;
		float errup=g.getXMax()-fittedmass, errdown=g.getXMin()-fittedmass;
		//std::cout << " +" << errup;
		//std::cout << " " << errdown << std::endl;
		mt=fittedmass;
		mterrup=errup;
		mterrd=fabs(errdown);


		float diff=fittedmass - mtmc;
		float errdiffup,errdiffdown;

		x=jointLH.getBinNoX(fittedmass);
		y=jointLH.getBinNoY(xsec);

		//split part from xsec and rest to account for correlation of mtmc and xsec
		float mterrfromtheoup=jointlh2d.getXErrorUp(x,y);
		float mterrfromtheodown=jointlh2d.getXErrorDown(x,y);
		if(fabs(mterrfromtheoup-errup) < 2*binwidth)
			mterrfromtheoup=errup;
		if(fabs(mterrfromtheodown-errdown) < 2*binwidth)
			mterrfromtheodown=errdown;
		std::cout << mterrfromtheoup << " " << errup << std::endl;

		float mterrfromxsecup=std::sqrt(errup*errup - mterrfromtheoup*mterrfromtheoup);
		float mterrfromxsecdown=std::sqrt(errdown*errdown - mterrfromtheodown*mterrfromtheodown);

		errdiffdown=sqrt(mterrfromtheodown*mterrfromtheodown + mterrfromxsecdown*mterrfromxsecdown + mtmcexperrup*mtmcexperrup
				+2*corrwithxsec * fabs(mterrfromxsecdown) * mtmcexperrup);// +0.2 * errdown*mtmcexperrup);
		errdiffup  =sqrt(mterrfromtheoup*mterrfromtheoup     + mterrfromxsecup*mterrfromxsecup     + mtmcexperrdown*mtmcexperrdown
				+2*corrwithxsec * mterrfromxsecup * fabs(mtmcexperrdown));//  +0.2 * errup*mtmcexperrdown) ;

		std::cout << "contribution of correlated part to uncertainty (in sq scale): \n+" << std::sqrt(2*corrwithxsec * mterrfromxsecup * fabs(mtmcexperrdown) )<<std::endl;
		std::cout << "-"<<std::sqrt(2*corrwithxsec * fabs(mterrfromxsecdown) * mtmcexperrup) <<std::endl;
		//corr coeff is positive, corr of mterrfromxsec and xsec negative, error prop should be with "-2" -> +2

		//std::cout << "calibration to MC mass: "
		//		<< diff << " +"
		//		<< errdiffup <<" -"
		//		<< errdiffdown <<std::endl;
		dmt=diff;
		dmtup=errdiffup;
		dmtdown=fabs(errdiffdown);

		std::cout << "extracted mass for " << selectedpred->idString() << "\n"<< mt << " +" << mterrup<<" -" << mterrd << std::endl;

		maxgraphs.push_back(gp);
		//std::cout << g.getNPoints() << std::endl;
		//std::cout << gp.getNPoints() << std::endl;
	}
	std::cout << "last plots" <<std::endl;
	TCanvas c;
	//exphist.setAllZero();
	pl.setPlot(&exphist_ext);
	pl.usePad(&c);
	if(seventev)
		pl.readTextBoxesInCMSSW("/src/WbWbXAnalysis/Analysis/configs/general/noCMS_boxes.txt","CMSnoSplitRight2D7TeV");
	else
		pl.readTextBoxesInCMSSW("/src/WbWbXAnalysis/Analysis/configs/general/noCMS_boxes.txt","CMSnoSplitRight2D");

	if(!noplots){
		pl.draw(); //get margins right.. temp!
		TLegend * leg=new TLegend(0.3,0.6,0.7,0.88);
		leg->SetFillStyle(0);
		leg->SetBorderSize(0);


		for(size_t i=0;i<contourgraphs.size();i++){
			//std::cout <<contourgraphs.at(i).getNPoints() << std::endl;
			TGraphAsymmErrors *tg=contourgraphs.at(i).getTGraph();
			int color=0;
			TString label="";
			if(((TString)preds.at(i).idString()).Contains("NNLO")){
				label="NNLO";
				if(pole)
					label+="+NNLL";
				color=kBlue;
			}
			else if(((TString)preds.at(i).idString()).Contains("NLO")){
				label="NLO";
				if(pole)
					label+="+NLL";
				color=kRed;
			}
			else if(((TString)preds.at(i).idString()).Contains("LO")){
				label="LO";
				if(pole)
					label+="+LL";
				color=kMagenta;
			}
			tg->SetLineColor(color);
			tg->SetMarkerColor(color);
			tg->SetLineWidth(2);
			tg->SetMarkerSize(0.5);
			tg->Draw("L");
			TGraphAsymmErrors *tg2=maxgraphs.at(i).getTGraph();
			tg2->SetMarkerColor(color);

			//TLegendEntry* entr=
			leg->AddEntry(tg2,label,"pe");
			//entr->SetTextColor(10);


			tg2->Draw("P");
		}
		leg->Draw("same");
		//	g2.getTGraph()->Draw("L");
		c.Print("2DLH_joint_"+addtoplots+".pdf");
		TString sysstr="pdftopng 2DLH_joint_"+addtoplots+".pdf 2DLH_joint_"+addtoplots+";convert 2DLH_joint_"+addtoplots+"-000001.png  -density 1200 -quality 100 2DLH_joint_"+addtoplots+".pdf; rm -f 2DLH_joint_"+addtoplots+"-000001.png" ;
		system(sysstr.Data());
	}

}

void coutTeXLine(float val,float valup,float valdown){
	ztop::formatter fmt;
	std::cout << "$"<<fmt.toFixedCommaTString(val,0.1) << " \\pm^{"
			<<fmt.toFixedCommaTString(valup,0.1) << "}_{"
			<<fmt.toFixedCommaTString(valdown,0.1)<< "}$" <<std::endl;

}


invokeApplication(){
	const float corrcoeff=parser->getOpt<float>("c",0.3,"correlation coefficient");
	const bool seventev=parser->getOpt<TString>("e","","energy 7TeV/8TeV").Contains("7TeV");
	const bool pole = parser->getOpt<TString>("m","","pole or msbar").Contains("pole");

	const bool convtest = parser->getOpt<bool>("-conv",false,"convergence on/off");

	const std::string pred = parser->getOpt<std::string>("p","","prediction id (give 7 TeV if comb is desired)");
	const std::string predextr = parser->getOpt<std::string>("-dex","","extra prediction id. if specified, interpreted as 8 TeV id!");
	std::string outid = parser->getOpt<std::string>("o","","out id");

	const bool noplots =  parser->getOpt<bool>("-noplots",false,"no plots");

	const bool noalphas = parser->getOpt<bool>("-noalphas",false,"no alphas uncertainty");
	parser->doneParsing();

	using namespace ztop;

	//
	float mt, mtup,mtdown,dmt,dmtup,dmtd;
	//pole mass convergence
	if(convtest){
		extract(corrcoeff,seventev,false,pole,false,mt, mtup,mtdown,dmt,dmtup,dmtd, pred, outid, noplots,noalphas);
		return 1;
	}
	else{
		//get full tot unc
		extract(corrcoeff,true,false,pole,true,mt, mtup,mtdown,dmt,dmtup,dmtd, pred, outid, noplots,noalphas);
		float mtex=mt,
				mttotup=mtup,
				mttotd=mtdown,
				dmtex=dmt,
				dmttotup=dmtup,
				dmttotd=dmtd;
		//only correlated parts
		extract(corrcoeff,true,true,pole,true,mt, mtup,mtdown,dmt,dmtup,dmtd, pred, outid+"corr", noplots,noalphas);
		float mtcorrup=mtup,
				mtcorrd=mtdown,
				dmtcorrup=dmtup,
				dmtcorrd=dmtd;

		std::cout << pred << "\nextracted mt: ";
		coutTeXLine(mtex,mttotup,mttotd);
		std::cout << '\n' <<std::endl;

		std::cout << " correlated: ";
		coutTeXLine(mtex,mtcorrup,mtcorrd);

		float mtuncorrup=std::sqrt(mttotup*mttotup -  mtcorrup*mtcorrup);
		float mtuncorrdown=std::sqrt(mttotd*mttotd -  mtcorrd*mtcorrd);

		std::cout << " uncorrelated: ";
		coutTeXLine(mtex,mtuncorrup,mtuncorrdown);

		graph g7tevmt(1);
		g7tevmt.setPointXContent(0,1);
		g7tevmt.addErrorGraph("corr_up",g7tevmt);
		g7tevmt.addErrorGraph("corr_down",g7tevmt);
		g7tevmt.setPointYContent(0,mtex);
		g7tevmt.setPointYStat(0,std::max(mtuncorrup,mtuncorrdown));
		g7tevmt.setPointYContent(0,mtex+mtcorrup,0);
		g7tevmt.setPointYContent(0,mtex-mtcorrd,1);

		std::cout << pred << "\nextracted delta mt: ";
		coutTeXLine(dmtex,dmttotup,dmttotd);
		std::cout << '\n' <<std::endl;

		std::cout << " correlated: ";
		coutTeXLine(dmtex,dmtcorrup,dmtcorrd);

		float dmtuncorrup=std::sqrt(dmttotup*dmttotup -  dmtcorrup*dmtcorrup);
		float dmtuncorrdown=std::sqrt(dmttotd*dmttotd -  dmtcorrd*dmtcorrd);


		std::cout << " uncorrelated: ";
		coutTeXLine(dmtex,dmtuncorrup,dmtuncorrdown);


		graph g7tevdmt=g7tevmt;
		g7tevdmt.setPointYContent(0,dmtex);
		g7tevdmt.setPointYStat(0,std::max(dmtuncorrup,dmtuncorrdown));
		g7tevdmt.setPointYContent(0,dmtex+dmtcorrup,0);
		g7tevdmt.setPointYContent(0,dmtex-dmtcorrd,1);

		outid+="8TeV";
		extract(corrcoeff,false,false,pole,true,mt, mtup,mtdown,dmt,dmtup,dmtd, predextr, outid, noplots,noalphas);
		float emtex=mt,
				emttotup=mtup,
				emttotd=mtdown,
				edmtex=dmt,
				edmttotup=dmtup,
				edmttotd=dmtd;
		//only correlated parts
		extract(corrcoeff,false,true,pole,true,mt, mtup,mtdown,dmt,dmtup,dmtd, predextr, outid+"corr", noplots,noalphas);
		float emtcorrup=mtup,
				emtcorrd=mtdown,
				edmtcorrup=dmtup,
				edmtcorrd=dmtd;

		std::cout  <<'\n' <<predextr << " extracted mt: ";
		coutTeXLine(emtex,emttotup,emttotd);
		std::cout << '\n' <<std::endl;

		std::cout << " correlated: ";
		coutTeXLine(emtex,emtcorrup,emtcorrd);

		float emtuncorrup=std::sqrt(emttotup*emttotup -  emtcorrup*emtcorrup);
		float emtuncorrdown=std::sqrt(emttotd*emttotd -  emtcorrd*emtcorrd);

		std::cout << " uncorrelated: ";
		coutTeXLine(emtex,emtuncorrup,emtuncorrdown);

		graph g8tevmt=g7tevmt;
		g8tevmt.setPointYContent(0,emtex);
		g8tevmt.setPointYStat(0,std::max(emtuncorrup,emtuncorrdown));
		g8tevmt.setPointYContent(0,emtex+emtcorrup,0);
		g8tevmt.setPointYContent(0,emtex-emtcorrd,1);

		std::cout <<'\n'<< predextr << " extracted delta mt: ";
		coutTeXLine(edmtex,edmttotup,edmttotd);
		std::cout << '\n' <<std::endl;

		std::cout << " correlated: ";
		coutTeXLine(edmtex,edmtcorrup,edmtcorrd);

		float edmtuncorrup=std::sqrt(edmttotup*edmttotup -  edmtcorrup*edmtcorrup);
		float edmtuncorrdown=std::sqrt(edmttotd*edmttotd -  edmtcorrd*edmtcorrd);


		std::cout << " uncorrelated: ";
		coutTeXLine(edmtex,edmtuncorrup,edmtuncorrdown);


		graph g8tevdmt=g7tevmt;
		g8tevdmt.setPointYContent(0,edmtex);
		g8tevdmt.setPointYStat(0,std::max(edmtuncorrup,edmtuncorrdown));
		g8tevdmt.setPointYContent(0,edmtex+edmtcorrup,0);
		g8tevdmt.setPointYContent(0,edmtex-edmtcorrd,1);

		std::cout << '\n' <<std::endl;
		std::cout << "combine\n\n" <<std::endl;


		resultCombiner combmt;
		resultCombiner::debug=true;

		combmt.addInput(g7tevmt);
		combmt.addInput(g8tevmt);
		combmt.produceWeightedMean();
		histo1D mtout=combmt.getOutput();
		mtout.coutBinContent(1);
		//combmt.coutSystBreakDownInBin(1);

		std::cout << "\ncombined mt:" <<std::endl;
		coutTeXLine(mtout.getBinContent(1),mtout.getBinErrorUp(1),mtout.getBinErrorDown(1));


		resultCombiner combdmt;
		combdmt.addInput(g7tevdmt);
		combdmt.addInput(g8tevdmt);
		combdmt.produceWeightedMean();
		histo1D dmtout=combdmt.getOutput();
		dmtout.coutBinContent(1);
		//combdmt.coutSystBreakDownInBin(1);
		std::cout << "\ncombined deltamt:" <<std::endl;
		coutTeXLine(dmtout.getBinContent(1),dmtout.getBinErrorUp(1),dmtout.getBinErrorDown(1));


		//do 8 TeV

	}



	return 1;
}

