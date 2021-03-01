/*
 * top_pt_massdep.cc
 *
 *  Created on: Mar 27, 2015
 *      Author: kiesej
 */




#include "WbWbXAnalysis/Tools/interface/applicationMainMacro.h"

#include "TFile.h"
#include "TString.h"
#include "TGraph.h"
#include "TH1D.h"

#include "WbWbXAnalysis/Tools/interface/histo1D.h"
#include "WbWbXAnalysis/Tools/interface/graph.h"
#include "WbWbXAnalysis/Tools/interface/graphFitter.h"
#include "WbWbXAnalysis/Tools/interface/plotterCompare.h"
#include "WbWbXAnalysis/Tools/interface/plotterMultiplePlots.h"


invokeApplication(){

	using namespace ztop;
	TString dir="~/top_pt_vs_mass";
	std::vector<TString> infiles;
	infiles << "/Nominal/combined/DiffXS_HypToppT_source.root";
	infiles << "/MASS_UP/combined/DiffXS_HypToppT_source.root";
	infiles << "/MASS_DOWN/combined/DiffXS_HypToppT_source.root";

	std::vector<graph> datas;
	std::vector<graph> mcs;

	for(size_t i=0;i<infiles.size();i++){
		TFile * f = new TFile(dir+infiles.at(i),"READ");
		TH1D* Tmc=(TH1D*)f->Get("mc");
		TGraphAsymmErrors* Td=(TGraphAsymmErrors*)f->Get("data_staterror_only");
		histo1D mc;
		mc.import(Tmc,true);
		mc.removeStatFromAll();
		graph tmp;tmp.import(&mc,true);

		graph data;data.import(Td);
		datas << data;
		f->Close();
		delete f;
		//make sure x-coords fit
		//datatomc
		graph mcx=data;
		for(size_t j=0;j<data.getNPoints();j++){
			for (int sys = -1; sys < (int)data.getSystSize(); ++sys) {
				mcx.setPointYContent(j,tmp.getPointYContent(j),sys);
				mcx.setPointYStat(j,(tmp.getPointError(j,true,true,false)+tmp.getPointError(j,true,false,false))/2);
			}
		}
		mcs<<mcx;
	}



	plotterCompare pl;
	pl.readStyleFromFile("comparePlots_toppt.txt");
	pl.setNominalPlot(&mcs.at(0));
	pl.setComparePlot(&mcs.at(1),0);
	pl.setComparePlot(&mcs.at(2),1);
	pl.printToPdf("mc");

	pl.cleanMem();
	pl.setNominalPlot(&datas.at(0));
	pl.setComparePlot(&datas.at(1),0);
	pl.setComparePlot(&datas.at(2),1);
	pl.printToPdf("data");

	std::vector<graph> datatomc;
	for(size_t i=0;i<datas.size();i++){
		graph tmp=mcs.at(i);
		tmp.normalizeToGraph(datas.at(i));
		datatomc << tmp;
	}




	pl.cleanMem();
	pl.setNominalPlot(&datatomc.at(0));
	pl.setComparePlot(&datatomc.at(1),0);
	pl.setComparePlot(&datatomc.at(2),1);
	pl.printToPdf("dataratio");


	graph ratioup=datatomc.at(1),ratiodown=datatomc.at(2);
	ratioup.normalizeToGraph(datatomc.at(0));
	ratiodown.normalizeToGraph(datatomc.at(0));



	plotterMultiplePlots plm;
	plm.readStyleFromFile("multiplePlots_toppt.txt");
	plm.addPlot(&ratioup);
	plm.setLastNoLegend();
	plm.addPlot(&ratiodown);
	plm.setLastNoLegend();
	plm.printToPdf("ratio");

	graphFitter gf;
	gf.setFitMode(graphFitter::fm_pol3);
	gf.readGraph(&ratioup);
	gf.fit();
	graph ex=gf.exportFittedCurve(300);
	plm.addPlot(&ex);
	plm.setLastNoLegend();
	gf.readGraph(&ratiodown);
	gf.fit();
	ex=gf.exportFittedCurve(300);
	plm.addPlot(&ex);
	plm.setLastNoLegend();
	plm.printToPdf("ratioFits");


	return 0;
}
