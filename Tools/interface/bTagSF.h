#ifndef BTAGSF_H
#define BTAGSF_H

#include <map>
#include "TH2D.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TtZAnalysis/DataFormats/interface/NTJet.h"
#include "TtZAnalysis/Tools/interface/miscUtils.h"
#include <iostream>

namespace top{

class bTagSF {

public:
  bTagSF(double WP=0.244);
  ~bTagSF(){};
  
  void setName(TString name){name_=name;};
  TString getName(){return name_;}

  void setMakeEff(bool makee){makeeffs_=makee;}
  void setIs2011(bool is){is2011_=is;}
  
  void fillEff(top::NTJet &, double); //! jet, WP, samplename, norm (important for combined samples)
  void prepareEff(TString , double );

  ///// for reading

  void makeEffs();
  //  
  void setSampleName(TString);                                  //! better to set it once then to do it for each event or even jet!
  double getSF(std::vector<top::NTJet> & );
  
  void setSystematic(TString);
  

  void writeToTFile(TFile *, TString treename="stored_objects");
  void readFromTFile(TFile *, TString name="no_name",TString treename="stored_objects");  

  std::vector<TH2D> * getEffHistos(TString );


private:
  // TH2D bjets,bjetstagged;
  // TH2D cjets,cjetstagged;
  // TH2D ljets,ljetstagged;
  TString name_;
  
  std::map<TString,std::vector<TH2D> > histos_;      //! bjets, btagged, cjets, ctagged, ljets, ltagged
  std::map<TString,std::vector<TH2D> > effhistos_;   //! beff, ceff, leff
  std::map<TString,std::vector<TH2D> >::iterator sampleit_, effit_;
  std::vector<TH2D> * histp_;
  std::vector<TH2D> * effhistp_;

  std::vector<TH2D> SFs;

  double wp_;
  double tempnorm_;

  bool makeeffs_;

  TString tempsamplename_;  

  std::vector<double> ptbins_;
  std::vector<double> etabins_;

  bool cbflatineta_;

  // unsigned int nptbins_, netabins_;

  bool is2011_;
  int syst_;

  // bool sfready_;


  double BJetSF ( double pt, double eta, int sys=0, double multiplier=1);
  double CJetSF ( double pt, double eta, int sys=0, double multiplier=1);
  double LJetSF ( double pt, double eta, int sys=0, double multiplier=1);

};


}
#endif

#define EXSRC
#ifndef EXSRC

//////////Functions -> make external in stable version

bTagSF::bTagSF(double WP){

  cbflatineta_=true;

  wp_=WP;
  makeeffs_=false;
  histp_=0;
  effhistp_=0;
  syst_=0;

  is2011_=false;

  // init the SF histos and put all info from btag POG here; ONLY here! -- doesnt work
  double  ptbins[]  = {30.,40.,50.,60.,70.,80.,100.,120.,160.,210.,260.,320.,400.,500.,670.};    // errors are written to TH2Ds
  unsigned int npt=15;
  double  etabins[] = {0.0,0.5,1.0,1.5,2.0,2.4,3.0};
  unsigned int neta=7;


  ptbins_.clear();
  etabins_.clear();
  for(unsigned int i=0;i<npt;i++)  ptbins_  << ptbins[i];
  for(unsigned int i=0;i<neta;i++) etabins_ << etabins[i];
 
}


void bTagSF::prepareEff(TString samplename, double norm){

  if(makeeffs_){
    
    //only first run
    tempsamplename_=samplename;
    tempnorm_=norm;
    TString add="";
   
    

    if(histos_.find(samplename)==histos_.end()){ //samplename not used before
      TH1::AddDirectory(kFALSE);

      std::cout << "preparing b-tag efficiency histos for " << samplename << std::endl;

      double tempptbins[ptbins_.size()];
      double tempetabins[etabins_.size()];
      for(unsigned int i=0;i<ptbins_.size();i++)  tempptbins[i]=ptbins_.at(i);
      for(unsigned int i=0;i<etabins_.size();i++) tempetabins[i]=etabins_.at(i);

      TH2D bjets          = TH2D("bjets2D_"+samplename ,     "unTagged Bjets", ptbins_.size()-1, tempptbins, etabins_.size()-1, tempetabins);    bjets.Sumw2();
      TH2D bjetstagged    = TH2D("bjetsTagged2D_"+samplename , "Tagged Bjets", ptbins_.size()-1, tempptbins, etabins_.size()-1, tempetabins);    bjetstagged.Sumw2(); 
      TH2D cjets          = TH2D("cjets2D_"+samplename , "unTagged Cjets", ptbins_.size()-1, tempptbins, etabins_.size()-1, tempetabins);        cjets.Sumw2();
      TH2D cjetstagged    = TH2D("cjetsTagged2D_"+samplename , "Tagged Cjets", ptbins_.size()-1, tempptbins, etabins_.size()-1, tempetabins);    cjetstagged.Sumw2(); 
      TH2D ljets          = TH2D("ljets2D_"+samplename , "unTagged Ljets", ptbins_.size()-1, tempptbins, etabins_.size()-1, tempetabins);        ljets.Sumw2();
      TH2D ljetstagged    = TH2D("ljetsTagged2D_"+samplename , "Tagged Ljets", ptbins_.size()-1, tempptbins, etabins_.size()-1, tempetabins);    ljetstagged.Sumw2();
      
      std::vector<TH2D> temp;
      temp << bjets << bjetstagged << cjets << cjetstagged << ljets << ljetstagged;
      histos_[samplename]=temp;
      
    temp.clear();
    
    TH2D beff          = TH2D("beff2D_"+samplename ,     "Bjets eff", ptbins_.size()-1, tempptbins, etabins_.size()-1, tempetabins);    beff.Sumw2();
    TH2D ceff          = TH2D("ceff2D_"+samplename ,     "Cjets eff", ptbins_.size()-1, tempptbins, etabins_.size()-1, tempetabins);    ceff.Sumw2();
    TH2D leff          = TH2D("leff2D_"+samplename ,     "Ljets eff", ptbins_.size()-1, tempptbins, etabins_.size()-1, tempetabins);    leff.Sumw2();
    
    temp << beff << ceff << leff;
    
    effhistos_[samplename]=temp;
    }
    else{
      std::cout << "loading b-tag efficiency histos for " << samplename << std::endl;
    }
  }
  setSampleName(samplename);
  //histos initialized, histp_, effhistp_ set to the right vectors 
}

void bTagSF::fillEff(top::NTJet &jet, double puweight){ // puweights applied later - does that work?!?

  if(!makeeffs_ || jet.genPartonFlavour() == 0 )
    return;

  //  make efficiencies
  if(histp_==0){
    std::cout << "bTagSF::fillEff: no sample set or loaded! EXIT!" <<std::endl;
    std::exit (EXIT_FAILURE);
  }

  double fullweight=puweight;//tempnorm_*puweight;

  double abs_eta= fabs(jet.eta());

  if(jet.genPartonFlavour() == 5 || jet.genPartonFlavour() == -5){ // b jets
    if(cbflatineta_) abs_eta=1;
    histp_->at(0).Fill(jet.pt(),abs_eta,fullweight);
    if(jet.btag() > wp_) 
      histp_->at(1).Fill(jet.pt(),abs_eta,fullweight);
  }
  else if(jet.genPartonFlavour() == 4 || jet.genPartonFlavour() == -4){ // c jets
    if(cbflatineta_) abs_eta=1;
    histp_->at(2).Fill(jet.pt(),abs_eta,fullweight);
    if(jet.btag() > wp_) 
      histp_->at(3).Fill(jet.pt(),abs_eta,fullweight);
  }  
  else if(fabs(jet.genPartonFlavour()) < 4 && fabs(jet.genPartonFlavour()) > 0){ // light jets
    histp_->at(4).Fill(jet.pt(),abs_eta,fullweight);
    if(jet.btag() > wp_) 
      histp_->at(5).Fill(jet.pt(),abs_eta,fullweight);
  }
  else{ // 

  }

}

void bTagSF::setSampleName(TString samplename){
  sampleit_=histos_.find(samplename);
  if(sampleit_!=histos_.end())
    histp_ = & (sampleit_->second);
  else
    histp_ = 0;
  effit_=effhistos_.find(samplename);
  if(effit_!=effhistos_.end())
    effhistp_ = & (effit_->second);
  else
    effhistp_ = 0;
}

double bTagSF::getSF(std::vector<top::NTJet> & jets){
  if(makeeffs_) return 1.;

  if(! effhistp_){
    std::cout << "bTagSF::getSF: no sample set! EXIT!" <<std::endl;
    std::exit (EXIT_FAILURE);
  }

  double OneMinusEff=1;
  double OneMinusSEff=1;
  
  for(unsigned int jetiter=0;jetiter<jets.size();jetiter++){
    top::NTJet jet=jets.at(jetiter);

    if(jet.genPartonFlavour() == 0)
      return 1;

    double pt=jet.pt();
    double abs_eta=fabs(jet.eta());

    if(cbflatineta_) abs_eta=1;
    
    double sf=1;
    double multi=1;
    unsigned int effh=100;
    if(!is2011_)
      multi=1.5;
    
    if(abs(jet.genPartonFlavour()) == 5){
      effh=0;

      if(abs(syst_>1) || syst_==0)  //default
	sf=BJetSF(pt,abs_eta);
      else if(syst_==-1)            //bjets down
	sf=BJetSF(pt,abs_eta,-1,multi);
      else if(syst_==1)             //bjets up
	sf=BJetSF(pt,abs_eta,1,multi);
    }
    else if(abs(jet.genPartonFlavour()) == 4){
      effh=1;
      if(abs(syst_>1) || syst_==0)  //default
	sf=CJetSF(pt,abs_eta);
      else if(syst_==-1)            //cjets down
	sf=CJetSF(pt,abs_eta,-1,multi);
      else if(syst_==1)             //cjets up
	sf=CJetSF(pt,abs_eta,1,multi);
    }
    else if(abs(jet.genPartonFlavour()) < 4 && abs(jet.genPartonFlavour()) > 0){
      effh=2;
      if(fabs((float)syst_) < 1.1) // default
	sf=LJetSF(pt,abs_eta);
      else if(syst_==-2)            // ljets down
	sf=LJetSF(pt,abs_eta,-1,multi);
      else if(syst_==2)             // ljets up
	sf=LJetSF(pt,abs_eta,1,multi);
    }
    
    if(effh==100) continue; //no defined jet
    

    int ptbin=0;
    int etabin=0; 
    int bla=0;
    double eff=1;
    effhistp_->at(effh).GetBinXYZ(effhistp_->at(effh).FindBin(pt, abs_eta), ptbin, etabin, bla);
    if(effh==0){
       if(syst_==1)
	eff=effhistp_->at(effh).GetBinContent ( ptbin, etabin ) + effhistp_->at(effh).GetBinError ( ptbin, etabin );
      if(syst_==-1)
	eff=effhistp_->at(effh).GetBinContent ( ptbin, etabin ) - effhistp_->at(effh).GetBinError ( ptbin, etabin );
      else
	eff=effhistp_->at(effh).GetBinContent ( ptbin, etabin );
    }
    else if(effh==1){
      if(syst_==1)
	eff=effhistp_->at(effh).GetBinContent ( ptbin, etabin ) + effhistp_->at(effh).GetBinError ( ptbin, etabin );
      if(syst_==-1)
	eff=effhistp_->at(effh).GetBinContent ( ptbin, etabin ) - effhistp_->at(effh).GetBinError ( ptbin, etabin );
      else
	eff=effhistp_->at(effh).GetBinContent ( ptbin, etabin );
    }
    else{
      if(syst_==2)
	eff=effhistp_->at(effh).GetBinContent ( ptbin, etabin ) + effhistp_->at(effh).GetBinError ( ptbin, etabin );
      if(syst_==-2)
	eff=effhistp_->at(effh).GetBinContent ( ptbin, etabin ) - effhistp_->at(effh).GetBinError ( ptbin, etabin );
      else
	eff=effhistp_->at(effh).GetBinContent ( ptbin, etabin );
    }
    OneMinusEff = OneMinusEff* ( 1-eff );
    OneMinusSEff= OneMinusSEff* ( 1-sf*eff );
  }

  return ( 1.-OneMinusSEff ) / ( 1.-OneMinusEff );
}


void bTagSF::makeEffs(){
  if(makeeffs_ && histp_ && effhistp_){ // protection
    for(int binx=1;binx<=effhistp_->at(0).GetNbinsX()+1;binx++){
      for(int biny=1;biny<=effhistp_->at(0).GetNbinsY()+1;biny++){
	for(int effh=0;effh<3;effh++){
	  double cont=1;
	  double err=1;
	  if(histp_->at(effh*2).GetBinContent(binx,biny) !=0){
	    cont=histp_->at((effh*2)+1).GetBinContent(binx,biny) / histp_->at(effh*2).GetBinContent(binx,biny);
	    err=sqrt(cont*(1-cont)/ histp_->at(effh*2).GetBinContent(binx,biny));
	  }
	  else{
	    cont=0;
	    if(effh!=0){
	      cont=0;
	    }
	    err=1;
	    //  std::cout << "warning! bTagSF::makeSFs: denominator = 0 in bin (" << binx << ", " << biny << ") for effhisto: " << effh  << std::endl;
	  }
	  effhistp_->at(effh).SetBinContent(binx,biny,cont);
	  effhistp_->at(effh).SetBinError(binx,biny,err);

	}
      }
    }
    effhistp_->at(0).Write();
    effhistp_->at(1).Write();
    effhistp_->at(2).Write();
  }
}



void bTagSF::setSystematic(TString ID){ //consider SF_c and SF_b as correlated, SF_light uncorrelated to SF_b and SF_c

  if(ID.Contains("heavy")){
    if(ID.Contains("up")){
      syst_=1;
    }
    if(ID.Contains("down")){
      syst_=-1;
    }
  }
  if(ID.Contains("light")){
    if(ID.Contains("up")){
      syst_=2;
    }
    if(ID.Contains("down")){
      syst_=-2;
    }
  }
}









///////// put all the SFs here!!!!

double bTagSF::BJetSF( double pt, double eta , int sys, double multiplier){
    //CSVL b-jet SF
    //From BTV-11-004 and https://twiki.cern.ch/twiki/pub/CMS/BtagPOG/SFb-mujet_payload.txt

  double abs_eta=fabs(eta);
  abs_eta++; // not used right now;

    if ( pt < 30 ) pt = 30;
    if ( pt > 670 ) pt = 670;

    
    double sf= 1.02658*((1.+(0.0195388*pt))/(1.+(0.0209145*pt)));
    if(sys==0)
      return sf;

    double SFb_error[] = { 0.0188743, 0.0161816, 0.0139824, 0.0152644, 0.0161226, 0.0157396, 0.0161619, 0.0168747, 0.0257175, 0.026424, 0.0264928, 0.0315127, 0.030734, 0.0438259 };
    double abserr=0;
    for(unsigned int i=1;i<ptbins_.size();i++){
      if(ptbins_.at(i) > pt){
	abserr=SFb_error[i-1];
      }
    }
    if(sys<0)
      return sf-(abserr*multiplier);
    if(sys>0)
      return sf+(abserr*multiplier);
    else 
      return 1;
}



double bTagSF::CJetSF ( double pt, double eta, int sys, double multiplier){
  return BJetSF(pt, eta,sys ,multiplier*2);
}


double bTagSF::LJetSF ( double pt, double eta, int sys, double multiplier){    
//CSVL ligth jet mistag SF.
    //From BTV-11-004 and https://twiki.cern.ch/twiki/pub/CMS/BtagPOG/SFlightFuncs.C

    double eta_abs = fabs(eta);


    multiplier++; // to avoid warnings

    double sf=1;

    if(is2011_){
      if ( pt > 670 ) {
        sf =(((0.956023+(0.000825106*pt))+(-3.18828e-06*(pt*pt)))+(2.81787e-09*(pt*(pt*pt))));
      } 
      else {
        if ( eta_abs <= 0.5 ) {
	  sf =(((0.994425+(-8.66392e-05*pt))+(-3.03813e-08*(pt*pt)))+(-3.52151e-10*(pt*(pt*pt)))) ;
        } else if ( eta_abs <= 1.0 ) {
	  sf =(((0.998088+(6.94916e-05*pt))+(-4.82731e-07*(pt*pt)))+(1.63506e-10*(pt*(pt*pt)))) ;
        } else if ( eta_abs <= 1.5 ) {
	  sf =(((1.00294+(0.000289844*pt))+(-7.9845e-07*(pt*pt)))+(5.38525e-10*(pt*(pt*pt)))) ;
        } else {
	  sf =(((0.979816+(0.000138797*pt))+(-3.14503e-07*(pt*pt)))+(2.38124e-10*(pt*(pt*pt)))) ;
        }
      }
    }    
    else{
      if ( pt > 670 ) {
        sf =((((0.956023+(0.000825106*pt))+(-3.18828e-06*(pt*pt)))+(2.81787e-09*(pt*(pt*pt)))) * (0.979396 + 0.000205898*pt + 2.49868e-07*pt*pt)) ;
      } 
      else {
        if ( eta_abs <= 0.5 ) {
	  sf =((((0.994425+(-8.66392e-05*pt))+(-3.03813e-08*(pt*pt)))+(-3.52151e-10*(pt*(pt*pt)))) * (0.979396 + 0.000205898*pt + 2.49868e-07*pt*pt)) ;
        } else if ( eta_abs <= 1.0 ) {
	  sf =((((0.998088+(6.94916e-05*pt))+(-4.82731e-07*(pt*pt)))+(1.63506e-10*(pt*(pt*pt)))) * (0.979396 + 0.000205898*pt + 2.49868e-07*pt*pt)) ;
        } else if ( eta_abs <= 1.5 ) {
	  sf =((((1.00294+(0.000289844*pt))+(-7.9845e-07*(pt*pt)))+(5.38525e-10*(pt*(pt*pt)))) * (0.979396 + 0.000205898*pt + 2.49868e-07*pt*pt)) ;
        } else {
	  sf =((((0.979816+(0.000138797*pt))+(-3.14503e-07*(pt*pt)))+(2.38124e-10*(pt*(pt*pt)))) * (0.979396 + 0.000205898*pt + 2.49868e-07*pt*pt)) ;
        }
      }
      
    }
    if(sys==0)
      return sf;
    else
      return sf;
    //just to avoid warnings
    //here there would be space for syst stuff

}

  void bTagSF::writeToTFile(TFile * f, TString treename){
    TString name;
    if(name_=="") name="no_name";
    else name = name_;

    f->cd();
    TTree * t=0;
    if(f->Get(treename)){
      t = (TTree*) f->Get(treename);
    }
    else{
    t = new TTree(treename,treename);
    }
    if(t->GetBranch("allBTagSF")){ //branch does not exist yet
      bTagSF * tag = this;
      t->SetBranchAddress("allBTagSF",&tag);
    }
    else{
      t->Branch("allBTagSF",this);
      std::cout << "bTagSF::writeAllToTFile: added branch" << std::endl;
    }
    t->Fill();
    t->Write("",TObject::kOverwrite);
    delete t;

  }
  void bTagSF::readFromTFile(TFile * f, TString name, TString treename){
    bTagSF * tag=0;
    TTree * t = (TTree*)f->Get(treename);
    t->SetBranchAddress("allBTagSF", &tag);
    bool found=false;
    for(float n=0;n<t->GetEntries();n++){
      t->GetEntry(n);
      if(tag->getName() == name){
	*this=*tag; //copy
	found=true;
	break;
      }
    }
    if(!found) std::cout << "bTagSF::readFromTFile: " << name << " not found in tree " << f->GetName() << "->" << treename << std::endl;

  }


  std::vector<TH2D> * bTagSF::getEffHistos(TString samplename){
    setSampleName(samplename);
    return effhistp_;
  }

}

#endif
