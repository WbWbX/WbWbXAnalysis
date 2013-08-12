#include <vector>
#include "../interface/container.h"
#include "../interface/containerUF2.h"
#include "../interface/container2D.h"
#include "../interface/containerStack.h"
#include "../interface/containerStackVector.h"
//#include "../interface/TUnfoldIncludes.h"


namespace
{
  struct dictionary {

    std::vector<double> klklkl;
    std::vector<TString> kkj;
    std::pair<TString,std::vector<double> > jj;
    std::vector<std::pair<TString,std::vector<double> > > j;
    std::vector<long int> kl;

    ztop::container1DUF2 kkksad;
    ztop::container1D a;
    std::vector<ztop::container1D> b;
    ztop::container2D aa;
    std::vector<ztop::container2D> bb;
    ztop::container1DStack asda;
    std::vector<ztop::container1DStack> bbd;
    ztop::container1DStackVector aaa;
    std::vector<ztop::container1DStackVector> bbb;
    std::map<TString,size_t > kokssd;


    std::vector<TH2D> ijij;
    std::map<TString,std::vector<TH2D> > kokd;
/*
    TUnfold tunf;
    TUnfoldDensity tunfdes;
    TUnfoldSys tunfsys;
    TUnfoldBinning tunfb;
    std::vector<TUnfold> tunfv;
    std::vector<TUnfoldDensity> tunfdesv;
    std::vector<TUnfoldSys> tunfsysv;
    std::vector<TUnfoldBinning> tunfbv;
*/
  };
}

