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
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
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
	using namespace ztop;
	bool debug=true;

	/*
	 *
	 * get options
	 */
	TString out="",in="",signal="";
	bool ignoremass=false;
	bool moreoutput=false;
	bool inset=false;
	for(int i=1;i<argc;i++){

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
		else if((TString)argv[i] == "-s"){
			if (i + 1 != argc){
				signal=(TString)argv[i+1];
				i++;
			}
		}
		else if((TString)argv[i] == "-i"){
			ignoremass=true;
		}
		else {
			if(inset){
				std::cout << "You have to specify exactly one input file" << std::endl;
				return;
			}
			in=(TString)argv[i];
			inset=true;
		}
	}

	if(!inset){
		std::cout << "You have to specify exactly one input" << std::endl;
		return;
	}
	//input checked
	if(!in.EndsWith(".root"))
		in.Append(".root");

	TFile * f = new TFile(in,"READ");
	if(f->IsZombie()){
		std::cout << "file could not be opened/does not exist. exit" << std::endl;
		return;
	}
	if(!(f->Get("stored_objects"))){
		std::cout << "tree stored_objects not found in input file. exit" << std::endl;
		return;
	}

	if(moreoutput){
		ztop::containerUnfolder::debug=true;
		ztop::container2D::debug=false;
		ztop::container1DUnfold::debug=true;
		ztop::containerStack::debug=false;
	}
	ztop::containerUnfolder::printinfo = moreoutput;

	/*
	 * configure output and options
	 *
	 */

	TString outfile=in;
	outfile.ReplaceAll(".root","");
	outfile+="_"+out;
	TString outdir=outfile+"_unfolded/";
	outfile+="_unfolded.root";


	system(("mkdir -p "+outdir).Data());

	std::vector<TString> ign;
	if(ignoremass){
		ign << "MT_3_down" << "MT_3_up"<< "MT_6_down" << "MT_6_up";
	}



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

	//get right stacks and set signal
	size_t csvsize=csv.getVector().size();
	std::vector<ztop::containerStack> tobeunfolded;
	for(size_t i=0; i<csvsize;i++){
		ztop::containerStack  stack=csv.getStack(i);
		if(stack.is1DUnfold()){
			bool use=true;
			if(signal!="")
				use=stack.setsignal(signal);
			if(use){
				std::cout << "Set signal to " << signal << " for stack " << stack.getName() <<std::endl;
				tobeunfolded.push_back(stack);
			}
		}
	}


	///unfolding and output part!!

	ztop::containerUnfolder unfolder;
	//unfolder.setRegMode(TUnfold::kRegModeDerivative);
	unfolder.setScanMode(ztop::containerUnfolder::minCorr);
	unfolder.setIgnoreSyst(ign);

	size_t ufsize=tobeunfolded.size();
	//parallel for?!?
	f = new TFile(outdir+outfile,"RECREATE");
	f->cd();
	TTree *outtree= new TTree("stored_objects","stored_objects");
	ztop::container1D * cpointer=0;
	ztop::container1DUnfold * cufpointer=0;
	outtree->Branch("unfoldedContainers",&cpointer);
	outtree->Branch("container1DUnfolds",&cufpointer);
	TDirectory * d=0;
	for(size_t i=0;i<ufsize;i++){
		unfolder.clear();
		ztop::containerStack * stack=&tobeunfolded.at(i);

		TString name=stack->getName();
		TString nameus=name;
		nameus.ReplaceAll(" ","_");
		f->cd();
		d = f->mkdir(nameus,nameus);
		d->cd();
		TString outdirin=outdir+nameus+"/";
		system(("mkdir -p "+outdirin).Data());

		ztop::container1DUnfold  data=stack->produceUnfoldingContainer();
		if(data.getBackground().integral() > data.getRecoContainer().integral() * 0.3 ||  data.getRecoContainer().integral()==0){
			std::cout << "containerUnfolder::unfold: " << name << " has more background than signal, skipping xcheck" <<std::endl;
			continue;
		}
		std::cout << "unfolding " << data.getName() << " syst: ";
		//	for(size_t s=0;s<data.getSystSize();s++)
		//		std::cout  << data.getSystErrorName(s) << " ";
		std::cout  << std::endl;
		unfolder.unfold(data);
		ztop::container1D unfolded=data.getUnfolded();
		ztop::container1D refolded=data.getRefolded();

		unfolded.setName(data.getName()+"_unfolded");
		refolded.setName(data.getName()+"_refolded");
		cpointer=&unfolded;
		cufpointer=&data;
		outtree->Fill();

		//TDirectory *d = new TDirectory(data.getName(),data.getName());
		//d->cd();
		/////////
		d->cd();
		TCanvas * c = new TCanvas(name+"_unfolded",name+"_unfolded");
		c->Draw();
		unfolded.drawFullPlot();
		if(unfolded.getNBins() < 3){
			std::cout << "\n\n" << name << std::endl;
			for(size_t bin=1;bin<=unfolded.getNBins();bin++)
				//unfolded.coutBinContent(bin);
				data.coutUnfoldedBinContent(bin); //dangerous
			std::cout << "\n\n"  << std::endl;

		}
		d->cd();
		//c->Write();
		d->WriteTObject(c,c->GetName());
		c->Print(outdirin+name+"_unfolded.pdf");
		c->Clear();

		TDirectory * histdir=d->mkdir("hists","hists");
		TGraphAsymmErrors * singleg=unfolded.getTGraph();
		histdir->WriteTObject(singleg,singleg->GetName());
		delete singleg;
		d->cd();

		setNameAndTitle(c,name+"_refolded_recoBG");
		refolded.drawFullPlot();
		ztop::container1D reco=data.getRecoContainer();
		//ztop::container1D recmbg=reco-data.getBackground();
		reco.drawFullPlot("",true,"same");
		data.getBackground().drawFullPlot("",true,"same");
		d->cd();
		//c->Write();
		d->WriteTObject(c,c->GetName());
		c->Print(outdirin+name+"_refolded_recoBG.pdf");

		if(moreoutput)
			std::cout << "making control plot" << std::endl;
		c=stack->makeTCanvas();
		d->WriteTObject(c,((TString)c->GetName()+"_cplot").Data());
		delete c;


		if(moreoutput)
			std::cout << "making stab/pur plot" << std::endl;
		c=new TCanvas();
		c->cd(1)->SetBottomMargin(0.15);
		c->cd(1)->SetLeftMargin(0.15);
		setNameAndTitle(c,name+"_pur_stab");
		ztop::container1DUnfold cuf=stack->getSignalContainer();
		cuf.checkCongruentBinBoundariesXY();
		TH1D * pur=cuf.getPurity().getTH1D("purity",false,false,false);
		TH1D * stab=cuf.getStability().getTH1D("stability",false,false,false);
		TLegend * leg=0;
		if(pur){
			stab->SetLineColor(kRed);
			stab->SetMarkerColor(kRed);
			pur->GetYaxis()->SetRangeUser(0,1);
			pur->Draw();
			stab->Draw("same");
			 leg=new TLegend((Double_t)0.65,(Double_t)0.50,(Double_t)0.95,(Double_t)0.90);
			leg->AddEntry(pur,"purity","ep");
            leg->AddEntry(stab,"stability","ep");
            leg->Draw("same");

		}
		d->WriteTObject(c,((TString)c->GetName()+"_purStab").Data());
		if(leg) delete leg;

		///
		if(moreoutput)
			std::cout << "making regularization plots" << std::endl;
		if(!data.isBinByBin()){

			///lcurve plots
			c=new TCanvas();
			setNameAndTitle(c,name+"_lcurve");
			unfolder.getNominalUnfolder()->drawLCurve();
			d->WriteTObject(c,c->GetName());
			c->Clear();

			setNameAndTitle(c,name+"_tau");
			unfolder.getNominalUnfolder()->drawTau();
			d->WriteTObject(c,c->GetName());
			/////check distributions
			c->Clear();

			//all syst curves:

			TDirectory * indir=d->mkdir("sys","sys");
			indir->cd();
			for(size_t sys=0;sys<data.getSystSize()-ign.size();sys++){
				setNameAndTitle(c,name+"_"+data.getSystErrorName(sys)+"_tau");
				unfolder.getSystUnfolder().at(sys)->drawTau();
				indir->WriteTObject(c,c->GetName());
				/////check distributions
				c->Clear();
				setNameAndTitle(c,name+"_"+data.getSystErrorName(sys)+"_lcurve");
				unfolder.getSystUnfolder().at(sys)->drawLCurve();
				indir->WriteTObject(c,c->GetName());
				/////check distributions
				c->Clear();

			}
		}

		////////make cross check
		unfolder.clear();
		std::cout << "Making XCheck for " << name << std::endl;
		d->cd();
		setNameAndTitle(c,name+"_checkUnfoldGen.pdf");

		ztop::container1DUnfold  check=stack->produceXCheckUnfoldingContainer();
		unfolder.unfold(check);

		ztop::container1D checkunfolded=check.getUnfolded();
		float max=checkunfolded.getYMax(true);
		ztop::container1D generated=check.getBinnedGenContainer();
		generated *= (1/check.getLumi());
		if(max < generated.getYMax(true)) max=generated.getYMax(true);
		TH1D * ufgen=checkunfolded.getTH1D("genUF",true,true);
		TH1D*   gen =generated.getTH1D("gen",true,true);
		gen->SetMarkerColor(kRed);
		ufgen->GetYaxis()->SetRangeUser(0,max*1.1);
		ufgen->Draw();
		gen->Draw("same");
		if(debug)
			c->Print(outdirin+name+"_checkUnfoldGen.pdf");
		d->cd();
		//c->Write();
		d->WriteTObject(c,c->GetName());
		d->Close();
		delete d;
		delete ufgen;
		delete gen;
		delete pur;
		delete stab;

	}
	f->WriteTObject(outtree,outtree->GetName());
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
