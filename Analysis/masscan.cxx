

#include "Hathor.h"
#include "HathorPdf.h"
#include <iostream>
#include <fstream>
#include "WbWbXAnalysis/Tools/interface/optParser.h"
#include "WbWbXAnalysis/Tools/interface/applicationMainMacro.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "WbWbXAnalysis/Tools/interface/graph.h"
#include "WbWbXAnalysis/Tools/interface/graphFitter.h"
#include "WbWbXAnalysis/Tools/interface/plotterMultiplePlots.h"
#include <string>
#include <vector>
#include "TFile.h"
#include  <limits>

invokeApplication(){
	std::string output=parser->getOpt<std::string>("o","mass.out","output file");
	std::string pdf=parser->getOpt<std::string>("-pdf","MSTW2008nnlo68cl","");
	double energy = parser->getOpt<float>("e",8000.,"");
	//double mt =parser->getOpt<float>("-mt",160.,"");
	double renmulti=parser->getOpt<float>("-renmulti",1.,"");
	double facmulti=parser->getOpt<float>("-facmulti",1.,"");
	std::string order=parser->getOpt<std::string>("a","NNLO","Order in QCD (LO,NLO,NNLO)");
	const bool onlypdfscan =parser->getOpt<bool>("-pdfscan",false,"");
	const bool polemass=parser->getOpt<bool>("-polemass",false,"");

	parser->doneParsing();

	bool nnlob= order == (std::string)"NNLO";
	bool nlob=order == (std::string)"NLO";
	bool lob=order == (std::string)"LO";

	if(nnlob){
		nlob=false;lob=false;
	}

	using namespace std;
	using namespace ztop;

	double val,err,chi;


	Hathor::COLLIDERTYPE ctype=Hathor::PP;
	Lhapdf lhapdf(pdf);
	Hathor XS(lhapdf);


	Hathor::MASSRENORMALISATIONSCHEME massscheme=Hathor::MS_MASS;
	if(polemass)
		massscheme=Hathor::POLE_MASS;

	if(nlob){
		XS.setScheme(Hathor::LO | Hathor::NLO | massscheme);
	}
	else if(lob){
		XS.setScheme(Hathor::LO | massscheme);
	}
	else{
		nnlob=true;
		XS.setScheme(Hathor::LO | Hathor::NLO | Hathor::NNLO | massscheme);
	}

	XS.setPrecision(Hathor::HIGH); //DEBUG


	std::vector<double> mts;
	if(!polemass){
		mts.push_back(140);
		mts.push_back(145);
		mts.push_back(150);
	}
	mts.push_back(155);
	mts.push_back(160);
	mts.push_back(165);
	mts.push_back(170);
	mts.push_back(175);
	mts.push_back(180);
	if(polemass){
		mts.push_back(185);
		mts.push_back(190);
		mts.push_back(195);
	}
	size_t pickedpoint=5;

	ztop::graph outgraph;

	//[0]+x*[1]+x*x*[2]+x*x*x*[3]+x*x*x*x*[4]+y*[5] +y*y*[6]+y*y*y*[7]+y*y*y*y*[8]


	XS.setSqrtShad(energy);
	if(!onlypdfscan)
		for(size_t i=0;i<mts.size();i++){

			double mt=mts.at(i);

			XS.getXsection(mt,mt*renmulti,mt*facmulti);
			XS.getResult(0,val,err,chi);

			cout << "e="<< energy<<" mt="<< mt << " as =" << XS.getAlphas(mt) <<" xsec ="<< val << " interr =" << err << endl;

			outgraph.addPoint(mt,val,err);

		}


	double max=0,min=10000000;
	//get scale for ~central
	double oneres=1;
	if(!onlypdfscan)
		for(double r=0.5;r<2.1;r+=0.5){
			for(double f=0.5;f<2.1;f+=0.5){
				XS.getXsection(mts.at(pickedpoint),mts.at(pickedpoint)*r,mts.at(pickedpoint)*f);
				XS.getResult(0,val,err,chi);
				if(r==1. && f==1.)
					oneres=val;
				cout << "e="<< energy<< " " << r << " mt="<< mts.at(pickedpoint) <<" xsec ="<< val << " interr =" << err << endl;
				if(max<val)max=val;
				if(min>val)min=val;
			}
		}
	double scalevarmax=(max-oneres)/oneres;
	double scalevarmin=(min-oneres)/oneres;



	if(nlob){
		XS.setScheme(Hathor::LO | Hathor::NLO | Hathor::MS_MASS| Hathor::PDF_SCAN);
	}
	else if(lob){
		XS.setScheme(Hathor::LO | Hathor::MS_MASS| Hathor::PDF_SCAN);
	}
	else{
		nnlob=true;
		XS.setScheme(Hathor::LO | Hathor::NLO | Hathor::NNLO | Hathor::MS_MASS| Hathor::PDF_SCAN);
	}


	XS.getXsection(mts.at(pickedpoint),mts.at(pickedpoint),mts.at(pickedpoint));
	for (int i=0; i< XS.getPdfNumber(); i++) {
		XS.getResult(i,val,err,chi);
		cout << "PDF member "<<i << " " << val << " " << err << endl;
	}
	double pdfup,pdfdown;
	XS.getPdfErr(pdfup,pdfdown);
	pdfup/=oneres;
	pdfdown/=oneres;


	//get alphas for central 172.5
	/*	double alphasup=0,alphasdown=0;
	Lhapdf lhapdfalphup(pdf);
	XS.setPDF(lhapdfalphup);
	XS.getXsection(mts.at(pickedpoint),mts.at(pickedpoint),mts.at(pickedpoint));
	XS.getResult(0,val,err,chi);
	alphasup=val/oneres;
	Lhapdf lhapdfalphdown(pdf);
	XS.setPDF(lhapdfalphdown);
	XS.getXsection(mts.at(pickedpoint),mts.at(pickedpoint),mts.at(pickedpoint));
	XS.getResult(0,val,err,chi);
	alphasdown=val/oneres; */
	//later fill by hand


	cout << "written to " <<  output << endl;
	outgraph.writeToFile(output+".ztop");


	ztop::graphFitter fitter;
	fitter.setFitMode(graphFitter::fm_pol6);
	fitter.setDoXYShift(true);
	fitter.readGraph(&outgraph);
	fitter.fit();

	graph fitted=fitter.exportFittedCurve();

	plotterMultiplePlots pl;
	pl.addPlot(&outgraph);
	pl.addPlot(&fitted);
	pl.printToPdf((output+pdf+order).data());

	std::ofstream file(output.data(), std::ios_base::out | std::ios_base::app);
	//std::ostringstream oss;
	file.flags (std::ios::scientific);
	file.precision (std::numeric_limits<double>::digits10 + 1);
	if(polemass)
		file << "[pred - Hathor_" << pdf << "_" << order << "_"<<toString(energy)<<"_pole]\n";
	else
		file << "[pred - Hathor_" << pdf << "_" << order << "_"<<toString(energy)<<"_MSbar]\n";
	file << "central for mt "<< mts.at(pickedpoint) <<" = " << oneres  << "\n";
	file << "xshift   = " << fitter.getXShift() << "\n";
	file << "yshift   = " << fitter.getYShift() << "\n";
	file << "par0     = " << fitter.getParameters()->at(0) << "\n";
	file << "par1     = " << fitter.getParameters()->at(1) << "\n";
	file << "par2     = " << fitter.getParameters()->at(2) << "\n";
	file << "par3     = " << fitter.getParameters()->at(3) << "\n";
	file << "par4     = " << fitter.getParameters()->at(4) << "\n";
	file << "par5     = " << fitter.getParameters()->at(5) << "\n";
	file << "par6     = " << fitter.getParameters()->at(6) << "\n";
	file << "relerr   = true\n";
	file << "scaleerrup = " << scalevarmax << "\n";
	file << "scaleerrdown = " << scalevarmin << "\n";
	file << "pdferrup   = " << pdfup<< "\n";
	file << "pdferrdown   = " << -fabs(pdfdown)<< "\n";
	file << "alphaserrup   = pleasefill \n";
	file << "alphaserrdown   = pleasefill \n";
	file << "adderrup   = pleasefill \n";
	file << "adderrdown   = pleasefill \n";
	file << "[end pred]\n\n" ;
	return 0;
}
