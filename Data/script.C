#include "TFile.h"
//#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "TH2D.h"
#include <iostream>

void script(){

	// using namespace ztop;

	//void addRelError(TH2D &h, double err);

	TFile * f  = new TFile("elec_tight_SF.root","UPDATE");

	TH2D h= *( (TH2D*)f->Get("GlobalSF_err"));


	// addRelError(h,0.01);

	h.SetName("GlobalSF_err_err_err");

	for(int i=4;i<=7;i++){
		double cont=h.GetBinContent(i,1);
		double err=h.GetBinError(i,1);
		std::cout << cont << " " << err << std::endl;
		double add=0.08;
		if(i<5 || i >6)
			add=0.02;
		add*=cont;

		h.SetBinError(i,1, sqrt(err*err + add*add) );

	}

	 h.Write();

	f->Close();
}
