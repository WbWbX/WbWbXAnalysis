#ifndef ELECRHOISOADDER_H
#define ELECRHOISOADDER_H

#include "NTElectron.h"
#include <vector>
#include <iostream>
#include <algorithm>

namespace ztop{

  class elecRhoIsoAdder{

  public:

    elecRhoIsoAdder(){incorr_=false;cone_=0.3;};
    elecRhoIsoAdder(bool IsMC, bool use2012=true){ismc_=IsMC;use2012EA_=use2012;cone_=0.3;incorr_=false;};
    ~elecRhoIsoAdder(){};

    void setUse2012EA(bool use){use2012EA_=use;}
    void setCone(float cone){cone_=cone;}

    void setRho(float rho){rho_=rho;};

    void addRhoIso(std::vector<ztop::NTElectron> &);
    void addRhoIso(ztop::NTElectron &);


  private:

    void checkCorrectInput();

    float getRhoIso(ztop::NTElectron &);
    float Aeff(float);

    float rho_;
    float cone_;
    bool ismc_;
    bool use2012EA_;
    bool incorr_;

  };
}

#endif
