#ifndef JERBASE_H
#define JERBASE_H

#include "Math/GenVector/LorentzVector.h"
#include <vector>
#include "TtZAnalysis/Tools/interface/miscUtils.h"
#include <algorithm>
#include <iostream>

// following recommendation https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetResolution;   https://twiki.cern.ch/twiki/bin/viewauth/CMS/TWikiTopRefSyst

namespace top{

  typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LorentzVector;

  class JERBase{
  public:
    JERBase(){setSystematics("def");}
    ~JERBase(){}

    void setResolutionEtaRanges(std::vector<double> ranges){resranges_=ranges;}
    void setResolutionFactors(std::vector<double> factors){resfactors_=factors;}

    void setSystematics(std::string type);

    void correctP4(top::LorentzVector & ,const top::LorentzVector & );

  protected:

    std::vector<double> resranges_;
    std::vector<double> resfactors_;

  };
}
#endif
