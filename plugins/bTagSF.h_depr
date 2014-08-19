#ifndef BTAGSF_H
#define BTAGSF_H

#include <map>
#include "TH2D.h"
#include "TFile.h"
#include "TString.h"
#include "../DataFormats/interface/NTJet.h"
#include "../Tools/interface/miscUtils.h"
#include <iostream>


class bTagSF {

public:
  bTagSF(double WP=0.244);
  ~bTagSF(){};
  
  void setName(TString name){name_=name;};

  void setMakeEff(bool makee){makeeffs_=makee;}
  void setIs2011(bool is){is2011_=is;}
  
  // void writeToTFile(TFile *);
  void fillEff(top::NTJet &, double); //! jet, WP, samplename, norm (important for combined samples)
  void prepareEff(TString , double );

  ///// for reading

  void makeEffs();
  //  void readFromTFile(TFile *);
  void setSampleName(TString);                                  //! better to set it once then to do it for each event or even jet!
  double getSF(std::vector<top::NTJet> & );
  
  void setSystematic(TString);
  
  
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

  double * ptbins_;
  double * etabins_;

  unsigned int nptbins_, netabins_;

  bool is2011_;
  int syst_;

  // bool sfready_;


  double BJetSF ( double pt, double eta, int sys=0, double multiplier=1);
  double CJetSF ( double pt, double eta, int sys=0, double multiplier=1);
  double LJetSF ( double pt, double eta, int sys=0, double multiplier=1);

};




//////////Functions -> make external in stable version

bTagSF::bTagSF(double WP){
  wp_=WP;
  makeeffs_=false;
  histp_=0;
  effhistp_=0;
  syst_=0;

  is2011_=false;

  // init the SF histos and put all info from btag POG here; ONLY here! -- doesnt work
  double ptbins[]  = {30.,40.,50.,60.,70.,80.,100.,120.,160.,210.,260.,320.,400.,500.,670.};    // errors are written to TH2Ds
  double etabins[] = {0.0,0.5,1.0,1.5,2.0,2.4,3.0};
  nptbins_=15;
  netabins_=7;

  ptbins_=ptbins;
  etabins_=etabins;
}


void bTagSF::prepareEff(TString samplename, double norm){

  if(makeeffs_){
    
    //only first run
    tempsamplename_=samplename;
    tempnorm_=norm;
    TString add="";
   
    // std::cout << "preparing histos for " << samplename << std::endl;
    

    if(histos_.find(samplename)==histos_.end()){ //samplename not used before

      TH2D bjets          = TH2D("bjets2D_"+samplename+add ,     "unTagged Bjets", nptbins_-1, ptbins_, netabins_-1, etabins_);    bjets.Sumw2();
      TH2D bjetstagged    = TH2D("bjetsTagged2D_"+samplename+add , "Tagged Bjets", nptbins_-1, ptbins_, netabins_-1, etabins_);    bjetstagged.Sumw2(); 
      TH2D cjets          = TH2D("cjets2D_"+samplename+add , "unTagged Cjets", nptbins_-1, ptbins_, netabins_-1, etabins_);        cjets.Sumw2();
      TH2D cjetstagged    = TH2D("cjetsTagged2D_"+samplename+add , "Tagged Cjets", nptbins_-1, ptbins_, netabins_-1, etabins_);    cjetstagged.Sumw2(); 
      TH2D ljets          = TH2D("ljets2D_"+samplename+add , "unTagged Ljets", nptbins_-1, ptbins_, netabins_-1, etabins_);        ljets.Sumw2();
      TH2D ljetstagged    = TH2D("ljetsTagged2D_"+samplename+add , "Tagged Ljets", nptbins_-1, ptbins_, netabins_-1, etabins_);    ljetstagged.Sumw2();
      
      std::vector<TH2D> temp;
      temp << bjets << bjetstagged << cjets << cjetstagged << ljets << ljetstagged;
      histos_[samplename]=temp;
      
    temp.clear();
    
    TH2D beff          = TH2D("beff2D_"+samplename+add ,     "Bjets eff", nptbins_-1, ptbins_, netabins_-1, etabins_);    beff.Sumw2();
    TH2D ceff          = TH2D("ceff2D_"+samplename+add ,     "Cjets eff", nptbins_-1, ptbins_, netabins_-1, etabins_);    ceff.Sumw2();
    TH2D leff          = TH2D("leff2D_"+samplename+add ,     "Ljets eff", nptbins_-1, ptbins_, netabins_-1, etabins_);    leff.Sumw2();
    
    temp << beff << ceff << leff;
    
    effhistos_[samplename]=temp;
    }
    
  }
  setSampleName(samplename);
  //histos initialized, histp_, effhistp_ set to the right vectors 
}

void bTagSF::fillEff(top::NTJet &jet, double puweight){ // puweights applied later - does that work?!?

  if(!makeeffs_)
    return;

  //  make efficiencies
  if(histp_==0){
    std::cout << "bTagSF::fillEff: no sample set! EXIT!" <<std::endl;
    std::exit (EXIT_FAILURE);
  }

  double fullweight=tempnorm_*puweight;
  //  std::cout << "filling eff histos" << std::endl;

  if(abs(jet.genPartonFlavour()) == 5){ // b jets
    histp_->at(0).Fill(jet.pt(),jet.eta(),fullweight);
    if(jet.btag() > wp_) 
      histp_->at(1).Fill(jet.pt(),jet.eta(),fullweight);
  }
  else if(abs(jet.genPartonFlavour()) == 4){ // c jets
    histp_->at(2).Fill(jet.pt(),jet.eta(),fullweight);
    if(jet.btag() > wp_) 
      histp_->at(3).Fill(jet.pt(),jet.eta(),fullweight);
  }  
  else if(abs(jet.genPartonFlavour()) < 4 && abs(jet.genPartonFlavour()) > 0){ // light jets
    histp_->at(4).Fill(jet.pt(),jet.eta(),fullweight);
    if(jet.btag() > wp_) 
      histp_->at(5).Fill(jet.pt(),jet.eta(),fullweight);
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
  if(effhistp_==0){
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
    double eta=jet.eta();
    
    double sf=1;
    double multi=1;
    unsigned int effh=100;
    if(!is2011_)
      multi=1.5;
    
    if(abs(jet.genPartonFlavour()) == 5){
      effh=0;
      if(abs(syst_>1) || syst_==0)  //default
	sf=BJetSF(pt,eta);
      else if(syst_==-1)            //cjets down
	sf=BJetSF(pt,eta,-1,multi);
      else if(syst_==1)             //cjets up
	sf=BJetSF(pt,eta,1,multi);
    }
    else if(abs(jet.genPartonFlavour()) == 4){
      effh=1;
      if(abs(syst_>1) || syst_==0)  //default
	sf=CJetSF(pt,eta);
      else if(syst_==-1)            //cjets down
	sf=CJetSF(pt,eta,-1,multi);
      else if(syst_==1)             //cjets up
	sf=CJetSF(pt,eta,1,multi);
    }
    else if(abs(jet.genPartonFlavour()) < 4 && abs(jet.genPartonFlavour()) > 0){
      effh=2;
      if(fabs((float)syst_) < 1.1) // default
	sf=LJetSF(pt,eta);
      else if(syst_==-2)            // ljets down
	sf=LJetSF(pt,eta,-1,multi);
      else if(syst_==2)             // ljets up
	sf=LJetSF(pt,eta,1,multi);
    }
    
    if(effh==100) continue; //no defined jet
    
    //do stuff with histp_->at(i)
    int ptbin=0;
    int etabin=0; 
    int bla=0;
    double eff=1;
    effhistp_->at(effh).GetBinXYZ(effhistp_->at(effh).FindBin(pt, eta), ptbin, etabin, bla);
    eff=effhistp_->at(effh).GetBinContent ( ptbin, etabin );

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
	    cont=1;
	    err=1;
	    std::cout << "warning! bTagSF::makeSFs: denominator = 0 in bin (" << binx << ", " << biny << ")" << std::endl;
	  }
	  effhistp_->at(effh).SetBinContent(binx,biny,cont);
	  effhistp_->at(effh).SetBinError(binx,biny,err);
	}
      }
    }
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
    for(unsigned int i=1;i<nptbins_;i++){
      if(ptbins_[i] > pt){
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

#endif
