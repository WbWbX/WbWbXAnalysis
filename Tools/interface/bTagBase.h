#ifndef BTAGBASE_H
#define BTAGBASE_H

#include <map>
#include "TH2D.h"
#include "TFile.h"
#include "TTree.h"
//#include "TString.h"
#include "TtZAnalysis/Tools/interface/miscUtils.h"
#include <iostream>
#include <string>

#include "mathdefs.h"

/*
workflow:
-create object
-set sample name              setSampleName(name)
-fill efficiencies with jets  fillEff(p4,genPartonFlavour,PUweight)
-getEventWeight() returns 1 here
-after loop run makeEffs() once to create histos

-load from file or use already filled object 
-setMakeEff(false)
-(fillEff,makeEffs etc can remain in the loop/at the end of the loop, does nothing now)
-getEventWeight() now returns an additional weight

ALL VALUES FOR WP=0.244
 */
namespace top{

  
  class bTagBase {
  public:
    bTagBase();
    ~bTagBase(){};

    void setIs2011(bool is){is2011_=is;}
  
    void setSampleName(const std::string &); //checks if effs should be made, if sample exists,..

    void setMakeEff(bool makee){makeeffs_=makee;}
    void fillEff(const top::PolarLorentzVector & , int, double, double);      //p4 and genpartonFlavour, bDiscrValue  and PUweight
    void fillEff(const top::LorentzVector & v, int i , double d, double dd){
      top::PolarLorentzVector vp;
      vp=v;
      fillEff(vp,i,d,dd);
    } 
    void makeEffs(); //include check whether stat is too small?

    double getEventWeight(const std::vector< top::PolarLorentzVector > &, std::vector<int>); //jets.p4 & and jets.genpartonFlavour
    
    //maybe make another function that utilises the random tool provided by the btv. it might need efficiencies, so the rest can stay the same, just add a swtch


    void setSystematic(const std::string &); //! up, down, def + heavy, light (check if string contains) - not protected agains weird input like "heavy up down light" etc

    void writeToTFile(TFile *, std::string treename="stored_objects"){}; //! writes whole class to TFile
    void readFromTFile(TFile *, std::string treename="stored_objects"){};  //! reads whole class from TFile

    //don't forget to set makeeffs_ to false after read-in

//   std::vector<TH2D> * getEffHistos(const std::string &){}; //! returns efficiency histos for samplename
//    std::vector<TH2D> * getHistos(const std::string &){};    //! returns histos for samplename

//    TH2D * getEffHisto(const std::string &, const std::string &); //! returns efficiency histo for samplename, histoname
//    TH2D * getHisto(const std::string &, const std::string &);    //! returns histo for samplename, histoname

    // protected: //only commented for testing reasons

    std::map<std::string, std::vector<TH2D> > histos_;      //! bjets, btagged, cjets, ctagged, ljets, ltagged
    std::map<std::string, std::vector<TH2D> > effhistos_;   //! beff, ceff, leff

    std::vector<TH2D> SFs;

    double wp_;
    bool is2011_;
    bool cbflatineta_;
    int syst_;

    bool makeeffs_;

    //some temps
    std::string tempsamplename_;  
    std::vector<TH2D> * histp_;
    std::vector<TH2D> * effhistp_;


    //SFs btv input
    double BJetSF ( double pt, double eta, int sys=0, double multiplier=1);
    double CJetSF ( double pt, double eta, int sys=0, double multiplier=1);
    double LJetSF ( double pt, double eta, int sys=0, double multiplier=1);
    
  };

}
////////////////////////////////////
#endif

