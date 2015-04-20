/*
 * testCorrM.cc
 *
 *  Created on: Apr 13, 2015
 *      Author: kiesej
 */




#include "../interface/corrMatrix.h"
#include "../interface/histo1D.h"
#include "../interface/plotterMultiplePlots.h"

int main(){
	using namespace ztop;

	std::vector<TString> names;
	names.push_back("bla1");
	names.push_back("bla2");
	names.push_back("bla3");
	names.push_back("bla4");
	names.push_back("bla5");

	corrMatrix m(names);



	histo1D histo(histo1D::createBinning(10,0,10));

	for(size_t i=0;i<10;i++)
		for(size_t j=0;j<100;j++) histo.fill(i,i);

	for(size_t i=0;i<5;i++){
		for(size_t j=0;j<5;j++)
			 m.setEntry(i,j,-3.*j*i/75) ;

		float multi= 1+( 3.*(i+1)/20);
		histo.addErrorContainer(m.getEntryName(i)+"_up", (histo)*multi );
		histo.addErrorContainer(m.getEntryName(i)+"_down", (histo)*(1/multi) );

	}

	for(size_t i=0;i<5;i++){
		std::cout <<  m.getEntryName(i) << "   " ;
		for(size_t j=0;j<5;j++)
			std::cout <<  m.getEntry(i,j) << "   " ;
		std::cout<<std::endl;
	}

	histo1D histocp=histo;
	histocp.mergeAllErrors("merged",false,m);

	plotterMultiplePlots pl;
	pl.addPlot(&histocp);
	pl.addPlot(&histo);
	pl.printToPdf("testCorrM");

	return 0;
}
