/*
 * unfold.C
 *
 *  Created on: Sep 15, 2013
 *      Author: kiesej
 */


#include "TtZAnalysis/Tools/interface/containerUnfolder.h"
#include "TtZAnalysis/Tools/interface/container1DUnfold.h"
#include "TtZAnalysis/Tools/interface/containerStackVector.h"
#include "TtZAnalysis/Tools/interface/containerStack.h"
#include "TString.h"
#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TCanvas.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"



namespace application{
/**
 * input: files with fully merged systematics (as you like), not necessarily Canvases
 * output option: -o, otherwise <input>_unfolded
 */
void setNameAndTitle(TCanvas *c, TString name){
	c->SetTitle(name);
	c->SetName(name);
}

void unfold(int argc, char* argv[]){
	TH1::AddDirectory(false);
	AutoLibraryLoader::enable();

	ztop::containerUnfolder::debug=false;
	ztop::container2D::debug=false;
	ztop::container1DUnfold::debug=false;
	ztop::containerStack::debug=false;

	bool debug=true;

	//std::vector<TString> ttsignals,zsignals;
	//ttsignals.push_back("t#bar{t}");
	//zsignals.push_back("Z#rightarrowll");
	//signals.push_back("t#bar{t}(#tau)");

	std::vector<TString> stacknames;

	ztop::containerUnfolder unfolder;
	/*
	 * set unfolding options etc.... not yet available
	 */

	TString out="",in="",signal="";
	bool moreoutput=false;
	for(int i=1;i<argc;i++){
		//  std::cout << argv[i] << std::endl;;

		if((TString)argv[i] == "-o"){
			if (i + 1 != argc){
				out=(TString)argv[i+1];
				std::cout << out << std::endl;
				i++;
			}
		}
		else if((TString)argv[i] == "-v"){
			moreoutput=true;
		}
		else if((TString)argv[i] == "-v"){
			if (i + 1 != argc){
				signal=(TString)argv[i+1];
				i++;
			}
		}
		else {
			in=(TString)argv[i];
			std::cout << in << std::endl;
		}
	}
	if(in=="" || (out=="" && argc>2) || (out != "" && argc > 4)){
		std::cout << "You have to specify axactly one input file and might specify an output name! exit" << std::endl;
		return;
	}
	//input checked
	if(!in.EndsWith(".root"))
		in.Append(".root");

	TFile * f = new TFile(in,"READ");
	if(!(f->Get("stored_objects"))){
		std::cout << "tree stored_objects not found in input file. exit" << std::endl;
		return;
	}

	ztop::containerUnfolder::printinfo = moreoutput;

	TTree * t = (TTree*)f->Get("stored_objects");
	ztop::containerStackVector *pcsv;
	TString csvname=in;
	csvname.ReplaceAll("_plots.root","");
	csvname.ReplaceAll(".root","");

	if(t->GetBranch("allContainerStackVectors")){
		pcsv=pcsv->getFromTree(t,csvname);
	}
	else{
		std::cout << "containerStackVectors not found! exit" << std::endl;
		return;
	}
	if(!pcsv){
		csvname.ReplaceAll("_syst","");
		pcsv=pcsv->getFromTree(t,csvname);
	}
	if(!pcsv){
		std::cout << "containerStackVector not found! exit" << std::endl;
		return;
	}
	/*
	 * csv prepared and read. close input file
	 */
	ztop::containerStackVector csv=*pcsv;
	f->Close();
	delete f;

	//prepare output file
	TString outfile=out;
	if(outfile==""){
		outfile=in;
		outfile.ReplaceAll(".root","_unfolded.root");
	}

	//get right stacks and set signal
	size_t csvsize=csv.getVector().size();
	std::vector<ztop::containerStack> tobeunfolded;
	for(size_t i=0; i<csvsize;i++){
		ztop::containerStack  stack=csv.getStack(i);
		if(stack.is1DUnfold()){
			stacknames.push_back(stack.getName());
			tobeunfolded.push_back(stack);
		}
	}


	//prepare container1DUnfolds
	if(debug)
		std::cout << "main: produceUnfoldingContainer..." <<std::endl;
	std::vector<ztop::container1DUnfold> datatobeunf,xchecktobeunf;
	for(size_t i=0;i<tobeunfolded.size();i++){
		ztop::containerStack * stack=&tobeunfolded.at(i);
		datatobeunf.push_back(stack->produceUnfoldingContainer());
		if(debug)
			std::cout << "main: produceUnfoldingContainer for "<< stack->getName() <<std::endl;
		xchecktobeunf.push_back(stack->produceXCheckUnfoldingContainer());
		if(debug)
			std::cout << "main: produceXCheckContainer for "<< stack->getName() <<std::endl;
	}

	//do unfolding

	for(size_t i=0;i<stacknames.size();i++)
		stacknames.at(i).ReplaceAll(" ", "_");

	///unfolding and output part!!
	TString outdir="unfolded"+out+"/";
	system(("mkdir -p unfolded"+out).Data());

	size_t ufsize=datatobeunf.size();
	//parallel for?!?
	f = new TFile(outdir+outfile,"RECREATE");
	f->cd();
	TTree *outtree= new TTree("stored_objects","stored_objects");
	ztop::container1D * cpointer=0;
	outtree->Branch("unfoldedContainers",&cpointer);
	TDirectory * d=0;
	for(size_t i=0;i<ufsize;i++){
		TString name=stacknames.at(i);
		TString nameus=name;
		nameus.ReplaceAll(" ","_");
		f->cd();
		d = new TDirectory(nameus,nameus);
		d->cd();
		outdir="unfolded/"+nameus+"/";
		system(("mkdir -p "+outdir).Data());
		ztop::container1DUnfold & data=datatobeunf[i];
		ztop::container1DUnfold & check=xchecktobeunf[i];
		std::cout << "unfolding " << data.getName() << " syst: ";
		for(size_t s=0;s<data.getSystSize();s++)
			std::cout  << data.getSystErrorName(s) << " ";
		std::cout  << std::endl;
		unfolder.unfold(data);
		ztop::container1D unfolded=data.getUnfolded();
		ztop::container1D refolded=data.getRefolded();
		unfolded.setName(data.getName()+"_unfolded");
		refolded.setName(data.getName()+"_refolded");
		cpointer=&unfolded;
		outtree->Fill();
		cpointer=&refolded;
		outtree->Fill();
		//TDirectory *d = new TDirectory(data.getName(),data.getName());
		//d->cd();
		/////////
		TCanvas * c = new TCanvas(name+"_unfolded",name+"_unfolded");
		unfolded.drawFullPlot();
		if(unfolded.getNBins() < 3){
			std::cout << "\n\n" << name << std::endl;
			for(size_t bin=1;bin<=unfolded.getNBins();bin++)
				//unfolded.coutBinContent(bin);
				data.coutUnfoldedBinContent(bin); //dangerous
			std::cout << "\n\n"  << std::endl;

		}
		if(debug)
			c->Print(outdir+name+"_unfolded.eps");
		c->Write();
		c->Clear();
		setNameAndTitle(c,name+"_refolded_recoBG");
		refolded.drawFullPlot();
		ztop::container1D reco=data.getRecoContainer();
		//ztop::container1D recmbg=reco-data.getBackground();
		reco.drawFullPlot("",true,"same");
		data.getBackground().drawFullPlot("",true,"same");
		if(debug)
			c->Print(outdir+name+"_refolded_recoBG.eps");
		c->Write();

		c->Clear();
		setNameAndTitle(c,name+"_checkUnfoldGen");

		unfolder.unfold(check);
		ztop::container1D checkunfolded=check.getUnfolded();
		const ztop::container1D& generated=check.getGenContainer();
		checkunfolded.drawFullPlot();
		TH1D*   gen =check.getGenContainer().getTH1D("",true,true);
		gen->SetMarkerColor(kRed);
		gen->Draw("same");
		if(debug)
			c->Print(outdir+name+"_checkUnfoldGen.eps");
		c->Write();
		c->Clear();
		setNameAndTitle(c,name+"normalized");
		unfolded.getTH1D()->DrawNormalized();
		gen->DrawNormalized("same");
		c->Write();
		if(debug)
			c->Print(outdir+name+"normalized");
		delete c;
		delete d;
	}

	f->Close();
	delete f;

}
}
//////////////for compling////////
int main(int argc, char* argv[]){
	application::unfold(argc,argv);
	return 0;
}
//namespace
