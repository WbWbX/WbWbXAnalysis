#ifndef reweightPU_h
#define reweightPU_h

#include "TH1D.h"
#include <vector>
#include "TString.h"


//! PU reweighting. Now provides a PU variation of +-1.
//! contradicts to the official recommendation to use different data distributions with varied total inelastic cross section by +-5%
//! so reweighting is changed by changing the input distributions provided at (naf afs) kiesej/public

class PUReweighter {

public:
    void setDataTruePUInput(TH1* dataPUdist);
    void setDataTruePUInput(const char * rootfile);
    double getPUweight(size_t trueBX);
    void setMCDistrSum12(TString scenario="S10");
    void setMCDistrFall11(TString scenario="S06");
    void clear();
    
private:
    std::vector<double> datapu_;
    std::vector<double> mcpu_;
    double dataint_;
    double mcint_;
};

#endif
