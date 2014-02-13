#ifndef MATHDEFS_H
#define MATHDEFS_H

//#include "Math/GenVector/LorentzVector.h"
//#include "DataFormats/Math/interface/LorentzVector.h"

//#define M_PI    3.14159265358979323846f


#include <Math/LorentzVector.h>
#include <Math/PtEtaPhiM4D.h>

//#include "LorentzVector.h"

namespace ztop{
  //typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> > LorentzVector;
  //typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<float> > PolarLorentzVector;


  typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > D_LorentzVector;
  typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > D_PolarLorentzVector;

}


#endif
