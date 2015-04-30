

#include "TtZAnalysis/Tools/interface/histo2D.h"
#include "TFile.h"
#include "TGraphAsymmErrors.h"
#include "TString.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include <stdexcept>
#include <iostream>

void muonEffTopTH2D(){
	using namespace ztop;
	TString infilename="TnP_Muon_Iso_SF.root";

	TString outfilename="muonEffTop2D_Iso.root";
	TString outhistoname="eff_pt_eta";

	TString prefix="DATA_over_MC_pt_PLOT_abseta_bin";
        TString suffix = "_&_Tight2012_pass_&_mcTrue_true";
        //TString suffix="_&_mcTrue_true";
	std::vector<TString> graphnames;
	graphnames
	<< "0"
	<< "1"
	<< "2"
	<< "3";
	std::vector<float> ybins;
	ybins<<0<<0.9<<1.2<<2.1<<2.4;

	if(ybins.size()!=graphnames.size()+1)
		throw std::out_of_range("muonEffTopTH2D: needs as many input graphs as 2d eta bins");
	histo2D tmpcont(ybins,ybins);

	TFile f(infilename,"OPEN");
	std::vector<TGraphAsymmErrors*> gs;

	for(size_t i=0;i<graphnames.size();i++){
                //if(! f.Get(prefix+graphnames.at(i))) break;
		TGraphAsymmErrors * g = (TGraphAsymmErrors*) f.Get(prefix+graphnames.at(i)+suffix);
		std::cout << g->GetName() <<std::endl;
		gs.push_back(g);
	}
	//bool importasBWdiv=false;

	tmpcont.import(gs,false);
	f.Close();//
	for(size_t i=0;i<tmpcont.getNBinsX();i++){
			std::cout << tmpcont.getBinContent(i,1) << std::endl;
	}

	//now add global uncertainties
	tmpcont.addGlobalRelError("Id",0.005);
	tmpcont.addGlobalRelError("Iso",0.002);
	tmpcont.addGlobalRelError("TopZ",0.005);

	TFile * fout=new TFile(outfilename,"RECREATE");

	TH2D * honlystat=tmpcont.getTH2D(outhistoname+"_onlystat",false,true);
	TH2D * hfullerr=tmpcont.getTH2D(outhistoname+"_fullerr",false,false);

	std::cout << honlystat->GetCellContent(1,1) <<std::endl;

	tmpcont.addGlobalRelError("Track",0.005);
	TH2D * hfullerrtr=tmpcont.getTH2D(outhistoname+"_fullwithtrackerr",false,false);
	honlystat->Write();
	hfullerr->Write();
	hfullerrtr->Write();
	fout->Close();
}
int main(){
	muonEffTopTH2D();
}
