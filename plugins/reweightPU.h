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

    PUReweighter(){};
    ~PUReweighter(){};


  void setDataTruePUInput(TH1D dataPUdist){
    datapu_.clear();
    datapu_.push_back(1); //zero bin
    for(int i=1; i< dataPUdist.GetNbinsX()+1 ; i++){
      int bin=dataPUdist.FindBin(i);
      datapu_.push_back(dataPUdist.GetBinContent(bin));
    }
    dataint_=0;
    for(unsigned int i=1;i<datapu_.size();i++) dataint_+=datapu_[i];

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
  
  double getPUweight(double trueBX){
    double weight=1;
    unsigned int bin=(unsigned int)trueBX;
    if(bin>=0 && bin < datapu_.size() && bin < mcpu_.size()){
      weight=(datapu_[bin]/dataint_)/(mcpu_[bin]/mcint_);
    }
    return weight;
  }
  double getPUweight(float trueBX){
    return getPUweight((double) trueBX);
  }
  double getPUweight(int trueBX){
    return getPUweight((double) trueBX);
  }

  ////////hardcoded stuff. Not elegant, but unlikely to change that often

    void setMCDistrSum12(TString scenario="S10"){
      // Distribution used for Summer2012 MC.
      
      
      std::vector<double> V;

      if(scenario == "S07"){
      
	Double_t Summer2012_S07[60] = {2.344E-05, 2.344E-05, 2.344E-05, 2.344E-05, 4.687E-04, 4.687E-04, 7.032E-04, 9.414E-04, 1.234E-03, 1.603E-03, 2.464E-03, 3.250E-03, 5.021E-03, 6.644E-03, 8.502E-03, 1.121E-02, 1.518E-02, 2.033E-02, 2.608E-02, 3.171E-02, 3.667E-02, 4.060E-02, 4.338E-02, 4.520E-02, 4.641E-02, 4.735E-02, 4.816E-02, 4.881E-02, 4.917E-02, 4.909E-02, 4.842E-02, 4.707E-02, 4.501E-02, 4.228E-02, 3.896E-02, 3.521E-02, 3.118E-02, 2.702E-02, 2.287E-02, 1.885E-02, 1.508E-02, 1.166E-02, 8.673E-03, 6.190E-03, 4.222E-03, 2.746E-03, 1.698E-03, 9.971E-04, 5.549E-04, 2.924E-04, 1.457E-04, 6.864E-05, 3.054E-05, 1.282E-05, 5.081E-06, 1.898E-06, 6.688E-07, 2.221E-07, 6.947E-08, 2.047E-08}; 

      V = std::vector<double>(Summer2012_S07,Summer2012_S07  + sizeof(Summer2012_S07)/sizeof(Summer2012_S07[0]));
 
      }

      if(scenario== "S10"){
	Double_t Summer2012_S10[60] = { 2.560E-06, 5.239E-06, 1.420E-05, 5.005E-05, 1.001E-04, 2.705E-04, 1.999E-03, 6.097E-03, 1.046E-02, 1.383E-02, 1.685E-02, 2.055E-02, 2.572E-02, 3.262E-02, 4.121E-02, 4.977E-02, 5.539E-02, 5.725E-02, 5.607E-02, 5.312E-02, 5.008E-02, 4.763E-02, 4.558E-02, 4.363E-02, 4.159E-02, 3.933E-02, 3.681E-02, 3.406E-02, 3.116E-02, 2.818E-02, 2.519E-02, 2.226E-02, 1.946E-02, 1.682E-02, 1.437E-02, 1.215E-02, 1.016E-02, 8.400E-03, 6.873E-03, 5.564E-03, 4.457E-03, 3.533E-03, 2.772E-03, 2.154E-03, 1.656E-03, 1.261E-03, 9.513E-04, 7.107E-04, 5.259E-04, 3.856E-04, 2.801E-04, 2.017E-04, 1.439E-04, 1.017E-04, 7.126E-05, 4.948E-05, 3.405E-05, 2.322E-05, 1.570E-05, 5.005E-06}

	V = std::vector<double>(Summer2012_S10,Summer2012_S10  + sizeof(Summer2012_S10)/sizeof(Summer2012_S10[0]));

      }
      ////////for other PU scenarios!!!

      std::cout << "setting MC pileup distribution to summer12_"+scenario << std::endl;
      //    V.insert(V.begin(),1); //fill zero bin
      mcpu_=V;
      mcint_=0;
      for(unsigned int i=1;i<mcpu_.size();i++) mcint_ += mcpu_[i];

    }

    void clear(){
      datapu_.clear();
      mcpu_.clear();
      std::cout << "\n\nWARNING!!! RESETTING ALL PU WEIGHTS! ONLY FOR TESTING!!\n\n" << std::endl;
    }
    
  
  private:

    std::vector<double> datapu_;
    std::vector<double> mcpu_;
    double dataint_;
    double mcint_;
  };
}


#endif

