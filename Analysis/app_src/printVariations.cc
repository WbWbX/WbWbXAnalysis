/*
 * printVariations.cc
 *
 *  Created on: Nov 6, 2013
 *      Author: kiesej
 */



#include "TtZAnalysis/Tools/interface/containerStackVector.h"
#include "TtZAnalysis/Tools/interface/fileReader.h"
#include "TtZAnalysis/Tools/interface/plotter.h"
#include "TtZAnalysis/Tools/interface/simpleRatioPlotter.h"
#include <vector>
#include "TString.h"
#include "TFile.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#define __CINT__
#include "TtZAnalysis/DataFormats/interface/NTBTagSF.h"
#include <fstream>
#include <string>
/*
ztop::container1D extractBinDependence(const ztop::container1D & cont, const size_t& bin, const TString var){ //give var name and var value independently
	using namespace ztop;



	return container1D(); //default return
}
 */
std::vector<TObject *> allobjs_p; //for printing outside main

void drawSystDependenceInBin(const ztop::graph& datadep, const ztop::graph& mcdep, const size_t & bin, const TString & variation,float bincenter){
    /*
	ztop::container1D datadep=data.getDependenceOnSystematic(bin,variation);
	ztop::container1D mcdep=mc.getDependenceOnSystematic(bin,variation);

	float ymax=datadep.getYMax(false);
	float ymin=datadep.getYMin(false);
	if(mcdep.getYMax(false)>ymax) ymax=mcdep.getYMax(false);
	if(mcdep.getYMin(false)<ymin) ymin=mcdep.getYMin(false);

	TH1D * h=datadep.getTH1D(ztop::toTString(bincenter),false,false,false);
	allobjs_p.push_back((TObject*)h);

	TGraphAsymmErrors * gdata=datadep.getTGraph(ztop::toTString(bincenter),false,false,true);
	allobjs_p.push_back((TObject*)gdata);
	TGraphAsymmErrors * gdatastat=datadep.getTGraph(ztop::toTString(bincenter),false,true,true);
	allobjs_p.push_back((TObject*)gdatastat);

	TGraphAsymmErrors * gmc=mcdep.getTGraph(ztop::toTString(bincenter),false,false,true);
	allobjs_p.push_back((TObject*)gmc);
	TGraphAsymmErrors * gmcstat=mcdep.getTGraph(ztop::toTString(bincenter),false,true,true);
	allobjs_p.push_back((TObject*)gmcstat);
     */

    float ymax=datadep.getYMax();
    float ymin=datadep.getYMin();
    if(mcdep.getYMax()>ymax) ymax=mcdep.getYMax();
    if(mcdep.getYMin()<ymin) ymin=mcdep.getYMin();


    TGraphAsymmErrors * gdata=datadep.getTGraph(ztop::toTString(bincenter),false);
    allobjs_p.push_back((TObject*)gdata);
    TGraphAsymmErrors * gdatastat=datadep.getTGraph(ztop::toTString(bincenter),true);
    allobjs_p.push_back((TObject*)gdatastat);

    TGraphAsymmErrors * gmc=mcdep.getTGraph(ztop::toTString(bincenter),false);
    allobjs_p.push_back((TObject*)gmc);
    TGraphAsymmErrors * gmcstat=mcdep.getTGraph(ztop::toTString(bincenter),true);
    allobjs_p.push_back((TObject*)gmcstat);

    float xmin=mcdep.getXMin();
    float xmax=mcdep.getXMax();
    TH1D *h = new TH1D("","",1,xmin-fabs(xmax-xmin)*0.1,xmax+fabs(xmax-xmin)*0.1);
    allobjs_p.push_back(h);
    h->Fill(xmin+(xmax-xmin)/2,ymax);

    h->GetYaxis()->SetRangeUser(ymin-fabs(ymin*0.2),ymax+fabs(ymax*0.2));
    h->Draw("AXIS");
    gmc->GetYaxis()->SetRangeUser(ymin-fabs(ymin*0.2),ymax+fabs(ymax*0.2));
    gmc->SetLineColor(kRed);
    gmcstat->SetLineColor(kRed);
    gmc->SetMarkerColor(kRed);
    gmcstat->SetMarkerColor(kRed);
    gmc->Draw("AP,e1,same");
    gmcstat->Draw("P,e1,same");
    gdata->Draw("P,e1,same");
    gdatastat->Draw("P,e1,same");



}

double sigmtopmulti(double mref,double deltam,bool eighttev=true){ //following mitov NNLO paper
    double a1=0;
    double a2=0;
    if(eighttev){
        a1=-1.1125;
        a2=0.070778;
    }
    else{
        a1=-1.24243;
        a2=0.890776;
    }
    double reldm=mref/(mref+deltam);

    double out= (reldm*reldm*reldm*reldm) * (1+ a1*(deltam)/mref + a2*(deltam/mref)*(deltam/mref));
    std::cout << "sigmtopmulti deltam: " << deltam << "\tmulti="<< out <<std::endl;
    return out;

}

int getColor(size_t i,bool up,bool reset){
    static int counterup=0;
    static int counterdown=0;
    if(reset){
        counterup=0;
        counterdown=0;
        return 0;
    }
    if(up){
        counterup++;
        return kRed+counterup; //to be omple
    }
    counterdown++;
    return kBlue+counterdown;
}

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


ztop::container1DStackVector getCSVFromFile(TString name)
{
    TFile * ftemp=new TFile(stripRoot(name)+".root","read");
    if(ftemp->IsZombie()){
        delete ftemp;
        return ztop::container1DStackVector();
    }
    TTree * ttemp = (TTree*)ftemp->Get("stored_objects");
    ztop::container1DStackVector vtemp;
    vtemp.loadFromTree(ttemp,stripStuff(name));
    delete ttemp;
    delete ftemp;
    return vtemp;
}

ztop::container1DUnfold getCUFFromFile(TString filename,TString plotname)
{
    TFile * ftemp=new TFile(stripRoot(filename)+".root","read");
    TTree * ttemp = (TTree*)ftemp->Get("stored_objects");
    if(!ttemp || ttemp->IsZombie()){
        delete ttemp;
        return ztop::container1DUnfold();
    }
    ztop::container1DUnfold * cuftemp=0;
    if(!ttemp->GetBranch("container1DUnfolds"))
        return ztop::container1DUnfold();
    bool found=false;
    ttemp->SetBranchAddress("container1DUnfolds", &cuftemp);
    for(float n=0;n<ttemp->GetEntries();n++){
        ttemp->GetEntry(n);
        if(cuftemp->getName().EndsWith(plotname)){
            found=true;
            break;
        }
    }
    ztop::container1DUnfold cuf;
    if(cuftemp && found)
        cuf=*cuftemp;

    delete ttemp;
    delete ftemp;
    return cuf;
}
/**
 * arguments:
 *  -v: variation (without _up, _down or value) default MT
 *  -p: plot to use default: m_lb unfold step 8
 *  -s: signal (list in "") not implmented yet!
 */
int main(int argc, char* argv[]){
    using namespace ztop;
    using namespace std;
    AutoLibraryLoader::enable();
    TString plot="m_lb unfold step 8";
    TString variation="MT";
    TString inputfile="";
    TString ignore="";
    containerStackVector csv;
    size_t ninputfiles=0;
    double lumi=-1;
    for(int i=1;i<argc;i++){
        if((TString)argv[i] == "-p"){ //output
            if(i+1<argc){
                plot=(TString)argv[i+1];
                i++;}
            continue;

        }
        else if((TString)argv[i] == "-v"){ //variation
            if(i+1<argc){
                variation=(TString)argv[i+1];
                i++;}
            continue;

        }
        else if((TString)argv[i] == "-l"){ //variation
            if(i+1<argc){
                lumi=atof(argv[i+1]);
                i++;}
            continue;

        }
        else if((TString)argv[i] == "-i"){ //variation
            if(i+1<argc){
                ignore=(TString)argv[i+1];
                i++;}
            continue;

        }

        else if((TString)argv[i] == "-s"){ //signal definition (list in "") NOT IMPLEMENTED YET!!!
            if(i+1<argc){
                //variation=(TString)argv[i+1];
                std::cout << "WARNING: SIGNAL DEF NOT SUPPORTED YET, using standard signal def" <<std::endl;
                i++;}
            continue;

        }
        //else inputfile
        ninputfiles++;
        inputfile=(TString)argv[i];
        csv=getCSVFromFile(inputfile);
    }
    if(ninputfiles>1){
        std::cout << "WARNING: more than 1 input file, using last......" <<std::endl;
    }


    container1DUnfold cuf;
    cuf=getCUFFromFile(inputfile,plot);
    bool wasunfolded=true;
    if(cuf.isDummy()){//
        wasunfolded=false;
        //try to get from stack
        containerStack stack=csv.getStack(plot);
        //get signal container
        cuf=stack.getSignalContainer();
    }
    if(cuf.isDummy()){//still not found
        std::cout << "printvariations: no containers found!!" << std::endl;
        return -1;
    }

    if(lumi<0)
        lumi=cuf.getLumi();


    if(ignore!=""){
        std::vector<TString> errorstoremove;
        for(size_t i=0;i<cuf.getSystSize();i++){
            if(cuf.getSystErrorName(i).Contains(ignore)){
                errorstoremove.push_back(cuf.getSystErrorName(i));
            }
        }
        for(size_t i=0;i<errorstoremove.size();i++){
            std::cout << "will remove error: " << errorstoremove.at(i) << std::endl;
            cuf.removeError(errorstoremove.at(i));
        }
    }

    container1D signal=cuf.getGenContainer();

    //store indices of relevant variations
    std::vector<size_t> sysidx;
    for(size_t i=0;i<signal.getSystSize();i++){
        if(signal.getSystErrorName(i).BeginsWith(variation+"_"))
            sysidx.push_back(i);
    }

    if(sysidx.size() < 1){
        std::cout << "Variation " << variation << " not found, exit" << std::endl;
        return -1;
    }

    //get only nominal
    container1D nominal=signal.getSystContainer(-1);


    //get relevant variations (naming done automatically)
    std::vector<container1D> varcont;
    for(size_t i=0;i<sysidx.size();i++){
        varcont.push_back(signal.getSystContainer(sysidx.at(i)));
    }

    ///PRINT///
    plot.ReplaceAll(" ","_");

    TFile * f = new TFile(plot+"_"+variation+".root","RECREATE");
    TCanvas * c = new TCanvas(plot+"_"+variation,plot+"_"+variation);
    TLegend* leg=new TLegend((Double_t)0.65,(Double_t)0.50,(Double_t)0.95,(Double_t)0.90);
    leg->Clear();
    leg->SetFillStyle(0);
    leg->SetBorderSize(0);
    nominal*=(1/lumi);
    bool renorm=false;
    if(variation=="MT" || variation=="TT_SCALE")
        renorm=true;


    TH1D * h = nominal.getTH1D();
    std::cout << "integral nominal: "<< nominal.integral() << std::endl;
    h->SetMarkerSize(0);
    h->SetLineWidth(2);
    //h->Draw("");
    leg->AddEntry(h,"nominal","lpf");

    std::vector<TH1D *> phists;
    std::vector<container1D> conts;
    std::vector<int> colz;

    conts.push_back(nominal);
    colz.push_back(kBlack);

    double nnorm=1;
    float max=nominal.getYMax(true);
    for(size_t i=0;i<varcont.size();i++){
        TString sysname=signal.getSystErrorName(sysidx.at(i));
        container1D tcont=varcont.at(i);
        tcont*=(1/lumi);
        float tmax=tcont.getYMax(true);
        if(tmax>max)max=tmax;

        if(renorm){
            double sign=-1;
            if(sysname.Contains("_up"))
                sign=1;
            double deltam=0;

            if(sysname.Contains("MT_3_")){
                deltam=3;
                nnorm= sigmtopmulti(172.5,sign*deltam);
            }
            else if(sysname.Contains("MT_6_")){
                deltam=6;
                nnorm= sigmtopmulti(172.5,sign*deltam);
            }
            else if(sysname.Contains("TT_SCALE_up")){
                nnorm=1.025;
            }
            else if(sysname.Contains("TT_SCALE_down")){
                nnorm=0.966;
            }
            tcont *= nnorm;
        }
        conts.push_back(tcont);
        TH1D * htemp=tcont.getTH1D();
        phists.push_back(htemp);
        htemp->SetMarkerSize(0);
        int color=getColor(i,signal.getSystErrorName(sysidx.at(i)).Contains("_up"),false);
        colz.push_back(color);
        htemp->SetLineColor(color);

        //htemp->Draw("same");
        leg->AddEntry(htemp,sysname,"lpf");
        std::cout << "integral " << sysname<< " " << tcont.integral() << std::endl;
        std::cout << "color " << color<<std::endl;

    }
    /*
	h->GetYaxis()->SetRangeUser(0,max*1.1);
	h->Draw("");
	for(size_t i=0;i<phists.size();i++)
		phists.at(i)->Draw("same");
     */
    c->Clear();
    delete c;
    gStyle->SetOptStat(0);
    if(!wasunfolded){
        simpleRatioPlotter p;
        p.setDrawOption("5");
        c=p.plotAll("variations",conts,colz);

        leg->Draw("same");
        c->Write();
        c->Print(plot+"_"+variation+".pdf");
    }
    else{
        container1D ufcont=cuf.getUnfolded();
        std::vector<container1D> ufvars;
        std::vector<size_t> mcsysidx=sysidx;
        sysidx.clear();
        colz.clear();
        getColor(1,true,true); //just reset
        for(size_t i=0;i<ufcont.getSystSize();i++){
            if(ufcont.getSystErrorName(i).BeginsWith(variation+"_")){
                sysidx.push_back(i);
            }
        }
        colz.push_back(kBlack);
        container1D ufwovar=ufcont;
        for(size_t i=0;i<sysidx.size();i++){
            ufwovar.removeError(ufcont.getSystErrorName(sysidx.at(i)));
        }
        ufvars.push_back(ufwovar);
        for(size_t i=0;i<sysidx.size();i++){
            TString sysname=ufcont.getSystErrorName(sysidx.at(i));
            container1D tcont=ufcont.getSystContainer(sysidx.at(i));
            ufvars.push_back(tcont);
            colz.push_back(getColor(i,sysname.Contains("up"),false));
        }
        simpleRatioPlotter p;
        p.setDrawOption("5");
        c=p.plotAll("variations",conts,colz);
        leg->Draw("same");
        c->Print(plot+"_"+variation+"_gen.pdf");

        p.setDrawSame(true);
        p.setDrawOption("e1,P");
        p.plotAll("variations",ufvars,colz);
        c->Print(plot+"_"+variation+".pdf(");
        c->Write();


        c->Clear();
        //do bin-by-bin plots of both distributions
        c->SetName("vars per bin");
        c->SetTitle("vars per bin");
        //prepare MC container (only add relevant syst)
        for(size_t i=0;i<mcsysidx.size();i++)
            nominal.addErrorContainer(signal.getSystErrorName(mcsysidx.at(i)),conts.at(i+1)); //nominal at 0



        int vdivs=1;
        if(ufcont.getNBins()>3)
            vdivs=2;
        if(ufcont.getNBins()>6)
            vdivs=3;

        int hdivs=(int)((float)ufcont.getNBins()/(float)vdivs);
        if(ufcont.getNBins()%vdivs) hdivs++;
        c->Divide(vdivs,hdivs);
        gStyle->SetOptTitle(1);
        std::vector<ztop::graph> sysdepperbind;
        std::vector<ztop::graph> sysdepperbinmc;
        for(size_t i=1;i<ufcont.getBins().size()-1;i++){
            c->cd(i);
            c->cd(i)->SetBottomMargin(0.15);
            c->cd(i)->SetLeftMargin(0.15);

            ztop::graph datadep=ufcont.getDependenceOnSystematic(i,variation);
            ztop::graph mcdep=nominal.getDependenceOnSystematic(i,variation);
            sysdepperbind.push_back(datadep);
            sysdepperbinmc.push_back(mcdep);

            drawSystDependenceInBin(datadep,mcdep,i,variation,ufcont.getBinCenter(i));
        }

        c->Write();
        c->Print(plot+"_"+variation+".pdf");
        c->Clear();
        c->SetName("chi2point per bin");
        c->SetTitle("chi2point per bin");
        c->Divide(vdivs,hdivs);
        //draw single chi2points
        std::vector<ztop::graph> vchi2points;
        for(size_t i=1;i<ufcont.getBins().size()-1;i++){
            c->cd(i);
            c->cd(i)->SetBottomMargin(0.15);
            c->cd(i)->SetLeftMargin(0.15);
            ztop::graph * gd=&sysdepperbind.at(i-1);
            ztop::graph chi2point=gd->getChi2Points(sysdepperbinmc.at(i-1));
            //chi2point.setYAxisName("#Delta/#Delta_{err}");
            vchi2points.push_back(chi2point);
            TGraphAsymmErrors * chi2pg=chi2point.getTGraph(toTString(ufcont.getBinCenter(i)));
            allobjs_p.push_back(chi2pg);
            chi2pg->Draw("AP");
        }
        c->Write();
        c->Print(plot+"_"+variation+".pdf");
        c->Clear();
        size_t binmin=1,binmax=ufcont.getBins().size()-1;
        ztop::graph fullChi2;
        if(binmin>0 && binmin<ufcont.getBins().size()-1 && binmax>0 && binmax<=ufcont.getBins().size()-1){
            fullChi2=vchi2points.at(binmin-1);
            //fullChi2.clear();
            for(size_t i=binmin+1;i<binmax;i++){
                fullChi2=fullChi2.addY(vchi2points.at(i-1));
            }
            fullChi2.setYAxisName("#chi^{2}");

            c->SetName("chi2");
            c->SetTitle("chi2");
            c->SetBottomMargin(0.15);
            c->SetLeftMargin(0.15);
            TGraphAsymmErrors * chi2=fullChi2.getTGraph();
            allobjs_p.push_back(chi2);
            chi2->Draw("AP");
            c->Write();
            chi2->Write();
        }
        else{
            std::cout << "chosen bins not appropriate " << std::endl;
        }
        c->Print(plot+"_"+variation+".pdf)");
    }
    //delete c;
    //p.cleanMem();
    f->Close();
    //for(size_t i=0;i<phists.size();i++)
    //if( phists.at(i)) delete phists.at(i);
    //delete h;
    delete f;

    for(size_t i=0;i<allobjs_p.size();i++)
        delete allobjs_p.at(i);
    //delete c;
}
