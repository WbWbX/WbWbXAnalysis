#ifndef MATHDEFS_H
#define MATHDEFS_H

//#include "Math/GenVector/LorentzVector.h"
#include "DataFormats/Math/interface/LorentzVector.h"

namespace ztop{
  typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LorentzVector;
  typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > PolarLorentzVector;




}


#endif
