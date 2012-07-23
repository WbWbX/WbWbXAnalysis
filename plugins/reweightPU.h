#ifndef reweightPU_h
#define reweightPU_h

#include "TH1D.h"
#include "TFile.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include "TString.h"


//! PU reweighting. Now provides a PU variation of +-1.
//! contradicts to the official recommendation to use different data distributions with varied total inelastic cross section by +-5%
//! so reweighting is changed by changing the input distributions provided at (naf afs) kiesej/public

namespace top{

  class PUReweighter {

  public:

    PUReweighter(){init_=false;};
    ~PUReweighter(){};

    //void setDataTruePUInput(TH1D);
    //void setDataTruePUInput(const char *);
    //double getPUweight(double,int);
    //void setMCDistribution(double *);
    //void setMCDistrSum12();


    //root doesn't allow function prototypes and then declarations outside the class in some cases... why?


  void setDataTruePUInput(TH1D dataPUdist){
    dataPUdist_=dataPUdist;
    init_=true;
    std::cout << "Data PU distribution set" << std::endl;
  }

  void setDataTruePUInput(const char * rootfile){
    TFile *f = new TFile(rootfile);
    TH1D *datapileup = (TH1D*)f->Get("pileup");
    setDataTruePUInput(*datapileup);
    delete datapileup;
    f->Close();
    delete f;
  }
  
  double getPUweight(double trueBX, int downDefUp=1){
    double weight=1;
    unsigned int bin=(unsigned int)trueBX;
    if(bin>0 && bin < reweights_.size()){
      if(downDefUp == 1){
	weight=reweights_[bin];
      }
      else if(downDefUp == 0){
	weight=reweightsdown_[bin];
      }
      else if(downDefUp == 2){
	weight=reweightsup_[bin];
      }
      else{
	std::cout << "PileUpReweighter: getPUweight(trueBX, downDefaultUp) downDefaultUp must be 0 (down), 1 (default) or 2 (up)" << std::endl;
      }
    }
    return weight;
  }
  double getPUweight(float trueBX, int downDefUp=1){
    return getPUweight((double) trueBX, downDefUp);
  }
  double getPUweight(int trueBX, int downDefUp=1){
    return getPUweight((double) trueBX, downDefUp);
  }

  void setMCDistribution(std::vector<double>  distr){
    if(init_){
      int sizeofArr= distr.size(); //sizeof(distr) / sizeof(distr[0]);

      std::cout << sizeofArr << std::endl;

      double mcint = 0;
      for(int i=0;i<sizeofArr;i++){
	mcint+=distr[i];
      }

      double dataint=dataPUdist_.Integral();

      int maxBins=std::min(sizeofArr+1, dataPUdist_.GetNbinsX()+1);

      //reset
      reweights_.clear();
      reweightsup_.clear();
      reweightsdown_.clear();
      //fill the zero bin
      reweights_.push_back(1);
      reweightsup_.push_back(1);
      reweightsdown_.push_back(1);

      for(int i=1; i< maxBins ; i++){
	int bin=dataPUdist_.FindBin(i);
	double dataCont=dataPUdist_.GetBinContent(bin)/dataint;


	reweights_.push_back(       dataCont/(distr[i-1] / mcint));

	if(i-2 >= 0)
	  reweightsup_.push_back(   dataCont/(distr[i-2]   / mcint));
	else
	  reweightsup_.push_back( 1);

	if(i<sizeofArr)
	  reweightsdown_.push_back( dataCont/(distr[i]  / mcint));
	else
	  reweightsdown_.push_back( 1 );
      }
   
    }
    else{
      std::cout << "Warning! No data pileup input set!" << std::endl;
    }
  }


  ////////hardcoded stuff. Not elegant, but unlikely to change that often

    void setMCDistrSum12(TString scenario="S07"){
      // Distribution used for Summer2012 MC.
      
      
      std::vector<double> V;

      if(scenario == "S07"){
      
      Double_t Summer2012[60] = {
	2.344E-05,
	2.344E-05,
	2.344E-05,
	2.344E-05,
	4.687E-04,
	4.687E-04,
	7.032E-04,
	9.414E-04,
	1.234E-03,
	1.603E-03,
	2.464E-03,
	3.250E-03,
	5.021E-03,
	6.644E-03,
	8.502E-03,
	1.121E-02,
	1.518E-02,
	2.033E-02,
	2.608E-02,
	3.171E-02,
	3.667E-02,
	4.060E-02,
	4.338E-02,
	4.520E-02,
	4.641E-02,
	4.735E-02,
	4.816E-02,
	4.881E-02,
	4.917E-02,
	4.909E-02,
	4.842E-02,
	4.707E-02,
	4.501E-02,
	4.228E-02,
	3.896E-02,
	3.521E-02,
	3.118E-02,
	2.702E-02,
	2.287E-02,
	1.885E-02,
	1.508E-02,
	1.166E-02,
	8.673E-03,
	6.190E-03,
	4.222E-03,
	2.746E-03,
	1.698E-03,
	9.971E-04,
	5.549E-04,
	2.924E-04,
	1.457E-04,
	6.864E-05,
	3.054E-05,
	1.282E-05,
	5.081E-06,
	1.898E-06,
	6.688E-07,
	2.221E-07,
	6.947E-08,
	2.047E-08
      }; 

      V = std::vector<double>(Summer2012,Summer2012  + sizeof(Summer2012)/sizeof(Summer2012[0]));
 
      }
      ////////for other PU scenarios!!!

      std::cout << "setting MC pileup distribution to summer12" << std::endl;


      setMCDistribution(V);

  }
    
  
  private:

    TH1D dataPUdist_;
    std::vector<double> reweights_;
    std::vector<double> reweightsup_;
    std::vector<double> reweightsdown_;
    bool init_;
  };
}


#endif

