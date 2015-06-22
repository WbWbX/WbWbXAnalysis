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
#include "TtZAnalysis/Tools/interface/textFormatter.h"
#include "TCanvas.h"
#include "TStyle.h"


invokeApplication(){

	using namespace ztop;
	const float corrcoeff=parser->getOpt<float>("c",-100,"correlation coefficient between fitted cross sections");
	const bool noplots=parser->getOpt<bool>("-noplots",false,"don't create plots");
	//const float energy = parser->getOpt<float>("e",8,"energy -  TESTMODE!");
	const std::vector<std::string> in=parser->getRest<std::string>();

	const std::string predfile=getenv("CMSSW_BASE") +(std::string)"/"+ parser->getOpt<std::string>("-i",
			"src/TtZAnalysis/Analysis/data/predicitions/NNLO_paper.txt","input file for prediction");
	const std::string predids=parser->getOpt<std::string>("-p","8TeV:Top++_paper_8000_pole,7TeV:Top++_paper_7000_pole","prediction ids <energy>:<id>");

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

	std::vector<graph> results;


	if(ids.size()!=energies.size())
		throw std::out_of_range("Needs a prediction id for each energy");

	for(size_t ergy=0;ergy<energies.size();ergy++){
		std::string energy=energies.at(ergy);

		std::vector<std::string> files=splitfiles.at(ergy);
		mtFromInclXsec ex;
		mtFromInclXsec::debug=true;
		//ex.setEnergy(energy);
		ex.setEnvelopeUnc("fit");
		ex.readInFiles(files);
		ex.readPrediction(predfile,ids.at(ergy));
		ex.extract();

		if(!noplots){

			TString nrgstr="_"+energy;
			TCanvas cv;
			gStyle->SetOptStat(0);
			plotter2D pl;
			textBoxes tb2d,tb22d;;
			if(energies.at(ergy).find("8TeV") != std::string::npos){
				tb2d.readFromFileInCMSSW("/src/TtZAnalysis/Analysis/configs/general/CMS_boxes.txt","CMSPaperNoSplitRight2D7TeV");
				tb22d.add(0.8,   0.91,   "19.7 fb^{-1} (8 TeV)",   0.048,  42, 31);
			}else{
				tb2d.readFromFileInCMSSW("/src/TtZAnalysis/Analysis/configs/general/CMS_boxes.txt","CMSPaperNoSplitRight2D");
				tb22d.add(0.8,   0.91,   "5.0 fb^{-1} (7 TeV)",   0.048,  42, 31);
			}

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
			tb22d.drawToPad(&cv);
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
			tb22d.drawToPad(&cv);
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

		plotterMultiplePlots plm;
		plm.addPlot(&combine0,false);
		plm.addPlot(&combine1,false);
		plm.printToPdf("tobecombined");

		comb.addInput(combine0);
		comb.addInput(combine1);

		combine0.coutBinContent(1);
		combine1.coutBinContent(1);

		comb.setCombinedMinos(true);
		resultCombiner::debug=true;
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
