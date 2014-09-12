/*
 * tW_shapeComparison.cc
 *
 *  Created on: Aug 27, 2014
 *      Author: kiesej
 */

#include "../interface/containerStackVector.h"
#include "../interface/plotterMultiplePlots.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include "TCanvas.h"
#include "TString.h"
#include "TLine.h"

int main(){

	TString plot="m_lb step 8";

	TString filedir="/afs/desy.de/user/k/kiesej/recentProject/src/TtZAnalysis/Analysis/workdir/tWShapeComparison/";

	using namespace ztop;

	containerStackVector* csv=new containerStackVector();

	container1D withvar166, withoutvar166, withvar178, withoutvar178,nominal;

	//std::vector<TString> names;
	//names <<"emu_8TeV_"+mass+"_nominal_tWvar.root" << "emu_8TeV_"+mass+"_nominal.root" ;


	csv->loadFromTFile(filedir+"emu_8TeV_166.5_nominal_tWvar.root");
	containerStack stack=csv->getStack(plot);
	withvar166=stack.getFullMCContainer();
	withvar166.setName("#splitline{pred. @ m_{t}=166.5 GeV}{(tW @ m_{t}=166.5 GeV)}");
	delete csv;
	csv=new containerStackVector();
	csv->loadFromTFile(filedir+"emu_8TeV_166.5_nominal.root");
	stack=csv->getStack(plot);
	withoutvar166=stack.getFullMCContainer();
	withoutvar166.setName("#splitline{pred. @ m_{t}=166.5 GeV}{(tW @ m_{t}=172.5 GeV)}");
	delete csv;
	csv=new containerStackVector();
	csv->loadFromTFile(filedir+"emu_8TeV_172.5_nominal.root");
	stack=csv->getStack(plot);
	nominal=stack.getFullMCContainer();
	nominal.setName("pred. @ m_{t}=172.5 GeV");
	delete csv;
	csv=new containerStackVector();
	csv->loadFromTFile(filedir+"emu_8TeV_178.5_nominal_tWvar.root");
	withvar178=csv->getStack(plot).getFullMCContainer();
	withvar178.setName("#splitline{pred. @ m_{t}=178.5 GeV}{(tW @ m_{t}=178.5 GeV)}");
	delete csv;
	csv=new containerStackVector();
	csv->loadFromTFile(filedir+"emu_8TeV_178.5_nominal.root");
	withoutvar178=csv->getStack(plot).getFullMCContainer();
	withoutvar178.setName("#splitline{pred. @ m_{t}=178.5 GeV}{(tW @ m_{t}=172.5 GeV)}");

	stack.clear();

	//make the ratios:

	TCanvas cv;
	plotterMultiplePlots pl;
	pl.readStyleFromFileInCMSSW( "/src/TtZAnalysis/Analysis/configs/topmass/multiplePlots_tWvar.txt"  );//"src/TtZAnalysis/Tools/styles/comparePlots_3MC.txt");
	pl.usePad(&cv);

	nominal.normalize(false,false);
	nominal.setAllErrorsZero();
	withvar166.normalize(false,false);
	withoutvar166.normalize(false,false);
	withvar178.normalize(false,false);
	withoutvar178.normalize(false,false);

	histoContent::divideStatCorrelated=false;
	withvar166/=nominal;
	withoutvar166/=nominal;
	withvar178/=nominal;
	withoutvar178/=nominal;

	withvar166.setYAxisName("N/N(m_{t}=172.5 GeV)");

	pl.addPlot(&withvar166,false);
	pl.addPlot(&withoutvar166,false);
	pl.addPlot(&withoutvar178,false);
	pl.addPlot(&withvar178,false);

	pl.draw();

	TLine line=TLine(0,1,350,1);
	line.Draw("same");

	cv.Print("tW_var_normd.pdf");







}
