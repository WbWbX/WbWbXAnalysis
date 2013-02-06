#include "TH1D.h"
#include "TFile.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include "TString.h"
#include "PUReweighter.h"

//! PU reweighting. Now provides a PU variation of +-1.
//! contradicts to the official recommendation to use different data distributions with varied total inelastic cross section by +-5%
//! so reweighting is changed by changing the input distributions provided at (naf afs) kiesej/public

void PUReweighter::setDataTruePUInput(TH1* dataPUdist){
    datapu_.clear();
    datapu_.push_back(1); //zero bin
    for(int i = 1; i< dataPUdist->GetNbinsX()+1 ; ++i){
        int bin = dataPUdist->FindBin(i);
        datapu_.push_back(dataPUdist->GetBinContent(bin));
    }
    dataint_ = 0;
    for(size_t i=1; i<datapu_.size(); i++) dataint_ += datapu_[i];

    std::cout << "Data PU distribution set" << std::endl;
}

void PUReweighter::setDataTruePUInput(const char * rootfile){
    TFile f(rootfile);
    if (f.IsZombie()) { 
        std::cerr << "Cannot find PU file: " << rootfile << std::endl; 
        exit(1);
    }
    TH1 *datapileup = dynamic_cast<TH1*>(f.Get("pileup"));
    if (!datapileup) {
        std::cerr << "Cannot find PU histogram: " << rootfile << std::endl; 
        exit(2);
    }
    setDataTruePUInput(datapileup);
    f.Close();
}
  
double PUReweighter::getPUweight(size_t trueBX){
    if(trueBX < datapu_.size() && trueBX < mcpu_.size()){
        return (datapu_[trueBX]/dataint_)/(mcpu_[trueBX]/mcint_);
    }
    return 1;
}

////////hardcoded stuff. Not elegant, but unlikely to change that often

void PUReweighter::setMCDistrSum12(TString scenario){
    // Distribution used for Summer2012 MC.
    std::vector<double> V;

    if (scenario == "S07") {
        Double_t Summer2012_S07[60] = {2.344E-05, 2.344E-05, 2.344E-05, 2.344E-05, 4.687E-04, 4.687E-04, 7.032E-04, 9.414E-04, 1.234E-03, 1.603E-03, 2.464E-03, 3.250E-03, 5.021E-03, 6.644E-03, 8.502E-03, 1.121E-02, 1.518E-02, 2.033E-02, 2.608E-02, 3.171E-02, 3.667E-02, 4.060E-02, 4.338E-02, 4.520E-02, 4.641E-02, 4.735E-02, 4.816E-02, 4.881E-02, 4.917E-02, 4.909E-02, 4.842E-02, 4.707E-02, 4.501E-02, 4.228E-02, 3.896E-02, 3.521E-02, 3.118E-02, 2.702E-02, 2.287E-02, 1.885E-02, 1.508E-02, 1.166E-02, 8.673E-03, 6.190E-03, 4.222E-03, 2.746E-03, 1.698E-03, 9.971E-04, 5.549E-04, 2.924E-04, 1.457E-04, 6.864E-05, 3.054E-05, 1.282E-05, 5.081E-06, 1.898E-06, 6.688E-07, 2.221E-07, 6.947E-08, 2.047E-08}; 
        V = std::vector<double>(Summer2012_S07,Summer2012_S07  + sizeof(Summer2012_S07)/sizeof(Summer2012_S07[0]));
    }

    if (scenario == "S10") {
        Double_t Summer2012_S10[60] = { 2.560E-06, 5.239E-06, 1.420E-05, 5.005E-05, 1.001E-04, 2.705E-04, 1.999E-03, 6.097E-03, 1.046E-02, 1.383E-02, 1.685E-02, 2.055E-02, 2.572E-02, 3.262E-02, 4.121E-02, 4.977E-02, 5.539E-02, 5.725E-02, 5.607E-02, 5.312E-02, 5.008E-02, 4.763E-02, 4.558E-02, 4.363E-02, 4.159E-02, 3.933E-02, 3.681E-02, 3.406E-02, 3.116E-02, 2.818E-02, 2.519E-02, 2.226E-02, 1.946E-02, 1.682E-02, 1.437E-02, 1.215E-02, 1.016E-02, 8.400E-03, 6.873E-03, 5.564E-03, 4.457E-03, 3.533E-03, 2.772E-03, 2.154E-03, 1.656E-03, 1.261E-03, 9.513E-04, 7.107E-04, 5.259E-04, 3.856E-04, 2.801E-04, 2.017E-04, 1.439E-04, 1.017E-04, 7.126E-05, 4.948E-05, 3.405E-05, 2.322E-05, 1.570E-05, 5.005E-06};
        V = std::vector<double>(Summer2012_S10,Summer2012_S10  + sizeof(Summer2012_S10)/sizeof(Summer2012_S10[0]));
    }
    ////////for other PU scenarios!!!

    std::cout << "setting MC pileup distribution to summer12_"+scenario << std::endl;
    //    V.insert(V.begin(),1); //fill zero bin
    mcpu_=V;
    mcint_=0;
    for(unsigned int i=1;i<mcpu_.size();i++) mcint_ += mcpu_[i];

}

void PUReweighter::setMCDistrFall11(TString scenario){

  std::vector<double> V;

  if(scenario=="S06"){
    Double_t Fall2011[50] = {  0.003388501,  0.010357558,  0.024724258,  0.042348605,  0.058279812,  0.068851751,  0.072914824,  0.071579609,  0.066811668,  0.060672356,  0.054528356,  0.04919354,  0.044886042,  0.041341896,  0.0384679,  0.035871463,  0.03341952,  0.030915649,  0.028395374,  0.025798107,  0.023237445,  0.020602754,  0.0180688,  0.015559693,  0.013211063,  0.010964293,  0.008920993,  0.007080504,  0.005499239,  0.004187022,  0.003096474,  0.002237361,  0.001566428,  0.001074149,  0.000721755,  0.000470838,  0.00030268,  0.000184665,  0.000112883,  6.74043E-05,  3.82178E-05,  2.22847E-05,  1.20933E-05,  6.96173E-06,  3.4689E-06,  1.96172E-06,  8.49283E-07,  5.02393E-07,  2.15311E-07,  9.56938E-08};
    V = std::vector<double>(Fall2011,Fall2011  + sizeof(Fall2011)/sizeof(Fall2011[0]));
  }
  else{
    std::cout << "PUReweighter::setMCDistrSum12: scenario " << scenario << "not known!" << std::endl;
  }
  
  std::cout << "setting MC pileup distribution to Fall11_"+scenario << std::endl;
  mcpu_=V;
  mcint_=0;
  for(unsigned int i=1;i<mcpu_.size();i++) mcint_ += mcpu_[i];
}
void PUReweighter::clear()
{
    std::cout << "\n\nWARNING!!! RESETTING ALL PU WEIGHTS! ONLY FOR TESTING!!\n\n" << std::endl;
    datapu_.clear();
    mcpu_.clear();
}

