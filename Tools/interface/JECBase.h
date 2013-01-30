#ifndef JECBASE_H
#define JECBASE_H

#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include "Math/GenVector/LorentzVector.h"



namespace top{
  typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LorentzVector;


  class JECBase{
  public:

    JECBase(){}
    JECBase(const top::JECBase &);
    JECBase & operator = (const top::JECBase &);
    ~JECBase();

    void setFile(std::string pathToFile, bool quiet=false);
    void setSystematics(std::string); //! up, down, no

    std::vector<unsigned int> &  sources(){return sources_;}

    void applyJECUncertainties(top::LorentzVector &);

  protected:

    std::string pathToFile_;
    std::vector<JetCorrectionUncertainty*> vsrc_;
    JetCorrectionUncertainty* totalunc_;
    int noupdown_;
    std::vector<unsigned int> sources_;

    void copyFrom(const top::JECBase &);

  };

}
#endif
