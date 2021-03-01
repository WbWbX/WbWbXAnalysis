/*
 * extractMtFromInclXsec.cc
 *
 *  Created on: Mar 24, 2015
 *      Author: kiesej
 */


#include "../interface/mtFromInclXsec.h"
#include "WbWbXAnalysis/Tools/interface/applicationMainMacro.h"
#include "WbWbXAnalysis/Tools/interface/plotter2D.h"
#include "WbWbXAnalysis/Tools/interface/plotterMultiplePlots.h"
#include "WbWbXAnalysis/Tools/interface/resultCombiner.h"
#include "WbWbXAnalysis/Tools/interface/textFormatter.h"
#include "TCanvas.h"
#include "TStyle.h"
#include  "WbWbXAnalysis/Tools/interface/formatter.h"
#include "TFile.h"
#include "TLegend.h"
#include "WbWbXAnalysis/Tools/interface/fileReader.h"
invokeApplication(){

	using namespace ztop;
	const float corrcoeff=parser->getOpt<float>("c",-100,"correlation coefficient between fitted cross sections");
	const bool noplots=parser->getOpt<bool>("-noplots",false,"don't create plots");
	const bool nocmslogo=parser->getOpt<bool>("-nologo",false,"don't add CMS logo");
	//const float energy = parser->getOpt<float>("e",8,"energy -  TESTMODE!");
	const std::vector<std::string> in=parser->getRest<std::string>();

	const std::string predfile=getenv("CMSSW_BASE") +(std::string)"/src/WbWbXAnalysis/Analysis/data/predicitions/"+ parser->getOpt<std::string>("i",
			"ttbar_predictions.txt","input file for prediction");
	const std::string predids=parser->getOpt<std::string>("p","8TeV:Top++_NNPDF30_nnlo_as_0118NNLO8000_pole,7TeV:Top++_NNPDF30_nnlo_as_0118NNLO7000_pole","prediction ids <energy>:<id>");

	parser->doneParsing();


	std::vector<std::string> energies;
	std::vector<std::vector<std::string> > splitfiles;
	std::vector<std::string> ids;

	textFormatter tf;
	tf.setDelimiter(",");
	tf.setComment("");
	std::vector<std::string> splitids=tf.getFormatted(predids);
	tf.setDelimiter(":");
	for(size_t i=0;i<in.size();i++){
		const std::string& file=in.at(i);
		std::string en="";
		if(file.find("7TeV") != std::string::npos){
			en="7TeV";
		}
		else if(file.find("8TeV") != std::string::npos){
			en="8TeV";
		}
		if(en.length()>0){
			size_t enpos=std::find(energies.begin(),energies.end(),en)-energies.begin();
			if(enpos==energies.size()){
				energies.push_back(en);
				for(size_t k=0;k<splitids.size();k++){
					std::vector<std::string> iden=tf.getFormatted(splitids.at(k));
					if(iden.size()!=2)
						throw std::runtime_error("prediction id format not recognized");
					if(iden.at(0) == en){
						ids.push_back(iden.at(1));
						break;
					}
				}
			}
			splitfiles.resize(energies.size());
			splitfiles.at(enpos).push_back(file);
		}
	}

	std::vector<histo1D> results;


	if(ids.size()!=energies.size())
		throw std::out_of_range("Needs a prediction id for each energy");


	//rather hard-coded
	const float topmassmin=170,topmassmax=178.,xsecmin=143,xsecmax=287;
	histo2D alljoint = histo2D(histo1D::createBinning(800,topmassmin,topmassmax),histo1D::createBinning(500,xsecmin,xsecmax),"","m_{t} [GeV]",
			"#sigma_{t#bar{t}} [pb]", "L_{pred}(m_{t}, #sigma_{t#bar{t}})");

	std::vector<TGraphAsymmErrors*> forfinalLine,forfinalPoint;

	int predstyle=9;
	int predcolor=kBlue;

	for(size_t ergy=0;ergy<energies.size();ergy++){
		std::string energy=energies.at(ergy);

		std::vector<std::string> files=splitfiles.at(ergy);
		for(size_t filesi=0;filesi<files.size();filesi++)
			std::cout << files.at(filesi)<< " " ;
		std::cout << std::endl;
		mtFromInclXsec ex;
		mtFromInclXsec::debug=true;
		//ex.setEnergy(energy);
		ex.setEnvelopeUnc("fit");
		ex.readInFiles(files);
		ex.readPrediction(predfile,ids.at(ergy));
		ex.extract();

		std::cout << ids.at(ergy)<<"\n"<< ex.getMtopCentral() << std::endl;
		ZTOP_COUTVAR(ex.getMtopDown());
		ZTOP_COUTVAR(ex.getMtopUp());

		if(!noplots){

			TString nrgstr="_"+energy;
			TCanvas cv;
			gStyle->SetOptStat(0);
			plotter2D pl;
			textBoxes tb2d,tb22d;;

			if(energies.at(ergy).find("8TeV") != std::string::npos){
				tb2d.readFromFileInCMSSW("/src/WbWbXAnalysis/Analysis/configs/general/CMS_boxes.txt","CMSPaperNoSplitRight2D");
			}else{
				tb2d.readFromFileInCMSSW("/src/WbWbXAnalysis/Analysis/configs/general/CMS_boxes.txt","CMSPaperNoSplitRight2D7TeV");
			}



			textBoxes tbtheo;
			pl.readStyleFromFileInCMSSW("src/WbWbXAnalysis/Analysis/configs/mtFromXsec2/plot2D_0to1.txt");
			//pl.setZAxis("L_{pred}(m_{t},#sigma_{t#bar{t}})");
			if(energies.at(ergy).find("8TeV") != std::string::npos){
				tbtheo.readFromFileInCMSSW("/src/WbWbXAnalysis/Analysis/configs/general/CMS_boxes.txt","nodataCMSnoSplitRight2D");
			}else{
				tbtheo.readFromFileInCMSSW("/src/WbWbXAnalysis/Analysis/configs/general/CMS_boxes.txt","nodataCMSnoSplitRight2D7TeV");
			}
			pl.usePad(&cv);
			pl.setPlot(&ex.getTheoLikelihood());
			pl.draw();
			tbtheo.drawToPad(&cv);
			cv.Print(("theoOnly" +nrgstr+".pdf").Data());
			//cv.Print("expOnly" +nrgstr+".pdf");
			pl.cleanMem();



			pl.setPlot(&ex.getExpLikelihood());
			pl.usePad(&cv);
			//pl.setZAxis("L_{exp}(m_{t},#sigma_{t#bar{t}})");
			pl.draw();
			//TGraphAsymmErrors * g=countourexp.getTGraph("",true);
			//g->SetMarkerSize(0.15);
			//g->Draw("P,same");
			ex.getExpPoints().getTGraph()->Draw("P");

			TFile f("testfile.root","RECREATE");
			tb2d.drawToPad(&cv);
			//tb22d.drawToPad(&cv);
			cv.Print("expOnly" +nrgstr+".pdf");
			cv.Write();
			f.Close();
			pl.cleanMem();

			histo2D joint=ex.getJointLikelihood();

			histo2D tmpjoint=alljoint;
			ex.getPrediction()->exportLikelihood(&tmpjoint);
			//tmpjoint.copyContentFrom(joint);
			alljoint+=tmpjoint;

			pl.setPlot(&joint);
			pl.usePad(&cv);
			//pl.setZAxis("L_{exp} L_{pred}");
			pl.draw();
			//TGraphAsymmErrors* g2=countourcomb.getTGraph("",true);
			//	g2->SetMarkerSize(0.15);
			//	g2->Draw("P,same");
			//g->Draw("P,same");
			graph onesigma=ex.getOneSigmaPointsJoint();
			TGraphAsymmErrors *tg=onesigma.getTGraph();
			tg->SetMarkerSize(0.2);
			tg->Draw("P");
			tb2d.drawToPad(&cv);
			cv.Print("expTheo" +nrgstr+".pdf");
			pl.cleanMem();




			forfinalPoint.push_back(tg);


			tg=ex.getMassDependence("total_up").exportFittedCurve(20,topmassmin,topmassmax+20).getTGraph();
			tg->SetLineStyle(3);
			forfinalLine.push_back(tg);
			tg=ex.getMassDependence("total_down").exportFittedCurve(20,topmassmin,topmassmax+20).getTGraph();
			tg->SetLineStyle(3);
			tg->SetName("measerr");
			forfinalLine.push_back(tg);
			tg=ex.getMassDependence().exportFittedCurve(20,topmassmin,topmassmax+20).getTGraph();
			tg->SetLineStyle(7);
			tg->SetLineColor(kGreen+4);
			tg->SetLineWidth(2);
			tg->SetName("meas");
			forfinalLine.push_back(tg);

			likelihood2D lhtheo;
			lhtheo.import(ex.getTheoLikelihood());
			tg=lhtheo.getMaxLine(20).getTGraph();
			//tg=ex.getMassDepGraphTheo(mtFromInclXsec::sys_nominal,20).getTGraph();
			tg->SetLineStyle(predstyle);
			tg->SetLineColor(predcolor);
			tg->SetLineWidth(2);
			tg->SetName("predic");
			//forfinalLine.push_back(tg);

			graph respoint;
			respoint.addPoint(ex.getMtopCentral(),ex.getXsec());
			tg=respoint.getTGraph();
			forfinalPoint<<tg;

//
//			pl.setPlot(&ex.getJointLikelihood());
//			pl.usePad(&cv);
//			pl.draw();
//			tg->Draw("P");
//
//			tg->SetMarkerSize(0.2);
//			tg->Draw("P");
//			graph theonom=ex.getMassDepGraphTheo(mtFromInclXsec::sys_nominal,20);
//			float xmin=theonom.getXMin();
//			float xmax=theonom.getXMax();
//			graph nominal=ex.getMassDependence().exportFittedCurve(20,xmin,xmax);
//			tg=nominal.getTGraph();
//			tg->SetMarkerSize(0.);
//			tg->Draw("L");
//			tg=theonom.getTGraph();
//			tg->SetMarkerSize(0.);
//			tg->Draw("L");
//			ex.getExpPoints().getTGraph()->Draw("P");
//
//			tb2d.drawToPad(&cv);
//			cv.Print("expTheo2" +nrgstr+".pdf");

		}

		float mtop=ex.getMtopCentral();
		float mtopup=ex.getMtopUp();
		float mtopdown=ex.getMtopDown();

		ex.scaleFitUnc(corrcoeff);//extracted value will have correlated unc only
		ex.extract();

		float mtopc=ex.getMtopCentral();
		float mtopupc=ex.getMtopUp();
		float mtopdownc=ex.getMtopDown();

		float uncorrup= sqrt(  mtopup*mtopup    -mtopupc*mtopupc);
		float uncorrdown=sqrt( mtopdown*mtopdown-mtopdownc*mtopdownc);

		if(!isApprox(mtopc,mtop,0.1))
			throw std::runtime_error("changing uncertainties resulted in different central value");
		/*
		ZTOP_COUTVAR(mtop);
		ZTOP_COUTVAR(mtopup);
		ZTOP_COUTVAR(mtopdown);
		ZTOP_COUTVAR(mtopc);
		ZTOP_COUTVAR(mtopupc);
		ZTOP_COUTVAR(mtopdownc);
		ZTOP_COUTVAR(uncorrup);
		ZTOP_COUTVAR(uncorrdown);
		 */
		float symmuncorr=std::max(uncorrup,uncorrdown);

		histo1D res(histo1D::createBinning(1,-0.5,0.5));
		res.addErrorContainer("corr_up",res);
		res.addErrorContainer("corr_down",res);
		res.setBinContent(1,mtop);
		res.setBinStat(1,symmuncorr);
		res.setBinContent(1,mtop+mtopupc,0);
		res.setBinContent(1,mtop-mtopdownc,1);

		results.push_back(res);
	}


	//alljoint

	if(results.size()==2 ){

		if(corrcoeff<-90)
			throw std::logic_error("provide a correlation coefficient for the fitted uncertainty between both datasets!");

		resultCombiner comb;
		histo1D combine0,combine1;
		combine0=(results.at(0));
		combine1=(results.at(1));
		/*
		double spltfrac=corrcoeff*corrcoeff;

		combine0.splitSystematic(combine0.getSystErrorIndex("fit_up"),spltfrac,"fitcorr_up","stat_up");
		combine0.splitSystematic(combine0.getSystErrorIndex("fit_down"),spltfrac,"fitcorr_down","stat_down");
		combine1.splitSystematic(combine1.getSystErrorIndex("fit_up"),spltfrac,"fitcorr_up","stat_up");
		combine1.splitSystematic(combine1.getSystErrorIndex("fit_down"),spltfrac,"fitcorr_down","stat_down");

		combine1.transformSystToStat();
		combine0.transformSystToStat();

		combine0.equalizeSystematicsIdxs(combine1);
		 */
		plotterMultiplePlots plm;
		plm.addPlot(&combine0,false);
		plm.addPlot(&combine1,false);
		plm.printToPdf("tobecombined");

		comb.addInput(combine0);
		comb.addInput(combine1);

		combine0.coutFullContent();
		combine1.coutFullContent();

		comb.produceWeightedMean();

		histo1D out=comb.getOutput();

		out.coutBinContent(1);

		//comb.coutSystBreakDownInBin(1);

		//std::cout << "uncertainty: +" << out.getBinContent(1,0)-out.getBinContent(1) << " " << out.getBinContent(1,1)-out.getBinContent(1)<<std::endl;

		std::cout << "\"stat\" is the uncorrelated part of the combined uncertainty " <<std::endl;
		formatter fmt;
		std::cout << "combined mt= $"<< fmt.toFixedCommaTString(out.getBinContent(1),0.1)
												<< "\\pm^{" << fmt.toFixedCommaTString(out.getBinErrorUp(1),0.1)
												<< "}_{"     <<fmt.toFixedCommaTString(out.getBinErrorDown(1),0.1)
												<< "}\\GeV$" <<std::endl;



		TCanvas cvfin;
		plotter2D pl;
		pl.usePad(&cvfin);
		pl.setPlot(&alljoint);
		pl.readStyleFromFileInCMSSW("src/WbWbXAnalysis/Analysis/configs/fitTtBarXsec/plotCombinedMpole.txt");
		pl.draw();
		for(size_t i=0;i<forfinalPoint.size();i++){
			forfinalPoint.at(i)->Draw("P");
		}
		TGraphAsymmErrors * meas=0, *measerr=0, *pred=0;
		for(size_t i=0;i<forfinalLine.size();i++){
			forfinalLine.at(i)->Draw("L");
			TString name=forfinalLine.at(i)->GetName();
			if(name=="meas"){
				meas=forfinalLine.at(i);
			}
			else if(name=="measerr"){
				measerr=forfinalLine.at(i);
			}
			else if(name=="predic"){
				pred=forfinalLine.at(i);
			}
		}
		graph combined;
		combined.addPoint(out.getBinContent(1),210,"combined");
		combined.addErrorGraph("tot_up",combined);
		combined.addErrorGraph("tot_down",combined);
		combined.setPointXContent(0,out.getBinErrorUp(1)+out.getBinContent(1),0);
		combined.setPointXContent(0,-out.getBinErrorDown(1)+out.getBinContent(1),1);
		//combined.getTGraph()->Draw("P");
		//combined.getTextBoxesFromPoints().drawToPad(&cvfin);

		TFile * fcomb = new TFile("mass_ex.root","RECREATE");
		TLegend * leg=new TLegend(0.18,0.48,0.45,0.64);
		if(meas)
		leg->AddEntry(meas,"Measured","l");
		if(pred)
		leg->AddEntry(pred,"Predicted","l");
		if(measerr)
		leg->AddEntry(measerr,"Uncertainty","l");
		//TLine * line= new TLine(topmassmin,212,topmassmax,212);
		//line->Draw("same");
		leg->SetFillColor(kWhite);
		//leg->SetFillStyle(1);
		//leg->Draw("same");
		textBoxes tb;
		tb.add(0.20,0.58,"19.7 fb^{-1} (8 TeV)",0.045,42,11,1);
		tb.add(0.20,0.22,"5.0 fb^{-1} (7 TeV)",0.045,42,11,1);
		if(!nocmslogo)
			tb.add(0.78,   0.83,   "CMS",   0.06, 61, 31);
		tb.drawToPad(&cvfin);
		cvfin.Write();
		cvfin.Print("massex_combined.pdf");
		fcomb->Close();

	}
	if(!noplots){
		std::cout << "performing some post-processing to limit plot size..." <<std::endl;
		//some stupid post processing
		system("cp massex_combined.pdf massex_combined_hires.pdf");
		system("for f in massex_combined expOnly_8TeV expOnly_7TeV expTheo_7TeV expTheo_8TeV theoOnly_7TeV theoOnly_8TeV; do pdftopng $f.pdf $f;convert $f-000001.png  -density 1200 -quality 100 $f.pdf; mv $f-000001.png $f.png; done");
	}
	return 0;

}
