#ifndef BTAGSF_H
#define BTAGSF_H

#include <map>
#include "TH2D.h"
#include "TFile.h"
#include "TString.h"
#include "../DataFormats/interface/NTJet.h"


class bTagSF {

public:
  bTagSF(double WP=0.244);
  ~bTagSF(){};
  
  void setName(TString name){name_=name;};
  
  // void writeToTFile(TFile *);
  void fillSF(top::NTJet &, double, TString, double); //! jet, WP, samplename, norm (important for combined samples)


  ///// for reading

  //  void readFromTFile(TFile *);
  void setSampleName(TString);                                  //! better to set it once then to do it for each event or even jet!
  double getSF(top::NTJet &, double, double, TString samplename="");
  
  
  
private:
  TH2D bjets,bjetstagged;
  TH2D cjets,cjetstagged;
  TH2D ljets,ljetstagged;
  TString name_;
  
  std::map<TString,std::vector<TH2D> > histos_; // bjets, btagged, cjets, ctagged, ljets, ltagged
  
  std::map<TString,std::vector<TH2D> >::iterator sampleit_;
  std::vector<TH2D> * histp_;

  double wp_;
  double tempnorm_;

  boo sampleusedbefore_;
  
};




//////////Functions -> make external in stable version

bTagSF::bTagSF(double WP){
  wp_=WP;
  sampleusedbefore_=false;
}


void bTagSF::prepareSF(TString samplename, double norm){

  //only first run

  tempnorm_=norm;
  TString add="";
  if(histos_.find(samplename)){
    sampleusedbefore_=true;
    add="_2";
  }

  double ptbins[]  = {0.,30.,40.,50.,60.,70.,80.,100.,120.,160.,210.,260.,320.,400.,500.,670.,1000.,2000.};    // errors are written to TH2Ds
  double etabins[] = {0.0,0.5,1.0,1.5,2.0,2.4,3.0};

 

  //produce temp histograms
  bjets          = TH2D("bjets2D_"+samplename+add ,     "unTagged Bjets", 17, ptbins, 6, etabins);    bjets.Sumw2();
  bjetstagged    = TH2D("bjetsTagged2D_"+samplename+add , "Tagged Bjets", 17, ptbins, 6, etabins);    bjetstagged.Sumw2();

  cjets          = TH2D("cjets2D_"+samplename+add , "unTagged Cjets", 17, ptbins, 6, etabins);        cjets.Sumw2();
  cjetstagged    = TH2D("cjetsTagged2D_"+samplename+add , "Tagged Cjets", 17, ptbins, 6, etabins);    cjetstagged.Sumw2();

  ljets          = TH2D("ljets2D_"+samplename+add , "unTagged Ljets", 17, ptbins, 6, etabins);        ljets.Sumw2();
  ljetstagged    = TH2D("ljetsTagged2D_"+samplename+add , "Tagged Ljets", 17, ptbins, 6, etabins);    ljetstagged.Sumw2();

}

void bTagSF::fillSF(top::NTJet &jet, double puweight){

  /// make efficiencies



  if(abs(jet.genPartonFlavour()) < 4 && abs(jet.genPartonFlavour()) > 0){ // light jets
    ljets.Fill(jet.pt(),jet.eta(),puweight);
    if(jet.btag() > WP) ljetstagged.Fill(jet.pt(),jet.eta(),puweight);
  }
  else if(abs(jet.genPartonFlavour()) == 4){ // c jets


  }
  else if(abs(jet.genPartonFlavour()) == 5){ // b jets


  }
  // else if 0 or 6 do nothing... top jets... well.. ;)


}

void bTagSF::setSampleName(TString samplename){
  sampleit_=histos_.find(samplename);
  histp_ = & (sampleit_->second);
  //now, sampleit_->second gets the histogram vector
}

double bTagSF::getSF(top::NTJet &, double, TString samplename){
  if(samplename != ""){
    setSampleName(samplename);
  }

  //do stuff with histp_->at(i)
  return 9.;
}

#endif
