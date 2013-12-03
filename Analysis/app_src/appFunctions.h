/*
 * appFunctions.h
 *
 *  Created on: Nov 26, 2013
 *      Author: kiesej
 */

#ifndef APPFUNCTIONS_H_
#define APPFUNCTIONS_H_

#include "TString.h"
#include <vector>
#include "TtZAnalysis/Tools/interface/containerStackVector.h"
#include "TtZAnalysis/Tools/interface/containerUnfolder.h"
#include "TtZAnalysis/Tools/interface/container1DUnfold.h"
#include "TtZAnalysis/Tools/interface/containerStack.h"
#include "TtZAnalysis/Tools/interface/fileReader.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include <vector>
#include "TString.h"
#include "TFile.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#define __CINT__
#include "TtZAnalysis/DataFormats/interface/NTBTagSF.h"
#include <fstream>
#include <string>
#include "TTree.h"
#include "TDirectory.h"
#include "TCanvas.h"
#include <time.h>


namespace application{


ztop::container1DStackVector getFromFile(TString name)
{
	name.ReplaceAll(".root","");
	AutoLibraryLoader::enable();
	TFile * ftemp=new TFile(name+".root","read");
	TTree * ttemp = (TTree*)ftemp->Get("stored_objects");
	ztop::container1DStackVector vtemp;
	vtemp.loadFromTree(ttemp,name);
	delete ttemp;
	delete ftemp;
	return vtemp;
}

namespace dumpToWWW{
int appendToIndex(TString title, TString description,TString plotdir, TString indexfile){
	using namespace ztop;
	using namespace std;

	string strReplace="<!--NEW ENTRY HERE - DON'T DELETE-->";

	string strNew = "<!--NEW ENTRY HERE - DON'T DELETE-->";
	strNew+="\n";
	strNew+=(string)"<b>" + (string)"<a href=\"" +plotdir+ "/index.html\">"+title+"</a></b>\n<br/>";
	strNew+=description+"\n<br/><br/><hr />\n";
	std::ifstream filein(indexfile); //File to read from
	std::ofstream fileout((indexfile+"_tmp").Data()); //Temporary file
	if(!filein || !fileout)
	{
		cout << "Error opening files!" << endl;
		return -1;
	}

	string strTemp;
	//bool found = false;
	while(filein.good())
	{
		if (!getline( filein, strTemp )) break;

		if(strTemp == strReplace){
			strTemp = strNew;
			//found = true;
		}
		strTemp += "\n";
		fileout << strTemp;
		//if(found) break;
	}
	fileout.close();
	filein.close();
	system(("mv "+indexfile+"_tmp "+indexfile).Data());
	return 0;
}

void makePlotHTML(TString title, TString descr, TString where , ztop::containerStackVector& csv){
	std::ofstream fileout((where+"index.html").Data());

	using namespace std;

	string header=(string)"<html><head><title>"+ (string)title  +(string)"</title></head><body>";
	header+="<b>"+title + "</b>\n<br/>\n<hr /><br/>\n";
	header+=descr;
	header+="\n<hr /><br/>\n";
	fileout << header;

	//some table stuff for the plots
	std::vector<TString> names=csv.getStackNames(true);
	std::vector<TString> fullnames=csv.getStackNames(false);
	string plotspart;

	for(size_t i=0;i<names.size();i++){
		plotspart+="<b>" + names.at(i) + "</b><br/>\n";
		for(size_t j=0;j<fullnames.size();j++){
			if(fullnames.at(j).BeginsWith(names.at(i)+" step")){
				TString newname=fullnames.at(j);
				newname.ReplaceAll(" ","_");
				plotspart+="<img src="+ newname +".png width=\"270\" height=\"175\" alt=" + newname +".png />";
			}
		}
		plotspart+="\n<hr /><br/>\n";
	}
	csv.printAll("",where,".png");

	fileout << plotspart;

	string endfile="</body>";
	fileout << endfile;

	fileout.close();

}


int dumpToWWW(TString infile, TString title,TString description){
	using namespace ztop;

	if(title ==""){
		std::cerr << "at least title needs to be specified" << std::endl;
		return -1;
	}


	TString dumpdir="/afs/desy.de/user/k/kiesej/www/analysisDump/";
	TString index="index.html";
	TString fullIndex=dumpdir+index;

	std::ofstream testtoken((dumpdir+"testtmp").Data());
	if(!testtoken){
		std::cerr << "File " << dumpdir<< " not accessible, no token?" <<std::endl;
		return -1;
	}
	system(("rm -f "+dumpdir+"testtmp").Data());



	TString plotdir=title;
	plotdir.ReplaceAll(" ","_");
	time_t seconds;

	seconds = time (NULL);
	plotdir+=toTString<time_t>(seconds);

	fileReader fr;
	fr.setComment("<br/>");
	std::string shortdescr=description.Data();
	fr.trimcomments(shortdescr);


	appendToIndex(title,shortdescr,plotdir,fullIndex);
	plotdir+="/";
	plotdir=dumpdir+plotdir;
	system(("mkdir "+plotdir).Data());
	containerStackVector csv=getFromFile(infile);
	makePlotHTML(title,  description, plotdir,csv);
	return 0;

}


}//dumptowww

namespace mergeSyst{

TString stripRoot(TString s){
	if(s.EndsWith(".root")){ //cut ".root"
		s.ReplaceAll(".root","");
	}
	return s;
}

TString stripStuff(TString s){      /// needs rewriting for "/" part
	if(s.EndsWith(".root")){ //cut ".root"
		s.ReplaceAll(".root","");
	}
	//get rid of dir
	if(s.Contains("/")){
		ztop::fileReader fr;
		fr.setTrim("/");
		std::string st=s.Data();
		fr.rtrim(st); //make sure no / at end
		size_t pos = st.find("/");
		if( std::string::npos != pos)
		{
			st = st.substr(pos);
		}
		//now its only the filename that remains
		s=TString(st);
	}
	return s;
}
TString getChannel(TString s){
	if(s.Contains("ee_")) return "ee";
	if(s.Contains("emu_")) return "emu";
	if(s.Contains("mumu_")) return "mumu";
	return "";
}

size_t associateChannel(TString s){
	if(s.Contains("ee_")) return 0;
	if(s.Contains("emu_")) return 1;
	if(s.Contains("mumu_")) return 2;
	else return 3; //exception
}

size_t associateEnergy(TString s){
	if(s.Contains("_8TeV_")) return 0;
	if(s.Contains("_7TeV_")) return 1;
	else return 2; //exception
}

/*
 * returns outputfile names
 */
std::vector<TString> mergeSyst(std::vector<TString> names, TString output, TString ignore){
	using namespace ztop;
	using namespace std;
	AutoLibraryLoader::enable();
	vector<TString> outputfilenames;

	containerStackVector::fastadd=false;

	vector<vector<ztop::container1DStackVector> >vnominal; //energy , channels
	vector<ztop::container1DStackVector>  vsysvecs;

	ztop::container1DStackVector def;
	vector<ztop::container1DStackVector> temp;
	for(size_t i=0;i<3;i++)
		temp.push_back(def);
	for(size_t i=0;i<2;i++)
		vnominal.push_back(temp);

	if(ignore!=""){
		std::vector<TString> newnames;
		for(size_t i=0;i<names.size();i++){
			if(names.at(i).Contains(ignore))
				continue;
			newnames.push_back(names.at(i));

		}
		names=newnames;
	}
	//fill nominal

	cout << "loading nominal plots... ";
	for(size_t i=0;i<names.size();i++){
		if(names.at(i).Contains("_nominal")){
			vnominal.at(associateEnergy(names.at(i))).at(associateChannel(names.at(i))) = getFromFile(names.at(i));
			cout << names.at(i) << " ";
		}
	}
	cout << endl;

	//get syst
	cout << "loading syst variations... ";
	for(size_t i=0;i<names.size();i++){
		if(!names.at(i).Contains("_nominal")){
			vsysvecs.push_back(getFromFile(names.at(i)));
			cout << names.at(i) << " ";
		}
	}
	cout << endl;
	///maybe do some BG variations here//add to vsysvecs

	//fill other syst

	cout << "adding variations to nominal..." <<endl;

	for(size_t i=0;i<vsysvecs.size();i++){
		TString name=vsysvecs.at(i).getName();
		cout << "adding variation " << name << std::endl;
		ztop::container1DStackVector  *nominal = & vnominal.at(associateEnergy(name)).at(associateChannel(name));
		nominal->addMCErrorStackVector(vsysvecs.at(i));
	}

	cout << "writing output..." << endl;
	TString outadd="";
	if(output != "")
		outadd="_"+output;

	for(size_t i=0;i<vnominal.size();i++){ //energies
		for(size_t j=0;j<vnominal.at(i).size();j++){ //channels
			ztop::container1DStackVector *outvec=&vnominal.at(i).at(j);
			if(outvec->getName() =="")//in case energies are not considered
				continue;
			outvec->setName(outvec->getName()+outadd+"_syst");
			cout << "writing " << outvec->getName() << endl;
			outputfilenames << outvec->getName()+".root";
			outvec->writeAllToTFile(outvec->getName()+".root",true);
			cout << "syst: " << outvec->getStack(0).getContainer(0).getSystSize() << endl;
		}
	}

	/////////part to merge btagSF


	NTBTagSF btags;
	//check if file exists, if yes, assume btags have been recreated and merge

	if(names.size()>0){
		std::ifstream OutFileTest((names.at(0)+"_btags.root").Data());
		if(OutFileTest) { //btags were created
			cout << "merging btag SF" <<endl;

			for(size_t i=0;i<names.size();i++){
				TString filename=names.at(i)+"_btags.root";
				NTBTagSF temp;

				std::ifstream OutFileTest2(filename.Data());
				if(OutFileTest2){
					temp.readFromTFile(filename);
					btags=btags+temp;
				}
				else{
					cout << "btag file " << filename << " not found" << endl;
				}
			}
			btags.writeToTFile("all_btags.root");
		}

	}

	return outputfilenames;
}

ztop::NTBTagSF mergeBTagSF(std::vector<TString> filenames){

	return ztop::NTBTagSF();
}


}//mergeSyst


namespace unfoldApp{
void setNameAndTitle(TCanvas *c, TString name){
	c->SetTitle(name);
	c->SetName(name);
}
/*
 * add output with total unfolded to some textfile
 */
TString unfold(TString out,TString in, TString signal, bool moreoutput,bool ignoremass){
	TH1::AddDirectory(false);
	AutoLibraryLoader::enable();
	using namespace ztop;
	bool debug=true;

	TString outputstring;
	//formatted string with all \n etc to make a nice line for all syst


	//input checked
	if(!in.EndsWith(".root"))
		in.Append(".root");

	TFile * f = new TFile(in,"READ");
	if(f->IsZombie()){
		std::cout << "file could not be opened/does not exist. exit" << std::endl;
		throw std::runtime_error( "file could not be opened/does not exist. exit");
	}
	if(!(f->Get("stored_objects"))){
		std::cout << "tree stored_objects not found in input file. exit" << std::endl;
		throw std::runtime_error("tree stored_objects not found in input file. exit" );
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
	if(out.Length()>0)
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
		throw std::runtime_error("containerStackVectors not found! exit");
	}
	if(!pcsv){
		csvname.ReplaceAll("_syst","");
		pcsv=pcsv->getFromTree(t,csvname);
	}
	if(!pcsv){
		std::cout << "containerStackVector not found! exit" << std::endl;
		throw std::runtime_error("containerStackVector not found! exit" );
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
			outputstring+=name+"\n\n";
			for(size_t bin=1;bin<=unfolded.getNBins();bin++){
				//unfolded.coutBinContent(bin);
				outputstring+=data.coutUnfoldedBinContent(bin); //dangerous
				outputstring+="\n\n";
				std::cout << "\n\n"  << std::endl;
			}
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
		setNameAndTitle(c,name+"_pur_stab");
		ztop::container1DUnfold cuf=stack->getSignalContainer();
		cuf.checkCongruentBinBoundariesXY();
		TH1D * pur=cuf.getPurity().getTH1D("purity",false,false,false);
		TH1D * stab=cuf.getStability().getTH1D("stability",false,false,false);
		if(pur){
			stab->SetLineColor(kRed);
			stab->SetMarkerColor(kRed);
			pur->GetYaxis()->SetRangeUser(0,1);
			pur->Draw();
			stab->Draw("same");
		}
		d->WriteTObject(c,((TString)c->GetName()+"_purStab").Data());


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
		ztop::container1D generated=check.getGenContainer();
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

	return outputstring;

}


}//unfoldApp

}//namespace



#endif /* APPFUNCTIONS_H_ */
