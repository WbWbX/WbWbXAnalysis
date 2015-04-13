/*
 * extractMtFromInclXsec.cc
 *
 *  Created on: Mar 24, 2015
 *      Author: kiesej
 */


#include "../interface/mtFromInclXsec.h"
#include "TtZAnalysis/Tools/interface/applicationMainMacro.h"
#include "TtZAnalysis/Tools/interface/plotter2D.h"
#include "TtZAnalysis/Tools/interface/plotterMultiplePlots.h"
#include "TtZAnalysis/Tools/interface/resultCombiner.h"
#include "TCanvas.h"
#include "TStyle.h"


invokeApplication(){

	using namespace ztop;
	const float corrcoeff=parser->getOpt<float>("c",-100,"correlation coefficient between fitted cross sections");
	const bool noplots=parser->getOpt<bool>("-noplots",false,"correlation coefficient between fitted cross sections");
	//const float energy = parser->getOpt<float>("e",8,"energy -  TESTMODE!");
	const std::vector<std::string> in=parser->getRest<std::string>();
	parser->doneParsing();

	std::vector<float> energies;
	std::vector<std::vector<std::string> > splitfiles;
	for(size_t i=0;i<in.size();i++){
		const std::string& file=in.at(i);
		float en=0;
		if(file.find("7TeV") != std::string::npos){
			en=7;
		}
		else if(file.find("8TeV") != std::string::npos){
			en=8;
		}
		if(en){
			size_t enpos=std::find(energies.begin(),energies.end(),en)-energies.begin();
			if(enpos==energies.size())
				energies.push_back(en);
			splitfiles.resize(energies.size());
			splitfiles.at(enpos).push_back(file);
		}
	}

	std::vector<graph> results;

	for(size_t ergy=0;ergy<energies.size();ergy++){
		float energy=energies.at(ergy);
		std::vector<std::string> files=splitfiles.at(ergy);
		mtFromInclXsec ex;
		ex.setEnergy(energy);
		ex.setEnvelopeUnc("fit");
		ex.readInFiles(files);

		ex.extract();

		if(!noplots){

			TString nrgstr="_"+toTString(energy)+"TeV";
			TCanvas cv;
			gStyle->SetOptStat(0);
			plotter2D pl;
			textBoxes tb2d;
			tb2d.readFromFileInCMSSW("/src/TtZAnalysis/Analysis/configs/general/CMS_boxes.txt","CMSnoSplitRight2D");
			for(size_t i=0;i<tb2d.size();i++)
				tb2d.at(i).setColor(0);


			pl.readStyleFromFileInCMSSW("src/TtZAnalysis/Analysis/configs/mtFromXsec2/plot2D_0to1.txt");
			//pl.setZAxis("L_{pred}(m_{t},#sigma_{t#bar{t}})");
			pl.setPlot(&ex.getTheoLikelihood());
			pl.printToPdf(("theoOnly" +nrgstr).Data());
			pl.cleanMem();



			pl.setPlot(&ex.getExpLikelihood());
			pl.usePad(&cv);
			//pl.setZAxis("L_{exp}(m_{t},#sigma_{t#bar{t}})");
			pl.draw();
			//TGraphAsymmErrors * g=countourexp.getTGraph("",true);
			//g->SetMarkerSize(0.15);
			//g->Draw("P,same");
			ex.getExpPoints().getTGraph()->Draw("P");
			graph onesigma=ex.createOneSigmaPoints(ex.getExpLikelihood());
			TGraphAsymmErrors * tg=onesigma.getTGraph();
			tg->SetMarkerSize(0.2);
			tg->Draw("P");
			tb2d.drawToPad(&cv);
			cv.Print("expOnly" +nrgstr+".pdf");
			pl.cleanMem();


			pl.setPlot(&ex.getJointLikelihood());
			pl.usePad(&cv);
			//pl.setZAxis("L_{exp} L_{pred}");
			pl.draw();
			//TGraphAsymmErrors* g2=countourcomb.getTGraph("",true);
			//	g2->SetMarkerSize(0.15);
			//	g2->Draw("P,same");
			//g->Draw("P,same");
			onesigma=ex.createOneSigmaPoints(ex.getJointLikelihood());
			tg=onesigma.getTGraph();
			tg->SetMarkerSize(0.2);
			tg->Draw("P");
			tb2d.drawToPad(&cv);
			cv.Print("expTheo" +nrgstr+".pdf");
			pl.cleanMem();
		}
		graph res=ex.getResult();

		res.coutAllContent(false);

		results.push_back(res);
	}

	if(results.size()==2){

		if(corrcoeff<-90)
			throw std::logic_error("provide a correlation coefficient for the fitted uncertainty between both datasets!");

		resultCombiner comb;
		histo1D combine0,combine1;
		combine0.import(results.at(0));
		combine1.import(results.at(1));

		double spltfrac=corrcoeff*corrcoeff;

		combine0.splitSystematic(combine0.getSystErrorIndex("fit_up"),spltfrac,"fitcorr_up","stat_up");
		combine0.splitSystematic(combine0.getSystErrorIndex("fit_down"),spltfrac,"fitcorr_down","stat_down");
		combine1.splitSystematic(combine1.getSystErrorIndex("fit_up"),spltfrac,"fitcorr_up","stat_up");
		combine1.splitSystematic(combine1.getSystErrorIndex("fit_down"),spltfrac,"fitcorr_down","stat_down");

		combine1.transformSystToStat();
		combine0.transformSystToStat();

		combine0.equalizeSystematicsIdxs(combine1);

		comb.addInput(combine0);
		comb.addInput(combine1);

		comb.setCombinedMinos(true);

		bool succ=comb.minimize();
		if(!succ){
			std::cout << "failed to combine " <<std::endl;
		}
		histo1D out=comb.getOutput();

		out.coutFullContent();

		comb.coutSystBreakDownInBin(1);

		std::cout << "uncertainty: +" << out.getBinContent(1,0)-out.getBinContent(1) << " " << out.getBinContent(1,1)-out.getBinContent(1)<<std::endl;

		std::cout << "\"stat\" is the uncorrelated part of the fit uncertainty " <<std::endl;

	}

	return 0;

}
